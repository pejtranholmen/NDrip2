/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*=-=-=-=-=-                  FILE: NewMAP.H                   -=-=-=-=-=*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#pragma once

#include "./MR.h"
#ifndef COUPSTD
#include "../Chart/PlotDesignInfo/PlotStorage.h"
#endif
#include "../Util/pugixml.hpp"
enum class doc_enabled { ALL, ENABLED, NOT_DEFAULT };


class NewMap :public MR{

public:
	bool MakeMapComplete();
	bool UpdateFilterTags(bool with_activefilter);
	string WriteEntireModelToXmlFile(doc_enabled enable_level = doc_enabled::ALL);
	bool WriteHeaderToXmlFile(pugi::xml_node node);
	bool Read_Header_FromXmlFile(pugi::xml_node node);
	bool WriteValidationToXmlFile(pugi::xml_node node);
	bool Read_Validation_FromXmlFile(pugi::xml_node node);
	bool WriteMultiRunSettingsToXmlFile(pugi::xml_node node);
	bool Read_MultiRunSettings_FromXmlFile(pugi::xml_node node);
	bool WriteDynamicParToXmlFile(pugi::xml_node node);
	bool Read_DynamicPar_FromXmlFile(pugi::xml_node node);
	bool WriteHistoryToXmlFile(pugi::xml_node node);
	bool WriteAllFinalStateToXmlFile(pugi::xml_node node);
	bool Read_AllFinalState_FromXmlFile(pugi::xml_node node);

	bool Read_History_FromXmlFile(pugi::xml_node node);
	bool Read_RunReport_FromXmlFile(pugi::xml_node node);
	bool WriteRunReportToXmlFile(pugi::xml_node node);
	bool WriteOutputSummaryToXmlFile(pugi::xml_node node);
	bool Read_OutputSummary_FromXmlFile(pugi::xml_node node);
	bool WriteDataBaseLinksToXmlFile(pugi::xml_node node);
	bool Read_DataBaseLinks_FromXmlFile(pugi::xml_node node);



	bool Read_SimB_FromXmlFile(pugi::xml_node node);
	bool WriteSimB_ToXmlFile(pugi::xml_node node, simtype, doc_enabled def = doc_enabled::ALL);

	bool WriteDoc_To_Postgres();
	bool ReDefinePostgresDataBase();
	bool SelectDoc_From_Postgres(int pkey);
	vector<pair<int, string>> GetDataBaseSimulations();

	bool CheckStart_EndTime();
	void UpdateArchiveLists(size_t icat=8);
	void RemoveOriginalValues(string typ, string GroupSet, bool original=true, size_t ViewCategory=0);
	void SetOriginalValues(string typ, string GroupSet="") {RemoveOriginalValues(typ, GroupSet,true);};
	void CleanAllLists();
	bool Sort_TableList_Arrays(bool) {return true;};
	NewMap();						// standard constructor
	~NewMap();						// standard constructor
			// updates all variable interactions
	void SetPathName(string filename) {m_DocFileName=filename; m_CurrentFile=filename;};
	bool ReadDocFile(string filename="");
	bool WriteDocFile();
	bool ReadXmlDocFile(string filename);


	void Init_BlankDocument();
	bool CreateMapSvFlexFile(size_t index=0);

	void  Export_MBinToCSV(bool accepted);
	string Export_XLSX(bool accepted, size_t choice=0, int IndexOut=-1, string varname="");

	bool Info_Header(bool ReadFlag=true);
	bool Info_Switches(bool ReadFlag=true);
	bool Info_Parameters(bool ReadFlag=true);
	bool Info_ParTables(bool ReadFlag=true);
	bool Info_Files(bool ReadFlag=true);
	bool Info_Outputs(bool ReadFlag=true);
	bool Info_Validation(bool ReadFlag=true);
	bool Info_SumValidation(bool ReadFlag=true);
	bool Info_RunReport(bool ReadFlag=true);
	bool Info_MultiRun(bool ReadFlag=true);
	bool Info_WithinRunOptions(bool ReadFlag=true);
	bool Info_FinalStateVariables(bool ReadFlag=true);
	bool Info_Plottings(bool ReadFlag=true);
	bool Info_History(bool ReadFlag=true);
	bool Info_DataBase(bool ReadFlag=true);


	ValidationData* GetValidDataPtr() {return &m_ValidationData;};

	bool Info_Annim(bool ReadFlag=true);
	string Sim2XLSX(bool ReadFlag = true, bool AllItems=false);
	
	bool ID_MapsForPostgresReady = false;
	void SetLocalHost(bool status);
	bool GetLocalHost();

#ifndef COUPSTD
	CPlotStorage m_PlotStorage;
#endif

protected:

	vector<SIMB> UpdateOutVector(vector<SimB*> vp,string GroupSet, bool original, int ViewCategory=0);
	bool m_continueRead;
	size_t m_iError;
	vector<SIMB> SortSimB_ByGroup(vector<SIMB> v);
	vector<SIMB> SortSimB_ByCategory(vector<SIMB> v);

private:
	void AssignNewValue_toPs(Ps* pPs, double newvalue);
	bool AssignNewValue_toP(P* pP, size_t row, double newvalue, size_t &ReCalc);

};



