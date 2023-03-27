
using namespace std;

#include "./CommonModelInfo.h"
#include "./ModelCompNames.hpp"
#include "../ModelTypes/SimB.h"

CommonModelInfo::CommonModelInfo() {

		m_RunInfo.StartMode=true;
		m_RunInfo.ReadyToSimulate=true;
		m_RunInfo.Running=false;
		m_RunInfo.MultiRunning=false;
		m_RunInfo.ViewStatus=0;
		size_t count = 0;
		map <string, elements> SortedListElementNames;

		for(size_t i=0; i <ModelCompNames::NoElementNames;++i) {
			SortedListElementNames.insert(pair<string, elements>(ModelCompNames::ElementNames[i], elements(count)));
			m_ElementNamesMap.insert(pair<elements, string>(elements(count), ModelCompNames::ElementNames[i]));
			count++;
		};
		m_NamesElementMap = SortedListElementNames;
		for (auto it = SortedListElementNames.begin(); it != SortedListElementNames.end(); ++it) {
			m_ElementSorted.push_back((*it).first);
		}
		map <string, fysprocess> SortedListPhysProcNames;
		count = 0;
		for (size_t i = 0; i < ModelCompNames::NoFysProcessNames;++i) {
			SortedListPhysProcNames.insert(pair<string, fysprocess>(ModelCompNames::FysProcessNames[i], fysprocess(count)));
			m_PhysProcNamesMap.insert(pair<fysprocess, string>(fysprocess(count), ModelCompNames::FysProcessNames[i]));
			count++;
		};
		m_NamesFysProcMap = SortedListPhysProcNames;
		for (auto it = SortedListElementNames.begin(); it != SortedListElementNames.end(); ++it) {
			m_FysProcessNamesSorted.push_back((*it).first);
		}
		map <string, bioprocess> SortedBiolProcessNames;
		count = 0;
		for (size_t i = 0; i < ModelCompNames::NoBioProcessNames; ++i) {
			SortedBiolProcessNames.insert(pair<string, enum bioprocess>(ModelCompNames::BioProcessNames[i], bioprocess(count)));
			m_BioProcNamesMap.insert(pair<bioprocess, string>(bioprocess(count), ModelCompNames::BioProcessNames[i]));
			count++;
		};
		m_NamesBioProcMap = SortedBiolProcessNames;
		for (auto it = SortedBiolProcessNames.begin(); it != SortedBiolProcessNames.end(); ++it) {
			m_BioProcessNamesSorted.push_back((*it).first);
		}


		m_GroupMap.clear();
		for (size_t i = 0; i < ModelCompNames::NoGroupNames; i++)
			m_GroupMap[ModelCompNames::GroupNames[i]] = i;
		m_TypeMap.clear();
		for (size_t i = 0; i < ModelCompNames::NoTypNames; ++i)
			m_TypeMap[ModelCompNames::TypNames[i]] = i;
		m_GroupCategory.clear();
		for (size_t i = 0; i < ModelCompNames::NoGroupCategoryNames; ++i)
			m_GroupCategory[ModelCompNames::GroupCategoryNames[i]] = i;

		auto koll = GetAllStringBasedUnits();



}
CommonModelInfo::~CommonModelInfo() {

}
UNIT_TYPES CommonModelInfo::GetUnitType(string key) {
	auto it = m_StringBasedUnits.find(key);
	if (it != m_StringBasedUnits.end())
		return (*it).second;
	else
		return UNIT_TYPES::NO_UNIT;
};
size_t CommonModelInfo::GetTypeIndex(string str) {
	auto it=m_TypeMap.find(str);
	if(it!=m_TypeMap.end())
		return it->second;
	else
		return string::npos;
}
size_t CommonModelInfo::GetGroupIndex(string str) {
	auto it = m_GroupMap.find(str);
	if (it != m_GroupMap.end())
		return it->second;
	else
		return string::npos;
}
int CommonModelInfo::GetGroupCategory(string str) {
	auto it=m_GroupCategory.find(str);
	if(it!=m_GroupCategory.end())
		return it->second;
	else
		return -1;
}
bool CommonModelInfo::DefineSwitchMap(vector<SimB*> allSwitches) {
	if (m_SwitchMap.size() > 0)  m_SwitchMap.clear();
	for (int i = 0; i < allSwitches.size(); ++i) {
		string name = allSwitches[i]->GetName();
		m_SwitchMap[name] = i + 1;
	}
	if (m_SwitchMap.size() == allSwitches.size()) return true;
	return false;
};
bool CommonModelInfo::DefineSwitchMap(vector<pair<string, int>> vector_pair) {
	if (m_SwitchMap.size() > 0)  m_SwitchMap.clear();
	for (int i = 0; i < vector_pair.size(); ++i) {
		m_SwitchMap[vector_pair[i].first] = vector_pair[i].second;
	};
	if (m_SwitchMap.size() > 0) return true;
}
int CommonModelInfo::GetSwitchId(string name) {
	auto it = m_SwitchMap.find(name);
	if (it != m_SwitchMap.end())
		return it->second;
	else
		return -1;
};
string CommonModelInfo::GetSwitchName(int id) {
	for (const auto& [key, value] : m_SwitchMap) {
		if (value == id) return key;
	}
	return "";	
};
bool CommonModelInfo::DefineSingleParMap(vector<SimB*> allSinglePar) {
	if (m_SingleParMap.size() > 0)  m_SingleParMap.clear();
	for (int i = 0; i < allSinglePar.size(); ++i) {
		string name = allSinglePar[i]->GetName();
		m_SingleParMap[name] = i + 1;
	}
	if (m_SingleParMap.size() == allSinglePar.size()) return true;
	return false;
};
bool CommonModelInfo::DefineSingleParMap(vector<pair<string, int>> vector_pair) {
	if (m_SingleParMap.size() > 0)  m_SingleParMap.clear();
	for (int i = 0; i < vector_pair.size(); ++i) {
		m_SingleParMap[vector_pair[i].first] = vector_pair[i].second;
	};
	if (m_SingleParMap.size() > 0) return true;
}
int CommonModelInfo::GetSingleParId(string name) {
	auto it = m_SingleParMap.find(name);
	if (it != m_SingleParMap.end())
		return it->second;
	else
		return -1;
};
string CommonModelInfo::GetSingleParameterName(int id) {
	for (const auto& [key, value] : m_SingleParMap) {
		if (value == id) return key;
	}
	return "";
};
bool CommonModelInfo::DefineVectorParMap(vector<SimB*> allVectorPar) {
	if (m_VectorParMap.size() > 0)  m_VectorParMap.clear();
	for (int i = 0; i < allVectorPar.size(); ++i) {
		string name = allVectorPar[i]->GetName();
		m_VectorParMap[name] = i + 1;
	}
	if (m_VectorParMap.size() == allVectorPar.size()) return true;
	return false;
};
bool CommonModelInfo::DefineVectorParMap(vector<pair<string, int>> vector_pair) {
	if (m_VectorParMap.size() > 0)  m_VectorParMap.clear();
	for (int i = 0; i < vector_pair.size(); ++i) {
		m_VectorParMap[vector_pair[i].first] = vector_pair[i].second;
	};
	if (m_VectorParMap.size() > 0) return true;
}
int CommonModelInfo::GetVectorParId(string name) {
	auto it = m_VectorParMap.find(name);
	if (it != m_VectorParMap.end())
		return it->second;
	else
		return -1;
};
string CommonModelInfo::GetVectorParameterName(int id) {
	for (const auto& [key, value] : m_VectorParMap) {
		if (value == id) return key;
	}
	return "";
};

