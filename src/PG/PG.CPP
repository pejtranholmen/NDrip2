//#include "../stdafx.h"
#include "../ModelTypes/SimB.h"
#include "../NewBase/MultiRun/Residuals.h"
#include "./PG.H"
#include "../Util/FUtil.hpp"
#include "../NewBase/Doc.h"
#ifndef COUPSTD
#include "../CoupModelDoc.h"
#include "../CUG/SimDoc\MyCug.h"
#include "../CoupModelView.h"
#include "../MainFrm.h"
#include "../NewBase/FileScanner.h"
#endif

#ifdef MS_CODE
#include "../BasicExcel/mexcel.h"
using namespace miniexcel;

#include "../BasicExcel/BasicExcel.hpp"
using namespace YExcel;

#define SAVEPATH "c:\\temp\\a.xls"
#endif

#define BASE   1721424L                /* Jan 1, AD 1 */


CPG::CPG() {
	m_DateSelection=false;
	m_NumWithinYear=0;
	m_withinYearAllocated=0;
	m_WithinYearForResiduals=false;
	m_UpdatedMultiAccepted=m_UpdatedMultiAll=false;
	m_ApplyConditionCheck = false;
	m_AllValuesAssigned = false;
	m_OnlyMemoryUse = false;
}
CPG::CPG(string filename)
{
	pg.status.FileName=filename;
	m_IsOpen=false;
	pg.Shape.NumVar=0;

	m_str=nullptr;
	m_startRec=1;
	m_varx_minute=nullptr;
	m_varx_hour=nullptr;
	m_varx_days=nullptr;

	m_varx_years=nullptr;
	pg.Shape.RecordSize=0;
	m_DataInFocus=0;
	m_DateSelection=false;

	m_OutFileStatus=false;
	pg.status.CompleteFileRead=false;
	pg.Shape.NumRecords=0;
	m_Closed=false;
	GapProblemFinded=true;
	m_WithinYearForResiduals=false;
	m_YearIndexStart=nullptr;
	m_YearRecords=nullptr;


	pg.status.FileVersion=1;
	m_AllValuesAssigned = false;
	m_IncreaseTimeTest= m_IncreaseTime=false;
	m_OnlyMemoryUse = false;

}
CPG::~CPG()
{
	Reset();
}


vector<float> CPG::Get_AllRep_VarValue(size_t varno, size_t var_rec)
{
	vector <float> v_ut;
	float value;	
	if(pg.Shape.RepetitionMode) {
		if(pg.status.CompleteFileRead) {
			for(size_t j=1;j<=pg.Shape.NumRepetitions;j++) {
				value=pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+varno+(j-1)*pg.Shape.RecordSize_Repetion];
				v_ut.push_back(value);
			}
		}
		else {
			for(size_t j=1;j<=pg.Shape.NumRepetitions;j++) {
				size_t newpos=pg.Shape.RecordSize*var_rec+(j-1)*pg.Shape.RecordSize_Repetion*4;
				m_MainPGStreamReadWrite.seekg(newpos,ios::beg);
				m_MainPGStreamReadWrite.read((char*)&pg.Var.v[0],pg.Shape.RecordSize);
				value=pg.Var.v[varno];
				v_ut.push_back(value);
			}
		}
	}
	return v_ut;
}
float CPG::GetVarValue_MinPos(size_t varno, size_t minutv)
{
	//return *m_var[varno-1][var_rec-1];
	size_t var_rec;
	var_rec=GetRecord(minutv);
	if(var_rec<1)
		return MISSING;
	else 
		return GetVarValue(varno,var_rec);
}
size_t CPG::GetRecord(size_t minutv, bool exact)
{
	size_t record;
	record=1;
	while(GetLongTime(record)<minutv) {
		record++;
		if(record>pg.Shape.NumRecords) {
			m_startRec=1;
			return string::npos;
		}
	}
	if(GetLongTime(record)==minutv){
		m_startRec=record;
		return record;
	}
	else if(exact)
		return string::npos;
	else
		return record;
}
void CPG::Reset_Start()
{
	m_startRec=1;
}

double CPG::GetFloatTime(size_t var_rec)
{	
	return GetDoubleTime(var_rec);
}
double CPG::GetDoubleTime(size_t var_rec)
{
	size_t minut=GetLongTime(var_rec);
	return 60.*double(minut-m_time1900_Zero);
/*	
	if(pg.status.CompleteFileRead) 
		return (*(size_t*)&pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize]-m_time1900_Zero)*double(60);
	else {

		size_t size_t newpos=pg.Shape.RecordSize*var_rec;
		m_driveFile.seekg(newpos,ios::beg);
		m_driveFile.read((char*)&pg.Var.v[0],pg.Shape.RecordSize);
		return (*(size_t*)&pg.Var.v[0]-m_time1900_Zero)*double(60);

	}*/
}
double CPG::GetDoubleTimeStart()
{
	return (GetLongTimeStart()-m_time1900_Zero)*double(60);
}
double CPG::GetDoubleTimeEnd()
{
	return (GetLongTimeEnd()-m_time1900_Zero)*double(60);
}


string CPG::GetStartDate()
{
	size_t minut=GetLongTimeStart();
	return PGUtil::StringDatum(minut);
	/*if(pg.status.CompleteFileRead)
		return PGUtil::StringDatum(*(size_t*)&pg.Var.v[0]);
	else {
		size_t size_t newpos=pg.Shape.RecordSize;
		m_driveFile.seekg(newpos,ios::beg);
		m_driveFile.read((char*)&pg.Var.v[0],pg.Shape.RecordSize);

		return PGUtil::StringDatum(*(size_t*)&pg.Var.v[0]);

	}*/
}

string CPG::GetEndDate()
{

	size_t minut=GetLongTimeEnd();
	return PGUtil::StringDatum(minut);
//	return PGUtil::StringDatum(*m_time[m_time.GetUpperBound()]);
/*	if(pg.status.CompleteFileRead)
		return PGUtil::StringDatum(*(size_t*)&pg.Var.v[(pg.Shape.NumRecords-1)*pg.Shape.RecordIndexSize]);
	else {
		size_t size_t newpos=pg.Shape.RecordSize*pg.Shape.NumRecords;
		m_driveFile.seekg(newpos,ios::beg);
		m_driveFile.read((char*)&pg.Var.v[0],pg.Shape.RecordSize);
		return PGUtil::StringDatum(*(size_t*)&pg.Var.v[0]);

	}*/
}

bool CPG::FileExist()
{
	return FUtil::IsFileExisting(pg.status.FileName);

}

size_t CPG::GetVarIndex(string str, size_t rep)
{
	size_t count=0;
	for(size_t i=0;i<pg.Shape.NumVar;i++) {
		string test;
		test=pg.Des.base.Name[i];
		if(pg.Des.base.Name[i].find(str)==0){
			count++;
			if(count==rep||rep==-1)
				return i;	
		}
	}
	return -1;
}
size_t CPG::GetVarIndex(string str,string strId, size_t rep)
{
	size_t count=0;
	for(size_t i=0;i<pg.Shape.NumVar;i++) {
		string test;
		test=pg.Des.base.Name[i];
		if(pg.Des.base.Name[i].find(str)==0&&pg.Des.base.Id[i].find(strId)>=0){
			count++;
			if(count==rep||rep==-1)
				return i;	
		}
	}
	return -1;
}

bool CPG::WriteSelectionToExcel()
{
/*	string tmp, status;
	size_t nvar, selvar, i, j;
	string filename;

	filename=pg.status.FileName;
	filename.TrimRight(".Bin");
	filename.TrimRight(".BIN");
	filename+="_Temp.xls";
	status="Wait while writing summary to excel file";
	((CCoupModelView*)m_pView)->writeStatus(status);
	nvar=GetNumOfSelected();
	TRY {
		FILE *f = fopen (filename, "wb");
		CMiniExcel miniexcel;

	miniexcel(3,0) = "Date";
  

	  for(size_t j=0; j<nvar; j++) { 
			selvar=GetSelection(j)+1;
			tmp=GetVarName(selvar);
			tmp.TrimRight(" ");
			miniexcel(0,j+1) = tmp;
	  }
 	  for(size_t j=0; j<nvar; j++) { 
			selvar=GetSelection(j)+1;
			tmp=GetVarId(selvar);
			tmp.TrimRight(" ");
			tmp.TrimLeft(" ");
			miniexcel(1,j+1) = tmp;
	  }
	  for(size_t j=0; j<nvar; j++) { 
			selvar=GetSelection(j)+1;
			tmp=GetVarUnit(selvar);
			tmp.TrimRight(" ");
			miniexcel(2,j+1) = tmp;
	  }
	  for(size_t j=0; j<nvar; j++) { 
			selvar=GetSelection(j)+1;
			tmp=GetVarPos(selvar);
			tmp.TrimRight(" ");
			miniexcel(3,j+1) = tmp;
	  }


	  for(size_t i=0; i<pg.Shape.NumRecords;i++) {
			miniexcel(i+4,0)=GetFloatTime(i+1);
			for (j=0; j<nvar;j++) {	
	 			selvar=GetSelection(j)+1;
				miniexcel(i+4,j+1) = GetVarValue(selvar,i+1);
			}
	  }
	    

	  miniexcel.write((char*)f);
	  miniexcel.~CMiniExcel();
	  return true;
	}
	CATCH_ALL(e) {
		return false;
	}
	END_CATCH_ALL;

	status="Summary successfully written to excel file";
	((CCoupModelView*)m_pView)->writeStatus(status);*/
	return true;
}

string CPG::WriteExcelFile()
{
  string tmp, status, filename;
  size_t ist, iend;


//  status="Wait while writing summary to excel file";
//  ((CCoupModelView*)m_pView)->writeStatus(status);

#ifndef COUPSTD

    CWinApp* pApp = AfxGetApp();
  string UserDirectory="";
  UserDirectory=FUtil::GetProfileStringStd("UserDirectory", UserDirectory);


	pApp->BeginWaitCursor();

		
	

  filename=pg.status.FileName;
  size_t pos=filename.rfind(".");
  if(pos!=string::npos)
	 filename=filename.substr(0,pos);
  filename+=".xls";
	ist=1;
	iend=GetNumVariables();
	size_t numsheets=(iend-1)/255+1;


	BasicExcel e;
	e.New(numsheets);


	for(size_t ish=0; ish<numsheets;ish++) {
		 string s3=	"Sheet";
		 s3+=FUtil::STD_ItoAscii(ish+1);
		BasicExcelWorksheet* sheet = e.GetWorksheet(s3.c_str());
		if(sheet==nullptr) return "";
		sheet->Cell(3,0)->SetString("Date");
		size_t ist_x=1+255*ish;
		size_t iend_x=iend;
		if(ish<numsheets-1)
			iend_x=255*ish+255;
	  
	
	  for(size_t j=ist_x; j<=iend_x; j++) { 
			tmp=GetVarName(j);

			FUtil::trim(tmp);
			sheet->Cell(0,j-ist_x+1)->SetString(tmp.data());
	  }
	  for(size_t j=ist_x; j<=iend_x; j++) { 
			tmp=GetVarId(j);
			FUtil::trim(tmp);

			sheet->Cell(1,j-ist_x+1)->SetString(tmp.data());
	  }
	  for(size_t j=ist_x; j<=iend_x; j++) { 
			tmp=GetVarUnit(j);

			FUtil::trim(tmp);
			sheet->Cell(2,j-ist_x+1)->SetString(tmp.data());

	  }
	  for(size_t j=ist_x; j<=iend_x; j++) { 
			tmp=GetVarPos(j);
			FUtil::trim(tmp);
			sheet->Cell(3,j-ist_x+1)->SetString(tmp.data());
			//miniexcel(3,j-ist+1) = tmp;
	  }
	  ReOpen();
	  for(size_t i=0; i<pg.Shape.NumRecords;i++) {
			sheet->Cell(i+4,0)->SetString(GetDate(i+1).data());
			//sheet->Cell(i+4,0)->SetInteger(i+1);
		//	miniexcel(i+4,0)=GetDate(i+1);
			for (size_t j=ist_x; j<=iend_x;j++) {	
				double f= GetVarValue(j,i+1);
				//pg.Var.v=i+double(j)/double(iend);
				sheet->Cell(i+4,j-ist_x+1)->SetDouble(f);
			}
	  }
	  CloseFile();
	}

	  e.SaveAs(filename.c_str());


  	pApp->EndWaitCursor();
//	status="Summary successfully written to excel file";
//	((CCoupModelView*)m_pView)->writeStatus(status);
#endif
return filename;

}
void CPG::SetXTGDIndex(size_t v1, size_t v2, size_t v3, size_t v4, size_t v5, size_t v6, size_t v7, size_t v8)
{
	m_Xind_Start=v1;
	m_Xind_End=v2;

	m_Tind_Start=v3;
	m_Tind_End=v4;

	m_Gind_Start=v5;
	m_Gind_End=v6;

	m_Dind_Start=v7;
	m_Dind_End=v8;
	if(v8<pg.Shape.NumVar) {
		m_Dind_End=pg.Shape.NumVar;
		if(m_Dind_Start==0) {
			if(m_Gind_End>0) m_Dind_Start=m_Gind_End+1;	
		}
   		if(m_Dind_Start==0) {
			if(m_Tind_End>0) m_Dind_Start=m_Tind_End+1;	
		}
		if(m_Dind_Start==0) {
			m_Dind_Start=m_Xind_End+1;	
		}
	}


}

