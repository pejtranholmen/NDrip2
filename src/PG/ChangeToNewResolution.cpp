#include "./ChangeToNewResolution.h"
#include "../stdafx.h"
#include "../MainFrm.h"

bool SortByValue(const MONTHPAIR &lhs, const MONTHPAIR &rhs) { return lhs.value < rhs.value;};
size_t FindInSortedVector(vector<MONTHPAIR> &v, float value) {
	size_t index=0;
	int step=v.size()/2;
	if(value<v[index].value) return 0;
	bool larger=true;
	while (abs(step)>0) {
		if (value>v[index].value) {
			if(larger)
				index+=step;
			else {
				larger=true;
				step=-step/2;
				index+=step;
			}				
		}
		else if(value<v[index].value) {
			if(!larger)
				index+=step;
			else {
				larger=false;
				step=-step/2;
				index+=step;
			}
		}
		else 
			return index;


	
		if(index+1>v.size()) {
			index=v.size()-1;
			if(larger&&value>v[index].value) return index;
		}
	}
	if(value>v[index].value)
		return index;
	else
		return index-1
		;
};

MONTHVECTORS ChangeToNewResolution::EstimateMeanMonthValues(int irep) {
	MONTHVECTORS out;
	
	auto NumMonth=m_pPGSource->GetNumRecordsMonth(1);
	float *pFloat;

	auto nvar=m_pPGSource->GetNumVariables();

	m_pPGSource->SetMeanReset(1);
	if(!m_pPGSource->SetRepititionSequence(irep, true)) return out;

	NumMonth=m_pPGSource->GetNumRecordsMonth(1,irep);
	pFloat=m_pPGSource->GetMeanPointer(1);

	vector<MONTHPAIR> Temp[12], Prec[12];

	MONTHPAIR pair;
	size_t indexTcount=0;
	size_t indexPcount=0;
	for(size_t i=0;i<NumMonth;i++) {
		size_t imon=i%12;
		size_t iyear=i/12;
		for(size_t ivar=0;ivar<nvar;ivar++) {
			if(ivar==2) {
				pair.value=*(pFloat+i*(nvar+1)+m_MatchingVar.main_source_var);
				pair.minute=*(long*)(pFloat+i*(nvar+1));
				pair.orgindex=iyear;
				Temp[imon].push_back(pair);
			}
			else if(ivar==3) {
				pair.value=*(pFloat+i*(nvar+1)+m_MatchingVar.second_source_var);
				pair.minute=*(long*)(pFloat+i*(nvar+1));
				pair.orgindex=iyear;
				Prec[imon].push_back(pair);
			}
		}
	}
	for(size_t i=0; i<12; i++) {
		sort(Temp[i].begin(),Temp[i].end(),SortByValue); 
		out.Temp[i]=Temp[i];
		out.Prec[i]=Prec[i];
	}
	return out;
}
bool ChangeToNewResolution::CreateNewFile(CPG *pPG, CPG *pPGScaling, int Option, int ExtraVarOption)
{
	CWinApp* pApp = AfxGetApp();
	CCoupModelApp *pCoupModel=(CCoupModelApp*)pApp;
	CMainFrame *p_mainframe;
 
	p_mainframe=(CMainFrame*)pCoupModel->m_pMainWnd;
	CPG *pPG_Dest;
	pPG_Dest= new CPG;
	m_pPGDest=pPG_Dest;

	bool fail=true;
	if(pPG==nullptr&&m_pPGSource==nullptr) return nullptr;
	else if(pPG!=nullptr)
		m_pPGSource=pPG;

	m_pPGSource->CloseFile();
	m_pPGSource->SetCompleteRead(false);
	m_pPGSource->Open(m_pPGSource->GetFileName());
	m_pPGSource->ReOpen();

	auto nrep=m_pPGSource->GetNumRepititions();
	auto nvar=m_pPGSource->GetNumVariables();
	auto numrec=m_pPGSource->GetNumRecords();
	m_pPGSource->ReadNewBuffer(1, 1);
	auto kool1=m_pPGSource->GetStartDate();
	int NewNumRec=0;

	pPGScaling->ReOpen();
	auto NewPeriod=pPGScaling->GetLongTimeEnd()-pPGScaling->GetLongTimeStart();
	auto dist=pPGScaling->GetNormalTimeInterval();
	auto OldPeriod=m_pPGSource->GetLongTimeEnd()-m_pPGSource->GetLongTimeStart();
	auto RepScaling=pPGScaling->GetNumRepititions();
	NewNumRec=NewPeriod/1440+31;

	if(NewNumRec==0) {
		delete pPG_Dest;
		return false;
	}

	pPG_Dest->SetCompleteRead(false);
	pPG_Dest->SetCompleteRepRead(false);
	pPG_Dest->SetInfoMode();
	nrep=min(nrep, RepScaling);
	size_t extra_utvar=4;
	if(ExtraVarOption==0) extra_utvar=0;

	pPG_Dest->CreateNewFile(nvar+extra_utvar, NewNumRec,nrep);
	if(m_DestFileName.size()==0) m_DestFileName="test.bin";
	m_recout=0;
	for(size_t rep=0; rep<nrep; rep++) {
		string txt;
			for(size_t i=1; i<=nvar; i++) {
							txt=m_pPGSource->GetVarName(i,rep+1);
							pPG_Dest->SetVarName(i, txt, rep+1);
							txt=m_pPGSource->GetVarUnit(i, rep+1);
							pPG_Dest->SetVarUnit(i, txt, rep+1);
							txt=m_pPGSource->GetVarId(i, rep+1);
							pPG_Dest->SetVarId(i, txt, rep+1);
							txt=m_pPGSource->GetVarPos(i, rep+1);
							pPG_Dest->SetVarPos(i, txt, rep+1);
							double Lat=m_pPGSource->GetVarLat(i, rep+1);
							pPG_Dest->SetVarLat(i, Lat, rep+1);
							double Long=m_pPGSource->GetVarLong(i, rep+1);
							pPG_Dest->SetVarLong(i, Long, rep+1);
							double Alt=m_pPGSource->GetVarAlt(i, rep+1);
							pPG_Dest->SetVarAlt(i, Alt, rep+1);
							string txt=m_pPGSource->GetVarStation(i, rep+1);
							pPG_Dest->SetVarStation(i, txt, rep+1);
							txt=m_pPGSource->GetVarCountry(i, rep+1);
							pPG_Dest->SetVarCountry(i, txt, rep+1);
			}
			for(size_t i=1; i<=extra_utvar; i++) {
							if(i==1) txt="Year from OrigFile";
							else if(i==2) txt="Diff in Temp";
							else if(i==3) txt="New Month Temp";
							else if(i==4) txt="Prec Correction";
							pPG_Dest->SetVarName(i+nvar, txt, rep+1);
			}
	}
	//pPG_Dest->Open(m_DestFileName);

	if(!pPG_Dest->SaveAs(m_DestFileName, true)) {
		pPG_Dest->ReOpen(true);
	}
	if(!pPG_Dest->IsGood()) {
		pPG_Dest->ClearFile();
		pPG_Dest->ReOpen(true);
		if(!pPG_Dest->IsGood()) {
			pPG_Dest->Open(m_DestFileName);
			if(!pPG_Dest->IsGood()) {

			}
		}
	}
	//pPG_Dest->ReOpen(true);
	auto yearstart=PGUtil::YearFunction(pPGScaling->GetLongTimeStart());
	auto yearend=PGUtil::YearFunction(pPGScaling->GetLongTimeEnd());
	if(!pPGScaling->ReOpen()) return false;
	pPGScaling->SetRepititionMode(true);

	for(size_t irep=1; irep<=nrep; irep++) {
		if(!pPGScaling->SetRepititionSequence(irep, true)) return false;


		//1 Make Montly mean value of Original File with respect to Temp and Montly Sum of Prec.
		auto kool=m_pPGSource->GetStartDate();
		MONTHVECTORS sortedmonth=EstimateMeanMonthValues(irep);
		vector<float> TargetTemp, TargetPrec;
		TargetTemp.resize(12); TargetPrec.resize(12);
		kool=m_pPGSource->GetStartDate();
		for(size_t yearindex=yearstart; yearindex<=yearend; yearindex++) {

			float rel=float(yearindex-yearstart)/float(yearend-yearstart)/float(nrep)+float(irep-1)/float(nrep);
			p_mainframe->UpdateProgress(0, rel);

			size_t index;
			long recno=(yearindex-yearstart)*12+1;
			double PrecScaling;
			for(size_t i=0; i<12; i++) {
				auto temp_value=pPGScaling->GetVarValue(m_MatchingVar.main_scaling_var, recno+i);
				long longdest, longdestend;

				longdest=pPGScaling->GetLongTime(recno+i);
				if(yearend==yearindex&&i==11) {
					longdestend=pPGScaling->GetLongTimeEnd()+1440*31;
				}
				else {
					longdestend=pPGScaling->GetLongTime(recno+i+1);
				
				}

				if(Option==1) {
					size_t minstart_newperiod, minend_newperiod;
					minstart_newperiod=PGUtil::cminutx(yearindex,i+1,1,12,0);
					if(i==11)
						minend_newperiod=PGUtil::cminutx(yearindex,i+2,1,12,0);
					else
						minend_newperiod=PGUtil::cminutx(yearindex+1,i+1,1,12,0);
					if(minstart_newperiod>=m_pPGSource->GetLongTimeStart()&&
						minstart_newperiod<m_pPGSource->GetLongTimeEnd()) {
						longdest=minstart_newperiod;
						longdestend=minend_newperiod;
					}
					//Make start and End to the 


				}
				if(temp_value>-1.E37)
					index=FindInSortedVector(sortedmonth.Temp[i], temp_value);
				else
					index=0;

				auto prec_value_target=pPGScaling->GetVarValue(m_MatchingVar.main_scaling_var, recno+i);
				auto prec_In_TempYear=sortedmonth.Prec[i][sortedmonth.Temp[i][index].orgindex].value;
				if(prec_value_target>-1.E37) {
					PrecScaling=prec_value_target/(prec_In_TempYear*PGUtil::LengthOfMonth(longdest));
				}
				else
					PrecScaling=1;
				if(Option==0)
					WriteOneMonthToSource(irep, sortedmonth.Temp[i][index].minute,longdest,  longdestend,temp_value,sortedmonth.Temp[i][index].value, PrecScaling, extra_utvar );			
				else if(Option==1)
					WriteOneMonthToSource(irep, longdest,longdest,  longdestend,temp_value,sortedmonth.Temp[i][index].value, PrecScaling, extra_utvar );
				// Write the new month with all variables in the target PG-file
			}
			index=1;
		}
	};
	pPG_Dest->SaveAs(m_DestFileName, true);








	/*for(size_t rep=0; rep<nrep; rep++) {
		float rel=float(rep)/float(nrep);
		p_mainframe->UpdateProgress(0, rel);
		//m_pPGSource->ReadNewBuffer(1, rep+1);
		//m_pPGSource->SetRepititionSequence(rep+1, true);
				vector<float> min_v, max_v;
				min_v.assign(nvar, 1.1E38); max_v.assign(nvar,-1.E38);
				for(size_t rec=0; rec<NewNumRec; rec++) {
					void *pFloat;void *pDate;
					if(Option==0) {
						pFloat=m_pPGSource->GetRecordPointer(NewNumRec+rec+1, rep+1);
						pDate=m_pPGSource->GetRecordPointer(rec+1, rep+1);		
					}
					else {
						pFloat=m_pPGSource->GetRecordPointer(m_StartOut+rec, rep+1);
						pDate=m_pPGSource->GetRecordPointer(m_StartOut+rec, rep+1);	
					}

					pPG_Dest->SetDatePointer((float*)pDate);
					pPG_Dest->SetVarPointer((float*)pFloat+1,0, nvar);
					for(size_t i=0; i<nvar;i++) {
						float value=pPG_Dest->GetVarValue(i+1);
						if(min_v[i]>value) min_v[i]=value;
						if(max_v[i]<value) max_v[i]=value;
					}
					//if(pPG_Dest->SetWritePosition(rep*NewNumRec+rec+1))
					if(!pPG_Dest->WritePGFileRecord(rec+1, rep+1)) return false;
				;
					//pPG_Dest->WriteNewPGFileRecord();
				}
				for(size_t i=0; i<nvar;i++) {
					pPG_Dest->SetVarMin(i+1, min_v[i], rep+1);
					pPG_Dest->SetVarMax(i+1, max_v[i], rep+1);	
				}

	}
	*/
	string txt;
	if(pPG_Dest->WritePGFileEnd(NewNumRec, nrep)) {
		if(Option==0) txt="New file created with new resolution of data for the entire period:";
		else if(Option==1) txt="New subperiod in Output file :";
		m_ResultSum.push_back(txt);
		m_ResultSum.push_back(pPG_Dest->GetFileName());
		m_ResultSum.push_back("Click on File Name to open File");
		
	}
	else {
		txt="Calculation failed no file created";
		m_ResultSum.push_back(txt);
	}
	pPG_Dest->CloseFile();
	delete pPG_Dest;

	return true;
}



