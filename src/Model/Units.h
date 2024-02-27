#pragma once
#include "../ModelTypes/SimB.h"


struct MODELUNITS {
    enum M_UNITS {};
	string value;
};

class Units {
public:
	Units();
	~Units();
	size_t GetNumUnits() { return m_TypeBasedUnits.size();};
	string GetUnitString(UNIT_TYPES key, bool withparanthesis=false) {
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
	UNIT_TYPES GetUnitType(string key) {
		auto it = m_StringBasedUnits.find(key);
		if (it != m_StringBasedUnits.end())
			return (*it).second;
		else
			return UNIT_TYPES::NO_UNIT;
	};
	vector<string> GetAllStringBasedUnits() {
		vector<string> out;
		for(auto it=m_TypeBasedUnits.begin(); it!=m_TypeBasedUnits.end();++it)  {
			out.push_back((*it).second);
		};
		return out;
	};
	vector<string> GetSelectedStringBasedUnits(vector<UNIT_TYPES> key) {
		vector<string> out;
		for_each(key.begin(), key.end(), [&](UNIT_TYPES& key) {
			auto it = m_TypeBasedUnits.find(key);
			out.push_back((*it).second);
		});
		return out;
	};
private:
	void SetUnitMap(UNIT_TYPES type,string str) {
		m_TypeBasedUnits.insert(pair<UNIT_TYPES, string>(type, str));
		m_StringBasedUnits.insert(pair<string, UNIT_TYPES>(str,type));
	};

	map<enum UNIT_TYPES, string> m_TypeBasedUnits;
	map<string, enum UNIT_TYPES> m_StringBasedUnits;
};