size_t CPG::GetNumOfSelected()
{
	return m_Selected.size();
}
void CPG::AddSelection(size_t pgindex, float Value)
{
	m_Selected.push_back(pgindex);
	m_SelectedCoef.push_back(Value);
}
void CPG::SetSelectionWeight(size_t index, float Value) 
{
	if(index<m_SelectedCoef.size())
		m_SelectedCoef[index]=Value;
}
void CPG::SetCalcStart(size_t ir)
{

	m_CalcStart=ir;
}
void CPG::SetCalcStop(size_t ir)
{

	m_CalcStop=ir;
}
size_t CPG::GetCalcStart()
{
	return m_CalcStart;
}
size_t CPG::GetCalcStop()
{
	return m_CalcStop;
}
void CPG::SetCalcVar(size_t var)
{
	m_CalcVar=var;
}
size_t CPG::GetCalcVar()
{
	return m_CalcVar;
}
void CPG::SetCalcVarNew(size_t var)
{
	m_CalcVarNew=var;
}
void CPG::SetVarElementType(size_t VarNo, size_t Repit, elements elem)
{
	if (Repit > 0 && Repit != string::npos) {
		size_t add = 0;
		add = (Repit - 1)*pg.Shape.NumVar;
		VarNo += add;
	}
	if (pg.Des.addinfo.size() >= VarNo)
		pg.Des.addinfo[VarNo - 1].Element = elem;
}
size_t CPG::GetCalcVarNew()
{
	return m_CalcVarNew;
}
float CPG::GetSelectionWeight(size_t index)
{
	if(m_SelectedCoef.size()>index)
		return m_SelectedCoef[index];
	else
		return MISSING;

}
void CPG::SetSelection(size_t index, size_t pgindex) 
{
	if(m_Selected.size()<=index)
		m_Selected.resize(index+1);
	m_Selected[index]= pgindex;
}
size_t CPG::GetSelection(size_t index)
{
	if(index<GetNumOfSelected())
		return m_Selected[index];
	else
		return -1;
}
bool CPG::AddAllSelections()
{
	ResetSelections();
	for(size_t i=0;i<GetNumVariables();i++)
		AddSelection(i+1);
	return true;
}
void CPG::ResetSelections()
{
	m_Selected.clear();
	m_SelectedCoef.clear();
}
void CPG::SetDateSelection(bool selection)
{
	m_DateSelection=selection;

}
bool CPG::GetDateSelection()
{
	return m_DateSelection;
}
float CPG::GetVarValueMinuteMean(size_t varno, size_t var_rec, size_t minutes)
{
	//return *m_var[varno-1][var_rec-1];
	if(!m_varxMinutes) {
		GetNumRecordsMinute(minutes);
	}
	if(var_rec<=m_numrecords_Minutes)
		return m_varx_minute[(var_rec-1)*pg.Shape.RecordIndexSize+varno];
	else
		return MISSING;
}
float CPG::GetVarValueHourMean(size_t varno, size_t var_rec, size_t hour)
{
	//return *m_var[varno-1][var_rec-1];
	if(!m_varxHours) {
		GetNumRecordsHour(hour);
	}
	if(var_rec<=m_numrecords_Hours)
		return m_varx_hour[(var_rec-1)*pg.Shape.RecordIndexSize+varno];
	else
		return MISSING;
}
float CPG::GetVarValueDaysMean(size_t varno, size_t var_rec, size_t days)
{
	if(!m_varxDays) {
		GetNumRecordsHour(days);
	}
	if(var_rec<=m_numrecords_Days)
		return m_varx_days[(var_rec-1)*pg.Shape.RecordIndexSize+varno];
	else
		return MISSING;
	}

float CPG::GetVarValueMonthsMean(size_t varno, size_t var_rec, size_t months)
{
	//return *m_var[varno-1][var_rec-1];
	if(!m_varxMonths) {
		GetNumRecordsMonth(months);
	}
	if(var_rec<=m_numrecords_Months)
		return m_varx_months[(var_rec-1)*pg.Shape.RecordIndexSize+varno];
	else
		return MISSING;
	}
float CPG::GetVarValueYearsMean(size_t varno, size_t var_rec, size_t year)
{
	//return *m_var[varno-1][var_rec-1];
	if(!m_varxHours) {
		GetNumRecordsYear(year);
	}
	if(var_rec<=m_numrecords_Years)
		return m_varx_years[(var_rec-1)*pg.Shape.RecordIndexSize+varno];
	else
		return MISSING;
	}

string CPG::GetVarDateMinuteMean(size_t var_rec, size_t minutes)
{
	//return *m_var[varno-1][var_rec-1];
	if(!m_varxMinutes) {
		GetNumRecordsMinute(minutes);
	}
	if(var_rec<=m_numrecords_Minutes)
		return PGUtil::StringDatum(*(size_t*)&m_varx_minute[(var_rec-1)*pg.Shape.RecordIndexSize]);
	else
		return "";
}

string CPG::GetVarDateHourMean(size_t var_rec, size_t hour)
{
	//return *m_var[varno-1][var_rec-1];
	if(!m_varxHours) {
		GetNumRecordsHour(hour);
	}
	if(var_rec<=m_numrecords_Hours)
		return PGUtil::StringDatum(*(size_t*)&m_varx_hour[(var_rec-1)*pg.Shape.RecordIndexSize]);
	else
		return "";
}

string CPG::GetVarDateDaysMean(size_t var_rec, size_t days)
{
	if(!m_varxDays) {
		GetNumRecordsHour(days);
	}
	if(var_rec<=m_numrecords_Days)
		return PGUtil::StringDatum(*(size_t*)&m_varx_days[(var_rec-1)*pg.Shape.RecordIndexSize]);
	else
		return "";
	}

string CPG::GetVarDateMonthsMean(size_t var_rec, size_t months)
{
	//return *m_var[varno-1][var_rec-1];
	if(!m_varxMonths) {
		GetNumRecordsMonth(months);
	}
	if(var_rec<=m_numrecords_Months)
		return PGUtil::StringDatum(*(size_t*)&m_varx_months[(var_rec-1)*pg.Shape.RecordIndexSize]);
	else
		return "";
	}
string CPG::GetVarDateYearsMean(size_t var_rec, size_t year)
{
	//return *m_var[varno-1][var_rec-1];
	if(!m_varxYears) {
		GetNumRecordsYear(year);
	}
	if(var_rec<=m_numrecords_Years)
		return PGUtil::StringDatum(*(size_t*)&m_varx_years[(var_rec-1)*pg.Shape.RecordIndexSize]);
	else
		return "";
	}

size_t CPG::GetNumRecordsMinute(size_t minutes, size_t rep)
{
if(!m_varxMinutes) {
		m_numrecords_Minutes=(GetLongTimeEnd()-GetLongTimeStart())/minutes+1;
		m_varx_minute= new float[pg.Shape.RecordSize*m_numrecords_Minutes];
		size_t currentmin;
		size_t ist,next;
		size_t *inum=new size_t[pg.Shape.NumVar];
		float *sumv=new float[pg.Shape.NumVar];
		size_t count;
		currentmin=GetLongTimeStart()-minutes/2;
		ist=1;
		next=GetLongTime(ist)+minutes/2;
		count=0;
		while (ist<=pg.Shape.NumRecords) {	
			for(size_t i=0;i<pg.Shape.NumVar;i++){
				*(sumv+i)=float(0);
				*(inum+i)=size_t(0);

			}
			while(currentmin<next&&ist<=pg.Shape.NumRecords) {
				for(size_t i=0;i<pg.Shape.NumVar;i++) {
					if(pg.Var.v[(ist-1)*pg.Shape.RecordIndexSize+i+1]>-1.e37) {
						*(inum+i)+=1;
						*(sumv+i)+=pg.Var.v[(ist-1)*pg.Shape.RecordIndexSize+i+1];
					}					
				}
				ist++;
				if(ist<=pg.Shape.NumRecords)
					currentmin=GetLongTime(ist);
			}
			for(size_t i=0;i<pg.Shape.NumVar;i++){
				if(*(inum+i)>0) {
					*(m_varx_minute+(count)*pg.Shape.RecordIndexSize+i+1)=*(sumv+i)/size_t(*(inum+i));
					*(sumv+i)=float(0);
					*(inum+i)=size_t(0);
				}
				else
					*(m_varx_minute+count*pg.Shape.RecordIndexSize+i+1)=MISSING;
			}	
			size_t t=next-minutes/2;
			*(m_varx_minute+ count*pg.Shape.RecordIndexSize)=*(float*)&t;
			count++;
			next+=minutes;
		}
		delete []inum;
		delete []sumv;
		m_varxMinutes=true;
	}
	return m_numrecords_Minutes;
}
size_t CPG::GetNumRecordsHour(size_t hour, size_t rep)
{
	if(!m_varxHours) {
		size_t iy=GetYear(1);
		size_t im=GetMonth(1);
		size_t id=GetDay(1);

		m_numrecords_Hours=(GetLongTimeEnd()-GetLongTimeStart())/(hour*60)+1;
		m_varx_hour= new float[pg.Shape.RecordSize*m_numrecords_Hours];
		size_t currentmin;
		size_t ist,next;
		size_t *inum=new size_t[pg.Shape.NumVar];
		float *sumv=new float[pg.Shape.NumVar];
		size_t count;
		currentmin=GetLongTimeStart();
		string datumstart=PGUtil::StringDatum(currentmin);
		datumstart=datumstart.substr(0,14);
		datumstart+="00";

		currentmin=PGUtil::MinutConv(datumstart);
		ist=1;
		next=currentmin+60*hour;
		count=0;
		while (ist<=pg.Shape.NumRecords) {	
			for(size_t i=0;i<pg.Shape.NumVar;i++){
				*(sumv+i)=float(0);
				*(inum+i)=size_t(0);

			}
			while(currentmin<=next&&ist<=pg.Shape.NumRecords) {
				for(size_t i=0;i<pg.Shape.NumVar;i++) {
					if(GetVarValue(i+1, ist)>-1.E37) {
						*(inum+i)+=1;
						*(sumv+i)+=GetVarValue(i+1, ist);
					}					
				}
				ist++;
				if(ist<=pg.Shape.NumRecords)
					currentmin=GetLongTime(ist);
			}
			for(size_t i=0;i<pg.Shape.NumVar;i++){
				if(*(inum+i)>0) {
					*(m_varx_hour+(count)*pg.Shape.RecordIndexSize+i+1)=*(sumv+i)/size_t(*(inum+i));
					*(sumv+i)=float(0);
					*(inum+i)=size_t(0);
				}
				else
					*(m_varx_hour+count*pg.Shape.RecordIndexSize+i+1)=MISSING;
			}	

			size_t t=next-(hour*60)/2;
			*(m_varx_hour+ count*pg.Shape.RecordIndexSize)=*(float*)&t;
			count++;
			next+=hour*60;
		}
		delete []inum;
		delete []sumv;
		m_varxHours=true;
		if (count < m_numrecords_Hours) m_numrecords_Hours = count;
		return m_numrecords_Hours;
	}
	else
		return -1;
}
size_t CPG::GetNumRecordsDays(size_t days, size_t rep)
{
	if(!m_varxDays) {
		int iy=int(GetYear(1));
		int im=int(GetMonth(1));
		int id=int(GetDay(1));
		size_t prior,last;
		size_t count;
		string Datum,tmp;
		/*tmp.Format("%04u", iy);
		Datum=tmp;
		tmp.Format("%02u", im);
		Datum+=tmp;
		tmp.Format("%02u", id);
		Datum+=tmp;
		tmp.Format("%02u", 0);
		Datum+=tmp;
		tmp.Format("%02u", 0);
		Datum+=tmp;*/
		Datum.resize(13);
		auto koll=snprintf(&Datum[0],13,"%04u%02u%02u%02u%02u",iy,im,id,0,0);
		prior=PGUtil::MinutConv(Datum);
		iy=GetYear(pg.Shape.NumRecords);
		im=GetMonth(pg.Shape.NumRecords);
		id=GetDay(pg.Shape.NumRecords);
	/*	tmp.Format("%04u", iy);
		Datum=tmp;
		tmp.Format("%02u", im);
		Datum+=tmp;
		tmp.Format("%02u", id);
		Datum+=tmp;
		tmp.Format("%02u", 24);
		Datum+=tmp;
		tmp.Format("%02u", 0);
		Datum+=tmp;*/
		koll=snprintf(&Datum[0],13,"%04u%02u%02u%02u%02u",iy,im,id,24,0);
		last=PGUtil::MinutConv(Datum);
		m_numrecords_Days=(last-prior)/(days*1440);
		m_varx_days= new float[pg.Shape.RecordSize*m_numrecords_Days];
		size_t currentmin;
		size_t ist,next;
		size_t *inum=new size_t[pg.Shape.NumVar];
		float *sumv=new float[pg.Shape.NumVar];
		auto str = GetDate(1);
		currentmin=GetLongTimeStart();
		auto datestr= GetDate(1);
		ist=1;
		next=prior+1440*days;
		count=0;
		while (ist<=pg.Shape.NumRecords) {	
			for(size_t i=0;i<pg.Shape.NumVar;i++){
				*(sumv+i)=float(0);
				*(inum+i)=size_t(0);
			}
			while(currentmin<next&&ist<=pg.Shape.NumRecords) {
				for(size_t i=0;i<pg.Shape.NumVar;i++) {
					if(pg.Var.v[(ist-1)*pg.Shape.RecordIndexSize+i+1]>-1.e37) {
						*(inum+i)+=1;
						*(sumv+i)+=pg.Var.v[(ist-1)*pg.Shape.RecordIndexSize+i+1];
					}					
				}
				ist++;	
				if(ist<=pg.Shape.NumRecords) {
					currentmin=GetLongTime(ist);
				}
			}
			for(size_t i=0;i<pg.Shape.NumVar;i++){
				if(*(inum+i)>0) {
					*(m_varx_days+(count)*pg.Shape.RecordIndexSize+i+1)=*(sumv+i)/size_t(*(inum+i));
					*(sumv+i)=float(0);
					*(inum+i)=size_t(0);
				}
				else
					*(m_varx_days+count*pg.Shape.RecordIndexSize+i+1)=MISSING;
			}		
			size_t t=next-(days*1440)/2;
			*(m_varx_days+ count*pg.Shape.RecordIndexSize)=*(float*)&t;
			count++;
			next+=days*1440;
		}
		delete []inum;
		delete []sumv;
		m_varxDays=true;
		return m_numrecords_Days;
	}
	else
		return -1;
}
size_t CPG::GetNumMonths() {

		int im=int(GetMonth(1));
		int iy=int(GetYear(1));
		int ime=int(GetMonth(pg.Shape.NumRecords));
		int iye=int(GetYear(pg.Shape.NumRecords));
		size_t prior;
		string Datum,tmp;
		Datum.resize(13);

		auto koll=snprintf(&Datum[0],13,"%04u%02u%02u%02u%02u",iy,im,1,0,0);
		prior=PGUtil::MinutConv(Datum);
		m_numrecords_Months=(iye-iy)*12+ime-im+1;
		return m_numrecords_Months;

}
size_t CPG::GetNumRecordsMonth(size_t months, size_t rep)
{
	if(!m_varxMonths) {
		int im=int(GetMonth(1));
		int iy=int(GetYear(1));
		int ime=int(GetMonth(pg.Shape.NumRecords));
		int iye=int(GetYear(pg.Shape.NumRecords));
		size_t prior;
		size_t count;
		string Datum,tmp;
		Datum.resize(13);

		snprintf(&Datum[0],13,"%04u%02u%02u%02u%02u",iy,im,1,0,0);
		prior=PGUtil::MinutConv(Datum);
		prior=PGUtil::cminutx(iy,im,1,0,0);
		m_numrecords_Months=(iye-iy)*12+ime-im+1;
		m_varx_months.resize(pg.Shape.RecordSize*m_numrecords_Months);
		size_t currentmin;
		size_t ist,next;


		if(im<12)
			im++;
		else {
			iy++;
			im=1;
		}
		currentmin=GetLongTimeStart();
		if (currentmin == string::npos) {
			if (ReOpen()) {
				currentmin = GetLongTimeStart();
			}
			if (currentmin == string::npos) {
				return 0;
			}
		}

		size_t *inum = new size_t[pg.Shape.NumVar];
		float *sumv = new float[pg.Shape.NumVar];



		ist=1;
		snprintf(&Datum[0],13,"%04u%02u%02u%02u%02u",iy,im,1,0,0);

		next=PGUtil::MinutConv(Datum);
		next=PGUtil::cminutx(iy,im,1,0,0);
		count=0;
		while (ist<=pg.Shape.NumRecords) {	
			for(size_t i=0;i<pg.Shape.NumVar;i++){
				*(sumv+i)=float(0);
				*(inum+i)=size_t(0);
			}
			while(currentmin<=next&&ist<=pg.Shape.NumRecords) {
				for(size_t i=0;i<pg.Shape.NumVar;i++) {
					float value=GetVarValue(i+1, ist);
					if(value>-1.e37) {
						*(inum+i)+=1;
						*(sumv+i)+=value;;
					}					
				}
				ist++;
				if(ist<=pg.Shape.NumRecords) {
					currentmin=GetLongTime(ist);
				}
				else
					currentmin=next;
			}
			for(size_t i=0;i<pg.Shape.NumVar;i++){
				if(*(inum+i)>0) {
					m_varx_months[count*pg.Shape.RecordIndexSize+i+1]=*(sumv+i)/size_t(*(inum+i));
					*(sumv+i)=float(0);
					*(inum+i)=size_t(0);
				}
				else
					m_varx_months[count*pg.Shape.RecordIndexSize+i+1]=MISSING;
			}		
			size_t t=(prior+next)/2;
			m_varx_months[count*pg.Shape.RecordIndexSize]=*(float*)&t;
			count++;
			if(im<12)
				im++;
			else {
				iy++;
				im=1;
			}
			prior=next;
			snprintf(&Datum[0],13,"%04u%02u%02u%02u%02u",iy,im,1,0,0);
			next=PGUtil::cminutx(iy,im,1,0,0);
			//next=PGUtil::MinutConv(Datum);
		}
		delete []inum;
		delete []sumv;
		m_varxMonths=true;
		return m_numrecords_Months;
	}
	else
		return m_numrecords_Months; 
}
size_t CPG::GetNumRecordsYear(size_t years, size_t repn)
{
	if(!m_varxYears) {
		size_t global_rep_index, maxrec;
		if(!pg.status.CompleteFileRead) {
				global_rep_index=0;
				maxrec=pg.status.b.buf_Size;
		}
		else {
				maxrec=pg.Shape.NumRecords;
				global_rep_index=(repn-1)*pg.Shape.NumRecords*pg.Shape.RecordIndexSize;
		}

		int im=1;
		int iy=int(GetYear(1));
		int ime=1;
		int iye=int(GetYear(maxrec));
		size_t prior;
		size_t count;
		string Datum,tmp;
		Datum.resize(13);

		snprintf(&Datum[0],13,"%04u%02u%02u%02u%02u",iy,1,1,0,0);
		prior=PGUtil::MinutConv(Datum);
		m_numrecords_Years=(iye-iy)*1+1;
		m_varx_years= new float[pg.Shape.RecordSize*m_numrecords_Years];
		size_t currentmin;
		size_t ist,next;
		size_t *inum=new size_t[pg.Shape.NumVar];
		float *sumv=new float[pg.Shape.NumVar];
		iy++;
		currentmin=GetLongTime(global_rep_index+1);
		ist=1;
		snprintf(&Datum[0],13,"%04u%02u%02u%02u%02u",iy,1,1,0,0);
		next=PGUtil::MinutConv(Datum);
		count=0;
		while (currentmin<GetLongTime(maxrec)&&ist<maxrec) {	
			for(size_t i=0;i<pg.Shape.NumVar;i++){
				*(sumv+i)=float(0);
				*(inum+i)=size_t(0);
			}
			while(currentmin<next) {
				for(size_t i=0;i<pg.Shape.NumVar;i++) {
					//float value=GetVarValueRead(i+1, ist);
					if(pg.Var.v[(ist-1)*pg.Shape.RecordIndexSize+i+1+global_rep_index]>-1.e37) {
						*(inum+i)+=1;
						*(sumv+i)+=pg.Var.v[(ist-1)*pg.Shape.RecordIndexSize+i+1+global_rep_index];
					}					
				}
				if(ist<maxrec) {
					ist++;
					currentmin=GetLongTime(ist+global_rep_index );
				}
				else
					currentmin=next;
			}
			for(size_t i=0;i<pg.Shape.NumVar;i++){
				if(*(inum+i)>0) {
					*(m_varx_years+count*pg.Shape.RecordIndexSize+i+1)=*(sumv+i)/size_t(*(inum+i));
					*(sumv+i)=float(0);
					*(inum+i)=size_t(0);
				}
				else
					*(m_varx_years+count*pg.Shape.RecordIndexSize+i+1)=MISSING;
			}	
			size_t t=(prior+next)/2;
			*(m_varx_years+ count*pg.Shape.RecordIndexSize)=*(float*)&t;
			count++;
			iy++;
			prior=next;
			snprintf(&Datum[0],13,"%04u%02u%02u%02u%02u",iy,1,1,0,0);
			next=PGUtil::MinutConv(Datum);
		}
		delete []inum;
		delete []sumv;
		m_varxYears=true;
		if(m_numrecords_Years>count)
			m_numrecords_Years=count;
		return m_numrecords_Years;
	}
	else
		return -1;
}
float *CPG::GetMeanPointer(size_t MeanOption)
{
	switch(MeanOption) {
		case 0:	return m_varx_years;
		case 1:	return m_varx_months.data();
		case 2: return m_varx_days;
		case 3: return m_varx_hour;
		case 4: return m_varx_minute;
	}
	return nullptr;
}
void CPG::SetMeanReset(size_t MeanOption)
{
	switch(MeanOption) {
		case 0:	m_varxYears=false; if(m_varx_years!=nullptr) delete m_varx_years; m_varx_years=nullptr; return;
		case 1:	m_varxMonths=false; if(m_varx_months.size()>0)  m_varx_months.clear(); return;
		case 2: m_varxDays=false; if(m_varx_days!=nullptr) delete m_varx_days;m_varx_days=nullptr; return;
		case 3: m_varxHours=false;  if(m_varx_hour!=nullptr) delete m_varx_hour;m_varx_hour=nullptr; return;
		case 4: m_varxMinutes=false; if(m_varx_minute!=nullptr) delete m_varx_minute; m_varx_minute=nullptr; return;
	}
	return;
}

