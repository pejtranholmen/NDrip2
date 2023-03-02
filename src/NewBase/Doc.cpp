
#include "./Doc.h"
#include <ctime>
#ifndef COUPSTD
#include "../CoupModel.h"
#include "../Chart/ChartProperties/ChartProp.h"
#endif
#include "../Util/FUtil.hpp"

//#include "..\Base\ModelType.h"


Doc::Doc() {

	m_Monitor1View=0;
	m_Monitor2View=0;
#ifndef COUPSTD
	m_Link.m_Map=this;
#endif
	m_PrevSheet=0;
	m_NewChart=false;
	m_NumValidValFiles=-1;
	m_GroupEnabled.resize(50);
	m_pModelView=nullptr;
	m_CurrentSheet=0;
	m_SelectedDocuments.clear();
	m_ActualDocChartType = CHART_TYPES::TIME_SERIE;

}
Doc::~Doc() {


}


string Doc::GetMultiBinFileName() 
{

	string strApp=m_AppString;
	string UserDirectory, numstring;
#ifndef COUPSTD
	UserDirectory=MFC_Util::GetProfileStringStd("UserDirectory", UserDirectory);
#else
	UserDirectory="";
#endif
	int numo=UserDirectory.size();
	if(UserDirectory.rfind('\\')!=numo-1) 
			UserDirectory+="\\";

	string NewFolder=GetCurrentSimFile();//CDocument::GetTitle();
	int ipp=NewFolder.find('_');
	int ipm=NewFolder.find("_Multi");

	string orignumfromfile;
	if (ipm > 0) {
		ipp = ipm;
		orignumfromfile = NewFolder.substr(ipm + 7, 6);

	}
	UserDirectory=NewFolder.substr(0,ipp);

	int StartNo=m_DocFile.m_SimulationRunNo-MR_Get_ActualRunNumber()+1;
		
	m_Multi_Bin_FileName=UserDirectory+"\\Multi_Coup_";

	numstring = FUtil::ItoNumAscii(StartNo);
	m_Multi_Bin_FileName+=numstring+".Bin";
	m_OutputBinFileExist=FUtil::IsFileExisting(m_Multi_Bin_FileName);
	if (!m_OutputBinFileExist) {
		m_Multi_Bin_FileName = UserDirectory + "\\Multi_Coup_"+orignumfromfile+".Bin";
		m_OutputBinFileExist = FUtil::IsFileExisting(m_Multi_Bin_FileName);
/*		auto posnolla = numstring.find("0");
		if (posnolla == 0) {
			numstring = numstring.substr(1, 5);
			m_Multi_Bin_FileName = UserDirectory + "\\Multi_Coup_";
			m_Multi_Bin_FileName += numstring + ".Bin";
			m_OutputBinFileExist = FUtil::IsFileExisting(m_Multi_Bin_FileName);
		}*/
	}

	return m_Multi_Bin_FileName;

}
bool Doc::IsOutputBinFileValid()
{	if(IsMultiSimulation()) m_OutputBinFileExist = FUtil::IsFileExisting(m_Multi_Bin_FileName);
	return m_OutputBinFileExist;
}
void Doc::SetOutputBinFileMissing()
{
	m_OutputBinFileExist=false;
}
void Doc::SetOutputBinFileExist()
{
	m_OutputBinFileExist=true;
}
bool Doc::IsOutputValidBinFileValid()
{
	if(m_OutputValidBinFileExist||(m_OutputValidXBinFileExist&&IsMultiSimulation()))
		return true;
	else
		return false;
}
void Doc::SetOutputValidBinFileMissing()
{
	m_OutputValidBinFileExist=false;
}

int Doc::Get_TotalNumResidualTimeRecords()
{
	int num;
	
	string name;
	num=0;
#ifndef COUPSTD
	CPG* pPG1;
	for(int i=1; i<9;i++) {
		if(MR_XBin_IsOpen(i)) {
			pPG1=m_PlotStorage.GetPGPointer(0,i);
			name=pPG1->GetFileName();
			if(pPG1->Open(name.c_str())) {

				num+=pPG1->GetNumRecords();
				pPG1->CloseFile();
			}
		}


	}
#endif

	return num;
}

