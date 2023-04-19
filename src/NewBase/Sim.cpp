//#include "../stdafx.h"
#include "./Sim.h"
#include "../Util/FUtil.hpp"
#include <ctime>
#include <thread>
//#include <direct.h>
#include <iostream>


#ifndef COUPSTD
#include "../MainFrm.h"


static Sim* pRunDoc;

UINT _cdecl Run_SingleCoupModel_Background(void *param)
{	

#ifdef COUPSTD
		cout<<"Simulation Started"<<endl;
#endif
#ifndef COUPSTD
		CWinApp* pApp = AfxGetApp();
		CCoupModelApp* pCoupModel = (CCoupModelApp*)pApp;
		CMainFrame* p_mainframe;
		p_mainframe = (CMainFrame*)pCoupModel->m_pMainWnd;
#endif

	
		bool simresult;
		simresult=true;
		Sim *pRunDoc=p_ModelInfo->GetRunDoc();
		pRunDoc=(Sim*)param;
		if(!pRunDoc->m_Simulator.Start())  simresult=false;
		if(simresult) if(!pRunDoc->m_Simulator.Run()) simresult=false;
		if(simresult) if(!pRunDoc->m_Simulator.End()) simresult=false;

	//SOIL_MAIN_mp_SOIL_RUN();

	if(simresult) {

			time_t tt;
			tt=time(nullptr);
			struct tm timeinfo;
			localtime_s(&timeinfo, &tt);
			char buffer[24];
			strftime(buffer,24, "%Y-%m-%d %H:%M:%S", &timeinfo);
			pRunDoc->m_DocFile2.m_DateRun=buffer;


		pRunDoc->MR_CHECK_Bayesian();
		pRunDoc->m_Protect=false;
		pRunDoc->History_Add(12,"Single Run Completed");
		pRunDoc->SetFinishedSimulation();
		pRunDoc->m_ValidationData.SetPointersToOutputValidationFiles();
#ifndef COUPSTD
		string Res="Simulation completed Run No: ";
		Res+=FUtil::STD_ItoAscii(pRunDoc->m_DocFile.m_SimulationRunNo);
		p_mainframe->UpdatePaneText(0, Res);
	
		//MFC_Util::MessageBox( Res,"Simulation report" ,MB_OK);	

#endif
#ifdef COUPSTD
		cout<<"Simulation Completed"<<endl;
#endif

		//int SimulationRunNo=MFC_Util::GetProfileIntNo("SimulationRunNo", pRunDoc->m_DocFile.m_SimulationRunNo);
		//MFC_Util::WriteProfileInt("SimulationRunNo", SimulationRunNo+1);

		
		if (!pRunDoc->WriteDocFile()){

		}
	}
	else {
		//Failure 
#ifndef COUPSTD
		MFC_Util::MessageBox("Simulation completed with Failure!\nCheck Messages in Document to find reasons. ","Simulation report" ,MB_OK);	

#endif
#ifdef COUPSTD
		cout<<"Simulation Completed with Failure\nCheck Messages in Document to find reasons. "<<endl;
#endif



	}

	p_ModelInfo->SetRunning(false);
	p_ModelInfo->SetRunning(false);
	pRunDoc->SetRunning(false);
	p_ModelInfo->SetViewStatus(1);
	p_mainframe->UpdatePaneText(0, "Completed Single Run - Click to continue");
	

	return 1;
		
	
/*	else {

		pRunDoc->History_Add(12,"Single Run Failed");
#ifndef COUPSTD
		choice=MessageBox((HWND)param, "Simulation completed with errors \n no update of run number ! See Messages in the Document window to get details on error !","Simulation report" ,MB_YESNO|MB_ICONQUESTION|MB_TASKMODAL);
		ReadyToSimulate=true;
		if(choice==IDYES) ViewStatus=0;
		pRunDoc->m_Link.ReportReady();
		Running=false;
#endif
		return 1;
	}*/


}
UINT _cdecl Run_MultiCoupModel_Background(void* param)
{	int choice;
#ifndef COUPSTD
CWinApp* pApp = AfxGetApp();
CCoupModelApp* pCoupModel = (CCoupModelApp*)pApp;
CMainFrame* p_mainframe;
p_mainframe = (CMainFrame*)pCoupModel->m_pMainWnd;
#endif
	
	Sim *pRunDoc=p_ModelInfo->GetRunDoc();
	pRunDoc=(Sim*)param;
#ifndef COUPSTD
	//MultiRunning=true;
	//Running=true;
	p_ModelInfo->SetMultiRunning(true);
	p_ModelInfo->SetRunning(true);
	
	p_mainframe->UpdatePaneText(0,"Multi Run Started");
#endif
//	PARAMETERS_mp_RUN_NO=(pRunDoc->m_DocFile.m_SimulationRunNo-1)%99999+1;

	//pRunDoc->CH_Checker_build();
	bool CompleteLoop=false;
	bool FirstRun=true;
	
	StatUtil::rand_init(size_t(pRunDoc->m_Simulator.m_StructureModel.m_Structure.FRandomSeed));
	pRunDoc->MR_INIT_Bayesian_Nelder_Mead();
	pRunDoc->MR_UpdateValues();

	((Sim*)pRunDoc)->m_Simulator.m_pModelInfo->TotalNoMulRun=pRunDoc->MR_Get_TotalNumberofRuns();
	while(!(CompleteLoop||pRunDoc->IsMultiStopped())	) {
		if(pRunDoc->MR_Get_ActualRunNumber()==pRunDoc->MR_Get_TotalNumberofRuns()) 	CompleteLoop=true;

		bool simresult;
		simresult=true;
		((Sim*)pRunDoc)->m_Simulator.m_pModelInfo->ActualNoMulRun=pRunDoc->MR_Get_ActualRunNumber();
		if(!((Sim*)pRunDoc)->m_Simulator.Start(true, FirstRun)) {
			simresult=false;
			p_ModelInfo->SetMultiRunning(false);
			p_ModelInfo->SetRunning(false);
			return 0;
		}
		if(!((Sim*)pRunDoc)->m_Simulator.Run(true)) simresult=false;
		pRunDoc->SetCurrentName_SimNo(pRunDoc->m_DocFile.m_SimulationRunNo);

		if(!((Sim*)pRunDoc)->m_Simulator.End(true, CompleteLoop, FirstRun)) simresult=false;
		FirstRun=false;



	//	SOIL_MAIN_mp_SOIL_RUN();
		//int i=0;
		if(simresult) {
			//Posterior to sucessful Simulaton
			pRunDoc->SetCurrentName_SimNo(pRunDoc->m_DocFile.m_SimulationRunNo);

			//pRunDoc->m_ValidationData.SendValIndextof90();
			pRunDoc->MR_CHECK_Bayesian(); 
			pRunDoc->MR_Storage_Add();
#ifndef COUPSTD
		//	ReadyToSimulate=true;
			p_ModelInfo->SetReady(true);
			pRunDoc->m_Link.ReportUpdate();
#endif
			if(pRunDoc->MR_Get_ActualRunNumber()%1000==1)
				pRunDoc->MR_Storage_Close();	

			if(p_ModelInfo->GetNoSingleRun()==0){
				 pRunDoc->SetFinishedSimulation();
				 pRunDoc->SetPathName(pRunDoc->GetCurrentSimFile());
				 if (!pRunDoc->WriteDocFile()) {
#ifndef COUPSTD
					 p_ModelInfo->SetMultiRunning(false);
					 p_ModelInfo->SetRunning(false);
					 choice=MFC_Util::MessageBoxReturn( "Simulation completed with errors \n ","Write Result Write Error" ,MB_YESNO|MB_ICONQUESTION|MB_TASKMODAL);
#endif						
					 return 0;
				 }
				 pRunDoc->SetNewSimulation();
			}

			if(!CompleteLoop) {
				//Prior New MultiSim
				pRunDoc->MR_SetResetMonitoring();
				if(pRunDoc->MR_UpdateDimCounter())
					pRunDoc->MR_UpdateValues();
				
				//BAYESIANCALIB_mp_ACCEPTEDRUN=true;

				pRunDoc->ApplyOptStartValues();
	//! New Actions for WaterShed Simulations
				if(pRunDoc->m_WShed_Active) {
					pRunDoc->WSHED_UpdateCounter();
					pRunDoc->WSHED_UpdateValues();
				}
			//	pRunDoc->m_ValidationData.SendValIndextof90();
				//PARAMETERS_mp_RUN_NO=(pRunDoc->m_DocFile.m_SimulationRunNo-1)%99999+1;
			}

		}
		else {

			int actno=pRunDoc->MR_Get_ActualRunNumber();
			pRunDoc->History_Add(12,"Single Run Failed During MultiRun");
#ifndef COUPSTD
			p_ModelInfo->SetMultiRunning(false);
			p_ModelInfo->SetRunning(false);
			choice=MFC_Util::MessageBoxReturn("Simulation completed with errors \n no update of run number ! Do You want to toggle to run report view ?","Simulation report" ,MB_YESNO|MB_ICONQUESTION|MB_TASKMODAL);
			if(choice) {

			}
#endif
			return 0;

		}
	}


	pRunDoc->SetCurrentFileName(pRunDoc->m_OrgPathName);
	pRunDoc->SetPathName(pRunDoc->m_OrgPathName);
	pRunDoc->SetRunning(false);
#ifndef COUPSTD	
	p_ModelInfo->SetMultiRunning(false);
	p_ModelInfo->SetRunning(false);
	p_ModelInfo->SetReady(false);
#endif
	pRunDoc->MR_RESET_Bayesian();
	pRunDoc->MR_ResetDimCounter();
	pRunDoc->MR_Storage_InitEvaluateStat();
	pRunDoc->MR_Storage_InitEnsembles();
	

	if(pRunDoc->IsBayesianCalibration()) {
		pRunDoc->UpdateCoVar();
		pRunDoc->MR_ReCalculatePostDist();

	}

	if(pRunDoc->IsMultiStopped()) {
		pRunDoc->CH_Checker_Reset();
		pRunDoc->History_Add(12,"MultiRun Interupt by user");
#ifndef COUPSTD
		MFC_Util::MessageBox("MultiRun Interupt by user \n");
#endif
	}
	else {
		pRunDoc->History_Add(12,"MultiRun Completed");
		pRunDoc->SetFinishedSimulation();
		pRunDoc->m_DocFile.m_SimulationRunNo-=pRunDoc->MR_Get_TotalNumberofRuns()-1;
		if(!pRunDoc->WriteDocFile()) {

			return 0;
		}

#ifndef COUPSTD
		p_mainframe->UpdatePaneText(0,"Multi Run Completed");

		p_mainframe->m_wndEvalMulti.m_ValidationCug.m_pSimDoc =(Doc*)pRunDoc;
		p_mainframe->m_wndEvalMulti.m_ValidationCug.OnSetup();
		//pRunDoc->m_Link.ReportReady(pCoupDoc);
		//MFC_Util::MessageBox("MultiRun Completed \n");
#endif
	}
	pRunDoc->MR_Storage_Close();
	pRunDoc->SetFinishedSimulation();
	pRunDoc->UpdateValidationPerformance_SmartFlexVariables();
	pRunDoc->m_ValidationData.SetPointersToOutputValidationFiles();
	p_mainframe->UpdatePaneText(0, "Multi Run Completed - Click to continue");
	return 0;



}
#endif
Sim::Sim()
{
	m_strCmdLine="";
	m_SimulationPriority=0;
	m_Simulator.SetSimPointer(this);
	m_UserDirectory="";
}
Sim::~Sim()
{
	m_strCmdLine.clear();
}