size_t CPG::GetYear(size_t var_rec)
{
	string str=GetDate(var_rec);
	auto im=FUtil::AtoInt(str.substr(0,4));
	return size_t(im);
}
size_t CPG::GetMonth(size_t var_rec)
{
	string str=GetDate(var_rec);
	if(str.size()>5) {
		size_t im=FUtil::AtoInt(str.substr(5,2));
		return im;
	}
	else 
		return -1;
}
size_t CPG::GetDay(size_t var_rec)
{
	string str=GetDate(var_rec);
	if(str.size()>8) {
		size_t id=FUtil::AtoInt(str.substr(8,2));
		return id;
	}
	else
		return -1;
}
size_t CPG::GetDayNumber(size_t var_rec)
{
	string str=GetDate(var_rec);
	string newyear=str.substr(0,4)+"01010000";
	size_t styear=PGUtil::MinutConv(newyear);
	size_t id=GetLongTime(var_rec)-styear+1440;
	return size_t(id/1440);
}
size_t CPG::GetMinuteStartThisYear(size_t var_rec)
{
	string str=GetDate(var_rec);
	string newyear;
	if(str.size()<10)
		newyear="000101010000";
	else
		newyear=str.substr(0,4)+"01010000";
	return PGUtil::MinutConv(newyear);
}
double CPG::GetDoubleTimeYearStart(size_t var_rec)
{
	size_t min;
	min=GetMinuteStartThisYear(var_rec);
	double value;
	value=(min-m_time1900_Zero)*double(60);
	return value;
}
double CPG::GetDoubleTimeYearEnd(size_t var_rec)
{
	size_t min;
	min=GetMinuteStartNextYear(var_rec);
	double value;
	value=(min-m_time1900_Zero)*double(60);
	return value;
}
size_t CPG::GetMinuteStartNextYear(size_t var_rec)
{
	auto year=int(GetYear(var_rec));
	year++;
	string str;
	str.resize(10);
	if(year<0||year>10000) return 0;
	//str.Format("%u4",year);
	snprintf(&str[0],6,"%u",year);
	//str=FUtil::ItoNumAscii(year);
	string newyear=str.substr(0,4)+"01010000";

	return PGUtil::MinutConv(newyear);
}
size_t CPG::GetMinuteStartNextDay(size_t var_rec)
{
	string str;
	str=GetDate(var_rec);
	str=str.substr(0,11)+"00:00";
	return PGUtil::MinutConv(str)+1440;	
}
size_t CPG::GetMinuteNumber(size_t var_rec)
{
	string str=GetDate(var_rec);
	str[11]='0';
	str[12]='0';
	str[14]='0';
	str[15]='0';
	size_t stday=PGUtil::MinutConv(str);
	size_t id=GetLongTime(var_rec)-stday;
	return size_t(id);
}		
void CPG::SetTimeConvBase(size_t value)
{
	m_time_base=value;	
}
size_t CPG::GetTimeConvBase()
{
	return m_time_base;	
}
void CPG::SetTimeConvUnit(size_t value)
{
	m_time_unit=value;	
}
size_t CPG::GetTimeConvUnit()
{
	return m_time_unit;
}
bool CPG::CalcTimeToVar(size_t varno, size_t repn) 
{
	float vnew, vmin, vmax;
	bool missing;
	size_t time, timestart;
	vnew = MISSING;
	vmax=MISSING;
	vmin=abs(MISSING);
	timestart=GetLongTimeStart();
	size_t var_out;
	var_out=GetCalcVarNew();

	size_t global_rep_index, maxrec;
	if(!pg.status.CompleteFileRead) {
		global_rep_index=0;
		maxrec=pg.status.b.buf_Size;
	}
	else {
		maxrec=pg.Shape.NumRecords;
		global_rep_index=(repn-1)*pg.Shape.NumRecords*pg.Shape.RecordIndexSize;
	}


	for(size_t var_rec=1;var_rec<=maxrec;var_rec++) {
		if(m_expression==4) {
			time=GetLongTime(var_rec);
			vnew=float(double(time-timestart)/1440.);
		}
		else if(m_expression==1) {
			vnew=float(GetDayNumber(var_rec));
		}
		else if(m_expression==2) {
			vnew=float(GetMonth(var_rec));		
		}
		else if(m_expression==3) {
			vnew=float(GetMinuteNumber(var_rec));
		}
		missing=false;
		if(!m_DateSelection) 
			pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out]=vnew;
		else if(var_rec>=m_CalcStart&&var_rec<=m_CalcStop)
			pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out]=vnew;
		else {
			//pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out]=MISSING;
			missing=true;
		}
		if(!missing) {
			if(vnew>vmax) vmax=vnew;
			if(vnew<vmin) vmin=vnew;
		}
	}
	SetVarMin(var_out,vmin);
	SetVarMax(var_out,vmax);
	return true;
}
bool CPG::CalcTimeChange(size_t repn)
{	
	float ADay, AMin;

	size_t mindiff, minut;
	size_t var_in;
	mindiff = 0;

	if(m_expression<=2&&GetNumOfSelected()>1) {
		ADay=GetSelectionWeight(0);
		AMin=GetSelectionWeight(1);
		mindiff=size_t(float(ADay*1440)+AMin);
	}
	else if(m_expression==3) {
		var_in=GetCalcVar();
		if(var_in<=0) return false;
	}
	else if (m_expression == 4) {
		mindiff = 1440;
		minut = GetLongTimeStart();
	}
	else {
		return false;
	}

	size_t global_rep_index, maxrec;
	bool withinbuffer;
	withinbuffer=true;
	if(!pg.status.CompleteFileRead) {
		global_rep_index=0;
		maxrec=pg.status.b.buf_Size;
	}
	else {
		maxrec=pg.Shape.NumRecords;
		global_rep_index=(repn-1)*pg.Shape.NumRecords*pg.Shape.RecordIndexSize;
	}


	for(size_t var_rec=1;var_rec<=maxrec;var_rec++) {
		minut=GetLongTime(var_rec, withinbuffer);
		if(m_expression==1)
			minut-=mindiff;
		else if(m_expression==2)
			minut+=mindiff;
		else if(m_expression==3) {
			float vnew=GetVarValue(var_in,var_rec);//*(m_varx+(var_rec-1)*pg.Shape.RecordIndexSize+var_in);
			if(m_time_unit==1)
				mindiff=size_t(double(vnew*365.25)*1440.);
			else if(m_time_unit==2)
				mindiff=size_t(vnew*1440.);
			else if(m_time_unit==3)
				mindiff=size_t(vnew*60.);
			else if(m_time_unit==4)
				mindiff=size_t(vnew);
			minut=mindiff+m_time_base;
		}
		else if (m_expression == 4) {
			minut = GetLongTimeStart() + mindiff*(var_rec - 1);

		}

		if(!m_DateSelection) 
			SetLongTime(var_rec, minut, withinbuffer);
			//*(m_varx+(var_rec-1)*pg.Shape.RecordIndexSize)=*(float*)&minut;
		else if(var_rec>=m_CalcStart&&var_rec<=m_CalcStop)
			//*(m_varx+(var_rec-1)*pg.Shape.RecordIndexSize)=*(float*)&minut;	
			SetLongTime(var_rec,minut, withinbuffer);
	}
	return true;
	//return true;// WritePGFileData(maxrec);
	//return SaveAs(GetFileName(), true);

	//return true;
}
bool CPG::CalcVarTimeDerivate(size_t repn)
{	size_t mindiff, minut, minprev;
	float derive;

	minprev=GetLongTime(1);

	size_t global_rep_index, maxrec;
	if(!pg.status.CompleteFileRead) {
		global_rep_index=0;
		maxrec=pg.status.b.buf_Size;
	}
	else {
		maxrec=pg.Shape.NumRecords;
		global_rep_index=(repn-1)*pg.Shape.NumRecords*pg.Shape.RecordIndexSize;
	}

	for(size_t var_rec=1;var_rec<=maxrec;var_rec++) {
		minut=GetLongTime(var_rec);
		mindiff=minut-minprev;
		derive=float((minut-mindiff)/1440.);
		minprev=minut;
		if(mindiff>0) {
			derive=float(mindiff)/1440.f;
			for (size_t j=0;j<pg.Shape.NumVar;j++) {
				pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+j+1]=pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+j+1]/derive;
			}
		
		}
		
		minut=minut-mindiff/2;
		if(!m_DateSelection) 
			SetLongTime(var_rec, minut,true);
		else if(var_rec>=m_CalcStart&&var_rec<=m_CalcStop)
			SetLongTime(var_rec,minut, true);
	}

	return SaveAs(GetFileName(), true);

}
bool CPG::CalcVarDifference(size_t var_in, size_t var_out, size_t repn) 
{
	float vnew,x, vmin, vmax;
	float prev;
	bool missing;
	vmax=MISSING;
	vmin=abs(MISSING);
	if(var_out<0) var_out=var_in;
	if(!(var_in>0&&var_in<=pg.Shape.NumVar&&var_out>0&&var_out<=pg.Shape.NumVar)) {
		return false;
	}
	if(GetNumOfSelected()<3)
		return false;

	prev=pg.Var.v[var_in];
	size_t global_rep_index, maxrec;
	if(!pg.status.CompleteFileRead) {
		global_rep_index=0;
		maxrec=pg.status.b.buf_Size;
	}
	else {
		maxrec=pg.Shape.NumRecords;
		global_rep_index=(repn-1)*pg.Shape.NumRecords*pg.Shape.RecordIndexSize;
	}
	for(size_t var_rec=1;var_rec<=maxrec;var_rec++) {	
		x=pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_in+pg.status.a.LocalStartIndex];
		if(x>-1.E37) {
			vnew=x-prev;
			prev=x;

			missing=false;
			if(!m_DateSelection) 
				pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out]=vnew;
			else if(var_rec>=m_CalcStart&&var_rec<=m_CalcStop)
				pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out]=vnew;
			else {
				//pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out]=MISSING;
				missing=true;
			}
			if(!missing) {
				if(vnew>vmax) vmax=vnew;
				if(vnew<vmin) vmin=vnew;
			}
		}
		else {
			pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out]=MISSING;
		}
	}
	SetVarMin(var_out,vmin, m_ActualSequence);
	SetVarMax(var_out,vmax, m_ActualSequence);
	return true;
}