string CommonModelInfo::GetVectorOutputName(int id) {
	for (const auto& [key, value] : m_VectorOutputMap) {
		if (value == id) return key;
	}
	return "";
};
string CommonModelInfo::GetSingleOutputName(int id) {
	for (const auto& [key, value] : m_SingleOutputMap) {
		if (value == id) return key;
	}
	return "";
};
bool CommonModelInfo::DefineNEVectorMap(vector<SimB*> allVectorPar) {
	if (m_NE_VectorMap.size() > 0)  m_NE_VectorMap.clear();
	for (int i = 0; i < allVectorPar.size(); ++i) {
		string name = allVectorPar[i]->GetName();
		m_NE_VectorMap[name] = i + 1;
	}
	if (m_NE_VectorMap.size() == allVectorPar.size()) return true;
	return false;
};
int CommonModelInfo::GetNEVectorId(string name) {
	auto it = m_NE_VectorMap.find(name);
	if (it != m_NE_VectorMap.end())
		return it->second;
	else
		return -1;
};
bool CommonModelInfo::DefineSingleOutputMap(vector<SimB*> allVectorPar) {
	if (m_SingleOutputMap.size() > 0)  m_SingleOutputMap.clear();
	for (int i = 0; i < allVectorPar.size(); ++i) {
		string name = allVectorPar[i]->GetName();
		m_SingleOutputMap[name] = i + 1;
	}
	if (m_SingleOutputMap.size() == allVectorPar.size()) return true;
	return false;
};
bool CommonModelInfo::DefineSingleOutputMap(vector<pair<string, int>> vector_pair) {
	if (m_SingleOutputMap.size() > 0)  m_SingleOutputMap.clear();
	for (int i = 0; i < vector_pair.size(); ++i) {
		m_SingleOutputMap[vector_pair[i].first] = vector_pair[i].second;
	};
	if (m_SingleOutputMap.size() > 0) return true;
}
int CommonModelInfo::GetSingleOutputId(string name) {
	auto it = m_SingleOutputMap.find(name);
	if (it != m_SingleOutputMap.end())
		return it->second;
	else
		return -1;
};
bool CommonModelInfo::DefineVectorOutputMap(vector<SimB*> allVectorPar) {
	if (m_VectorOutputMap.size() > 0)  m_VectorOutputMap.clear();
	for (int i = 0; i < allVectorPar.size(); ++i) {
		string name = allVectorPar[i]->GetName();
		m_VectorOutputMap[name] = i + 1;
	}
	if (m_VectorOutputMap.size() == allVectorPar.size()) return true;
	return false;
};
bool CommonModelInfo::DefineVectorOutputMap(vector<pair<string, int>> vector_pair) {
	if (m_VectorOutputMap.size() > 0)  m_VectorOutputMap.clear();
	for (int i = 0; i < vector_pair.size(); ++i) {
		m_VectorOutputMap[vector_pair[i].first] = vector_pair[i].second;
	};
	if (m_VectorOutputMap.size() > 0) return true;
}
int CommonModelInfo::GetVectorOutputId(string name) {
	auto it = m_VectorOutputMap.find(name);
	if (it != m_VectorOutputMap.end())
		return it->second;
	else
		return -1;
};
bool CommonModelInfo::DefineParameterFunctionMap(vector<SimB*> allVectorPar) {
	if (m_ParameterFunctionMap.size() > 0)  m_ParameterFunctionMap.clear();
	for (int i = 0; i < allVectorPar.size(); ++i) {
		string name = allVectorPar[i]->GetName();
		m_ParameterFunctionMap[name] = i + 1;
	}
	if (m_ParameterFunctionMap.size() == allVectorPar.size()) return true;
	return false;
};
int CommonModelInfo::GetParameterFunctionId(string name) {
	auto it = m_ParameterFunctionMap.find(name);
	if (it != m_ParameterFunctionMap.end())
		return it->second;
	else
		return -1;
};
bool CommonModelInfo::DefineTimeSeriesMap(vector<SimB*> allVectorPar) {
	if (m_TimeSeriesMap.size() > 0)  m_TimeSeriesMap.clear();
	for (int i = 0; i < allVectorPar.size(); ++i) {
		string name = allVectorPar[i]->GetName();
		m_TimeSeriesMap[name] = i + 1;
	}
	if (m_TimeSeriesMap.size() == allVectorPar.size()) return true;
	return false;
};
bool CommonModelInfo::DefineTimeSeriesMap(vector<pair<string, int>> vector_pair) {
	if (m_TimeSeriesMap.size() > 0)  m_TimeSeriesMap.clear();
	for (int i = 0; i < vector_pair.size(); ++i) {
		m_TimeSeriesMap[vector_pair[i].first] = vector_pair[i].second;
	};
	if (m_TimeSeriesMap.size() > 0) return true;
}
int CommonModelInfo::GetTimeSeriesId(string name) {
	auto it = m_TimeSeriesMap.find(name);
	if (it != m_TimeSeriesMap.end())
		return it->second;
	else
		return -1;
};
string CommonModelInfo::GetTimeSeriesName(int id) {
	for (const auto& [key, value] : m_TimeSeriesMap) {
		if (value == id) return key;
	}
	return "";
};