void Sim::ClearRunReportArray()
{
	m_Report_Array.clear();

}
size_t Sim::AddToRunReport(string str) {
	m_Report_Array.push_back(str);
	return m_Report_Array.size();

}

string Sim::GetCurrentSubDirectory()
{
	if(m_CurrentSubDirectory=="") {
		int i=m_DocFile2.m_Multi_MBin_File.find("_Multi_");
		if(i>0)
			m_CurrentSubDirectory=m_DocFile2.m_Multi_MBin_File.substr(0,i);
	}
	return m_CurrentSubDirectory;
}
string Sim::GetCurrentSubDirectoryFile()
{
	return m_CurrentSubDirectoryFile;
}

pair<bool, unique_ptr<Register>> Sim::RunModel_Using_Postgres(int pkey, bool makechildDocument, unique_ptr<Register> pReg) {

	//if (SelectDoc_From_Postgres(pkey, false)) {
		m_PG_OutPutFile.SetOnlyMemoryUse(true);

		m_ChildDocument = makechildDocument;
		m_pCommonModelInfo->SetForRunningWithoutFiles(true);
		for(size_t i=0;i<MAXSIMVAL;++i)
			ValidationResultPG_Pointer(i)->SetOnlyMemoryUse(true);

		if (GetMultiRunIfPossible() && IsMultiRunEnabled()) {


							
			m_PG_MultiOutputFile.SetOnlyMemoryUse(true);
			if (GetDB_Action() != 0) {

				pair<int, unique_ptr<Register>> pn = FUtil::GetProfileIntNo("SimulationRunNo", 1, move(pReg));
				int testno = pn.first; pReg = move(pn.second);

				if (m_DocFile.m_SimulationRunNo <= testno) {
					m_DocFile.m_SimulationRunNo = testno;
				}
				m_DocFile.m_SimulationRunNo++;
				pReg = FUtil::WriteProfileInt("SimulationRunNo", m_DocFile.m_SimulationRunNo, move(pReg));
			}
			
			
			
			
			
			
			CheckAndUpdateFileName(true, true);

			
			return MakeMultiRun(true, pkey, move(pReg));
		}
		else
			return MakeSingleRun(true, pkey, move(pReg));
	//}
	//m_pCommonModelInfo->SetForRunningWithoutFiles(false);
}
void Sim::SetCurrentName_SimNo(int NewNo)
{
	string numstr, newPathName;
	numstr=FUtil::ItoNumAscii(NewNo);
	int ipos2, ipos3;

	ipos2=m_CurrentFile.rfind("_");
	ipos3=m_CurrentFile.rfind(".");
			
	if(ipos2>0&&ipos3>ipos2) {
		if(m_xmlFileToUse) 
			newPathName = m_CurrentFile.substr(0, ipos2 + 1) + numstr + ".xml";
		else
			newPathName=m_CurrentFile.substr(0,ipos2+1)+numstr+".Sim";
	}
	else if(ipos3>0) {
		if (m_xmlFileToUse)
			newPathName = m_CurrentFile.substr(0, ipos3) + numstr + ".xml";
		else
			newPathName=m_CurrentFile.substr(0,ipos3)+numstr+".Sim";
	}
	else {
		newPathName="MySim_"+numstr+".Sim";
	}
	m_CurrentFile=newPathName;


}
void Sim::SetCurrentFileName(string pzPathName)
{
	m_CurrentFile=pzPathName;
	m_DocFileName=m_CurrentFile;
	cout << m_DocFileName << endl;
	int iv;
#ifndef LINUX2
		iv = m_CurrentFile.rfind('\\');
#else
	iv = m_CurrentFile.rfind('/');
#endif

	if (iv>1) m_CurrentDirectory = m_CurrentFile.substr(0, iv + 1);
	
}
void Sim::SetCurrentSubDirectory(string str)
{
	m_CurrentSubDirectory=str;

	
	string file;
size_t ipos=m_CurrentFile.rfind("\\");
	if(ipos<m_CurrentFile.size()&&ipos!=string::npos) {
		file=m_CurrentFile.substr(ipos+1);
		m_CurrentSubDirectoryFile=m_CurrentSubDirectory+file;
	}
}
bool Sim::FixSumIndex()
{
#ifndef COUPSTD
	pRunDoc=this;
#endif
//	SOIL_MAIN_mp_FIXSUMINDEX();
	return true;
}