bool CPG::CalcVarScaling(size_t var_in, size_t var_out, size_t repn) 
{
	float vnew,x, vmin, vmax;
	float A0,A1,A2;
	bool missing;
	vmax=MISSING;
	vmin=abs(MISSING);
	if(var_out<0) var_out=var_in;

	if(!(var_in>0&&var_in<=pg.Shape.NumVar&&var_out>0&&var_out<=pg.Shape.NumVar)) {
		return false;
	}
	if(GetNumOfSelected()<3)
		return false;
	A0=GetSelectionWeight(0);
	A1=GetSelectionWeight(1);
	A2=GetSelectionWeight(2);

	size_t global_rep_index, maxrec;
	if(!pg.status.CompleteFileRead) {
		global_rep_index=0;
		maxrec=pg.status.b.buf_Size;
	}
	else {
		maxrec=pg.Shape.NumRecords;
		global_rep_index=(repn-1)*pg.Shape.NumRecords*pg.Shape.RecordIndexSize;
	}
	bool TestCond = false;
	bool ValidCond = true;
	if (IsApplyConditions()) TestCond = true;

	for(size_t var_rec=1;var_rec<=maxrec;var_rec++) {	
		if (TestCond) ValidCond = CheckConditions(var_rec);
		x=pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_in];
		if(x>-1.E37&&ValidCond) {
			
			vnew=A0+A1*x+A2*x*x;


			missing=false;
			if(!m_DateSelection) 
				pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out]=vnew;
			else if(var_rec>=m_CalcStart&&var_rec<=m_CalcStop)
				pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out]=vnew;
			else {
				//pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out]=MISSING;
				missing=true;
			}
			if(!missing) {
				if(vnew>vmax) vmax=vnew;
				if(vnew<vmin) vmin=vnew;
			}
		}
		else {
			pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out]=MISSING;
		}
	}
	SetVarMin(var_out,vmin, m_ActualSequence);
	SetVarMax(var_out,vmax, m_ActualSequence);
	return true;
}

bool CPG::CalcVarSmoothing(size_t var_in, size_t var_out, size_t repn) 
{
	float vnew,x, vmin, vmax;
	float vdiffneg, vdiffpos, vlastvalid, currentdiff;
	bool missing;

	if(var_out<0) var_out=var_in;
	vmax=MISSING;
	vmin=abs(MISSING);
	if(!(var_in>0&&var_in<=pg.Shape.NumVar&&var_out>0&&var_out<=pg.Shape.NumVar)) {
		return false;
	}
	if(GetNumOfSelected()<3)
		return false;
	vdiffneg=GetSelectionWeight(1);
	vdiffpos=GetSelectionWeight(0);
	vlastvalid=MISSING;
	size_t global_rep_index, maxrec;
	if(!pg.status.CompleteFileRead) {
		global_rep_index=0;
		maxrec=pg.status.b.buf_Size;
	}
	else {
		maxrec=pg.Shape.NumRecords;
		global_rep_index=(repn-1)*pg.Shape.NumRecords*pg.Shape.RecordIndexSize;
	}
	for(size_t var_rec=1;var_rec<=maxrec;var_rec++) {	
		x=pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_in+pg.status.a.LocalStartIndex];
		if(x>-1.E37) {
			if(vlastvalid<-1.e37)
				vlastvalid=x;
			currentdiff=x-vlastvalid;
			if(currentdiff>vdiffneg&&currentdiff<vdiffpos) {
				vnew=x;
				vlastvalid=vnew;
			}
			else
				vnew=MISSING;


			missing=false;
			if(!m_DateSelection) 
				pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out]=vnew;
			else if(var_rec>=m_CalcStart&&var_rec<=m_CalcStop)
				pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out]=vnew;
			else {
		//		pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out]=MISSING;
				missing=true;
			}
			if(!missing) {
				if(vnew>vmax) vmax=vnew;
				if(vnew<vmin) vmin=vnew;
			}
		}
	}
	SetVarMin(var_out,vmin);
	SetVarMax(var_out,vmax);
	return true;
}
size_t CPG::CalcVarMissingByRandomPeriod(size_t var_in, size_t repn) 
{
	float vnew,v_new, vmin, vmax;
	vmax=MISSING;
	vmin=abs(MISSING);
	bool missing;
	size_t count;
	size_t var_out=string::npos;

	size_t year_min=1440*350;
	size_t year_max=1440*370;
	size_t year_mean=1440*365;

	if (var_out == string::npos) var_out = var_in;


	count=0;
	size_t global_rep_index, maxrec;
	if(!pg.status.CompleteFileRead) {
		global_rep_index=0;
		maxrec=pg.status.b.buf_Size;
	}
	else {
		maxrec=pg.Shape.NumRecords;
		global_rep_index=(repn-1)*pg.Shape.NumRecords*pg.Shape.RecordIndexSize;
	}
	for(size_t var_rec=1;var_rec<=maxrec;var_rec++) {
		vnew=pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_in+global_rep_index];
		missing=true;
		if(vnew<-1.E37) {
					size_t jback=var_rec-1;
					size_t ist,iend, ilong;
					ilong=GetDayNumber(var_rec);
					bool YearMatch=false;
					size_t nstep;
					nstep=year_mean/GetNormalTimeInterval();

					while((pg.Var.v[jback*pg.Shape.RecordIndexSize+var_in+global_rep_index]<-1e37||!YearMatch)&&jback>nstep){
						jback-=nstep;
						auto koll = ilong; koll -= GetDayNumber(jback + 1);
						if(koll<10)
							YearMatch=true;
						if(pg.Var.v[jback*pg.Shape.RecordIndexSize+var_in+global_rep_index]>MISSING&&YearMatch)
							break;
					}
					ist=jback;
					YearMatch=false;
					size_t jforward=var_rec-1;
					while(jforward<pg.Shape.NumRecords-nstep&&(pg.Var.v[jforward*pg.Shape.RecordIndexSize+var_in+global_rep_index]<-1.e37||!YearMatch)) {
						jforward+=nstep;
						auto koll = ilong; koll -= GetDayNumber(jforward + 1);
						if(koll<10)
							YearMatch=true;
						if(pg.Var.v[jforward*pg.Shape.RecordIndexSize+var_in+global_rep_index]>MISSING&&YearMatch)
							break;
					}
					iend=jforward;
					float vst, vend;
					if(jforward<pg.Shape.NumRecords&&jback>=0) {
						vst=pg.Var.v[jback*pg.Shape.RecordIndexSize+var_in+global_rep_index];
						vend=pg.Var.v[jforward*pg.Shape.RecordIndexSize+var_in+global_rep_index];
						if(vst>-1.E37&&vend>-1.E37) {
							v_new=(vend+vst)/2;
							count++;
							missing=false;
						}
						else if(vst>MISSING) {
							v_new=vst;
							count++;
							missing=false;

						}
						else if(vend>MISSING) {
							v_new=vend;
							count++;
							missing=false;
						}
						if(!missing)
							pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out+global_rep_index]=v_new;
					}
					


		}

	}

	return count;
}

