#include "NewMap.h"
#include "../Util/FUtil.hpp"

#define SECS_TO_FT_MULT 10000000
#define TIMBASE_SHIFT 11644473600

//#include "../External.h"
//#include "../Util/
#include <nlohmann/json.hpp>
#include "./NewMap.h"
#include "../ModelTypes/NewModelType.h"
#include "../SoilData/GetSoilStorage.h"
#include "../SoilData/PlotPf/PFCurve.h"
#include "../Util/CoupSim_XML.h"
#include "../Util/CoupModel_DB_Link.hpp"
#include "../PG/PG.H"

#ifndef COUPSTD


#include "../MainFrm.h"
#include "../SP/SP_Retrieve.h"

auto getexcel_col = [](int no) {
	auto newcol = xlnt::fill::solid(xlnt::rgb_color(255, 255, 255));
	switch (no) {
	case 1:
		newcol = xlnt::fill::solid(xlnt::rgb_color(230, 248, 255));
		break;
	case 2:
		newcol = xlnt::fill::solid(xlnt::rgb_color(150, 255, 150));
		break;
	case 3:
		newcol = xlnt::fill::solid(xlnt::rgb_color(255, 200, 150));
		break;
	case 4:
		newcol = xlnt::fill::solid(xlnt::rgb_color(100, 200, 255));
		break;
	case 5:
		newcol = xlnt::fill::solid(xlnt::rgb_color(100, 205, 100));
		break;
	case 6:
		newcol = xlnt::fill::solid(xlnt::rgb_color(205, 205, 100));
		break;
	case 7:
		newcol = xlnt::fill::solid(xlnt::rgb_color(255, 255, 255));
		break;
	case 8:
		newcol = xlnt::fill::solid(xlnt::rgb_color(245, 245, 245));
		break;
	}
	return newcol;
};

auto getexcel_yellow = xlnt::fill::solid(xlnt::rgb_color(255, 255, 105));
auto getexcel_grey = xlnt::fill::solid(xlnt::rgb_color(240, 240, 240));
auto getexcel_changed = xlnt::fill::solid(xlnt::rgb_color(255, 137, 137));


#else
#include <../PG/PGUtil.h>

#endif


extern "C" bool ReadResultsFromFile=true;


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*=-=-=-=-=-                  CLASS: NewMap                   -=-=-=-=-=*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
NewMap::NewMap() {

	m_DocFileName="Kalle.Sim";
	//PARAMETERS_mp_SUPERFLAG=0;
	m_pCommonModelInfo=p_ModelInfo;

	m_Protect=false;
	m_SavedSum=false;
	m_SavedTimeSerie=false;
	//PARAMETERS_mp_PRESIMULATIONDAYS=0;
	m_BayesianStepSize=0.5f;
	m_NumOutputs=0;//PARAMETERS_mp_NUM_OUTPUTS;
	MR_UseBayesianCoVar=false;

	m_MultiStopped=false;
	m_CurrentSubDirectory="";

	DB_Names.clear();
	DB_Names.push_back("Plant");
	DB_Names.push_back("Soil Properties");
	DB_Names.push_back("Soil evaporation");
	DB_Names.push_back("Snow");
	DB_Names.push_back("Soil frost");
	DB_Names.push_back("Soil water flows");
	DB_Names.push_back("Plant Growth");
	DB_Names.push_back("Site Management");
	DB_Names.push_back("Soil Organic Processes");
	DB_Names.push_back("Soil Mineral N Processes");
	MR_AddExtraMonitor(4);

	m_pValidIndexSet=nullptr;
	m_ReReadMStorageFile=false;
	for(size_t i=0; i<MAXSIMVAL;i++)
		m_ValidationData.m_NumberSelectedValidationVar[i]=0;
	
	m_OutputBinFileExist=true;
	m_OutputValidBinFileExist=true;
	m_MultiStorageBinFileExist=true;
	m_OutputValidXBinFileExist=false;

	m_NoUpdateRunNo=false;
	m_MultiStoragelocked=true;
	m_LinkedDocNumber=1;
	m_MultiRunsOriginal=0;
	m_CountError=0;
	m_iError=0;
	for (size_t i = 0; i < m_ResidualFileVector.size(); i++) {
		for (size_t ii = 0; ii < m_ResidualFileVector[i].size(); ii++)
			m_ResidualFileVector[i][ii]->InitDoc(this, i + 1);
	}
	m_ErrorCount=0;

	ReadResultsFromFile=true;

}

//Destructor for NewMap
//The deletion of the dynamic objects created is taken care of here.
NewMap::~NewMap() 
{
  //Call to functions for deletion of the dynamic pointers in the pointermaps

	string str=m_DocFile2.m_OriginFileName;



}
bool NewMap::SetLocalCredentials(string dbname, string user, string password, int port) {
	return checkLocalCredentials(dbname, user, password, port);
}

bool NewMap::SetRemoteCredentials(string dbname, string user, string password, string host) {
	return checkRemoteCredentials(dbname, user, password, host);
}

void NewMap::SetLocalHost(bool value) {
	coup_pg::LocalHost = value;
}
bool NewMap::GetLocalHost() {
	return coup_pg::LocalHost;
}
void NewMap::SetDB_Action(size_t value) {
	coup_pg::db_action = DB_RUN_ACTION(value);
}
size_t NewMap::GetDB_Action() {
	return size_t(coup_pg::db_action);
};
void NewMap::SetMakeMultiRunIfPossible(bool status) {
	coup_pg::MakeMultiRunIfPossible = status;
}
bool NewMap::GetMultiRunIfPossible() {
	return coup_pg::MakeMultiRunIfPossible;
}


string NewMap::WriteEntireModelToXmlFile(doc_enabled enable_level, string localdirectory)
{
	if (m_pCommonModelInfo == nullptr) {
		auto pP = GetPPointer("Upper Depth");
		if(pP!=nullptr)	m_pCommonModelInfo = pP->getCommonModelPointer();
	}

	if (m_pCommonModelInfo == nullptr) return "";
	string DocName;
	string filein;
	auto GetFileName = [&]() {
		string fileout;
		auto ip = DocName.find(".Sim");
		if (ip == string::npos) ip = DocName.find(".xml");
		if (ip != string::npos) {
			fileout = DocName.substr(0, ip);
			if (enable_level==doc_enabled::ALL) fileout += "_All";
			else if (enable_level == doc_enabled::ENABLED) fileout += "_Enabled";
			//else if (enable_level == doc_enabled::NOT_DEFAULT) fileout += "_All";
			fileout += ".xml";
			if (localdirectory.size() > 0) {
				fileout = localdirectory + FUtil::FileNameOnly(fileout);
			}

		}
		return fileout;
	};

	pugi::xml_document doc;
	doc.reset();

	pugi::xml_node node = doc.append_child("CoupModel");

	// add description node with text child
	pugi::xml_node descr = node.append_child("Run_Info");
	pugi::xml_node database= node.append_child("Local_Database_Links");

	pugi::xml_node switches = node.append_child("Model_Options");
	pugi::xml_node parameters = node.append_child("Parameters");
	pugi::xml_node parameters_arrays = node.append_child("Parameters_Arrays");

	pugi::xml_node outputs = node.append_child("Output_Variables");
	pugi::xml_node outputs_Xs = outputs.append_child("State");
	pugi::xml_node outputs_Ts = outputs.append_child("Flow");
	pugi::xml_node outputs_Gs = outputs.append_child("Auxiliary");
	pugi::xml_node outputs_Ds = outputs.append_child("Driving");
	pugi::xml_node outputs_X = outputs.append_child("State_Arrays");
	pugi::xml_node outputs_T = outputs.append_child("Flow_Arrays");
	pugi::xml_node outputs_G = outputs.append_child("Auxiliary_Arrays");
	pugi::xml_node outputs_D = outputs.append_child("Driving_Arrays");
	pugi::xml_node Results_Summary = node.append_child("Output_Summary");

	pugi::xml_node model_files = node.append_child("Model_Files");
	pugi::xml_node validation = node.append_child("Validation");
	pugi::xml_node dynamic_par = node.append_child("Dynamic_Parameters");
	pugi::xml_node multi_run = node.append_child("MultiRun_Settings");
	pugi::xml_node allfinalstate = node.append_child("AllFinalState");
	pugi::xml_node history_list = node.append_child("History");
	pugi::xml_node run_report = node.append_child("Run_Report"); 


	WriteHeaderToXmlFile(descr);

	WriteSimB_ToXmlFile(switches, simtype(SWITCH), enable_level);
	WriteSimB_ToXmlFile(parameters, simtype(PAR_SINGLE), enable_level);
	WriteSimB_ToXmlFile(parameters_arrays, simtype(PAR_TABLE), enable_level);
	WriteSimB_ToXmlFile(outputs_Xs, simtype(STATE_SINGLE), enable_level);
	WriteSimB_ToXmlFile(outputs_Ts, simtype(FLOW_SINGLE), enable_level);
	WriteSimB_ToXmlFile(outputs_Gs, simtype(AUX_SINGLE), enable_level);
	WriteSimB_ToXmlFile(outputs_Ds, simtype(DRIVE_SINGLE), enable_level);
	WriteSimB_ToXmlFile(outputs_X, simtype(STATE), enable_level);
	WriteSimB_ToXmlFile(outputs_T, simtype(FLOW), enable_level);
	WriteSimB_ToXmlFile(outputs_G, simtype(AUX), enable_level);
	WriteSimB_ToXmlFile(outputs_D, simtype(DRIVE), enable_level);
	WriteSimB_ToXmlFile(model_files, simtype(PGFILE), enable_level);

	WriteValidationToXmlFile(validation);
	WriteMultiRunSettingsToXmlFile(multi_run);
	WriteDynamicParToXmlFile(dynamic_par);
	WriteHistoryToXmlFile(history_list);
	WriteAllFinalStateToXmlFile(allfinalstate);
	WriteRunReportToXmlFile(run_report);
	if(IsFinishedSimulation()) WriteOutputSummaryToXmlFile(Results_Summary);
	WriteDataBaseLinksToXmlFile(database);

	DocName = m_DocFileName;

	auto filename = GetFileName();

	doc.save_file(filename.c_str());
	return filename;
}
bool NewMap::WriteSimB_ToXmlFile(pugi::xml_node node, simtype type, doc_enabled _doc_enabled) {

	pugi::xml_node modelstruc, modelfacilities, modelcat[8];
	vector<SimB*> v_simb, v_all;
	bool hit_made = false;

	auto set_switch = [&](size_t group_cat_no, size_t groupno, size_t catno) {
		string group_cat_str = m_pCommonModelInfo->GetGroupCategoryNames(group_cat_no);
		m_pCommonModelInfo->trim_xml(group_cat_str);
		v_all.clear();
		if (m_pCommonModelInfo->GetGroupCategoryNo(groupno) == group_cat_no) {
			v_simb = GetPtrVector(type, groupno, true);
			if (type == PAR_TABLE) {
				bool Original = true;
				for (size_t i = 0; i < v_simb.size(); i++) {
					P* pP = (P*)v_simb[i];
					if (pP->IsEnabled()) {
						for (size_t ii = 0; ii < pP->GetSize(); ii++)
							if (pP->IsNotOriginalValue(ii)) Original = false;
						if (pP->GetSize() != pP->GetOriginalSize()) Original = false;
					}
					if (!Original) pP->SetNotOriginalValue();
				}
			}
			for (auto it = v_simb.begin(); it != v_simb.end(); it++) {
				if ((*it)->IsEnabled() && _doc_enabled == doc_enabled::ENABLED)
					v_all.push_back(*it);
				else if ((*it)->IsNotOriginalValue() && _doc_enabled == doc_enabled::NOT_DEFAULT&& (type== SWITCH|| type == PAR_SINGLE || type== PAR_TABLE))
					v_all.push_back(*it);
				else if (_doc_enabled == doc_enabled::ALL)
					v_all.push_back(*it);
				else if ((*it)->IsEnabled()) {
					if (type >= STATE_SINGLE && type <= DRIVE_SINGLE && _doc_enabled == doc_enabled::NOT_DEFAULT)
					v_all.push_back(*it);
					else if (type >= STATE && type <= DRIVE && _doc_enabled == doc_enabled::NOT_DEFAULT )
					v_all.push_back(*it);
					else if (type == PGFILE && _doc_enabled == doc_enabled::NOT_DEFAULT )
					v_all.push_back(*it);
				}

			}
		}
		if (v_all.size() > 0 && !hit_made) {
			modelcat[catno] = node.append_child(group_cat_str.c_str());
			hit_made = true;
		}
		string groupname = m_pCommonModelInfo->GetGroupNames(groupno);
		m_pCommonModelInfo->trim_xml(groupname);
		if (v_all.size() > 0) {
			auto group_node = modelcat[catno].append_child(groupname.c_str());
			for (size_t isw = 0; isw < v_all.size(); isw++) {
				auto pSimB = v_all[isw];
				string swname = v_all[isw]->GetName();
				m_pCommonModelInfo->trim_xml(swname);
				auto n = group_node.append_child(swname.c_str());
				if (type == SWITCH) {
					Sw* pSw = static_cast<Sw*>(v_all[isw]);
					n.append_attribute("Current") = int(pSw->GetIntValue());
					n.append_attribute("Default") = int(pSw->GetOriginalIntValue());
					for (size_t i = 0; i < pSw->GetNumOptions(); i++) {
						string opt_no;
						opt_no = "Opt_" + FUtil::STD_ItoAscii(i);
						n.append_attribute(opt_no.c_str()) = pSw->GetOption(i).c_str();
					}
				}
				else if (type == PAR_SINGLE) {
					Ps* pPs = static_cast<Ps*>(v_all[isw]);
					n.append_attribute("Current") = pPs->GetValue();
					n.append_attribute("Default") = pPs->GetOriginalValue();
					n.append_attribute("Min") = pPs->GetMinValue();
					n.append_attribute("Max") = pPs->GetMaxValue();

				}
				else if (type == PAR_TABLE) {
					P* pP = static_cast<P*>(v_all[isw]);
					n.append_attribute("SizeofArray") = pP->GetSize();
					NE* pNE = pP->GetNEPointer();
					auto str = pNE->GetName();
					m_pCommonModelInfo->trim_xml(str);
					n.append_attribute("ArrayName") = str.c_str();




					for (size_t i = 0; i < pP->GetSize(); i++) {
						string index;
						index = "Index_" + FUtil::STD_ItoAscii(i + 1);
						auto narray = n.append_child(index.c_str());
						narray.append_attribute("value") = pP->GetValue(i);				
						narray.append_attribute("default") = pP->GetOriginalValue(i);
						narray.append_attribute("min") = pP->GetMinValue(i);
						narray.append_attribute("max") = pP->GetMaxValue(i);
					}

				}
				else if (type >= STATE_SINGLE && type <= DRIVE_SINGLE) {
					OutSingle* pO = static_cast<OutSingle*>(v_all[isw]);
					n.append_attribute("OutSingle") = pO->GetStoreFlag();
					n.append_attribute("OutMulti") = pO->GetMultiStoreFlag();
				}
				else if (type >= STATE && type <= DRIVE) {
					OutVector* pO = static_cast<OutVector*>(v_all[isw]);
					n.append_attribute("SizeofArray") = pO->GetNumberOfFlags();
					NE* pNE = pO->GetNEPointer();
					auto str = pNE->GetName();
					m_pCommonModelInfo->trim_xml(str);
					n.append_attribute("ArrayName") = str.c_str();

					for (size_t i = 0; i < pO->GetNumberOfFlags(); i++) {
						string index;
						index = "Index_" + FUtil::STD_ItoAscii(i + 1);
						auto n_array = n.append_child(index.c_str());
						n_array.append_attribute("SingleRunFlag") = pO->GetStoreFlag(i);
						n_array.append_attribute("MultiRunFlag") = pO->GetMultiStoreFlag(i);
					}
				}
				else if (type == PGFILE) {
					F* pF = static_cast<F*>(v_all[isw]);
					string str = pF->GetStrValue();
					if (str.find('.') == string::npos) str += ".Bin";
					FUtil::trim_xmlstring(str);
					n.append_attribute("PG_File") = str.c_str();
				}
				if (type == PAR_SINGLE) {
					auto test = pSimB->GetUnit();
					m_pCommonModelInfo->trim_xml(test);
				}

				size_t icount = 0;
				while (pSimB->GetCondition(icount) != "") {
					string cond_no;
					cond_no = "Condition_" + FUtil::STD_ItoAscii(icount + 1);
					n.append_attribute(cond_no.c_str()) = pSimB->GetCondition(icount).c_str();
					icount++;
				}

			}
		}
	};

	// Model Structure
	set_switch(7, 2, 6);
	set_switch(7, 4, 6);
	set_switch(7, 23, 6);
	set_switch(7, 28, 6);
	set_switch(7, 31, 6);

	// Model processes
	for (size_t i = 1; i <= 6; i++) {
		hit_made = false;
		for (size_t groupno = 0; groupno < 38; groupno++) {
			set_switch(i, groupno, i - 1);
		}
	}
	// Technical
	hit_made = false;
	for (size_t groupno = 0; groupno < 38; groupno++) {						   
		set_switch(8, groupno, 7);
	}
	return false;
}
bool NewMap::Read_SimB_FromXmlFile(pugi::xml_node node, bool Reset_to_Default) {

	string value_to_assign = "Current";


	if (Reset_to_Default) value_to_assign = "Default";
	
	simtype type{ simtype::SWITCH };
	string typ_name = node.name();

	if (typ_name == "Model_Options") {
		type = simtype::SWITCH;
		m_Sw_Array.clear();
	}
	else if (typ_name == "Parameters") {
		m_P_Array.clear();
		type = simtype::PAR_SINGLE;
	}
	else if (typ_name == "Parameters_Arrays") {
		type = simtype::PAR_TABLE;
		m_Tab_Array.clear();
	}
	else if (typ_name == "State") 	type = simtype::STATE_SINGLE;
	else if (typ_name == "Flow") 	type = simtype::FLOW_SINGLE;
	else if (typ_name == "Auxiliary") 	type = simtype::AUX_SINGLE;
	else if (typ_name == "Driving") 	type = simtype::DRIVE_SINGLE;
	else if (typ_name == "State_Arrays") 	type = simtype::STATE;
	else if (typ_name == "Flow_Arrays") 	type = simtype::FLOW;
	else if (typ_name == "Auxiliary_Arrays") 	type = simtype::AUX;
	else if (typ_name == "Driving_Arrays") 	type = simtype::DRIVE;
	else if (typ_name == "Model_Files") {
		m_F_Array.clear();
		type = simtype::PGFILE;
	}


	
		for (pugi::xml_node category = node.first_child(); category; category = category.next_sibling())
		{
			auto catname = category.name();
			for (pugi::xml_node group = category.first_child(); group; group = group.next_sibling()) {
				auto groupname = group.name();
				for (pugi::xml_node item = group.first_child(); item; item = item.next_sibling()) {
					string item_name = string(item.name());
					
					if (type == simtype::SWITCH) {
						auto pSw = static_cast<Sw*>(GetPtr(type, item_name));
						if (pSw == nullptr) {
							item_name = m_pCommonModelInfo->getOrigName(item_name);
							pSw = static_cast<Sw*>(GetPtr(type, item_name));
						}
						if (pSw != nullptr) {
							auto value = item.attribute(value_to_assign.c_str()).as_uint();
							size_t recalc = pSw->SetIntValue(value);
							if (recalc > 0) LinkedChangeToSwitches(pSw, recalc);
						
							m_Sw_Array.push_back(pSw);
						}
						else {
							int problem = 0;
						}
					}
					else if (type == simtype::PAR_SINGLE) {
						auto pPs = static_cast<Ps*>(GetPtr(type, item_name));
						if (pPs == nullptr) {
							item_name = m_pCommonModelInfo->getOrigName(item_name);
							pPs = static_cast<Ps*>(GetPtr(type, item_name));
						}
						if (pPs != nullptr) {
							auto value = item.attribute(value_to_assign.c_str()).as_float();
							AssignNewValue_toPs(pPs, value);
						}
						else {
							int problem = 0;
						}
					}
					else if (type == simtype::PAR_TABLE) {
						auto pP = (P*)GetPtr(type, item_name);
						if (pP == nullptr) {
							item_name = m_pCommonModelInfo->getOrigName(item_name);
							pP = (P*)GetPtr(type, item_name);
						}
						if (pP != nullptr) {
							auto NumElem = item.attribute("SizeofArray").as_uint();
							auto OldNumElem = pP->GetSize();
							if (NumElem != OldNumElem) pP->SetSize(NumElem);
							size_t count = 0;
							size_t ReCalcTot = 0;
							for (pugi::xml_node item2 = item.first_child(); item2; item2 = item2.next_sibling()) {
								size_t ReCalc = 0;
								auto value = item2.attribute("value").as_double();
								bool newvalid = AssignNewValue_toP(pP, count, value, ReCalc);
								if (ReCalc > 0) ReCalcTot = ReCalc;
								count++;
							}
							if (ReCalcTot > 0) LinkedChangeToParameters(pP, ReCalcTot);
						}
						else {
							int problem = 0;
						}
					}
					else if (type == simtype::STATE_SINGLE|| type == simtype::FLOW_SINGLE||type == simtype::AUX_SINGLE||type==simtype::DRIVE_SINGLE) {
						auto pXs = static_cast<OutSingle*>(GetPtr(type, item_name));
						if (pXs == nullptr) {
							item_name = m_pCommonModelInfo->getOrigName(item_name);
							pXs = static_cast<OutSingle*>(GetPtr(type, item_name));
						}
						if (pXs != nullptr) {
							auto store= item.attribute("OutSingle").as_int();
							pXs->SetStoreFlag(store);
							auto store2= item.attribute("OutMulti").as_bool();
							pXs->SetMultiStoreFlag(store2);
						}
						else {
							int problem = 0;
						}
					}
					else if (type == simtype::STATE || type == simtype::FLOW || type == simtype::AUX || type == simtype::DRIVE) {
						auto pX = static_cast<OutVector*>(GetPtr(type, item_name));
						if (pX == nullptr) {
							item_name = m_pCommonModelInfo->getOrigName(item_name);
							pX = static_cast<OutVector*>(GetPtr(type, item_name));
						}
						if (pX != nullptr) {
							auto NumElem = item.attribute("SizeofArray").as_uint();
							pugi::xml_attribute attr = item.first_attribute();
							attr = attr.next_attribute();
							size_t count = 0;
							for (pugi::xml_node item2 = item.first_child(); item2; item2 = item2.next_sibling()) {
								auto store = item2.attribute("SingleRunFlag").as_int();
								pX->SetStoreFlag(count, store);
								auto store2 = item2.attribute("MultiRunFlag").as_int();
								pX->SetMultiStoreFlag(count, store2);
								count++;
							}
						}
						else {
							int problem = 0;
						}
					}
					else if (type == simtype::PGFILE) {
						auto pF = static_cast<F*>(GetPtr(type, item_name));

						if (pF==nullptr&&item_name.find("ValidationFile") == 0) {
							auto nc = item_name.size();
							string num = item_name.substr(14);
							if (num.size() == 1) item_name = item_name.substr(0, 14) + "_0" + num;
							else if (num.size() == 2) item_name = item_name.substr(0, 14) + "_" + num;
							pF = static_cast<F*>(GetPtr(type, item_name));
						}
 						

						if (pF == nullptr) {
							item_name = m_pCommonModelInfo->getOrigName(item_name);
							SimB* pSimB = GetPtr(type, item_name);
							pF = static_cast<F*>(pSimB);
							
						}
						if (pF != nullptr) {
							string str_value = item.attribute("PG_File").as_string();
							pF->SetValue(str_value);
							if (!pF->CheckFileNameAndAssignNameToPGClass()) {
								if (item_name == "Output File") {
										str_value = GetOutputFileName(0).c_str();
										pF->SetValue(str_value);
										pF->CheckFileNameAndAssignNameToPGClass();
								}
								else {
									string fileonly;
									string str;
									auto pos = str_value.rfind('\\');
									fileonly = str_value.substr(pos + 1);
									str_value = m_CurrentDirectory + fileonly;
								}
								pF->SetValue(str_value);
								cout << str_value << endl;
							};
							m_F_Array.push_back(pF);
						}
						else {
							int problem = 0;
						}
					}
				}
			}
		}
		if (type == PAR_TABLE) {
			RemoveOriginalValues("Tables", "ALL", true);
			RemoveOriginalValues("Tables", "ALL", false);
		}
		else if (type == SWITCH) {
			RemoveOriginalValues("Switches", "ALL", false);
			m_pValidIndexSet = dynamic_cast<Sw*>(GetPtr(SWITCH, "IndexForValidation"));
			if (m_pValidIndexSet != nullptr) {
				auto iv = m_pValidIndexSet->GetIntValue();
				m_pValidIndex = dynamic_cast<Ps*>(GetPtr(PAR_SINGLE, "Index in val file"));
				m_pValidIndexRep = dynamic_cast<Ps*>(GetPtr(PAR_SINGLE, "No of internal index loop"));
				m_pValidIndexLoopSize = dynamic_cast<Ps*>(GetPtr(PAR_SINGLE, "Size of internal loop"));

				auto pSw = dynamic_cast<Sw*>(GetPtr(SWITCH, "ValidationMode"));
				m_NumValidFiles = pSw->GetIntValue();
				ResidualFile_AddFile(m_NumValidFiles);
			}



		}
	return true;
}
bool NewMap::WriteHeaderToXmlFile(pugi::xml_node node) {


	pugi::xml_node general, sim_period, time_resolution, numerical, additional;

	general = node.append_child("General_Information");
	sim_period = node.append_child("Simulation_Period");
	time_resolution = node.append_child("Time_Resolution");
	numerical = node.append_child("Numerical_Options");
	additional = node.append_child("Additional_Details");

	general.append_attribute("FileVersionNumber") = int(m_DocFile.m_FileVersionNumber);
	const char* test = m_DocFile2.m_OriginFileName.c_str();
	general.append_attribute("OriginalFileName") = test;
	general.append_attribute("ChildDocument") = m_ChildDocument;

	string ExeDate;
	ExeDate = "Version 6.2.2, ";
#ifndef COUPSTD
	CWinApp* pApp;
	pApp = AfxGetApp();
	CFileStatus status;
	string str = CT2A(pApp->m_pszHelpFilePath).m_psz;
	size_t ipos = str.find(".CHM");
	if (ipos < 0) ipos = str.find(".chm");
	if (ipos > 0) {
		str = str.substr(0, ipos);
		str += ".exe";
		CFile::GetStatus(CString(str.c_str()), status);
		CTime kk = status.m_mtime;
		ExeDate += CT2A(kk.Format(_T("%d %B, %Y"))).m_psz;
	}

#endif

	string exdate = string(ExeDate) + " ";

	general.append_attribute("ExeFileDate") = exdate.c_str();

	if (m_DocFile2.m_DateRun.size() > 0) node.append_attribute("SimulationDate") = m_DocFile2.m_DateRun.c_str();
	general.append_attribute("MultiSimulation") = m_DocFile.m_MultiRunning;
	general.append_attribute("Finished") = m_DocFile.m_FinishedSimulation;
	general.append_attribute("MBinFile") = m_DocFile2.m_Multi_MBin_File.c_str();
	general.append_attribute("Comments") = m_DocFile2.m_Comments.c_str();
	general.append_attribute("RundId") = m_DocFile2.m_RunId.c_str();
	general.append_attribute("SimNumber") = m_DocFile.m_SimulationRunNo;




	sim_period.append_attribute("SimStartMin") = RunOpt.longMinStart;
	string datestart = PGUtil::StringDatum(RunOpt.longMinStart);
	sim_period.append_attribute("SimStartDate") = datestart.c_str();
	sim_period.append_attribute("SimEndMin") = int(RunOpt.longMinEnd);
	datestart = PGUtil::StringDatum(RunOpt.longMinEnd);
	sim_period.append_attribute("SimEndDate") = datestart.c_str();

	numerical.append_attribute("NumberIterations") = RunOpt.noofiter;

	sim_period.append_attribute("SimStartScaling") = int(m_DocFile.m_TimePeriodScaleFactor);

	sim_period.append_attribute("PriorSimTime") = int(m_DocFile.ipredays);

	if (RunOpt.longMinPostEnd > RunOpt.longMinEnd)
		m_DocFile.ipostdays = (RunOpt.longMinPostEnd - RunOpt.longMinEnd) / 1440;
	else
		m_DocFile.ipostdays = 0;

	sim_period.append_attribute("LengthPostPeriod") = int(m_DocFile.ipostdays);

	auto pSw = dynamic_cast<Sw*>(GetPtr(simtype::SWITCH, "TimeResolution"));
	auto iv = pSw->GetIntValue();

	time_resolution.append_attribute("TimeResoluton") = int(iv);
	time_resolution.append_attribute("OutputInterval_Minutes") = RunOpt.oiminutes;
	time_resolution.append_attribute("OutputInterval_Days") = RunOpt.oidays;




	general.append_attribute("Date_Created") = int(m_DocFile.m_TimeCreated);
	general.append_attribute("Date_Modification") = int(m_DocFile.m_TimeModified);
	additional.append_attribute("LockedMultiStore") = int(m_DocFile.m_MultiStoragelocked);

	return true;
};
bool NewMap::Read_Header_FromXmlFile(pugi::xml_node node) {
	auto item = node.first_child();

	auto group = item.name();
	m_FileVersionNumberRead = item.attribute("FileVersionNumber").as_int();
	m_DocFile2.m_OriginFileName = item.attribute("OriginalFileName").as_string();
	m_ChildDocument = item.attribute("ChildDocument").as_bool();
	m_DocFile2.m_ExeFileDate = item.attribute("ExeFileDate").as_string();
	m_DocFile2.m_DateRun = item.attribute("SimulationDate").as_string();
	m_DocFile.m_MultiRunning = item.attribute("MultiSimulation").as_bool();
	m_DocFile.m_FinishedSimulation = item.attribute("Finished").as_bool();
	m_DocFile2.m_Multi_MBin_File = item.attribute("MBinFile").as_string();
	m_DocFile2.m_Comments = item.attribute("Comments").as_string();
	m_DocFile2.m_RunId = item.attribute("RunId").as_string();
	m_DocFile.m_SimulationRunNo = item.attribute("SimNumber").as_uint();
	m_DocFile.m_TimeCreated = item.attribute("Date_Created").as_uint();
	m_DocFile.m_TimeModified = item.attribute("Date_Modification").as_uint();



	auto sim_period = item.next_sibling();
	RunOpt.longMinStart = sim_period.attribute("SimStartMin").as_uint();
	auto datestart = sim_period.attribute("SimStartDate").as_string();
	RunOpt.longMinEnd = sim_period.attribute("SimEndMin").as_uint();
	auto date_end = sim_period.attribute("SimEndDate").as_string();
	m_DocFile.m_TimePeriodScaleFactor = sim_period.attribute("SimStartScaling").as_int();
	m_DocFile.ipredays = sim_period.attribute("PriorSimTime").as_int();
	RunOpt.longMinPreStart = RunOpt.longMinStart - max(size_t(0), m_DocFile.ipredays * 1440);
	m_DocFile.ipostdays = sim_period.attribute("LengthPostPeriod").as_int();
	RunOpt.longMinPostEnd = RunOpt.longMinEnd + max(size_t(0), m_DocFile.ipostdays * 365);

	auto time_resolution = sim_period.next_sibling();
	RunOpt.oiminutes = time_resolution.attribute("OutputInterval_Minutes").as_int();
	RunOpt.oidays = time_resolution.attribute("OutputInterval_Days").as_int();


	auto numerical = time_resolution.next_sibling();
	RunOpt.noofiter = numerical.attribute("NumberIterations").as_int();


	auto additional = numerical.next_sibling();
	m_DocFile.m_MultiStoragelocked = additional.attribute("LockedMultiStore").as_bool();

	return true;
};

bool NewMap::WriteValidationToXmlFile(pugi::xml_node node) {
	const std::vector<const char*> valTableAtt = { "ValFileIndex", "OutType", "Group", "Name","LocalIndex", "n", "R2","A0", "A1", "ME", "RMSE", "MeanSim",
"MeanVal","P_Error","A_Error","LogLi","AccTest","ValFileNumber", "LogTrans","ValidationFileResultIndex","Duration",
"Monitoring", "NSE", "P_NewError", "A_NewError", "BoxCox_Power", "BoxCox_Offset" };




	size_t iv = m_Val_Array.size();
	if (iv > 0)
		if (MR_GetValidIndex_IndexInValFile_InDim1() != string::npos) {
			//	auto NumberofDim1 = m_MultiRun_Array[0].NumberOfRepeatitionsWithinDimension;
			//	if (NumberofDim1 > 0 && (iv / NumberofDim1 > 1)) 	iv = iv / NumberofDim1;
		};
	for (size_t i = 0; i < iv; i++) {
		auto vst = m_Val_Array[i];
		string numstring = "Variables_" + FUtil::STD_ItoAscii(i + 1);

		auto variables = node.append_child(numstring.c_str());
		variables.append_attribute(valTableAtt[0]) = vst.ValidationFileIndex;
		variables.append_attribute(valTableAtt[1]) = vst.OutputType;
		variables.append_attribute(valTableAtt[2]) = vst.Group.c_str();
		variables.append_attribute(valTableAtt[3]) = vst.Name.c_str();
		variables.append_attribute(valTableAtt[4]) = vst.LocalIndex;
		variables.append_attribute(valTableAtt[17]) = vst.ValidationFileNumber;
		variables.append_attribute(valTableAtt[19]) = vst.ValidationFileResultIndex;
		variables.append_attribute(valTableAtt[21]) = vst.Monitoring;

		string Condstring = "Conditions_" + FUtil::STD_ItoAscii(i + 1);

		auto conditions = node.append_child(Condstring.c_str());
		conditions.append_attribute(valTableAtt[13]) = vst.P_Error;
		conditions.append_attribute(valTableAtt[14]) = vst.A_Error;
		conditions.append_attribute(valTableAtt[16]) = vst.AccTest;
		conditions.append_attribute(valTableAtt[18]) = vst.LogTrans;
		conditions.append_attribute(valTableAtt[20]) = vst.Duration;

		string Resultstring = "Results_" + FUtil::STD_ItoAscii(i + 1);

		auto results = node.append_child(Resultstring.c_str());
		results.append_attribute(valTableAtt[5]) = vst.nNumber;
		results.append_attribute(valTableAtt[6]) = vst.R2;
		results.append_attribute(valTableAtt[7]) = vst.A0;
		results.append_attribute(valTableAtt[8]) = vst.A1;
		results.append_attribute(valTableAtt[9]) = vst.ME;
		results.append_attribute(valTableAtt[10]) = vst.RMSE;
		results.append_attribute(valTableAtt[11]) = vst.MeanSim;
		results.append_attribute(valTableAtt[12]) = vst.MeanVal;
		results.append_attribute(valTableAtt[15]) = vst.LogLi;
		results.append_attribute(valTableAtt[22]) = vst.NSE;

	}

	return true;
};

bool NewMap::Read_Validation_FromXmlFile(pugi::xml_node node) {
	const std::vector<const char*> valTableAtt = { "ValFileIndex", "OutType", "Group", "Name","LocalIndex", "n", "R2","A0", "A1", "ME", "RMSE", "MeanSim",
"MeanVal","P_Error","A_Error","LogLi","AccTest","ValFileNumber", "LogTrans","ValidationFileResultIndex","Duration",
"Monitoring", "NSE", "P_NewError", "A_NewError", "BoxCox_Power", "BoxCox_Offset" };
	m_Val_Array.clear();
	VALv vst;
	for (pugi::xml_node level1 = node.first_child(); level1; level1 = level1.next_sibling()) {
		auto namestring = string(level1.name());
		
		if (namestring.find("Variables") == 0) {
			vst.ValidationFileIndex=level1.attribute(valTableAtt[0]).as_uint() ;
			vst.OutputType = level1.attribute(valTableAtt[1]).as_uint();
			vst.Group =level1.attribute(valTableAtt[2]).as_string();
			vst.Name = level1.attribute(valTableAtt[3]).as_string();
			vst.LocalIndex= level1.attribute(valTableAtt[4]).as_uint();
			vst.ValidationFileNumber= level1.attribute(valTableAtt[17]).as_uint();
			vst.ValidationFileResultIndex = level1.attribute(valTableAtt[19]).as_uint();
			vst.Monitoring= level1.attribute(valTableAtt[21]).as_uint();

		}
		else if (namestring.find("Conditions") == 0) {
			vst.P_Error=level1.attribute(valTableAtt[13]).as_float();
			vst.A_Error = level1.attribute(valTableAtt[14]).as_float();
			vst.AccTest = level1.attribute(valTableAtt[16]).as_bool();
			vst.LogTrans = level1.attribute(valTableAtt[18]).as_bool();
			vst.Duration = level1.attribute(valTableAtt[20]).as_uint();
		}
		else if (namestring.find("Results") == 0) {
			vst.nNumber=level1.attribute(valTableAtt[5]).as_uint();
			vst.R2 =level1.attribute(valTableAtt[6]).as_float();
			vst.A0 = level1.attribute(valTableAtt[7]).as_float();
			vst.A1 = level1.attribute(valTableAtt[8]).as_float();
			vst.ME = level1.attribute(valTableAtt[9]).as_float();
			vst.RMSE = level1.attribute(valTableAtt[10]).as_float();
			vst.MeanSim = level1.attribute(valTableAtt[11]).as_float();
			vst.MeanVal = level1.attribute(valTableAtt[12]).as_float();
			vst.LogLi= level1.attribute(valTableAtt[15]).as_float();
			vst.NSE = level1.attribute(valTableAtt[22]).as_float();
			m_Val_Array.push_back(vst);
		}


	}


	size_t count_valid_files = 0;
	for (size_t i = 0; i < MAXSIMVAL; i++) {
		if (ValidationFilePointer(i + 1)->m_Exist) {
			ValidationFilePointer(i + 1)->SetNumberOfSelectedVal(m_ValidationData.m_NumberSelectedValidationVar[i]);
			count_valid_files++;
		}
		else if (m_ValidationData.m_NumberSelectedValidationVar[i] > 0) {
			//m_F_Array.push_back(ValidationFilePointer(i));
			ValidationFilePointer(i + 1)->SetNumberOfSelectedVal(m_ValidationData.m_NumberSelectedValidationVar[i]);
			count_valid_files++;
		}
	}
	Sw* pSw;
	pSw = (Sw*)GetPtr(SWITCH, "ValidationMode");
	m_NumValidFiles = GetSw("ValidationMode")->GetIntValue();
	if (count_valid_files > m_NumValidFiles) {
		pSw->SetIntValue(count_valid_files);
		m_NumValidFiles = GetSw("ValidationMode")->GetIntValue();
	}

	Tab* pTab = (Tab*)GetPtr(TABLE, "Scaling of Observed Variables");
	if (pTab != nullptr) {
		auto oldnelements = pTab->GetNumOfRow();
		if (m_Val_Array.size() != oldnelements)  pTab->SetNumOfRow(m_Val_Array.size());

	}

	if (IsFinishedSimulation())
		m_ValidationData.SetPointersToOutputValidationFiles();

	/*Tab* pTab = (Tab*)GetPtr(TABLE, "Scaling of Observed Variables");
	if (pTab != nullptr) {
		auto oldnelements = pTab->GetNumOfRow();
		if (iv != oldnelements)  pTab->SetNumOfRow(iv);

	}
	if (m_FileVersionNumberRead > 48) {
		rSize_t(&m_NewLogFunc); rSize_t(&m_NewBoxCox);
		enum simtype typ = SWITCH;
		group = "Technical";
		name = "Likelihood Function";
		pSw = (Sw*)GetPtr(typ, name);
		pSw->SetIntValue(m_NewLogFunc);
		name = "Box-Cox Tranformation";
		pSw = (Sw*)GetPtr(typ, name);
		pSw->SetIntValue(m_NewBoxCox);
	} */
	if (IsFinishedSimulation())
		m_ValidationData.SetPointersToOutputValidationFiles();



	return true;
}
bool NewMap::WriteMultiRunSettingsToXmlFile(pugi::xml_node node)
{
	const std::vector<const char*> MultiTableAtt = { "NumberRepetions","NCount", "Numpar" };
	auto head = node.append_child("Bayesian_Setting");
	head.append_attribute(("StepSize")) = m_BayesianStepSize;
	auto nb = GetNumBurnIn();
	head.append_attribute(("Number_BurnInStep")) = nb;
	SIMB valPAR;
	
	auto parnode= node.append_child("List_of_changes");
	for (size_t i = 0; i < m_MultiRun_Array.size(); i++) {
		string dimstring = "Dimension" + FUtil::STD_ItoAscii(i + 1);
		auto dim = parnode.append_child(dimstring.c_str());
		MRv valMr = m_MultiRun_Array[i];
		dim.append_attribute("Number_Repetitions") = valMr.NumberOfRepeatitionsWithinDimension;
		dim.append_attribute("n_Count") = valMr.nCount;

		size_t numpar = MR_Get_NumberOfParametersWithinDim(i + 1);
		for (size_t ii = 0; ii < numpar; ii++) {

			valPAR.pBase = MR_Get_pPar(i + 1, ii + 1);
			valPAR.TabIndex = MR_GetTabIndex(i + 1, ii + 1);
			string group_psw = MR_GetParGroup(i + 1, ii + 1);
			string name_psw = MR_GetParName(i + 1, ii + 1);
			auto parstring = "Parameters" + FUtil::STD_ItoAscii(ii + 1);
			auto par = dim.append_child(parstring.c_str());

			par.append_attribute("Group") = group_psw.c_str();
			par.append_attribute("Name") = name_psw.c_str();
			par.append_attribute("Index") = valPAR.TabIndex;

			auto imethod = MR_GetMethod(i + 1, ii + 1);
			par.append_attribute("Method") = imethod;
			auto imon = MR_GetMonitoring(i + 1, ii + 1);
			par.append_attribute("Monitoring") = imon;
			if (imethod == 4) {
				for (size_t iii = 0; iii < valMr.NumberOfRepeatitionsWithinDimension; iii++) {
					string rowstring = "row_" + FUtil::STD_ItoAscii(iii + 1);
					auto row = par.append_child(rowstring.c_str());
					float value;
					if (valPAR.TabIndex !=string::npos)
						value = ((P*)valPAR.pBase)->MR_Get_TabValue(valPAR.TabIndex, iii);
					else
						value = ((Ps*)valPAR.pBase)->MR_Get_TabValue(iii);
					row.append_attribute("Value") = value;
				}
			}
			else if (imethod == 5) {
				CDB* pCDB = (CDB*)valPAR.pBase;
				for (size_t iii = 0; iii < valMr.NumberOfRepeatitionsWithinDimension; iii++) {
					string rowstring = "row_" + FUtil::STD_ItoAscii(iii + 1);
					auto row = par.append_child(rowstring.c_str());
					row.append_attribute("Selection") = pCDB->MR_GetSelection(iii).c_str();
					row.append_attribute("Key") = pCDB->MR_GetSelection(iii).c_str();
				}
			}
			else if (imethod > 5 && imethod < 9) {
				SimB* pParDep;
				SimB* pCP = MR_Get_pPar(i + 1, ii + 1);
				size_t indpar, ind_dep;
				indpar = MR_GetTabIndex(i + 1, ii + 1);
				if (pCP->Is_P()) {
					pParDep = ((Ps*)pCP)->Get_MR_ParDependence();
					ind_dep = ((Ps*)pCP)->Get_MR_ParDependenceIndex();
				}
				else {
					pParDep = ((P*)pCP)->Get_MR_ParDependence(indpar);
					ind_dep = ((P*)pCP)->Get_MR_ParDependenceIndex(indpar);
				}
				if (pParDep != nullptr) {
					string group_psw = pParDep->GetGroup();
					string name_psw = pParDep->GetName();

					par.append_attribute("GroupDepPar") = group_psw.c_str();
					par.append_attribute("NameDepPar") = name_psw.c_str();
					par.append_attribute("IndexDepPar") = ind_dep;

					if (imethod > 6) {
						par.append_attribute("MinValue") = MR_GetMin(i + 1, ii + 1);
						par.append_attribute("MaxValue") = MR_GetMax(i + 1, ii + 1);
					}
				}
			}
			else {
				par.append_attribute("MinValue") = MR_GetMin(i + 1, ii + 1);
				par.append_attribute("MaxValue") = MR_GetMax(i + 1, ii + 1);
				par.append_attribute("StartValue") = MR_GetStart(i + 1, ii + 1);
			}

		}



	}



	auto parbayes = node.append_child("Bayesian_Correlations");
	size_t iv = MR_Get_NumTotalPar();
	parbayes.append_attribute("TotNumPar") = iv;
	
	for (size_t i = 0; i < iv; i++) {
		string bayespar = "BayesPar_" + FUtil::STD_ItoAscii(i + 1);
		auto bayes=parbayes.append_child(bayespar.c_str());
		auto fvalue = MR_Get_BayesMean(i);
		bayes.append_attribute("MeanValue") = fvalue;

		for (size_t ii = 0; ii < iv; ii++) {
			bayespar = "Dependence_" + FUtil::STD_ItoAscii(ii + 1);
			auto covar = bayes.append_child(bayespar.c_str());
			fvalue = MR_Get_BayesCoVar(i, ii);
			covar.append_attribute("CoVar") = fvalue;
			fvalue = MR_Get_BayesCoCor(i, ii); wFloat(&fvalue);
			covar.append_attribute("CoCor") = fvalue;
		}
	}






	return false;
}
bool NewMap::Read_MultiRunSettings_FromXmlFile(pugi::xml_node node)
{
	m_MultiRun_Array.clear();
	m_P_MR_Array.clear();

	SimB* pP = nullptr;
	SIMB valPAR;
	MRv valMR;

	for (pugi::xml_node level1 = node.first_child(); level1; level1 = level1.next_sibling()) {
		auto namestring = string(level1.name());
		if (namestring == "Bayesian_Setting") {
			m_BayesianStepSize = level1.attribute("StepSize").as_double();
			SetNumBurnIn(level1.attribute("Number_BurnInStep").as_uint());
			
		}
		else if (namestring == "List_of_changes") {
			size_t DimNumber=0;
			for (pugi::xml_node dim = level1.first_child(); dim; dim = dim.next_sibling()) {
				valMR.NumberOfRepeatitionsWithinDimension = dim.attribute("Number_Repetitions").as_uint();
				valMR.nCount = dim.attribute("n_Count").as_uint();
				DimNumber++;
				m_MultiRun_Array.push_back(valMR);
				for (pugi::xml_node par = dim.first_child(); par; par = par.next_sibling()) {
					auto group = par.attribute("Group").as_string();
					auto name = par.attribute("Name").as_string();
					auto index = par.attribute("Index").as_uint();
					auto method = par.attribute("Method").as_uint();
					auto monitoring = par.attribute("Monitoring").as_uint();
					auto minvalue = par.attribute("MinValue").as_double();
					auto maxvalue = par.attribute("MaxValue").as_double();
					auto startvalue = par.attribute("StartValue").as_double();
					if (group == "DataBase") {
						CDB* pDB;
						string strID, strIDKey;
						pDB = DB_GetPointer(name);
						valPAR.pBase = pDB;
						valPAR.TabIndex = index;
						pDB->MR_Set_Method(MR_METHOD(method));
						for (pugi::xml_node row = par.first_child(); row; row = row.next_sibling()) {
							auto strID = row.attribute("Selection").as_string();
							auto strKey = row.attribute("Key").as_string();
							pDB->MR_AddSelection(strID, strKey);					
						}
						pDB->MR_Set_Dim(DimNumber);
						MR_CP_Set((Ps*)pDB, string::npos, true);
					}

					else {
						string type = "Parameters";
						if (name == "Index in val file" && group == "Irrigation") group = "Technical";
						if (name == "Size of internal loop" && group == "Irrigation") group = "Technical";
						if (name == "No of internal index loop" && group == "Irrigation") group = "Technical";
						if (name == "SaltHalfReductionn") name = "";
						pP = GetPtr(type, group, name);
						if (pP == nullptr) {
							if (name == "Intial Soil Storage") name = "Initial Soil Storage";
							if (name == "Intial Peak Storage") name = "Initial Peak Storage";
							if (name == "Intial Base Storage") name = "Initial Base Storage";
							if (name == "Index for CsvFile") name = "Index Fert Table CsvFile";
							pP = GetPtr(type, group, name);
						}
						if (pP == nullptr) {
							// Table Parameters
							type = "Table Parameters";
							if (name == "SaltHalfReduction") {
								name = "Salt c50";
								valPAR.TabIndex = 0;
							}
							SimB* pPar;
							//pPar=GetPtr(type,group,name);
							pPar = GetPtr(PAR_TABLE, name);
							P* pkoll = dynamic_cast<P*>(pPar);
							valPAR.pBase = dynamic_cast<SimB*>(pPar);
							if (pPar == nullptr)  return false;
							valPAR.TabIndex = index;
							if (m_FileVersionNumberRead > 43) {
								if (!pPar->Is_PTable())
									((Ps*)pPar)->MR_Set_Monitoring(monitoring);
								else
									((P*)pPar)->MR_Set_Monitoring(valPAR.TabIndex, monitoring);
							}
							if (!pPar->Is_PTable())
								((Ps*)pPar)->MR_Set_Method(MR_METHOD(method));
							else {
								if (((P*)pPar)->GetSize() <= valPAR.TabIndex) valPAR.TabIndex = ((P*)pPar)->GetSize() - 1;
								((P*)pPar)->MR_Set_Method(valPAR.TabIndex, MR_METHOD(method));
							}
							pP = valPAR.pBase;

							if (method == 4) {
								for (pugi::xml_node row = par.first_child(); row; row = row.next_sibling()) {
									auto fvalue = row.attribute("Value").as_float();
									((P*)pP)->MR_Append_TableValue(valPAR.TabIndex, fvalue);
								}
							}
							else if (method > 5 && method < 9) {
								int deptabindex;
								auto groupdep = par.attribute("GroupDepPar").as_string();
								auto namedep = par.attribute("NameDepPar").as_string();
								auto Indexdep = par.attribute("IndexDepPar").as_uint();


								if (Indexdep == string::npos)
									type = "Parameters";
								else
									type = "Table Parameters";
								Ps* pParDep;
								pParDep = (Ps*)GetPtr(type, groupdep, namedep);

								if (pParDep != nullptr) {

									if (!pP->Is_PTable()) {
										if (pParDep->Is_P())
											((Ps*)pP)->Set_MR_ParDependence(pParDep);
										else
											((Ps*)pP)->Set_MR_ParDependence(pParDep, Indexdep);
									}
									else {
										if (pParDep->Is_P())
											((P*)pP)->Set_MR_ParDependence(valPAR.TabIndex,pParDep);
										else
											((P*)pP)->Set_MR_ParDependence(valPAR.TabIndex, pParDep, Indexdep);
									}
								}
								else {
									// not identified parameter dependence

								}
								if (method > 1) {
									if (pP->Is_P()) {
										((Ps*)pP)->MR_Set_Min(minvalue);
										((Ps*)pP)->MR_Set_Max(maxvalue);
									}
									else {
										((P*)pP)->MR_Set_Min(valPAR.TabIndex, minvalue);
										((P*)pP)->MR_Set_Max(valPAR.TabIndex, maxvalue);
									}

								}
							}
							else {
								if (pP->Is_P()) {
									((Ps*)pP)->MR_Set_Min(minvalue);
									((Ps*)pP)->MR_Set_Max(maxvalue);
								}
								else {
									((P*)pP)->MR_Set_Min(valPAR.TabIndex, minvalue);
									((P*)pP)->MR_Set_Max(valPAR.TabIndex, maxvalue);
								}
								if (m_FileVersionNumberRead > 30) {
									if (pP->Is_P())
										((Ps*)pP)->MR_Set_Start(startvalue);
									else
										((P*)pP)->MR_Set_Start(valPAR.TabIndex, startvalue);
								}

							}
						}
						else {
							//Single Value Parameters

							valPAR.pBase = pP;
							valPAR.TabIndex = string::npos;
							if (m_FileVersionNumberRead > 43) {
								if (valPAR.pBase->Is_P())
									((Ps*)pP)->MR_Set_Monitoring(monitoring);
								else
									((P*)valPAR.pBase)->MR_Set_Monitoring(valPAR.TabIndex, monitoring);
							}
							((Ps*)pP)->MR_Set_Method(MR_METHOD(method));

							if (method == 4) {
								for (pugi::xml_node row = par.first_child(); row; row = row.next_sibling()) {
									auto fvalue = row.attribute("Value").as_float();
									((Ps*)pP)->MR_Append_TableValue(fvalue);
								}
							}
							else if (method > 5 && method < 9) {
								int deptabindex;
								auto groupdep = par.attribute("GroupDepPar").as_string();
								auto namedep = par.attribute("NameDepPar").as_string();
								auto Indexdep = par.attribute("IndexDepPar").as_uint();


								if (valPAR.TabIndex==string::npos)
									type = "Parameters";
								else
									type = "Table Parameters";
								Ps* pParDep;
								pParDep = (Ps*)GetPtr(type, groupdep, namedep);

								if (pParDep != nullptr) {
									if (pParDep->Is_P())
										((Ps*)pP)->Set_MR_ParDependence(pParDep);
									else
										((Ps*)pP)->Set_MR_ParDependence(pParDep, Indexdep);
								}
								else {
								
									// not identified parameter dependence

								}
								if (method > 1) {
									if (pP->Is_P()) {
										((Ps*)pP)->MR_Set_Min(minvalue);
										((Ps*)pP)->MR_Set_Max(maxvalue);
									}
									else {
										((P*)pP)->MR_Set_Min(valPAR.TabIndex, minvalue);
										((P*)pP)->MR_Set_Max(valPAR.TabIndex, maxvalue);
									}

								}
							}
							else {
								if (pP->Is_P()) {
									((Ps*)pP)->MR_Set_Min(minvalue);
									((Ps*)pP)->MR_Set_Max(maxvalue);
								}
								else {
									((P*)pP)->MR_Set_Min(valPAR.TabIndex, minvalue);
									((P*)pP)->MR_Set_Max(valPAR.TabIndex, maxvalue);
								}
								if (m_FileVersionNumberRead > 30) {
									if (pP->Is_P())
										((Ps*)pP)->MR_Set_Start(startvalue);
									else
										((P*)pP)->MR_Set_Start(valPAR.TabIndex, startvalue);
								}

							}
						}
						if (valPAR.pBase->Is_P())
							((Ps*)pP)->MR_Set_Dim(DimNumber);
						else
							((P*)pP)->MR_Set_Dim(valPAR.TabIndex, DimNumber);
					}
					m_P_MR_Array.push_back(valPAR);
				}
			}


		}

		else if (namestring == "Bayesian_Correlations") {
			auto iv = node.attribute("TotNumPar").as_uint();
			MR_Init_BayesCovar(iv);
			MR_Init_BayesCocor(iv);
			MR_Set_NumBayesPar(iv);
			size_t i1 = 0, i2 = 0;
			for (pugi::xml_node dependence1 = level1.first_child(); dependence1; dependence1=dependence1.next_sibling()) {
				auto fmean = dependence1.attribute("MeanValue").as_float();
				MR_Set_BayesMean(i1, fmean);
				
				for (pugi::xml_node dependence2 = dependence1.first_child(); dependence2; dependence2 = dependence2.next_sibling()) {
					auto value = dependence2.attribute("CoVar").as_float();
					MR_Set_BayesCoVar(i1, i2, value);
					if (m_FileVersionNumberRead > 33) {
						value = dependence2.attribute("CoCor").as_float();
						MR_Set_BayesCoCor(i1, i2, value);
					}
					i2++;
				}
				i1++;
			}

			

		}
	}


	return false;
}
bool NewMap::WriteDynamicParToXmlFile(pugi::xml_node node)
{
	CHPARv ch;
	for (size_t i = 0; i < m_P_CH_Array.size(); i++) {
		ch = m_P_CH_Array[i];
		string chstring = "Parameter_";
		chstring += FUtil::STD_ItoAscii(i + 1);
		auto row=node.append_child(chstring.c_str());
		row.append_attribute("Type") = ch.Type;
		if (ch.pBase != nullptr) {
			if (ch.Type == 0) {
				auto pPar = (Ps*)ch.pBase;
				auto TabIndex = ch.TabIndex;
				row.append_attribute("Group") = pPar->GetGroup().c_str();
				row.append_attribute("Name") = ((Ps*)ch.pBase)->GetName().c_str();
				row.append_attribute("Index") = ch.TabIndex;
				row.append_attribute("NumChanges") = ch.NumDates;
				for (size_t ii = 0; ii < ch.NumDates; ii++) {
					string dateline = "date_" + FUtil::STD_ItoAscii(ii + 1);
					auto row2 = row.append_child(dateline.c_str());
					row2.append_attribute("MinDate") = pPar->GetChaDate(ii);
					row2.append_attribute("NewValue") = pPar->GetChaParValue(ii);
				}
			}
			else if (ch.Type == 1) {
				P* pPar = (P*)ch.pBase;
				row.append_attribute("Group") = pPar->GetGroup().c_str();
				row.append_attribute("Name") = pPar->GetName().c_str();
				row.append_attribute("Index") = ch.TabIndex;
				row.append_attribute("NumChanges") = ch.NumDates;
				for (size_t ii = 0; ii < ch.NumDates; ii++) {
					string dateline = "date_" + FUtil::STD_ItoAscii(ii + 1);
					auto row2 = row.append_child(dateline.c_str());
					row2.append_attribute("MinDate") = pPar->GetChaDate(ch.TabIndex, ii);
					row2.append_attribute("NewValue") = pPar->GetChaParValue(ch.TabIndex, ii);
				}

			}
			else if (ch.Type == 2) {
				CDB* pDB;
				pDB = (CDB*)ch.pBase;
				row.append_attribute("Group") = pDB->GetGroup().c_str();
				row.append_attribute("Name") = pDB->GetName().c_str();
				row.append_attribute("Index") = ch.TabIndex;
				row.append_attribute("NumChanges") = ch.NumDates;

				for (size_t ii = 0; ii < ch.NumDates; ii++) {
					string dateline = "date_" + FUtil::STD_ItoAscii(ii + 1);
					auto row2 = row.append_child(dateline.c_str());
					row2.append_attribute("MinDate") = pDB->GetChaDate(ii);
					row2.append_attribute("ParValue") = pDB->GetChaParValue(ii).c_str();
					row2.append_attribute("ParKey") = pDB->GetChaParKey(ii).c_str();
				}
			}
		}
	}

	return true;
}
bool NewMap::Read_DynamicPar_FromXmlFile(pugi::xml_node node)
{
	Ps* pP = nullptr;
	string type, group, name;
	float fvalue;
	CHPARv ch;
	P* pPt;
	unsigned int datemin;
	m_P_CH_Array.clear();
	VALv vst;
	for (pugi::xml_node level1 = node.first_child(); level1; level1 = level1.next_sibling()) {
		auto namestring = string(level1.name());
		ch.Type = level1.attribute("Type").as_int();
		if (ch.Type == 0) {
			group = level1.attribute("Group").as_string();
			name = level1.attribute("Name").as_string();
			type = "Parameters";
			pP = (Ps*)GetPtr(type, group, name);
			if (pP != nullptr) {
				ch.pBase = static_cast<SimB*>(pP);
				ch.TabIndex = level1.attribute("Index").as_uint();
				ch.NumDates = level1.attribute("NumChanges").as_uint();
				for (pugi::xml_node level2 = level1.first_child(); level2; level2 = level2.next_sibling()) {
					datemin = level2.attribute("MinDate").as_uint();
					fvalue = level2.attribute("NewValue").as_float();
					pP->SetChaDateAndValue(datemin, fvalue);
				}
				ch.NumDates = pP->GetNumChaDates();
				ch.nCount = ch.NumDates;
			}
		}
		else if (ch.Type == 1) {
			group = level1.attribute("Group").as_string();
			name = level1.attribute("Name").as_string();
			type = "Table Parameters";
			pPt = (P*)GetPtr(type, group, name);
			ch.pBase = static_cast<SimB*>(pPt);
			ch.TabIndex = level1.attribute("Index").as_uint();
			if (pPt != nullptr) {
				ch.pBase = pPt;
				for (pugi::xml_node level2 = level1.first_child(); level2; level2 = level2.next_sibling()) {
					datemin = level2.attribute("MinDate").as_uint();
					fvalue = level2.attribute("NewValue").as_float();
					pPt->SetChaDateAndValue(datemin, ch.TabIndex, fvalue);
				}
				ch.NumDates = pPt->GetNumChaDates(ch.TabIndex);
			
			} 
			

		}
		else if (ch.Type == 2) {
			ch.Type = 2;
			CDB* pDB;
			name = level1.attribute("Name").as_string();
			pDB = DB_GetPointer(name);
			ch.pBase = pDB;
			string Id, Key;
			for (pugi::xml_node level2 = level1.first_child(); level2; level2 = level2.next_sibling()) {
				datemin = level2.attribute("MinDate").as_uint();
				Id = level2.attribute("ParValue").as_string();
				Key = level2.attribute("ParKey").as_string();
				pDB->SetChaDateAndValues(datemin, Id, Key);
			}
			ch.NumDates = pDB->GetNumChaDates();

		}
		m_P_CH_Array.push_back(ch);
		
	}


	return false;
}
bool NewMap::WriteHistoryToXmlFile(pugi::xml_node node)
{
	string group, name, str, type;

	size_t startindex;

	if (m_ChildDocument) {
		startindex = m_History_Array.size() - 1;
		for (size_t i = 0; i < m_History_Array.size(); i++) {
			auto h_node = m_History_Array[i];
			SimB* pBase;
			pBase = h_node.pBase;
			if (HIST_INFO(h_node.RunInfoType) == HIST_INFO::RUN_INFO) {
				name = ((CRunInfo*)pBase)->GetName();
				if (name == "NewRunNo") {
					str = h_node.strvalue;
					size_t runno = FUtil::AtoInt(str);
					if (m_DocFile.m_SimulationRunNo - 1 <= runno) startindex = i;
				}
			}
		}
	}
	else
		startindex = 0;


	for (size_t i = startindex; i < m_History_Array.size(); i++) {
		auto h_node = m_History_Array[i];
		string chstring = "Row_";
		chstring += FUtil::STD_ItoAscii(i + 1);
		auto row = node.append_child(chstring.c_str());
		SimB* pBase;
		pBase = h_node.pBase;

		if (h_node.RunInfoType < 0 || pBase == nullptr) {
			group = "";
			name = "";
			str = "";

		}
		else if (HIST_INFO(h_node.RunInfoType) == HIST_INFO::DATA_BASE_INFO) {
			// DAtaBase
			group = "";
			name = ((CDB*)pBase)->GetName();
			str = History_GetString(i);
			type = "DataBase";

		}
		else if (HIST_INFO(h_node.RunInfoType) == HIST_INFO::SWITCH_INFO) {
			group = pBase->GetGroup();
			name = pBase->GetName();
			str = h_node.strvalue;
			type = "Switches";

		}
		else if (HIST_INFO(h_node.RunInfoType) == HIST_INFO::RUN_INFO) {
			group = ((CRunInfo*)pBase)->GetGroup();
			name = ((CRunInfo*)pBase)->GetName();
			str = h_node.strvalue;
			type = "Run Info";

		}
		else if (h_node.RunInfoType == 1) {
			if (pBase->GetName() == "Soil Properties" || pBase->Is_DB()) {
				group = "";
				name = ((Sw*)pBase)->GetName();
			}
			else {
				group = "";
				name = pBase->GetName();
				str = FUtil::STD_FtoAscii(History_GetFloat(i));
			}
		}
		else if (HIST_INFO(h_node.RunInfoType) == HIST_INFO::PARSINGLE_INFO) {
			group = pBase->GetGroup();
			name = pBase->GetName();
			str = FUtil::STD_FtoAscii(History_GetFloat(i));
			type = "Parameters";
		}
		else if (HIST_INFO(h_node.RunInfoType) == HIST_INFO::PARTABLE_INFO) {
			group = pBase->GetGroup();
			name = pBase->GetName();
			str = FUtil::STD_FtoAscii(History_GetFloat(i));
			type = "Table Parameters";
		}
		else if (HIST_INFO(h_node.RunInfoType) == HIST_INFO::MODELFILE_INFO) {
			group = pBase->GetGroup();
			name = pBase->GetName();
			type = "ModelFiles";
		}
		row.append_attribute("RunInfoType") = h_node.RunInfoType;
		row.append_attribute("StringType") = type.c_str();
		row.append_attribute("GroupName") = group.c_str();
		row.append_attribute("ItemName") = name.c_str();
		row.append_attribute("StringValue") = str.c_str();
		row.append_attribute("LocalIndex") = h_node.LocalIndex;
		row.append_attribute("TimeStamp") = h_node.tvalue;
		row.append_attribute("User") = h_node.User.c_str();
		row.append_attribute("Computer") = h_node.Computer.c_str();
	}
	return false;
}

bool NewMap::WriteAllFinalStateToXmlFile(pugi::xml_node node)
{
	vector<SimB*> vpp, vp_final;
	size_t iv = 0;

	vpp = GetPtrVector("State Variables", "");
	for (size_t i = 0; i < vpp.size(); i++) {
		auto pPtr = (SimB*)vpp[i];
		auto name = pPtr->GetName();
		//if ((name == "WaterStorage" || name == "SoilHeat" || name.find("Humus") != string::npos || name.find("Litter") != string::npos) ) {
		iv++;
		vp_final.push_back(pPtr);
		//}
	}

	bool IsVector = false;
	for (size_t i = 0; i < vp_final.size(); i++) {
		auto pXT = static_cast<X*>(vp_final[i]);
		auto pX = static_cast<Xs*>(vp_final[i]);
		IsVector = vp_final[i]->Is_Vector();

		string row = "row_" + FUtil::STD_ItoAscii(i + 1);
		auto xnode = node.append_child(row.c_str());
		auto group = pXT->GetGroup();
		auto name = pXT->GetName();
		xnode.append_attribute("Group") = group.c_str();
		xnode.append_attribute("Name") = name.c_str();
		size_t nn;
		if (IsVector) {
			nn = pXT->GetNumberOfFlags();
			xnode.append_attribute("NumberofValue") = nn;
			for (size_t ii = 0; ii < nn; ii++) {
				string subrow = "row_" + FUtil::STD_ItoAscii(ii + 1);
				auto rownode = xnode.append_child(subrow.c_str());
				rownode.append_attribute("FinalValue") = pXT->GetFinal(ii);
				rownode.append_attribute("InitialValue") = pXT->GetInitial(ii);
			}
		}
		else {
			xnode.append_attribute("FinalValue") = pX->GetFinal();
			xnode.append_attribute("InitialValue") = pX->GetInitial();
		}
	}

	return false;

}

bool NewMap::Read_AllFinalState_FromXmlFile(pugi::xml_node node)
{
	for (pugi::xml_node level1 = node.first_child(); level1; level1 = level1.next_sibling()) {
		auto namestring = string(level1.name());

		auto group= level1.attribute("Group").as_string();
		auto name = level1.attribute("Name").as_string();
		auto num = level1.attribute("NumberofValue").as_uint();
		for (pugi::xml_node level2 = level1.first_child(); level2; level2=level2.next_sibling()) {
			auto final = level2.attribute("FinalValue").as_double();
			auto initial = level2.attribute("InitalValue").as_double();
		}

	}
	return false;
}

bool NewMap::Read_History_FromXmlFile(pugi::xml_node node)
{
	HISTv h;
	string group, type, name;
	for (pugi::xml_node level1 = node.first_child(); level1; level1 = level1.next_sibling()) {
		auto namestring = string(level1.name());
		h.RunInfoType = level1.attribute("RunInfoType").as_uint();
		group = level1.attribute("GroupName").as_string();
		name = level1.attribute("ItemName").as_string();
		type = level1.attribute("StringType").as_string();
		h.strvalue = level1.attribute("StringValue").as_string();															 

		h.LocalIndex = level1.attribute("LocalIndex").as_uint();
		h.tvalue= level1.attribute("TimeStamp").as_uint();
		h.User = level1.attribute("User").as_string();
		h.Computer = level1.attribute("Computer").as_string();
		History_NewAdd(h.RunInfoType, group, type, name, h.strvalue, h.LocalIndex, h.tvalue, h.User, h.Computer);
	}

	return false;
}

bool NewMap::Read_RunReport_FromXmlFile(pugi::xml_node node)
{
	m_Report_Array.clear();
	for (pugi::xml_node level1 = node.first_child(); level1; level1 = level1.next_sibling()) {
		auto str = level1.attribute("str").as_string();
		m_Report_Array.push_back(str);
	}
	return true;
}

bool NewMap::WriteRunReportToXmlFile(pugi::xml_node node)
{
	string str;
	for (size_t i = 0; i < m_Report_Array.size(); i++) {
		auto checkstring = m_Report_Array[i];
		if (checkstring.size() > 0) {
			string chstring = "Row_";
			chstring += FUtil::STD_ItoAscii(i + 1);
			auto row = node.append_child(chstring.c_str());
			FUtil::trim_xmlstring(checkstring);
			row.append_attribute("str") = checkstring.c_str();
		}
	}

	return false;

}

bool NewMap::WriteOutputSummaryToXmlFile(pugi::xml_node node)
{
	vector<SIMB> out, pvv;
	vector<SimB*> vp;
	OutSingle* pSingle;
	OutVector* pVector;


	auto makeout = [&](pugi::xml_node localnode) {
		for (size_t i = 0; i < pvv.size(); i++) {
			pVector = dynamic_cast<OutVector*>(pvv[i].pBase);
			string childname;
			childname = pvv[i].pBase->GetName();
			p_ModelInfo->trim_xml(childname);
			if (pVector != nullptr) {
				childname += "_" + FUtil::STD_ItoAscii(pvv[i].TabIndex + 1);
				auto row = localnode.append_child(childname.c_str());
				row.append_attribute("Initial") = pVector->GetInitial(pvv[i].TabIndex);
				row.append_attribute("Final") = pVector->GetFinal(pvv[i].TabIndex);
				row.append_attribute("Min") = pVector->GetMin(pvv[i].TabIndex);
				row.append_attribute("Max") = pVector->GetMax(pvv[i].TabIndex);
				row.append_attribute("Mean") = pVector->GetMean(pvv[i].TabIndex);
				row.append_attribute("Accumulated") = pVector->GetAccumulated(pvv[i].TabIndex);
				row.append_attribute("PgFileIndex") = pVector->GetPgFileIndex(pvv[i].TabIndex);
				row.append_attribute("PgMultiFileIndex") = pVector->GetPgMultiFileIndex(pvv[i].TabIndex);
				row.append_attribute("StoreFlag") = pVector->GetStoreFlag(pvv[i].TabIndex);
			}
			else {
				pSingle = dynamic_cast<OutSingle*>(pvv[i].pBase);
				if (pSingle != nullptr) {
					auto row = localnode.append_child(childname.c_str());
					row.append_attribute("Initial") = pSingle->GetInitial();
					row.append_attribute("Final") = pSingle->GetFinal();
					row.append_attribute("Min") = pSingle->GetMin();
					row.append_attribute("Max") = pSingle->GetMax();
					row.append_attribute("Mean") = pSingle->GetMean();
					row.append_attribute("Accumulated") = pSingle->GetAccumulated();
					row.append_attribute("PgFileIndex") = pSingle->GetPgFileIndex();
					row.append_attribute("PgMultiFileIndex") = pSingle->GetPgMultiFileIndex();
					row.append_attribute("StoreFlag") = pSingle->GetStoreFlag();
				}
			}
		}

	};

	for (size_t i = 1; i < 5; i++) {
		pvv.clear();
		vp = GetPtrVector(simtype(i), false);
		out = SelectedOutVector(vp);
		for (size_t i = 0; i < out.size(); i++)
			pvv.push_back(out[i]);

		vp = GetPtrVector(simtype(i+7), false);
		out = SelectedOutVector(vp);
		for (size_t i = 0; i < out.size(); i++)
			pvv.push_back(out[i]);
		if (pvv.size() > 0) {
			string out_type;
			switch (i) {
			case 1:
				out_type = "State";
				break;
			case 2:
				out_type = "Flow";
				break;
			case 3:
				out_type = "Auxiliary";
				break;
			case 4:
				out_type = "Driving";

			}
			auto newchild = node.append_child(out_type.c_str());

			makeout(newchild);
		}
	}


	return false;
}

bool NewMap::Read_OutputSummary_FromXmlFile(pugi::xml_node node)
{
	OUTPUT output;
	simtype type_id;
	Clear_XTGD();
	output.PgFileIndex = output.PgMultiFileIndex = 0;
	for (pugi::xml_node level1 = node.first_child(); level1; level1 = level1.next_sibling()) {
		string level1name=level1.name();
		if (level1name == "State") type_id = simtype(1);
		else if (level1name == "Flow") type_id =  simtype(2);
		else if (level1name == "Auxiliary") type_id = simtype(3);
		else if (level1name == "Driving") type_id = simtype(4);

		for (pugi::xml_node level2 = level1.first_child(); level2; level2 = level2.next_sibling()) {

			string varstr = level2.name();
			SimB* pSimB = nullptr;

			output.Initial = level2.attribute("Initial").as_float();
			output.Final = level2.attribute("Final").as_float();
			output.Min = level2.attribute("Min").as_float();
			output.Max = level2.attribute("Max").as_float();
			output.Mean = level2.attribute("Mean").as_float();
			output.Accumulated = level2.attribute("Accumulated").as_float();
			output.StoreFlag = level2.attribute("StoreFlag").as_uint();

			if (output.StoreFlag >= 1) output.PgFileIndex++;
			if (output.StoreFlag == 2) output.PgMultiFileIndex++;

			pSimB = GetPtr(simtype(type_id + 7), varstr);


			if (varstr.rfind("_") != string::npos&&pSimB == nullptr) {
				pSimB = GetPtr(type_id,varstr.substr(0,varstr.rfind("_")));
				if (pSimB == nullptr) {
					string oldname = varstr.substr(0, varstr.rfind("_"));
					string newname = p_ModelInfo->getOrigName(oldname);
					pSimB = GetPtr(type_id, newname);
				}
				OutVector* pVector;
				pVector = static_cast<OutVector*>(pSimB);
				output.LocalIndex = FUtil::AtoInt(varstr.substr(varstr.rfind("_") + 1));
				if (pVector != nullptr) {
					pVector->AddOutPutVar(output.LocalIndex - 1, output);
					pVector->SetStoreFlag(output.LocalIndex - 1, output.StoreFlag);
				}
			}
			else {
				pSimB = GetPtr(simtype(type_id+7),varstr);
				if (pSimB == nullptr) {
					string newname = p_ModelInfo->getOrigName(varstr);
					pSimB = GetPtr(simtype(type_id+7), newname);
					if (pSimB == nullptr) {

						auto koll = 1;

					}
				}
				OutSingle* pSingle;
				output.LocalIndex = string::npos;
				pSingle = static_cast<OutSingle*>(pSimB);
				if (pSingle != nullptr) {
					pSingle->AddOutPutVar(output);
					pSingle->SetStoreFlag(output.StoreFlag);
				}
			} 
			SIMB out;
			out.pBase = pSimB;
			out.TabIndex = output.LocalIndex;
			if (pSimB != nullptr) {
				if (type_id == STATE) m_X_Array.push_back(out);
				if (type_id == FLOW) m_T_Array.push_back(out);
				if (type_id == AUX) m_G_Array.push_back(out);
				if (type_id == DRIVE) m_D_Array.push_back(out);
			}

		}
	}
	m_X_Array = SortSimB_ByGroup(m_X_Array);
	m_T_Array = SortSimB_ByGroup(m_T_Array);
	m_G_Array = SortSimB_ByGroup(m_G_Array);
	m_D_Array = SortSimB_ByGroup(m_D_Array);
	if (!IsFinishedSimulation()) {
		RemoveOriginalValues("Outputs", "ALL", 0, 0);	
	}

	return false;
}

bool NewMap::WriteDataBaseLinksToXmlFile(pugi::xml_node node)
{
	string str;
	time_t timestamp;
	if (m_ChildDocument) return false;
	for (size_t i = 0; i < 10; i++) {
		str = DB_GetNames(i);
		FUtil::trim_space(str);
		auto row=node.append_child(str.c_str());
		timestamp= DB_GetImportTimeT(i);
		row.append_attribute("TimeStamp") = size_t(timestamp);
		string strvalue= DB_GetChoice(DB_types(i)).c_str();
		auto pos1 = strvalue.find('\t');
		if (pos1 != string::npos) {
			auto str1 = strvalue.substr(0, pos1);
			if (str1.size() > 0) {
				row.append_attribute("System") = str1.c_str();
			}
			strvalue = strvalue.substr(pos1 + 1);
			pos1 = strvalue.find('\t');

			if (pos1 != string::npos) {
				auto str2 = strvalue.substr(0, pos1);
				if (str1.size() > 0) {
					row.append_attribute("Objekt") = str2.c_str();
				}
				strvalue = strvalue.substr(pos1 + 1);
				pos1 = strvalue.find('\t');
			}
			if (pos1 != string::npos) {
				auto str2 = strvalue.substr(0, pos1);
				if (DB_types(i) == DB_types::Soil_Properties) {
					auto key = str2.substr(0, 6);
					row.append_attribute("KeyId") = key.c_str();
					auto sitename = str2.substr(7);
					row.append_attribute("SiteName") = sitename.c_str();
				}
				else {
					FUtil::trim_space(str2);
					row.append_attribute("SiteName") = str2.c_str();
				}
				strvalue = strvalue.substr(pos1 + 1);
				pos1 = strvalue.find('\t');
			}

			if (pos1 != string::npos) {
				auto cord = strvalue.substr(0, pos1);

				auto pos4 = cord.find('/');
				if (pos4 != string::npos) {
					auto X = cord.substr(0, pos4);
					row.append_attribute("X_cord") = X.c_str();
					auto y = cord.substr(pos4 + 1);
					row.append_attribute("Y_cord") = y.c_str();
				}

				strvalue = strvalue.substr(pos1 + 1);
			}
			if (strvalue.size() > 0) {
				auto site = strvalue;
				if (site.size() > 3) {
					auto county = site.substr(0, 2);
					FUtil::trim_space(county);
					auto country = site.substr(3);
					FUtil::trim_space(country);
					row.append_attribute("County") = county.c_str();

					row.append_attribute("Country") = country.c_str();

				}
				else {
					auto country = site;
					FUtil::trim_space(country);
					row.append_attribute("Country") = country.c_str();
				}

			}



		}
		else {
			DB_OldHeader header = m_DB_header[i];
			FUtil::trim_xmlstring(header.SiteName);
			FUtil::trim_xmlstring(header.Details);
			

			if (DB_types(i) == DB_types::Soil_Properties) {
				row.append_attribute("Name") = header.Name.c_str();
				row.append_attribute("SiteName") = header.SiteName.c_str();
				row.append_attribute("Type") = header.Type;
				row.append_attribute("Latitude") = header.Latitude.c_str();
				row.append_attribute("Longitude") = header.Longitude.c_str();
				row.append_attribute("Country") = header.Country.c_str();
				row.append_attribute("DriveFile") = header.DriveFile.c_str();
				row.append_attribute("Details") = header.Details.c_str();
				row.append_attribute("KeyId") = header.keyId.c_str();
			}
			else {
				row.append_attribute("Name") = header.Name.c_str();
				row.append_attribute("SiteName") = header.SiteName.c_str();
				row.append_attribute("Type") = header.Type;
				row.append_attribute("Latitude") = header.Latitude.c_str();
				row.append_attribute("Longitude") = header.Longitude.c_str();
				row.append_attribute("Country") = header.Country.c_str();
				row.append_attribute("DriveFile") = header.DriveFile.c_str();
				row.append_attribute("Details") = header.Details.c_str();
				row.append_attribute("IdUser") = header.IdUser.c_str();
				row.append_attribute("IdNum") = header.IdNum;
			}
		}
	


		

	}
	return false;
}

bool NewMap::Read_DataBaseLinks_FromXmlFile(pugi::xml_node node)
{

	for (pugi::xml_node level1 = node.first_child(); level1; level1 = level1.next_sibling()) {
		string level1name = level1.name();
		size_t i_id=string::npos;

		for (size_t i = 0; i < 10; i++) {
			auto dbname=(DB_GetNames(i));
			FUtil::trim_space(dbname);
			if (dbname == level1name) {
				i_id = i;
				break;
			}

		}
		CDB* pDB;
		DB_OldHeader header;
		if (m_DB_Array.size() > i_id) {
			pDB = m_DB_Array[i_id];
			if (pDB != nullptr) {
				auto timestamp = level1.attribute("TimeStamp").as_uint();
				header.Name = level1.attribute("Name").as_string();
				header.SiteName = level1.attribute("SiteName").as_string();
				header.Type = level1.attribute("Type").as_uint();
				header.Latitude = level1.attribute("Latitude").as_string();
				header.Longitude = level1.attribute("Longitude").as_string();
				header.Country = level1.attribute("Country").as_string();
				header.DriveFile = level1.attribute("DriveFile").as_string();

				if (DB_types(i_id) == DB_types::Soil_Properties) {
					header.keyId = level1.attribute("KeyId").as_string();
				}
				else {
					header.Details = level1.attribute("Details").as_string();
					header.IdUser = level1.attribute("IdUser").as_string();
					header.IdNum = level1.attribute("IdNum").as_int();
				}
				m_DB_header[i_id] = header;
				DB_SetImportTimeT(i_id, timestamp);
			}
		}
	}
	return false;
}

string NewMap::GetParentDocFile(string xmlfile) {
	pugi::xml_document doc;
	auto result = doc.load_file(xmlfile.c_str());
	auto root = doc.root();
	auto child = root.first_child().first_child();
	auto item = child.first_child();
	string str = item.attribute("OriginalFileName").as_string();
	auto validchild = item.attribute("ChildDocument").as_bool();
	if (validchild)
		return str;
	else
		return "";
}

bool NewMap::ReadDocFile(string file)
{
	#ifndef COUPSTD
		CMainFrame *p_mainframe;
		p_mainframe = (CMainFrame*) AfxGetMainWnd();
		string txt;
		txt="Reading Document File:";
		txt+=file;
		p_mainframe->UpdatePaneText(0,txt);
	#endif
	if(file.size()>0) {
	//	PARAMETERS_mp_PARTY_OVER();
		SetPathName(file);

	}

	long long totlength=0;
	bool good=false;
	if(FUtil::IsFileExisting(file)) {


		ifstream DocFileIn;
		//DocFileIn.open(file, ios::in|ios::binary);
		m_DocFileIn.open(file, ios::in|ios::binary);
		m_DocFileIn.seekg(0,ios::beg);
		good=m_DocFileIn.good();
		string xmlfile;
		auto pos = m_DocFileName.find(".Sim");
		if (pos == string::npos) pos = m_DocFileName.find(".Sim");
		if (pos != string::npos) {
			xmlfile = m_DocFileName.substr(0, pos);
			xmlfile += ".xml";
		}
		if (pos == string::npos) {
			pos = m_DocFileName.find(".xml");
			if (pos != string::npos) xmlfile = m_DocFileName;

		}

	
		if (FUtil::IsFileExisting(xmlfile)) {
			Init_BlankDocument();
			if (ReadXmlDocFile(xmlfile)) {
#ifndef COUPSTD
				m_PlotStorage.Update(m_DocFile.m_SimulationRunNo, m_FileVersionNumberRead);
#endif
				return true;
			}
		}


	} 
	if(m_DocFileIn.is_open()) {
		m_DocFileIn.seekg(0,ios::end);
		totlength=m_DocFileIn.tellg();
		Init_BlankDocument();
		good=m_DocFileIn.good();
		if(totlength==0&&!good) {
			m_DocFileIn.close();
			return true;

		}
		else if(totlength==0) {
			m_DocFileIn.close();
			return true;

		};
	}


	if(m_DocFileIn.is_open()&&totlength>0) {



		long long actpos{ 0 };
		m_DocFileIn.seekg(0,ios::beg);
		good=m_DocFileIn.good();
		string problem;

		size_t ReadSectionCount=0;
		while(good&&ReadSectionCount<16) {

			switch(ReadSectionCount) {
			case 0:
				problem="Header";
				good=Info_Header(true);
				break;
			case 1:
				problem="Switches";
				good=Info_Switches(true);break;
			case 2:
				problem="Parameters";
				good=Info_Parameters(true);	break;
			case 3:
				problem="Parameter Tables";
				good=Info_ParTables(true); 
				if(!good&&m_continueRead) {
				//	MapVariables(); 
					m_DocFileIn.seekg(0,ios::beg);	
					good=m_DocFileIn.good();
					ReadSectionCount=-1;
				}
				break;
			case 4:
				problem="File Names";
				good=Info_Files(true);	break;
			case 5:
				problem="Outputs";
				good=Info_Outputs(true); break;
			case 6:
				problem="Validation Variables";
				good=Info_Validation(true);
				break;
			case 7:
				problem="Summary Validation";
				good=Info_SumValidation(true); break;
			case 8:
				problem="Run Reports";
				good=Info_RunReport(true); break;
			case 9:
				problem="Multi Run options";
				good=Info_MultiRun(true); break;
			case 10:
				problem="Within Run options";
				good=Info_WithinRunOptions(true); break;
			case 11:
				problem="Data base information";
				good=Info_DataBase(true); break;
			case 12:
				problem="Final State Variables";
				good=Info_FinalStateVariables(true); break;
			case 13:
				problem="Plottings";
				good=Info_Plottings(true); break;
			case 14:
				problem="History (log)";
				good=Info_History(true); break;
			case 15:
				problem="Annimation specification";
				good=Info_Annim(true); break;
			}
			if(good) {
				ReadSectionCount++;
				if(ReadSectionCount<16)
				actpos=m_DocFileIn.tellg();
				if(actpos<0) good=false; 
			}
		}

		if(m_Report_Array.size()>0) {
			m_HasBeenRunning=true;
			m_Protect=true;
			if(m_FileVersionNumberRead<36) m_DocFile.m_FinishedSimulation=true;
		}
		if(m_DocFileIn.good()) {
			m_DocFileIn.close();
		}
		else {
			m_DocFileIn.close();
		}
		auto iv=m_Val_Array.size();
		Tab *pTab=(Tab*)GetPtr(TABLE, "Scaling of Observed Variables");
		if(pTab!=nullptr) {
				auto oldnelements=pTab->GetNumOfRow();
		  		if (iv!=oldnelements&&iv!=string::npos)  pTab->SetNumOfRow(iv);
		}


		if(ReadSectionCount>14) {
			bool test = false;
			for (size_t i = 0; i < m_OptionsForReading.size(); i++) {
				test=LinkedChangeToDB_Parameters(m_OptionsForReading[i].pSimB, m_OptionsForReading[i].options);
			}
			if (test) m_OptionsForReading.clear();
			

// Completly read file
#ifdef COUPSTD
	cout<<" Reading of file succesful "; cout<<m_DocFileName; cout<<endl;

#endif


	#ifndef COUPSTD
			p_mainframe->UpdatePaneText(0,"Reading of file succesfully completed");
			
			m_PlotStorage.Update(m_DocFile.m_SimulationRunNo, m_FileVersionNumberRead);
	#endif
			return true;
		}

// Partly read file
		string mes;
		char buf[50];
//        printf(&buf[0],30,"%u", ReadSectionCount);
        mes="Only ";
        mes.append(buf);
		mes.append(" of 15 section read");
        mes.append("Error when reading:");
        mes.append(problem);
#ifndef COUPSTD
		MFC_Util::MessageBox( mes,"Input Error",	MB_OK|MB_ICONEXCLAMATION);
	


#endif
#ifdef COUPSTD
//    cout<<" Reading of file "; cout<<m_DocFileName; cout<<mes; cout<<endl;

#endif
			Init_BlankDocument();
			return true;
	}

	else {
// No file existing

			// Select Temperature as standard output for new blank file
			Init_BlankDocument();

#ifndef COUPSTD
			m_PlotStorage.Update(m_DocFile.m_SimulationRunNo, m_FileVersionNumberRead);
#endif

#ifdef COUPSTD
	cout<<" Reading of file failure, file was not find ("; cout<<m_DocFileName; cout<<")"; cout<<endl;

#endif

			return false;
	}

return true;

}

bool NewMap::ReadXmlDocFile(string xmlfile) {
	pugi::xml_document doc;
	auto result = doc.load_file(xmlfile.c_str());
	auto root = doc.root();
	auto child = root.first_child().first_child();
	Read_Header_FromXmlFile(child);
	
	for (child = child.next_sibling(); child; child = child.next_sibling()) {
		string levelname = child.name();
		if (levelname == "Output_Variables") {
			for (auto child2 = child.first_child(); child2; child2 = child2.next_sibling())
				Read_SimB_FromXmlFile(child2);
		}
		else if (levelname == "MultiRun_Settings") 		Read_MultiRunSettings_FromXmlFile(child);
		else if (levelname == "Validation") 			Read_Validation_FromXmlFile(child);
		else if (levelname == "Local_Database_Links")	Read_DataBaseLinks_FromXmlFile(child);
		else if (levelname == "Dynamic_Parameters")		Read_DynamicPar_FromXmlFile(child);
		else if (levelname == "AllFinalState")			Read_AllFinalState_FromXmlFile(child); 
		else if (levelname == "History") 				Read_History_FromXmlFile(child);
		else if (levelname == "Run_Report") 			Read_RunReport_FromXmlFile(child);
		else if (levelname == "Output_Summary") 		Read_OutputSummary_FromXmlFile(child);
		else if (levelname == "AllFinalState") 			Read_AllFinalState_FromXmlFile(child);
		else											Read_SimB_FromXmlFile(child);
	}
	//Post Setting to make sure the Document is complete
#ifndef COUPSTD
	m_PlotStorage.Update(m_DocFile.m_SimulationRunNo, m_FileVersionNumberRead);
#endif
	SetSimPeriodFromClimateFile();



	//if(IsBayesianCalibration()) {

	auto pSw = (Sw*)GetPtr(SWITCH, "TimeSerie Errors File");
	if (pSw != nullptr) {
		auto i = pSw->GetIntValue();
		if (i > 0)
			m_OutputValidXBinFileExist = true;
		else
			m_OutputValidXBinFileExist = false;
	}
	//}
	if (m_FileVersionNumberRead > 60 && m_ValidationData.GetNumSumVarVariables() > 0) {
		if (!m_ValidationData.IsSumValSet() && IsMultiSimulation() && IsFinishedSimulation()) {
			bool result;
			string pgfilename = GetOutputFileName(0, false, true);
			if (pgfilename.size() > 0) {
				m_PG_MultiOutputFile.Open(pgfilename);
				m_PG_MultiOutputFile.CloseFile();
			}
			result = CalculatePerformanceAndSetValues();
			m_ValidationData.SetSumValSet(result);
		}


	}

	m_xmlFileToUse = true;
	return true;

}

void NewMap::Init_BlankDocument() 
{
		string group, name, type, test;
		type="Auxiliary Variables";
		name="Temperature";
		group="Soil heat flows";
		
		OutVector* pStore;
		OutSingle* pSingle;

		pStore=dynamic_cast<OutVector*>(GetPtr(AUX,"Temperature"));
		pSingle=dynamic_cast<OutSingle*>(GetPtr(AUX_SINGLE,"TempSoilSurf"));
		if(pStore!=nullptr) {
				pStore->SetStoreFlag(7,true);
				pStore->SetStoreFlag(5,true);
				pStore->SetStoreFlag(2,true);
				pStore->SetStoreFlag(0,true);
				pSingle->SetStoreFlag(true);
		}
		RemoveOriginalValues("Outputs","ALL",false);
	
		m_pValidIndexSet=(Sw*)GetPtr("Switches", "Technical", "IndexForValidation");
		if (m_pValidIndexSet == nullptr) {
			m_pValidIndexSet = (Sw*)GetPtrByName("IndexForValidation");
		}


		Sw *pSw;
		pSw=(Sw*)GetPtr("Switches", "Technical", "ValidationMode");
		if (pSw != nullptr) {
			m_NumValidFiles = ((Sw*)GetPtr("Switches", "Technical", "ValidationMode"))->GetIntValue();
			m_pValidIndex = (Ps*)GetPtr("Parameters", "Technical", "Index in val file");
			m_pValidIndexRep = (Ps*)GetPtr("Parameters", "Technical", "No of internal index loop");
			m_pValidIndexLoopSize = (Ps*)GetPtr("Parameters", "Technical", "Size of internal loop");


		}

		m_ValidationData.Init(this);
		m_MStorage.Init(this);
}
bool NewMap::WriteDocFile(string localdirectory, bool DB_Source)
{
	if (m_IsUsingDB_Source) {
	
		 if (localdirectory.size() == 0) {

			  WriteDoc_To_Postgres(true,DB_Source);
		 }
		 else {

			 auto string = WriteEntireModelToXmlFile(doc_enabled::NOT_DEFAULT, localdirectory);
		 }
		 return true;
	}
	else if (m_xmlFileToUse) {
		auto string=WriteEntireModelToXmlFile(doc_enabled::NOT_DEFAULT);
		return true;
	}
	else {
		m_DocFileOut.open(m_DocFileName, ios_base::binary | ios_base::out);
		if (m_DocFileOut.is_open()) {

#ifndef COUPSTD
			CMainFrame* p_mainframe;
			p_mainframe = (CMainFrame*)AfxGetMainWnd();
			p_mainframe->UpdatePaneText(0, "Writing to Documument file Started");
#endif
			bool good = false;

			long long actpos = m_DocFileOut.tellp();
			good = Info_Header(false);
			actpos = m_DocFileOut.tellp();
			good = Info_Switches(false);
			actpos = m_DocFileOut.tellp();
			good = Info_Parameters(false);
			actpos = m_DocFileOut.tellp();
			good = Info_ParTables(false);
			actpos = m_DocFileOut.tellp();

			good = Info_Files(false);
			actpos = m_DocFileOut.tellp();
			good = Info_Outputs(false);
			actpos = m_DocFileOut.tellp();
			good = Info_Validation(false);
			good = Info_SumValidation(false);
			good = Info_RunReport(false);

			good = Info_MultiRun(false);
			good = Info_WithinRunOptions(false);
			good = Info_DataBase(false);

			good = Info_FinalStateVariables(false);
			good = Info_Plottings(false);
			good = Info_History(false);
			good = Info_Annim(false);


			m_DocFileOut.close();

#ifdef COUPSTD
			cout << "File writing completed " << m_DocFileName << endl;
#endif
#ifndef COUPSTD
			p_mainframe->UpdatePaneText(0, "Writing to Documument file Completed - shift to use xml instead of Sim also made");
#endif
			m_xmlFileToUse = true;
			m_CurrentFile = m_DocFileName;
			return true;
		}
		return false;
	}

}

bool NewMap::DeleteDoc_From_Postgres(int pkey) {
#ifdef COUP_POSTGRES
	if (pkey == -1) {
		return CleanFileAchivesFromUnUsedFiles();
	}

	string current_str, sql;
	try {
		if (!m_pCommonModelInfo->ID_MapsForPostgresReady) m_pCommonModelInfo->ID_MapsForPostgresReady = DefineUniqueIdMaps(m_pCommonModelInfo, this);
		pqxx::connection c = initconnection("Delete records from postgres");
		pqxx::work txn{ c };
		{
			pqxx::result r = txn.exec("DELETE FROM runinfo WHERE id_simulations = " + to_string(pkey)) ;
			r = txn.exec("DELETE FROM initial_final_states WHERE id_simulations = " + to_string(pkey));
			r=txn.exec("DELETE FROM modified_switch_values WHERE id_simulations = " + to_string(pkey));
			r=txn.exec("DELETE FROM modified_singleparameter_values WHERE id_simulations = " + to_string(pkey));
			r=txn.exec("DELETE FROM modified_vectorparameter_values WHERE id_simulations = " + to_string(pkey)) ;
			r= txn.exec("DELETE FROM modified_singleoutputs_resultvalues WHERE id_simulations = " + to_string(pkey)) ;
			r= txn.exec("DELETE FROM modified_singleoutputs_storevalues WHERE id_simulations = " + to_string(pkey)) ;
			r= txn.exec("DELETE FROM modified_vectoroutputs_resultvalues WHERE id_simulations = " + to_string(pkey)) ;
			r= txn.exec("DELETE FROM modified_vectoroutputs_storevalues WHERE id_simulations = " + to_string(pkey)) ;
			r= txn.exec("DELETE FROM filenamearchive_uses WHERE id_simulations = " + to_string(pkey)) ;
			r= txn.exec("DELETE FROM dynamic_parameters WHERE id_simulations = " + to_string(pkey)) ;
			r= txn.exec("DELETE FROM multirun_settings WHERE id_simulations = " + to_string(pkey)) ;
			r= txn.exec("DELETE FROM multirun_Results WHERE id_simulations = " + to_string(pkey)) ;
			r= txn.exec("DELETE FROM multirun_Residuals WHERE id_simulations = " + to_string(pkey)) ;
			r= txn.exec("DELETE FROM multirun_Ensemble_Statistics WHERE id_simulations = " + to_string(pkey)) ;
			r = txn.exec("DELETE FROM validation WHERE id_simulations = " + to_string(pkey));
			r = txn.exec("DELETE FROM multirun_ensembles WHERE id_simulations = " + to_string(pkey));
			r = txn.exec("DELETE FROM multirun_ensemble_definedcriteria WHERE id_simulations = " + to_string(pkey));
			r = txn.exec("DELETE FROM history_of_changes WHERE id_simulations = " + to_string(pkey));




			sql = "SELECT * FROM filenamearchive_uses WHERE id_simulations = " + to_string(pkey);
			r = txn.exec(sql);

			for (auto row : r) {
				int id_filename = row[1].as<int>();
				pqxx::result rr = txn.exec("SELECT COUNT(id_filename) FROM filenamearchive_uses");
				int num;
				for (auto rownum : rr) {
					num = rownum[0].as<int>();
				}
				if (num == 1) {
					r = txn.exec("DELETE FROM modified_timeseries_inputs WHERE id_filename = " + to_string(id_filename));			
					r = txn.exec("DELETE FROM filenamearchive_uses WHERE id_filename = " + to_string(id_filename));
					r = txn.exec("DELETE FROM filenamearchive_descriptions WHERE id_filename = " + to_string(id_filename));
					r = txn.exec("DELETE FROM filenamearchive_data WHERE id_filename = " + to_string(id_filename));
					r = txn.exec("DELETE FROM filenamearchive WHERE id_filename = " + to_string(id_filename));
				
				}

			}

			sql = "SELECT * FROM modified_timeseries_inputs WHERE id_simulations = " + to_string(pkey);
			r = txn.exec(sql);
			for (auto row : r) {
				int id_filename = row[2].as<int>();

				r = txn.exec("DELETE FROM modified_timeseries_inputs WHERE id_simulations = " + to_string(pkey));
				r = txn.exec("DELETE FROM filenamearchive_uses WHERE id_filename = " + to_string(id_filename));
				r = txn.exec("DELETE FROM filenamearchive_descriptions WHERE id_filename = " + to_string(id_filename));
				r = txn.exec("DELETE FROM filenamearchive_data WHERE id_filename = " + to_string(id_filename));
				r = txn.exec("DELETE FROM filenamearchive WHERE id_filename = " + to_string(id_filename));
			}
			//r= txn.exec("DELETE FROM multirun_Ensemble_DefinedCriteria WHERE id_simulations = " + to_string(pkey)) ;
  			r= txn.exec("DELETE FROM simulations WHERE id_simulations = " + to_string(pkey));

		}

		txn.commit();
		
	}
	catch (const std::exception& e) {
			cerr << e.what() << std::endl;
			cerr << sql;
			return false;
	}
	return true;
#endif

}

vector<int> NewMap::PreSelectDoc_From_Postgres(int pkey) {
	vector<int> allkeys_to_read;
	try {
		pqxx::connection c = initconnection("PreSelect from postgres");
		pqxx::work txn{ c };
		{
			int keys = pkey;
			vector<int> priorkeys;
			while (keys> 1) {

				pqxx::result r = txn.exec("SELECT childdocument, originalfilename FROM runinfo WHERE id_simulations = " + to_string(keys));

				for (auto row : r) {
					auto child = row[0].as<bool>();
					keys = 0;
					if (child) {
						auto document = row[1].as<string>();
						pqxx::result rr = txn.exec("SELECT * FROM simulations WHERE name = '" + document + "'");
						auto row_rr = rr.begin();
						auto keys = row_rr[0].as<int>();
						priorkeys.push_back(keys);
					}
				}
			}
			if (priorkeys.size() > 0) {
				allkeys_to_read.push_back(priorkeys.back());
				priorkeys.erase(priorkeys.end() - 1);
			}
			allkeys_to_read.push_back(pkey);
		}
	}
	catch (const std::exception& e) {
		cerr << e.what() << std::endl;
	}
	return allkeys_to_read;
}
bool NewMap::SelectDoc_From_Postgres(int pkey, bool init_call, bool download, string localdirectory, bool TimeSerieAsCSV) {
#ifdef COUP_POSTGRES
	string current_str;
	try {
		pqxx::connection c = initconnection("Select from postgres");
		pqxx::work txn{ c };
		{

	

			if (init_call) {
				if (!m_pCommonModelInfo->ID_MapsForPostgresReady) m_pCommonModelInfo->ID_MapsForPostgresReady = DefineUniqueIdMaps(m_pCommonModelInfo, this);
				ResetDocument_to_DefaultValues();
				if (pkey == 1) Init_BlankDocument();
			}
		


			

			m_DocFile.m_TimeCreated = time(nullptr);

			bool keyfind = false;
		    pqxx::result r= txn.exec("SELECT * FROM simulations WHERE id_simulations = " + to_string(pkey));
			for (auto row : r) {
				if (pkey == row[0].as<int>()) {
					keyfind = true;
					string name = row["name"].as<string>();
					name = FUtil::FileNameOnly(name);
					auto dotpos = name.find(".");
					if (dotpos != string::npos) name = name.substr(0, dotpos);
					m_CurrentFile=name;
					if ( name.rfind('_') != string::npos) {
						string str;
						str = name.substr(name.rfind('_')+1);
						m_DocFile.m_SimulationRunNo = FUtil::AtoInt(str);
					}
					m_DocFileName = name+".xml";
					m_SiteName = row["sitename"].as<string>();
				};
			};
			if (!keyfind) return false;


		}
		cout << m_DocFileName << endl;
		pqxx::result r = txn.exec("SELECT * FROM runinfo  WHERE id_simulations = " + to_string(pkey));
		auto row = r.begin();
		if(r.size()==0 && pkey == 1) return true;


		current_str = "RunInfo";

		
		if (pkey == row["id_simulations"].as<int>()) {
			m_DocFile.m_MultiRunning = row["multisimulation"].as<bool>();
			m_ChildDocument = row["childdocument"].as<bool>();
			m_DocFile.m_FinishedSimulation = row["finished"].as<bool>();
			RunOpt.longMinStart = row["simstartmin"].as<int>();
			RunOpt.longMinEnd = row["simendmin"].as<int>();
			m_DocFile.m_TimePeriodScaleFactor = row["simperiodscaling"].as<int>();
			RunOpt.longMinPreStart = row["priorsimperiod"].as<int>();
			RunOpt.longMinPostEnd = row["postsimperiod"].as<int>();
			auto pSw = dynamic_cast<Sw*>(GetPtr(SWITCH, "TimeResolution"));
			pSw->SetIntValue(row["timeresolution"].as<int>());
			RunOpt.oidays = row["outputinterval_days"].as<int>();
			RunOpt.oiminutes = row["outputinterval_minutes"].as<int>();
			RunOpt.noofiter = row["numberofiterations"].as<int>();
			m_DocFile.m_MultiStoragelocked = row["lockedmultistore"].as<bool>();
			m_DocFile.m_TimeCreated = row["date_created"].as<int>();
			m_DocFile.m_TimeModified = row["date_modified"].as<int>();
			m_FileVersionNumberRead = row["fileversionnumber"].as<int>();
			m_DocFile2.m_OriginFileName = m_DocFileName;

			m_ExeFileDate = row["exefiledate"].as<string>();
		}

		r = txn.exec("SELECT id_switch, value FROM modified_switch_values  WHERE id_simulations = " + to_string(pkey));
		current_str = "Modified_switch";
		vector<string> names1, names2;
		vector<int> ints1, ints2;
		vector<float> floats1, floats2;
		for (auto row : r) {
			string name = p_ModelInfo->GetSwitchName(row["id_switch"].as<int>());
			Sw* pSw = GetSw(name);
			if (pSw != nullptr) {
				size_t recalc=pSw->SetIntValue(row[1].as<int>());
				if(recalc>0) LinkedChangeToSwitches(pSw, recalc);
				pSw->SetNotOldValue();
			}

			names1.push_back(name);
			ints1.push_back(row["id_switch"].as<int>());
		}
		r = txn.exec("SELECT S.name,M.id_switch, M.value FROM switches AS S JOIN modified_switch_values AS M ON S.id_switch = M.id_Switch WHERE M.id_simulations = " + to_string(pkey));
		for (auto row : r) {
			string name = row[0].as<string>();
			Sw* pSw = GetSw(name);
			if (pSw != nullptr) {
				size_t recalc=pSw->SetIntValue(row[2].as<int>());
				if (recalc > 0) {
					LinkedChangeToSwitches(pSw, recalc);
				}
			}
			names2.push_back(name);
			ints2.push_back(row["id_switch"].as<int>());
		}

		r = txn.exec("SELECT id_singlepar, value FROM modified_singleparameter_values  WHERE id_simulations = " + to_string(pkey));
		current_str = "Modified_singleparameters";

		for (auto row : r) {
			string name = p_ModelInfo->GetSingleParameterName(row["id_singlepar"].as<int>());
			Ps* pPs = GetPs(name);
			double NewValue = row[1].as<float>();
			if (pPs != nullptr&&pPs->GetMinValue()<=NewValue&&pPs->GetMaxValue()>=NewValue) pPs->SetValue(NewValue);
			names1.push_back(name);
			floats1.push_back(row["id_singlepar"].as<float>());
		}
		r = txn.exec("SELECT S.name,M.id_singlepar, M.value FROM singleparameters AS S JOIN modified_singleparameter_values AS M ON S.id_singlepar = M.id_singlepar WHERE M.id_simulations = " + to_string(pkey));
		for (auto row : r) {
			string name = row[0].as<string>();
			Ps* pPs = GetPs(name);
			double NewValue = row[2].as<float>();
			if (pPs != nullptr && pPs->GetMinValue() <= NewValue && pPs->GetMaxValue() >= NewValue) pPs->SetValue(NewValue);
			names2.push_back(name);
			floats2.push_back(row["id_singlepar"].as<float>());
		}

		//vector par
		r = txn.exec("SELECT id_vectorpar, arraysize, values FROM modified_vectorparameter_values  WHERE id_simulations = " + to_string(pkey));
		current_str = "Modified vectorpar";

		for (auto row : r) {
			string name = p_ModelInfo->GetVectorParameterName(row["id_vectorpar"].as<int>());
			P* pP = GetP(name);
			int number_elements = row[1].as<int>();
			if (pP != nullptr) {
				if (pP->GetSize() != number_elements) {
					pP->ReSize(number_elements);
					pP->SetSize(number_elements);
				}
				auto pvector = row[2].as_array();

				auto next = pvector.get_next();
				next = pvector.get_next();
				size_t count{ 0 }; double value;
				size_t ReCalcTot = 0;

				while (next.second.size() > 0) {
					size_t ReCalc = 0;
					value = stod(string(next.second));
					bool newvalid = AssignNewValue_toP(pP, count, value, ReCalc);
					if (ReCalc > 0) ReCalcTot = ReCalc;

					//pP->SetValue(count, value);
					next = pvector.get_next();
					count++;
				}
				if (ReCalcTot > 0) LinkedChangeToParameters(pP, ReCalcTot);
			}

		}
		r = txn.exec("SELECT S.id_vectorname,M.id_vectorpar,M.arraysize, M.values FROM vectorparameters AS S JOIN modified_vectorparameter_values AS M ON S.id_vectorpar = M.id_vectorpar WHERE M.id_simulations = " + to_string(pkey));
		for (auto row : r) {   
			string name = row[0].as<string>();
			P* pP = GetP(name);
			int number_elements = row[2].as<int>();
			if (pP != nullptr) {
				if (pP->GetSize() != number_elements) {
					pP->SetSize(number_elements);
				}
				auto pvector = row[3].as_array();
				auto next = pvector.get_next();
				next = pvector.get_next();
				size_t count{ 0 }; double value;
				while (next.second.size() > 0) {
					value = stod(string(next.second));
					pP->SetValue(count, value);
					next = pvector.get_next();
					count++;
				}
			}
		}




		// outputs
		r = txn.exec("SELECT id_singleoutputs, storeflag FROM modified_singleoutputs_storevalues  WHERE id_simulations = " + to_string(pkey));

		current_str = "Modified_single outputs storevalues";

		for (auto row : r) {
			string name = p_ModelInfo->GetSingleOutputName(row["id_singleoutputs"].as<int>());
			OutSingle* pP = GetSingleOutputPtr(name);
			if (pP != nullptr) pP->SetStoreFlag(row[1].as<int>());
		}
		r = txn.exec("SELECT S.name,M.id_singleoutputs, M.storeflag FROM singleoutputs AS S JOIN modified_singleoutputs_storevalues AS M ON S.id_singleoutputs = M.id_singleoutputs WHERE M.id_simulations = " + to_string(pkey));
		for (auto row : r) {
			string name = row[0].as<string>();
			OutSingle* pP = GetSingleOutputPtr(name);
			if (pP != nullptr) pP->SetStoreFlag(row[2].as<int>());
		}
		r = txn.exec("SELECT id_singleoutputs, initial, final, min, max, mean, accumulated,singlerun_out_index, multirun_out_index  FROM modified_singleoutputs_resultvalues  WHERE id_simulations = " + to_string(pkey));

		for (auto row : r) {
			string name = p_ModelInfo->GetSingleOutputName(row["id_singleoutputs"].as<int>());
			OutSingle* pP = GetSingleOutputPtr(name);
			if (pP != nullptr) pP->AddSumVar(row[1].as<float>(), row[2].as<float>(), row[3].as<float>(), row[4].as<float>(), row[5].as<float>(), row[6].as<float>(),
				row[7].as<int>(), row[8].as<int>());
		}


		r = txn.exec("SELECT id_vectoroutputs, numberstoreflag, storeflags FROM modified_vectoroutputs_storevalues  WHERE id_simulations = " + to_string(pkey));

		current_str = "Modified_vector outputs storevalues";

		for (auto row : r) {
			string name = p_ModelInfo->GetVectorOutputName(row["id_vectoroutputs"].as<int>());
			OutVector* pP = GetVectorOutputPtr(name);
			int number_elements = row[1].as<int>();
			if (pP != nullptr) {
				if (pP->GetNumVector() != number_elements) {
					int skallintebli = 0;
				}
				auto pvector = row[2].as_array();

				auto next = pvector.get_next();
				next = pvector.get_next();
				size_t count{ 0 }; double value;

				while (next.second.size() > 0) {
					value = stoi(string(next.second));
					pP->SetStoreFlag(count, value);
					next = pvector.get_next();
					count++;
				}
			}


			//	if (pP != nullptr) pP->SetStoreFlag(row[1].as<int>());
		}
		r = txn.exec("SELECT S.name,M.id_singleoutputs, M.storeflag FROM singleoutputs AS S JOIN modified_singleoutputs_storevalues AS M ON S.id_singleoutputs = M.id_singleoutputs WHERE M.id_simulations = " + to_string(pkey));
		for (auto row : r) {
			string name = row[0].as<string>();
			OutSingle* pP = GetSingleOutputPtr(name);
			if (pP != nullptr) pP->SetStoreFlag(row[2].as<int>());
		}

		r = txn.exec("SELECT id_vectoroutputs, numindexout,indexout, initial, final, min, max, mean, accumulated,singlerun_out_index, multirun_out_index  FROM modified_vectoroutputs_resultvalues  WHERE id_simulations = " + to_string(pkey));

		current_str = "Modified_vectoroutputs resultsvalues";
		for (auto row : r) {
			string name = p_ModelInfo->GetVectorOutputName(row["id_vectoroutputs"].as<int>());
			OutVector* pP = GetVectorOutputPtr(name);
			int numindexout = row[1].as<int>();
			auto pindexvector = row[2].as_array();

			auto next = pindexvector.get_next();
			next = pindexvector.get_next();
			size_t count{ 0 }; int i_value;

			vector<int> index; index.resize(numindexout);
			while (next.second.size() > 0) {
				i_value = stoi(string(next.second));
				index[count] = i_value - 1;
				next = pindexvector.get_next();
				count++;
			}

			vector< vector<double>> fvector;
			vector<double> finnervector;

			finnervector.resize(numindexout);
			for (size_t i = 0; i < 6; ++i) {
				auto item = row[3 + i].as_array();
				auto next = item.get_next();
				next = item.get_next();
				count = 0;
				while (next.second.size() > 0) {
					finnervector[count] = stod(string(next.second));
					next = item.get_next();
					count++;
				}
				fvector.push_back(finnervector);
			}
			vector< vector<int>> i_vector;
			vector<int> i_innervector;
			i_innervector.resize(numindexout);
			for (size_t i = 0; i < 2; ++i) {
				auto item = row[9 + i].as_array();
				auto next = item.get_next();
				next = item.get_next();
				count = 0;
				while (next.second.size() > 0) {
					i_innervector[count] = stoi(string(next.second));
					next = item.get_next();
					count++;
				}
				i_vector.push_back(i_innervector);
			}

			for (size_t i = 0; i < i_vector[0].size(); i++) {
				pP->AddSumVar(i, fvector[0][i], fvector[1][i], fvector[2][i], fvector[3][i], fvector[4][i], fvector[5][i], i_vector[0][i], i_vector[1][i]);
			}


		}

		// PG Files

		r = txn.exec("SELECT id_timeserie, filename FROM modified_timeseries_inputs  WHERE id_simulations = " + to_string(pkey));
		current_str = "Modified_timeseries_inputs";

		map<string, CPG*> links_to_pg_pointers;
		for (auto row : r) {
			CPG* pPG;
			int id_timeserie = row["id_timeserie"].as<int>();
			string name = p_ModelInfo->GetTimeSeriesName(id_timeserie);
			bool savefilename_as_modelfilename = true;
			bool save_as_Output_Files=false;
			if (download) {
				
				F* pF = GetF(name);
				string filename = row["filename"].as<string>();
				
				if (localdirectory.size() > 0) {
					filename = localdirectory + FUtil::FileNameOnly(filename);
					pF->SetValue(filename);
				}
				else {
					pF->SetValue(filename);
				}

				if (name == "Output File"||name.find("ValidationOutput")!=string::npos)	save_as_Output_Files = true;
				if (!TimeSerieAsCSV) save_as_Output_Files = true;
				if (save_as_Output_Files) {
					pPG = pF->GetPointer();
					links_to_pg_pointers.insert(pair<string, CPG*>(FUtil::FileNameOnly(filename,true), pPG));
				}

			}
			else {
				F* pF = GetF(name);
				string filename = row["filename"].as<string>();
				if (name == "Output File"|| name.find("ValidationOutput") != string::npos)	savefilename_as_modelfilename = false;
				if (savefilename_as_modelfilename) {
					pPG = pF->GetPointer();
					links_to_pg_pointers.insert(pair<string, CPG*>(filename, pPG));
				}
			}
		}

		r = txn.exec("SELECT id_simulations, id_filename FROM filenamearchive_uses  WHERE id_simulations = " + to_string(pkey));
		current_str = "filenamearchives";
		for (auto row : r) {
			int id_simulat = row["id_simulations"].as<int>();
			int id_filename = row["id_filename"].as<int>();
			bool donotload_outputs = false;
			bool SaveToFile = false;
			
			pqxx::result rr = txn.exec("SELECT id_filename, filename, numvar, numrecords FROM filenamearchive  WHERE id_filename = " + to_string(id_filename));
			string filename;
			for (auto row_inner : rr) {
				int numvar = row_inner["numvar"].as<int>();
				int numrec = row_inner["numrecords"].as<int>();
				filename = row_inner["filename"].as<string>();
			}
			if (download) {
				for (auto link : links_to_pg_pointers) {
					if (filename.find(link.first)!=string::npos) SaveToFile = true;
				}
				if (SaveToFile) {
					if (TimeSerieAsCSV) {
						download_pg_file(id_filename, localdirectory, TimeSerieAsCSV, m_pCommonModelInfo);
					}
					else
						download_pg_file(id_filename, localdirectory, TimeSerieAsCSV, m_pCommonModelInfo);
				}
			}
			else if(links_to_pg_pointers.size()>0&&!donotload_outputs) {
				load_pg_file(id_filename,links_to_pg_pointers);

			}


		}

		// Validation
		r = txn.exec("SELECT * FROM Validation  WHERE id_simulations = " + to_string(pkey));
		m_Val_Array.clear();
		m_ValSum_Array.clear();
		current_str = "Validations";

		VALv vst;
		for (const auto row : r) {
			vst.NSE = MISSING;
			vst.ValidationFileIndex = row[1].as<int>();
			vst.OutputType = row[2].as<int>();
			vst.Group = row[3].as<string>();
			vst.Name = row[4].as<string>();
			vst.LocalIndex = row[5].as<int>();
			vst.ValidationFileNumber = row[6].as<int>();
			vst.Monitoring = row[8].as<int>();
			vst.P_Error = row[9].as<float>();
			vst.A_Error = row[10].as<float>();
			vst.AccTest = row[11].as<bool>();
			vst.LogTrans = row[12].as<bool>();
			vst.Duration = row[13].as<int>();
			vst.nNumber = row[14].as<int>();
			vst.R2 = row[15].as<float>();
			vst.A0 = row[16].as<float>();
			vst.A1 = row[17].as<float>();
			vst.ME = row[18].as<float>();
			vst.RMSE = row[19].as<float>();
			vst.MeanSim = row[20].as<float>();
			vst.MeanVal = row[21].as<float>();
			vst.LogLi = row[22].as<float>();
			vst.NSE = row[23].as<float>();		
			m_Val_Array.push_back(vst);
		}

		// MultiRun 
		size_t DimNumber = 1;
		size_t count_par = 0;
		MRv valMR;
		m_MultiRun_Array.clear();
		m_P_MR_Array.clear();

		r = txn.exec("SELECT * FROM multirun_settings WHERE id_simulations = '" + to_string(pkey)+"'");
		

		for (const auto row : r) {
			auto idim=row["dimension"].as<int>();
			auto id_group = row["id_group"].as<int>();
			string parname = row["name"].as<string>();
			int vector_index = row["vectorindex"].as<int>();
			auto id_method = row["id_method"].as<int>();
			auto monitoring = row["monitoring"].as<int>();
			auto minvalue = row["min"].as<float>();
			auto maxvalue = row["max"].as<float>();
			auto startvalue = row["start"].as<float>();
		
			string dependence_parname = row["dependence_name"].as<string>();
			int dependence_index = row["dependence_index"].as<int>();
			auto  fixedvaluevector = row["fixedvalues"].as_array(); 
			auto fixedkey  = row["fixedkey"].as_array();
			auto fixedkeyvalue = row["fixedkeyvalue"].as_array();
			int numrep = row["numberrepetition"].as<int>();

			SIMB valpar;
			if (vector_index == -1) {
				auto pSim = GetPsPointer(parname);
				if (pSim == nullptr) FUtil::trim_xmlstring(parname);
				pSim = GetPsPointer(parname);
				if (pSim != nullptr) {
					valpar.pBase = pSim;
					valpar.TabIndex = string::npos;
					static_cast<Ps*>(valpar.pBase)->MR_Set_Monitoring(size_t(monitoring));
					static_cast<Ps*>(valpar.pBase)->MR_Set_Method(MR_METHOD(id_method));
					valpar.TabIndex = string::npos;
					if (MR_METHOD(id_method) == MR_METHOD::TABLE_VAL) {
						auto next = fixedvaluevector.get_next();
						next = fixedvaluevector.get_next();
						float value;
						while (next.second.size() > 0) {
							value = stof(string(next.second));
							static_cast<Ps*>(valpar.pBase)->MR_Append_TableValue(value);
							next = fixedvaluevector.get_next();
						}
					}
					if (MR_METHOD(id_method) >= MR_METHOD::SAME_AS && MR_METHOD(id_method) <= MR_METHOD::OP_REL_SAME_AS) {
						Ps* pParDep = GetPsPointer(dependence_parname);
						if (pParDep != nullptr) {
							if (pParDep->Is_P()) pParDep->Set_MR_ParDependence(pParDep);
							else {
								auto pParDep2 = GetPPointer(dependence_parname);
								if (pParDep2 != nullptr && pParDep2->Is_Vector()) pParDep->Set_MR_ParDependence(static_cast<SimB*>(pParDep2), dependence_index);
							}
						}
					}
					if (MR_METHOD(id_method) > MR_METHOD::LOG_CHANGE) {
						pSim->MR_Set_Min(minvalue);
						pSim->MR_Set_Max(maxvalue);
					}


					

				}
			}
			else {
					auto pSim = GetPPointer(parname);
					if (pSim == nullptr) FUtil::trim_xmlstring(parname);
					pSim = GetPPointer(parname);
					if (pSim != nullptr) {
						valpar.pBase = pSim;
						valpar.TabIndex = vector_index;
						if (MR_METHOD(id_method) == MR_METHOD::TABLE_VAL) {
							auto next = fixedvaluevector.get_next();
							next = fixedvaluevector.get_next();
							float value;
							while (next.second.size() > 0) {
								value = stof(string(next.second));
								static_cast<P*>(valpar.pBase)->MR_Append_TableValue(valpar.TabIndex, value);
								next = fixedvaluevector.get_next();
							}
						}
						if (MR_METHOD(id_method) >= MR_METHOD::SAME_AS && MR_METHOD(id_method) <= MR_METHOD::OP_REL_SAME_AS) {
							P* pParDep = GetPPointer(dependence_parname);
							if (pParDep != nullptr) {
								if (pParDep->Is_Vector()) static_cast<P*>(valpar.pBase)->Set_MR_ParDependence(valpar.TabIndex, pParDep, dependence_index);
								else {
									auto pParDep2 = GetPsPointer(dependence_parname);
									if (pParDep2 != nullptr && pParDep2->Is_P()) static_cast<P*>(valpar.pBase)->Set_MR_ParDependence(valpar.TabIndex, static_cast<SimB*>(pParDep2));
								}
							}
						}
						if (MR_METHOD(id_method) > MR_METHOD::LOG_CHANGE) {
								pSim->MR_Set_Min(valpar.TabIndex, minvalue);
								pSim->MR_Set_Max(valpar.TabIndex, maxvalue);
						}
						
					}

			}
			if (valpar.pBase->Is_P())
				static_cast<Ps*>(valpar.pBase)->MR_Set_Dim(DimNumber);
			else
				static_cast<P*>(valpar.pBase)->MR_Set_Dim(valpar.TabIndex, DimNumber);
			
			m_P_MR_Array.push_back(valpar);
			if (idim == DimNumber && count_par == 0||idim>DimNumber) {
				DimNumber = idim;
				valMR.nCount = DimNumber;
				valMR.NumberOfRepeatitionsWithinDimension = numrep;
				m_MultiRun_Array.push_back(valMR);
				count_par++;
			}
			else
				count_par++;

		}
		if (download&&!TimeSerieAsCSV) {
			m_IsUsingDB_Source = true;
			WriteDocFile(localdirectory);

		}
		{   //Final adjustment of settings after all information is assigned
			SetSimPeriodFromClimateFile();


		}
		return true;
	}
	catch (const std::exception& e) {
		cerr << e.what() << std::endl;
		cerr << current_str;
		return false;
	}

#endif
	return false;

};

int NewMap::GetCurrentRunNoFromPostgres() {
#ifdef COUP_POSTGRES
	string current_str;
	try {
		pqxx::connection c = initconnection("Select from postgres");
		pqxx::work txn{ c };
		pqxx::result rr = txn.exec("SELECT id_item, name, intnumber FROM register WHERE name = 'RunNo' ");

		int runno;
		for (auto row : rr) {
			auto id= row[0].as<int>();
			auto name = row[1].as<string>();
			runno = row[2].as<int>();
		}

		return runno;
	}
	catch (const std::exception& e) {
		cerr << e.what() << std::endl;
	}
	return false;
#endif	

};
bool NewMap::SetCurrentRunNoInPostgres() {
#ifdef COUP_POSTGRES
	string current_str;
	try {
		pqxx::connection c = initconnection("Select from postgres");
		pqxx::work txn{ c };

		//pqxx::result rr = txn.exec("SELECT id_item, name, intnumber FROM register WHERE name=RunNo");
		m_DocFile.m_SimulationRunNo;
		auto rr = txn.exec("UPDATE register SET intnumber = "+to_string(m_DocFile.m_SimulationRunNo)+ " WHERE name = 'RunNo' ");

		txn.commit();
		return true;
	}
	catch (const std::exception& e) {
		cerr << e.what() << std::endl;
	}
	return false;
#endif	


};

bool NewMap::DownLoadTimeSerie(int id_file, string filebase, string localdirectory, bool TimeSerieAsCSV) {
#ifdef COUP_POSTGRES
	string current_str;
	try {
		pqxx::connection c = initconnection("Select from postgres");
		pqxx::work txn{ c };



		pqxx::result rr = txn.exec("SELECT id_filename, filename, numvar, numrecords FROM filenamearchive") ;
		string filename;
		std::map<int,string> fil_id_names;
		for (auto row_inner : rr) {
			int numvar = row_inner["numvar"].as<int>();
			int numrec = row_inner["numrecords"].as<int>();
			filename = row_inner["filename"].as<string>();
			int id_filename = row_inner["id_filename"].as<int>();
			if (filename.find(filebase) != string::npos) {
				fil_id_names.insert(pair<int, string>(id_filename, filename));
			}
		}

		for (auto i = fil_id_names.begin(); i != fil_id_names.end();++i) {
			id_file = i->first;
			if (TimeSerieAsCSV)
				download_pg_file(id_file, localdirectory, TimeSerieAsCSV, m_pCommonModelInfo);
			else
				download_pg_file(id_file, localdirectory, TimeSerieAsCSV, m_pCommonModelInfo);
		}

		return true;

	}
	catch (const std::exception& e) {
		cerr << e.what() << std::endl;
	}
	return false;
#endif	

}
bool NewMap::Export_OLDSOILDB_toPostgres() {

	string sql;

	if (p_PFCurve == nullptr) {
		p_PFCurve = make_unique<PFCurve>();
	}

	p_PFCurve->CloseMainDataBase();
	p_PFCurve->OpenMainFile(m_DataBaseDirectory + "PfProf");
	p_PFCurve->ReadHeaderFile();
	p_PFCurve->SetDataBaseDirectory(m_DataBaseDirectory);
	try {
		connection c = initconnection("Soil Database");
		pqxx::work W{ c };
		int id_profile{ 0 }, id_layer{ 0 };
	
		size_t newcount = 10000;
		for (size_t i = 0; i < p_PFCurve->GetNumProfiles(); i++) {
			sql ="INSERT INTO soil_profiles VALUES(";
			auto prof = p_PFCurve->GetProfile(i);
			sql += "Default,";
			if (prof.KeyProf > 1000000) {
				newcount++;
				prof.KeyProf = newcount;
			}
			sql += to_string(prof.KeyProf) + ",'";
			FUtil::trim_xmlstring(prof.Name);
			sql += prof.Name + "','";
			sql += prof.Country + "','";
			sql += prof.County + "',";
			sql += to_string(prof.Long) + ",";
			sql += to_string(prof.Lat) + ",";
			sql += to_string(prof.NumLayers) + ",";
			sql += to_string(prof.LowDepth) + ",";
			sql += to_string(prof.Upper_Clay) + ",";
			sql += to_string(prof.Upper_Sand) + ",";
			sql += to_string(prof.Upper_Org) + ",";
			sql += to_string(prof.Lower_Clay) + ",";
			sql += to_string(prof.Lower_Sand) + ",";
			sql += to_string(prof.Lower_Org) + ",";
			sql += to_string(prof.Year) + ",'";
			string soiltype = "agriculture";
			if (prof.KeyProf > 10000) soiltype = "forest";
			if (prof.KeyProf > 40100) soiltype = "agriculture";
			if (prof.KeyProf > 50000) soiltype = "Organic";
 			sql += soiltype + "') returning id_profile;";		
			result r=W.exec(sql.c_str());
			id_profile = r.begin().at(0).as<int>();

			auto koll=p_PFCurve->SetHeader(i);
			if (koll) {
				auto pressure = p_PFCurve->GetPF_Pressure(prof.RecLayers);
				auto lf = [](float value) {
					auto str = to_string(value) + ","; return str;
				};
				auto li = [](int value) {
					auto str = to_string(value) + ","; return str;
				};
				auto lfv = [](vector<float> fv) {
					string s = "'{"; for (size_t i = 0; i < fv.size(); i++) {
						s += to_string(fv[i]); if (i < fv.size() - 1) s += ",";
					}
					s += "}',"; return s;
				};
				for (size_t ilayer = 0; ilayer < prof.NumLayers; ilayer++) {
					sql = "INSERT INTO soil_layers VALUES("; sql += "Default,";
					auto thetas = p_PFCurve->GetPF_Theta(ilayer);
					auto coefs = p_PFCurve->GetPF_Coef(ilayer);
					auto textures = p_PFCurve->GetTexture(ilayer);

					sql += lf(coefs.UpperDepth); sql += lf(coefs.LowerDepth);
					sql += li(0);//soiltype
					 sql += lf(0.); // drybulk
					 sql += lf(2.65); // particledensity
					 sql += lf(textures[7]); // organic content
					 sql += lf(0.); // carbon content
					 sql += lf(0.); // nitrogen content
					 sql += lf(0.); // P content
					 sql += lfv(textures); // texture
					 sql += lf(textures[0]); //clay
					 sql += lf(textures[0]); // silt
					 sql += lf(textures[6]); // sand
					 sql += lf(0.); // gravel
					 sql += li(pressure.size()); // numberofheads
					 sql += lfv(pressure);
					 sql += lfv(thetas);
					 sql += lf(coefs.Saturation);
					 sql += lf(coefs.Wilting);
					 sql += lf(coefs.Residual);
					 sql += lf(coefs.AirEntry);
					 sql += lf(coefs.Lambda);
					 sql += lf(coefs.UpperBoundary);
					 sql += lf(coefs.LowerBoundary); //Macropores
					 sql += lf(coefs.Gen_Alfa); 
					 sql += lf(coefs.Gen_M); 
					 sql += lf(coefs.Gen_N); 
					 sql += lf(coefs.TotConductivity); 
					 sql += lf(coefs.MatConductivity);
					 sql += lf(coefs.Mualem_n);
					 sql += lf(coefs.N_SrCoef);
					 sql += to_string(coefs.N_SECoef)+") returning id_layers;";
					 result r = W.exec(sql.c_str());
					 id_layer = r.begin().at(0).as<int>();
					 
					 sql = "INSERT INTO soil_profile_layer_linking VALUES (";
					 sql += to_string(id_profile) + "," + to_string(id_layer) + "," + to_string(0.)+ "," + to_string(0.)+")";
					 W.exec(sql.c_str());
				}
				sql = "INSERT INTO soil_profile_details VALUES (";
				string note = p_PFCurve->GetComments();
				FUtil::trim_xmlstring(prof.CreatedBy);
				FUtil::trim_xmlstring(prof.ModifiedBy);
				FUtil::trim_xmlstring(note);
				sql += to_string(id_profile) + ",'";
				sql += note + "','" +prof.CreatedBy + "','" + prof.CreateDate + "','" + prof.ModifiedBy +"','"+prof.ModifiedDate+ "')";
				W.exec(sql.c_str());
			}
		}
		W.commit();
		return true;
	}
	catch (const std::exception& e) {
		cerr << e.what() << std::endl;
		cerr << sql << std::endl;
		return -1;
	}

}
bool NewMap::WriteDoc_To_Postgres(bool UpdatedRecord, bool DB_Source ) {
#ifdef COUP_POSTGRES

	if (!m_pCommonModelInfo->ID_MapsForPostgresReady) m_pCommonModelInfo->ID_MapsForPostgresReady=DefineUniqueIdMaps(m_pCommonModelInfo, this);
	

	if (UpdatedRecord) {
		auto name = m_DocFileName;
		if (name.rfind('_') != string::npos) {
			string str;
			str = name.substr(0, name.rfind('_')+1);
			m_DocFileName = str + FUtil::ItoNumAscii(m_DocFile.m_SimulationRunNo) + ".xml";
		}
	}
	
	m_DocFile2.m_DateRun = FUtil::GetCurrentDateTime();
	bool keyfind = false;
	sim_doc_simulation t;
	t.comment = m_DocFile2.m_Comments;
	t.simno = m_DocFile.m_SimulationRunNo;
	t.name = FUtil::FileNameOnly(m_DocFileName,true);

	pair<string, unique_ptr<Register>> p = FUtil::GetProfileStringStd("Creator", "NN", move(m_pRegister));
	t.creator = p.first; m_pRegister = move(p.second);
	if (m_SiteName.size() == 0) {
		p = FUtil::GetProfileStringStd("SiteNameId", "-", move(m_pRegister));
		t.sitename = p.first; m_pRegister = move(p.second);
	}
	else t.sitename = m_SiteName;
	if (t.comment.size() == 0) {
		p= FUtil::GetProfileStringStd("Comment", "-", move(m_pRegister));
	    t.comment = p.first; m_pRegister = move(p.second);
	}
	
	// Set SimulationRunNo

	pair<int, unique_ptr<Register>> pn = FUtil::GetProfileIntNo("SimulationRunNo", 1, move(m_pRegister));
	auto maxno = max(int(m_DocFile.m_SimulationRunNo), pn.first); m_pRegister = move(pn.second);

	if (maxno == int(m_DocFile.m_SimulationRunNo)|| pn.first == maxno) m_pRegister = FUtil::WriteProfileInt("SimulationRunNo", maxno+1, move(m_pRegister));
	

	m_SiteName = t.sitename;
	

	int pkey = transfer_Document(t);
	if (pkey == -1) {
		cout << "Document already exist - please change name or remove previous document: "+ m_DocFileName<<endl;
		return false;
	}
	// Runinfo
	{ //Runinfo
		run_info_document r;
		r.key_simulation = pkey;
		r.fileversionnumber = m_FileVersionNumberRead;
		r.originalfilename = m_DocFile2.m_OriginFileName;
		r.childdocument = m_ChildDocument;
		if (m_DocFile2.m_ExeFileDate.size() == 0) {
			m_DocFile2.m_ExeFileDate = "Version 6.2.2 ";
		}
		r.exefiledata = m_DocFile2.m_ExeFileDate;
		r.multisimulation = m_DocFile.m_MultiRunning;
		r.finished = m_DocFile.m_FinishedSimulation;
		r.simstartmin = RunOpt.longMinStart;
		r.simendmin = RunOpt.longMinEnd;
		r.simperiodscaling = m_DocFile.m_TimePeriodScaleFactor;
		r.priorsimperiod = RunOpt.longMinPreStart;
		r.postsimperiod = RunOpt.longMinPostEnd;
		auto pSw = dynamic_cast<Sw*>(GetPtr(SWITCH, "TimeResolution"));
		r.timeresolution = pSw->GetIntValue();
		r.output_interval_days = RunOpt.oidays;
		r.output_interval_minutes = RunOpt.oiminutes;
		r.numberofiterations = RunOpt.noofiter;
		r.lockedmultistore = m_DocFile.m_MultiStoragelocked;
		r.date_created = m_DocFile.m_TimeCreated;
		r.date_modified = m_DocFile.m_TimeModified;
		int koll = transfer_RunInfo_Document(r);
	}

	// Switches
	{
		vector<switch_set> s;
	//	RemoveOriginalValues("Switches", "ALL", false);

		auto pSimVector = GetPtrVector(SWITCH, false, false);
		for (auto pSim : pSimVector) {
			Sw* pSw = static_cast<Sw*>(pSim);
			switch_set ss;
			ss.key_simulation = pkey;
			ss.id_switch = p_ModelInfo->GetSwitchId(pSw->GetName());
			ss.value = pSw->GetIntValue();
			s.push_back(ss);
		}

		if(s.size()>0) transfer_Modified_Switches(s);
	}

	// Single Par
	{
		vector<singlepar_set> s;
	//	RemoveOriginalValues("Parameters", "ALL", false);

		auto pSimVector = GetPtrVector(PAR_SINGLE, false, false);
		for (auto pSim : pSimVector) {
			Ps* pP = static_cast<Ps*>(pSim);
			singlepar_set ss;
			ss.key_simulation = pkey;
			ss.id_singlepar = p_ModelInfo->GetSingleParId(pP->GetName());
			ss.value = pP->GetValue();
			s.push_back(ss);
		}
		if(s.size()>0) int koll = transfer_Modified_SinglePar(s);
	}
	// vector Par
	{
		vector<vectorpar_set> s;
		auto kollv = GetPtrVector(PAR_TABLE, true, true);
		m_Pt_Array.clear();
		for (auto k : kollv) {
			if (k->IsNotOriginalValue()) m_Pt_Array.push_back(static_cast<P*>(k));
		}

		
		for (P* pPt : m_Pt_Array) {
			vectorpar_set ss;
			ss.key_simulation = pkey;
			ss.id_vectorpar = p_ModelInfo->GetVectorParId(pPt->GetName());

			for (size_t i = 0; i < pPt->GetSize(); i++) {
				ss.values.push_back(pPt->GetValue(i));
			}
			s.push_back(ss);
		}
		if(s.size()>0)  transfer_Modified_VectorPar(s);
	}
	// Vector outputs
	{
		vector<vectoroutput_set> s;
		vector<SimB*> outputs;

		vector<SimB*> vall;
		vall = GetPtrVector(STATE, false);

		outputs = GetPtrVector(FLOW, false);
		for (SimB* v : outputs) vall.push_back(v);

		outputs = GetPtrVector(AUX, false);
		for (SimB* v : outputs) vall.push_back(v);

		outputs = GetPtrVector(DRIVE, false);
		for (SimB* v : outputs) vall.push_back(v);

		vall = ReturnIfSelectionMade(vall);

		for (SimB* pSimB : vall) {
			OutVector* pP = static_cast<OutVector*>(pSimB);
			vectoroutput_set ss;
			ss.key_simulation = pkey;
			ss.id_vectoroutput = p_ModelInfo->GetVectorOutputId(pP->GetName());
			for (size_t i = 0; i < pP->GetNumVector(); i++) ss.storeflags.push_back(pP->GetStoreFlag(i));
			vector<size_t> selected_index = pP->GetAllSelectedLocalIndex();
			for (size_t i : selected_index) ss.selectedindex.push_back(int(i));
			for (size_t i : selected_index) ss.initials.push_back(pP->GetInitial(i - 1));
			for (size_t i : selected_index) ss.finals.push_back(pP->GetFinal(i - 1));
			for (size_t i : selected_index) ss.mins.push_back(pP->GetMin(i - 1));
			for (size_t i : selected_index) ss.maxs.push_back(pP->GetMax(i - 1));
			for (size_t i : selected_index) ss.means.push_back(pP->GetMean(i - 1));
			for (size_t i : selected_index) ss.cumulateds.push_back(pP->GetAccumulated(i - 1));
			for (size_t i : selected_index) ss.singlerun_out_index.push_back(pP->GetPgSingleFileIndex(i - 1));
			for (size_t i : selected_index) ss.multirun_out_index.push_back(pP->GetPgMultiFileIndex(i - 1));
			s.push_back(ss);
		}
		if(s.size()>0) transfer_Modified_VectorOutput(s);
	}
	// Single outputs
	{
		vector<singleoutput_set> s;
		vector<SimB*> outputs;

		vector<SimB*> vall;
		vall = GetPtrVector(STATE_SINGLE, false);

		outputs = GetPtrVector(FLOW_SINGLE, false);
		for (SimB* v : outputs) vall.push_back(v);

		outputs = GetPtrVector(AUX_SINGLE, false);
		for (SimB* v : outputs) vall.push_back(v);

		outputs = GetPtrVector(DRIVE_SINGLE, false);
		for (SimB* v : outputs) vall.push_back(v);

		vall = ReturnIfSelectionMade(vall);


		for (SimB* pSimB : vall) {
			OutSingle* pP = static_cast<OutSingle*>(pSimB);
			singleoutput_set ss;
			ss.key_simulation = pkey;
			ss.id_singleoutput = p_ModelInfo->GetSingleOutputId(pP->GetName());
			if (ss.id_singleoutput == 495) {
				int i = 1;

			}
			ss.storeflagvalue = pP->GetStoreFlag();
			ss.initial = pP->GetInitial();
			ss.final = pP->GetFinal();
			ss.min = pP->GetMin();
			ss.max = pP->GetMax();
			ss.mean = pP->GetMean();
			ss.cumulated = pP->GetAccumulated();
			ss.singlerun_out_index = pP->GetPgSingleFileIndex();
			ss.multirun_out_index = pP->GetPgMultiFileIndex();
			s.push_back(ss);
		}
		if(s.size()>0) transfer_Modified_SingleOutput(s);
	}
	// Model Files
	tuple<int, int, vector<float>> record;
	vector< tuple<int, int, vector<float>>> vrecs;
	string outputfiledir, outputfilename, infiledir;
	auto pos = m_DocFileName.rfind('\\');
	infiledir = m_DocFileName.substr(0, pos + 1);

		{
		vector<SimB*> vall;
		vall = GetPtrVector(PGFILE, false);
		outputfiledir.clear();
		outputfilename.clear();
		for (SimB* pSimB : vall) {
			F* pF = static_cast<F*>(pSimB);

			string name = pF->GetName();
			string OutputValFile, PGFileName;
			bool store_outputfile = false;
			CPG* pPG = pF->GetPointer();
			PGFileName = pPG->GetFileName();
			
			if (name.find("ValidationFile") != string::npos) {
				string strnum = name.substr(15);
				int ival = FUtil::AtoInt(strnum);
				OutputValFile = outputfiledir + "V" + FUtil::STD_ItoAscii(ival) + "_";
				OutputValFile += outputfilename;			
				if(!DB_Source)
					if (!FUtil::IsFileExisting(OutputValFile)) OutputValFile.clear();
			}
			else if (name.find("ValidationOutputFile") != string::npos) {
				if (DB_Source) {

				}

			}

			if (name == "Output File") {
				if (DB_Source) {
					pF->SetPointer(&m_PG_OutPutFile);
					pPG = pF->GetPointer();
					PGFileName = pPG->GetFileName();
				}
				auto pos = PGFileName.rfind("COUP_");
				outputfilename = PGFileName.substr(pos);
				outputfiledir = PGFileName.substr(0, pos);
			}



			auto set_info_file = [&vrecs, &record,&DB_Source](int key, string name, string filename, string sitename, CPG* pPG) {
				timeserie_set ss;
				bool file_exist = true;
				ss.key_simulation = key;
				ss.id_timeserie = p_ModelInfo->GetTimeSeriesId(name);
				ss.filename = filename;

				if (!DB_Source) {
					std::string f = ss.filename;
					pPG->SetFileName(f);
					auto koll = pPG->Open(f);
					if (!koll) {
						string ans;
						string shortname;
						auto pos2 = f.rfind('\\');
						shortname = f.substr(pos2 + 1);
						f = FUtil::GetDocumentPath() + shortname;
						auto koll = pPG->Open(f);
						cout << "file not open:" << f << endl;
						file_exist=false;
					}
				}
				if (file_exist) {
					ss.NumVar = pPG->GetNumVariables();
					ss.NumRec = pPG->GetNumRecords();
					ss.NumRepetions = pPG->GetNumRepititions();
					ss.NormalTimeStep = pPG->GetNormalTimeInterval();
					for (size_t i = 0; i < ss.NumVar; ++i) {
						string str = pPG->GetVarName(i + 1); FUtil::trim(str);
						FUtil::trim_xmlstring(str);
						ss.Name.push_back(str);
						ss.Units.push_back(pPG->GetVarUnit(i + 1));
						ss.I_Units.push_back(int(pPG->GetVarUnitType(i + 1)));
						ss.Id.push_back(pPG->GetVarId(i + 1));
						ss.Pos.push_back(pPG->GetVarPos(i + 1));
						ss.Min.push_back(pPG->GetVarMin(i + 1));
						ss.Max.push_back(pPG->GetVarMax(i + 1));
						ss.Country.push_back(pPG->GetVarCountry(i + 1));
						ss.Station.push_back(pPG->GetVarStation(i + 1));
						ss.Latitude.push_back(pPG->GetVarLat(i + 1));
						ss.Longitude.push_back(pPG->GetVarLong(i + 1));
						ss.Altitude.push_back(pPG->GetVarAlt(i + 1));
					}
					vrecs.clear();
					for (size_t i = 0; i < ss.NumRec; i++) {
						vector<float> var; var.resize(ss.NumVar);
						for (size_t j = 0; j < ss.NumVar; j++)
							var[j] = pPG->GetVarValue(j + 1, i + 1);

						record = make_tuple(0, pPG->GetLongTime(i + 1), var);
						vrecs.push_back(record);

					}
					if (ss.filename != FUtil::FileNameOnly(ss.filename, true)) {
						if(ss.filename.find(sitename)==string::npos)
							ss.filename = sitename+"_" + FUtil::FileNameOnly(ss.filename, true);
						else
							ss.filename = FUtil::FileNameOnly(ss.filename, true);
					}
					transfer_Modified_TimeSeries(ss, vrecs);
					cout << ss.filename << endl;
				}
				else
					cout << "File Does not exist : " << ss.filename << endl;
			};
			set_info_file(pkey, name, PGFileName,m_SiteName, pPG );
		}
	}
	// Validation
	{
		vector< vector <string>> v;
		vector<string> v_inner; v_inner.resize(23);
		int count = 0;
		for (VALv vst : m_Val_Array) {
			count++;
			v_inner[0] = to_string(vst.ValidationFileIndex); v_inner[1] = to_string(vst.OutputType);
			v_inner[2] = "'" + vst.Group + "'"; v_inner[3] = +"'" + vst.Name + "'";
			v_inner[4] = to_string(int(vst.LocalIndex)); v_inner[5] = to_string(vst.ValidationFileNumber); v_inner[6] = to_string(count);
			v_inner[7] = to_string(int(vst.Monitoring)); v_inner[8] = to_string(vst.P_Error); v_inner[9] = to_string(vst.A_Error);
			v_inner[10] = to_string(vst.AccTest); v_inner[11] = to_string(vst.LogTrans); v_inner[12] = to_string(vst.Duration);
			v_inner[13] = to_string(vst.nNumber);	v_inner[14] = to_string(vst.R2); v_inner[15] = to_string(vst.A0); v_inner[16] = to_string(vst.A1);
			v_inner[17] = to_string(vst.ME); v_inner[18] = to_string(vst.RMSE); v_inner[19] = to_string(vst.MeanSim); v_inner[20] = to_string(vst.MeanVal);
			v_inner[21] = to_string(vst.LogLi);
			v_inner[22] = to_string(vst.NSE);
			if (v_inner[22] == "-inf") v_inner[22] = "Null";
			v.push_back(v_inner);
		}
		transfer_Validation(pkey, v);



	}

	// Dynamic parameters
	{
		tuple<int, int, string, int, int, vector<int>, vector<float>, vector<string>> tp;
		vector<tuple<int, int, string, int, int, vector<int>, vector<float>, vector<string>> > v_tp;
		for (CHPARv r : m_P_CH_Array) {
			get<0>(tp) = r.Type;
			SimB* pSimB = r.pBase;
			get<1>(tp) = int(pSimB->GetGroupNo());

			get<2>(tp) = pSimB->GetName();
			get<3>(tp) = r.TabIndex;
			get<4>(tp) = r.NumDates;
			for (size_t i = 0; i < r.NumDates; ++i) {
				if (r.Type == 0) get<5>(tp).push_back(static_cast<Ps*>(pSimB)->GetChaDate(i));
				else if (r.Type == 1) get<5>(tp).push_back(static_cast<P*>(pSimB)->GetChaDate(r.TabIndex, i));
				else if (r.Type == 2) get<5>(tp).push_back(static_cast<CDB*>(pSimB)->GetChaDate(i));
				if (r.Type == 0) get<6>(tp).push_back(static_cast<Ps*>(pSimB)->GetChaParValue(i));
				else if (r.Type == 1) get<6>(tp).push_back(static_cast<P*>(pSimB)->GetChaParValue(r.TabIndex, i));
				else if (r.Type == 2) get<7>(tp).push_back(static_cast<CDB*>(pSimB)->GetChaParValue(i) + ":" +
					static_cast<CDB*>(pSimB)->GetChaParKey(i));
			}
			v_tp.push_back(tp);
		}
		transfer_dynamic_parameters(pkey, v_tp);
	}

	//MultiRuns
	{
		vector<tuple<int, int, string, int, int, int, float, float, float, string, int, vector<float>, vector<string>, vector<string>, int>> allmr;
		tuple<int, int, string, int, int, int, float, float, float, string, int, vector<float>, vector<string>, vector<string>, int> mr;
		for (size_t i = 0; i < m_MultiRun_Array.size(); i++) {
			MRv valMr = m_MultiRun_Array[i];
			size_t numpar = MR_Get_NumberOfParametersWithinDim(i + 1);
			size_t n = MR_GetNumberOfRepeationsForThisDimension(i);
			get<14>(mr) = n;
			for (size_t ii = 0; ii < numpar; ii++) {
				SimB* pBase = MR_Get_pPar(i + 1, ii + 1);
				size_t TabIndex = MR_GetTabIndex(i + 1, ii + 1);
				string group_psw = MR_GetParGroup(i + 1, ii + 1);
				string name = MR_GetParName(i + 1, ii + 1);
				get<0>(mr) = i + 1;
				get<1>(mr) = pBase->GetGroupNo();
				get<2>(mr) = pBase->GetName();
				get<3>(mr) = int(TabIndex);
				int method = int(MR_GetMethod(i + 1, ii + 1));
				get<4>(mr) = method;
				get<5>(mr) = int(MR_GetMonitoring(i + 1, ii + 1));
				get<6>(mr) = float(MR_GetMin(i + 1, ii + 1));
				get<7>(mr) = float(MR_GetMax(i + 1, ii + 1));
				get<8>(mr) = float(MR_GetStart(i + 1, ii + 1));
				SimB* pCP = MR_Get_pPar(i + 1, ii + 1);
				size_t indpar, ind_dep;
				SimB* pParDep;
				indpar = MR_GetTabIndex(i + 1, ii + 1);
				if (pBase->Is_P()) {
					pParDep = ((Ps*)pBase)->Get_MR_ParDependence();
					ind_dep = ((Ps*)pBase)->Get_MR_ParDependenceIndex();
				}
				else {
					pParDep = ((P*)pBase)->Get_MR_ParDependence(indpar);
					ind_dep = ((P*)pBase)->Get_MR_ParDependenceIndex(indpar);
				}
				if (pParDep != nullptr) {
					string group_psw = pParDep->GetGroup();
					string name_psw = pParDep->GetName();
					get<9>(mr) = pParDep->GetName();
					get<10>(mr) = ind_dep;

				}
				if (method == 4) {
					vector<float> fvalues;
					fvalues.resize(MR_GetNumberOfRepeationsForThisDimension(i));
					for (size_t iii = 0; iii < MR_GetNumberOfRepeationsForThisDimension(i); iii++) {
						if (MR_GetTabIndex(i + 1, ii + 1) == -1)
							fvalues[iii] = float(((Ps*)pCP)->MR_Get_TabValue(iii));
						else
							fvalues[iii] = float(((P*)pCP)->MR_Get_TabValue(MR_GetTabIndex(i + 1, ii + 1), iii));
					}
					get<11>(mr) = fvalues;
				}
				else if (method == 5) {
					vector<string> key, strvalues;
					CDB* pCDB = (CDB*)MR_Get_pPar(i + 1, ii + 1);

					key.resize(n); strvalues.resize(n);
					for (size_t iii = 0; iii < n; iii++) {
						strvalues[iii] = pCDB->MR_GetSelection(iii);
						key[iii] = pCDB->MR_GetKeySelection(iii);
					}
					get<12>(mr) = key;
					get<13>(mr) = strvalues;
				}
				allmr.push_back(mr);
			}		
			transfer_multirun_setting(pkey, allmr);
		}
	}

	transfer_initial_final(pkey, this);
	transfer_history(pkey, this);

	cout << "Document archived in database :" << m_DocFileName << endl;

	//Results from Completed MultiRuns
	{
		//if (MR_Storage_Open()) {
		if(m_DocFile.m_MultiRunning){
			transfer_MultiStorage(pkey, this);
			transfer_Residuals(pkey, this);
			transfer_ensemble_statistics(pkey, this);

		}

	}

	return true;
#endif

	return false;
}
bool NewMap::ReDefinePostgresDataBase()
{
	// Call to units which construct all necessary tables for Postgres
#ifdef COUP_POSTGRES
	auto init_tables = create_Init_Tables(m_pCommonModelInfo);
	auto main_tables = create_Main_Tables(m_pCommonModelInfo);
	auto add_tables = create_Additional_Tables(m_pCommonModelInfo, this);

	ID_MapsForPostgresReady = true;

#endif

	return true;
}

vector<pair<int, string>> NewMap::GetDataBaseSoilProfiles() {
	vector<pair<int, string>> out;
#ifdef COUP_POSTGRES

	pair<int, string> a;
	try {
		pqxx::connection c = initconnection("Select soil_profiles");
		pqxx::work txn{ c };
		{
			pqxx::result r{ txn.exec("SELECT id_profile, id_oldkey, name, numlayers, upper_clay, upper_sand, upper_org FROM soil_profiles") };
			for (auto row : r) {
				int ii = row[0].as<int>();
				string str = row[2].as<string>();
				a = pair<int, string>(ii, str);
				out.push_back(a);
			};
		}
	}
	catch (const std::exception& e) {
		cerr << e.what() << std::endl;
	}
	return out;
#endif
	return out;
}
string NewMap::SetDataFromSoilProfile(int key) {

#ifdef COUP_POSTGRES
	if (p_PFCurve == nullptr) {
		p_PFCurve = make_unique<PFCurve>();
	}


	PFCOEF pf;
	pair<int, string> a;
	try {

		pqxx::connection c = initconnection("Select soil_profiles");
		pqxx::work txn{ c };
		{
			NEWHEADER prof;
			pqxx::result r{ txn.exec("SELECT * FROM soil_profiles WHERE id_profile = "+to_string(key) )};
			for (auto row : r) {
				int ii = row[0].as<int>();
				prof.KeyProf = row[1].as<int>();
				prof.Name = row[2].as<string>();			
				prof.Country = row["country"].as<string>();
				prof.County = row["county"].as<string>();
				prof.Long= row["longitude"].as<double>();
				prof.Lat = row["latitude"].as<double>();
				prof.NumLayers = row["numlayers"].as<int>();
				prof.LowDepth = row["lowerdepth"].as<int>();
				prof.Upper_Clay = row["upper_clay"].as<double>();
				prof.Upper_Sand = row["upper_sand"].as<double>();
				prof.Upper_Org = row["upper_org"].as<double>();
				prof.Lower_Clay = row["lower_clay"].as<double>();
				prof.Lower_Sand = row["lower_sand"].as<double>();
				prof.Lower_Org = row["lower_org"].as<double>();
				prof.Year = row["year"].as<int>();
			};
			
			vector<PFCOEF> pf_vector;
			r=txn.exec("SELECT id_profile, id_layers, upperdepthfromprofile, lowerdepthfromprofile FROM soil_profile_layer_linking WHERE id_profile = " + to_string(key)) ;
			size_t count_row = 0;

			for (auto row : r) {
				int iprof = row[0].as<int>();
				int keylayer = row[1].as<int>();


				double upper_b = row[2].as<double>();
				double lower_b = row[3].as<double>();
				if (count_row == 0) {
					p_PFCurve->DefineWithPostGres();
					p_PFCurve->SetPostGresProfile(prof);
				}

				auto setv = [](array_parser pvec) {
					vector<float> vut;
					auto next = pvec.get_next();
					next = pvec.get_next();
					while (next.second.size() > 0) {
						float value;
						while (next.second.size() > 0) {
							value = stof(string(next.second));							
							vut.push_back(value);
							next = pvec.get_next();
						}
					}
					return vut;				
				};
				pqxx::result rr=txn.exec("SELECT * FROM soil_layers WHERE id_layers = " + to_string(keylayer));
				for (auto row : rr) {
					pf.UpperDepth = row["upperdepth"].as<double>();
					pf.LowerDepth = row["lowerdepth"].as<double>();
					pf.Saturation = row["saturation"].as<double>();
					pf.Wilting = row["wiltingpoint"].as<double>();
					pf.Residual = row["residual"].as<double>();
					pf.AirEntry = row["airentry"].as<double>();
					pf.Lambda = row["lambda"].as<double>();
					pf.UpperBoundary = row["upperboundaryhead"].as<double>();
					pf.LowerBoundary = row["macropore"].as<double>();
					pf.Gen_Alfa = row["genalpha"].as<double>();
					pf.Gen_M = row["genm"].as<double>();
					pf.Gen_N = row["genn"].as<double>();
					pf.TotConductivity = row["totconductivity"].as<double>();
					pf.MatConductivity = row["matricconductivity"].as<double>();
					pf.Mualem_n = row["mualem_n"].as<double>();
					pf.N_SrCoef = row["n_srcoef"].as<double>();
					pf.N_SECoef = row["n_secoef"].as<double>();
					p_PFCurve->SetPF_Coef(count_row, pf);

					auto pvec = row["atterbergfractions"].as_array();
					auto afrac = setv(pvec);

					p_PFCurve->SetPF_Texture(count_row, afrac);

					if (count_row == 0) {
						auto pvec1 = row["heads"].as_array();
						auto ahead = setv(pvec1);
						p_PFCurve->SetPF_Pressure(ahead);
					}

					auto pvec2=row["thetameasured"].as_array();
					auto atheta = setv(pvec2);
					p_PFCurve->SetPF_Theta(count_row, atheta);
					count_row++;
				}

			};
			if (count_row > 0) {
				return GetProfileFromPlotPF(false, false);

			}		
			
		}
	}
	catch (const std::exception& e) {
		cerr << e.what() << std::endl;
	}
	
#endif




	return "";
}




vector<pair<int, string>> NewMap::GetDataBaseSimulations() {
	vector<pair<int, string>> out;
#ifdef COUP_POSTGRES
	
	pair<int, string> a;
	try {
		pqxx::connection c = initconnection("Select from postgres");
		pqxx::work txn{ c };
		{
			bool keyfind = false;
			pqxx::result r{ txn.exec("SELECT id_simulations, name FROM simulations") };
			for (auto row : r) {
				int ii = row[0].as<int>();
				string str = row[1].as<string>();
				a = pair<int, string>(ii,str );
				out.push_back(a);
			};
		}

	}
	catch (const std::exception& e) {
		cerr << e.what() << std::endl;
	}
	return out;
#endif
	return out;


}


bool NewMap::Info_Header(bool reading)
{
	size_t iv;
	ifstream::pos_type size;
	string str;
	char cbuf[80];
//  Get date of exe-file
	long long pos;


	Sw* pSw;

	m_OptionsForReading.clear();
	if(reading) {
		pSw=nullptr;
		m_DocFileIn.seekg (0, ios::beg);
		rSize_t(&m_FileVersionNumberRead);
		if(m_FileVersionNumberRead>25)
			rString(&m_DocFile2.m_OriginFileName);
		rString(&m_DocFile2.m_ExeFileDate);
		size_t ipos=m_DocFile2.m_ExeFileDate.find("1970");
		if(ipos==25)
			m_DocFile2.m_ExeFileDate=m_DocFile2.m_ExeFileDate.substr(0, 11);
		if(m_FileVersionNumberRead>11)
			rString(&m_DocFile2.m_DateRun);
		if(m_FileVersionNumberRead>35) {
			if(m_FileVersionNumberRead<50) {
				rBOOL(&m_DocFile.m_MultiRunning);
				rBOOL(&m_DocFile.m_FinishedSimulation);

			}
			else {
				rbool(&m_DocFile.m_MultiRunning);
				rbool(&m_DocFile.m_FinishedSimulation);
			}
		
			rString(&m_DocFile2.m_Multi_MBin_File);
		}
			rString(&str);
			if(str=="Run Options") {
				rInt(&RunOpt.oiminutes);
				rInt(&RunOpt.oidays);
				rUint(&RunOpt.longMinStart);
				rUint(&RunOpt.longMinEnd);
				rInt(&RunOpt.noofiter);
				rSize_t(&m_DocFile.m_SimulationRunNo);
				if(m_FileVersionNumberRead>17)
					rSize_t(&m_DocFile.m_TimePeriodScaleFactor);
				rString(&m_DocFile2.m_Comments);
				rString(&m_DocFile2.m_RunId);
				if(m_FileVersionNumberRead>=7) {
					rSize_t(&iv);
					pSw=dynamic_cast<Sw*>(GetPtr(SWITCH,"TimeResolution"));
					if(pSw!=nullptr) {
						if(iv<0||iv>3) iv=0;
						pSw->SetIntValue(iv);
					}
				}
				if(m_FileVersionNumberRead>=29) {
					rSize_t(&m_DocFile.ipredays);
					RunOpt.longMinPreStart=RunOpt.longMinStart-m_DocFile.ipredays*1440;
	//				PARAMETERS_mp_PRESIMULATIONDAYS=m_DocFile.ipredays;
					if (m_FileVersionNumberRead >= 68) 	rSize_t(&m_DocFile.ipostdays);
					if (m_DocFile.ipostdays > 1000)  m_DocFile.ipostdays = 0;
					RunOpt.longMinPostEnd = RunOpt.longMinEnd + m_DocFile.ipostdays * 1440;
				}

				pos=m_DocFileIn.tellg();
				m_DocFileIn.read((char*)cbuf,40);
				m_DocFileIn.seekg(pos,ios::beg);

				if(m_FileVersionNumberRead>36) {
					rTime(&m_DocFile.m_TimeCreated);
					rTime(&m_DocFile.m_TimeModified);
					
				}
				size_t apos=0;
				while(!(cbuf[apos]=='S'&&cbuf[apos+1]=='w')&&apos<38) 
					apos++;		
				if(apos==33) m_ShortTimeRead=false;
				if(m_FileVersionNumberRead<36) {
					apos=0;
				}
				else if(m_FileVersionNumberRead==46||m_FileVersionNumberRead<=40||m_FileVersionNumberRead==45)
					apos-=5;
				else if(m_FileVersionNumberRead<50)
					apos-=9;
				else
					apos-=6;
				pos+=apos;
				m_DocFileIn.seekg(pos,ios::beg);
				if(m_FileVersionNumberRead>46) {
					if(m_FileVersionNumberRead<50)
						rBOOL(&m_DocFile.m_MultiStoragelocked);
					else
						rbool(&m_DocFile.m_MultiStoragelocked);
					if(!m_DocFile.m_MultiStoragelocked)
						UnLockMultiStorageFile();
				}
			}
			SetNextChaParDate(RunOpt.longMinStart);
	}
	else {
		wSize_t(&m_DocFile.m_FileVersionNumber);

		wString(&m_DocFile2.m_OriginFileName);
		if(m_Protect) {
			wString(&m_DocFile2.m_ExeFileDate);
		}
		else { //kk
			string ExeDate; 
			ExeDate="Version 6.2.2, ";
#ifndef COUPSTD
			CWinApp *pApp;
			pApp=AfxGetApp();
			CFileStatus status;
			string str=CT2A(pApp->m_pszHelpFilePath).m_psz;
			size_t ipos= str.find(".CHM");
			if(ipos<0) ipos=str.find(".chm");
			if(ipos>0) {
				str=str.substr(0,ipos);
				str+=".exe";
				CFile::GetStatus(CString(str.c_str()), status);
				CTime kk=status.m_mtime;
				ExeDate+=CT2A(kk.Format(_T("%d %B, %Y"))).m_psz;
			}
#endif
            string exdate = string(ExeDate);
            
			wString(&exdate );
		}
		wString(&m_DocFile2.m_DateRun);
		wbool(&m_DocFile.m_MultiRunning);
		wbool(&m_DocFile.m_FinishedSimulation);
		wString(&m_DocFile2.m_Multi_MBin_File);
		string str="Run Options";
		wString(&str);
		wInt(&RunOpt.oiminutes);
		wInt(&RunOpt.oidays);
		wUint(&RunOpt.longMinStart);
		wUint(&RunOpt.longMinEnd);
		wInt(&RunOpt.noofiter);
		wSize_t(&m_DocFile.m_SimulationRunNo);
		wSize_t(&m_DocFile.m_TimePeriodScaleFactor);
		wString(&m_DocFile2.m_Comments);	
		wString(&m_DocFile2.m_RunId);
		pSw=dynamic_cast<Sw*>(GetPtr(SWITCH,"TimeResolution"));
		iv=pSw->GetIntValue();
		wSize_t(&iv);
		wSize_t(&m_DocFile.ipredays);	 
		if (RunOpt.longMinPostEnd > RunOpt.longMinEnd)
			m_DocFile.ipostdays = (RunOpt.longMinPostEnd - RunOpt.longMinEnd) / 1440;
		else
			m_DocFile.ipostdays = 0;
		wSize_t(&m_DocFile.ipostdays);
		wTime(&m_DocFile.m_TimeCreated);
		wTime(&m_DocFile.m_TimeModified);
		wbool(&m_DocFile.m_MultiStoragelocked);


	} 
return true;
}
bool NewMap::Info_Switches(bool reading)
{ 
	Sw *pSw=nullptr;
	string type,group,name;
	size_t  value;
	size_t iv = 0;

	if(reading) {
		ResidualFile_Reset();
		m_Sw_Array.clear();
		rSize_t(&iv);
		rString(&type);
		if(type!="Switches")
			return false;
		else {
			for(size_t i=0; i<iv;i++) {
				rString(&group);
				if(group=="Driving variables") group="Meteorological Data";
				if(group=="Soil mineral nitrogen processes") group="Soil mineral N processes";
				rString(&name);
				rSize_t(&value);
				if(group=="Model Structure"&&name=="TimeResolution") group="Hidden";
 				if(group=="Technical"&&name=="TimeResolution") group="Hidden";
 				if(group=="Irrigation"&&name=="IndexForValidation") group="Technical";
				if(m_FileVersionNumberRead<=24&&group=="Potential Transpiration"&&name=="Aerodyn. Resistance") name="Aerodynamic Resistance";
				pSw=dynamic_cast<Sw*>(GetPtr(SWITCH,name));
				if(pSw!=nullptr) {
					size_t recalc=pSw->SetIntValue(value);
					if(recalc>0) LinkedChangeToSwitches(pSw, recalc);
					pSw->SetNotOldValue();
					//Sw->Apply();
					m_Sw_Array.push_back(pSw);
				} 
				else {
					string koll=name;


				}



			}
		}

		m_pValidIndexSet=dynamic_cast<Sw*>(GetPtr(SWITCH, "IndexForValidation"));
		if (m_pValidIndexSet != nullptr) {
			auto iv = m_pValidIndexSet->GetIntValue();
			m_pValidIndex = dynamic_cast<Ps*>(GetPtr(PAR_SINGLE, "Index in val file"));
			m_pValidIndexRep = dynamic_cast<Ps*>(GetPtr(PAR_SINGLE, "No of internal index loop"));
			m_pValidIndexLoopSize = dynamic_cast<Ps*>(GetPtr(PAR_SINGLE, "Size of internal loop"));

			pSw = dynamic_cast<Sw*>(GetPtr(SWITCH, "ValidationMode"));
			m_NumValidFiles = pSw->GetIntValue();
			ResidualFile_AddFile(m_NumValidFiles);
		}

	}
	else {
		RemoveOriginalValues("Switches","ALL",false);
		iv=m_Sw_Array.size();
		wSize_t(&iv);
		type="Switches";
		wString(&type);
        string group_psw;
        string name_psw;
        
		for(size_t i=0;i<iv;i++) {
			pSw=m_Sw_Array[i];
            group_psw = pSw->GetGroup();
            name_psw = pSw->GetName();
			wString(&group_psw);
			wString(&name_psw);
            
			auto ivalue=pSw->GetIntValue();
			wSize_t(&ivalue);
		}
	}

	

return true;
}
bool NewMap::Info_Parameters(bool reading)
{	
	Ps *pPs=nullptr;
	string type,group,name;
	size_t iv;
	float fvalue;
	if(reading) {
		m_P_Array.clear();
		rSize_t(&iv);
		rString(&type);
//        cout << "Parameteres\n***********\n";
		if(type=="Parameters") {
			for(size_t i=0; i<iv;i++) {
				rString(&group);
				if(group=="Driving variables") group="Meteorological Data";
				if(group=="Soil mineral nitrogen processes") group="Soil mineral N processes";
		
				rString(&name);
				rFloat(&fvalue);
				if(name=="RateCoefHumus"&&m_FileVersionNumberRead<=21) {
					fvalue=fvalue*2;
				}
				pPs=dynamic_cast<Ps*>(GetPtr(PAR_SINGLE,name));
				if(pPs==nullptr) {
						if(name=="Intial Soil Storage") name="Initial Soil Storage";
						if(name=="Intial Peak Storage") name="Initial Peak Storage";
						if(name=="Intial Base Storage") name="Initial Base Storage";
						pPs=dynamic_cast<Ps*>(GetPtr(PAR_SINGLE,name));
				}
				if(pPs!=nullptr) {
					AssignNewValue_toPs(pPs, fvalue);
				}
			}
		}
	}
	else {
		RemoveOriginalValues("Parameters","ALL",false);
		iv=m_P_Array.size();
		wSize_t(&iv);
		type="Parameters";
		wString(&type);
        string group_psw;
        string name_psw;
		for(size_t i=0;i<iv;i++) {
			pPs=m_P_Array[i];
            group_psw = pPs->GetGroup();
            name_psw = pPs->GetName();
			wString(&group_psw);
			wString(&name_psw);
			fvalue=float(pPs->GetValue());
			wFloat(&fvalue);
		}
	}

return true;
}
bool NewMap::Info_ParTables(bool reading)
{

	P *pPt=nullptr;
	Tab* pTab=nullptr;
	m_continueRead=false;
	string type,group,name;
	size_t iv, nColSize, aNE;
	float fvalue;
	string str, comp_str;
	if(reading) {
		m_Tab_Array.clear();
		rSize_t(&iv);
		rString(&type);
  //      cout << "Table Parameteres\n***********\n";
		if(type=="Tables") {
			for(size_t i=0; i<iv;i++) {
				rString(&group);
				if(group=="Driving variables") group="Meteorological Data";
				if(group=="Soil mineral nitrogen processes") group="Soil mineral N processes";	
				rString(&name);
				pTab=dynamic_cast<Tab*>(GetPtr(TABLE,name));
				rSize_t(&nColSize);
                
     //           cout << name << "\n";
				vector <string> TableVarNames;
				for(size_t nCol=0;nCol<nColSize;nCol++) {

					rString(&str);
					TableVarNames.push_back(str);
					if(pTab!=nullptr) {
						comp_str=string(pTab->GetTitle(nCol));
						if(str!=comp_str&&name!="Plant diameter - multiple canopies"&&name!="Snow Albedo Coefficients") {
								string mes="Mismatching variable in table:";
								mes+=name;
								mes+=" - ";
								mes+=str;
								mes+=" not equal";
								mes+=comp_str;
	#ifndef COUPSTD
							//MFC_Util::MessageBox(mes,"Input Error",	MB_OK|MB_ICONEXCLAMATION);
	#endif
						}
					}

				}
				rSize_t(&aNE);
				if(pTab!=nullptr) {
					size_t oldNE=pTab->GetNumOfRow();
					bool validnewinfo=false;
					if(aNE!=oldNE&&oldNE>0) {
						pTab->SetNumOfRow(aNE);
					}
					P* pP;
					auto NewTest = [&](string name) {
						return GetPPointer(name);

					};

					vector<size_t> ReCalc; ReCalc.assign(nColSize,0);
					size_t ReCalcFromCol;
					for(size_t nRow=0; nRow<aNE ; nRow++){
	  		  				for (size_t nCol=0; nCol<nColSize; nCol++){
								ReCalcFromCol = ReCalc[nCol];
								pP = pTab->GetP_Pointer(nCol);
								string testparname = TableVarNames[nCol];
								auto pVar=GetPPointer(testparname);
								if (pVar == nullptr) {
									if (testparname.find("I C OldRoots") != string::npos) pVar = NewTest("I C OldRoot");
									if (testparname.find("I C OldCRoots") != string::npos) pVar = NewTest("I C OldCoarseRoot");
									if (testparname.find("I C Roots") != string::npos) pVar = NewTest("I C Root");
									if (testparname.find("I C CRoots") != string::npos) pVar = NewTest("I C CoarseRoot");
									if (testparname.find("I N OldRoots") != string::npos) pVar = NewTest("I N OldRoot");
									if (testparname.find("I N OldCRoots") != string::npos) pVar = NewTest("I N OldCoarseRoot");
									if (testparname.find("I N Roots") != string::npos) pVar = NewTest("I N Root");
									if (testparname.find("I N CRoots") != string::npos) pVar = NewTest("I N CoarseRoot");
								}

								if (pVar != pP&&pVar != nullptr) pP = pVar;

								rFloat(&fvalue);
								pP->SetNotOriginalValue();

								if(pP!=nullptr) {
									bool LocalValidnewinfo=AssignNewValue_toP(pP, nRow, fvalue, ReCalcFromCol);
									if (LocalValidnewinfo) validnewinfo = true;
									ReCalc[nCol] = ReCalcFromCol;
								}
	  		  				}
					}
					for (size_t nCol=0; nCol<nColSize; nCol++){
							pP=pTab->GetP_Pointer(nCol);
							auto pVar = GetPPointer(TableVarNames[nCol]);
							if (pVar != pP&&pVar != nullptr) pP = pVar;


							if(pP!=nullptr) {
								string str=pP->GetName();
								if(ReCalc[nCol]>0) LinkedChangeToParameters(pP, ReCalc[nCol]);
							}
					}
					if(validnewinfo)
						m_Tab_Array.push_back(pTab);
					//pTab->SetNotOriginalValue();
				}
				else {
				  for(size_t nRow=0;nRow<aNE;nRow++) {
					   for (size_t nCol=0; nCol<nColSize;nCol++)
						   rFloat(&fvalue);
				  }

				 // if(name!="Plant diameter - multiple canopies");
				  if(name!="Snow Albedo Coefficients"&&name!="Brunts Net long wave Coefficients"&&name!="Brunts incoming long wave Coefficients"
					  &&name!="Satterlunds incoming long wave Coefficients"&&name!="Konzelmann incoming long wave Coefficients") {
							  string mes="Table name not Identified and input file not fully read";
							  m_continueRead=false;

							  mes+=name;
							 #ifndef COUPSTD
							  MFC_Util::MessageBox(mes,"Fatal Input Error",	MB_OK|MB_ICONEXCLAMATION);
							#endif
				   }
				  //return false;

				}

		}
		m_HideAllTableContents=false;
		if(m_continueRead) return false;
	}

	RemoveOriginalValues("Tables","ALL",true);

}
else {
		RemoveOriginalValues("Tables","ALL",false);
		iv=m_Tab_Array.size();
		wSize_t(&iv);
		type="Tables";
		wString(&type);
    string group_psw;
    string name_psw;
	for(size_t i=0;i<iv;i++) {
			pTab=(Tab*)m_Tab_Array[i];
            group_psw = pTab->GetGroup();
            name_psw = pTab->GetName();
			wString(&group_psw);
			wString(&name_psw);
			auto v=pTab->GetNumOfCol();
			wSize_t(&v);
			for(size_t j=0; j<v;j++) {
				str=pTab->GetTitle(j);
				wString(&str);
			}
			v =pTab->GetNumOfRow();
			wSize_t(&v);
			P *pP;
			for(size_t nRow=0; nRow<pTab->GetNumOfRow(); nRow++) {
				for(size_t nCol=0; nCol<pTab->GetNumOfCol(); nCol++) {
						pP=pTab->GetP_Pointer(nCol);
						fvalue=float(pP->GetValue(nRow));
						wFloat(&fvalue);
				}
			}
		}

}  
return true;
}
bool NewMap::Info_Files(bool reading)
{ 
	F *pF=nullptr;
	string type,group,name, FileName, IdVar;
	size_t iv;
	size_t multilistnum;


	if (reading) {
		m_F_Array.clear();
		rSize_t(&iv);
		rString(&type);
		if (type == "Files") {
			if (iv == 0) UpdateArchiveLists(7);
			for (size_t i = 0; i < iv; i++) {
				rString(&group);
				if (group == "Driving variables") group = "Meteorological Data";
				if (group == "Soil mineral nitrogen processes") group = "Soil mineral N processes";

				rString(&name);

				if (name == "Validation File") {
					name = "Validation File1";
					Sw *pSw;
					pSw = dynamic_cast<Sw*>(GetPtr(SWITCH, "ValidationMode"));
					pSw->SetIntValue(1);
					//pSw->Apply();			
				}
				rString(&FileName);
				FileName = GetExistingDocFileName(FileName);
				pF = dynamic_cast<F*>(GetPtr(PGFILE, name));
				if (pF != nullptr&&group != "Hidden") {
					pF->SetValue(FileName);
					pF->CheckFileNameAndAssignNameToPGClass();
					m_F_Array.push_back(pF);

				}

			}
			if (m_FileVersionNumberRead > 61) {
				m_ValsumFlexFiles.resize(0);

				VALSUMLINK sumlink;
				rSize_t(&iv);
				for (size_t i = 0; i < iv; i++) {
					rString(&FileName);
					rString(&sumlink.IDVarKey);
					rSize_t(&sumlink.MultiSimNumKey);
					FileName = GetExistingDocFileName(FileName);

					if (FUtil::IsFileExisting(FileName)) {
						sumlink.FileName = FileName;
						m_ValsumFlexFiles.push_back(sumlink);
						CreateMapSvFlexFile(i);
					}
					else {





					}
				}
			}
			if (m_FileVersionNumberRead > 66) {
				UpdateArchiveLists(8);
				string fileid;

				string filename;
				rSize_t(&iv);
				for (size_t i = 0; i < iv; i++) {
					rString(&fileid);
					rString(&filename);
					filename = GetExistingDocFileName(filename);
					FCSV *pCSV = dynamic_cast<FCSV*>(GetPtr(CSVFILE, fileid));
					if (pCSV != nullptr) {
						pCSV->SetValue(filename);
					if (pCSV->CheckFileName(this) && fileid == "MASCUR File") {


							if(!pGetSoil->IsOpen())
								pGetSoil->Init(filename);
						}
						else {
							string filename;
							if (fileid == "MASCUR File") {
								
#ifndef COUPSTD
								MFC_Util::MessageBox("Specify File Name for MASCUR soil File");
								FILEDLG FileDlg = MFC_Util::GetFileDialog("*.csv", "", "Comma separated file (*.csv)|*.csv|");
								if (FileDlg.opengood != IDOK) {
									return false;
								}
								filename = FileDlg.filename;
#else
								cout << "Specify file Name Mascur file";
								cin >> filename;

#endif
								pCSV->SetValue(filename);
								if (pCSV->CheckFileName(this)) {
										if (!pGetSoil->IsOpen()) {
										pGetSoil->Init(filename);
									}
								}

							}
							else if (!pCSV->CheckFileName(this)&& fileid == "FertTable File") {
#ifndef COUPSTD
								MFC_Util::MessageBox("Specify File Name for Fert Table File");
								FILEDLG FileDlg = MFC_Util::GetFileDialog("*.csv", "", "Comma separated file (*.csv)|*.csv|");
								if (FileDlg.opengood != IDOK) {
									return false;
								}
								string filename = FileDlg.filename;
#else
								cout << "Specify file Name Fert Table file";
								cin >> filename;

#endif
								pCSV->SetValue(filename);
								pCSV->CheckFileName(this);
							}
							else if (pCSV->CheckFileName(this) && fileid == "FertTable File") {

								string filename = pCSV->GetStrValue();
							}
						}
					}
				}
			}
			//m_DocFileIn.read( &iv,4);

		}
		SetSimPeriodFromClimateFile();
	}
	else {
		iv=m_F_Array.size();
		type="Files";
		wSize_t(&iv);
		wString(&type);
        string group_psw;
        string name_psw;
        string strval_psw;
        
		for(size_t i=0; i<iv;i++) {
			pF=m_F_Array[i];
            group_psw = pF->GetGroup();
            name_psw = pF->GetName();
            strval_psw = pF->GetStrValue();
			wString(&group_psw);
			wString(&name_psw);
			wString(&strval_psw);
		}
// New Flex Validation file - FileVersion 62

		iv = m_ValsumFlexFiles.size();
		wSize_t(&iv);
		for (size_t i = 0; i < iv; i++) {
			wString(&m_ValsumFlexFiles[i].FileName);
			wString(&m_ValsumFlexFiles[i].IDVarKey);
			wSize_t(&m_ValsumFlexFiles[i].MultiSimNumKey);
		}
	// New Multi Parfiles - FileVersion 67
		UpdateArchiveLists(8);
		iv = m_FCSV_Array.size();
		wSize_t(&iv);
        
		for (size_t i = 0; i < iv; i++) {
            name_psw = m_FCSV_Array[i]->GetName();
            strval_psw = m_FCSV_Array[i]->GetStrValue();
			wString(&name_psw);
			wString(&strval_psw);
		}


	}

return true;
}
bool NewMap::Info_Outputs(bool reading)
{
	//X *pXT=nullptr;
	//G *pGD=nullptr;
	OutSingle *pSingle;
	OutVector *pVector;
	string type,group,name;
	string FlagString, FlagString2;
	size_t nOutputs;
	size_t iv;
	size_t pgindex=0, pgmulindex=0;
	size_t iType;
	enum simtype typ { PAR };
	float initial,final,min,max,mean,accumulated;
	size_t localindex{ 0 }, validFlagIndex;
	size_t ivalue;
	size_t ivalshift=45;
	Clear_XTGD();
	//m_X_Array.clear();	m_T_Array.clear();	m_G_Array.clear();	m_D_Array.clear();

	if(reading) {
       // cout << "\nOutput Table Parameteres\n***********\n";
		pgindex=pgmulindex=0;
		rSize_t(&iv);
		rString(&type);
		//if(IsFinishedSimulation())
			m_NumOutPuts=0;
		if(type=="Outputs") {
			for(size_t i=0; i<iv;i++) {
				rSize_t(&iType);
				rString(&group);
				if(group=="Driving variables") group="Meteorological Data";
				if(group=="Soil mineral nitrogen processes") group="Soil mineral N processes";
		
				rString(&name);
  //              cout << name << "\n";
				if(m_FileVersionNumberRead<60) {
					rString(&FlagString);
					if(m_FileVersionNumberRead>ivalshift)
						rString(&FlagString2);
				}

				if(m_FileVersionNumberRead>13&&m_FileVersionNumberRead<61) rSize_t(&nOutputs);
				else
					nOutputs=1;
				m_NumOutPuts+=nOutputs;
				switch (iType) {
				case 1:
				case 8:
					typ=STATE;
					break;
				case 2:
				case 9:
					typ=FLOW;
					break;
				case 3:
				case 10:
					typ=AUX;
					break;
				case 4:
				case 11:
					typ=DRIVE;
					break;
				}
					OutVector *ptest;
					pVector=dynamic_cast<OutVector*>(GetPtr(typ,name));
					ptest=dynamic_cast<OutVector*>(GetPtr(AUX,name));
					if(ptest!=nullptr&&pVector==nullptr) {
						pVector=ptest;

					}
					if(ptest==nullptr&&pVector==nullptr) {
						name = FUtil::NameRevision2018(name);
						pVector = dynamic_cast<OutVector*>(GetPtr(typ, name));
						ptest = dynamic_cast<OutVector*>(GetPtr(AUX, name));


					}

					SIMB out;
					size_t storeflag{ string::npos };
					if(pVector!=nullptr) {
						if(m_FileVersionNumberRead<60) {
							pVector->SetFlags(FlagString.c_str(),1);
							if(m_FileVersionNumberRead>ivalshift)
								pVector->SetFlags(FlagString2.c_str(),2);
						}
						else 
							nOutputs=1;
						for(size_t ii=0;ii<nOutputs;ii++){
							rSize_t(&localindex);rFloat(&initial);rFloat(&final);rFloat(&min);
							rFloat(&max);rFloat(&mean);rFloat(&accumulated);rSize_t(&validFlagIndex);
							if(m_FileVersionNumberRead>35&&m_FileVersionNumberRead<61)
								rSize_t(&pgindex);
							if(m_FileVersionNumberRead>47&&m_FileVersionNumberRead<61)
								rSize_t(&pgmulindex);
							if(m_FileVersionNumberRead>59) {
								rSize_t(&storeflag);
								if(m_FileVersionNumberRead>60) {
									if(storeflag>=1) pgindex++;
									if(storeflag==2) pgmulindex++;
								}
							}
							if(ReadResultsFromFile){
								if(m_FileVersionNumberRead>59)
									pVector->AddSumVar(localindex,initial, final, min, max, mean,accumulated, size_t(pgindex), size_t(pgmulindex), storeflag);
								else {
									pVector->AddSumVar(localindex,initial, final, min, max, mean,accumulated, size_t(pgindex), size_t(pgmulindex));
									pVector->SetStoreFlag(localindex,1);
								}
								pVector->SetValidFlagIndex(localindex,size_t(validFlagIndex));
								if((pgmulindex>0&&pgmulindex<1000)&&m_FileVersionNumberRead<61) {
									pVector->SetMultiStoreFlag(localindex,true);
									pVector->SetStoreFlag(localindex,2);
								}
								else if(pgmulindex>0&&storeflag==2) {
									pVector->SetMultiPGFileIndex(localindex, pgmulindex);
									pVector->SetStoreFlag(localindex,2);
								}
									 
								

							}
						}
						if(nOutputs>0) {
							out.pBase=pVector;
							out.TabIndex=localindex;
						}
						else {
							out.pBase=nullptr;
							out.TabIndex=0;

						}
					}
					else {
						typ=simtype(size_t(typ)+7);
						pSingle=(OutSingle*)GetPtr(typ,name);
						if(pSingle==nullptr) {
							if(name=="Soil Resp no roots") name="Soil Resp (no roots)";
							pSingle=(OutSingle*)GetPtr(typ,name);
							if(typ==AUX_SINGLE&&pSingle==nullptr) {
								typ=FLOW_SINGLE;
								pSingle=(OutSingle*)GetPtr(typ,name);

							}



						}

						if(nOutputs>0) {
							rSize_t(&localindex);rFloat(&initial);rFloat(&final);rFloat(&min);
							rFloat(&max);rFloat(&mean);rFloat(&accumulated);rSize_t(&validFlagIndex);
							if(m_FileVersionNumberRead>35&&m_FileVersionNumberRead<61)
								rSize_t(&pgindex);
							if(m_FileVersionNumberRead>47&&m_FileVersionNumberRead<61)
								rSize_t(&pgmulindex);
							if(m_FileVersionNumberRead>59) {
								rSize_t(&storeflag);
								if(m_FileVersionNumberRead>60) {
									if(storeflag>=1) pgindex++;
									if(storeflag==2) pgmulindex++;
								}
							}
							
							if(ReadResultsFromFile&&pSingle!=nullptr){
								if(m_FileVersionNumberRead>59)
									pSingle->AddSumVar(initial, final, min, max, mean,accumulated, size_t(pgindex), size_t(pgmulindex), storeflag);

								else {
									pSingle->AddSumVar(initial, final, min, max, mean,accumulated, size_t(pgindex), size_t(pgmulindex));
									pSingle->SetStoreFlag(1);	
								}
								if((pgmulindex>0&&pgmulindex<1000)&&m_FileVersionNumberRead<61) {
									pSingle->SetMultiStoreFlag(true);
									pSingle->SetStoreFlag(2);
								}
								else if(pgmulindex>0&&storeflag==2) {
									pSingle->SetMultiPGFileIndex(pgmulindex);
									pSingle->SetStoreFlag(2);
								}
								pSingle->SetValidFlagIndex(size_t(validFlagIndex));
							}
						}
						out.pBase=pSingle;
						out.TabIndex=-1;
					}

					if(out.pBase!=nullptr&&nOutputs>0) {
						if(iType==1||iType==8) 
							m_X_Array.push_back(out);
						else if(iType==2||iType==9)
							m_T_Array.push_back(out);
						else if(iType==3||iType==10)
							m_G_Array.push_back(out);
						else if(iType==4||iType==11)
							m_D_Array.push_back(out);
					}
					else {
						auto koll = out;

					}
				}
			}
			m_X_Array=SortSimB_ByGroup(m_X_Array);
			m_T_Array=SortSimB_ByGroup(m_T_Array);
			m_G_Array=SortSimB_ByGroup(m_G_Array);
			m_D_Array=SortSimB_ByGroup(m_D_Array);
	}
	else { // Output of Outputs
		size_t lin;
		iv=GetSize_XTGD();
		type="Outputs";

		vector<SIMB> pvv;
		pvv=GetAllSelectedOutputs();
		size_t ivv=pvv.size();
		wSize_t(&ivv);
		wString(&type);

			for(size_t i=0; i<pvv.size();i++) {
				auto itt=pvv[i].pBase->GetType();
				wSize_t(&itt);
				if(itt==1||itt==8) 
						m_X_Array.push_back(pvv[i]);
				else if(itt==2||itt==9)
						m_T_Array.push_back(pvv[i]);
				else if(itt==3||itt==10)
						m_G_Array.push_back(pvv[i]);
				else if(itt==4||itt==11)
						m_D_Array.push_back(pvv[i]);

				pVector=dynamic_cast<OutVector*>(pvv[i].pBase);
				if(pVector!=nullptr){
					group=pVector->GetGroup();
					wString(&group);
					name=pVector->GetName();
					wString(&name);
				//	iv=pVector->GetNumberOfSelectedFlags();
				//	wInt(&iv);
					lin=pvv[i].TabIndex;
					wSize_t(&lin);
					float fvalue;
					fvalue=pVector->GetInitial(lin);wFloat(&fvalue);
					fvalue=pVector->GetFinal(lin);wFloat(&fvalue);
					fvalue=pVector->GetMin(lin);wFloat(&fvalue);
					fvalue=pVector->GetMax(lin);wFloat(&fvalue);
					fvalue=pVector->GetMean(lin);wFloat(&fvalue);
					fvalue=pVector->GetAccumulated(lin);wFloat(&fvalue);
					ivalue=pVector->GetValidFlagIndex(lin);wSize_t(&ivalue);
					//ivalue=pVector->GetPgSingleFileIndex(lin);wInt(&ivalue);
					//ivalue=pVector->GetPgMultiFileIndex(lin);wInt(&ivalue);
					ivalue=pVector->GetStoreFlag(lin);wSize_t(&ivalue);
				}
				else {
					pSingle=dynamic_cast<OutSingle*>(pvv[i].pBase);
					if(pSingle!=nullptr) {
						group=pSingle->GetGroup();
						wString(&group);
						name=pSingle->GetName();
						wString(&name);
						//iv=1;
						//wInt(&iv);
						lin=string::npos;
						wSize_t(&lin);
						float fvalue;
						fvalue=pSingle->GetInitial();wFloat(&fvalue);
						fvalue=pSingle->GetFinal();wFloat(&fvalue);
						fvalue=pSingle->GetMin();wFloat(&fvalue);
						fvalue=pSingle->GetMax();wFloat(&fvalue);
						fvalue=pSingle->GetMean();wFloat(&fvalue);
						fvalue=pSingle->GetAccumulated();wFloat(&fvalue);
						ivalue=string::npos;wSize_t(&ivalue);
						//ivalue=pSingle->GetPgSingleFileIndex();wInt(&ivalue);
						//ivalue=pSingle->GetPgMultiFileIndex();wInt(&ivalue);
						ivalue=pSingle->GetStoreFlag();wSize_t(&ivalue);
						//}
					}
				}
		//	}
		}
	}
	return true;
}
bool NewMap::Info_Validation(bool reading)
{
	Ps *pP=nullptr;
	string type,group,name;
	size_t iv;
	Sw *pSw;
//	G *pGD;
	SimB *pSimB;
	size_t m_NewLogFunc, m_NewBoxCox;


	
	VALv vst;
	vst.Monitoring=1;
	vst.Duration=60;
	vst.P_Error=0.15f;
	vst.A_Error=0.1f;
	vst.P_NewError=0.;
	vst.A_NewError=0.1f;
	vst.NewBoxCox_Power=1;
	vst.NewBoxCox_Offset=1;
	vst.ValidationFileNumber=1;
	vst.LogTrans=false;
	for(size_t i=0;i<MAXSIMVAL;i++)
		m_ValidationData.m_NumberSelectedValidationVar[i]=0;


	if(reading) {
		m_Val_Array.clear();
		rSize_t(&iv);
		rString(&type);
		if(type!="Validation")
			return false;
		else 
		{
			for(size_t i=0; i<iv;i++) {
				rSize_t(&vst.ValidationFileIndex);rSize_t(&vst.OutputType);rString(&vst.Group);rString(&vst.Name);rSize_t(&vst.LocalIndex);
				rSize_t(&vst.nNumber);	rFloat(&vst.R2);rFloat(&vst.A0);rFloat(&vst.A1);
				rFloat(&vst.ME);rFloat(&vst.RMSE);rFloat(&vst.MeanSim);rFloat(&vst.MeanVal);
				if(m_FileVersionNumberRead>49) {
					rFloat(&vst.P_Error);rFloat(&vst.A_Error);rDouble(&vst.LogLi);rbool(&vst.AccTest);
					rSize_t(&vst.ValidationFileNumber);rbool(&vst.LogTrans);rSize_t(&vst.Duration);
					rSize_t(&vst.Monitoring);rFloat(&vst.NSE);
					rFloat(&vst.P_NewError);rFloat(&vst.A_NewError);
					rFloat(&vst.NewBoxCox_Power);rFloat(&vst.NewBoxCox_Offset);
				}
				else if(m_FileVersionNumberRead>48) {
					rFloat(&vst.P_Error);rFloat(&vst.A_Error);rDouble(&vst.LogLi);rBOOL(&vst.AccTest);
					rSize_t(&vst.ValidationFileNumber);rBOOL(&vst.LogTrans);rSize_t(&vst.Duration);
					rSize_t(&vst.Monitoring);rFloat(&vst.NSE);
					rFloat(&vst.P_NewError);rFloat(&vst.A_NewError);
					rFloat(&vst.NewBoxCox_Power);rFloat(&vst.NewBoxCox_Offset);
				}
				else if(m_FileVersionNumberRead>43) {
					rFloat(&vst.P_Error);rFloat(&vst.A_Error);rDouble(&vst.LogLi);rBOOL(&vst.AccTest);
					rSize_t(&vst.ValidationFileNumber);rBOOL(&vst.LogTrans);rSize_t(&vst.Duration);
					rSize_t(&vst.Monitoring);
				}
				else if(m_FileVersionNumberRead>=37) {
					rFloat(&vst.P_Error);rFloat(&vst.A_Error);rDouble(&vst.LogLi);rBOOL(&vst.AccTest);
				}

				size_t koll;
				if(vst.ValidationFileNumber>0&&vst.ValidationFileNumber<MAXSIMVAL+1) {
					
					m_ValidationData.m_NumberSelectedValidationVar[vst.ValidationFileNumber-1]++;
					F* pF=ValidationFilePointer(vst.ValidationFileNumber);
					if(pF->GetNumberOfSelectedVal()==0) pF->ResetValNumbers();
					if(pF->SetValNumber(vst.ValidationFileIndex, size_t(i+1))) {
						koll=pF->GetNumberOfSelectedVal();
					}
				}

				string typstr="";
				switch(vst.OutputType) {
					case 1:	typstr="State Variables"; break;
					case 2:	typstr="Flow Variables"; break;
					case 3:	typstr="Auxiliary Variables"; break;
					case 4:	typstr="Driving Variables"; break;	
				}
				pSimB=GetPtr(typstr,vst.Group,vst.Name);
				if(pSimB==nullptr) {

							if(vst.Name=="Soil Resp no roots") vst.Name="Soil Resp (no roots)";
							pSimB=GetPtr(typstr,vst.Group,vst.Name);

				}
				m_Val_Array.push_back(vst);				
				if(pSimB!=nullptr) {
					if(pSimB->GetType()<5)
						((OutVector*)pSimB)->SetValidFlagIndex(vst.LocalIndex, i+1);
					else 
						((OutSingle*)pSimB)->SetValidFlagIndex( i+1);
				}
			}

			size_t count_valid_files=0;
			for(size_t i=0;i<MAXSIMVAL;i++) {
				if(ValidationFilePointer(i+1)->m_Exist) {
					ValidationFilePointer(i+1)->SetNumberOfSelectedVal(m_ValidationData.m_NumberSelectedValidationVar[i]);
					count_valid_files++;
				}
				else if(m_ValidationData.m_NumberSelectedValidationVar[i]>0){
					//m_F_Array.push_back(ValidationFilePointer(i));
					ValidationFilePointer(i+1)->SetNumberOfSelectedVal(m_ValidationData.m_NumberSelectedValidationVar[i]);
					count_valid_files++;
				}
			}
			Sw *pSw;
			pSw=(Sw*)GetPtr(SWITCH, "ValidationMode");
			m_NumValidFiles=GetSw( "ValidationMode")->GetIntValue();
			if(count_valid_files>m_NumValidFiles) {
				pSw->SetIntValue(count_valid_files);
				m_NumValidFiles=GetSw( "ValidationMode")->GetIntValue();
			}

			Tab *pTab=(Tab*)GetPtr(TABLE,"Scaling of Observed Variables" );
			if(pTab!=nullptr) {
				auto oldnelements=pTab->GetNumOfRow();
		  		if (iv!=oldnelements)  pTab->SetNumOfRow(iv);
			
			}
			if(m_FileVersionNumberRead>48) {
				rSize_t(&m_NewLogFunc);rSize_t(&m_NewBoxCox);
				enum simtype typ=SWITCH;
				group="Technical";
				name="Likelihood Function";
				pSw=(Sw*)GetPtr(typ,  name);
				pSw->SetIntValue(m_NewLogFunc);
				name="Box-Cox Tranformation";
				pSw=(Sw*)GetPtr(typ, name);
				pSw->SetIntValue(m_NewBoxCox);
			}
			if(IsFinishedSimulation())
				m_ValidationData.SetPointersToOutputValidationFiles();
		}
		
	}
	else {
		iv=m_Val_Array.size();
		if(iv>0)
		if (MR_GetValidIndex_IndexInValFile_InDim1() != string::npos) {
			auto NumberofDim1 = m_MultiRun_Array[0].NumberOfRepeatitionsWithinDimension;
			if(NumberofDim1>0 && (iv/NumberofDim1 > 1)) 	iv =  iv/NumberofDim1;
		}

		type="Validation";
		wSize_t(&iv); wString(&type);
		for (size_t i=0; i<iv; i++) {
			vst=m_Val_Array[i];
			wSize_t(&vst.ValidationFileIndex);wSize_t(&vst.OutputType);wString(&vst.Group);wString(&vst.Name);wSize_t(&vst.LocalIndex);
				wSize_t(&vst.nNumber);	wFloat(&vst.R2);wFloat(&vst.A0);wFloat(&vst.A1);
				wFloat(&vst.ME);wFloat(&vst.RMSE);wFloat(&vst.MeanSim);wFloat(&vst.MeanVal);
				wFloat(&vst.P_Error);wFloat(&vst.A_Error);wDouble(&vst.LogLi);wbool(&vst.AccTest);
				wSize_t(&vst.ValidationFileNumber);wbool(&vst.LogTrans);wSize_t(&vst.Duration);
				wSize_t(&vst.Monitoring);wFloat(&vst.NSE);
				wFloat(&vst.P_NewError);wFloat(&vst.A_NewError);
				wFloat(&vst.NewBoxCox_Power);wFloat(&vst.NewBoxCox_Offset);
		}


		enum simtype typ=SWITCH;
		name="Likelihood Function";
		group="Technical";
		pSw=(Sw*)GetPtr(typ, name);
		m_NewLogFunc=pSw->GetIntValue();
		name="Box-Cox Tranformation";
		pSw=(Sw*)GetPtr(typ, name);
		m_NewBoxCox=pSw->GetIntValue();
		wSize_t(&m_NewLogFunc);wSize_t(&m_NewBoxCox);
	}	
	return true;
}
bool NewMap::Info_SumValidation(bool reading)
{
	Ps *pP=nullptr;
	SimB *pSimB;
	string type,group,name;
	size_t iv;
	VALSUMv vst;
	vst.SimValueSource = SIMSUMSOURCE::MULTIFILESTAT;
	vst.Sim_Value_Residual_Index = 0;



	if(reading) {
		m_ValSum_Array.clear();

		if(m_FileVersionNumberRead<32) return true;
		rSize_t(&iv);
		rString(&type);
		if(type=="ExtraValidation") {
			for(size_t i=0; i<iv;i++) {
				vst.ObsScaling = false;
				vst.MultiBin_StartMin = vst.MultiBin_EndMin = 0;
				rSize_t(&vst.OutputType);rSize_t(&vst.SumVarType);
				rString(&vst.Group);rString(&vst.Name);
				rSize_t(&vst.LocalIndex);
				float fvalue; double dvalue;
				rFloat(&vst.ME_mean); rFloat(&vst.RMSE_mean);rFloat(&vst.SimValue);
				rFloat(&vst.ObsValue);rFloat(&vst.P_Error);
				rFloat(&vst.A_Error);
				rDouble(&vst.LogLi_mean);
				if (m_FileVersionNumberRead > 61) {
					rbool(&vst.FlexSumVar);
					rString(&vst.IDFileName);
					rString(&vst.IDVarName);
					if (m_FileVersionNumberRead > 62) {

						if(m_FileVersionNumberRead > 63) rSize_t(&vst.ObsScaling);
						else {
							bool bvalue;
							rbool(&bvalue);
							if (bvalue)
								vst.ObsScaling = 1;
							else
								vst.ObsScaling = 0;
						}
						rDouble(&vst.ObsScalingFactor);
						if (m_FileVersionNumberRead > 64) {
							size_t source;
							rSize_t(&source); vst.SimValueSource = SIMSUMSOURCE(source);
							rSize_t(&vst.Sim_Value_Residual_Index);
							if (vst.SimValueSource == SIMSUMSOURCE(2)) {
								rUint(&vst.MultiBin_StartMin);
								rUint(&vst.MultiBin_EndMin);
							}
						}
					}
					else
						vst.ObsScaling = 0;
				}
				else {
					vst.FlexSumVar = false;
					vst.IDFileName = "";
					vst.IDVarName = "";
				}
				m_ValSum_Array.push_back(vst);
				switch(vst.OutputType) {
					case 1:
						type="State Variables";
						break;
					case 2:
		 				type="Flow Variables";
		 				break;
					case 3:
						type="Auxiliary Variables";
	 					break;
					 case 4:
		 				type="Driving Variables";
	 					break;
				}
				
				pSimB=GetPtrFromIntType(vst.OutputType, vst.Group, vst.Name);
				if(pSimB==nullptr)  pSimB=GetPtrFromIntType(1, vst.Group, vst.Name);
				if(pSimB==nullptr)  pSimB=GetPtrFromIntType(2, vst.Group, vst.Name);
				if(pSimB==nullptr)  pSimB=GetPtrFromIntType(3, vst.Group, vst.Name);

				if(pSimB!=nullptr) {
					if(pSimB->Is_Vector())
						((OutVector*)pSimB)->SetValidFlagIndex(vst.LocalIndex,i+1);
					else
						((OutSingle*)pSimB)->SetValidFlagIndex(i+1);

				}

			}
		}
		CheckFileNameLinkedto_SV_Variables();
		if (m_FileVersionNumberRead > 68) {
			rSize_t(&iv);
			m_MStorage.m_ValSumValues_Stored.clear();

			for (size_t i = 0; i < iv; i++) {
				VALSUM valsum;
				string type, group, name;
				size_t index;
				rString(&type); rString(&group); rString(&name);
				SIMB simb;
				simb.pBase = GetPtr(type,group, name);
				rSize_t(&simb.TabIndex);
				valsum.simb = simb;
				rUint(&valsum.SumIndex);
				rFloat(&valsum.ValueMin);
				rFloat(&valsum.ValueOpt);
				rFloat(&valsum.ValueMax);
				m_MStorage.m_ValSumValues_Stored.push_back(valsum);
			}

		}

	}
	else {
		iv = m_ValSum_Array.size();
		type = "ExtraValidation";
		wSize_t(&iv);	wString(&type);

		for (size_t i = 0; i < iv; i++) {
			vst = m_ValSum_Array[i];
			wSize_t(&vst.OutputType); wSize_t(&vst.SumVarType);
			wString(&vst.Group); wString(&vst.Name);
			wSize_t(&vst.LocalIndex);
			wFloat(&vst.ME_mean); wFloat(&vst.RMSE_mean); wFloat(&vst.SimValue);
			wFloat(&vst.ObsValue); wFloat(&vst.P_Error);
			wFloat(&vst.A_Error);
			wDouble(&vst.LogLi_mean);
			wbool(&vst.FlexSumVar);
			wString(&vst.IDFileName);
			wString(&vst.IDVarName);
			wSize_t(&vst.ObsScaling);
			wDouble(&vst.ObsScalingFactor);
			int itemp = int(SIMSUMSOURCE(vst.SimValueSource));
			wInt(&itemp);
			wSize_t(&vst.Sim_Value_Residual_Index);
			if (vst.SimValueSource == SIMSUMSOURCE::MULTI_OUTPGFILE) {
				wUint(&vst.MultiBin_StartMin);
				wUint(&vst.MultiBin_EndMin);
			}
		}

		iv = m_MStorage.m_ValSumValues_Stored.size();
		wSize_t(&iv);
		for (size_t i = 0; i < iv; i++) {
			auto vst = m_MStorage.m_ValSumValues_Stored[i];
			auto pBase = vst.simb.pBase;
			size_t index = vst.simb.TabIndex;

			string type = pBase->GetTypeName();
			string group = pBase->GetGroup();
			string name = pBase->GetName();
			wString(&type); wString(&group); wString(&name);
			wSize_t(&vst.simb.TabIndex);
			wUint(&vst.SumIndex);
			wFloat(&vst.ValueMin);
			wFloat(&vst.ValueOpt);
			wFloat(&vst.ValueMax);
		}

	}
	//if(iv>0&&IsFinishedSimulation())
			//UpdateValidationPerformance_SmartFlexVariables();
	
	


	return true;
}
bool NewMap::Info_RunReport(bool reading)
{
	size_t iv;
	string str;
	if(reading) {
		m_Report_Array.clear();
		rSize_t(&iv);
		rString(&str);
		if (str == "RunReport") {
			for (size_t i = 0; i < iv; i++) {
				rString(&str);
				FUtil::trim(str);
				if (ReadResultsFromFile && iv < 20) m_Report_Array.push_back(str);
			}
		}
		else
			return false;
	}
	else {
		iv=m_Report_Array.size();
		str="RunReport";
		wSize_t(&iv);
		wString(&str);
		for(size_t i=0; i<iv;i++) {
			str=m_Report_Array[i];
			wString(&str);
		}
	}
	return true;
}
bool NewMap::Info_MultiRun(bool reading)
{	
	SimB *pP=nullptr;
	string type,group,name;
	size_t iv;
	float fvalue;
	SIMB valPAR;
	MRv valMR;
	size_t numpar;
	int imethod, imonitor;

	if(reading) {
		m_MultiRun_Array.clear();
		m_P_MR_Array.clear();
		rSize_t(&iv);
		rString(&type);
		if(m_FileVersionNumberRead>32)
			rFloat(&m_BayesianStepSize);
		if(m_FileVersionNumberRead>52) {
			size_t ib;
			rSize_t(&ib);
			SetNumBurnIn(ib);
		}
		if(type=="MultiRunOptions"&&iv>0) {

			m_MStorage.Init(this);
			for(size_t i=0; i<iv;i++) {
				rSize_t(&valMR.NumberOfRepeatitionsWithinDimension);
				rSize_t(&valMR.nCount);
				m_MultiRun_Array.push_back(valMR);
				rSize_t(&numpar);
				for(size_t ii=0; ii<numpar;ii++) {
					rString(&group);
					rString(&name);
					if(group=="DataBase") {
						   CDB *pDB;
						   string strID, strIDKey;
						   pDB=DB_GetPointer(name);
						   valPAR.pBase=pDB;
						   int iread;
						   rInt(&iread);
						   if(iread<0) valPAR.TabIndex = string::npos;
						   else valPAR.TabIndex = iread;
						   rInt(&imethod);
						   if(m_FileVersionNumberRead>43) {
							   rInt(&imonitor);  // no use of imonitoring Sept 2008
						   }
						   
						   pDB->MR_Set_Method(MR_METHOD(imethod));
						   for(size_t i3=0;i3<valMR.NumberOfRepeatitionsWithinDimension;i3++) {
							   	   string strID, strIDKey;
								   rString(&strID);
								   rString(&strIDKey);
  								   pDB->MR_AddSelection(strID, strIDKey);
						   }
						   size_t idim;
						   rSize_t(&idim);
						   pDB->MR_Set_Dim(idim);
						   pDB->MR_Set_Method(MR_METHOD(5));
						   MR_CP_Set((Ps*)pDB,-1,true);
						   

					}
					else {  // Non-data base
						type="Parameters";
  						if(name=="Index in val file"&&group=="Irrigation") group="Technical";
						if(name=="Size of internal loop"&&group=="Irrigation") group="Technical";
						if(name=="No of internal index loop"&&group=="Irrigation") group="Technical";
						if(name=="SaltHalfReductionn") name="";
						pP=GetPtr(type, group, name);
						if(pP==nullptr) {
							if(name=="Intial Soil Storage") name="Initial Soil Storage";
							if(name=="Intial Peak Storage") name="Initial Peak Storage";
							if(name=="Intial Base Storage") name="Initial Base Storage";
							if (name == "Index for CsvFile") name = "Index Fert Table CsvFile";
							pP=GetPtr(type, group, name);
						}


						if(pP==nullptr) {
  							type="Table Parameters";
							if(name=="SaltHalfReduction") {
								name="Salt c50";
								valPAR.TabIndex=0;
							}
							int iread;
							rInt(&iread); 
							if (iread < 0) valPAR.TabIndex = string::npos;
							else valPAR.TabIndex=size_t(iread);
							
							SimB *pPar;
							//pPar=GetPtr(type,group,name);
							pPar=GetPtr(PAR_TABLE, name);
							rInt(&imethod);
							P* pkoll= dynamic_cast<P*>(pPar);
						   valPAR.pBase=dynamic_cast<SimB*>(pPar);
						   if(pPar==nullptr)  return false;
							   if(valPAR.TabIndex==string::npos) valPAR.TabIndex=0;
							   if(m_FileVersionNumberRead>43) {
								   rInt(&imonitor);  // no use of imonitoring Sept 2008
								   if(!pPar->Is_PTable())
  											((Ps*)pPar)->MR_Set_Monitoring(imonitor);
										else
											((P*)pPar)->MR_Set_Monitoring(valPAR.TabIndex,imonitor);
							   }
							   if(!pPar->Is_PTable())
								   ((Ps*)pPar)->MR_Set_Method(MR_METHOD(imethod));
							   else {
								   if(((P*)pPar)->GetSize()<=valPAR.TabIndex) valPAR.TabIndex=((P*)pPar)->GetSize()-1;
								   ((P*)pPar)->MR_Set_Method(valPAR.TabIndex, MR_METHOD(imethod));
							   }
							   if(imethod==4) {
  									for(size_t iii=0;iii<valMR.NumberOfRepeatitionsWithinDimension;iii++) {
  										   rFloat(&fvalue);
										   double dvalue=fvalue;
	  									   ((P*)pPar)->MR_Append_TableValue(valPAR.TabIndex,dvalue);
									}	

							   }
							   else if(imethod>5&&imethod<9) {
								   int deptabindex;
								   rString(&group);
								   rString(&name);
								   rInt(&deptabindex);
								  // if(m_FileVersionNumberRead<60) deptabindex--;
								   if(deptabindex<0) 
										type="Parameters";
									else
								 		type="Table Parameters";
								   SimB *pParDep;
								   pParDep=GetPtr(type,group, name);
								   if(pParDep==nullptr&&deptabindex<0) {
									   type="Table Parameters";
									   pParDep=GetPtr(type,group, name);
									   deptabindex=0;
									}
							 
								   if(pParDep!=nullptr) {								
										if(deptabindex<0)
											((P*)pPar)->Set_MR_ParDependence(valPAR.TabIndex, pParDep);
										else
											((P*)pPar)->Set_MR_ParDependence(valPAR.TabIndex, pParDep, deptabindex);
											
								  }
									else {
											// not identified parameter dependence

									}
									if(imethod>6) {
											rFloat(&fvalue);
											if(valPAR.TabIndex==string::npos) 
												((Ps*)pPar)->MR_Set_Min(fvalue);
											else
												((P*)pPar)->MR_Set_Min(valPAR.TabIndex, fvalue);
											rFloat(&fvalue);
											if(valPAR.TabIndex==string::npos) 
												((Ps*)pPar)->MR_Set_Max(fvalue);
											else
												((P*)pPar)->MR_Set_Max(valPAR.TabIndex, fvalue);
									}
								}
							   else {
										rFloat(&fvalue);
										if(valPAR.TabIndex==string::npos) 
											((Ps*)pPar)->MR_Set_Min(fvalue);
										else
											((P*)pPar)->MR_Set_Min(valPAR.TabIndex, fvalue);
										rFloat(&fvalue);
										if(valPAR.TabIndex==string::npos) 
											((Ps*)pPar)->MR_Set_Max(fvalue);
										else
											((P*)pPar)->MR_Set_Max(valPAR.TabIndex, fvalue);
										if(m_FileVersionNumberRead>30) {
											rFloat(&fvalue);
											double dvalue=fvalue;
											if(valPAR.TabIndex==string::npos) 
												((Ps*)pPar)->MR_Set_Start(dvalue);
											else
												((P*)pPar)->MR_Set_Start(valPAR.TabIndex, dvalue);
										}

								 }
								 size_t idim;
								 rSize_t(&idim);
								 if(valPAR.TabIndex==string::npos) 
									((Ps*)pPar)->MR_Set_Dim(idim);
								 else
									((P*)pPar)->MR_Set_Dim(valPAR.TabIndex, idim);
								
	
					}

//// single value PARAMETER CASE BELOW

					else {  // Non-data base
						   valPAR.pBase=pP;
						   //valPAR.Type=0;

						   int iread;
							rInt(&iread);
							if (iread < 0)
								valPAR.TabIndex = string::npos;
							else
								valPAR.TabIndex = iread;
							rInt(&imethod);
						   if(m_FileVersionNumberRead>43) {
							   rInt(&imonitor);  // no use of imonitoring Sept 2008
							   if(valPAR.pBase->Is_P())
  										((Ps*)pP)->MR_Set_Monitoring(imonitor);
									else
										((P*)valPAR.pBase)->MR_Set_Monitoring(valPAR.TabIndex,imonitor);
						   }
						   ((Ps*)pP)->MR_Set_Method(MR_METHOD(imethod));
						   if(imethod==4) {
  								for(size_t iii=0;iii<valMR.NumberOfRepeatitionsWithinDimension;iii++) {
  									   rFloat(&fvalue);
	  								   ((Ps*)pP)->MR_Append_TableValue(fvalue);
								}	

						   }
						   else if(imethod>5&&imethod<9) {
							   int deptabindex;
							   rString(&group);
							   rString(&name);
							   rInt(&deptabindex);
							   if(deptabindex<0) 
									type="Parameters";
								else
								 	type="Table Parameters";
							   Ps *pParDep;
							   pParDep=(Ps*)GetPtr(type,group, name);
							 
							   if(pParDep!=nullptr) {								
									if(deptabindex<0)
											((Ps*)pP)->Set_MR_ParDependence(pParDep);
										else
											((Ps*)pP)->Set_MR_ParDependence(pParDep, deptabindex);
								}
								else {
									// not identified parameter dependence

								}
								if(imethod>6) {
									rFloat(&fvalue);
									if(valPAR.pBase->Is_P()) 
										((Ps*)pP)->MR_Set_Min(fvalue);
									else
										((P*)pP)->MR_Set_Min(valPAR.TabIndex, fvalue);
									rFloat(&fvalue);
									if(valPAR.pBase->Is_P()) 
										((Ps*)pP)->MR_Set_Max(fvalue);
									else
										((P*)pP)->MR_Set_Max(valPAR.TabIndex, fvalue);
								}
						   
						}
						else {
							
								rFloat(&fvalue);
								if(pP->Is_P()) 
									((Ps*)pP)->MR_Set_Min(fvalue);
								else 
									((P*)pP)->MR_Set_Min(valPAR.TabIndex, fvalue);
								rFloat(&fvalue);
								if(pP->Is_P()) 
									((Ps*)pP)->MR_Set_Max(fvalue);
								else
									((P*)pP)->MR_Set_Max(valPAR.TabIndex, fvalue);
								if(m_FileVersionNumberRead>30) {
									rFloat(&fvalue);
									if(pP->Is_P()) 
										((Ps*)pP)->MR_Set_Start(fvalue);
									else
										((P*)pP)->MR_Set_Start(valPAR.TabIndex, fvalue);
								}

						 }
						 size_t idim;
						 rSize_t(&idim);
						 if(valPAR.pBase->Is_P()) 
							((Ps*)pP)->MR_Set_Dim(idim);
						 else
							 ((P*)pP)->MR_Set_Dim(valPAR.TabIndex, idim);
					}
					m_P_MR_Array.push_back(valPAR);
					}
				}
			}
		}
		else {

			//No MultiRun -No action
		}
	
		MR_CheckCalibMethod();
		if(m_FileVersionNumberRead>32&&m_iError<5) {
				rSize_t(&iv);
				if(iv>0&&iv<1000000) {
						MR_Init_BayesCovar(iv);
						MR_Init_BayesCocor(iv);
					   // Num_BayesPar=iv;
						MR_Set_NumBayesPar(iv);
						for(size_t i=0;i<iv;i++) {
							rFloat(&fvalue);
							MR_Set_BayesMean(i, fvalue);
							for(size_t ii=0;ii<iv;ii++) {
								rFloat(&fvalue);
								MR_Set_BayesCoVar(i,ii,fvalue);
								if(m_FileVersionNumberRead>33) {
									rFloat(&fvalue);
									MR_Set_BayesCoCor(i,ii,fvalue);
								}
							}
					}
				}
			}
			//if(IsBayesianCalibration()) {
				MR_ReCalculatePostDist(IsBayesianCalibration());

			auto pSw=(Sw*)GetPtr(SWITCH,"TimeSerie Errors File");
			if(pSw!=nullptr) {
				auto i=pSw->GetIntValue();
				if(i>0)
					m_OutputValidXBinFileExist=true;
				else
					m_OutputValidXBinFileExist=false;
			}
			//}
		if(m_FileVersionNumberRead>60&&m_ValidationData.GetNumSumVarVariables()>0) {
			if(!m_ValidationData.IsSumValSet()&&IsMultiSimulation()&&IsFinishedSimulation()) {
				bool result;
				string pgfilename = GetOutputFileName(0, false, true);
				if (pgfilename.size() > 0) {
					m_PG_MultiOutputFile.Open(pgfilename);
					m_PG_MultiOutputFile.CloseFile();
				}
				result=CalculatePerformanceAndSetValues();
				m_ValidationData.SetSumValSet(result);
			}


		}


	}

	else { //Writing Section

			iv=m_MultiRun_Array.size();
			numpar=m_P_MR_Array.size();
			type="MultiRunOptions";
			wSize_t(&iv);
			wString(&type);
			wFloat(&m_BayesianStepSize);
			auto nb=GetNumBurnIn();
			wSize_t(&nb);
			

			for(size_t i=0; i<iv;i++) {
				valMR=m_MultiRun_Array[i];
				wSize_t(&valMR.NumberOfRepeatitionsWithinDimension);
				wSize_t(&valMR.nCount);
				numpar=MR_Get_NumberOfParametersWithinDim(i+1);
				wSize_t(&numpar);
                string group_psw;
                string name_psw;
				for(size_t ii=0; ii<numpar;ii++) {

					valPAR.pBase = MR_Get_pPar(i + 1, ii + 1);
					valPAR.TabIndex = MR_GetTabIndex(i + 1, ii + 1);
                    group_psw = MR_GetParGroup(i+1,ii+1);
                    name_psw = MR_GetParName(i+1,ii+1);
					wString(&group_psw);
					wString(&name_psw);
	
					wSize_t(&valPAR.TabIndex);
	
					auto imethod=MR_GetMethod(i+1,ii+1);
					wSize_t(&imethod);
					auto imon=MR_GetMonitoring(i+1,ii+1);
					wSize_t(&imon);
					if(imethod==4) {
						SimB* pCP=MR_Get_pPar(i+1,ii+1);
						for(size_t iii=0;iii<MR_GetNumberOfRepeationsForThisDimension(i);iii++){
							if(MR_GetTabIndex(i+1,ii+1)==-1)
								 fvalue=float(((Ps*)pCP)->MR_Get_TabValue(iii));
							else 
								 fvalue=float(((P*)pCP)->MR_Get_TabValue(MR_GetTabIndex(i+1,ii+1),iii));
							wFloat(&fvalue);
						}
					}
					else if(imethod==5) {
						CDB* pCP=(CDB*)MR_Get_pPar(i+1,ii+1);
                        string selection_pcp;
                        string keyselection_pcp;
						for(size_t iii=0;iii<MR_GetNumberOfRepeationsForThisDimension(i);iii++){
                            selection_pcp = pCP->MR_GetSelection(iii);
                            keyselection_pcp = pCP->MR_GetKeySelection(iii);
							wString(&selection_pcp);
							wString(&keyselection_pcp);
						}
					}
					else if(imethod>5&&imethod<9) {
						SimB* pParDep;
						SimB* pCP=MR_Get_pPar(i+1,ii+1);
						size_t indpar,ind_dep;
						indpar=MR_GetTabIndex(i+1,ii+1);
						if(pCP->Is_P()) {
							pParDep=((Ps*)pCP)->Get_MR_ParDependence();
							ind_dep=((Ps*)pCP)->Get_MR_ParDependenceIndex();
						}
						else {
							pParDep=((P*)pCP)->Get_MR_ParDependence(indpar);
							ind_dep=((P*)pCP)->Get_MR_ParDependenceIndex(indpar);
						}
						if (pParDep != nullptr) {
                            string group_psw = pParDep->GetGroup();
                            string name_psw = pParDep->GetName();
							wString(&group_psw);
							wString(&name_psw);
							wSize_t(&ind_dep);
							if (MR_GetMethod(i + 1, ii + 1)>6) {
								fvalue = float(MR_GetMin(i + 1, ii + 1)); wFloat(&fvalue);
								fvalue = float(MR_GetMax(i + 1, ii + 1)); wFloat(&fvalue);
							}
						}
						else {

							int ivvv = 0;
						}
					}
					else {
						fvalue=float(MR_GetMin(i+1,ii+1));wFloat(&fvalue);
						fvalue=float(MR_GetMax(i+1,ii+1));wFloat(&fvalue);						
						fvalue=float(MR_GetStart(i+1,ii+1));wFloat(&fvalue);	
					}
					size_t i1=i+1;
					wSize_t(&i1);
				}
			}
			iv=MR_Get_NumTotalPar();
			//Num_BayesPar=iv;
			wSize_t(&iv);
			for(size_t i=0;i<iv;i++) {
				fvalue=MR_Get_BayesMean(i);
				wFloat(&fvalue);
				for(size_t ii=0;ii<iv;ii++) {
					fvalue=MR_Get_BayesCoVar(i,ii);wFloat(&fvalue);
					fvalue=MR_Get_BayesCoCor(i,ii);wFloat(&fvalue);
				}
			}



		}	 
	return true;
}
bool NewMap::Info_WithinRunOptions(bool reading)
{

	Ps *pP=nullptr;
	string type,group,name;
	size_t iv;
	float fvalue;
	CHPARv ch;
	P* pPt;
	unsigned int datemin;
	ch.nCount=0;

	if(reading) {
		m_P_CH_Array.clear();
		rSize_t(&iv);
		rString(&type);
		if (type == "WithinRunOptions") {
			for (size_t i = 0; i < iv; i++) {
				rString(&group);
				rString(&name);
				rSize_t(&ch.NumDates);
				if (group != "DataBase") {
					ch.TabIndex = 0;
					rSize_t(&ch.TabIndex);
					//ch.NumDates--;
					type = "Parameters";
					pP = (Ps*)GetPtr(type, group, name);

					if (pP == nullptr) {
						type = "Table Parameters";
						ch.Type = 1;

						pPt = (P*)GetPtr(type, group, name);
						if (pPt == nullptr) return false;
						ch.pBase = pPt;
						if (m_FileVersionNumberRead < 38) {
							ch.TabIndex--;
							if (ch.TabIndex < 0) ch.TabIndex = 0;
						}
						for (size_t ii = 0; ii < ch.NumDates; ii++) {
							rUint(&datemin);
							rFloat(&fvalue);
							pPt->SetChaDateAndValue(datemin, ch.TabIndex, fvalue);
						}
					}
					else {
						// "Parameters";
						ch.Type = 0;
						ch.pBase = pP;
						ch.TabIndex = -1;
						//rLong(&datemin);
						//rFloat(&fvalue);
						for (size_t ii = 0; ii < ch.NumDates; ii++) {
							rUint(&datemin);
							rFloat(&fvalue);
							pP->SetChaDateAndValue(datemin, fvalue);
						}
					}
				}
				else {
					// "DataBase";
					ch.Type = 2;
					CDB* pDB;
					pDB = DB_GetPointer(name);
					ch.pBase = pDB;
					string Id, Key;
					for (size_t ii = 0; ii < ch.NumDates; ii++) {
						rUint(&datemin);
						rString(&Id);
						rString(&Key);
						pDB->SetChaDateAndValues(datemin, Id, Key);
					}
				}
				m_P_CH_Array.push_back(ch);
			}
		}
		else return false;
	}
	else {
		iv=m_P_CH_Array.size();
		wSize_t(&iv);
		type="WithinRunOptions";
		wString(&type);
		for(size_t i=0; i<iv;i++) {
			ch=m_P_CH_Array[i];
			Ps* pPar=(Ps*)ch.pBase;
			auto TabIndex=ch.TabIndex;
			if(ch.Type==0) {
				pPar=(Ps*)ch.pBase;
                string group_psw = pPar->GetGroup();
                string name_psw = pPar->GetName();
				wString(&group_psw);
				wString(&name_psw);
				wSize_t(&ch.NumDates);
				wSize_t(&ch.TabIndex);
				for(size_t ii=0; ii<ch.NumDates;ii++) {
					unsigned int min=pPar->GetChaDate( ii);
					wUint(&min);
					fvalue=float(pPar->GetChaParValue(ii));
					wFloat(&fvalue);
				}
			}
			else if(ch.Type==1) {
				P* pPar=(P*)ch.pBase;
                string group_psw = pPar->GetGroup();
                string name_psw = pPar->GetName();
				wString(&group_psw);
				wString(&name_psw);
				wSize_t(&ch.NumDates);
				wSize_t(&ch.TabIndex);
				for(size_t ii=0; ii<ch.NumDates;ii++) {
					unsigned int min=pPar->GetChaDate(ch.TabIndex, ii);
					wUint(&min);
					fvalue=float(pPar->GetChaParValue(ch.TabIndex, ii));
					wFloat(&fvalue);
				}
			}
			else if(ch.Type==2) {
				CDB *pDB;
				pDB=(CDB*)ch.pBase;
				group="DataBase";
				wString(&group);
				name=pDB->GetName();
				wString(&name);
				wSize_t(&ch.NumDates);
                string chaparvalue;
                string chaparkey;
				for(size_t ii=0; ii<ch.NumDates;ii++) {
					unsigned int min=pDB->GetChaDate(ii);
					wUint(&min);
                    chaparvalue = pDB->GetChaParValue(ii);
                    chaparkey = pDB->GetChaParKey(ii);
					wString(&chaparvalue);
					wString(&chaparkey);
				}
			}

		}

	}

return true;
}
bool NewMap::Info_FinalStateVariables(bool reading)
{

	X *pXT=nullptr;
	string type,group,name;
	size_t iv;
	float fvalue;
	size_t nindex;
	vector<SimB*> vpp, vp_final;
	long long pos;
	bool trying=true;

	size_t testcount=0;

//	CFloatArray m_WShed_Slope;
//	CUIntArray m_WShed_ID_Receiver;
//	CUIntArray m_WShed_ID;

	if(reading) {
		if(m_FileVersionNumberRead<28) return false;
		pos=m_DocFileIn.tellg();
		m_DocFileIn.read(m_cbuf,40);
		m_DocFileIn.seekg(pos,ios::beg);
		while(trying) {
			rSize_t(&iv);
			rString(&type);
			if(type=="Final State Variable values") {
				type="State Variables";
				for(size_t ii=0; ii<iv;ii++) {
					rString(&group);
					rString(&name);
					rSize_t(&nindex);
					pXT=(X*)GetPtr(STATE,name);
					for( size_t i=0; i<nindex;i++) {
						rFloat(&fvalue);
						if(pXT!=nullptr) pXT->SetOptInitialValue(i,fvalue);
					}
				}
				trying=false;
			}
			else {
				m_DocFileIn.seekg(pos-80,ios::beg);
				m_DocFileIn.read(m_cbuf,200);

				size_t apos=5;
				while(!(m_cbuf[apos]=='F'&&m_cbuf[apos+1]=='i'&&m_cbuf[apos+2]=='n')&&apos<160) 
						apos++;			
				apos-=5;
				pos+=-80+apos;
				m_DocFileIn.seekg(pos,ios::beg);
				testcount++;
				
				if(!m_DocFileIn.good()||testcount>2) return false;




			}
		}
// Also Reading Watershed options

		rString(&type);
		rSize_t(&iv);
		if(type=="WaterShedOptions") {
			if(m_FileVersionNumberRead<50)
				rBOOL(&m_WShed_Active);
			else
				rbool(&m_WShed_Active);
			rSize_t(&m_WShed_ActiveIndex);
			rSize_t(&m_WShed_Number_X_grids);
		}
// The WaterShedoptions are not complete




	}
	else {
		vp_final.clear();
		SimB* pPtr;
		iv=0;
		vpp=GetPtrVector("State Variables","");
		for(size_t i=0; i<vpp.size(); i++) {
			pPtr=(SimB*)vpp[i];
			name=pPtr->GetName();
			if((name=="WaterStorage"||name=="SoilHeat"||name.find("Humus")!=string::npos||name.find("Litter") != string::npos)&&pPtr->Is_Vector() ) {
				iv++;
				vp_final.push_back(pPtr);
			}

		}
		type="Final State Variable values";
		wSize_t(&iv);
		wString(&type);
		for(size_t i=0; i<vp_final.size();i++) {
			pXT=(X*)vp_final[i];
			group=pXT->GetGroup();
			name=pXT->GetName();
			auto nn=pXT->GetNumberOfFlags();
			wString(&group);wString(&name);wSize_t(&nn);
			for(size_t ii=0; ii<nn;ii++) {
				fvalue=pXT->GetFinal(ii);
				//if(fvalue<MISSING*0.5f) fvalue=float(pXT->GetValue(ii));
				if(fvalue<MISSING*0.5f) fvalue=pXT->GetInitial(ii);
				wFloat(&fvalue);
			}
		}
		type="WaterShedOptions";
		iv=m_WShed_ID.size();
		wString(&type);
		wSize_t(&iv);
		wbool(&m_WShed_Active);
		wSize_t(&m_WShed_ActiveIndex);
		wSize_t(&m_WShed_Number_X_grids);
		for(size_t i=0; i<iv;i++) {
//REMAIN
	//		ar<<m_WShed_ID[i];
	//		ar<<m_WShed_ID_Receiver[i];
	//		Value=*(float*)m_WShed_Slope[i];
	//		ar<<Value;
		}



	}	
	return true;
}
bool NewMap::Info_Plottings(bool reading)
{

	Ps *pP=nullptr;
	string type,group,name,str;
	size_t  ncount;
	size_t iChart, iIndex,idoc, nCharts;
	CHART_TYPES iChartType;
	size_t iv;
	SimB *pBase;

	if(reading) {
		rString(&type);
		rSize_t(&ncount);
		rSize_t(&nCharts);

		//m_PlotStorage.SetNumberSavedCharts(ncount);

		rSize_t(&idoc);
		if(idoc==string::npos||idoc>100) {
			ncount=0;
			nCharts=0;
			idoc=0;
			return false;

		}

		if(m_FileVersionNumberRead>38) {
				for(size_t i=0;i<idoc;i++) {
					rString(&str);
			//		m_PlotStorage.m_XTGD_DocNameArray.push_back(str);
				}
		}
		else {
				//m_PlotStorage.m_XTGD_DocNameArray.push_back(GetCurrentSimFile());
		}






		if(type=="PlotSelections"&&ncount>0){
#ifndef COUPSTD
			m_PlotStorage.RemoveAll();

#endif


			void *Ptr;
			size_t ivalfile, imrun, ivarno, iType, id;
			size_t prev_IChart=0, ichart_type;
			for(size_t i=0;i<ncount;i++) {
				rString(&group);
				rString(&name);
				rSize_t(&iType);
				rSize_t(&iIndex);
				rSize_t(&iChart);
				rSize_t(&ichart_type);
				iChartType = CHART_TYPES(ichart_type);
				if(m_FileVersionNumberRead>38) 
					rSize_t(&id);
				else
					id=0;
				if(m_FileVersionNumberRead>42) 
					rSize_t(&ivalfile);
				else
					ivalfile=0;
				if(m_FileVersionNumberRead>44) {
					rSize_t(&ivarno);
					rSize_t(&imrun);
				}
				else {
					ivarno=0;
					imrun=0;
				}

				bool Add=true;
				enum simtype typ;
				typ=STATE;
		   		switch (iType) {
					case 0:
						type="Files";
						typ=PGFILE;
						break;
					case 11: ;case 21:;
					case 1:; case 101:;
						type="State Variables";
						typ=STATE;
						break;
					case 12:; case 22:;
					case 2:;case 102:;
						type="Flow Variables";
						typ=FLOW;
						break;
					case 13:; case 23:;
					case 3: case 103:;
						type="Auxiliary Variables";
						typ=AUX;
						break;
					case 14:; case 24:;
					case 4:; case 104:;
						type="Driving Variables";
						typ=DRIVE;
						break;
					case 5: // Validation Variables
						Ptr=nullptr;
						break;
					case 10:
						bool opennew=true;
#ifndef COUPSTD
						CPG *pPG;
						for(size_t i=0;i<m_PlotStorage.m_PGFileArray.size();i++){
							pPG=(CPG*)m_PlotStorage.m_PGFileArray[i];
							if(pPG->GetFileName()==name.c_str()) {
								opennew=false;			
								Ptr=pPG;
							}
						}
						if(opennew) {
							pPG=new CPG;
							if(pPG->Open(name.c_str(),false)) {
								Ptr=pPG;
								m_PlotStorage.m_PGFileArray.push_back(pPG);
								m_PlotStorage.m_PGFileArrayShow.push_back(0);
								pPG->CloseFile();
							}
							else {
								Add=false;
								delete pPG;
							}
						}
#endif
					}

					if(Add) {
						if (iChart <= prev_IChart) iChart = prev_IChart;
						if(iType!=10&&iType%10!=5) {
							Ptr=GetPtr(typ,  name); 
#ifndef COUPSTD							
							if(m_FileVersionNumberRead>38&&m_FileVersionNumberRead<=44)
								m_PlotStorage.Add(Ptr, iType, iIndex, iChart, iChartType, id, ivalfile);
							if(m_FileVersionNumberRead>44)
								m_PlotStorage.Add(Ptr, iType, iIndex, iChart, iChartType, id, ivalfile, ivarno, imrun);
#endif
							prev_IChart = iChart;
						}
					}
			}

		}


		iv=0;
		size_t idoc=0;
		size_t ivalue=0;
#ifndef COUPSTD
		iv=m_PlotStorage.NumSelected();
		ivalue=m_PlotStorage.GetNumberofCharts();
		idoc=m_PlotStorage.m_XTGD_DocNameArray.size();
#endif



	}
	else {
		type="PlotSelections";
		iv=0;
		size_t idoc=0;
		size_t ivalue=0;
#ifndef COUPSTD
		iv=m_PlotStorage.NumSelected();
		ivalue=m_PlotStorage.GetNumberofCharts();
		idoc=m_PlotStorage.m_XTGD_DocNameArray.size();
#endif

		wString(&type); 
		wSize_t(&iv);
		wSize_t(&ivalue);
		wSize_t(&idoc);
#ifndef COUPSTD
		for(size_t i=0; i<idoc;i++) {
			name=m_PlotStorage.GetDocFileName(i);
			wString(&name);
		}
		for(size_t i=0; i<iv; i++) {
			void *ptr=m_PlotStorage.GetPtr(i);

			auto iType=m_PlotStorage.GetDataType(i);
			
			if(iType==DATA_TYPES::PGDATA&&ptr!=nullptr) {
				CPG *pPG=(CPG*)ptr;
				group="PG File";
				name=pPG->GetFileName();
			}
			else if(ptr==nullptr) {
				//Validation
				group="Validation";
				name="Validation";
			}
			else {
				pBase=(SimB*)ptr;
				group=pBase->GetGroup();
				name=pBase->GetName();
			}

			iIndex=m_PlotStorage.GetLocalIndex(i);
			iChart=m_PlotStorage.GetChart(i);
			iChartType=m_PlotStorage.GetChartType(i);
			idoc=m_PlotStorage.GetDocIndex(i);
			auto ivalfile=m_PlotStorage.GetValFileNo(i);
			auto ivarno=m_PlotStorage.GetValVarNo(i);
			auto imrun=m_PlotStorage.GetValRunNo(i);
			wString(&group);
			wString(&name);
			int ivalue = int(iType);
			wInt(&(ivalue));
			wSize_t(&iIndex);
			wSize_t(&iChart);
			ivalue = int(iChartType);
			wInt(&ivalue);
			wSize_t(&idoc);
			wSize_t(&ivalfile);
			wSize_t(&ivarno);
			wSize_t(&imrun);

		}
#endif
	}



return true;
}
bool NewMap::Info_History(bool reading)
{

	Ps *pP=nullptr;
	string type,group,name, str, User, Computer;
	size_t iv;
	size_t intValue;
	time_t tt;
	HISTv h_node;
	RUNINFO i_node;

	if(reading) {
		m_History_Array.clear();
		rSize_t(&iv);
		rString(&type);
		if(type=="History List") {
			for(size_t i=0; i<iv;i++) {
				if(m_FileVersionNumberRead>49) rSize_t(&h_node.RunInfoType);
				rString(&group);
				rString(&type);
				rString(&name);
				rString(&str);
				rSize_t(&intValue);
				rTime(&tt);
				rString(&User);
				rString(&Computer);
				if (m_FileVersionNumberRead > 49) {
					if (i > 185) {
						int koll = 1;
					}

					if (type == "Validation"&& h_node.RunInfoType==12) {
						if (group== ""&& intValue<15) {
							type = "Run Info";

						}


					}
					History_NewAdd(h_node.RunInfoType, group, type, name, str, intValue, tt, User, Computer);

				}
				else if (intValue < 14 && intValue >= 0) {
					if (i > 185) {
						int koll = 1;
					}
					
					History_Add(group, type, name, str, intValue, tt, User, Computer);
				}
			}
		}
	}
	else {
		string str;
		SimB *pBase;
		iv=m_History_Array.size();
		type="History List";
		wSize_t(&iv);
		wString(&type);
		for(size_t i=0; i<iv; i++) {
			h_node=m_History_Array[i];
			pBase=h_node.pBase;
			str = h_node.strvalue;
			if (h_node.RunInfoType<0 || pBase == nullptr) {
				group="";
				name="";
				str="";

			}
			else if(HIST_INFO(h_node.RunInfoType)==HIST_INFO::DATA_BASE_INFO) {
				// DAtaBase
				group="";				
				name=((CDB*)pBase)->GetName();
				str=History_GetString(i);
				type = "DataBase";

			}
			else if(HIST_INFO(h_node.RunInfoType) == HIST_INFO::SWITCH_INFO) {
				group=pBase->GetGroup();
				name=pBase->GetName();
				str= h_node.strvalue;
				type = "Switches";

			}
			else if(HIST_INFO(h_node.RunInfoType)==HIST_INFO::RUN_INFO) {
				group=((CRunInfo*)pBase)->GetGroup();
				name = ((CRunInfo*)pBase)->GetName();
				str = h_node.strvalue;
				type = "Run Info";

			}
			else if(h_node.RunInfoType==1) {
				if (pBase->GetName() == "Soil Properties"|| pBase->Is_DB()) {
					group = "";
					name = ((Sw*)pBase)->GetName();
				}
				else {
					group = "";
					name = pBase->GetName();
					str = FUtil::STD_FtoAscii(History_GetFloat(i));
				}
			}
			else if(HIST_INFO(h_node.RunInfoType) == HIST_INFO::PARSINGLE_INFO) {
				group=pBase->GetGroup();
				name=pBase->GetName();
				str=FUtil::STD_FtoAscii(History_GetFloat(i));
				type = "Parameters";
			}
			else if (HIST_INFO(h_node.RunInfoType) == HIST_INFO::PARTABLE_INFO) {
				group = pBase->GetGroup();
				name = pBase->GetName();
				str = FUtil::STD_FtoAscii(History_GetFloat(i));
				type = "Table Parameters";
			}
			else if (HIST_INFO(h_node.RunInfoType) == HIST_INFO::MODELFILE_INFO) {
				group = pBase->GetGroup();
				name = pBase->GetName();
				type = "ModelFiles";
			}

			wSize_t(&h_node.RunInfoType);
			wString(&group);
			wString(&type);
			wString(&name);
			wString(&str);

			wSize_t(&h_node.LocalIndex);
			wTime(&h_node.tvalue);
			wString(&h_node.User);
			wString(&h_node.Computer);
		}




	}

return true;
}
bool NewMap::Info_DataBase(bool reading)
{

	string type, str;
	size_t iv;
	time_t time;

	if(reading) {
		//m_P_Array.clear();
		rSize_t(&iv);
		rString(&type);
		if (type == "DataBaseSelections") {
			for (size_t i = 0; i < iv; i++) {
				rString(&str);
				DB_SetChoice(DB_types(i), str);

			}
			if (m_FileVersionNumberRead > 36) {
				for (size_t i = 0; i < iv; i++)
					rTime(&time);
			}
		}
		else
			return false;

	}
	else {
		iv=10;
		type="DataBaseSelections";
		wSize_t(&iv);
		wString(&type);
		for(size_t i=0; i<iv;i++) {
			str=DB_GetChoice(DB_types(i));
			wString(&str);
		}
		for(size_t i=0;i<iv;i++) {
			time=DB_GetImportTimeT(i);
			wTime(&time);
		}

	//	m_DocFileOut.write(&iv ,4);


	}



return true;
}
bool NewMap::Info_Annim(bool reading)
{

	size_t iv;
	iv=0;
	string type, str, group, name;
	size_t ityp, ind, ich;	
	SimB* pBase;
	Annim anode;
	if(reading) {
		//m_DocFileIn>>iv;
		//m_DocFileIn.read( &iv,4);
		rSize_t(&iv);
		rString(&type);
		if(type=="AnnimationSelections") {
			rSize_t(&m_AnnimChartX);
			rSize_t(&m_AnnimChartY);
			if(m_AnnimChartX<1||m_AnnimChartX>2) m_AnnimChartX=2;
			if(m_AnnimChartY<1||m_AnnimChartY>2) m_AnnimChartY=2;
			for(size_t i=0; i<iv;i++) {
				rString(&name);
				rString(&group);
				rSize_t(&ityp);
				rSize_t(&ind);
				rSize_t(&ich);

				pBase=(SimB*)GetPtr(type,group,name);
				if(pBase!=nullptr) {
					anode.pBase=pBase;
					anode.type=ityp;
					anode.LocalIndex=ind;
					anode.iChart=ich;
					AnnimVarList.push_back(anode);
					auto seq=AnnimVarList.size();
					if(pBase->GetType()<<5)
						pBase->SetAnnimateChart(ind,size_t(ich),size_t(seq));
					else 
						pBase->SetAnnimateChart(size_t(ich),size_t(seq));

				}
			}
		}
		else
			return false;
	}
	else {
		type="AnnimationSelections";
		iv=AnnimVarList.size();
		

		wSize_t(&iv);
		wString(&type);
		wSize_t(&m_AnnimChartX);wSize_t(&m_AnnimChartY);
		for(auto ii=AnnimVarList.begin(); ii!=AnnimVarList.end();++ii) {
			ityp=(*ii).type;
			pBase=(*ii).pBase;
			ind=(*ii).LocalIndex;
			ich= (*ii).iChart;
			if(ityp<6) {
				name=((X*)pBase)->GetName();
				group=((X*)pBase)->GetGroup();
			}
			else {
				name=((G*)pBase)->GetName();
				group=((G*)pBase)->GetGroup();
			}
			wString(&name); wString(&group);
			wSize_t(&ityp);wSize_t(&ind);wSize_t(&ich);
		}

	}
return true;
}

vector<SIMB> NewMap::UpdateOutVector(vector<SimB*> vp_in,string GroupSet, bool original, int ViewCategory) {
	OutVector *pVector;
	OutSingle *pSingle;
	vector<SIMB> vect; 
	SIMB out ;
	vector<SimB*> vp;
	vector<SIMB> outcat;
	for (size_t i = 0; i < vp_in.size(); i++) {
		if (ViewCategory == 0 || p_ModelInfo->GetGroupCategory(vp_in[i]->GetGroup()) == ViewCategory)
			vp.push_back(vp_in[i]);
	}

	for( size_t j=0;j<vp.size();j++) {
			    pVector=dynamic_cast<OutVector*>(vp[j]);	
				if(pVector!=nullptr) {
						out.pBase=pVector;
						if(pVector->IsNotOriginalValue()&&pVector->IsEnabled()&&original) {
							for(size_t i=0; i<pVector->GetNumberOfFlags(); i++) {
								out.TabIndex=i;
							  	vect.push_back(out);
							}
						}
						else if((((m_GroupEnabled[pVector->GetGroupNo()]||original)&&pVector->IsEnabled()))) {
							for(size_t i=0; i<pVector->GetNumberOfFlags(); i++) {
								out.TabIndex=i;
								vect.push_back(out);
							}
						}
						else if(pVector->IsEnabled()) {	
							for(size_t i=0; i<pVector->GetNumberOfSelectedFlags(); i++) {
								out.TabIndex=pVector->GetSelectedLocalIndex(i);
								if(pVector->GetStoreFlag(out.TabIndex)>0) 
							  		vect.push_back(out);
							}
						}

				}
				else {
					pSingle=dynamic_cast<OutSingle*>(vp[j]);
					if(pSingle!=nullptr) {
						out.pBase=pSingle;
						out.TabIndex = string::npos;
						if(pSingle->IsNotOriginalValue()&&pSingle->IsEnabled()&&original)
							  	vect.push_back(out);
						else if(((( m_GroupEnabled[pSingle->GetGroupNo()]||original)&&pSingle->IsEnabled()))) {
								vect.push_back(out);
						}
						else if(pSingle->GetStoreFlag()>0&&pSingle->IsEnabled()) {	
							  	vect.push_back(out);
						}


					}
				}
	}
	return vect;
}
void NewMap::RemoveOriginalValues(string typ, string GroupSet, bool original, size_t ViewCategory_to_use) {

		vector<SimB*> vp;
		vector<SimB*> vp_single;

		if (typ=="Parameters") {
			if(m_P_Array.size()>0)			m_P_Array.clear();
			vp=GetPtrVector(PAR_SINGLE, false);
			for(auto it=vp.begin();it!=vp.end();it++) {
					Ps* pPs=dynamic_cast<Ps*>(*it);
					if(original) {
						if((m_GroupEnabled[pPs->GetGroupNo()] ||GroupSet=="ALL")) {
							m_P_Array.push_back(pPs);
						}
						else if (pPs->IsNotOriginalValue()) m_P_Array.push_back(pPs);
					}
					else if(pPs->IsNotOriginalValue()) {
						if((m_GroupEnabled[pPs->GetGroupNo()]||GroupSet=="ALL")) 
							m_P_Array.push_back(pPs);
					}
					else {
						if (m_GroupEnabled[pPs->GetGroupNo()] )
							m_P_Array.push_back(pPs);
					}
			}
		}
		else if (typ=="Tables") {
			if(m_Tab_Array.size()>0)			m_Tab_Array.clear();
			vp=GetPtrVector(TABLE, false);
			for(auto it=vp.begin();it!=vp.end();it++) {
					Tab* pTab=dynamic_cast<Tab*>(*it);
					if(original) {
						if((m_GroupEnabled[pTab->GetGroupNo()] ||GroupSet=="ALL")) {
							if(pTab->CheckEnabled()) m_Tab_Array.push_back(pTab);
						}
						else if (pTab->IsNotOriginalValue()) m_Tab_Array.push_back(pTab);
					}
					else{
						pTab->CheckOriginalValue();
						if(pTab->IsNotOriginalValue()&& pTab->CheckEnabled()) {
							if(m_GroupEnabled[pTab->GetGroupNo()] ||GroupSet=="ALL")
								m_Tab_Array.push_back(pTab);
						}
					}

			}
		}	
		else if (typ == "Parameter Tables") {
			if (m_Pt_Array.size() > 0)			m_Pt_Array.clear();
			vp = GetPtrVector(PAR_TABLE, false);
			for (auto it = vp.begin(); it != vp.end(); it++) {
				P* pP = dynamic_cast<P*>(*it);

				if (original) {
					if ((m_GroupEnabled[pP->GetGroupNo()] || GroupSet == "ALL")) {
						m_Pt_Array.push_back(pP);
					}
					else if (pP->IsNotOriginalValue(0)) m_Pt_Array.push_back(pP);
				}
				else if (pP->IsNotOriginalValue(0)) {
					if ((m_GroupEnabled[pP->GetGroupNo()] || GroupSet == "ALL"))
						m_Pt_Array.push_back(pP);
				}
				else {
					if (m_GroupEnabled[pP->GetGroupNo()])
						m_Pt_Array.push_back(pP);
				}

			}
		}
		else if(typ=="Switches") {
				Sw *pSw;
				if(m_Sw_Array.size()>0)          m_Sw_Array.clear();
				vp=GetPtrVector(SWITCH,"");
				for( size_t j=0;j<vp.size();j++) {
					pSw=dynamic_cast<Sw*>(vp[j]);
					if(pSw->IsNotOriginalValue()&&pSw->IsEnabled()&&original)
						m_Sw_Array.push_back(pSw);
					else if((m_GroupEnabled[pSw->GetGroupNo()]||GroupSet=="ALL")&&original) {
						m_Sw_Array.push_back(pSw);
					}
					else if(pSw->IsNotOriginalValue()&&pSw->IsEnabled())
						m_Sw_Array.push_back(pSw);	
					else if(m_GroupEnabled[pSw->GetGroupNo()]&&pSw->IsEnabled())
						m_Sw_Array.push_back(pSw);	
				}
		}
		
		
		if(typ=="State Variables"||typ==""||typ=="Outputs") {
			if(m_X_Array.size()>0)		  m_X_Array.clear();
			vp=GetPtrVector(STATE,true);
			vp_single=GetPtrVector(STATE_SINGLE,true);
			for(size_t i=0; i<vp_single.size(); i++)
				vp.push_back(vp_single[i]);
			m_X_Array=UpdateOutVector(vp, GroupSet, original, ViewCategory_to_use);	
			m_X_Array=SortSimB_ByGroup(m_X_Array);
		}
		if(typ=="Flow Variables"||typ==""||typ=="Outputs") {
			if(m_T_Array.size()>0)		  m_T_Array.clear();
			vp=GetPtrVector(FLOW, false);
			vp_single=GetPtrVector(FLOW_SINGLE, false);
			for(size_t i=0; i<vp_single.size(); i++)
				vp.push_back(vp_single[i]);
			m_T_Array=UpdateOutVector(vp, GroupSet, original, ViewCategory_to_use);
			m_T_Array=SortSimB_ByGroup(m_T_Array);
		}
		if(typ=="Auxiliary Variables"||typ==""||typ=="Outputs") {
			if(m_G_Array.size()>0)		  m_G_Array.clear();
			vp=GetPtrVector(AUX, false);
			vp_single=GetPtrVector(AUX_SINGLE, false);
			for(size_t i=0; i<vp_single.size(); i++)
				vp.push_back(vp_single[i]);
			m_G_Array=UpdateOutVector(vp, GroupSet, original, ViewCategory_to_use);
			m_G_Array=SortSimB_ByGroup(m_G_Array);
		}
		if(typ=="Driving Variables"||typ==""||typ=="Outputs") {
			if(m_D_Array.size()>0)		  m_D_Array.clear();
			vp=GetPtrVector(DRIVE, false);
			vp_single=GetPtrVector(DRIVE_SINGLE, false);
			for(size_t i=0; i<vp_single.size(); i++)
				vp.push_back(vp_single[i]);
			m_D_Array=UpdateOutVector(vp, GroupSet, original, ViewCategory_to_use);
			m_D_Array=SortSimB_ByGroup(m_D_Array);
		}
		
		

	//Sort_TableList_Arrays();
	//The updating of pointer to document file was successful
}
void NewMap::CleanAllLists()
{
	if(m_P_Array.size()>0)			m_P_Array.clear();
	if(m_Pt_Array.size()>0)          m_Pt_Array.clear();
	if(m_Sw_Array.size()>0)          m_Sw_Array.clear();
	if(m_F_Array.size()>0)           m_F_Array.clear();
	if(m_Tab_Array.size()>0)        m_Tab_Array.clear();
	if(m_X_Array.size()>0)        m_X_Array.clear();
	if(m_T_Array.size()>0)        m_T_Array.clear();
	if(m_G_Array.size()>0)        m_G_Array.clear();
	if(m_D_Array.size()>0)        m_D_Array.clear();
}
void NewMap::UpdateArchiveLists(size_t iType){
	Ps *pP; Sw *pSw; Tab *pTab; F *pF; FCSV *pCSV;
	OutVector *pVector;
	OutSingle *pSingle;
	vector<SimB*> vp;
	SIMB out;
	string typ;
  
	pVector=nullptr;
	// Write Values on Switches parameters outputs

	size_t istart, iabove;
	if(iType<9) {
		istart=iType;
		iabove=iType+1;
		}
	else {
		istart=0;
		iabove=9;
	}

	for(size_t j=istart;j<iabove;j++) {	
	//Choose the current name for the type
		switch (j) {
			case 0:	typ="Parameters"; 
				if(m_P_Array.size()>0)			m_P_Array.clear();
				vp=GetPtrVector(typ,"");
				for(size_t j=0;j<vp.size();j++) {
					pP=dynamic_cast<Ps*>(vp[j]);
					if(pP!=nullptr) 
						if(pP->IsNotOriginalValue()&&pP->GetGroup()!="Hidden")
							m_P_Array.push_back(pP);
				}				
				break;
			case 1:	typ="Switches"; 
				if(m_Sw_Array.size()>0)          m_Sw_Array.clear();
					vp=GetPtrVector(typ,"");
					for( size_t j=0;j<vp.size();j++) {
						pSw=dynamic_cast<Sw*>(vp[j]);
						if(pSw!=nullptr)
							if (pSw->IsNotOriginalValue()&&pSw->GetGroup()!="Hidden")
								m_Sw_Array.push_back(pSw);
					}
				break;
			case 2:	typ="Tables";
					if(m_Tab_Array.size()>0)          m_Tab_Array.clear();	
					vp=GetPtrVector(typ,"");
					for(size_t j=0;j<vp.size();j++) {
						pTab=dynamic_cast<Tab*>(vp[j]);
						if (pTab->IsEnabled())
							m_Tab_Array.push_back(pTab);
					}
				break;
			case 3:	typ="State Variables";
				if(m_X_Array.size()>0)		 m_X_Array.clear();
					vp=GetPtrVector(typ,"");
					for( size_t j=0;j<vp.size();j++) {
						pVector=dynamic_cast<OutVector*>(vp[j]);
						out.pBase=pVector;
						if(pVector!=nullptr)
							if(pVector->IsFlagsSet()&&pVector->IsEnabled())
								m_X_Array.push_back(out);
					}
					for( size_t j=0;j<vp.size();j++) {
						pSingle=dynamic_cast<OutSingle*>(vp[j]);
						out.pBase=pVector;
						if(pVector!=nullptr)
							if(pVector->IsFlagsSet()&&pSingle->IsEnabled())
								m_X_Array.push_back(out);
					}

				break;
			case 4:	typ="Flow Variables";
				if(m_T_Array.size()>0)		 m_T_Array.clear();
					vp=GetPtrVector(typ,"");
					for( size_t j=0;j<vp.size();j++) {
						pVector=dynamic_cast<OutVector*>(vp[j]);
						out.pBase=pVector;
						if(pVector!=nullptr)
							if(pVector->IsFlagsSet()&&pVector->IsEnabled())
								m_T_Array.push_back(out);
					}
					for( size_t j=0;j<vp.size();j++) {
						pSingle=dynamic_cast<OutSingle*>(vp[j]);
						out.pBase=pSingle;
						if(pSingle!=nullptr)
							if(pVector->IsFlagsSet()&&pSingle->IsEnabled())
								m_T_Array.push_back(out);
					}
				
				
				break;
			case 5:	typ="Auxiliary Variables";
		
				if(m_G_Array.size()>0)        m_G_Array.clear();
					vp=GetPtrVector(typ,"");
					for( size_t j=0;j<vp.size();j++) {
						pVector=dynamic_cast<OutVector*>(vp[j]);
						out.pBase=pVector;
						if(pVector!=nullptr)
							if(pVector->IsFlagsSet()&&pVector->IsEnabled())
								m_G_Array.push_back(out);
					}
					for( size_t j=0;j<vp.size();j++) {
						pSingle=dynamic_cast<OutSingle*>(vp[j]);
						out.pBase=pVector;
						if(pVector!=nullptr)
							if(pVector->IsFlagsSet()&&pSingle->IsEnabled())
								m_G_Array.push_back(out);
					}
				
				break;
			case 6:	typ="Driving Variables";
				if(m_D_Array.size()>0)		 m_D_Array.clear();
					vp=GetPtrVector(typ,"");
					for( size_t j=0;j<vp.size();j++) {
						pVector=dynamic_cast<OutVector*>(vp[j]);
						out.pBase=pVector;
						if(pVector!=nullptr)
							if(pVector->IsFlagsSet()&&pVector->IsEnabled())
								m_D_Array.push_back(out);
					}
					for( size_t j=0;j<vp.size();j++) {
						pSingle=dynamic_cast<OutSingle*>(vp[j]);
						out.pBase=pVector;
						if(pVector!=nullptr)
							if(pVector->IsFlagsSet()&&pSingle->IsEnabled())
								m_D_Array.push_back(out);
					}
				break;
			case 7:	typ="Files";
				if(m_F_Array.size()>0)        m_F_Array.clear();
					vp=GetPtrVector(typ,"");
					for(size_t j=0;j<vp.size();j++) {
							pF=dynamic_cast<F*>(vp[j]);
							if(pF->IsEnabled())
								m_F_Array.push_back(pF);
					}
			case 8:	typ = "CSV Files";
				if (m_FCSV_Array.size()>0)        m_FCSV_Array.clear();
				vp = GetPtrVector(typ, "");
				for (size_t j = 0; j<vp.size(); j++) {
					pCSV = dynamic_cast<FCSV*>(vp[j]);
					if (pCSV->IsEnabled())
						m_FCSV_Array.push_back(pCSV);
				}

			}
	}

}
bool NewMap::MakeMapComplete() {
	vector<SimB*> vptr;
	for(size_t i=0;i<14;i++) {
		enum simtype typ { PAR };
		switch (i) {
			case 0:
			typ=SWITCH;
			break;
			case 1:
			typ=PAR_SINGLE;
			break;
			case 2:
			typ=TABLE;
			break;
			case 3:
			typ=PAR_TABLE;
			break;
			case 4:
			typ=STATE;
			break;
			case 5:
			typ=FLOW;break;
			case 6:	typ=AUX;break;
			case 7:	typ=DRIVE; break;
			case 8:	typ=PGFILE; break;
			case 9:	typ=FLOW_SINGLE;break;
			case 10:	typ=AUX_SINGLE;break;
			case 11:	typ=DRIVE_SINGLE; break;
			case 12:	typ=STATE_SINGLE; break;
			case 13:	typ = CSVFILE; break;
			
		}
		vptr=GetPtrVector(typ, true);
		for( size_t j=0; j<vptr.size();j++) {
			vptr[j]->BuildDependencies(this);
			auto name=vptr[j]->GetName();
			p_ModelInfo->trim_xml(name);
		}
	}
	return true;
}
vector<SIMB> NewMap::SortSimB_ByGroup(vector<SIMB> v_in) {
	vector<SIMB> v_out;
	multimap<string, SIMB> SortMap;
	for(size_t i=0; i<v_in.size(); i++) {	
		SortMap.insert(pair<string, SIMB>(v_in[i].pBase->GetGroup(),v_in[i]));
	}

	for(auto it=SortMap.begin(); it!=SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
	return v_out;

}
vector<SIMB> NewMap::SortSimB_ByCategory(vector<SIMB> v_in) {

	vector<SIMB> v_out;
	multimap<size_t, SIMB> SortMap;
	for(size_t i=0; i<v_in.size(); i++) {	
		SortMap.insert(pair<size_t, SIMB>(p_ModelInfo->GetGroupCategory(v_in[i].pBase->GetGroup()),v_in[i]));
	}

	for(auto it=SortMap.begin(); it!=SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
	return v_out;
}
void NewMap::AssignNewValue_toPs(Ps* pPs, double newvalue)
{
	double orgvalue = pPs->GetOriginalValue();
	double reltest = (orgvalue - pPs->GetMinValue()) / (pPs->GetMaxValue() - pPs->GetMinValue());
	double relnew = (newvalue - pPs->GetMinValue()) / (pPs->GetMaxValue() - pPs->GetMinValue());
	double current = pPs->GetValue();
	double ratio = (newvalue + 1.E-10) / (current + 1.E-10);
	if (abs(reltest - relnew) > 1.E-5 || ratio > 1.01 || ratio < 0.99) {
		size_t recalc;
		if (newvalue > pPs->GetMaxValue()) {
			recalc = pPs->SetValue(pPs->GetMaxValue());
		}
		else if (newvalue < pPs->GetMinValue()) {
			recalc = pPs->SetValue(pPs->GetMinValue());
		}
		else
			recalc = pPs->SetValue(newvalue);
		if (recalc > 0) {
			PAR_OPTIONS p;
			p.pSimB = pPs;
			p.options = recalc;
			m_OptionsForReading.push_back(p);
		}
		pPs->SetNotOldValue();
		m_P_Array.push_back(pPs);
	}
}
bool NewMap::AssignNewValue_toP(P* pP, size_t nRow, double newvalue, size_t &ReCalc)
{
	double orgvalue, currentvalue;
	orgvalue = pP->GetOriginalValue(nRow);
	currentvalue = pP->GetValue(nRow);

	double range = pP->GetMaxValue(nRow) - pP->GetMinValue(nRow);
	if (range == 0) {
		if (nRow > 0) {
			double newrange = pP->GetMaxValue(nRow - 1) - pP->GetMinValue(nRow - 1);
			if (newrange > 0) {
				pP->SetMaxValue(nRow, pP->GetMaxValue(nRow - 1));
				pP->SetMinValue(nRow, pP->GetMinValue(nRow - 1));
			}
			range = newrange;
		}
	}
	if (range > 0) {
		double relcurrent = (pP->GetMaxValue(nRow) - currentvalue) / (pP->GetMaxValue(nRow) - pP->GetMinValue(nRow));
		double newrel = (pP->GetMaxValue(nRow) - newvalue) / (pP->GetMaxValue(nRow) - pP->GetMinValue(nRow));
		double ratio = (newvalue + 1.E-10) / (currentvalue + 1.E-10);
		if (abs(newrel - relcurrent) > 1.E-6 || ratio < 0.99 || ratio>1.01) {
			ReCalc = pP->SetValue(nRow, newvalue);
			pP->SetNotOldValue(size_t(nRow));
			pP->SetNotOriginalValue();


			return true;
		}
	}
	return false;

}
bool NewMap::CreateMapSvFlexFile(size_t index) {

	VALSUMLINK valsumlink;
	valsumlink = m_ValsumFlexFiles[index];
	SoilStorage soilstorage;
	ifstream sumfile;
	sumfile.open(valsumlink.FileName, ifstream::in);
	string line;
	line.resize(2000);
	sumfile.getline(&line[0], 2000);
	size_t n=sumfile.gcount();
	line.resize(n);
	size_t ipos;
	vector<string> header;
	ipos = line.find(';'); if (ipos == string::npos) ipos = line.find(',');
	size_t count = 0;


	while (ipos != string::npos) {
		string sub;
		
		sub = line.substr(0, ipos);
		MapFlexId[index].insert(pair<string, size_t>(sub, count));
		count++;
		line = line.substr(ipos + 1);
		ipos = line.find(';');	if (ipos == string::npos) ipos = line.find(',');
		if (line.size() > 0 && ipos == string::npos) {
			size_t nn = line.size();
			sub = line.substr(0, nn-1);
			nn = sub.size();
			MapFlexId[index].insert(pair<string, size_t>(sub, count));
			count++;
		}
	}




	line.resize(2000);
	while (sumfile.good()) {
		sumfile.getline(&line[0], 2000);
		auto k=sumfile.gcount();
		line.resize(k);
		vector<float> values;

		ipos = line.find(';'); if (ipos == string::npos) ipos = line.find(',');
		string sub;
		sub = line.substr(0, ipos);
		size_t value = FUtil::AtoInt(sub);
		float fvalue = FUtil::AtoFloat(sub);
		values.push_back(fvalue);

		line = line.substr(ipos + 1);
		ipos = line.find(';');	if (ipos == string::npos) ipos = line.find(',');
		while (ipos != string::npos) {
			string sub;
			sub = line.substr(0, ipos);
			float value = FUtil::AtoFloat(sub);
			values.push_back(value);
			if (line[0] =='\n'&& ipos == 0) {
				values.push_back(MISSING);
				ipos = string::npos;
			}
			else {
				line = line.substr(ipos + 1);
		
				ipos = line.find(';');	if (ipos == string::npos) ipos = line.find(',');
				if (ipos == string::npos) {
					float value = FUtil::AtoFloat(line);
					values.push_back(value);
				}
			}
		}
		if (line.size() > 0 && ipos == string::npos) {
			float value = FUtil::AtoFloat(line);
			values.push_back(value);
		}

		m_ValFlexValue[index].push_back(values);
		//FlexVal *Flex;
		//Flex = new FlexVal;
		//Flex->val = values;
		MapFlexVal[index].insert(pair<size_t, size_t>(value, m_ValFlexValue[index].size() - 1));
		size_t koll = line.size();
		line.clear();
		line.resize(2000);

	}
	//soilstorage.ReadHeaderFile();



	return true;
}

void NewMap::Export_MBinToCSV(bool AcceptedOnly)
{
	ofstream kalle;
	ofstream outfile[24];

	string strbase;
	bool fileexist = false;
	if (!MR_Storage_IsOpen()) fileexist = MR_Storage_Open();
	else fileexist = true;
	if (!fileexist) return;
	auto ipos=m_DocFile2.m_Multi_MBin_File.find(".MBin");
	if (ipos != string::npos) {
		strbase = m_DocFile2.m_Multi_MBin_File.substr(0, ipos);
		strbase += "_";
	}
	else
		return;

	auto filename = strbase + "_SumV.csv";


	vector<SIMB> out;
	string line, line2;
	string all_accept;
	if (AcceptedOnly)
		all_accept = "Accepted_";
	else
		all_accept = "All_";


	auto NumLines = GetSize_XTGD();
	for(size_t i=0; i<4; i++) {
		switch (i) {
		case 0:
			out = m_X_Array;
			filename = strbase + "X_";
			break;
		case 1:
			filename = strbase + "T_";
			out = m_T_Array;
			break;
		case 2:
			filename = strbase + "G_";
			out = m_G_Array;
			break;
		case 3:
			filename = strbase + "D_";
			out = m_D_Array;
			break;
		}
		filename += all_accept;
		outfile[i*6+0].open(filename+"Ini.csv");
		outfile[i * 6 + 1].open(filename + "Final.csv");
		outfile[i * 6 + 2].open(filename + "Min.csv");
		outfile[i * 6 + 3].open(filename + "Max.csv");
		outfile[i * 6 + 4].open(filename + "Mean.csv");
		outfile[i * 6 + 5].open(filename + "Acc.csv");
		string nameoutput, headerline;
		vector<int> sumindexvector;
		
		for (size_t j = 0; j < out.size(); j++) {
			int selected_Single;
			bool selected_Multi;
			auto pSw = out[j].pBase;
			auto ii = out[j].TabIndex;
			OutSingle* pSingle = nullptr;
			OutVector* pVector = nullptr;
			pVector = dynamic_cast<OutVector*>(pSw);
			size_t num_flags = 1;
			int lindex;
			if (pVector != nullptr) {
				num_flags = pVector->GetNumberOfSelectedFlags();
				pSingle = nullptr;
			}
			else {
				num_flags = 1;
				pSingle = dynamic_cast<OutSingle*>(pSw);
			}

			if (pSw->Is_Vector()) {
				selected_Single = pVector->GetStoreFlag(ii);
				selected_Multi = false;
			}
			else
			{
				if (pSingle == nullptr) continue;
				selected_Single = pSingle->GetStoreFlag();
				selected_Multi = false;
			}
			nameoutput = pSw->GetName().c_str();
			size_t sumindex;
			auto namevar = pSw->GetName();
			FUtil::trim_space(namevar);
			if (pSw->Is_Vector()) {
				lindex = ((OutVector*)pSw)->GetLocalIndex(ii);
				nameoutput += "(" + FUtil::STD_ItoAscii(ii + 1) + ")";
				sumindex = ((OutVector*)pSw)->GetSumVarIndex(lindex, 7);
				namevar += "_" + FUtil::STD_ItoAscii(lindex+1);
			}
			else {
				sumindex = ((OutSingle*)pSw)->GetSumVarIndex(7);
			}
			
			if (j == 0) headerline = "RunNo;"+ namevar + ";";
			else if (j < out.size() - 1) headerline += namevar + ";";
			else headerline += namevar+"\n";
			sumindexvector.push_back(sumindex);
		}


		for (int ivartyp = 0; ivartyp < 6; ivartyp++) {
				outfile[i * 6 + ivartyp].write((char*)&headerline[0], headerline.size());
				auto numout = m_MStorage.GetNumAccepted();
				if (AcceptedOnly) {
					numout = m_MStorage.GetNumberOfRuns();
					numout = m_pSimDoc->m_MStorage.m_AcceptedRuns[m_MStorage.m_ActualEnsemble].size();

					numout = m_MStorage.GetNumAccepted();
				}


				for (int nomulrun = 0; nomulrun < numout; nomulrun++) {
					int irun;
					if (AcceptedOnly)
						irun = m_pSimDoc->m_MStorage.Get_AcceptedRuns(nomulrun);
					else
						irun = nomulrun;

					line = FUtil::STD_ItoAscii(irun + 1);
					for (size_t var = 0; var < sumindexvector.size(); var++) {
						float value = m_MStorage.GetSumV(irun, sumindexvector[var] + ivartyp-6);
						line += ";" + FUtil::STD_FtoAscii(value);
					}
					line += "\n";
					outfile[i * 6 + ivartyp].write((char*)&line[0], line.size());					
				}
				outfile[i * 6 + ivartyp].close();
			}

	}
	filename = strbase + "V_" + all_accept;
	outfile[0].open(filename + "_R2.csv");
	outfile[1].open(filename + "_A0.csv");
	outfile[2].open(filename + "_A1.csv");
	outfile[4].open(filename + "_RMSE.csv");
	outfile[3].open(filename + "_ME.csv");
	outfile[5].open(filename + "_LogLi.csv");
	outfile[6].open(filename + "_NME.csv");

	for (size_t i = 0; i < 7; i++) {
		string nameoutput, headerline;
		vector<int> sumindexvector;

		headerline = "RunNo";
		for (size_t j = 0; j < m_Val_Array.size(); j++) {
			auto test = m_pSimDoc->m_ValidationData.GetValName(j);
			FUtil::trim_space(test);
			int iv = m_pSimDoc->m_ValidationData.GetValLocalIndex(j);
			if (iv > -1) test += "_" + FUtil::STD_ItoAscii(iv + 1);
			headerline += ";" + test;
		}
		headerline += "\n";

		outfile[i].write((char*)&headerline[0], headerline.size());
		auto numout = m_MStorage.GetNumAccepted();
		if (AcceptedOnly) {
			numout = m_MStorage.GetNumberOfRuns();
			numout = m_pSimDoc->m_MStorage.m_AcceptedRuns[m_MStorage.m_ActualEnsemble].size();
			numout = m_MStorage.GetNumAccepted();
		}

		for (int nomulrun = 0; nomulrun < numout; nomulrun++) {
			int irun;
			if (AcceptedOnly)
				irun = m_pSimDoc->m_MStorage.Get_AcceptedRuns(nomulrun);
			else
				irun = nomulrun;

			line = FUtil::STD_ItoAscii(irun + 1);
			for (size_t var = 0; var < m_Val_Array.size(); var++) {
				if (i == 6) {
					int ind_rmse;
					float rmse, rmse_m, re_2;
					ind_rmse = (i-2)*m_Val_Array.size()+var;
					rmse = m_pSimDoc->m_MStorage.GetValid(irun, ind_rmse);
					rmse_m = m_pSimDoc->m_ValidationData.GetValRMSE_O(var);
					if (rmse > MISSING) {
						if (rmse_m > 0) {
							re_2 = 1 - (rmse * rmse * m_pSimDoc->m_ValidationData.GetValn(i)) / (rmse_m);						
						}
						else
							re_2 = float(MISSING);
					}
					else
						re_2 = MISSING;
					line += ";" + FUtil::STD_FtoAscii(re_2);
				}
				else {
					float value = m_MStorage.GetValid(irun, var + i * m_Val_Array.size());
					line += ";" + FUtil::STD_FtoAscii(value);
				}
			}
			line += "\n";
			outfile[i].write((char*)&line[0], line.size());
		}
		outfile[i].close();
	}

	filename = strbase + "P_" + all_accept;
	outfile[0].open(filename + "_Par.csv");
	outfile[1].open(filename + "_SumPerformance.csv");

	 {
		string nameoutput, headerline;
		vector<int> sumindexvector;

		headerline = "RunNo";

		for (int i = 0; i < m_pSimDoc->m_MultiRun_Array.size(); i++) {
			for (int ii = 0; ii < m_pSimDoc->MR_Get_NumberOfParametersWithinDim(i + 1); ii++) {
				SimB* pPar;
				int index;
				float fmin, fmin1, fact;
				index = MR_GetTabIndex(i + 1, ii + 1);
				pPar = MR_Get_pPar(i + 1, ii + 1);
				auto test = pPar->GetName();
				FUtil::trim_space(test);
				if (index > -1) test += "_" + FUtil::STD_ItoAscii(i + 1);
				headerline += ";" + test;
			}
		}
		headerline += "\n";
		outfile[0].write((char*)&headerline[0], headerline.size());
		headerline = "RunNo;Accepted;LogLi;SumLogLi;TotLogLi;MapRunNo;AccTime;n;SSE;AIC;BIC";
		headerline += "\n";
		outfile[1].write((char*)&headerline[0], headerline.size());

		auto numout = m_MStorage.GetNumAccepted();
		if (AcceptedOnly) {
			numout = m_MStorage.GetNumberOfRuns();
			numout = m_pSimDoc->m_MStorage.m_AcceptedRuns[m_MStorage.m_ActualEnsemble].size();
			numout = m_MStorage.GetNumAccepted();
		}

		for (int nomulrun = 0; nomulrun < numout; nomulrun++) {
			int irun;
			if (AcceptedOnly)
				irun = m_pSimDoc->m_MStorage.Get_AcceptedRuns(nomulrun);
			else
				irun = nomulrun;
			line = FUtil::STD_ItoAscii(irun + 1);
			line2 = FUtil::STD_ItoAscii(irun + 1);
			for (int var = 0; var < m_pSimDoc->m_MStorage.m_NumMulti; var++) {
				float value = m_MStorage.GetMultiP(irun, var);
				if(var< m_pSimDoc->m_MStorage.m_NumMulti-10)
					line += ";" + FUtil::STD_FtoAscii(value);
				else
					line2 += ";" + FUtil::STD_FtoAscii(value);
			}
			line += "\n"; line2 += "\n";
			outfile[0].write((char*)&line[0], line.size());
			outfile[1].write((char*)&line2[0], line2.size());

		}
		outfile[0].close();
		outfile[1].close();




	}


	
};
bool NewMap::ResetDocument_to_DefaultValues() {

	auto SimBv=GetPtrVector(SWITCH);
	for (auto pSimB : SimBv) {
		auto pSw = static_cast<Sw*>(pSimB);
		pSw->SetIntValue(pSw->GetOriginalIntValue());
	}

	SimBv = GetPtrVector(PAR_SINGLE);
	for (auto pSimB : SimBv) {
		auto pP = static_cast<Ps*>(pSimB);
		pP->SetValue(pP->GetOriginalValue());
	}

	SimBv = GetPtrVector(PAR_TABLE);
	for (auto pSimB : SimBv) {
		auto pP = static_cast<P*>(pSimB);
		auto n=pP->GetSize();
		auto pNE = pP->GetNEPointer();
		for (size_t i = 0; i < n; i++) {
			pP->SetValue(i, pP->GetOriginalValue(i));
		}
		if (pP->GetSize() != pNE->GetOriginalNEValue()) pP->SetSize(pNE->GetOriginalNEValue());
		pNE->SetNEValue(pNE->GetOriginalNEValue());
	}
	for (size_t i = 0; i < 4; i++) {
		SimBv = GetPtrVector(simtype(STATE+i));
		for (auto pSimB : SimBv) {
			auto pOut = static_cast<OutVector*>(pSimB);
			auto n=pOut->GetNumberOfFlags();
			auto pNE = pOut->GetNEPointer();
			for (size_t i = 0; i < n; i++)
				pOut->SetValidFlagIndex(i, 0);
			pNE->SetNEValue(pNE->GetOriginalNEValue());
		}
	}
	for (size_t i = 0; i < 4; i++) {
		SimBv = GetPtrVector(simtype(STATE_SINGLE + i));
		for (auto pSimB : SimBv) {
			auto pOut = static_cast<OutSingle*>(pSimB);
			pOut->SetValidFlagIndex(0);
		}
	}

	SimBv = GetPtrVector(simtype(PGFILE));
	for (auto pSimB : SimBv) {
		auto pF = static_cast<F*>(pSimB);
		pF->ResetValNumbers();
		pF->SetOriginalValue();
	}
	return true;
}

string NewMap::Export_XLSX(bool AcceptedOnly, size_t choice, int IndexOut, string varname)
{
#ifndef COUPSTD
	string strbase;
	bool fileexist = false;
	if (!MR_Storage_IsOpen()) fileexist = MR_Storage_Open();
	else fileexist = true;
	if (!fileexist) return "";
	auto ipos = m_DocFile2.m_Multi_MBin_File.find(".MBin");
	if (ipos != string::npos) {
		strbase = m_DocFile2.m_Multi_MBin_File.substr(0, ipos);
	}
	else
		return "";

	vector<string> header;
	vector< vector<float>>	allvalues;
	string sheetname;
	xlnt::workbook wb;	
	wb.remove_sheet(wb.active_sheet());


	auto GetFileName = [&strbase, &AcceptedOnly, &choice]() {
		string fileout;
		string indicator;
		if (AcceptedOnly) indicator = "_Accepted";
		else indicator = "_All";

		if (choice == 0)
			fileout = strbase + indicator+ ".xlsx";
		else if (choice == 1)
			fileout = strbase + indicator+ "_Performance.xlsx";
		else if (choice == 2)
			fileout = strbase + indicator+ "_Outputs.xlsx";
		else
			fileout = strbase + indicator + "_Par.xlsx";
		return fileout;


	};

	auto filein = GetFileName();
	bool AddToExisting=false;
	if (FUtil::IsFileExisting(filein)) {
		wb.load(filein);
		AddToExisting = true;
	}


	auto FillSheet = [&wb,&AddToExisting](vector<string> header, vector< vector <float>> values, string sheetname) {
		xlnt::worksheet ws;
		int startcol = 1;

		if (!AddToExisting) {
			ws = wb.create_sheet();
			ws.title(sheetname);
		}
		else {
			ws = wb.active_sheet();
			int rows = 0; int cols = 0;
			for (auto row : ws.rows(false))
			{
				rows++;
				cols = 0;
				for (auto cell : row)
				{
					cols++;

				}
			}
			startcol = cols+1;
		}

		for (size_t col = 0; col < header.size(); col++) {
			ws.cell(col +startcol, 1).value(header[col]);
		}
		for (size_t row = 0; row < values.size(); row++) {
			for (size_t col = 0; col < values[row].size(); col++) {
				ws.cell(xlnt::cell_reference(col + startcol, 2 + row)).value(values.at(row).at(col));
			}
		}
	};
	if (choice == 0 || choice == 1) {
		string outname = "_Performance";
		if (AcceptedOnly) outname = "_Accepted_Performance";
		for (size_t i = 0; i < 7; i++) {
			header.clear();
			while (allvalues.size() > 0) {
				allvalues[allvalues.size() - 1].clear();
				allvalues.erase(allvalues.begin() + allvalues.size() - 1);
			}
			header.push_back("RunNo");
			for (size_t j = 0; j < m_Val_Array.size(); j++) {
				auto test = m_pSimDoc->m_ValidationData.GetValName(j);
				FUtil::trim_space(test);
				int iv = m_pSimDoc->m_ValidationData.GetValLocalIndex(j);
				if (iv > -1) test += "_" + FUtil::STD_ItoAscii(iv + 1);
				header.push_back(test);
			}

			size_t numout;
			if (AcceptedOnly) {
				numout = m_pSimDoc->m_MStorage.m_AcceptedRuns[m_MStorage.m_ActualEnsemble].size();
				numout = m_MStorage.GetNumAccepted();
			}
			else
				numout = m_MStorage.GetNumberOfTotalRunsWithAttachedFiles(); 

			allvalues.resize(numout);

			for (int nomulrun = 0; nomulrun < numout; nomulrun++) {
				allvalues[nomulrun].resize(m_Val_Array.size() + 1);
				int irun;
				if (AcceptedOnly)
					irun = m_pSimDoc->m_MStorage.Get_AcceptedRuns(nomulrun);
				else
					irun = nomulrun;

				allvalues[nomulrun][0] = float(irun + 1);
				for (size_t var = 0; var < m_Val_Array.size(); var++) {
					if (i == 6) {
						int ind_rmse;
						float rmse, rmse_m, re_2;
						ind_rmse = (i - 2) * m_Val_Array.size() + var;
						rmse = m_pSimDoc->m_MStorage.GetValid(irun, ind_rmse);
						rmse_m = m_pSimDoc->m_ValidationData.GetValRMSE_O(var);
						if (rmse > MISSING) {
							if (rmse_m > 0) {
								re_2 = 1 - (rmse * rmse * m_pSimDoc->m_ValidationData.GetValn(i)) / (rmse_m);
							}
							else
								re_2 = float(MISSING);
						}
						else
							re_2 = MISSING;
						allvalues[nomulrun][var + 1] = re_2;
					}
					else {
						float value = m_MStorage.GetValid(irun, var + i * m_Val_Array.size());
						allvalues[nomulrun][var + 1] = value;
					}
				}
			}

			switch (i) {
			case 0:
				sheetname = "R2" + outname;
				break;
			case 1:
				sheetname = "A0" + outname;
				break;
			case 2:
				sheetname = "A1" + outname;
				break;
			case 4:
				sheetname = "RMSE" + outname;
				break;
			case 3:
				sheetname = "ME" + outname;
				break;
			case 5:
				sheetname = "LogLi" + outname;
				break;
			case 6:
				sheetname = "NME" + outname;
				break;
			}

			FillSheet(header, allvalues, sheetname);

		}
	}


	vector<string> sheetnames;
	
	if (choice == 0 || choice == 2) {
		if (IndexOut >= 0) { // Single Variable Outputs
			auto numout = m_MStorage.GetNumberOfTotalRunsWithAttachedFiles();
			if (AcceptedOnly) {
				numout = m_MStorage.GetNumAccepted();
			}
			allvalues.resize(numout);
			header.clear();

			if (AddToExisting)
				header.push_back(varname);
			else {
				header.push_back("RunNo");
				header.push_back(varname);
			}

			for (int nomulrun = 0; nomulrun < numout; nomulrun++) {
				if(AddToExisting)
					allvalues[nomulrun].resize(1);
				else
					allvalues[nomulrun].resize(2);
				int irun;
				if (AcceptedOnly)
					irun = m_pSimDoc->m_MStorage.Get_AcceptedRuns(nomulrun)+1;
				else
					irun = nomulrun + 1;

				float value = m_MStorage.GetSumV(irun - 1, IndexOut);
				if (AddToExisting) {
					allvalues[nomulrun][0] = value;				
				}
				else {
					allvalues[nomulrun][0] = float(irun);				
					allvalues[nomulrun][1] = value;
				}

			}			
			FillSheet(header, allvalues, "SumVariables");
		}
		else { // All Outputs
			vector<SIMB> out;
			auto NumLines = GetSize_XTGD();
			for (size_t i = 0; i < 4; i++) {
				sheetnames.resize(6);
				string filename;
				switch (i) {
				case 0:
					out = m_X_Array;
					filename = "X_";
					break;
				case 1:
					filename = "T_";
					out = m_T_Array;
					break;
				case 2:
					filename = "G_";
					out = m_G_Array;
					break;
				case 3:
					filename = "D_";
					out = m_D_Array;
					break;
				}

				sheetnames[0] = "Ini_" + filename;
				sheetnames[1] = "Final_" + filename;
				sheetnames[2] = "Min_" + filename;
				sheetnames[3] = "Max_" + filename;
				sheetnames[4] = "Mean_" + filename;
				sheetnames[5] = "Acc_" + filename;

				string nameoutput, headerline;
				vector<int> sumindexvector;

				for (size_t j = 0; j < out.size(); j++) {
					int selected_Single;
					bool selected_Multi;
					auto pSw = out[j].pBase;
					auto ii = out[j].TabIndex;
					OutSingle* pSingle = nullptr;
					OutVector* pVector = nullptr;
					pVector = dynamic_cast<OutVector*>(pSw);
					size_t num_flags = 1;
					int lindex;
					if (pVector != nullptr) {
						num_flags = pVector->GetNumberOfSelectedFlags();
						pSingle = nullptr;
					}
					else {
						num_flags = 1;
						pSingle = dynamic_cast<OutSingle*>(pSw);
					}

					if (pSw->Is_Vector()) {
						selected_Single = pVector->GetStoreFlag(ii);
						selected_Multi = false;
					}
					else
					{
						if (pSingle == nullptr) continue;
						selected_Single = pSingle->GetStoreFlag();
						selected_Multi = false;
					}
					nameoutput = pSw->GetName().c_str();
					size_t sumindex;
					auto namevar = pSw->GetName();
					FUtil::trim_space(namevar);
					if (pSw->Is_Vector()) {
						lindex = ((OutVector*)pSw)->GetLocalIndex(ii);
						nameoutput += "(" + FUtil::STD_ItoAscii(ii + 1) + ")";
						sumindex = ((OutVector*)pSw)->GetSumVarIndex(lindex, 7);
						namevar += "_" + FUtil::STD_ItoAscii(lindex + 1);
					}
					else {
						sumindex = ((OutSingle*)pSw)->GetSumVarIndex(7);
					}

					if (j == 0) {
						header.clear();
						header.push_back("RunNo");
						header.push_back(namevar);
					}
					else if (j < out.size() - 1) header.push_back(namevar);
					else header.push_back(namevar);
					sumindexvector.push_back(sumindex);
				}


				for (int ivartyp = 0; ivartyp < 6; ivartyp++) {
					auto numout = m_MStorage.GetNumberOfTotalRunsWithAttachedFiles();
					if (AcceptedOnly) {
						numout = m_MStorage.GetNumberOfRuns();
						numout = m_pSimDoc->m_MStorage.m_AcceptedRuns[m_MStorage.m_ActualEnsemble].size();

						numout = m_MStorage.GetNumAccepted();
					}
					allvalues.resize(numout);

					for (int nomulrun = 0; nomulrun < numout; nomulrun++) {
						allvalues[nomulrun].resize(sumindexvector.size() + 1);
						int irun;
						if (AcceptedOnly)
							irun = m_pSimDoc->m_MStorage.Get_AcceptedRuns(nomulrun);
						else
							irun = nomulrun + 1;
						allvalues[nomulrun][0] = float(irun + 1);


						for (size_t var = 0; var < sumindexvector.size(); var++) {
							float value = m_MStorage.GetSumV(irun, sumindexvector[var] + ivartyp - 6);
							allvalues[nomulrun][var + 1] = value;
						}
					}
					FillSheet(header, allvalues, sheetnames[ivartyp]);

				}


			}

		}

	}

	if (choice == 0 || choice == 3) {
		header.clear();
		header.push_back("RunNo");

		for (int i = 0; i < m_pSimDoc->m_MultiRun_Array.size(); i++) {
			for (int ii = 0; ii < m_pSimDoc->MR_Get_NumberOfParametersWithinDim(i + 1); ii++) {
				SimB* pPar;
				int index;
				float fmin, fmin1, fact;
				index = MR_GetTabIndex(i + 1, ii + 1);
				pPar = MR_Get_pPar(i + 1, ii + 1);
				auto test = pPar->GetName();
				FUtil::trim_space(test);
				if (index > -1) test += "_" + FUtil::STD_ItoAscii(i + 1);
				header.push_back(test);
			}
		}

		vector<string> header2 = { "RunNo","Accepted","LogLi","SumLogLi","TotLogLi","MapRunNo","AccTime","n","SSE","AIC","BIC" };
		while (allvalues.size() > 0) {
			allvalues[allvalues.size() - 1].clear();
			allvalues.erase(allvalues.begin() + allvalues.size() - 1);
		}

		auto numout = m_MStorage.GetNumberOfTotalRunsWithAttachedFiles();;
		if (AcceptedOnly) {
			numout = m_pSimDoc->m_MStorage.m_AcceptedRuns[m_MStorage.m_ActualEnsemble].size();
			numout = m_MStorage.GetNumAccepted();
		}
		allvalues.resize(numout);
		vector< vector<float>> allvalues2;
		allvalues2.resize(numout);

		for (int nomulrun = 0; nomulrun < numout; nomulrun++) {
			allvalues[nomulrun].resize(m_pSimDoc->m_MStorage.m_NumMulti - 10 + 1);
			allvalues2[nomulrun].resize(11);
		
			int irun;
			if (AcceptedOnly) {
				irun = m_pSimDoc->m_MStorage.Get_AcceptedRuns(nomulrun);
				allvalues[nomulrun][0] = float(irun);
				allvalues2[nomulrun][0] = float(irun);
			}
			else {
				irun = nomulrun;
				allvalues[nomulrun][0] = float(irun+1);
				allvalues2[nomulrun][0] = float(irun+1);
			}


			for (int var = 0; var < m_pSimDoc->m_MStorage.m_NumMulti; var++) {
				float value = m_MStorage.GetMultiP(irun, var);
				if (var < m_pSimDoc->m_MStorage.m_NumMulti - 10)
					allvalues[nomulrun][var + 1] = value;
				else
					allvalues2[nomulrun][var - m_pSimDoc->m_MStorage.m_NumMulti + 10 + 1] = value;
			}
		}
		FillSheet(header, allvalues, "ParValues");
		FillSheet(header2, allvalues2, "PerformanceSummary");
	}


	auto fileout=GetFileName();

	wb.save(fileout);
	return fileout;
#else
return "";
#endif
	};
string NewMap::Sim2XLSX(bool reading, bool AllItems) {

#ifdef COUPSTD

	return "";
#else
	string sheetname;
	xlnt::workbook wb;

	string filein;
	auto GetFileName = [&](string DocName) {
		string fileout;
		auto ip = DocName.find(".Sim");
		if (ip != string::npos) {
			fileout = DocName.substr(0,ip);
			if (AllItems) fileout += "_Extended";
			fileout += ".xlsx";
		}
		return fileout;
	};

	 
	if (reading) {

		FILEDLG FileDlg = MFC_Util::GetFileDialog("*.xlsx", "", "List of Files (*.xlsx)|*.xlsx||");

		if (FileDlg.opengood != IDOK) {
			return "";
		}
		filein=FileDlg.filename;

	}
	else {
		filein = GetFileName(m_DocFileName);


	}


	bool AddToExisting = false;
	if (FUtil::IsFileExisting(filein)) {
		wb.load(filein);
		AddToExisting = true;
	}
	auto FillRunInfo = [&](bool reading) { 
		if (reading) {
			xlnt::worksheet ws;

			if (AddToExisting) {
				ws = wb.sheet_by_title("Run Info");

				auto str = ws.cell(2, 6).to_string();
				auto str3 = ws.cell(2, 6).value<string>();
				auto tid = PGUtil::Ato_IPGTID(str);
				RunOpt.longMinStart = PGUtil::MinutConv(tid);
				str = ws.cell(4, 6).to_string();
				tid = PGUtil::Ato_IPGTID(str);
				RunOpt.longMinEnd = PGUtil::MinutConv(tid);


				str = ws.cell(2, 7).to_string();
				tid = PGUtil::Ato_IPGTID(str);
				auto preminstart = PGUtil::MinutConv(tid);
				str = ws.cell(4, 7).to_string();
				tid = PGUtil::Ato_IPGTID(str);
				RunOpt.longMinPostEnd = PGUtil::MinutConv(tid);
			}
	
		}
		else {
			xlnt::worksheet ws;
			if (!AddToExisting) {
				wb.core_property(xlnt::core_property::modified, xlnt::datetime(2021, 1, 15,10,3));
				wb.remove_sheet(wb.active_sheet());
				ws = wb.create_sheet();
				ws.row_properties(1).height = 4.;
				ws.column_properties("A").width = 25.;
				ws.column_properties("B").width = 25.;
				ws.column_properties("C").width = 25.;
				ws.column_properties("D").width = 25.;
				ws.title("Run Info");
				ws.merge_cells("A1:D1");
				ws.cell(1, 1).value("General Simulation Information");
				ws.cell(1, 1).hyperlink("http://www.coupmodel.com/default.htm");
				auto grey=xlnt::fill::solid(xlnt::rgb_color(230, 230, 230));
				auto yellow = xlnt::fill::solid(xlnt::color::yellow());
				ws.cell(1, 1).fill(grey);
				ws.cell(1, 2).value("Created");
				ws.cell(2, 2).value(GetTimeCreated());

				ws.cell(3, 2).value("Modified");
				ws.cell(4, 2).value(GetTimeModified());

				ws.cell(1, 3).value("Simulation Number");
				ws.cell(2, 3).value(m_DocFile.m_SimulationRunNo);
				ws.cell(3, 3).value("No of MultiRuns");
				ws.cell(4, 3).value(MR_Get_TotalNumberofRuns());

				ws.cell(1, 4).value("Version Number");
				ws.cell(2, 4).value(m_DocFile2.m_ExeFileDate);

				ws.cell(1, 5).value("Simulation Period");
				ws.cell(1, 5).fill(grey);
				ws.merge_cells("A5:D5");

				
				ws.cell(1, 6).value("StartDate");
				ws.cell(2, 6).value(PGUtil::StringDatum(RunOpt.longMinStart)); ws.cell(2, 6).fill(yellow);
				ws.cell(3, 6).value("EndDate");
				ws.cell(4, 6).value(PGUtil::StringDatum(RunOpt.longMinEnd));  ws.cell(4, 6).fill(yellow);
				ws.cell(1, 7).value("PrePeriodDate");
				ws.cell(2, 7).value(PGUtil::StringDatum(RunOpt.longMinPreStart));  ws.cell(2, 7).fill(yellow);
				ws.cell(3, 7).value("PostPeriodDate");
				ws.cell(4, 7).value(PGUtil::StringDatum(RunOpt.longMinPostEnd));   ws.cell(4, 7).fill(yellow);
				ws.cell(1, 8).value("Scaling of Period");
				ws.cell(2, 8).value(m_DocFile.m_TimePeriodScaleFactor);	  ws.cell(2, 8).fill(yellow);
				ws.merge_cells("C8:D8");
				ws.cell(1, 9).value("Additional Settings");	ws.cell(1, 9).fill(grey);
				ws.merge_cells("A9:D9");

				ws.cell(1, 10).value("Input Time Resolution");
				auto pSw = static_cast<Sw*>(GetPtr(SWITCH, "TimeResolution"));
				ws.cell(2,10).value(pSw->GetOption(pSw->GetIntValue()));  ws.cell(2, 10).fill(yellow);
				ws.merge_cells("C10:D10");
				ws.cell(1, 11).value("Output Interval");
				ws.merge_cells("A11:A12");
				ws.cell(2, 11).value("Days"); ws.cell(3, 11).value("Minutes");
				ws.cell(2, 12).value(RunOpt.oidays); ws.cell(3, 12).value(RunOpt.oiminutes);
				ws.cell(2, 12).fill(yellow); ws.cell(3, 12).fill(yellow);
				ws.cell(1, 13).value("Number of Iterations per day");
				ws.cell(2, 13).value(RunOpt.noofiter);  ws.cell(2, 13).fill(yellow);
				ws.cell(3, 13).value("NC iterations per day");
				auto pParNitStep = static_cast<Ps*>(GetPtr(PAR_SINGLE, "NC Iteration"));
				auto numstep = pParNitStep->GetValue();
				ws.cell(4, 13).value(numstep);	 ws.cell(4, 13).fill(yellow); 
				ws.merge_cells("A14:D14");
				ws.cell(1, 14).value("This file is generated by CoupModel and can be used as complementary input file for Simulation");
				ws.merge_cells("A15:D15");
				ws.cell(1, 15).value("Explanations to variables are available at www.coupmodel.com");
				ws.cell(1, 15).hyperlink("http://www.coupmodel.com/default.htm");
				ws.cell(1, 16).value("Link to Details for how to use this file"); ws.cell(1, 16).hyperlink("http://www.coupmodel.com/archives/5947");
				ws.merge_cells("A16:D16");

			}
			else {
				ws = wb.sheet_by_title("Run Info");
				
				auto str = ws.cell(2, 6).to_string();
				auto str3 = ws.cell(2, 6).value<string>();
				auto tid = PGUtil::Ato_IPGTID(str);
				RunOpt.longMinStart = PGUtil::MinutConv(tid);
				str = ws.cell(4, 6).to_string();
				tid = PGUtil::Ato_IPGTID(str);
				RunOpt.longMinEnd = PGUtil::MinutConv(tid);

				
				str = ws.cell(2, 7).to_string();
				tid = PGUtil::Ato_IPGTID(str);
				auto preminstart = PGUtil::MinutConv(tid);
				str = ws.cell(4, 7).to_string();
				tid = PGUtil::Ato_IPGTID(str);
				RunOpt.longMinPostEnd = PGUtil::MinutConv(tid);
				auto cc = ws.cell(2, 8);
				int koll = cc.has_value();
				if (koll) {

					auto k=cc.value<double>();
					auto j = cc.value<int>();

				}
			

				auto str2 = ws.cell(2, 8).value<int>();
				m_DocFile.m_TimePeriodScaleFactor = ws.cell(2, 8).value<int>();
				string timeres = ws.cell(2, 10).value<string>();
				auto pSw = static_cast<Sw*>(GetPtr(SWITCH, "TimeResolution"));
				for (size_t i = 0; i < pSw->GetNumOptions(); i++) {
					if (pSw->GetOption(i) == timeres) pSw->SetIntValue(i);
				}
				RunOpt.oidays = ws.cell(2, 12).value<int>();
				RunOpt.oiminutes = ws.cell(2, 12).value<int>();
				RunOpt.noofiter = ws.cell(2, 12).value<int>();
				auto numNCSteps = ws.cell(2, 12).value<int>();;
				auto pParNitStep = static_cast<Ps*>(GetPtr(PAR_SINGLE, "NC Iteration"));
				pParNitStep->SetValue(numNCSteps);		  

			};





		}
	};
	auto FillSwitch = [&](bool reading) {
		if (reading) {
			xlnt::worksheet ws;

			if (AddToExisting) {
				ws = wb.sheet_by_title("Switches");
			}
			int rows = 0; int cols = 0;
			string group, name, opt, str;
			int ivalue;
			for (auto row : ws.rows(false))
			{
				rows++;
				cols = 0;
				if (rows == 1) continue;
				for (auto cell : row)
				{
					switch (cols) {
					case 0:
						group = cell.to_string();
						break;
					case 1:
						name = cell.to_string();
						break;
					case 2:
						str = cell.to_string();
						break;
					case 3:
						opt = cell.to_string();
						break;
					}
					cols++;
				}
				Sw* pSw = (Sw*)GetPtr(SWITCH, name, group);
				if (pSw != NULL) {
					pSw->SetStrValue(opt);
					pSw->SetIntValue(FUtil::AtoInt(str));
				}
			}
		}
		else {
			xlnt::worksheet ws;
			if (!AddToExisting) {
				ws = wb.create_sheet();
				ws.title("Switches");

				ws.cell(1, 1).value("Group");
				ws.cell(2, 1).value("Name");
				ws.cell(3, 1).value("Int Value");
				ws.cell(4, 1).value("String Value");
				if (AllItems) {
					ws.cell(5, 1).value("Options (grey)/ Conditions (color=category)");
					ws.merge_cells("E1:Q1");
				}
			}
			else {
				ws = wb.sheet_by_title("Switches");

			}
			auto Saved_Array = m_Sw_Array;
			if (AllItems) {
				auto ptrvector = GetPtrVector(SWITCH, true);
				m_Sw_Array.clear(); m_Sw_Array.resize(ptrvector.size());
				for (size_t i = 0; i < ptrvector.size(); i++)
					m_Sw_Array[i] =(Sw*)ptrvector[i];			
			}
			auto iv = m_Sw_Array.size();
			ws.column_properties("A").width = 25.;
			ws.column_properties("B").width = 25.;
			ws.column_properties("D").width = 25.;
			if (AllItems) {
				ws.column_properties("E").width = 25.;
				ws.column_properties("F").width = 25.;
				ws.column_properties("G").width = 25.;
				ws.column_properties("H").width = 25.;
				ws.column_properties("I").width = 25.;
				ws.column_properties("J").width = 25.;
				ws.column_properties("K").width = 25.;
				ws.column_properties("L").width = 25.;
			}
			for (size_t i = 0; i < iv; i++) {
				Sw* pSw = m_Sw_Array[i];
				auto group = pSw->GetGroup();
				auto name_psw = pSw->GetName();
				size_t iopt = pSw->GetIntValue();
				size_t idef = pSw->GetOriginalIntValue();
				auto opt = pSw->GetOption(iopt);
				auto colcat = p_ModelInfo->GetGroupCategoryNo(pSw->GetGroupNo());
				auto newcol = getexcel_col(colcat);
				auto yellow = xlnt::fill::solid(xlnt::color::yellow());
				ws.cell(1, i + 2).value(group);
				ws.cell(1, i + 2).fill(newcol);
				ws.cell(2, i + 2).value(name_psw);
				ws.cell(3, i + 2).value(iopt);
				if (iopt != idef) {
					ws.cell(3, i + 2).fill(getexcel_changed);
					ws.cell(4, i + 2).fill(getexcel_changed);
					if (opt == pSw->GetOption(iopt)) 	ws.cell(5 + iopt, i + 2).fill(getexcel_changed);
				}
				else {
					ws.cell(4, i + 2).fill(yellow);
					if (opt == pSw->GetOption(iopt)) 	ws.cell(5 + iopt, i + 2).fill(getexcel_changed);
				}
				ws.cell(4, i + 2).value(opt);
			
				if (AllItems) {
					int nopt = pSw->GetNumOptions();
					for (size_t iopt = 0; iopt < nopt; iopt++) {

						ws.cell(5 + iopt, i + 2).value(pSw->GetOption(iopt));
						ws.cell(5 + iopt, i + 2).fill(getexcel_grey);
						
					}
					auto icond = 0;
					string cond = pSw->GetCondition(icond);
					auto blue = xlnt::fill::solid(xlnt::color::blue());
					while (cond != "") {
						ws.cell(5 + nopt + icond, i + 2).value(cond);
						ws.cell(5 + nopt+icond, i + 2).fill(newcol);
						icond++;
						cond = pSw->GetCondition(icond);
					}
				}
			}

			if (AllItems) {
				m_Sw_Array = Saved_Array;
			}
			

		}
	};
	auto FillParameter = [&](bool reading) {
		if (reading) {
			xlnt::worksheet ws;
			if (AddToExisting) {
				ws = wb.sheet_by_title("Parameters");
			}
			int rows = 0; int cols = 0;
			string group, name, opt, str;
			int ivalue;
			double dvalue;
			for (auto row : ws.rows(false))
			{  	rows++;
				cols = 0;
				if (rows == 1) continue;
				for (auto cell : row)
				{
					switch (cols) {
					case 0:	group = cell.to_string();break;
					case 1:	name = cell.to_string(); break;
					case 2:	str = cell.to_string();	break;
					}
					cols++;
				}
				Ps* pP = (Ps*)GetPtr(PAR_SINGLE, name, group);
				if (pP != NULL) pP->SetValue(FUtil::AtoFloat(str));				
			}
		}
		else {
			xlnt::worksheet ws;
			if (!AddToExisting) {
				ws = wb.create_sheet();
				ws.title("Parameters");

				ws.cell(1, 1).value("Group");
				ws.cell(2, 1).value("Name");
				ws.cell(3, 1).value("Value");
				ws.cell(4, 1).value("Unit");
				ws.cell(5, 1).value("Default");
				ws.cell(6, 1).value("Min");
				ws.cell(7, 1).value("Max");
				ws.cell(8, 1).value("Warn Min");
				ws.cell(9, 1).value("Warn Max");
				ws.cell(10, 1).value("Function (grey)/Conditions (color=category)");
				ws.merge_cells("J1:Q1");

			}
			else {
				ws = wb.sheet_by_title("Parameters");
			}
			ws.column_properties("A").width = 25.;
			ws.column_properties("B").width = 25.;
			if (AllItems) {
				RemoveOriginalValues("Parameters", "ALL", true);
				ws.column_properties("J").width = 25.;
				ws.column_properties("K").width = 25.;
				ws.column_properties("J").width = 25.;
				ws.column_properties("K").width = 25.;
				ws.column_properties("L").width = 25.;
				ws.column_properties("M").width = 25.;
				ws.column_properties("N").width = 25.;
				ws.column_properties("O").width = 25.;
			}

			auto savedArray = m_P_Array;
			if (AllItems) {
				auto allpararray = GetPtrVector(PAR_SINGLE, true);
				m_P_Array.clear();
				m_P_Array.resize(allpararray.size());
				for (size_t i = 0; i < allpararray.size(); i++)
					m_P_Array[i] = (Ps*)allpararray[i];
			}
			auto iv = m_P_Array.size();
			for (size_t i = 0; i < iv; i++) {
				auto pP = m_P_Array[i];
				auto group = pP->GetGroup();
				auto name = pP->GetName();
				double value = pP->GetValue();
				auto unit = pP->GetUnit();
				auto def= pP->GetOriginalValue();
				auto colcat = p_ModelInfo->GetGroupCategoryNo(pP->GetGroupNo());
				auto newcol = getexcel_col(colcat);
				ws.cell(1, i + 2).value(group);
				ws.cell(1, i + 2).fill(newcol);
				ws.cell(2, i + 2).value(name);
				ws.cell(3, i + 2).value(value);
				if(def!=value) ws.cell(3, i + 2).fill(getexcel_changed);
		//		if(unit.find(''))
			//	if(i<15) ws.cell(4, i + 2).value(unit);
				ws.cell(5, i + 2).value(def);
				ws.cell(6, i + 2).value(pP->GetMinValue());
				ws.cell(7, i + 2).value(pP->GetMaxValue());
				ws.cell(8, i + 2).value(pP->GetWarnMinValue());
				ws.cell(9, i + 2).value(pP->GetWarnMaxValue());

				auto no_func = pP->GetNumOfTitles();
				auto grey = 0x0000ff;
				for (size_t ifunc = 0; ifunc < no_func; ifunc++) {
					ws.cell(10+ifunc, i + 2).value(pP->GetViewTitle(ifunc));
					ws.cell(10+ifunc, i + 2).fill(getexcel_grey);
				}
				auto icond = 0;
				string cond = pP->GetCondition(icond);
				auto blue = xlnt::fill::solid(xlnt::color::blue());
				while (cond != "") {
					ws.cell(10 + no_func + icond, i + 2).value(cond);
					ws.cell(10 + no_func + icond, i + 2).fill(newcol);
					icond++;
					cond = pP->GetCondition(icond);
				}



			}
			if (AllItems)  m_P_Array=savedArray;
		}
	};
	auto FillElements = [&](bool reading) {
		if (reading) {
			// No acction
		}
		else {
			xlnt::worksheet ws;
			if (!AddToExisting) {
				ws = wb.create_sheet();
				ws.title("Model Elements");
				ws.cell(1, 1).value("Name");
				ws.cell(2, 1).value("Item");
				ws.cell(3, 1).value("Number");

				ws.cell(4, 1).value("Variable");
				ws.merge_cells("D1:F1");

				ws.column_properties("A").width = 25.;
				ws.column_properties("B").width = 10.;
				ws.column_properties("C").width = 10.;
				ws.column_properties("D").width = 20.;
				ws.column_properties("E").width = 20.;
				ws.column_properties("F").width = 20.;
				ws.column_properties("G").width = 20.;
				ws.column_properties("H").width = 20.;
			}
			else {
				ws = wb.sheet_by_title("Model Elements");
			}
			auto ElementNameArray = GetNameOfNEPointers();
			auto colcat = p_ModelInfo->GetGroupCategoryNo(0);
			auto newcol = getexcel_col(colcat);


			NE* pNE;
			size_t lineno = 0;
			vector<string> strlink = { "Link to P", "Link to X", "Link to T", "Link to G", "Link to D" };
			vector<SimB*> ptrvector;
			auto iv = ElementNameArray.size();
			for (size_t i = 0; i < iv; i++) {
				auto name = ElementNameArray[i];
				pNE = GetNEPointer(name);
				auto nelement = pNE->GetNEValue();
				auto defvalue = pNE->GetOriginalNEValue();
				ws.cell(2, 2 + lineno).value("Elements");
				ws.cell(1, 2 + lineno).value(name);
				ws.cell(3, 2 + lineno).value(nelement);
				if (nelement == defvalue) ws.cell(3, 2 + lineno).fill(getexcel_yellow);
				else ws.cell(3, 2 + lineno).fill(getexcel_changed);

				for (size_t ilink = 0; ilink < 5; ilink++) {
					lineno++;
					ws.cell(2, 2 + lineno).value(strlink[ilink]);
					ptrvector.clear();
					size_t numlinks;
					switch (ilink) {
						case 0:
							numlinks = pNE->GetNumLinks_P();
							ptrvector.resize(numlinks);
							for (size_t linke = 0; linke < numlinks; linke++) {
								ptrvector[linke] = pNE->Get_PLink(linke);
							}
						break;
						case 1:
							numlinks = pNE->GetNumLinks_X();
							ptrvector.resize(numlinks);
							for (size_t linke = 0; linke < numlinks; linke++) {
								ptrvector[linke] = pNE->Get_XLink(linke);
							}
							break;
						case 2:
							numlinks = pNE->GetNumLinks_T();
							ptrvector.resize(numlinks);
							for (size_t linke = 0; linke < numlinks; linke++) {
								ptrvector[linke] = pNE->Get_TLink(linke);
							}
							break;
						case 3:
							numlinks = pNE->GetNumLinks_G();
							ptrvector.resize(numlinks);
							for (size_t linke = 0; linke < numlinks; linke++) {
								ptrvector[linke] = pNE->Get_GLink(linke);
							}
							break;
						case 4:
							numlinks = pNE->GetNumLinks_D();
							ptrvector.resize(numlinks);
							for (size_t linke = 0; linke < numlinks; linke++) {
								ptrvector[linke] = pNE->Get_DLink(linke);
							}
							break;
					}
					ws.cell(3 , 2 + lineno).value(ptrvector.size());
					for (size_t ik = 0; ik < ptrvector.size(); ik++) {					
						ws.cell(4+ik, 2 + lineno).value(ptrvector[ik]->GetName());
						colcat = p_ModelInfo->GetGroupCategoryNo(ptrvector[0]->GetGroupNo());
						newcol = getexcel_col(colcat);
					}
				};
				lineno++;
				for (size_t iline = 0; iline < 6; iline++) {
					ws.cell(1, 1+lineno - iline).fill(newcol);
				};
			};

		}
	};
	auto FillParameterVectors = [&](bool reading) {
		if (reading) {
			xlnt::worksheet ws;
			if (AddToExisting) {
				ws = wb.sheet_by_title("Parameter Vectors");
			}
	
		}
		else {
			xlnt::worksheet ws;
			if (!AddToExisting) {
				ws = wb.create_sheet();
				ws.title("Parameter Vectors");

				ws.cell(1, 1).value("Group");
				ws.cell(2, 1).value("Name");
				ws.cell(3, 1).value("Index");
				ws.cell(4, 1).value("Value");

				ws.cell(5, 1).value("Default");
				ws.cell(6, 1).value("Min");
				ws.cell(7, 1).value("Max");
				ws.cell(8, 1).value("Warn Min");
				ws.cell(9, 1).value("Warn Max");
				ws.cell(10, 1).value("Function (grey)/ Conditions(color category)");
				ws.merge_cells("J1:Q1");
			}
			else {
				ws = wb.sheet_by_title("Parameter Vectors");
			}
			ws.column_properties("A").width = 25.;
			ws.column_properties("B").width = 25.;
			if (AllItems) {
				RemoveOriginalValues("Parameters", "ALL", true);
				ws.column_properties("J").width = 25.;
				ws.column_properties("K").width = 25.;
				ws.column_properties("J").width = 25.;
				ws.column_properties("K").width = 25.;
				ws.column_properties("L").width = 25.;
				ws.column_properties("M").width = 25.;
				ws.column_properties("N").width = 25.;
				ws.column_properties("O").width = 25.;
			}

			auto savedArray = m_Pt_Array;
			if (AllItems) {
				auto allpararray = GetPtrVector(PAR_TABLE, true);
				m_Pt_Array.clear();
				m_Pt_Array.resize(allpararray.size());
				for (size_t i = 0; i < allpararray.size(); i++)
					m_Pt_Array[i] = (P*)allpararray[i];
			}
			auto iv = m_Pt_Array.size();
			size_t line_count = 0;
			for (size_t i = 0; i < iv; i++) {
				auto pP = m_Pt_Array[i];
				auto group = pP->GetGroup();
				auto name = pP->GetName();

				auto colcat = p_ModelInfo->GetGroupCategoryNo(pP->GetGroupNo());
				auto newcol = getexcel_col(colcat);
				ws.cell(1, line_count + 2).value(group);
				ws.cell(1, line_count + 2).fill(newcol);
				ws.cell(2, line_count + 2).value(name);
				xlnt::alignment center;
				center.vertical(xlnt::vertical_alignment::center);
				ws.cell(1, line_count + 2).alignment(center);
				ws.cell(2, line_count + 2).alignment(center);
			

				auto no_func = pP->GetNumOfTitles();
				auto grey = 0x0000ff;
				for (size_t ifunc = 0; ifunc < no_func; ifunc++) {
					ws.cell(10 + ifunc, line_count + 2).value(pP->GetViewTitle(ifunc));
					ws.cell(10 + ifunc, line_count + 2).fill(getexcel_grey);
				}
				auto icond = 0;
				string cond = pP->GetCondition(icond);
				auto blue = xlnt::fill::solid(xlnt::color::blue());
				while (cond != "") {
					ws.cell(10 + no_func + icond, line_count + 2).value(cond);
					ws.cell(10 + no_func + icond, line_count + 2).fill(newcol);
					icond++;
					cond = pP->GetCondition(icond);
				}
				for (size_t index = 0; index < pP->GetSize(); index++) {
					double value = pP->GetValue(index);
					auto def = pP->GetOriginalValue(index);
					ws.cell(3, line_count + 2).value(index+1);
					ws.cell(4, line_count + 2).value(value);
					if (def != value) ws.cell(4, line_count + 2).fill(getexcel_changed);
					ws.cell(5, line_count + 2).value(def);
					ws.cell(6, line_count + 2).value(pP->GetMinValue(index));
					ws.cell(7, line_count + 2).value(pP->GetMaxValue(index));
					ws.cell(8, line_count + 2).value(pP->GetWarnMinValue(index));
					ws.cell(9, line_count + 2).value(pP->GetWarnMaxValue(index));

				
					line_count++;
				}
				string cellformerge;
				if (pP->GetSize() > 1) {
					cellformerge = "A" + FUtil::STD_ItoAscii(line_count+2 - pP->GetSize()) + ":A" + FUtil::STD_ItoAscii(line_count+1);
					ws.merge_cells(cellformerge);
					cellformerge = "B" + FUtil::STD_ItoAscii(line_count + 2 - pP->GetSize()) + ":B" + FUtil::STD_ItoAscii(line_count + 1);
					ws.merge_cells(cellformerge);

				}

			}
			if (AllItems)  m_Pt_Array = savedArray;
		}
	};
	auto FillParameterFunctions = [&](bool reading) {
		if (reading) {
			xlnt::worksheet ws;
			if (AddToExisting) {
				ws = wb.sheet_by_title("Functions");
			}

		}
		else {
			xlnt::worksheet ws;
			if (!AddToExisting) {
				ws = wb.create_sheet();
				ws.title("Functions");

				ws.cell(1, 1).value("Group");
				ws.cell(2, 1).value("Name");
				ws.cell(3, 1).value("Parameters (grey)/ Parameter Vectors (yellow)");
				ws.merge_cells("C1:L1");
			
			}
			else {
				ws = wb.sheet_by_title("Functions");
			}
			ws.column_properties("A").width = 15.;
			ws.column_properties("B").width = 35.;
			ws.column_properties("C").width = 25.;			ws.column_properties("D").width = 25.;
			ws.column_properties("E").width = 25.;			ws.column_properties("F").width = 25.;

			vector<Func*> func_array;

			if (AllItems) {
				auto allpararray = GetPtrVector(FUNCTION, true);
				func_array.resize(allpararray.size());
				
				for (size_t i = 0; i < allpararray.size(); i++)
					func_array[i] = (Func*)allpararray[i];
			}
			auto iv = func_array.size();
			size_t line_count = 0;
			for (size_t i = 0; i < iv; i++) {
				auto pFunc = func_array[i];
				auto group = pFunc->GetGroup();
				auto name = pFunc->GetName();

				auto colcat = p_ModelInfo->GetGroupCategoryNo(pFunc->GetGroupNo());
				auto newcol = getexcel_col(colcat);
				ws.cell(1, line_count + 2).value(group);
				ws.cell(1, line_count + 2).fill(newcol);
				ws.cell(2, line_count + 2).value(name);
				xlnt::alignment center;
				center.vertical(xlnt::vertical_alignment::center);
				ws.cell(1, line_count + 2).alignment(center);
				ws.cell(2, line_count + 2).alignment(center);

				vector<string> ps_name = pFunc->GetParDep_Ps_Names();
				for (size_t ip = 0; ip < ps_name.size(); ip++) {
					ws.cell(3 + ip, line_count + 2).value(ps_name[ip]);
					ws.cell(3 + ip, line_count + 2).fill(getexcel_grey);
				}

				vector<string> p_name = pFunc->GetParDep_P_Names();
				for (size_t ip = 0; ip < p_name.size(); ip++) {
					ws.cell(3 + ip + ps_name.size(), line_count + 2).value(p_name[ip]);
					ws.cell(3 + ip, line_count + 2).fill(getexcel_yellow);
				}

				line_count++;
			}
		}
	};
	auto FillParTables = [&](bool reading) {
		if (reading) {
			xlnt::worksheet ws;
			if (AddToExisting) {
				ws = wb.sheet_by_title("Parameter Tables");
				int rows = 0; int cols = 0;
				string group, name, opt, str;
				vector<P*> ParPointers;
				bool ReadInsideTable = false;
				bool InsideTitles = false;
				int inside_row = 0, count_inside_row = 0;
				int ivalue;
				for (auto row : ws.rows(false))
				{
					rows++;
					cols = 0;
					if (rows == 1) continue;
					if (ReadInsideTable) {
						if (InsideTitles) {
							for (auto cell : row) {
								name = cell.to_string();
								if (cols > 0) {
									name = cell.to_string();
									if (name != "") {
										P* pP = (P*)GetP_Pointer(name);
										ParPointers.push_back(pP);
									}

								}
								if (name != "") cols++;
							}
							InsideTitles = false;
						}
						else {
							for (auto cell : row) {
								string str = cell.to_string();
								if (cols > 0 && str != "") {
									float value = FUtil::AtoFloat(str);
									if (ParPointers[cols - 1] != nullptr) {
										ParPointers[cols - 1]->SetValue(count_inside_row, value);
									}
								}
								if (str != "") cols++;

							}
							count_inside_row++;
							if (count_inside_row == inside_row||inside_row==0) {
								ReadInsideTable = false;
								InsideTitles = false;
								count_inside_row = 0;
								ParPointers.clear();
							}
						}
					}
					else {
						for (auto cell : row)
						{
							switch (cols) {
							case 0:
								group = cell.to_string();
								break;
							case 2:
								name = cell.to_string();
								break;
							case 5:
								str = cell.to_string();
								break;
							}
							cols++;
						}
						Tab* pTab = (Tab*)GetPtr(TABLE, name, group);
						if (pTab != NULL) {
							inside_row = FUtil::AtoInt(str);
							pTab->SetNumOfRow(inside_row);
							ReadInsideTable = true;
							InsideTitles = true;
						}
					}
				}
			}
		}
		else {
			xlnt::worksheet ws;
			xlnt::color fillcolor;
			auto green =xlnt::fill::solid(xlnt::rgb_color(100,10,100));
			if (!AddToExisting) {
				ws = wb.create_sheet();
				ws.title("Parameter Tables");
				ws.cell(1, 1).value("Group");
				ws.cell(3, 1).value("Name");
				ws.cell(6, 1).value("Number of Elements");
				ws.merge_cells("A1:B1");
				ws.merge_cells("C1:E1");
			}
			else {
				ws = wb.sheet_by_title("Parameter Tables");
   			}
			if (AllItems)  RemoveOriginalValues("Tables", "ALL", true);
			auto iv = m_Tab_Array.size();
			size_t rowcount = 0;


			for (size_t i = 0; i < iv; i++) {
				auto pTab = m_Tab_Array[i];
				auto group = pTab->GetGroup();
				auto colcat =p_ModelInfo->GetGroupCategoryNo(pTab->GetGroupNo());
				auto newcol = getexcel_col(colcat);
				auto name = pTab->GetName();
				auto nelement = pTab->GetNumOfRow();
				ws.cell(1, rowcount + 2).value(group);
				ws.cell(1, rowcount + 2).fill(newcol);
				ws.cell(2, rowcount + 2).fill(newcol);
				ws.cell(3, rowcount + 2).value(name);
				ws.cell(6, rowcount + 2).value(nelement);
				rowcount++;
				string pname;
				P* pP;
				ws.cell(1, rowcount + 2).value("Index");
				for (size_t nCol = 0; nCol < pTab->GetNumOfCol(); nCol++) {
					pP= pTab->GetP_Pointer(nCol);
					ws.cell(2+nCol,rowcount+2).value(pP->GetName());
				}
				auto NumRow = pTab->GetNumOfRow();
				for (size_t nRow = 0; nRow < NumRow; nRow++) {
					rowcount++;
					ws.cell(1, rowcount + 2).value(nRow+1);
					for (size_t nCol = 0; nCol < pTab->GetNumOfCol(); nCol++) {
						pP = pTab->GetP_Pointer(nCol);
						ws.cell(2+nCol, rowcount + 2).value(pP->GetValue(nRow));
					}
				}
				rowcount++;
			}
			if (AllItems)  RemoveOriginalValues("Tables", "ALL", false);
		}
	};
	auto FillModelFiles = [&](bool reading) {
		if (reading) {
			xlnt::worksheet ws;
			if (AddToExisting) {
				ws = wb.sheet_by_title("Model Files");
				int rows = 0; int cols = 0;
				string group, name, opt, str;
				int ivalue;
				for (auto row : ws.rows(false))
				{
					rows++;
					cols = 0;
					if (rows == 1) continue;
					for (auto cell : row)
					{
						switch (cols) {
						case 0:
							group = cell.to_string();
							break;
						case 1:
							name = cell.to_string();
							break;
						case 2:
							str = cell.to_string();
							break;

						}
						cols++;

					}
					F* pF = (F*)GetPtr(PGFILE, name, group);
					if (pF != NULL && group != "Hidden") {
						pF->SetValue(str);
					}
				}
			}
		}
		else {
			xlnt::worksheet ws;
			if (!AddToExisting) {
				ws = wb.create_sheet();
				ws.title("Model Files");
				ws.cell(1, 1).value("Group");
				ws.cell(2, 1).value("Option");
				ws.cell(3, 1).value("File Name");				
			}
			else {
				ws = wb.sheet_by_title("Model Files");
			}
			if (AllItems)  RemoveOriginalValues("Files", "ALL", true);
			auto iv = m_F_Array.size();
			ws.column_properties("A").width = 50.;
			ws.column_properties("B").width = 50.;
			ws.column_properties("C").width = 50.;
			for (size_t i = 0; i < iv; i++) {
				F* pF = m_F_Array[i];
				auto group = pF->GetGroup();
				auto name = pF->GetName();
				string filename = pF->GetStrValue();
				auto colcat = p_ModelInfo->GetGroupCategoryNo(pF->GetGroupNo());
				auto newcol = getexcel_col(colcat);
				ws.cell(1, i + 2).value(group);
				ws.cell(1, i + 2).fill(newcol);
				ws.cell(2, i + 2).value(name);
				ws.cell(3, i + 2).value(filename);
			}
			if (AllItems)  RemoveOriginalValues("Files", "ALL", false);
		}
	};
	auto FillOutputs = [&](bool reading) {
		if (reading) {
			xlnt::worksheet ws;
			if (AddToExisting) {
				ws = wb.sheet_by_title("Output Variables");
			}
			int rows = 0; int cols = 0;
			string group, name, opt, str;
			int ivalue;
			for (auto row : ws.rows(false))
			{
				rows++;
				cols = 0;
				if (rows == 1) continue;
				for (auto cell : row)
				{
					switch (cols) {
					case 0:
						group = cell.to_string();
						break;
					case 1:
						name = cell.to_string();
						break;
					case 2:
						str = cell.to_string();
						break;
					}
					cols++;
				}
				Ps* pP = (Ps*)GetPtr(PAR_SINGLE, name, group);
				if (pP != NULL) {
					pP->SetValue(FUtil::AtoFloat(str));

				}
			}
		}
		else {
			xlnt::worksheet ws;
			if (!AddToExisting) {
				ws = wb.create_sheet();
				ws.title("Output Variables");
				ws.cell(1, 1).value("Type");
				ws.cell(2, 1).value("Group");
				ws.cell(3, 1).value("Name");
				ws.cell(4, 1).value("Single Run");
				ws.cell(5, 1).value("Multi Run");
			}
			else {
				ws = wb.sheet_by_title("Output Variables");
				ws.cell(6, 1).value("Type");
				auto koll = ws.cell(1, 1).has_value();
				string koll2 = ws.cell(1, 1).value<string>();

				int rows = 0; int cols = 0;
				string type, group, name, opt, str;
				bool is_vector;
				int index, storeflag;
				for (auto row : ws.rows(false))
				{
					rows++;
					cols = 0;
					string k;
					if (rows == 1) continue;

					for (auto cell : row)
					{
						if (cell.has_value()) {
							string strindex;

							auto str1 = cell.value<string>();
							switch (cols) {
							case 0:
								type =str1;
								break;
							case 1:
								group= str1;
								break;
							case 2:
								name = str1;
								is_vector = false;
								if (name.find("(") != string::npos && name.find(")") != string::npos) {
									size_t len=name.find(")") - name.find("(") - 1;
									strindex = name.substr(name.find("(")+1
										, len);
									index = FUtil::AtoInt(strindex);
									is_vector = true;
									name = name.substr(0, name.find("("));
								}
								break;
							case 3:
								storeflag = 0;
								if (str1.find("X") != string::npos || str1.find("x") != string::npos) storeflag = 1;
								break;
							case 4:
								if (str1.find("X") != string::npos || str1.find("x") != string::npos) storeflag = 2;
								break;
							}
							cols++;
						}
					}				
					if (is_vector) {
						OutVector* pVector;
						pVector = dynamic_cast<OutVector*>(GetPtr(type, group,name));
						if (pVector != nullptr) {
							pVector->SetStoreFlag(index, storeflag);
						}
					}
					else {
						OutSingle* pSingle;
						pSingle = dynamic_cast<OutSingle*>(GetPtr(type,group, name));
						if (pSingle != nullptr) {
							pSingle->SetStoreFlag(storeflag);
						}
					}
				}
			}
			ws.column_properties("A").width = 25.;
			ws.column_properties("B").width = 35.;
			ws.column_properties("C").width = 35.;
			ws.column_properties("F").width = 25.;
			ws.column_properties("G").width = 25.;
			ws.column_properties("H").width = 25.;
			vector<SIMB> pvv;
			if (AllItems)  pvv = GetAllOutputs(string::npos, true);
			else pvv = GetAllSelectedOutputs();
			auto iv = pvv.size();

			string type, group, name;
			size_t storeflag=0;
			for (size_t i = 0; i < iv; i++) {
				type = pvv[i].pBase->GetTypeName();
				auto pVector = dynamic_cast<OutVector*>(pvv[i].pBase);
				size_t lastindex;
				bool hitoflast = false;
				if (pVector != nullptr) {
					group = pVector->GetGroup();			
					name = pVector->GetName();	
					lastindex = pVector->GetNumVector();
					auto lin = pvv[i].TabIndex;
					hitoflast = false;
					if (lin == lastindex - 1) hitoflast = true;
					name += "(" + FUtil::STD_ItoAscii(lin + 1) + ")";			
					storeflag = pVector->GetStoreFlag(lin);

				}
				else {
					auto pSingle = dynamic_cast<OutSingle*>(pvv[i].pBase);
					if (pSingle != nullptr) {
						group = pSingle->GetGroup();
						name = pSingle->GetName();
						storeflag = pSingle->GetStoreFlag(); 					
					}
				}
				auto colcat = p_ModelInfo->GetGroupCategoryNo(pvv[i].pBase->GetGroupNo());
				auto newcol = getexcel_col(colcat);
				ws.cell(1, i + 2).value(type);

				if (pVector != nullptr) {
					if (hitoflast) {
						ws.cell(2, i + 3-lastindex).value(group);
						ws.cell(2, i + 3-lastindex).fill(newcol);
						string merge;
						merge = "B" + FUtil::STD_ItoAscii(i + 3 - lastindex) + ":B" + FUtil::STD_ItoAscii(i + 2);

						ws.merge_cells(merge);
						merge = "A" + FUtil::STD_ItoAscii(i + 3 - lastindex) + ":A" + FUtil::STD_ItoAscii(i + 2);
						ws.merge_cells(merge);
						xlnt::alignment center;
						center.vertical(xlnt::vertical_alignment::center);
						ws.cell(2, i + 3-lastindex).alignment(center);
						ws.cell(1, i + 3-lastindex).alignment(center);
					}
				}
				else {
					ws.cell(2, i + 2).value(group);
					ws.cell(2, i + 2).fill(newcol);
				}

					ws.cell(3, i + 2).value(name);
				if(storeflag>=1) ws.cell(4, i + 2).value("X");
				if(storeflag==2) ws.cell(5, i + 2).value("X");
				auto icond = 0;
				string cond = pvv[i].pBase->GetCondition(icond);
				auto blue = xlnt::fill::solid(xlnt::color::blue());
				if (AllItems) {
					while (cond != "") {
						ws.cell(6 + icond, i + 2).value(cond);
						ws.cell(6 + icond, i + 2).fill(newcol);
						icond++;
						cond = pvv[i].pBase->GetCondition(icond);
					}
				}


			}
		}

	};

	FillRunInfo(reading);
	FillSwitch(reading);
	FillParameter(reading);
	if (AllItems) {
		FillElements(reading);
		FillParameterVectors(reading);
		FillParameterFunctions(reading);
	}
	FillParTables(reading);
	
	FillModelFiles(reading);
	FillOutputs(reading);
	if(!reading) wb.save(filein);
	wb.clear();
	return filein;
#endif
}