#ifdef COUPSTD
unique_ptr<Register> Sim::SetNewRunNo(bool DB_Source, int pkey, bool ChildDocument, unique_ptr<Register> reg_pointer)
{
	if (DB_Source == true) {
		if (GetDB_Action() != 0) {
			pair<int, unique_ptr<Register>> pn = FUtil::GetProfileIntNo("SimulationRunNo", 1, move(reg_pointer));
			int testno = pn.first; reg_pointer = move(pn.second);
			if (m_DocFile.m_SimulationRunNo <= testno) {
				m_DocFile.m_SimulationRunNo = testno;
			}
			m_DocFile.m_SimulationRunNo++;
			reg_pointer = FUtil::WriteProfileInt("SimulationRunNo", m_DocFile.m_SimulationRunNo, move(reg_pointer));
			m_ChildDocument = ChildDocument;
		}
	}
	History_Add(13, FUtil::ItoNumAscii(m_DocFile.m_SimulationRunNo));
	return move(reg_pointer);

}
pair<bool, unique_ptr<Register>> Sim::MakeSingleRun(bool DB_Source, int pkey, unique_ptr<Register> reg_pointer) 
{
	if (DB_Source == true) {
		m_IsUsingDB_Source = true;
		m_ValidationData.Init(this);
		m_ValidationData.SetPointersToOutputValidationFiles(true);
		m_MStorage.Init(this);
	}

	m_DocFile.m_TimeModified = time(nullptr);
	SetSingleSimulation();
	ApplyOptStartValues();
	if (!CheckAndUpdateFileName(false, DB_Source)) return pair<bool, unique_ptr<Register>>(false, move(reg_pointer));
	m_SingleStartTime = clock();
	p_ModelInfo->SetRunning(true);
	p_ModelInfo->SetRunDoc(this);


	bool simresult;
	simresult = true;
	
	if (!m_Simulator.Start())  simresult = false;
	if (simresult) if (!m_Simulator.Run()) simresult = false;
	if (simresult) if (!m_Simulator.End()) simresult = false;
	if (simresult) {
		m_DocFile2.m_DateRun = FUtil::GetCurrentDateTime();

		MR_CHECK_Bayesian();
		m_Protect = false;
		History_Add(12, "Single Run Completed");
		SetFinishedSimulation();
		if(!DB_Source)	m_ValidationData.SetPointersToOutputValidationFiles();

		cout << "Simulation Completed" << endl;
#ifndef NO_FILES
		if (pkey > 0&& GetDB_Action() == 0) DeleteDoc_From_Postgres(pkey);
		m_pRegister = move(reg_pointer);

		bool result=WriteDocFile("", DB_Source);
		reg_pointer = move(m_pRegister);
		return pair<bool, unique_ptr<Register>>(result, move(reg_pointer));
#else
        return true;
#endif
		}
	return pair<bool,unique_ptr<Register>>(true, move(reg_pointer));
}