size_t CPG::CalcVarMissingSubstitute(size_t var_in, size_t var_out, bool VariableValue,size_t repn, size_t min_gap) 
{
	float vnew,v_new, vmin, vmax;
	vmax=MISSING;
	vmin=abs(MISSING);
	bool missing;
	size_t count;
	size_t varsel;

	if(var_out<0) var_out=var_in;
	if(GetNumOfSelected()<1)
		return false;
	v_new=GetSelectionWeight(0);
	
	varsel=GetSelection(0);
	count=0;
	size_t global_rep_index, maxrec;

	if(!pg.status.CompleteFileRead) {
		global_rep_index=0;
		maxrec=GetCurrentRecords();
	}
	else {
		maxrec=pg.Shape.NumRecords;
		global_rep_index=(repn-1)*pg.Shape.NumRecords*pg.Shape.RecordIndexSize;
	}
	bool TestCond = false;
	bool ValidCond = true;
	if (IsApplyConditions()) TestCond = true;
	for(size_t var_rec=1;var_rec<=maxrec;var_rec++) {
		if (TestCond) ValidCond = CheckConditions(var_rec);
		size_t index;
		if(pg.Shape.RepetitionMode)
			index=(var_rec-1)*pg.Shape.RecordIndexSize+var_in;
		else
			index=(var_rec-1)*pg.Shape.RecordIndexSize+var_in+global_rep_index;
		if(index>=pg.Var.v.size()) break;
		vnew=pg.Var.v[index];
		missing=true;
		if(vnew<-1.E37&&ValidCond) {
			if(m_expression==5) {
					size_t jback=var_rec-1;
					size_t ist,iend, ilong;
					while(jback>0&&pg.Var.v[jback*pg.Shape.RecordIndexSize+var_in+global_rep_index]<-1e37) 
						jback--;
					ist=jback;
					size_t jforward=var_rec-1;
					while(jforward<pg.Shape.NumRecords&&pg.Var.v[jforward*pg.Shape.RecordIndexSize+var_in+global_rep_index]<-1.e37)
						jforward++;
					iend=jforward;
					float vst, vend;
					v_new=MISSING;
					if(jforward<pg.Shape.NumRecords&&jback>=0) {
						vst=pg.Var.v[jback*pg.Shape.RecordIndexSize+var_in+global_rep_index];
						vend=pg.Var.v[jforward*pg.Shape.RecordIndexSize+var_in+global_rep_index];
						if(vst>-1.E37&&vend>-1.E37) {
							ist=*(size_t*)&pg.Var.v[jback*pg.Shape.RecordIndexSize+global_rep_index];
							ist=GetLongTime(jback+1, true);

							iend=*(size_t*)&pg.Var.v[jforward*pg.Shape.RecordIndexSize+global_rep_index];//GetLongTime(jforward+1, true);
							if(iend-ist<min_gap) {
								ilong=*(size_t*)&pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+global_rep_index];//GetLongTime(var_rec, true);
								double xw=double(iend-ilong)/double(iend-ist);
								v_new=float((1.-xw)*(vend-vst)+vst);
								count++;
								missing=false;
							}
						}
					}
					
			}
			else if(VariableValue) {
				v_new=pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+varsel+global_rep_index];
			
			}
			if(!m_DateSelection) 
				pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out+global_rep_index]=v_new;
			else if(var_rec>=m_CalcStart&&var_rec<=m_CalcStop)
				pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out+global_rep_index]=v_new;
			else {
		//		pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out+(repn-1)*pg.Shape.NumRecords*pg.Shape.RecordIndexSize]=vnew;
				missing=true;
			}
		}
		else
			pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out+global_rep_index]=vnew;
		if(!missing&&vnew>-1.E37) {
			if(vnew>vmax) vmax=vnew;
			if(vnew<vmin) vmin=vnew;
		}
	}
	if(vmin<1.E37)
		SetVarMin(var_out,vmin);
	if(vmax>-1.E37)
		SetVarMax(var_out,vmax);
	return count;
}
size_t CPG::CalcVarMissing(size_t var_in, size_t var_out, size_t repn) 
{
	float vnew, ValidMin,ValidMax, vmin, vmax;

	bool missing;
	size_t count=0;
	if(var_out<0) var_out=var_in;
	ValidMin=GetSelectionWeight(0);
	ValidMax=GetSelectionWeight(1);
	vmax=MISSING;
	vmin=abs(MISSING);
	size_t global_rep_index, maxrec;
	if(!pg.status.CompleteFileRead) {
		global_rep_index=0;
		maxrec=pg.status.b.buf_Size;
	}
	else {
		maxrec=pg.Shape.NumRecords;
		global_rep_index=(repn-1)*pg.Shape.NumRecords*pg.Shape.RecordIndexSize;
	}
	bool TestCond = false;
	bool ValidCond = true;
	if (IsApplyConditions()) TestCond = true;
	for(size_t var_rec=1;var_rec<=maxrec;var_rec++) {
		if (TestCond) ValidCond = CheckConditions(var_rec);
		if (ValidCond) {
			vnew = pg.Var.v[(var_rec - 1)*pg.Shape.RecordIndexSize + var_in];
			if ((vnew<ValidMin || vnew>ValidMax) && m_expression == 2 ||
				(vnew > ValidMin&&vnew < ValidMax&&m_expression == 3)) {
				missing = false;
				if (!m_DateSelection)
					pg.Var.v[(var_rec - 1)*pg.Shape.RecordIndexSize + var_out] = MISSING;
				else if (var_rec >= m_CalcStart&&var_rec <= m_CalcStop)
					pg.Var.v[(var_rec - 1)*pg.Shape.RecordIndexSize + var_out] = MISSING;
				else {
					//pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out]=vnew;
					missing = true;
				}
				if (missing) {
					if (vnew > vmax) vmax = vnew;
					if (vnew < vmin) vmin = vnew;
				}
			}

			else {
				pg.Var.v[(var_rec - 1)*pg.Shape.RecordIndexSize + var_out] = vnew;
				if (vnew > vmax) vmax = vnew;
				if (vnew < vmin) vmin = vnew;
			}
		}
	}
	SetVarMin(var_out,vmin);
	SetVarMax(var_out,vmax);
	return count;
}
bool CPG::CalcVarDivide(size_t varno, size_t repn) 
{
	float vnew,x, vmin, vmax;
	double vv, num;
	size_t varsel;
	bool missing;
	vmax=MISSING;
	vmin=abs(MISSING);
	size_t var_out=GetCalcVarNew();
	size_t global_rep_index, maxrec;
	if(!pg.status.CompleteFileRead) {
		global_rep_index=0;
		maxrec=pg.status.b.buf_Size;
	}
	else {
		maxrec=pg.Shape.NumRecords;
		global_rep_index=(repn-1)*pg.Shape.NumRecords*pg.Shape.RecordIndexSize;
	}
	bool TestCond = false;
	bool ValidCond = true;
	if (IsApplyConditions()) TestCond = true;

	for(size_t var_rec=1;var_rec<=maxrec;var_rec++) {
		num=0;
		vv=0.;
		if (TestCond) ValidCond = CheckConditions(var_rec);
		if (ValidCond) {
			for (size_t i = 0; i < GetNumOfSelected(); i++) {
				varsel = GetSelection(i);
				//if(varsel>varno&&m_Replace) varsel++;
				x = pg.Var.v[(var_rec - 1)*pg.Shape.RecordIndexSize + varsel + pg.status.a.LocalStartIndex];
				if (x > -1.E37&&num > 0 && x != 0.) {
					vv = vv / x;
				}
				else if (x > -1.E37) {
					num = 1;
					vv = x;
				}

			}
			if (num > 0)
				vnew = float(vv);
			else
				vnew = MISSING;

			missing = false;
			if (!m_DateSelection)
				pg.Var.v[(var_rec - 1)*pg.Shape.RecordIndexSize + var_out + pg.status.a.LocalStartIndex] = vnew;
			else if (var_rec >= m_CalcStart&&var_rec <= m_CalcStop)
				pg.Var.v[(var_rec - 1)*pg.Shape.RecordIndexSize + var_out + pg.status.a.LocalStartIndex] = vnew;
			else {
				//	pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out]=MISSING;
				missing = true;
			}
			if (!missing) {
				if (vnew > vmax) vmax = vnew;
				if (vnew < vmin) vmin = vnew;
			}
		}
	}
	SetVarMin(var_out,vmin);
	SetVarMax(var_out,vmax);
	return true;
}
bool CPG::CalcVarMultiply(size_t varno, size_t repn) 
{
	float vnew,x, vmin, vmax;
	double vv, num;
	size_t varsel;
	bool missing;
	size_t var_out=GetCalcVarNew();
	vmax=MISSING;
	vmin=abs(MISSING);
	size_t global_rep_index, maxrec;
	if(!pg.status.CompleteFileRead) {
		global_rep_index=0;
		maxrec=pg.status.b.buf_Size;
	}
	else {
		maxrec=pg.Shape.NumRecords;
		global_rep_index=(repn-1)*pg.Shape.NumRecords*pg.Shape.RecordIndexSize;
	}
	bool TestCond = false;
	bool ValidCond = true;
	if (IsApplyConditions()) TestCond = true;
	for(size_t var_rec=1;var_rec<=maxrec;var_rec++) {
		num=0;
		vv=0.;
		if (TestCond) ValidCond = CheckConditions(var_rec);
		if (ValidCond) {
			for (size_t i = 0; i < GetNumOfSelected(); i++) {
				varsel = GetSelection(i);
				//if(varsel>varno&&m_Replace) varsel++;
				x = pg.Var.v[(var_rec - 1)*pg.Shape.RecordIndexSize + varsel + pg.status.a.LocalStartIndex];
				if (x > -1.E37&&num > 0) {
					vv = vv*x;
				}
				else if (x > MISSING) {
					num = 1;
					vv = x;
				}

			}
			if (num > 0)
				vnew = float(vv);
			else
				vnew = MISSING;

			missing = false;
			if (!m_DateSelection)
				pg.Var.v[(var_rec - 1)*pg.Shape.RecordIndexSize + var_out + pg.status.a.LocalStartIndex] = vnew;
			else if (var_rec >= m_CalcStart&&var_rec <= m_CalcStop)
				pg.Var.v[(var_rec - 1)*pg.Shape.RecordIndexSize + var_out + pg.status.a.LocalStartIndex] = vnew;
			else {
				//pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out]=MISSING;
				missing = true;
			}
			if (!missing) {
				if (vnew > vmax) vmax = vnew;
				if (vnew < vmin) vmin = vnew;
			}
		}
	}
	SetVarMin(var_out,vmin);
	SetVarMax(var_out,vmax);
	return true;
}
bool CPG::CalcVarSum(size_t varno, size_t repn) 
{
	float vnew,x, vmin, vmax, weight;
	double vv, num;
	size_t varsel;
	bool missing;
	size_t var_out=GetCalcVarNew();
	vmax=MISSING;
	vmin=abs(MISSING);
	size_t global_rep_index, maxrec;
	if(!pg.status.CompleteFileRead) {
		global_rep_index=0;
		maxrec=pg.status.b.buf_Size;
	}
	else {
		maxrec=pg.Shape.NumRecords;
		global_rep_index=(repn-1)*pg.Shape.NumRecords*pg.Shape.RecordIndexSize;
	}
	for(size_t var_rec=1;var_rec<=maxrec;var_rec++) {
		num=0;
		vv=0.;
		for(size_t i=0; i<GetNumOfSelected();i++) {
			varsel=GetSelection(i);
			weight=GetSelectionWeight(i);
			//if(varsel>varno&&m_Replace) varsel++;
			x=pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+varsel+pg.status.a.LocalStartIndex];
			if(x>-1.E37) {
				num+=1.;
				vv+=x*weight;
			}

		}
		if(num>0)
			vnew=float(vv);
		else
			vnew=MISSING;
		missing=false;
		if(!m_DateSelection) 
			pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out+pg.status.a.LocalStartIndex]=vnew;
		else if(var_rec>=m_CalcStart&&var_rec<=m_CalcStop)
			pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out+pg.status.a.LocalStartIndex]=vnew;
		else {
			//pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out]=MISSING;
			missing=true;
		}
		if(!missing) {
			if(vnew>vmax) vmax=vnew;
			if(vnew<vmin) vmin=vnew;
		}
	}
	SetVarMin(var_out,vmin);
	SetVarMax(var_out,vmax);
	return true;
}
bool CPG::CalcVarMean(size_t varno, size_t repn) 
{
	float vnew,x, vmin, vmax;
	double vv, num;
	size_t varsel;
	bool missing;
	size_t var_out=GetCalcVarNew();
	vmax=MISSING;
	vmin=abs(MISSING);
	size_t global_rep_index, maxrec;
	if(!pg.status.CompleteFileRead) {
		global_rep_index=0;
		maxrec=pg.status.b.buf_Size;
	}
	else {
		maxrec=pg.Shape.NumRecords;
		global_rep_index=(repn-1)*pg.Shape.NumRecords*pg.Shape.RecordIndexSize;
	}
	for(size_t var_rec=1;var_rec<=maxrec;var_rec++) {
		num=0;
		vv=0.;
		for(size_t i=0; i<GetNumOfSelected();i++) {
			varsel=GetSelection(i);
			//if(varsel>varno&&m_Replace) varsel++;
			x=pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+varsel+pg.status.a.LocalStartIndex];
			if(x>-1.E37) {
				num+=1.;
				vv+=x;
			}

		}
		if(num>0) {
			vnew=float(vv/num);
		}
		else
			vnew=MISSING;
			
		missing=false;
		if(!m_DateSelection) 
			pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out+pg.status.a.LocalStartIndex]=vnew;
		else if(var_rec>=m_CalcStart&&var_rec<=m_CalcStop)
			pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out+pg.status.a.LocalStartIndex]=vnew;
		else {
			//pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+var_out]=MISSING;
			missing=true;
		}
		if(!missing) {
			if(vnew>vmax) vmax=vnew;
			if(vnew<vmin) vmin=vnew;
		}

	}
	SetVarMin(var_out,vmin);
	SetVarMax(var_out,vmax);
	return true;
}
bool CPG::AddNewVariable(size_t varno)
{

	// We have to create a dumping area if this should work with more than one buffer.
	fstream	 dumpstream;
	string dumpfile;
	if(!pg.status.CompleteFileRead) {
		dumpfile=GetFileName();
		dumpfile=dumpfile.substr(0,dumpfile.size()-4);
		dumpfile+="_dump.bin";

		FUtil::CopyFile_Std(GetFileName(), dumpfile);

		dumpstream.open(dumpfile, ios::in|ios::out|ios::binary);
		if(!WritePGFileStartHeader(pg.Shape.NumRecords, pg.Shape.NumRepetitions,0, &dumpstream)) {
			CloseFile();
			ReOpen(true);
			if(!WritePGFileStartHeader(pg.Shape.NumRecords, pg.Shape.NumRepetitions,0, &dumpstream)) return false;
		}
		else {
			WritePGFileData(pg.Shape.NumRecords, &dumpstream);

		}
		CloseFile();
		ReOpen(true);
	}

	size_t numvarold = pg.Shape.NumVar;
	pg.Shape.NumVar = pg.Shape.NumVar + 1;
	size_t recsizeold = pg.Shape.RecordSize;
	size_t recindexsizeold = pg.Shape.RecordIndexSize;

	pg.Shape.RecordSize = pg.Shape.RecordSize + 4;
	pg.Shape.RecordIndexSize = pg.Shape.RecordSize / 4;

	auto m_rep_recsizeold = pg.Shape.RecordSize_Repetion;
	pg.Shape.RecordSize_Repetion = pg.Shape.NumRecords * pg.Shape.RecordIndexSize;
	dumpstream.seekg(recsizeold, ios::beg);
	if (!dumpstream.good()) {
		float kool;
		std::streamsize newpos;
		newpos = recsizeold * pg.status.b.buf_Size + recsizeold;
		dumpstream.seekg(newpos, ios::beg);
		dumpstream.read((char*)&kool, 4);

	}

	std::vector <float> vbuf;
	size_t ut,in;
for(size_t irep=0; irep<pg.Shape.NumRepetitions;irep++) {
	for(size_t nbuf=1; nbuf<=GetNumBuffers(); nbuf++) {
		if(!pg.status.CompleteFileRead) {
			size_t CurrentRecBuf;
			if(pg.Shape.NumRepetitions<=1) {
				if(nbuf<pg.status.b.NumberOfBuf) 
					CurrentRecBuf=pg.status.b.buf_Size;
				else
					CurrentRecBuf=(pg.Shape.NumRecords-1)%pg.status.b.buf_Size+1;
				std::streamsize newpos;
				newpos=recsizeold*(nbuf-1)*pg.status.b.buf_Size+recsizeold;
				dumpstream.seekg(newpos,ios::beg);
				dumpstream.read((char*)&pg.Var.v[0],recsizeold*CurrentRecBuf);
			}
			else {
				CurrentRecBuf=pg.status.b.buf_Size;
				std::streamsize newpos;
				newpos=recsizeold*irep*pg.status.b.buf_Size+recsizeold;
				dumpstream.seekg(newpos,ios::beg);
				dumpstream.read((char*)&pg.Var.v[0],recsizeold*CurrentRecBuf);
			}
			vbuf.resize((pg.Shape.NumVar+1)*CurrentRecBuf);
		}
		else {
			//pg.status.b.buf_Size=pg.Shape.NumRecords*pg.Shape.NumRepetitions;
			vbuf.resize((pg.Shape.NumVar+1)*pg.Shape.NumRecords*pg.Shape.NumRepetitions);
		}


		for(size_t i=0;i<GetCurrentRecords();i++) {
			if(pg.status.CompleteFileRead) {
				ut=i*pg.Shape.RecordIndexSize+irep*pg.Shape.RecordSize_Repetion;
				in=i*recindexsizeold+irep*m_rep_recsizeold;
			}
			else {
				ut=i*pg.Shape.RecordIndexSize;
				in=i*recindexsizeold;
			}
			vbuf[ut]=pg.Var.v[in];
			for(size_t ii=0;ii<varno;ii++) {
				vbuf[ut+ii+1]=pg.Var.v[in+ii+1];
			}
			vbuf[ut+varno+1]=MISSING;
			for (size_t ii=varno+1;ii<pg.Shape.NumVar;ii++) {
				vbuf[ut+ii+1]=pg.Var.v[in+ii];
			}
		}

		pg.Var.v.swap(vbuf);
		m_WriteEnabled = true;
		if(!pg.status.CompleteFileRead&&GetNumBuffers()>1) 
			WriteNewBuffer(nbuf);	
	}
	if(!pg.status.CompleteFileRead) 
		if(!WriteNewBuffer(irep+1)) return false;	
}

	if(!pg.status.CompleteFileRead) {
		dumpstream.close();
		remove(dumpfile.data());
	}

	vector<AddInfo>::iterator it;
	size_t varadd;
	for(size_t irep=0; irep<pg.Shape.NumRepetitions;irep++) {
		varadd=irep*pg.Shape.NumVar;
		pg.Des.base.var_max.insert(pg.Des.base.var_max.begin()+varno+varadd,-1.1E38f);
		pg.Des.base.var_min.insert(pg.Des.base.var_min.begin()+varno+varadd,1.1E38f);
		pg.Des.base.Name.insert(pg.Des.base.Name.begin()+varno+varadd,"New Inserted");
		pg.Des.base.Unit.insert(pg.Des.base.Unit.begin()+varno+varadd,"");
		pg.Des.base.Id.insert(pg.Des.base.Id.begin()+varno+varadd,"");
		pg.Des.base.Pos.insert(pg.Des.base.Pos.begin()+varno+varadd,"");
		pg.status.a.ToBeSaved.insert(pg.status.a.ToBeSaved.begin()+varno+varadd,1);
		pg.status.a.ToBeViewed.insert(pg.status.a.ToBeViewed.begin()+varno+varadd,1);
		pg.status.a.ToBeCalc.insert(pg.status.a.ToBeCalc.begin()+varno+varadd,0);
		it=pg.Des.addinfo.begin()+varno+varadd;
		pg.Des.addinfo.insert(it,pg.Des.defaultAddInfo);
	}
	
	WritePGFileEnd(pg.Shape.NumRecords, pg.Shape.NumRepetitions);

	return true;
}