bool ChangeToNewResolution::WriteOneMonthToSource(int irep, long minsource, long longmindeststart,long  longmindestend, float sourcetemp, float newtemp, double PrecScaling, int extra_utvar) {
	size_t count=0;
	m_pPGSource->ReadNewBuffer(1, 1);
	auto kool0=m_pPGSource->GetStartDate();
	long rec=m_pPGSource->GetRecord(minsource, false);

	auto smonth=PGUtil::MonthFunction(minsource);
	auto syear=PGUtil::YearFunction(minsource);
	auto sday=PGUtil::DayFunction(minsource);
	minsource=PGUtil::cminutx(syear,smonth,1,12,0);
	rec=m_pPGSource->GetRecord(minsource, false);
	auto minkoll=m_pPGSource->GetLongTime(rec);

	auto nmonth=PGUtil::MonthFunction(longmindeststart);
	auto nyear=PGUtil::YearFunction(longmindeststart);
	if(minkoll!=minsource) {
		rec=m_pPGSource->GetRecord(minsource, false);
		auto date=m_pPGSource->GetStartDate();
		date=m_pPGSource->GetDate(rec);


	}

	rec=max(1, rec);
	float *pFloat;
	vector<float> vv;
	vv.resize(9+extra_utvar);
	for(unsigned int imin=longmindeststart; imin<longmindestend;imin+=1440) {
		count++;
		m_pPGSource->ReadNewBuffer(1, 1);
		auto date=m_pPGSource->GetStartDate();
		pFloat=(float*)m_pPGSource->GetRecordPointer(rec+count, irep);
		vv[0]=*(float*)&imin;
		for(size_t i=1; i<9;i++) {
			vv[i]=*(pFloat+i);
		}
		vv[1]+=sourcetemp-newtemp;
		vv[2]+=sourcetemp-newtemp;
		vv[3]+=sourcetemp-newtemp;
		vv[4]*=float(PrecScaling);

		if(extra_utvar>0) {
			vv[8+1]=float(syear);
			vv[8+2]=sourcetemp-newtemp;
			vv[8+3]=newtemp;
			vv[8+4]=float(PrecScaling);
		}

		m_pPGDest->SetRecordPointer(vv.data());

		m_recout++;
		if(!m_pPGDest->WritePGFileRecord(m_recout, irep)) {
			if(!m_pPGDest->WritePGFileRecord(m_recout, irep)) 	return false;
		}
	
	}

	return true;
}