pair<bool, unique_ptr<Register>> Sim::MakeMultiRun(bool DB_Source, int pkey, unique_ptr<Register> reg_pointer) {

	if (DB_Source == true) {
		m_IsUsingDB_Source = true;
		m_ValidationData.Init(this);
		m_ValidationData.SetPointersToOutputValidationFiles(true);
		m_MStorage.Init(this);
		MR_RESET_Bayesian();
		MR_ResetDimCounter();
		if (GetDB_Action() != 0) {

			pair<int, unique_ptr<Register>> pn = FUtil::GetProfileIntNo("SimulationRunNo", 1, move(reg_pointer));
			int testno = pn.first; reg_pointer = move(pn.second);

			if (m_DocFile.m_SimulationRunNo <= testno) {
				m_DocFile.m_SimulationRunNo = testno;
			}
			m_DocFile.m_SimulationRunNo++;
			reg_pointer = FUtil::WriteProfileInt("SimulationRunNo", m_DocFile.m_SimulationRunNo, move(reg_pointer));
		}
	}

	p_ModelInfo->SetMultiRunning(true);
	p_ModelInfo->SetRunning(true);

	Sw *pSw=dynamic_cast<Sw*>(GetPtr(SWITCH,"No SingleRunSimFile"));

	bool test=bool(pSw->GetIntValue());
	p_ModelInfo->SetNoSingleRun(bool(pSw->GetIntValue()));

	bool CompleteLoop = false;
	bool FirstRun = true;
	StatUtil::rand_init(size_t(m_Simulator.m_StructureModel.m_Structure.FRandomSeed));

	int StartRunNo;
	SetCurrentFileName(m_DocFileName);
	
	string NewFolder = GetCurrentSimFile();
	m_OrgPathName = NewFolder;
	int ipp = NewFolder.find("_Multi");
	if (ipp<0) 
		return pair<bool, unique_ptr<Register>>(false, move(reg_pointer));

	StartRunNo = m_DocFile.m_SimulationRunNo;
	size_t numout;
	{
		auto v = GetAllSelectedOutputs();
		numout = v.size();
	}

	MR_Storage_Init(DB_Source, numout);

	string UserDirectory = "";
	{
		auto RegPointer = make_unique<Register>();
		pair<string, unique_ptr<Register>> p = FUtil::GetProfileStringStd("UserDirectory", UserDirectory, move(RegPointer));
		UserDirectory = p.first; RegPointer = move(p.second);
	}
	int numo = UserDirectory.size();
	m_UserDirectory = UserDirectory;
	if (UserDirectory.rfind("\\") != numo - 1)
		UserDirectory += "\\";
	UserDirectory = NewFolder.substr(0, ipp);

// Remove dir creation
//    _mkdir(UserDirectory.data());
	FUtil::CreateSubDirectory(UserDirectory);
	m_UserSubDirectory = UserDirectory+"\\";
	SetCurrentSubDirectory(m_UserSubDirectory);

	MR_INIT_Bayesian_Nelder_Mead();
	MR_UpdateValues();

	m_Simulator.m_pModelInfo->TotalNoMulRun = MR_Get_TotalNumberofRuns();
	while (!(CompleteLoop || IsMultiStopped())) {
		if (MR_Get_ActualRunNumber() == MR_Get_TotalNumberofRuns()) 	CompleteLoop = true;

		bool simresult;
		simresult = true;
		m_Simulator.m_pModelInfo->ActualNoMulRun = MR_Get_ActualRunNumber();
		if (!m_Simulator.Start(true, FirstRun)) { simresult = false; return pair<bool, unique_ptr<Register>>(false, move(reg_pointer)); }
		if (!m_Simulator.Run(true)) simresult = false;
		SetCurrentName_SimNo(m_DocFile.m_SimulationRunNo);

		if (!m_Simulator.End(true, CompleteLoop, FirstRun)) simresult = false;
		FirstRun = false;



		//	SOIL_MAIN_mp_SOIL_RUN();
		//int i=0;
		if (simresult) {
			//Posterior to sucessful Simulaton
			SetCurrentName_SimNo(m_DocFile.m_SimulationRunNo);

			//m_ValidationData.SendValIndextof90();
			MR_CHECK_Bayesian();
			MR_Storage_Add();

			if (MR_Get_ActualRunNumber() % 100 == 1)
				MR_Storage_Close();

			if (p_ModelInfo->GetNoSingleRun() == 0){
				SetFinishedSimulation();
				SetPathName(GetCurrentSimFile());

				bool result = WriteDocFile("", DB_Source);
				return pair<bool, unique_ptr<Register>>(result, move(reg_pointer));


				if (!result) {
#ifndef COUPSTD
					pCommonModel->SetMultiRunning(false);
					pCommonModel->SetRunning(false);

					choice = MessageBox((HWND)param, "Simulation completed with errors \n ", "Write Result Write Error", MB_YESNO | MB_ICONQUESTION | MB_TASKMODAL);
#else
					cout << "Simulation Completed with Error";

#endif						
					return pair<bool, unique_ptr<Register>>(false, move(reg_pointer));
					}
				SetNewSimulation();
				}

			if (!CompleteLoop) {
				//Prior New MultiSim
				MR_SetResetMonitoring();
				if (MR_UpdateDimCounter())
					MR_UpdateValues();
					ApplyOptStartValues();
				//! New Actions for WaterShed Simulations
				if (m_WShed_Active) {
					WSHED_UpdateCounter();
					WSHED_UpdateValues();
				}
			}

		}
		else {

			int actno = MR_Get_ActualRunNumber();
			History_Add(12, "Single Run Failed During MultiRun");
#ifndef COUPSTD
			pCommonModel->SetMultiRunning(false);
			pCommonModel->SetRunning(false);
			choice = MessageBox((HWND)param, "Simulation completed with errors \n no update of run number ! Do You want to toggle to run report view ?", "Simulation report", MB_YESNO | MB_ICONQUESTION | MB_TASKMODAL);
			if (choice) {

				}
#endif
			return pair<bool, unique_ptr<Register>>(false, move(reg_pointer));

			}
		}
	SetFinishedSimulation();

	SetCurrentFileName(m_OrgPathName);
	SetPathName(m_OrgPathName);
#ifndef COUPSTD	
	pCommonModel->SetMultiRunning(false);
	pCommonModel->SetRunning(false);
	pCommonModel->SetReady(false);
#endif
	MR_RESET_Bayesian();
	MR_ResetDimCounter();
	MR_Storage_InitEvaluateStat();
	MR_Storage_InitEnsembles();


	MR_Storage_Close();

	m_DocFile.m_SimulationRunNo = StartRunNo;

	if (pkey > 0 && GetDB_Action() == 0) DeleteDoc_From_Postgres(pkey);
	bool result = WriteDocFile("", DB_Source);
	return pair<bool, unique_ptr<Register>>(result, move(reg_pointer));


	MR_Storage_Reset();
	MR_Storage_Open();
#ifdef COUPSTD
	cout << "MultiRun Simulation completed " + m_DocFileName+'\n';
#endif
	if (IsBayesianCalibration()) {
		UpdateCoVar();
		MR_ReCalculatePostDist();

	}
	return pair<bool, unique_ptr<Register>>(result, move(reg_pointer));
}




#endif


bool Sim::OnSimStart() 