void CommonModelInfo::trim_xml(string& str)
{
	string in, out;
	in = str;
	auto pos = str.find(" ");
	while (pos != string::npos) {
		str.erase(pos, 1);
		pos = str.find(" ");
	}
	pos = str.find(",");
	while (pos != string::npos) {
		str.erase(pos, 1);
		pos = str.find(",");
	}
	pos = str.find("(");
	while (pos != string::npos) {
		str.erase(pos, 1);
		pos = str.find("(");
	}
	pos = str.find(")");
	while (pos != string::npos) {
		str.erase(pos, 1);
		pos = str.find(")");
	}
	pos = str.find("'");
	while (pos != string::npos) {
		str.erase(pos, 1);
		pos = str.find("'");
	}
	pos = str.find("&");
	while (pos != string::npos) {
		str.erase(pos, 1);
		pos = str.find("&");
	}
	pos = str.find("/");
	while (pos != string::npos) {
		str.erase(pos, 1);
		pos = str.find("/");
	}
	out = str;
	if (in != out) {
		changed_names.insert(pair<string, string>(out, in));

	}
}
void CommonModelInfo::insert_ChangedNames(vector<pair<string, string>> p)
{
	for (auto it = p.begin(); it != p.end(); it++) {
		string in = (*it).first;
		string out = (*it).second;
		changed_names.insert(pair<string, string>(in, out));

	}
}
/*size_t CommonModelInfo::GetGroupCategory(size_t groupno) {
	auto it = m_GroupCategoryNo.find(groupno);
	if (it != m_GroupCategoryNo.end())
		return it->second;
	else
		return string::npos;
}*/

