#pragma once
#include "../ModelTypes/SimB.h"
#include "./ModelCompNames.hpp"
#include <string>
#include <vector>
#include <map>

class Sim;



struct RunStatusOfDocumentInfo
{
	bool StartMode;
	int ViewStatus;
	bool Running;
	bool MultiRunning;
	bool ReadyToSimulate;
	bool NoSingleRunSimFile;

};

class CommonModelInfo 
{ 
public:
	CommonModelInfo();
	~CommonModelInfo();
	const std::string GetPassword() { return "pe1950"; };
	size_t GetTypeIndex(std::string str);
	size_t GetGroupIndex(std::string str);
	size_t GetGroupCategoryNo(size_t index) {
		auto it = ModelCompNames::GroupCategoryNo.find(index);
		if (it != ModelCompNames::GroupCategoryNo.end())
			return it->second;
		else
			return std::string::npos;
	};

	std::string GetGroupCategoryNames(size_t index) {if(index< ModelCompNames::NoGroupCategoryNames) return ModelCompNames::GroupCategoryNames[index];return "All Modules";};

	std::vector<std::string> GetGroupCategoryNames() { std::vector<std::string> s; for(size_t i = 0; i < ModelCompNames::NoGroupCategoryNames; i++) 
		s.push_back(ModelCompNames::GroupCategoryNames[i]);  return s; };

	std::string GetGroupNames(size_t index) {if(index<ModelCompNames::NoGroupNames) return ModelCompNames::GroupNames[index];return "";};
	std::string GetVarType(size_t ityp) { if(ityp<ModelCompNames::NoTypNames) return ModelCompNames::TypNames[ityp]; return "";};
	
	std::vector<std::string> GetFysProcName() { std::vector<std::string> s; for (size_t i = 0; i < ModelCompNames::NoFysProcessNames;i++) s.push_back(ModelCompNames::FysProcessNames[i]); return s; };
	std::string GetFysProcName(size_t index) {if(index< ModelCompNames::NoFysProcessNames) return ModelCompNames::FysProcessNames[index]; else return "";};




	std::vector<std::string> GetBioProcName() { std::vector<std::string> s; for (size_t i = 0; i < ModelCompNames::NoBioProcessNames; i++) s.push_back(ModelCompNames::BioProcessNames[i]); return s; };
	std::string GetBioProcName(size_t index) {if(index<ModelCompNames::NoBioProcessNames) return ModelCompNames::BioProcessNames[index]; else return "";};

	std::vector<std::string> GetElementName() { std::vector<std::string> s; for (size_t i = 0; i < ModelCompNames::NoElementNames; i++) s.push_back(ModelCompNames::ElementNames[i]); return s; };
	std::string ElementName(size_t index) {if(index<ModelCompNames::NoElementNames) return ModelCompNames::ElementNames[index]; else return ""; };

	int GetGroupCategory(std::string str);
	int GetGroupCategoryNo(int moduleNo) {
		auto it = ModelCompNames::GroupCategoryNo.find(moduleNo);
		if (it != ModelCompNames::GroupCategoryNo.end())
			return it->second;
		else
			return -1;
	};
	void trim_xml(std::string& str);
	void SetCommandLineMode(bool value) {m_CommandInfo=value;};
	bool GetCommandLineMode() {return m_CommandInfo;};
	bool IsRunning() {return m_RunInfo.Running;};
	void SetRunning(bool value) {m_RunInfo.Running=value;};
	bool IsMultiRunning() {return m_RunInfo.MultiRunning;};
	void SetMultiRunning(bool value) {m_RunInfo.MultiRunning=value;};
	bool IsStartMode() {return m_RunInfo.StartMode;};
	void SetStartMode(bool value) {m_RunInfo.StartMode=value;};
	bool IsReady() {return m_RunInfo.ReadyToSimulate;};
	void SetReady(bool value) {m_RunInfo.ReadyToSimulate=value;};
	void SetRunDoc(Sim* pSim) {m_pRunDoc=pSim;};
	int GetViewStatus() {return m_RunInfo.ViewStatus;};
	void SetViewStatus(int value) {m_RunInfo.ViewStatus=value;};
	bool GetNoSingleRun() {return m_RunInfo.NoSingleRunSimFile;};
	void SetNoSingleRun(bool value) {m_RunInfo.NoSingleRunSimFile=value;};
	std::string getOrigName(std::string txt) {
		auto it = changed_names.find(txt);
		if (it != changed_names.end())
			return (*it).second;
		else
			return txt;
	}
	void insert_ChangedNames(std::vector<std::pair<std::string, std::string>>);

	size_t GetNumUnits() { return m_TypeBasedUnits.size(); };