{
#ifndef NO_MS_GUI

	//pRunDoc=this;
	//pCoupDoc=pDoc;


	/*if(!m_Simulator.Start()) return false;
	if(!m_Simulator.Run()) return false;
	return m_Simulator.End();*/
	string message; 

	//BAYESIANCALIB_mp_ACCEPTEDRUN=true;

 	SetCurrentFileName(m_DocFileName);
	int SimulationNo=1;
	if(!m_NoUpdateRunNo&&!m_DocFile.m_MultiRunning)
		m_DocFile.m_SimulationRunNo=MFC_Util::GetProfileIntNo("SimulationRunNo", m_DocFile.m_SimulationRunNo);


	SimB* pStart;

	string Name, Group, Type;

	Name="StartWithPrevious";
	Group="Technical";
	Type="Switches";
    pStart=(SimB*)GetPtr(Type,Group, Name);


//	PARAMETERS_mp_STARTSTATEVALUES=pStart->GetValue();

	

	if(IsFinishedSimulation()&&(!m_DocFile.m_MultiRunning&&!m_WShed_Active)) {
			return false;

	}
	if(!m_DocFile.m_MultiRunning) {
			string UserDirectory="";
			UserDirectory=MFC_Util::GetProfileStringStd("UserDirectory", UserDirectory);
			int numo=UserDirectory.size();
			m_UserDirectory=UserDirectory;
			if(UserDirectory.rfind("\\")!=numo-1) 
					UserDirectory+="\\";

	}


		ApplyOptStartValues();

		//m_ValidationData.SendValIndextof90();


		//pRunDoc=this;	// Set static variable to keep the link to Fortran  



	
		//ClearRunReportArray();

		m_SingleStartTime=clock();
#ifndef COUPSTD
		//ReadyToSimulate=false;		
		p_ModelInfo->SetRunning(true);
		SetRunning(true);
		p_ModelInfo->SetRunDoc(this);

  		HWND hWnd=AfxGetMainWnd()->GetSafeHwnd();
		CWinThread* thread;
		 thread=nullptr;
		 void *pVoid=nullptr;
		if(m_SimulationPriority==0) 
			AfxBeginThread(Run_SingleCoupModel_Background,this, THREAD_PRIORITY_HIGHEST);
 		else if(m_SimulationPriority==1)
			thread=AfxBeginThread(Run_SingleCoupModel_Background,this, THREAD_PRIORITY_NORMAL);
		else
			thread=AfxBeginThread(Run_SingleCoupModel_Background,this, THREAD_PRIORITY_BELOW_NORMAL);

		if(thread==nullptr) {
			return false;


		}
		thread->m_bAutoDelete=FALSE;
		thread->ResumeThread();
//		thread first (Run_SingleCoupModel_Background);
		//first.join();
#endif
#ifndef COUP2013
#ifdef COUPSTD
		//Run_SingleCoupModel_Background(nullptr);
#endif


	//	RunOpt=PARAMETERS_mp_RUN_OPTIONS;
#endif	    */

#endif
		return true; 
}
bool Sim::OnSimMultiStart() 
{
	//pCoupDoc=pDoc;

#ifndef NO_MS_GUI
 // Read data from files if necessary.
  if (MR_ReadData() == -1) {
#ifndef COUPSTD
	  p_ModelInfo->SetMultiRunning(false);

	return false;
  }
  else {

	SetCurrentFileName(m_DocFileName);
	SetMultiSimulation();

	string NewFolder=GetCurrentSimFile();
	int ipp=NewFolder.find("_Multi");
	if(ipp<0) {
		return false;
		// No Multi in File Name

	}

	MR_Storage_Init();//
	
	string UserDirectory="";
	UserDirectory=MFC_Util::GetProfileStringStd( "UserDirectory", UserDirectory);
	int numo=UserDirectory.size();
	m_UserDirectory=UserDirectory;
	if(UserDirectory.rfind("\\")!=numo-1) 
			UserDirectory+="\\";
	UserDirectory=NewFolder.substr(0,ipp);




	if(!CreateDirectory(CString(UserDirectory.c_str()),nullptr)) {
		UserDirectory+="\\";
		if(!CreateDirectory(CString(UserDirectory.c_str()),nullptr)) {
			//MessageBox("Failed to create directory :"+UserDirectory,"File Name Error");
			//	return;
		}
		//Action if not created
	}
	else
		UserDirectory+="\\";	

	
//	PARAMETERS_mp_NUMCWORKDIR=UserDirectory.size();
//	for(int i=0;i<PARAMETERS_mp_NUMCWORKDIR;i++)
//		PARAMETERS_mp_WORKDIR[i]=UserDirectory[i];
	
	m_UserSubDirectory=UserDirectory;
	SetCurrentSubDirectory(m_UserSubDirectory);

	pRunDoc=this;
#endif
	m_OrgPathName=GetCurrentSimFile();

	//SetCurrentFileName(GetCurrentSubDirectoryFile());



	MR_INIT_Bayesian_Nelder_Mead();

	Sw *pSw;

	LockMultiStorageFile();

	if(m_ReReadMStorageFile) {
		MR_Storage_OpenPrevious(); //
		MR_Storage_Open(); // Reset to acual mBin
		MR_Storage_Close(); // Close actual mBin
	}
	MFC_Util::WriteProfileInt("SimulationRunNo", m_DocFile.m_SimulationRunNo+MR_Get_TotalNumberofRuns());



   // Sw=(Sw*)GetPtr("Switches","Technical","No SingleRunSimFile");
	
	pSw=dynamic_cast<Sw*>(GetPtr(SWITCH,"No SingleRunSimFile"));

	bool test=bool(pSw->GetIntValue());
	p_ModelInfo->SetNoSingleRun(bool(pSw->GetIntValue()));


	SimB* pStart;

	string Name, Group, Type;

	Name="StartWithPrevious";
	Group="Technical";
	Type="Switches";
    pStart=(SimB*)GetPtr(Type,Group, Name);

	//PARAMETERS_mp_RUN_OPTIONS=RunOpt ;
	//PARAMETERS_mp_STARTSTATEVALUES=pStart->GetValue();
	p_ModelInfo->SetRunDoc(this);
	SetRunning(true);


#ifndef COUPSTD

	//HWND hWnd=AfxGetMainWnd()->GetSafeHwnd();

	if(m_SimulationPriority==0) 
			AfxBeginThread(Run_MultiCoupModel_Background,this,THREAD_PRIORITY_NORMAL);
	else if(m_SimulationPriority==1)
			AfxBeginThread(Run_MultiCoupModel_Background,this,THREAD_PRIORITY_NORMAL);
	else
			AfxBeginThread(Run_MultiCoupModel_Background,this,THREAD_PRIORITY_BELOW_NORMAL);

#endif
#ifndef COUP2013
#ifdef COUPSTD
	Run_MultiCoupModel_Background(nullptr);

#endif
#endif

	//RunOpt=PARAMETERS_mp_RUN_OPTIONS;
	return true;

  }	
#endif
  return false;


}

	
bool Sim::CheckAndUpdateFileName(bool MultiRun, bool DB_Source) {
	string newfilename;
	string oldfilename = GetCurrentSimFile();
	m_OrgPathName = oldfilename;
	size_t iposExt, iposMul, iposSim, iposXml;
	iposExt = oldfilename.rfind("_");
	iposMul = oldfilename.rfind("_Multi_");
	iposSim = oldfilename.rfind(".Sim");
	iposXml = oldfilename.rfind(".xml");

	bool xmlType = true;
	if (iposXml == string::npos) xmlType = false;
	int SimulationRunNo;
	if (!DB_Source) {
		pair<int, unique_ptr<Register>> p = FUtil::GetProfileIntNo("SimulationRunNo", 1);
		SimulationRunNo = p.first;
		if (SimulationRunNo < 0) {
			if (iposExt != string::npos && iposSim != string::npos) {
				if (iposSim - iposExt == 7) {
					string filenum = oldfilename.substr(iposExt + 1, 6);
					SimulationRunNo = FUtil::AtoInt(filenum);
				}
			}
		}
		SimulationRunNo = max(1, min(SimulationRunNo, 99999));
		m_xmlFileToUse = true;

	}
	else {
		iposXml = true;
		SimulationRunNo = m_DocFile.m_SimulationRunNo++;

	}
	string NumStr = FUtil::ItoNumAscii(SimulationRunNo);
	string filtypstr = ".Sim";
	if (iposXml) filtypstr = ".xml";
	if (MultiRun) {

		if (iposMul != string::npos&&iposExt != string::npos)
			newfilename = oldfilename.substr(0, iposMul + 7) + NumStr + filtypstr;
		else if (iposExt != string::npos)
			newfilename = oldfilename.substr(0, iposExt + 1) + "Multi_"+ NumStr + filtypstr;
		else if (iposSim != string::npos)
			newfilename = oldfilename.substr(0, iposSim) + "_" +"_Multi_"+ NumStr + filtypstr;
		else
			newfilename = oldfilename;
	
	}
	else {

		if (iposMul != string::npos&&iposExt != string::npos)
			newfilename = oldfilename.substr(0, iposMul + 1) + NumStr + filtypstr;
		else if (iposExt != string::npos)
			newfilename = oldfilename.substr(0, iposExt + 1) + NumStr + filtypstr;
		else if (iposSim != string::npos)
			newfilename = oldfilename.substr(0, iposSim) + "_" + NumStr + filtypstr;
		else
			newfilename = oldfilename;
	}

	SetCurrentFileName(newfilename);
	m_DocFile.m_SimulationRunNo = SimulationRunNo;
	
#ifndef NO_FILES
	if(!DB_Source&&!WriteDocFile()) return false;
#endif

	return true;


}