vector<string> CommonModelInfo::GetSortedListofElements()
{
	return m_ElementSorted;
}
vector<string> CommonModelInfo::GetSortedListofElements(vector<elements> velements) {
	vector<string> out;
	for (elements e : velements) {
		size_t koll = e;
		out.push_back(ModelCompNames::ElementNames[koll]);
	}

	return out;
}
vector<string> CommonModelInfo::GetSortedListofPhysProc()
{
	return m_FysProcessNamesSorted;
}
vector<string> CommonModelInfo::GetSortedListofPhysProc(vector<fysprocess> v_process) {
	vector<string> out;
	for (fysprocess e : v_process) {
		size_t koll = e;
		out.push_back(ModelCompNames::FysProcessNames[koll]);
	}

	return out;
}
vector<string> CommonModelInfo::GetSortedListofBiolProc()
{
	return m_BioProcessNamesSorted;
}
vector<string> CommonModelInfo::GetSortedListofBiolProc(vector<bioprocess> v_process) {
	vector<string> out;
	for (bioprocess e : v_process) {
		size_t koll = e;
		out.push_back(ModelCompNames::BioProcessNames[koll]);
	}

	return out;
}

string CommonModelInfo::GetStringOfElement(elements elem) {
	auto it = m_ElementNamesMap.find(elem);
	if (it != m_ElementNamesMap.end())
		return (*it).second;
	else
		return "";
}
elements CommonModelInfo::GetElementFromString(string str) {
	auto it = m_NamesElementMap.find(str);
	if (it != m_NamesElementMap.end())
		return (*it).second;
	else
		return GENERAL;
}
fysprocess CommonModelInfo::GetPhysProcFromString(string str) {
	auto it = m_NamesFysProcMap.find(str);
	if (it != m_NamesFysProcMap.end())
		return (*it).second;
	else
		return NOFPROC;
}bioprocess CommonModelInfo::GetBioProcFromString(string str) {
	auto it = m_NamesBioProcMap.find(str);
	if (it != m_NamesBioProcMap.end())
		return (*it).second;
	else
		return NOBPROC;
}
vector<SimB*> CommonModelInfo::SortSimB_ByGroup(vector<SimB*> v_in) {
	vector<SimB*> v_out;
	multimap<string, SimB*> SortMap;
/*/	for (size_t i = 0; i<v_in.size(); i++) {
		SortMap.insert(pair<string, SimB*>(v_in[i]->GetGroup(),v_in[i]));
	}*/

	for(auto it=SortMap.begin(); it!=SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
	return v_out;

}
vector<SimB*> CommonModelInfo::SortSimB_ByCategory(vector<SimB*> v_in) {

	vector<SimB*> v_out;
	multimap<int, SimB*> SortMap;
/*	for (size_t i = 0; i<v_in.size(); i++) {
		SortMap.insert(pair<int, SimB*>(GetGroupCategory(v_in[i]->GetGroup()),v_in[i]));
	}*/

	for(auto it=SortMap.begin(); it!=SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
	return v_out;
}
void CommonModelInfo::SetForRunningWithoutFiles(bool status) {
	m_UsePostgresWithOutFiles = status;
}
bool CommonModelInfo::IsRunningWithoutFiles() {
	return m_UsePostgresWithOutFiles;
}

