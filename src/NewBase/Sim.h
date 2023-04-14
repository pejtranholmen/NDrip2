#pragma once

#include "../ModelTypes/SimB.h"
#include "./NewMap.h"
#include "./Link.h"
#include "../Simulator/Simulator.h"

class Sim :public NewMap
{

public:
	Sim();
	~Sim();
	
	void ClearRunReportArray();
	size_t AddToRunReport(string str);
	size_t m_SimulationPriority;
	string GetCurrentSubDirectory();
	string GetCurrentSubDirectoryFile();
	void SetCurrentName_SimNo(int simno);
	void SetCurrentFileName(string);

	bool OnCommandLine(string str) {return false;};

	bool OnSimStart();
	bool FixSumIndex();
	bool OnSimMultiStart();

	bool m_CommandLine_Run;
	bool m_CommandLine_Quit;
	bool m_CommandLine_UpdateRunNo;
	bool m_CommandLine_ChangeParameters;
	bool m_CommandLine_Save;
	int m_CommandLine_RunNo;
	int m_CommandLine_NewRunNo;
	string m_CommandLine_DocFileName;
	string m_CommandLine_ParFileName;
#ifndef COUPSTD
	Link m_Link;
#else 
	pair<bool, unique_ptr<Register>> MakeSingleRun(bool DB_Source=false, int pkey=-1, unique_ptr<Register> pReg=nullptr);
	unique_ptr<Register> SetNewRunNo(bool DB_Source = false, int pkey = -1, bool ChildDocument=false, unique_ptr<Register> pReg = nullptr);
	pair<bool, unique_ptr<Register>> MakeMultiRun(bool DB_Source = false, int pkey = -1, unique_ptr<Register> pReg = nullptr);
	pair<bool, unique_ptr<Register>> RunModel_Using_Postgres(int pkey, bool makeChildDocument, unique_ptr<Register> );
	
#endif
	Simulator m_Simulator;
	string  m_UserSubDirectory;
	string m_OrgPathName;
	
	bool CheckAndUpdateFileName(bool MultiRun=false, bool DB_Source=false);
	bool CreateNewDocFromCurrentDoc();

protected:
	time_t m_SingleStartTime,m_MultiStartTime;
	
	void SetCurrentSubDirectory(string);
	


	string m_strCmdLine;
	bool OnCommandLineGetCommandLine();
	bool OnCommandLineSimStart();
	void OnCommandLineOpenNewDocument();
	bool OnCommandLineChangeParameters();
};