	std::string GetUnitString(UNIT_TYPES key, bool withparanthesis = false) {
		auto it = m_TypeBasedUnits.find(key);
		if (it != m_TypeBasedUnits.end()) {
			if (withparanthesis) {
				return "(" + (*it).second + ")";
			}
			else
				return (*it).second;
		}
		else return "";
	};
	UNIT_TYPES GetUnitType(std::string key);
	std::vector<std::string> GetAllStringBasedUnits() {
		std::vector<std::string> out;
		for (auto it = m_TypeBasedUnits.begin(); it != m_TypeBasedUnits.end(); ++it) {
			out.push_back((*it).second);
		};
		return out;
	};
	std::vector<std::string> GetSelectedStringBasedUnits(std::vector<UNIT_TYPES> key) {
		std::vector<std::string> out;
		for_each(key.begin(), key.end(), [&](UNIT_TYPES& key) {
			auto it = m_TypeBasedUnits.find(key);
			out.push_back((*it).second);
			});
		return out;
	};
	std::vector<std::string> GetSortedListofElements();
	std::vector<std::string> GetSortedListofElements(std::vector<elements> v_elements);

	std::vector<std::string> GetSortedListofPhysProc();
	std::vector<std::string> GetSortedListofPhysProc(std::vector<fysprocess> v_fysprocess);

	std::vector<std::string> GetSortedListofBiolProc();
	std::vector<std::string> GetSortedListofBiolProc(std::vector<bioprocess> v_bioprocess);

	std::vector<std::string> GetSortedListofGroupNames() { return m_GroupNamesSorted; };
	std::vector<std::string> GetGroupNames() {
		std::vector<std::string> out;
		for (std::string name : ModelCompNames::GroupNames) {
			out.push_back(name);
		}
		return out;
	}
	size_t GetNumberOfElements() { return m_ElementSorted.size(); };
	std::string GetStringOfElement(elements elem);
	elements GetElementFromString(std::string str);
	fysprocess GetPhysProcFromString(std::string str);
	bioprocess GetBioProcFromString(std::string str);
	bool DefineSwitchMap(std::vector<SimB*> allSwitches);
	int GetSwitchId(std::string name);
	std::string GetSwitchName(int id);

	bool DefineSingleParMap(std::vector<SimB*> allSinglePar) ;
	int GetSingleParId(std::string name);
	std::string GetSingleParameterName(int id);


	bool DefineVectorParMap(std::vector<SimB*> allVectorPar) ;
	int GetVectorParId(std::string name);
	bool DefineNEVectorMap(std::vector<SimB*> allVectorPar);
	std::string GetVectorParameterName(int id);
	
	int GetNEVectorId(std::string name);

	bool DefineSingleOutputMap(std::vector<SimB*> allVectorPar);
	int GetSingleOutputId(std::string name);
	std::string GetSingleOutputName(int id);

	bool DefineVectorOutputMap(std::vector<SimB*> allVectorPar);
	int GetVectorOutputId(std::string name);
	std::string GetVectorOutputName(int id);

	bool DefineParameterFunctionMap(std::vector<SimB*> allVectorPar);
	int GetParameterFunctionId(std::string name);
	bool DefineTimeSeriesMap(std::vector<SimB*> allVectorPar);
	int GetTimeSeriesId(std::string name);
	std::string GetTimeSeriesName(int id);


	Sim *GetRunDoc() {return m_pRunDoc;};
	std::vector<SimB*> SortSimB_ByGroup(std::vector<SimB*> v);
	std::vector<SimB*> SortSimB_ByCategory(std::vector<SimB*> v);
	std::map<std::string, std::string> changed_names;


private:
	void SetUnitMap(UNIT_TYPES type, std::string str) {
		m_TypeBasedUnits.insert(std::pair<UNIT_TYPES, std::string>(type, str));
		m_StringBasedUnits.insert(std::pair<std::string, UNIT_TYPES>(str, type));
	};

	std::map <std::string, size_t> m_TypeMap, m_GroupMap;
	std::map <std::string, int> m_SwitchMap, m_SingleParMap, m_VectorParMap, m_NE_VectorMap, m_SingleOutputMap, m_VectorOutputMap;
	std::map <std::string, int> m_ParameterFunctionMap,m_TimeSeriesMap;
	std::map <std::string, int> m_GroupCategory;
	
	std::vector<std::string> m_GroupNamesSorted;
	std::vector<std::string> m_ElementSorted;
	std::vector<std::string> m_FysProcessNamesSorted, m_BioProcessNamesSorted;
	RunStatusOfDocumentInfo m_RunInfo;
	
	std::map <elements, std::string> m_ElementNamesMap;
	std::map <std::string,elements> m_NamesElementMap;

	std::map <fysprocess, std::string> m_PhysProcNamesMap;
	std::map <std::string, fysprocess> m_NamesFysProcMap;

	std::map <bioprocess, std::string> m_BioProcNamesMap;
	std::map <std::string, bioprocess> m_NamesBioProcMap;

	std::map<UNIT_TYPES, std::string> m_TypeBasedUnits;
	std::map<std::string, UNIT_TYPES> m_StringBasedUnits;



	bool m_CommandInfo;
	Sim *m_pRunDoc;
};