CResiduals* Doc::GetResidualPointer(size_t ifile, int addedfiles)
{
	if(!MR_XBin_IsOpen(ifile, addedfiles))
		return nullptr;
	else if (ifile <= m_ResidualFileVector.size()) {
		if (addedfiles < m_ResidualFileVector[ifile - 1].size())
			return m_ResidualFileVector[ifile - 1][addedfiles];
		else
			return nullptr;
	}
	else 
		return nullptr;

}
bool Doc::OpenOutputFile(int ValidIndex)
{
	string filename;
	string stdbinfilename;
	string dir=GetCurrentWorkDirectory();

#ifndef STD
	dir=FUtil::GetProfileStringStd("UserDirectory", m_UserDirectory);
#else
	dir="";
#endif
	
	string str;
		
			filename=GetOutputFileName(ValidIndex);

	
			//if(m_PlotStorage.m_PG->Open(filename.c_str(),false))
			//	return true;

			str=FUtil::STD_ItoAscii(m_DocFile.m_SimulationRunNo);
			if(str.size()==5) 
				stdbinfilename="COUP_"+str+".Bin";
			if(str.size()==4) 
				stdbinfilename="COUP_"+str+".Bin";
			else if(str.size()==3)
				stdbinfilename="COUP_"+str+".Bin";
			else if(str.size()==2)
				stdbinfilename="COUP_0"+str+".Bin";
			else if(str.size()==1)
				stdbinfilename="COUP_00"+str+".Bin";
			else
				stdbinfilename="COUP_"+str+".Bin";

			if(ValidIndex>0) {
				filename=dir+"V"+FUtil::STD_ItoAscii(ValidIndex)+"_"+stdbinfilename;		
		//		if(m_PlotStorage.m_PGVal->Open(filename.c_str(),false))
				return true;
			}
			else {
				filename=dir+stdbinfilename;
	
	//			if(m_PlotStorage.m_PG->Open(filename.c_str(),false))
				return true;
			}
			return false;
}


void Doc::OutputBinFileCheck()
{	Sw* pSw;	
	string name="Time Serie Outputs";
	string grp="Technical";
	string typ="Switches";

    pSw=(Sw*)GetPtr(typ, grp, name);
	if(pSw!=nullptr) {
		//int i=((CSw*)Sw)->GetValue();
		int i=pSw->GetIntValue();
		m_OutputBinFileExist=true;
		if(i>0&&IsMultiSimulation())
			m_OutputBinFileExist=false;
	}
	name="Validation Outputs";
    pSw=(Sw*)GetPtr(typ, grp, name);
	if(pSw!=nullptr) {
		int i=pSw->GetIntValue();
		if(i>0&&IsMultiSimulation())
			m_OutputValidBinFileExist=false;
		else
			m_OutputValidBinFileExist=true;
	}
	name="TimeSerie Errors File";
	pSw=(Sw*)GetPtr(typ, grp, name);
	if(pSw!=nullptr) {
		int i=pSw->GetIntValue();
		if(i>0)
			m_OutputValidXBinFileExist=true;
		else
			m_OutputValidXBinFileExist=false;
	}

}