bool CPG::RemoveFile()
{
	size_t res;
	res=remove(pg.status.FileName.data());
	if(res!=0)
		return false;
	return true;
}


bool CPG::AddNewRepititionDescriptions(size_t add)
{
	

	for(size_t i=0; i<pg.Shape.NumVar; i++) {
		if(add+i<pg.Des.base.Name.size()){
			pg.Des.base.Name[add+i]=pg.Des.base.Name[i];
			pg.Des.base.Unit[add+i]=pg.Des.base.Unit[i];
			pg.Des.base.Id[add+i]=pg.Des.base.Id[i];
			pg.Des.base.Pos[add+i]=pg.Des.base.Pos[i];
			pg.Des.base.var_min[add+i]=-1*MISSING;
			pg.Des.base.var_max[add+i]=MISSING;
			pg.Des.addinfo[add+i]=pg.Des.addinfo[i];
		}
		else if(pg.Des.base.Name.size()>=1) {
			pg.Des.base.Name.push_back(pg.Des.base.Name[pg.Des.base.Name.size()-1]);
			pg.Des.base.Unit.push_back(pg.Des.base.Unit[pg.Des.base.Unit.size()-1]);
			pg.Des.base.Id.push_back(pg.Des.base.Id[pg.Des.base.Id.size()-1]);
			pg.Des.base.Pos.push_back(pg.Des.base.Pos[pg.Des.base.Pos.size()-1]);
			pg.Des.base.var_min.push_back(pg.Des.base.var_min[pg.Des.base.var_min.size()-1]);
			pg.Des.base.var_max.push_back(pg.Des.base.var_max[pg.Des.base.var_max.size()-1]);
			pg.Des.addinfo.push_back(pg.Des.addinfo[pg.Des.addinfo.size()-1]);
		}
	}

	return true;
}

bool CPG::RemoveVariable(size_t varno)
{
	if(pg.status.b.buf_Size==pg.Shape.NumRecords) SetCompleteRead(true);
	if(!GetCompleteRead()) return false;
	size_t numvarold=pg.Shape.NumVar;
	pg.Shape.NumVar=pg.Shape.NumVar-1;
	size_t m_recsizeold=pg.Shape.RecordSize;
	size_t m_recindexsizeold=pg.Shape.RecordIndexSize;
	pg.Shape.RecordSize=pg.Shape.RecordSize-4;
	pg.Shape.RecordIndexSize=pg.Shape.RecordSize/4;

	auto m_rep_recsizeold=pg.Shape.RecordSize_Repetion;
	pg.Shape.RecordSize_Repetion=pg.Shape.NumRecords*pg.Shape.RecordIndexSize;

	varno=varno%numvarold;
	size_t first;
	first=varno;
	size_t si1=pg.Shape.RecordIndexSize*pg.Shape.NumRecords*pg.Shape.NumRepetitions;
	std::vector <float> vbuf;
	vbuf.resize(si1);
	size_t ut,in;
	for(size_t irep=0; irep<pg.Shape.NumRepetitions;irep++) {
		for (size_t i=0; i<pg.Shape.NumRecords;i++) {
			ut=i*(pg.Shape.NumVar+1)+irep*pg.Shape.RecordSize_Repetion;
			in=i*(numvarold+1)+irep*m_rep_recsizeold;
			for( size_t ii=0; ii<varno+1;ii++) {
				vbuf[ut+ii]=pg.Var.v[in+ii];
			}
			for(size_t ii=varno+2;ii<=numvarold;ii++) {
				vbuf[ut+ii-1]=pg.Var.v[in+ii];
			}
		}
	}

	pg.Var.v.resize(si1);

	pg.Var.v.swap(vbuf);
	si1=pg.Var.v.size();

	


	/// New space for data description part
	//float *pmin= new float;
	//float *pmax= new float;
	size_t varadd;
	for(size_t irep=0; irep<pg.Shape.NumRepetitions;irep++) {
		varadd=irep*pg.Shape.NumVar;
		pg.Des.base.var_max.erase(pg.Des.base.var_max.begin()+varno+varadd);
		pg.Des.base.var_min.erase(pg.Des.base.var_min.begin()+varno+varadd);
		pg.Des.base.Name.erase(pg.Des.base.Name.begin()+varno+varadd);
		pg.Des.base.Unit.erase(pg.Des.base.Unit.begin()+varno+varadd);
		pg.Des.base.Id.erase(pg.Des.base.Id.begin()+varno+varadd);
		pg.Des.base.Pos.erase(pg.Des.base.Pos.begin()+varno+varadd);
		pg.status.a.ToBeSaved.erase(pg.status.a.ToBeSaved.begin()+varno+varadd);
		pg.status.a.ToBeViewed.erase(pg.status.a.ToBeViewed.begin()+varno+varadd);
		pg.status.a.ToBeCalc.erase(pg.status.a.ToBeCalc.begin()+varno+varadd);
		if(pg.Des.addinfo.size()>varno+varadd) {
			for(size_t j=0; j<pg.Des.addinfo.size();j++) {
				pg.Des.addinfo[j].num_missing;
				size_t jj=pg.Des.addinfo[j].Key;
			}

			pg.Des.addinfo.erase(pg.Des.addinfo.begin()+varno+varadd);
		}
	}

	return true;
}
bool CPG::SetDataInFocus(size_t datatype)
{
	switch( datatype) {

		case 0:
			if(pg.Shape.NumRecords>0) {
				m_DataInFocus=0;
				return true;
			}
			break;
		case 1:
			if(m_numrecords_Years>0) {
				m_DataInFocus=1;
				return true;
			}
			break;
		case 2:
			if(m_numrecords_Months>0) {
				m_DataInFocus=2;
				return true;
			}
			break;

		case 3:
			if(m_numrecords_Days>0) {
				m_DataInFocus=3;
				return true;
			}
			break;
		case 4:
			if(m_numrecords_Hours>0) {
				m_DataInFocus=4;
				return true;
			}
			break;
		case 5:
			if(m_numrecords_Minutes>0){
				m_DataInFocus=5;
				return true;
			}
			break;



	}
	m_DataInFocus=0;
	return false;
}
void CPG::SetVarFunction(size_t expression)
{
	m_expression=expression;
}
void CPG::SetVarFunctionType(size_t expression_type)
{
	m_expression_type=expression_type;
}
void CPG::SetReplace(bool value)
{
	m_Replace=value;
	if(!value)
		m_CalcVarNew=m_CalcVar;
}
size_t CPG::GetVarFunction()
{
	return m_expression;

}
size_t CPG::GetVarFunctionType()
{
	return m_expression_type;
}
bool CPG::SetCommonPeriod(size_t start,size_t end)
{
	m_startRec=1;
	pg.status.a.OutRecordStart=GetRecord(start, false);
	if(pg.status.a.OutRecordStart<1||pg.status.a.OutRecordStart>pg.Shape.NumRecords) pg.status.a.OutRecordStart=1;
	pg.status.a.OutRecordEnd=GetRecord(end, false);
	if(pg.status.a.OutRecordEnd>pg.Shape.NumRecords||pg.status.a.OutRecordEnd<1) pg.status.a.OutRecordEnd=pg.Shape.NumRecords;
	if(pg.status.a.OutRecordStart>1||pg.status.a.OutRecordEnd<pg.Shape.NumRecords)
		return true;
	else
		return false;
}
bool CPG::SetCommonPeriod(string Start, string End)
{
	return SetCommonPeriod(PGUtil::MinutConv(Start),PGUtil::MinutConv(End));
}
bool CPG::GetOutFileStatus()
{
	return m_OutFileStatus;
}
string CPG::GetOutFileStart()
{
	return m_OutFileStart;
}
string CPG::GetOutFileEnd()
{
	return m_OutFileEnd;
}
size_t CPG::GetOutFileRecords()
{
	return m_OutFileRecord;
}
size_t CPG::GetOutFileVar()
{
	return m_OutFileVar;
}

void CPG::SetOnlyNoneMissingToBeSaved(bool value)
{
	pg.status.OnlyNoneMissing=value;
}
bool CPG::GetOnlyNoneMissingToBeSaved()
{
	return pg.status.OnlyNoneMissing;
}
void CPG::SetAllToBeSaved(bool value)
{
	for(size_t i=0;i<pg.status.a.ToBeSaved.size();i++) 
		pg.status.a.ToBeSaved[i]= value;
}
void CPG::SetToBeSaved(size_t i, bool value)
{
	if(i<pg.status.a.ToBeSaved.size()) 
		pg.status.a.ToBeSaved[i]= value;
}
void CPG::SetToBeSavedBelow(size_t index, bool value)
{
	for(size_t i=index;i<pg.status.a.ToBeSaved.size();i++)
		pg.status.a.ToBeSaved[i]=value;
}
bool CPG::GetToBeSaved(size_t i)
{
	if(i>=0&&i<pg.status.a.ToBeSaved.size()) 
		if(pg.status.a.ToBeSaved.at(i)==1)
			return true;
		else
			return false;
	else
		return true;
}
size_t CPG::GetNumToBeCalc()
{
	size_t count;
	count=0;
	for(size_t i=0;i<pg.status.a.ToBeCalc.size();i++) {
		if(pg.status.a.ToBeCalc.at(i)==1)
			count++;
	}
	return count;
}
void CPG::SetAllToBeCalc(bool value)
{
	for(size_t i=0;i<pg.status.a.ToBeCalc.size();i++) 
		pg.status.a.ToBeCalc[i]= value;
}
void CPG::SetToBeCalc(size_t i, bool value)
{
	if(i>=0&&i<pg.status.a.ToBeCalc.size()) 
		pg.status.a.ToBeCalc[i]= value;
}
bool CPG::GetToBeCalc(size_t i)
{
	if(i>=0&&i<pg.status.a.ToBeCalc.size()) 
		if(pg.status.a.ToBeCalc.at(i)==1)
			return true;
		else
			return false;
	else
		return true;
}
size_t CPG::GetColor(size_t index)
{
	if(index>=0&&index<m_Color.size())
		return size_t(m_Color[index]);
	else
#ifndef COUPSTD
		return RGB(255,255,255);
#else
		return 0;
	//return 0;
#endif
}
void CPG::SetColor(size_t index, size_t color)
{ size_t coldef;
#ifndef COUPSTD
		coldef=RGB(255,255,255);
#else
		coldef=0;
#endif
;

	if(index>=m_Color.size())
		m_Color.resize(index+1);
	m_Color[index]=color;
}
bool CPG::GetPicked(size_t index)
{
	if(index>=0&&index<m_Picked.size()) 
		return m_Picked[index];
	else
		return false;
}
bool CPG::GetPickedMean(size_t index)
{
	if(index>=0&&index<m_PickedMean.size()) 
		return m_PickedMean[index];
	else
		return false;
}
bool CPG::GetPickedMin(size_t index)
{
	if(index>=0&&index<m_PickedMin.size()) 
		return m_PickedMin[index];
	else
		return false;
}
bool CPG::GetPickedMax(size_t index)
{
	if(index>=0&&index<m_PickedMax.size()) 
		return m_PickedMax[index];
	else
		return false;
}
void CPG::SetPicked(size_t index, bool value)
{
	while (index>=m_Picked.size()) {
		m_Picked.push_back(false);
	}
	m_Picked[index]=value;

}
void CPG::SetPickedMean(size_t index, bool value)
{
	while (index>=m_PickedMean.size()) {
		m_PickedMean.push_back(false);
	}
	m_PickedMean[index]=value;

}
void CPG::SetPickedMin(size_t index, bool value)
{
	while (index>=m_PickedMin.size()) {
		m_PickedMin.push_back(false);
	}
	m_PickedMin[index]=value;

}
void CPG::SetPickedMax(size_t index, bool value)
{
	while (index>=m_PickedMax.size()) {
		m_PickedMax.push_back(false);
	}
	m_PickedMax[index]=value;

}


size_t CPG::GetRecordsInSection(size_t index)
{
	if(m_SectionYear)
		return GetRecordsInYear(index);
	else
		return GetRecordsInDay(index);
}
size_t CPG::GetIndexInSection(size_t index)
{
	if(m_SectionYear)
		return GetYearIndex(index);
	else
		return GetDayIndex(index);
}
size_t CPG::GetNumberofSections()
{
	if(m_SectionYear)
		return GetNumberofYears();
	else
		return GetNumberofDays();
}
size_t CPG::GetNumberofYears() {
	if(m_NumberOfYears==string::npos)
		if(CreateYearIndex())
			return m_NumberOfYears;	
return m_NumberOfYears;
}
bool CPG::CreateYearIndex() 
{
	vector<size_t> YearIndex, YearNumOfRec;
	size_t minthisyearst, minnextyearst;
	size_t index_st;
	size_t count;
	double StartFirstYear;
	minthisyearst=GetMinuteStartThisYear(1);
	StartFirstYear=GetDoubleTimeYearStart(1);
	minnextyearst=GetMinuteStartNextYear(1);
	index_st=1;
	for (size_t i=0; i<pg.Shape.NumRecords;i++) {
		if(GetLongTime(i+1)>=minnextyearst) {
			count=i+2-index_st;
			if(count>0) {
				YearIndex.push_back(index_st);
				YearNumOfRec.push_back(i+1-index_st);
				index_st=i+1;
				minnextyearst=GetMinuteStartNextYear(i+1);
			}
			else
				minnextyearst=GetMinuteStartNextYear(i+1);
		}
	}
	if(index_st<pg.Shape.NumRecords) {
		count=pg.Shape.NumRecords-index_st+1;
		if(count>0) {
			YearIndex.push_back(index_st);
			YearNumOfRec.push_back(pg.Shape.NumRecords+1-index_st);
		}
	}
	m_NumberOfYears=YearIndex.size();
	if(m_NumberOfYears!=string::npos) {
		m_YearIndexStart= new size_t[m_NumberOfYears];
		m_YearRecords= new size_t[m_NumberOfYears];
		m_YearShift= new double[m_NumberOfYears];
		for(size_t i=0; i<m_NumberOfYears;i++) {
			*(m_YearIndexStart+i)=YearIndex.at(i);
			*(m_YearRecords+i)=YearNumOfRec.at(i);
			*(m_YearShift+i)=GetDoubleTime(YearIndex.at(i))-StartFirstYear;
		}

	}

	return true;
}
size_t CPG::GetYearIndex(size_t index) 
{
	if(index<=m_NumberOfYears&&m_NumberOfYears!=string::npos)
		return *(m_YearIndexStart+index-1);
	return -1;

}
size_t CPG::GetRecordsInYear(size_t index)
{
	if(index<=m_NumberOfYears&&m_NumberOfYears != string::npos)
		return *(m_YearRecords+index-1);
	return -1;

}