bool Sim::CreateNewDocFromCurrentDoc() {

	auto reg_pointer = make_unique<Register>();
	string pzPathName, Num, newPathName;
	int ipos_sim, iposext, iposMul;
	// Create new document
	//if(ReadyToSimulate) return;

	pzPathName = GetCurrentSimFile();
	iposMul = pzPathName.find("_Multi");

	iposext = pzPathName.rfind('_');
	ipos_sim = pzPathName.rfind(".Sim");
	if (ipos_sim == string::npos) {

		ipos_sim = pzPathName.rfind(".xml");
		m_xmlFileToUse = true;
	}

	int PreviousRunNo;
	//if (!m_pSimDoc->CDocument::OnSaveDocument(pzPathName)) return;
#ifdef COUPSTD
	 pair<int, unique_ptr<Register>> p= FUtil::GetProfileIntNo("SimulationRunNo", 1, move(reg_pointer));
	 m_DocFile.m_SimulationRunNo = p.first; reg_pointer = move(p.second);
	PreviousRunNo = 0;
#else
	PreviousRunNo = m_DocFile.m_SimulationRunNo;
	m_DocFile.m_SimulationRunNo = max(PreviousRunNo, FUtil::GetProfileIntNo("SimulationRunNo", m_DocFile.m_SimulationRunNo));
#endif // COUPSTD



	m_DocFile.m_SimulationRunNo = (m_DocFile.m_SimulationRunNo - 1) % 999999 + 1;
	reg_pointer=FUtil::WriteProfileInt("SimulationRunNo", m_DocFile.m_SimulationRunNo, move(reg_pointer));
	p= FUtil::GetProfileIntNo("SimulationRunNo", m_DocFile.m_SimulationRunNo, move(reg_pointer));
	m_DocFile.m_SimulationRunNo = p.first; reg_pointer = move(p.second);

	if (m_DocFile.m_SimulationRunNo == PreviousRunNo) {
		m_DocFile.m_SimulationRunNo++;
		reg_pointer=FUtil::WriteProfileInt("SimulationRunNo", m_DocFile.m_SimulationRunNo, move(reg_pointer));
	}
	Num = FUtil::ItoNumAscii(m_DocFile.m_SimulationRunNo);

	string stringtype = ".Sim";
	if (m_xmlFileToUse) stringtype = ".xml";
	if (iposext > 0) {
#ifdef COUPSTD
		
		newPathName = pzPathName.substr(0, iposext + 1) + Num + stringtype;
		if (iposMul == string::npos&&m_MultiRun_Array.size() > 0) {

			newPathName = pzPathName.substr(0, iposext + 1) +"Multi_"+ Num + stringtype;

		}

#else
		newPathName = pzPathName.substr(0, iposext + 1) + Num + ".Sim";
#endif

	}
	else if (ipos_sim>0)
		newPathName = pzPathName.substr(0, ipos_sim) + "_" + Num + stringtype;
	else {
		int ipos = pzPathName.size();
		newPathName = pzPathName.substr(0, ipos) + "_" + Num + stringtype;
	}

	if (p_ModelInfo->IsMultiRunning())
		MR_ResetDimCounter();

	if (m_MultiRun_Array.size() > 0) {

		m_DocFile.m_SimulationRunNo+=MR_Get_NumberofRuns();
		reg_pointer=FUtil::WriteProfileInt("SimulationRunNo", m_DocFile.m_SimulationRunNo, move(reg_pointer));

	}

	string OldOriginFileName = m_DocFile2.m_OriginFileName;
	m_DocFile2.m_OriginFileName = pzPathName;
	SetNewSimulation();
	m_Protect = false;
	m_DocFile.m_MultiStoragelocked = true;
	m_FileVersionNumberRead = 0;
	m_DocFile.m_TimeCreated = time(nullptr);
	m_DocFile.m_TimeModified = 0;
	ClearRunReportArray();
	string str;
	str=FUtil::ItoNumAscii(m_DocFile.m_SimulationRunNo);
	SetCurrentFileName(newPathName);
    
    if (!WriteDocFile()) return false;
    if (!ReadDocFile(newPathName)) return false;
    
	return true;


}