void Doc::DB_SetImportTime(int index, time_t time)
{

	m_DB_ImportTime[index]=time;
}
string Doc::DB_GetImportDate(int index)
{
	if (m_DB_ImportTime[index] == 0)
		m_DB_ImportTime[index] = m_DB_ImportTimeT[index];

	if(m_DB_ImportTime[index]>10) {
			struct tm timeinfo;
        
#ifdef MS_CODE
            localtime_s(&timeinfo, &m_DB_ImportTime[index]);
#else
        localtime_r(&m_DB_ImportTime[index], &timeinfo);
#endif
		
        
        

		if (timeinfo.tm_hour == -1) {
			if (m_DB_ImportTimeT[index] != 0) {
				auto timestamp = m_DB_ImportTimeT[index];
				struct tm timeinfo;
#ifdef MS_CODE
				localtime_s(&timeinfo, &m_DB_ImportTime[index]);
#else
				localtime_r(&m_DB_ImportTime[index], &timeinfo);
#endif
				char buffer[24];
				strftime(buffer, 24, "%Y-%m-%d %H:%M", &timeinfo);
				return string(buffer);
			}
			else
				return "";
		}
			char buffer[24];
			strftime(buffer,24, "%Y-%m-%d %H:%M", &timeinfo);
		return string(buffer);
	}
	else
		return "";
}
void Doc::DB_ImportDate(int index)
{
	m_DB_ImportTime[index]=time(nullptr);
}
void Doc::TimeModified()
{
	m_DocFile.m_TimeModified=time(nullptr);
}
void Doc::TimeCreated()
{
	m_DocFile.m_TimeCreated=time(nullptr);
}
void Doc::SetTimeModified(time_t time)
{
	m_DocFile.m_TimeModified=time;
}
void Doc::SetTimeCreated(time_t time)
{
	m_DocFile.m_TimeCreated=time;
}
bool Doc::OpenSimDocument(long RunNo, bool NoMulti)
{

	string pzPathName=GetCurrentSimFile().c_str();
	string Num;
	string newPathName ;
	bool FirstTry, SecondTry;
	long PreviousRunNo;

	int iposext=pzPathName.find(".Sim");
	Num=FUtil::ItoNumAscii(RunNo);
	//Num.Format(%05u,RunNo);
   	if (iposext==-1||!(pzPathName.substr(iposext-4,1)=="_")) {
		auto rpos=pzPathName.rfind('_');
		if(NoMulti) {
			auto mulpos=pzPathName.rfind("_Multi");
			if(mulpos!=string::npos&&mulpos<rpos) rpos=mulpos;
		}
		newPathName=pzPathName.substr(0,rpos);
		newPathName+='_';
		newPathName+=Num;
		newPathName+=".Sim";
	}
	else
		return false;
#ifndef COUPSTD
	CWinApp* pApp = AfxGetApp();
	CCoupModelApp *pCoupModel=(CCoupModelApp*)pApp;

	FirstTry=true;
	SecondTry=true;
	while(!FUtil::IsFileExisting(string(newPathName))&&SecondTry) {
		string NewFolder=GetCurrentSimFile();;
		int ipp=NewFolder.rfind('_');
		string UserDirectory="";
		UserDirectory=FUtil::GetProfileStringStd( "UserDirectory", UserDirectory);
		int numo=UserDirectory.size();
		if(UserDirectory.rfind('\\')!=numo-1) 
			UserDirectory+="\\";
		UserDirectory+=NewFolder.substr(0,ipp);
		numo=newPathName.rfind('\\');
		newPathName=UserDirectory+newPathName.substr(numo);
		if(FirstTry)
			FirstTry=false;
		else if(SecondTry)
			SecondTry=false;	
	}
	
	if (FirstTry||SecondTry) {
		pCoupModel->OnFileOpen(CString(newPathName.c_str()));
	}
	else {
		Num=FUtil::ItoNumAscii(RunNo);
			// No Sim File found create a new document to make a run with the same parameters
		string str;
		str="Do you want to create a new Sim file (";
		str+=Num;
		str+=") to Rerun simulation";
		if(MFC_Util::MessageBoxReturn(str,"No Sim File  found",MB_YESNO|MB_ICONEXCLAMATION)==IDYES) {

			auto rpos=pzPathName.rfind('_');
			if(NoMulti) {
				auto mulpos=pzPathName.rfind("_Multi");
				if(mulpos!=string::npos&&mulpos<rpos) rpos=mulpos;
			}
			newPathName=pzPathName.substr(0,rpos);
			string exttype = ".Sim";
			if (m_xmlFileToUse)
				exttype = ".xml";

			newPathName+="_"+Num+exttype;
			SetNewSimulation();
			m_Protect=false;
			TimeCreated();
			SetTimeModified(0);
			//ClearRunReportArray();
			PreviousRunNo=m_DocFile.m_SimulationRunNo;
			
			m_NoUpdateRunNo=true;
			if(MR_UpdateValuesFromMBin(RunNo)) {
					//SaveFileName
					m_DocFileName=newPathName;	
					m_DocFile.m_SimulationRunNo=RunNo;
					if (WriteDocFile()) {
						SetFinishedSimulation();
						m_Protect=true;
						MR_UpdateValuesFromMBin(PreviousRunNo);
						m_DocFile.m_SimulationRunNo=PreviousRunNo;
					}
					CCoupModelApp* pApp;
					pApp = (CCoupModelApp*)AfxGetApp();

					pApp->m_pDocTemplate->OpenDocumentFile(CString(newPathName.c_str()));
				//OnOpenDocument(newPathName);
				}					
			}

		
			else
				return false;
		}
#endif	
	return true;



}
void Doc::LockMultiStorageFile()
{
	m_DocFile.m_MultiStoragelocked=true;
	m_MStorage.m_Lock=true;
}
void Doc::UnLockMultiStorageFile()
{
	m_DocFile.m_MultiStoragelocked=false;
	m_MStorage.m_Lock=false;
}
bool Doc::IsMultiStorageFileLocked()
{
	return	m_DocFile.m_MultiStoragelocked;
}
void Doc::SetMonitor1View(int value)
{
	m_Monitor1View=value;
}
void Doc::SetMonitor2View(int value)
{
	m_Monitor2View=value;
}
int Doc::GetMonitor1View()
{
	return m_Monitor1View;
}
int Doc::GetMonitor2View()
{
	return m_Monitor2View;
}
void Doc::SetDefaultAnnimationSelection()
{

		std::list<Annim> ::iterator i;
		Annim anode;
		SimB *	pSimB;
		int  seq;
		string Name, Type, Group;


		AnnimVarList.clear();
		Name="Temperature";
		Group="Soil heat flows";
		Type="Auxiliary Variables";
		pSimB=GetPtr(AUX,Name);

		anode.pBase=pSimB;
		anode.iChart=1;
		anode.type=6;
		seq=0;
		for(int i=0;i<7;i++) {
			anode.LocalIndex=i;
			AnnimVarList.push_back(anode);
			((OutVector*)pSimB)->SetAnnimateChart(anode.LocalIndex,anode.iChart,seq);
			seq++;
		}

		Name="WaterContent";
		Group="Soil water flows";
		Type="Auxiliary Variables";
		pSimB=GetPtr(AUX,Name);

		anode.pBase=pSimB;
		anode.iChart=2;
		anode.type=6;

		for(int i=0;i<7;i++) {
			anode.LocalIndex=i;
			AnnimVarList.push_back(anode);
			((OutVector*)pSimB)->SetAnnimateChart(anode.LocalIndex,anode.iChart,seq);
			seq++;
		}
		Name="Snow Depth";
		Group="SnowPack";
		Type="State Variables";
		pSimB=GetPtr(STATE_SINGLE,Name);


		anode.pBase=pSimB;
		anode.iChart=3;
		anode.type=4;

		anode.LocalIndex=0;
		AnnimVarList.push_back(anode);
		((OutSingle*)anode.pBase)->SetAnnimateChart(anode.iChart,seq);
		seq++;


		Group="Additional Variables";
		Type="State Variables";

		anode.iChart=4;
		anode.type=4;
		anode.LocalIndex=0;

		Name="AccSoilEvap";
		anode.pBase=GetPtr(STATE_SINGLE,Name);
		AnnimVarList.push_back(anode);
		((OutSingle*)anode.pBase)->SetAnnimateChart(anode.iChart,seq);
			seq++;
		Name="AccSoilEvapAndTransp";
		anode.pBase=(SimB*)GetPtr(STATE_SINGLE,Name);

		AnnimVarList.push_back(anode);
		((OutSingle*)anode.pBase)->SetAnnimateChart(anode.iChart,seq);
			seq++;
		Name="AccTotalEvap";
		anode.pBase=(SimB*)GetPtr(STATE_SINGLE,Name);

		AnnimVarList.push_back(anode);
		((OutSingle*)anode.pBase)->SetAnnimateChart(anode.iChart,seq);
			seq++;
		
		

}
void Doc::ResetAnnimationSelection()
{



		AnnimVarList.clear();
	
		

}
int Doc::GetNumberAnnimationCharts() {
	int NumCharts=0;	
//	std::list<Annim> ::iterator ii;
	int i1=0;
		int i2=0;
			int i3=0;
				int i4=0;
		for(auto ii=AnnimVarList.begin(); ii!=AnnimVarList.end();++ii) {
			int ich= (*ii).iChart;
			switch (ich) {
				case 1:
					i1++;
					break;
				case 2:
					i2++;
					break;
				case 3:
					i3++;
					break;
				case 4:
					i4++;
					break;
			}
		}

	   if(i1>0) {
		   NumCharts++;	
	   }
	   if(i2>0) {
		   NumCharts++;	
	   }
	   if(i3>0) {
		   NumCharts++;	
	   }
	   if(i4>0) {
		   NumCharts++;	
	   }

	return NumCharts;
}
vector <size_t>Doc::GetValIndexByGroup(size_t group)
{
	if(!m_IsValIndexUpdated) UpdateValIndexList();
	int index=group;
	return m_ValGroupIndex[index];
}
vector <size_t>Doc::GetValIndexByType(size_t type)
{
	if(!m_IsValIndexUpdated) UpdateValIndexList();
	int index=type-4;
	return m_ValTypeIndex[index];
}
void Doc::UpdateValIndexList()
{
	int igroup;
	VALv vst;
	int iv=m_Val_Array.size();
	for(int i=0;i<4;i++)
		m_ValTypeIndex[i].clear();
	for(int i=0;i<NUMGROUP;i++)
		m_ValGroupIndex[i].clear();


	for(int i=0;i<iv;i++) {
		vst=m_Val_Array[i];
		m_ValTypeIndex[vst.OutputType-1].push_back(i);
		igroup=p_ModelInfo->GetGroupIndex(vst.Group.c_str());
		m_ValGroupIndex[igroup].push_back(i);
	}

	m_IsValIndexUpdated=true;
}
int Doc::GetNumValidValFiles()
{
	if(m_NumValidValFiles<=0) {
		m_NumValidValFiles=0;
		F *pCF;
		for(size_t i=0;i<MAXSIMVAL;i++) {
			pCF=ValidationFilePointer(i+1);
			if(pCF!=nullptr) {
				if(pCF->m_Exist) {
					m_NumValidValFiles++;
				}
			}
		}
	}
	return m_NumValidValFiles;
}

