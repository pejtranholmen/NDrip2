#
#include "../std.h"
#include "./ModelMap.h"
#include "../PG/PGUtil.h"
#include "../ModelTypes/P.h"
#include "../NewBase/CommonModelInfo.h"


ModelMap::ModelMap() {
	
	RunOpt.StartYear=1985;
	RunOpt.StartMonth=5;
	RunOpt.StartDay=1;
	RunOpt.StartHour=0;
	RunOpt.StartMinute=0;
	RunOpt.longMinStart=PGUtil::MinutConv("198505010000");
	RunOpt.longMinPreStart=RunOpt.longMinStart;
	RunOpt.longMinEnd=PGUtil::MinutConv("198505110000");
	RunOpt.longMinPostEnd = RunOpt.longMinEnd;
	RunOpt.EndYear=1985;
	RunOpt.EndMonth=5;
	RunOpt.EndDay=1;
	RunOpt.EndHour=0;
	RunOpt.EndMinute=0;
	RunOpt.oiminutes=60;
	RunOpt.oidays=0;
	RunOpt.noofiter=96;
	RunOpt.TimeResolution=1;
	m_ExternalModelMap = false;
	m_pCommonModelInfo = p_ModelInfo;
};
ModelMap::~ModelMap(){
	SimB* pSimB;
	size_t count = 0;

	NE* pP;

	if (!m_ExternalModelMap) {
		for (auto it = GlobalMap.begin(); it != GlobalMap.end(); ++it) {
			pSimB = (*it).second;
			count++;
			if (count == 343) {
				auto koll = pSimB;
			}
			delete pSimB;
			(*it).second = nullptr;

		}


		count = 0;
		for (auto it = NE_Map.begin(); it != NE_Map.end(); ++it) {
			auto koll = (*it).second;
			pP = (*it).second;
			count++;
			if (koll != nullptr) {
				delete pP;
				(*it).second = nullptr;
			}
		}

	}

	for (auto it = MapErrors.begin(); it != MapErrors.end(); ++it) {
		auto koll = (*it).second;
		count++;

	}
	
};
bool ModelMap::SetModelMap(ModelMap* setMap) {

	GlobalMap = setMap->GlobalMap;
	NE_Map= setMap->NE_Map;
	for (size_t i = 0; i < NUMGROUP; ++i) {
		MapSw[i] = setMap->MapSw[i];
		MapPt[i] = setMap->MapPt[i];
		MapP[i] = setMap->MapP[i];
		MapTab[i] = setMap->MapTab[i];
		MapXs[i] = setMap->MapXs[i];
		MapTs[i] = setMap->MapTs[i];
		MapGs[i] = setMap->MapGs[i];
		MapDs[i] = setMap->MapDs[i];
		MapX[i] = setMap->MapX[i];
		MapT[i] = setMap->MapT[i];
		MapG[i] = setMap->MapG[i];
		MapD[i] = setMap->MapD[i];

		MapF[i] = setMap->MapF[i];
		MapFunc[i] = setMap->MapFunc[i];

	}
	m_ExternalModelMap = true;
	return true;
}
bool ModelMap::IsMapSet() { return m_ExternalModelMap; };

SimB* ModelMap::GetP_Pointer(string name) {
		SimB *pSimB;
		auto in=GlobalMap.count(name);
		if(GlobalMap.count(name)>0) {
			for (auto it=GlobalMap.equal_range(name).first; it!=GlobalMap.equal_range(name).second; ++it) {
				 pSimB=(*it).second;
				 if(pSimB->GetType()==PAR_TABLE) {
					 return pSimB;
				 }
			}
		}
		
		return nullptr;
}
NE* ModelMap::GetNEPointer(string name)
{	if(NE_Map.empty()) return nullptr;
			auto it=NE_Map.find(name);
			if(it!=NE_Map.end())
				return it->second;
			else
				return nullptr; 
};
vector<string> ModelMap::GetNameOfNEPointers() {
	vector<string> out;
	string str;
	for (auto it = NE_Map.begin(); it != NE_Map.end(); ++it) {
		str = it->first;
		out.push_back(str);
	}
	return out;
}
vector<SimB*> ModelMap::GetAll_NEPointers() {
	vector<SimB*> out;
	SimB* pSimB;
	
	for (auto it = NE_Map.begin(); it != NE_Map.end(); ++it) {
		pSimB = static_cast<SimB*>(it->second);
		out.push_back(pSimB);
	}
	return out;
}
vector<string> GetNameOfFuncPointers() {
	vector<string> out;
	string str;
	/*for (auto it = NE_Map.begin(); it != NE_Map.end(); ++it) {
		str = it->first;
		out.push_back(str);
	} */
	return out;






}
Ps* ModelMap::GetPsPointer(string name)
{	SimB *pSim;
		auto in=GlobalMap.count(name);
    
//        for (multimap<string, SimB*>::iterator itr = GlobalMap.begin(); itr != GlobalMap.end(); ) {
//
//            cout << itr->first << "\n";
//            ++itr;
//        }
    
    
		if(GlobalMap.count(name)>0) {
            
            
			for (auto it=GlobalMap.equal_range(name).first; it!=GlobalMap.equal_range(name).second; ++it) {
				 pSim=(*it).second;
				 if(pSim->GetType()==PAR_SINGLE) {
					 return (Ps*)pSim;
				 }
			}
		}
		return nullptr;
}
P* ModelMap::GetPPointer(string name)
{	SimB *pSim;
		auto in=GlobalMap.count(name);
		if(GlobalMap.count(name)>0) {
			for (auto it=GlobalMap.equal_range(name).first; it!=GlobalMap.equal_range(name).second; ++it) {
				 pSim=(*it).second;
				 if(pSim->GetType()==PAR_TABLE) {
					 return (P*)pSim;
				 }
			}
		}
		return nullptr;
}
Func* ModelMap::GetFuncPointer(string name, size_t ModuleNo)
{
	if (MapFunc[ModuleNo] .empty()) return nullptr;
	auto it = MapFunc[ModuleNo].find(name);
	if (it != MapFunc[ModuleNo].end())
		return it->second;
	else
		return nullptr;
}
Func* ModelMap::GetFuncPointer(MyFunc myfunc)
{
	auto it = MapMyFunc.find(myfunc);
	if (it != MapMyFunc.end())
		return it->second;
	else
		return nullptr;
}

Gs* ModelMap::GetGsPointer(string name)
{    SimB *pSim;
    auto in=GlobalMap.count(name);
    if(GlobalMap.count(name)>0) {
        for (auto it=GlobalMap.equal_range(name).first; it!=GlobalMap.equal_range(name).second; ++it) {
            pSim=(*it).second;
            
            cout << pSim->GetType() << "\n";
            //            if(pSim->GetType()==PAR_TABLE) {
            return (Gs*)pSim;
            //            }
        }
    }
    return nullptr;
}