void Sim::OnCommandLineOpenNewDocument()
{

	string Num;
	string pzPathName;
	string newPathName ;
#ifndef NO_MS_GUI
  	int PreviousRunNo=m_DocFile.m_SimulationRunNo;
	if(m_CommandLine_NewRunNo>0)	{
// Change RunNumber to Command line argument
		m_DocFile.m_SimulationRunNo = m_CommandLine_NewRunNo;
	}
	else
	{
		// Run Number of New simulation from registry
		m_DocFile.m_SimulationRunNo=MFC_Util::GetProfileIntNo("SimulationRunNo", m_DocFile.m_SimulationRunNo);
	} 

	m_DocFile.m_SimulationRunNo=(m_DocFile.m_SimulationRunNo-1)%999999+1;
	MFC_Util::WriteProfileInt("SimulationRunNo",m_DocFile.m_SimulationRunNo);
	m_DocFile.m_SimulationRunNo=MFC_Util::GetProfileIntNo("SimulationRunNo", m_DocFile.m_SimulationRunNo);
	if(m_DocFile.m_SimulationRunNo==PreviousRunNo) {
		m_DocFile.m_SimulationRunNo++;
		MFC_Util::WriteProfileInt("SimulationRunNo", m_DocFile.m_SimulationRunNo);
	}

		Num=FUtil::ItoNumAscii(m_DocFile.m_SimulationRunNo);

	// flag to remember which document to run from command line
	m_CommandLine_RunNo = m_DocFile.m_SimulationRunNo;

	pzPathName=GetCurrentSimFile();
	int iposext=pzPathName.find(".Sim");
	int iposMul=pzPathName.find("_Current_Multi_");
	int iposMul2=pzPathName.find("_Multi");
	if(iposMul>0) {
		newPathName=pzPathName.substr(0,iposMul)+"_Current_Multi_"+Num+".Sim";
	}
	else if(iposMul2>0) {
		newPathName=pzPathName.substr(0,iposMul2)+"_Multi_"+Num+".Sim";
	}
   	else if (iposext==-1||!(pzPathName.substr(iposext-4,1)=="_")) {
	 newPathName=pzPathName.substr(0,pzPathName.size()-4);
	 newPathName+="_"+Num+".Sim";
	}
	else if (iposext==-1||!(pzPathName.substr(iposext-4,1)=="_")) {
	 newPathName=pzPathName.substr(0,pzPathName.size()-4);
	 newPathName+="_"+Num+".Sim";
	}
	else {
		newPathName=pzPathName.substr(0,iposext-3)+Num+".Sim";
	}
	SetNewSimulation();

	ReadDocFile(newPathName);


	SetNewSimulation();
#endif	
	return;
}
bool Sim::OnCommandLineSimStart()
{
//	string message; 
 /*
	

	int SimulationNo=1;
	m_DocFile.m_SimulationRunNo=MFC_Util::GetProfileIntNo( "SimulationRunNo", m_DocFile.m_SimulationRunNo);


		if(IsFinishedSimulation()&&!m_DocFile.m_MultiRunning) {
			return false;
//			message="Simulation already made for this object run #"+FUtil::MFC_Util::ItoAscii(pRunDoc->m_DocFile.m_SimulationRunNo) 
//			+ "\n Do you want to make a new similar run run ? ";
//			int choice=MessageBox(LPCTSTR(message),"New Run Checking",MB_YESNO|MB_ICONQUESTION|MB_TASKMODAL);
//			if (choice==IDNO) return;
		}

		ApplyOptStartValues();
		ReallocateTheWholeMap();
		ApplyOptStartValues();


//		if(m_MakeAnnimation) ViewStatus=2;
		PARAMETERS_mp_RUN_OPTIONS=RunOpt ;
		PARAMETERS_mp_RUN_NO=(m_DocFile.m_SimulationRunNo-1)%9999+1;
		pRunDoc=this;	// Set static variable to keep the link to Fortran  
		PARAMETERS_mp_RUN_OPTIONS=RunOpt ;
		CH_Checker_build();
		ClearRunReportArray();
		if(m_DocFile.m_MultiRunning) {
			string Num;
			string pzPathName=GetCurrentSimFile();
			string newPathName;
			int iposext=pzPathName.find("_Current");;
			Num=ItoNumAscii(m_DocFile.m_SimulationRunNo);

   				if (iposext==-1) {
					newPathName=pzPathName.substr(0,pzPathName.size()-4);
					newPathName+="_Multi_"+Num+".Sim";
				}
				else {
					newPathName=pzPathName.substr(0,iposext)+"_Multi_"+Num+".Sim";
				}

			if (!WriteDocFile()) return 0;
			SetPathName(newPathName);

			if(MR_UpdateDimCounter()) {
				MR_UpdateValues();
			}
			else
				return false;
		}
		// Now comes the Run_CoupModel_Background equivalent piece of code:
			SOIL_MAIN_mp_SOIL_RUN();
		
		if(PARAMETERS_mp_RUN_RES_ID==1) {
			int SimulationNo=1;
			SimulationNo=MFC_Util::GetProfileIntNo("SimulationRunNo",SimulationNo);
			bool MakeTextFile=false;
			MakeTextFile=MFC_Util::GetProfileIntNo( "MakeTextFile", MakeTextFile);
			bool MakeExcelTimeSerieFile=false;
			MakeExcelTimeSerieFile=MFC_Util::GetProfileIntNo( "MakeExcelTimeSerieFile", MakeExcelTimeSerieFile);

			SetFinishedSimulation();
			
			time_t tt;
			tt=time(nullptr);
			struct tm timeinfo;
			localtime_s(&timeinfo, &tt);
			char buffer[24];
			strftime(buffer,24, "%Y-%m-%d %H:%M:%S", &timeinfo);
			m_DocFile2.m_DateRun=buffer;
			//m_DocFile.m_DateRun=s;
			SimulationNo++;
			SimulationNo=(SimulationNo-1)%99999+1;
 			WriteProfileInt("SimulationRunNo",SimulationNo);


		}


		CH_Checker_Reset();


		// This is the end of the Run_CoupModel_Background equivalent piece of code:

	//	HWND hWnd=GetSafeHwnd();

	//	if(pDoc->m_SimulationPriority==0) 
	//		AfxBeginThread(Run_CoupModel_Background,hWnd,THREAD_PRIORITY_HIGHEST);
 	//	else if(pDoc->m_SimulationPriority==1)
	//		AfxBeginThread(Run_CoupModel_Background,hWnd,THREAD_PRIORITY_NORMAL);
	//	else
	//		AfxBeginThread(Run_CoupModel_Background,hWnd,THREAD_PRIORITY_BELOW_NORMAL);
	
		RunOpt=PARAMETERS_mp_RUN_OPTIONS;


		*/
	return true;
}
bool Sim::OnCommandLineGetCommandLine()
{

 /*
// /F = flag proceeding file name
	int iStrPos;
	int iStrPos2;
	string strSearch="/F";

	if(m_strCmdLine.find(strSearch,0)>-1)
	{
		// find index of /F sign
		iStrPos = m_strCmdLine.find(strSearch,0);

		// look for other flasg after /F
		strSearch="/";
		iStrPos2 = m_strCmdLine.find(strSearch,iStrPos+2);

		// if no flags, then read to the end of CommandLine
		if(iStrPos2 == -1)
		{
			iStrPos2 = m_strCmdLine.size();
		}
			
		// if no text behind "/F", then the filname is read from private INI file
		if(iStrPos+2<iStrPos2)
		{
			m_CommandLine_DocFileName = m_strCmdLine.substr(iStrPos+2,iStrPos2-iStrPos-2);
		}
		else
		{
			m_CommandLine_DocFileName = "No filename on commandline";
		}
	}
	// if /F is omitted, the filename is assumed to be at the end of command line
	else
	{
		// find index of last flag in commandline
		strSearch="/";
		iStrPos = m_strCmdLine.rfind("/");
		if(iStrPos == -1)
		{
			iStrPos = -2;
		}

		int	iStrPos2 = m_strCmdLine.size();
			
		if(iStrPos+2<iStrPos2)
		{
			m_CommandLine_DocFileName = m_strCmdLine.substr(iStrPos+2,iStrPos2-(iStrPos+2));
			while (m_CommandLine_DocFileName[0]==" "){
				m_CommandLine_DocFileName = m_CommandLine_DocFileName.substr(1,m_CommandLine_DocFileName.size());}
			while (m_CommandLine_DocFileName[m_CommandLine_DocFileName.size()-1]==" "){
				m_CommandLine_DocFileName = m_CommandLine_DocFileName.substr(0,m_CommandLine_DocFileName.size()-1);}
		}
		else
		{
			m_CommandLine_DocFileName = "No filename on commandline";
		}

	}

	// /R = open document and RUN
	strSearch="/R";
	if(m_strCmdLine.find(strSearch,0)>-1)
	{
		m_CommandLine_Run = true;
	}
	else
	{
		m_CommandLine_Run = false;
	}

	// /Q = quit after processing command line tasks
	strSearch="/Q";
	if(m_strCmdLine.find(strSearch,0)>-1)
	{
		m_CommandLine_Quit = true;
	}
	else
	{	
		m_CommandLine_Quit = false;
	}

	//// /U = update simulation run no (=New document)
	//strSearch="/U";
	//if(strCmdLine.Find(strSearch,0)>-1)
	//{
	//	m_CommandLine_UpdateRunNo = true;
	//}
	//else
	//{
	//	m_CommandLine_UpdateRunNo = false;
	//}

	//// /-U = do not update simulation run no
	//strSearch="/-U";
	//if(strCmdLine.Find(strSearch,0)>-1)
	//{
	//	m_CommandLine_UpdateRunNo = false;
	//}
	//else
	//{
	//	m_CommandLine_UpdateRunNo = true;
	//}

	// /N new run number
	strSearch="/N";
	if(m_strCmdLine.find(strSearch,0)>-1)
	{
		m_CommandLine_UpdateRunNo = true;
		// find index of /N sign
		iStrPos = m_strCmdLine.find(strSearch,0);

		// look for other flags after /N
		strSearch="/";
		iStrPos2 = m_strCmdLine.find(strSearch,iStrPos+2);
		
		// read new number		
		m_CommandLine_NewRunNo = AtoInt(m_strCmdLine.substr(iStrPos+2,iStrPos2-iStrPos-2));

		// Check that new run number is positive between 1 and 9999, otherwise re-set flags to false
	}
	else
	{
		m_CommandLine_NewRunNo = -1;
		m_CommandLine_UpdateRunNo = false;
	}

	// /P = change parameters in document before running simulation
	strSearch="/P";
	if(m_strCmdLine.find(strSearch,0)>-1)
	{
		m_CommandLine_ChangeParameters = true;

		// find index of /P sign
		iStrPos = m_strCmdLine.find(strSearch,0);

		// look for other flasg after /P
		strSearch="/";
		iStrPos2 = m_strCmdLine.find(strSearch,iStrPos+2);

		// if no flags, then read to the end of CommandLine
		if(iStrPos2 == -1)
		{
			iStrPos2 = m_strCmdLine.size();
		}
			
		// if no text behind "/P", then there is no parameter file name
		if(iStrPos+1<iStrPos2)
		{
			m_CommandLine_ParFileName = m_strCmdLine.substr(iStrPos+2,iStrPos2-iStrPos-2);
			// double any backslash in ParFileName
			strSearch ="\\";
			int ipos=m_CommandLine_ParFileName.find("\\");
			if ( ipos > -1 )
			{
				m_CommandLine_ParFileName.replace(ipos,1,"/");
			}
			while (m_CommandLine_ParFileName[0]==" "){
				m_CommandLine_ParFileName = m_CommandLine_ParFileName.substr(1,m_CommandLine_ParFileName.size());}
			while (m_CommandLine_ParFileName[m_CommandLine_ParFileName.size()-1]==" "){
				m_CommandLine_ParFileName = m_CommandLine_ParFileName.substr(0,m_CommandLine_ParFileName.size()-1);}
		}
		else
		{
			m_CommandLine_ParFileName = "No filename on commandline";
			m_CommandLine_ChangeParameters = false;
		}
	}
	// if /F is omitted, the filename is assumed to be at the end of command line
	else
	{
		m_CommandLine_ChangeParameters = false;
	}
	// /S = save documents after processing command line options
	strSearch="/S";
	if(m_strCmdLine.find(strSearch,0)>-1)
	{
		m_CommandLine_Save = true;
	}
	else
	{
		m_CommandLine_Save = true;
	}

return true;
}
#ifndef COUP2013
bool Sim::OnCommandLineChangeParameters()
{
	fstream cmdLineParFile;
	string	row, Parname, Parvalue;
	string	typ, grp;
	int	iStrPos, iStrPos2, iel,	nRows,ParTyp;
	string	strSearch=";";

	SimB *Sw;
	float fvalue;
	int	intvalue;
// Get the document to run
//	string	message; 
	cmdLineParFile.open(m_CommandLine_ParFileName.data(), ios::in);

	if(!cmdLineParFile.is_open()) {
		#ifndef COUPSTD
		MessageBox(nullptr,"For some reason	the	file specified couldn´t	be opened.","File Open Error",MB_OK|MB_ICONEXCLAMATION);
		#endif
	}
	else
	{
// Read	the	parameter file and change parameters accordingly
		while(cmdLineParFile.good())
		{
			// Extract Group name
			getline(cmdLineParFile, row);
			iStrPos	= row.find(strSearch,0);
			grp	= row.substr(0,iStrPos);
			// Extract Parametername
			iStrPos2 = row.find(strSearch,iStrPos+1);
			Parname	= row.substr(iStrPos+1,iStrPos2-iStrPos-1);
			iStrPos	= iStrPos2;
			// Extract Parameter value
			iStrPos2 = row.find(strSearch,iStrPos+1);
			Parvalue = row.substr(iStrPos+1,iStrPos2-iStrPos-1);
			iStrPos	= iStrPos2;
			// Extract Table index / Parameter type
//			iStrPos2 = row.Find(strSearch,iStrPos+1);
//			typ	= row.Mid(iStrPos+1,iStrPos2-iStrPos-1);
			typ	= row.substr(iStrPos+1,row.size()-iStrPos-1);
//			iStrPos	= iStrPos2;
			ParTyp = AtoInt(typ);
			switch(ParTyp){
			case -2: // File name
				typ	= "Files";
				break;
			case -1: // Switches
				typ	= "Switches";
				break;
			case 0: // Parameters
				typ	= "Parameters";
				break;
			default: //	Parameter tables
				iel	= AtoInt(typ)-1;
				if(iel<0)
					iel	= 0;
				typ	="Table Parameters";
				break;
			};
			// Get pointer to the parameter

			Sw=(SimB*)GetPtr(typ,	grp, Parname);
				
			//If the parameter doesn´t exist then give a message about that, else
			//add the parameter
			if (Sw==nullptr)
			{
				fvalue = 0.0;
			}
			//IncorrectParameterMessage(Parname,cmdLineParFile,typ,grp);
			else
			{
				switch(ParTyp){
				case -1: // "Switches":
					intvalue = AtoInt(Parvalue);
					// Set new value
					Sw->SetValue(intvalue);
					// Set new value
					Sw->Apply();
					break;
				case -2: //"Files":
					(F*)Sw->SetValue(string(Parvalue));
					(F*)Sw->Apply();
					break;
				case 0: // "Parameters":
					fvalue=AtoFloat(Parvalue);
					// Set new value
					Sw->SetValue(fvalue);
					// Set new value
					Sw->Apply();
					break;
				default: //case "Table	Parameters":
					fvalue=AtoFloat(Parvalue);
					// check number	of rows
					nRows =	((P*)Sw)->GetSize();
					// Set new value
					((P*)Sw)->SetValue(iel, fvalue);
					// Set new value
					((P*)Sw)->Apply(nRows);
				break;
				};
			}
			continue;
		}
	}
	cmdLineParFile.close();	  */
	return true;
}

#ifndef COUPSTD
extern "C" int updateprog()
{
	return 0;
}
extern "C" void report(char* str)
{
	string newCString=str;
	if(pRunDoc!=nullptr) {
		pRunDoc->SetRunReportArray(newCString);
	//	m_pView->Invalidate();
	}
}

#endif