void Doc::SetGroupEnable(size_t index,bool value)
{
	m_GroupEnabled[index]=value;
}
bool Doc::GetGroupEnable(size_t index)
{ 
	return m_GroupEnabled[index];
}
PGDES Doc::GetSumOutPutDescription(int ilocal)
{
	PGDES out;
	out.Index="";
	vector<SIMB> vvp;
	size_t ind;
	ind=ilocal/6-1;
	vvp=GetAllSelectedOutputs();
	out.Name=vvp[ind].pBase->GetName();
	if(vvp[ind].pBase->Is_Vector()) {
		out.Name+="(";
		out.Name+=FUtil::STD_ItoAscii(vvp[ind].TabIndex+1);
		out.Name+=")";
		out.Index=FUtil::STD_ItoAscii(vvp[ind].TabIndex + 1);
	}
	else 
		out.Index="";
	out.Unit=vvp[ind].pBase->GetUnit();

/*	SimB* pPar;
	int lin, il;
	bool hit=false;
	for(size_t jk=0;jk<4;jk++) {
		switch(jk) {
		case 0:
			vvp=m_X_Array;
			break;
		case 1:
			vvp=m_T_Array;
			break;
		case 2:
			vvp=m_G_Array;
			break;
		case 3:
			vvp=m_D_Array;
			break;
		}
		for(size_t i=0; i<vvp.size();i++) {
						
			pPar=(SimB*)vvp[i].pBase;
			out.Name=pPar->GetName().c_str();
			if(pPar->Is_Vector() ){
				for(int ii=0;ii<((OutVector*)pPar)->GetNumberOfSelectedFlags();ii++) {
						lin=((OutVector*)pPar)->GetSelectedLocalIndex(ii);
						int ipg=((OutVector*)pPar)->GetPgFileIndex(lin);
						il=0;
						if(ipg==ilocal/6) {
							il=lin;
							ii=((OutVector*)pPar)->GetNumberOfSelectedFlags();
							out.Unit=((OutVector*)pPar)->GetUnit().c_str();
							out.Name=((OutVector*)pPar)->GetName().c_str();
							out.Index=FUtil::STD_ItoAscii(lin+1);
							i=vvp.size();
							hit=true;
							break;
						}
				}
			}
			else {
				lin=-1;
				int ipg=((OutSingle*)pPar)->GetPgFileIndex();
				if(ipg==ilocal/6) {
					out.Name=pPar->GetName().c_str();
					i=vvp.size();
					hit=true;
					break;
				}									


			}					
		}
		if(hit) break;
	}*/
	return out;
}
Sw* Doc::GetSwPointer(Sw *pSw, Doc *pDoc)
{
	Sw *pNew=nullptr;
	string name, group, type;
	type="Switches";
	name=pSw->GetName();
	group=pSw->GetGroup();
	pNew=(Sw*)pDoc->GetPtr(type,group,name);
	return pNew;
}
Ps* Doc::GetParPointer(Ps *pP, Doc *pDoc)
{
	Ps *pNew=nullptr;
	string name, group, type;
	type="Parameters";
	name=pP->GetName();
	group=pP->GetGroup();
	pNew=(Ps*)pDoc->GetPtr(type,group,name);
	return pNew;
}
P* Doc::GetParTabPointer(P *pPt, Doc *pDoc)
{
	P *pNew=nullptr;
	string name, group, type;
	type="Table Parameters";
	name=pPt->GetName();
	group=pPt->GetGroup();
	pNew=(P*)pDoc->GetPtr(type,group,name);
	return pNew;
}

bool Doc::CheckSelectedDocumentsValidity()
{
#ifndef STD
//		SimArchive *pArchive=MySingle::instance().GetSimArchivePointer();
	/*
		bool valid;
		for(size_t ii=0; ii<m_SelectedDocuments.size(); ii++) {
			valid=false;
			for(size_t i=0; i<pArchive->GetNumSimDoc();i++) {
				Doc *pSimDoc=pArchive->GetSimDocPointer(i);
				if(m_SelectedDocuments[ii]==pSimDoc) {
					valid=true;
					continue;
				}
			}
			if(!valid) m_SelectedDocuments.erase(m_SelectedDocuments.begin()+ii);
		}*/
#endif STD

	return true;
}
