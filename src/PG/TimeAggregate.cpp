#include "./TimeAggregate.h"
#include "../stdafx.h"
#include "../MainFrm.h"
bool TimeAggregate::CreateNewAggregatedFile(CPG *pPG, int Option)
{
	CPG *pPG_Dest;
	pPG_Dest= new CPG;

	bool fail=true;
	if(pPG==nullptr&&m_pPGSource==nullptr) return nullptr;
	else if(pPG!=nullptr)
		m_pPGSource=pPG;

	m_pPGSource->CloseFile();
	m_pPGSource->SetCompleteRead(false);
	m_pPGSource->SetCompleteRepRead(true);
	m_pPGSource->Open(m_pPGSource->GetFileName());
	m_pPGSource->ReOpen();

	auto nrep=m_pPGSource->GetNumRepititions();
	auto nvar=m_pPGSource->GetNumVariables();
	auto numrec=m_pPGSource->GetNumRecords();
	
	if(nrep==1) {
		m_pPGSource->SetActBufferSize(m_pPGSource->GetNumRecords());
	}



	m_pPGSource->ReadNewBuffer(1, 1);
	m_pPGSource->SetMeanReset(Option);
	int NewNumRec=0;
	float *pFloat;
	switch(Option) {
		case 0:
			NewNumRec=m_pPGSource->GetNumRecordsYear(1);		
			break;
		case 1:
			NewNumRec=m_pPGSource->GetNumRecordsMonth(1);
			break;
		case 2:
			NewNumRec=m_pPGSource->GetNumRecordsDays(1);
			break;
		case 3:
			NewNumRec=m_pPGSource->GetNumRecordsHour(1);
			break;
		case 4:
			NewNumRec=m_pPGSource->GetNumRecordsMinute(10);
			break;
	}

	if(NewNumRec<=0) {
		delete pPG_Dest;
		return false;
	}

	pPG_Dest->SetCompleteRead(false);
	pPG_Dest->SetCompleteRepRead(true);
	pPG_Dest->CreateNewFile(nvar, NewNumRec,nrep);
	pPG_Dest->SetInfoMode();
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
	}
	if(!pPG_Dest->SaveAs(m_DestFileName, true)) {
		pPG_Dest->ReOpen(true);
	}

	CWinApp* pApp = AfxGetApp();
	CCoupModelApp *pCoupModel=(CCoupModelApp*)pApp;
	CMainFrame *p_mainframe;
 
	p_mainframe=(CMainFrame*)pCoupModel->m_pMainWnd;
	if(nrep==1) {
		m_pPGSource->SetActBufferSize(m_pPGSource->GetNumRecords());
		pPG_Dest->SetActBufferSize(NewNumRec);
	}


	for(size_t rep=0; rep<nrep; rep++) {
		m_pPGSource->ReadNewBuffer(1, rep+1);
		m_pPGSource->SetMeanReset(Option);
		/*if(!pPG_Dest->SetRepititionSequence(rep+1, true)) {
				delete pPG_Dest;
				return false;	


		}*/
		float rel=float(rep)/float(nrep);
		p_mainframe->UpdateProgress(0, rel);
		if(!m_pPGSource->SetRepititionSequence(rep+1, true)) return false;

		switch(Option) {
		case 0:
			NewNumRec=m_pPGSource->GetNumRecordsYear(1, rep+1);		
			break;
		case 1:
			NewNumRec=m_pPGSource->GetNumRecordsMonth(1, rep+1);
			break;
		case 2:
			NewNumRec=m_pPGSource->GetNumRecordsDays(1, rep+1);
			break;
		case 3:
			NewNumRec=m_pPGSource->GetNumRecordsHour(1, rep+1);
			break;
		case 4:
			NewNumRec=m_pPGSource->GetNumRecordsMinute(10, rep+1);
			break;
		}
		pFloat=m_pPGSource->GetMeanPointer(Option);
		pPG_Dest->SetActBufferSize(NewNumRec);
		if(!pPG_Dest->SetPointerToAllData(pFloat)) {
			delete pPG_Dest;
			return false;
		}
		if(!pPG_Dest->GetCompleteRead()) {

			if(!pPG_Dest->WriteNewBuffer(rep+1)) {
				delete pPG_Dest;
				return false;				
				
			};
		}
		if(nrep==1) {
			pPG_Dest->SetCompleteRead(true);

		}
		if(!pPG_Dest->ReCalculateMinMax(rep+1)) {
			delete pPG_Dest;
			return false;
			
		}
	}


//	pPG_Dest->CheckMissingNumbers();
	string txt;
	if(pPG_Dest->WritePGFileEnd(NewNumRec,nrep)) {
		txt="Calculation made and file created with : ";
		txt+=FUtil::STD_ItoAscii(pPG_Dest->GetNumRecords());
		txt+=" records and ";
		txt+=FUtil::STD_ItoAscii(pPG_Dest->GetNumVariables());
		txt+=" variables ";
		pPG_Dest->GetNumVariables();
		m_ResultSum.push_back(txt);
		m_ResultSum.push_back(pPG_Dest->GetFileName());
		txt="Click on file name to open file";
		m_ResultSum.push_back(txt);
		
	}
	else {
		txt="Calculation failed no file created";
		m_ResultSum.push_back(txt);
	}
	pPG_Dest->CloseFile();




	delete pPG_Dest;

	return true;
}