size_t CPG::GetNumberofDays() {
	if(m_NumberOfDays<0)
		if(CreateDayIndex())
			return m_NumberOfDays;	
return m_NumberOfDays;
}
bool CPG::CreateDayIndex() 
{
	vector<size_t> DayIndex, DayNumOfRec;
	size_t minnextdayst;
	size_t index_st;
	size_t count;
	minnextdayst=GetMinuteStartNextDay(1);
	index_st=1;
	for (size_t i=0; i<pg.Shape.NumRecords;i++) {
		if(GetLongTime(i+1)>=minnextdayst) {
			count=i+2-index_st;
			if(count>0) {
				DayIndex.push_back(index_st);
				DayNumOfRec.push_back(i+2-index_st);
				index_st=i+1;
				minnextdayst=GetMinuteStartNextDay(i+1);
			}
			else
				minnextdayst=GetMinuteStartNextDay(i+1);
		}
	}
	if(index_st<pg.Shape.NumRecords) {
		count=pg.Shape.NumRecords-index_st+1;
		if(count>0) {
			DayIndex.push_back(index_st);
			DayNumOfRec.push_back(pg.Shape.NumRecords+1-index_st);
		}
	}
	m_NumberOfDays=DayIndex.size();
	if(m_NumberOfDays>0) {
		m_DayIndexStart= new size_t[m_NumberOfDays];
		m_DayRecords= new size_t[m_NumberOfDays];
		m_DayShift= new double[m_NumberOfDays];
	}

	return true;
}
size_t CPG::GetDayIndex(size_t index) 
{
	if(index<m_NumberOfDays)
		return *m_DayIndexStart;
	return -1;

}
size_t CPG::GetRecordsInDay(size_t index)
{
	if(index<m_NumberOfDays)
		return *m_DayRecords;
	return -1;

}
void CPG::SetYearSectionStyle(bool value)
{
	m_SectionYear=value;
}
void CPG::SetSectionNumber(size_t value)
{
	m_ActualSection=value;
}
double CPG::GetDoubleShiftTime(size_t section)
{
	if(section<=1)
		return 0.;
	if(m_SectionYear&&m_YearShift!=nullptr)
		return *(m_YearShift+section-1);
	else if(m_DayShift!=nullptr)
		return *(m_DayShift+section-1);
	else
		return 0;
}

size_t CPG::GetNumberOfCommonPoints()
{   size_t year;
	year=366*1440;
	if(m_EquiDistance>0&&m_SectionYear)
		return year/m_EquiDistance;
	else if(m_EquiDistance>0)
		return 1440/m_EquiDistance;
	else
		return 0;
}

bool CPG::EstimateWithinDayStatistics() 
{	float *sum,*sum_min,*sum_max,*sum2;
	// Estimate within day statistics for entire data set
	// and monthly average of within day statistics 
	size_t num_within;
	size_t num_days;
	size_t index;
	size_t imm;
	size_t im;
	float value;
	size_t ncount;
	float sumv, sum2v, min, max;

	if(IsEquiDistance()) {
		if(v_WithinDay.size()>0) 
			return true;
		if(m_EquiDistance>360)
			return false;
		num_within=1440/m_EquiDistance;
		// Allocation of space
		sum=new float[num_within*pg.Shape.NumVar*13];
		sum_min=new float[num_within*pg.Shape.NumVar*13];
		sum_max=new float[num_within*pg.Shape.NumVar*13];
		sum2=new float[num_within*pg.Shape.NumVar*13];

		m_NumWithinDay=num_within;
		
		v_WithinDay.resize(num_within*pg.Shape.NumVar*13);

		imm=num_within*pg.Shape.NumVar;
		m_numWithinDayTot=imm;
		// Set Start values
		for(size_t i=0;i<num_within;i++) {
			for(size_t ii=0; ii<pg.Shape.NumVar;ii++) {
				for(size_t im=0;im<13;im++) {
					index=i+ii*num_within+im*imm;
					v_WithinDay[index].NumData=0;
					*(sum+index)=0.;
					*(sum2+index)=0.;
					*(sum_min+index)=abs(MISSING);
					*(sum_max+index)=MISSING;
				}
			}
		}

		num_days=GetNumberofDays();
		size_t ist=0;
		size_t iw;
	// Calculating mean values
		for(size_t i=ist;i<pg.Shape.NumRecords;i++) {
			iw=i%num_within;
			im=GetMonth(i+1)-1;
			for(size_t ii=0; ii<pg.Shape.NumVar;ii++) {
				value=GetVarValue(ii+1,i+1);
				index=iw+ii*num_within+im*imm;
				if(abs(value)<1.E37) {
						*(sum+index)+=value;
						*(sum2+index)+=value*value;
						v_WithinDay[index].NumData++;
						if(value<*(sum_min+index)) *(sum_min+index)=value;
						if(value>*(sum_max+index)) *(sum_max+index)=value;
						index=index=iw+ii*num_within+12*imm; /// Year mean
						*(sum+index)+=value;
						*(sum2+index)+=value*value;
						v_WithinDay[index].NumData++;
						if(value<*(sum_min+index)) *(sum_min+index)=value;
						if(value>*(sum_max+index)) *(sum_max+index)=value;
				}
			}
		}
	// Calculating Quantiles

	// Calculating Outputs	
		for(size_t i=0; i<num_within; i++) {
			for(size_t ii=0; ii<pg.Shape.NumVar;ii++) {
					for (size_t im=0;im<13;im++) {
						index=i+ii*num_within+im*imm;
						ncount=v_WithinDay[index].NumData;
						sumv=*(sum+index);
						sum2v=*(sum2+index);
						min=*(sum_min+index);
						max=*(sum_max+index);
						if(ncount>2) {
							float mean=sumv/ncount;
							v_WithinDay[index].Mean=mean;
							v_WithinDay[index].Max=*(sum_max+index);
							v_WithinDay[index].Min=*(sum_min+index);
							v_WithinDay[index].Std=sqrt((sum2v-ncount*sumv*sumv)/(ncount-1));					
						}
						else {
							v_WithinDay[index].Mean=MISSING;
							v_WithinDay[index].Max=MISSING;
							v_WithinDay[index].Min=MISSING;
							v_WithinDay[index].Std=MISSING;		
						}

					}

			}
		}
		delete []sum;
		delete []sum_min;
		delete []sum_max;
		delete []sum2;
	}
	else 
		return false;
	return true;
}
void CPG::EstimateWithinYearClear(size_t rep)
{
  v_WithinYear.clear();

}
bool CPG::EstimateWithinYearStatistics(Doc *pDoc)
{
	if(v_WithinYear.size()>0) return true;
	size_t num_within, check;
	size_t n=GetNumberofYears();
	float value;

	size_t nmult;

	Doc *pDocAct=nullptr;
#ifndef COUPSTD
	CFileScanner ScanMeanProg(nullptr);
	ScanMeanProg.Create(IDD_PROGRESS,nullptr);
	ScanMeanProg.ShowWindow(SW_SHOW);
	ScanMeanProg.SetWindowText(_T("Estimating statistics for within year variability"));
#endif COUPSTD

	//CheckEquiDistance();
	if(pDoc!=nullptr) {
		m_WithinYearForResiduals=true;
		nmult=2;
		pDocAct=dynamic_cast<Doc*>(pDoc);
	}
	else {
		m_WithinYearForResiduals=false;
		nmult=1;
	}


	if(n>1&&IsEquiDistance()){
		num_within=GetRecordsInYear(1);
		for(size_t i=2;i<=n;i++) {
			check=GetRecordsInYear(i);						
			if(check>num_within) num_within=check;
		}

		// Allocation of space
		vector<vector<float> > MDF(num_within*pg.Shape.NumVar*nmult);
		v_WithinYear.resize(num_within*pg.Shape.NumVar*nmult);		
		m_NumWithinYear=num_within;
		m_withinYearAllocated=num_within*pg.Shape.NumVar;
		// Set Start values

		size_t index;
		for(size_t i1=0;i1<nmult; i1++) {
			for(size_t i=0;i<num_within;i++) {
				for(size_t ii=0; ii<pg.Shape.NumVar;ii++) {
					index=i+ii*num_within+i1*(num_within*pg.Shape.NumVar);
					v_WithinYear[index].NumData=0;
				}
			}
		}
		size_t shift=0;

		// Summation of all values
		for(size_t j=0;j<n;j++) {
			size_t start, end,  start_koll;
			start=GetYearIndex(j+1);
			end=start+GetRecordsInYear(j+1);
			if(j==0) {
				start_koll=1;//end-num_within;
				shift=num_within-end;

			}
			else {
				start_koll=start-1;
				shift=0;
			}
			for(size_t i1=0;i1<nmult; i1++) {
#ifndef COUPSTD
				//VALIDATION_mp_ISCAN=;

				ScanMeanProg.Update((j+i1/nmult)*100/(n*nmult));
#endif


				for(size_t i=start; i<end; i++) {

				for(size_t ii=0; ii<pg.Shape.NumVar;ii++) {
					if(i1==1) {
#ifndef COUPSTD
						size_t FileNo;
						FileNo=pDocAct->m_ValidationData.GetValFileNumber(ii);

						if(pDocAct->MR_XBin_IsOpen(FileNo)) {
							CResiduals *pRes=(CResiduals*)pDocAct->GetValidResidualPointer(FileNo);
							if(pRes!=nullptr) {
								size_t IndexVal=pDocAct->m_ValidationData.GetValFileIndex(ii);
								size_t ResRunNo=0;
								value=-pRes->GetResiduals(ResRunNo,IndexVal,i-1);
							}
						}
#endif				
					}
					else 
						value=GetVarValue(ii+1,i+1);
					if(abs(value)<1.E37) {
						index=i+ii*num_within+i1*(num_within*pg.Shape.NumVar)-start+shift;
						if(index<num_within*pg.Shape.NumVar) {
							MDF[index].push_back(value);
							v_WithinYear[index].NumData++;
						}
						else {
							continue;
						}
					}
				}
			}
		}
		}

	// Calculating Output	
		size_t n_count;
		DistStat stat;
		for(size_t i1=0;i1<nmult;i1++) {
			for(size_t i=0; i<num_within; i++) {
				for(size_t ii=0; ii<pg.Shape.NumVar;ii++) {
					index=i+ii*num_within+i1*(num_within*pg.Shape.NumVar);
					if(index<num_within*pg.Shape.NumVar) {
						if(MDF[index].size()>0) {
							StatUtil::SortArray(&MDF[index][0],MDF[index].size());
							MeanStat(&MDF[index][0],MDF[index].size(), &n_count, &stat);
							if(n_count>0) {
								v_WithinYear[index]=stat;
							}
						}
						else {
							size_t koll=MDF[index].size();

						}
					}
				}
			}
		}
		
		return true;
	}
	else 
		return false;
}

bool CPG::EstimateMultiStatistics(bool all, vector<size_t> *acceptedruns)
{
	if(!pg.Shape.RepetitionMode) return false;
	if(all&&m_UpdatedMultiAll) return true;
	if(!all&&m_UpdatedMultiAccepted) return true;
	size_t size;
	size=pg.Shape.NumVar*pg.Shape.NumRecords;


	if(all) {

		v_all.resize(size);
	}
	else {
		v_accepted.resize(size);
	}

	

	size_t rep;
	size_t n_count; // number of valid points within the ensemble
	for(size_t rec=0;rec<pg.Shape.NumRecords;rec++) {
		size_t i;
		if(all) {
			rep=0;
			i=0;
		}
		else {
			rep=acceptedruns->at(0);
			i=0;
		}
		n_count=0;
		vector<vector<float> > MDF(pg.Shape.NumVar);
		DistStat stat;

		for(size_t i=0; i<pg.Shape.NumVar; i++) {
			if(all)
				MDF[i].resize(pg.Shape.NumRepetitions);
			else
				MDF[i].resize(acceptedruns->size()) ;
		}
		
		while(rep<pg.Shape.NumRepetitions) {
			std::streamsize newpos=pg.Shape.RecordSize*(rec+1+rep*pg.Shape.NumRecords);
			m_MainPGStreamReadWrite.seekg(newpos,ios::beg);
			if(!m_MainPGStreamReadWrite.good()) {
				ReOpen();
				m_MainPGStreamReadWrite.seekg(newpos,ios::beg);
				if(!m_MainPGStreamReadWrite.good()) return false;

			}
			newpos=m_MainPGStreamReadWrite.tellg();
			m_MainPGStreamReadWrite.read((char*)&pg.Var.v[0],pg.Shape.RecordSize);		

			for(size_t n=0;n<pg.Shape.NumVar;n++) {
				MDF[n][n_count]=pg.Var.v[n+1];
			}
			i++;
			n_count++;
			if(!all) {
				if(i>=acceptedruns->size())
					rep=pg.Shape.NumRepetitions;
				else
					rep=acceptedruns->at(i);
			}
			else
				rep=i;
		}
		size_t ncheck;
		for(size_t n=0;n<pg.Shape.NumVar;n++) {		
			if(all) {
				StatUtil::SortArray(&MDF[n][0],pg.Shape.NumRepetitions );
				MeanStat(&MDF[n][0], pg.Shape.NumRepetitions, &ncheck, &stat);
				v_all[n+rec*pg.Shape.NumVar]=stat;
			}
			else {
				StatUtil::SortArray(&MDF[n][0],n_count );
				MeanStat(&MDF[n][0], n_count, &ncheck, &stat);
				v_accepted[n+rec*pg.Shape.NumVar]=stat;
			}
		}
#ifndef COUPSTD
		string txt;
		txt="Estimating Ensemble Statistics : ";
		txt+=MFC_Util::ItoAscii(rec)+" (";
		txt+=MFC_Util::ItoAscii(pg.Shape.NumRecords)+")";

		MySingle::instance().GetCugPointer()->UpdateStatusBar(txt);

#endif
	}
	if(all)
		m_UpdatedMultiAll=true;
	else
		m_UpdatedMultiAccepted=true;
	//CloseFile();
	//ReOpen();
	CloseFile();

	return true;

}
float CPG::GetVarValue_WithinYearMean(size_t varno, size_t recno, bool res)
{
	size_t index=(varno-1)*m_NumWithinYear+recno-1;
	if(index<v_WithinYear.size()&&m_NumWithinYear>0)
		return v_WithinYear[index].Mean;
	else
		return MISSING;
}
float CPG::GetVarValue_WithinYearSD(size_t varno, size_t recno, bool res)
{
		size_t index=(varno-1)*m_NumWithinYear+recno-1;
	if(index<v_WithinYear.size()&&m_NumWithinYear>0)
		return v_WithinYear[index].Std;
	else
		return MISSING;
}
float CPG::GetVarValue_WithinYearMax(size_t varno, size_t recno, bool res)
{
		size_t index=(varno-1)*m_NumWithinYear+recno-1;
	if(index<v_WithinYear.size()&&m_NumWithinYear>0)
		return v_WithinYear[index].Max;
	else
		return MISSING;
}
float CPG::GetVarValue_WithinYearMin(size_t varno, size_t recno, bool res)
{
	size_t index=(varno-1)*m_NumWithinYear+recno-1;
	if(index<v_WithinYear.size()&&m_NumWithinYear>0)
		return v_WithinYear[index].Min;
	else
		return MISSING;
}
float CPG::GetVarValue_WithinYearP5(size_t varno, size_t recno, bool res)
{
	size_t index=(varno-1)*m_NumWithinYear+recno-1;
	if(index<v_WithinYear.size()&&m_NumWithinYear>0)
		return v_WithinYear[index].P5;
	else
		return MISSING;
}
float CPG::GetVarValue_WithinYearP50(size_t varno, size_t recno, bool res)
{
	size_t index=(varno-1)*m_NumWithinYear+recno-1;
	if(index<v_WithinYear.size()&&m_NumWithinYear>0)
		return v_WithinYear[index].P50;
	else
		return MISSING;
}
float CPG::GetVarValue_WithinYearP95(size_t varno, size_t recno, bool res)
{
	size_t index=(varno-1)*m_NumWithinYear+recno-1;
	if(index<v_WithinYear.size()&&m_NumWithinYear>0)
		return v_WithinYear[index].P95;
	else
		return MISSING;
}

