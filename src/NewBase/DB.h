#pragma once

//#include ".\Base.h"
#include "./History.h"



class DB :public History
{

public:
	DB();
	bool Retrieve(string group, string key, bool NoApply=false);
	bool Read_DB_DocFile(string file, string key, bool Apply=false, bool ExportToCSV=false);
	bool Write_DB_DocFile(string file);
	bool Apply_DB_DocFile(string group, string key="", bool during=false);
	bool Apply_DB_DocFile(vector<Sw*> AppliedSw, vector<Ps*> AppliedPs, vector<Tab*> Applied_Tab, vector<F*> Applied_File);
	bool Export_DB_CSVFile(vector<Sw*> AppliedSw, vector<Ps*> AppliedPs, vector<Tab*> Applied_Tab, vector<F*> Applied_File);
	bool Apply_DB_Struc(string);
	void Actual_DB(string group="");

	bool ReadFrom_DB_Object(string ID_DB_Group, string key="", bool deletefile=false, bool NoApply=false, bool ExportToCSV=false);
	void SaveTo_DB_Object(string, string key);

	void SetGroupConnections(string Group);
	void Reset();
	bool m_ShortTimeRead;
	string m_Group;
	string m_DB_key;
	bool m_DuringRun;
	string m_SimFileName;
	Sw *m_pSwTimeRes;
	Sw *m_pSwPlantType;
	Sw *m_pSwLAI, *m_pSwRoot, *m_pSwGen1, *m_pSwGen2;
	Sw *m_pSw_NC;
	F *m_pFile;
	bool m_HideAllTableContents;

	string m_ID_DB_Group;
	string m_SelectedId;
	string m_SelectedKey;

	vector<Ps*>m_P_Array;
	vector<P*>m_Pt_Array;
	vector<Tab*>m_Tab_Array;
	vector<F*>m_F_Array;
	vector<Sw*>m_Sw_Array;
	vector<FCSV*>m_FCSV_Array;

protected:
	
	int m_Read_DB_Version;
	bool Info_DB_Switches(bool reading, int iv=0);
	vector<Sw*> ReadWrite_DB_Switches(bool reading, int iv = 0);
	bool Info_DB_Parameters(bool reading, int iv=0);
	vector<Ps*> ReadWrite_DB_Parameters(bool reading, int iv = 0);
	bool Info_DB_ParTables(bool reading, int iv=0);
	vector<Tab*> ReadWrite_DB_ParTables(bool reading, int iv = 0);
	bool Info_DB_Files(bool reading, int iv=0);
	vector<F*> ReadWrite_DB_Files(bool reading, int iv = 0);

	ifstream m_DocFileIn;
	ofstream m_DocFileOut; 
	
	string CreateFlagString(SimB* pBase, bool GDType, int FlagValue);
	bool wString(string *str);
	bool wInt(int *iv);
	bool wSize_t(size_t* iv);
	bool wUint(unsigned int * iv);
	bool wbool(bool *iv);
	bool wFloat(float *f);
	bool wDouble(double *f);
	bool wTime(time_t *time);
	
	bool rString(string *str);
	bool rInt(int *iv);
	bool rUint(unsigned int * iv);
	bool rSize_t(size_t* iv);
	bool rBOOL(bool *iv);
	bool rbool(bool *iv);
	bool rFloat(float *f);
	bool rDouble(double *f);
	bool rTime(time_t *time);
	char m_cbuf[400];




};