float CPG::GetVarValue_MultiMean(size_t varno, size_t var_rec, bool all)
{
	if(v_accepted.size()>0&&!all)
		return v_accepted[(var_rec-1)*pg.Shape.NumVar+varno-1].Mean;
	else if(v_all.size()>0&&all)
		return v_all[(var_rec-1)*pg.Shape.NumVar+varno-1].Mean;
	else
		return MISSING;
}
float CPG::GetVarValue_MultiSD(size_t varno, size_t var_rec, bool all)
{
	if(v_accepted.size()>0&&!all)
		return v_accepted[(var_rec-1)*pg.Shape.NumVar+varno-1].Std;
	else if(v_all.size()>0&&all)
		return v_all[(var_rec-1)*pg.Shape.NumVar+varno-1].Std;
	else
		return MISSING;
}
float CPG::GetVarValue_MultiMax(size_t varno, size_t var_rec, bool all)
{
	if(v_accepted.size()>0&&!all)
		return v_accepted[(var_rec-1)*pg.Shape.NumVar+varno-1].Max;
	else if(v_all.size()>0&&all)
		return v_all[(var_rec-1)*pg.Shape.NumVar+varno-1].Max;
	else
		return MISSING;
}
float CPG::GetVarValue_MultiMin(size_t varno, size_t var_rec, bool all)
{
	if(v_accepted.size()>0&&!all)
		return v_accepted[(var_rec-1)*pg.Shape.NumVar+varno-1].Min;
	else if(v_all.size()>0&&all)
		return v_all[(var_rec-1)*pg.Shape.NumVar+varno-1].Min;
	else
		return MISSING;
}
float CPG::GetVarValue_MultiP5(size_t varno, size_t var_rec, bool all)
{
	if(v_accepted.size()>0&&!all)
		return v_accepted[(var_rec-1)*pg.Shape.NumVar+varno-1].P5;
	else if(v_all.size()>0&&all)
		return v_all[(var_rec-1)*pg.Shape.NumVar+varno-1].P5;
	else
		return MISSING;
}
float CPG::GetVarValue_MultiP50(size_t varno, size_t var_rec, bool all)
{
	if(v_accepted.size()>0&&!all)
		return v_accepted[(var_rec-1)*pg.Shape.NumVar+varno-1].P50;
	else if(v_all.size()>0&&all)
		return v_all[(var_rec-1)*pg.Shape.NumVar+varno-1].P50;
	else
		return MISSING;
}
float CPG::GetVarValue_MultiP95(size_t varno, size_t var_rec, bool all)
{
	if(v_accepted.size()>0&&!all)
		return v_accepted[(var_rec-1)*pg.Shape.NumVar+varno-1].P95;
	else if(v_all.size()>0&&all)
		return v_all[(var_rec-1)*pg.Shape.NumVar+varno-1].P95;
	else
		return MISSING;
}
float CPG::GetVarValue_WithinDayMean(size_t varno, size_t recno, size_t im)
{ 
	size_t index=(varno-1)*m_NumWithinDay+im*m_numWithinDayTot+recno-1;
	if(index<v_WithinDay.size())
		return v_WithinDay[(varno-1)*m_NumWithinDay+im*m_numWithinDayTot+recno-1].Mean;
	else
		return MISSING;
}
float CPG::GetVarValue_WithinDaySD(size_t varno, size_t recno, size_t im)
{
	size_t index=(varno-1)*m_NumWithinDay+im*m_numWithinDayTot+recno-1;
	if(index<v_WithinDay.size())
		return v_WithinDay[(varno-1)*m_NumWithinDay+im*m_numWithinDayTot+recno-1].Std;
	else
		return MISSING;
}
float CPG::GetVarValue_WithinDayMax(size_t varno, size_t recno, size_t im)
{
	size_t index=(varno-1)*m_NumWithinDay+im*m_numWithinDayTot+recno-1;
	if(index<v_WithinDay.size())
		return v_WithinDay[(varno-1)*m_NumWithinDay+im*m_numWithinDayTot+recno-1].Max;
	else
		return MISSING;
}
float CPG::GetVarValue_WithinDayMin(size_t varno, size_t recno, size_t im)
{
	size_t index=(varno-1)*m_NumWithinDay+im*m_numWithinDayTot+recno-1;
	if(index<v_WithinDay.size())
		return v_WithinDay[(varno-1)*m_NumWithinDay+im*m_numWithinDayTot+recno-1].Min;
	else
		return MISSING;
}
bool CPG::GetReplaceMode()
{
	return m_Replace;
}

bool CPG::CheckConditions(size_t var_rec)
{
	bool Good;
	float LeftValue, RightValue;

	for (size_t i = 0; i < m_IfVector.size(); i++) {
		if (m_IfVector[i].LeftVariable != string::npos&&m_IfVector[i].LeftVariable <= pg.Shape.NumVar)
			LeftValue = pg.Var.v[(var_rec - 1)*pg.Shape.RecordIndexSize + m_IfVector[i].LeftVariable];
		else if (m_IfVector[i].Function > NO_USE_FUNC&&m_IfVector[i].LeftTimeFuncPos) {
			if (m_IfVector[i].Function == DAYNUMBERFUNC)
				LeftValue = float(GetDayNumber(var_rec));
			else if (m_IfVector[i].Function == MINUTENUMBERFUNC)
				LeftValue = float(GetMinuteNumber(var_rec));
		}
		else
			LeftValue = m_IfVector[i].LeftValue;

		if (m_IfVector[i].RightVariable != string::npos&&m_IfVector[i].RightVariable <= pg.Shape.NumVar)
			RightValue = pg.Var.v[(var_rec - 1)*pg.Shape.RecordIndexSize + m_IfVector[i].RightVariable];
		else if (m_IfVector[i].Function > NO_USE_FUNC&&!m_IfVector[i].LeftTimeFuncPos) {
			if (m_IfVector[i].Function == DAYNUMBERFUNC)
				RightValue = float(GetDayNumber(var_rec));
			else if (m_IfVector[i].Function == MINUTENUMBERFUNC)
				RightValue =float( GetMinuteNumber(var_rec));
		}
		else
			RightValue = m_IfVector[i].RightValue;

		Good = false;
		switch (m_IfVector[i].comp_method) {
			case SMALLER:if (LeftValue < RightValue) Good = true; break;
			case SMALLER_EQUAL:if (LeftValue <= RightValue) Good = true; break;
			case EQUAL:if (LeftValue == RightValue) Good = true; break;
			case GREATER_EQUAL:if (LeftValue >= RightValue) Good = true; break;
			case GREATER:if (LeftValue > RightValue) Good = true; break;
		}
		if (!Good) return false;
	}
	return true;
}

void CPG::AddVariableVector(vector<float> *Var, size_t varno)
{
	size_t n=Var->size();

	for(size_t i=0;i<n;i++) {
		pg.Var.v[i*pg.Shape.RecordIndexSize+varno+1]=Var->at(i);
	}
}
#ifndef COUPSTD
void CPG::AddTimeVector(DoubleArray Time)
{
	size_t n=Time.len;
	double vnew;
	for(size_t i=0;i<n;i++) {
		vnew=Time[i]/double(60);
		*(size_t*)&pg.Var.v[i*pg.Shape.RecordIndexSize]=size_t(vnew)+m_time1900_Zero;

	}
}
void CPG::AddDummySequenceTimeVector()
{
	size_t n =pg.Var.v.size()/pg.Shape.RecordIndexSize;
	double vnew;
	for (size_t i = 0; i<n; i++) {
		vnew = i * 1440.;
		*(size_t*)&pg.Var.v[i*pg.Shape.RecordIndexSize] = size_t(vnew) + m_time1900_Zero;
	}
}
void CPG::AddDummyTimeVector(DoubleArray Time)
{
	size_t n=Time.len;
	double vnew;
	for(size_t i=0;i<n;i++) {
		vnew=Time[i]*1440.;
		*(size_t*)&pg.Var.v[i*pg.Shape.RecordIndexSize]=size_t(vnew)+m_time1900_Zero;

	}
}
void CPG::AddVariableVector(DoubleArray Variable, size_t varno)
{
	size_t n=Variable.len;
	if (n * pg.Shape.RecordIndexSize >= pg.Var.v.size()) {
		pg.Var.v.resize(n * pg.Shape.RecordIndexSize);
	}
	for(size_t i=0;i<n;i++) {
		if(Variable[i]!=Chart::NoValue)
			pg.Var.v[i*pg.Shape.RecordIndexSize+varno]=float(Variable[i]);
		else
			pg.Var.v[i*pg.Shape.RecordIndexSize+varno]=MISSING;
	}
}
#endif
size_t CPG::GetNumberofVariablesWithMissings()
{
	if(!m_MissingTested)
		if(!CheckMissingNumbers())
			return -1;
	size_t countall;
	countall=0;

	for(size_t nrep=0; nrep<pg.Shape.NumRepetitions;nrep++) {
		for (size_t i=0;i<pg.Shape.NumVar;i++) {
				if(pg.Des.var_missing[i+nrep*pg.Shape.NumVar]>0)
					countall++;
		}	
	}

	return countall;
}
size_t CPG::GetNumberofMissing(size_t varno, size_t repit)
{	if(!m_MissingTested)
		if(!CheckMissingNumbers())
			return -1;
	size_t countall;
	size_t index=varno-1+(repit-1)*pg.Shape.NumVar;
	countall=0;
	if(varno<=0) {
	for(size_t nrep=0; nrep<pg.Shape.NumRepetitions;nrep++) {
		for (size_t i=0;i<pg.Shape.NumVar;i++) {
			if(pg.Des.var_missing[i+nrep*pg.Shape.NumVar]>0)
				countall+=pg.Des.var_missing[i+nrep*pg.Shape.NumVar];
		}
	}
		return countall;
	}
	else if(index<pg.Des.var_missing.size()) {
	
		return pg.Des.var_missing[index];

	}
	else
		return -1;

}
size_t CPG::GetVarKey(size_t index, size_t repit)
{
	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(pg.Des.addinfo.size()>=index)
		return pg.Des.addinfo[index-1].Key;
	else
		return 0;
}
double CPG::GetVarLat(size_t index, size_t repit)
{
	size_t add=0;
	if(index==0) index=1;
	if(repit==0) repit=1;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(pg.Des.addinfo.size()>=index&&index>0)
		return pg.Des.addinfo[index-1].Lat;
	else
		return double(MISSING);

}
double CPG::GetVarLong(size_t index, size_t repit)
{
	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(pg.Des.addinfo.size()>=index&&index>0)
		return pg.Des.addinfo[index-1].Long;
	else
		return double(MISSING);

}
double CPG::GetVarAlt(size_t index, size_t repit)
{
	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(pg.Des.addinfo.size()>=index&&index>0)
		return pg.Des.addinfo[index-1].Alt;
	else
		return double(MISSING);

}
string CPG::GetVarCountry(size_t index, size_t repit)
{
	string txt;
	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(pg.Des.addinfo.size()>=index) {
		return pg.Des.addinfo[index-1].CountryCode;
	}
	else
		return "";

}
string CPG::GetVarStation(size_t index, size_t repit)
{
	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(pg.Des.addinfo.size()>=index)
		return pg.Des.addinfo[index-1].StationName;
	else
		return "";

}
elements CPG::GetVarElement(size_t index, size_t repit)
{
	size_t add = 0;
	add = (repit - 1)*pg.Shape.NumVar;
	index += add;
	if (pg.Des.addinfo.size() >= index)
		return pg.Des.addinfo[index - 1].Element;
	else
		return GENERAL;

}
UNIT_TYPES CPG::GetVarUnitType(size_t index, size_t repit)
{
	size_t add = 0;
	add = (repit - 1)*pg.Shape.NumVar;
	index += add;
	if (pg.Des.addinfo.size() >= index)
		return pg.Des.addinfo[index - 1].Unit;
	else
		return UNIT_TYPES::NO_UNIT;

}

size_t CPG::GetVarMissing(size_t index, size_t repit)
{
	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(pg.Des.addinfo.size()>=index)
		return pg.Des.addinfo[index-1].num_missing;
	else
		return 0;
}
size_t CPG::GetVarSubstituted(size_t index, size_t repit)
{
	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(pg.Des.addinfo.size()>=index)
		return pg.Des.addinfo[index-1].num_substituted;
	else
		return 0;
}
