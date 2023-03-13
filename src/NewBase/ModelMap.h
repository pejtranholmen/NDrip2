#pragma once
#include "./CommonModelInfo.h"

#include "../Util/Register.h"
#include "../SoilData/GetSoilStorage.h"


static CommonModelInfo modelinfo;
static CommonModelInfo* p_ModelInfo = &modelinfo;





static GetSoilStorage getsoilstorage;
static GetSoilStorage* pGetSoil = &getsoilstorage;
class Sim_Xml;


struct R{
	int TimeResolution;
  	int	oiminutes;
	int	oidays;
	int	StartYear;	
	int	StartMonth  ;
	int	StartDay     ;
	int	StartHour    ;
	int	StartMinute  ;
	unsigned int longMinStart ;
	unsigned int longMinPreStart;
	unsigned int longMinPostEnd;
	int	EndYear      ;
	int	EndMonth     ;
	int	EndDay       ;
	int	EndHour      ;	
	int	EndMinute 	 ;
	unsigned int longMinEnd;
	int	runno;
	int	noofiter;
};	

#define MAXPGDOC 16
#define MAXSIMVAL 16
#define MAXCSVFILES 4;
const int NUMGROUP=38;
class SimB;
class NE;
class Sw;
class Ps;
class P;
class Gs;
class Tab;
class OutSingle;
class OutVector;
class F;
class Func;
class FCSV;
class FlexVal{
public:
    std::vector<float> val;
};

//#include "../ModelTypes/FunctFactory.h"
enum class  MyFunc;

class ModelMap  {
	public:
	ModelMap(void);	
	virtual ~ModelMap();
	bool SetModelMap(ModelMap* setmap);

	R RunOpt;
	std::multimap<std::string, SimB*> GlobalMap;
	std::map<std::string, NE*> NE_Map;
	std::map<std::string, Sw*> MapSw[NUMGROUP];
	std::map<std::string, Ps*> MapP[NUMGROUP];
	std::map<std::string, P*>  MapPt[NUMGROUP];
	std::map<std::string, Tab*> MapTab[NUMGROUP];
	std::map<std::string, OutSingle*> MapXs[NUMGROUP], MapTs[NUMGROUP], MapGs[NUMGROUP], MapDs[NUMGROUP];
	std::map<std::string, OutVector*> MapX[NUMGROUP], MapT[NUMGROUP], MapG[NUMGROUP], MapD[NUMGROUP];
	std::map<std::string, F*> MapF[NUMGROUP];
	std::map<std::string, FCSV*>  MapCSV;
	std::map<std::string, Func*> MapFunc[NUMGROUP];
	std::map<enum   MyFunc, Func*> MapMyFunc;
	std::map<int, Func*> MapPhysFunc;

	std::map<std::string, int> MapErrors;

	std::map<size_t, size_t> MapFlexVal[8];
	std::map<std::string, size_t> MapFlexId[8];

	NE* GetNEPointer(std::string name); 
	std::vector<SimB*> GetAll_NEPointers();

	std::vector<std::string> GetNameOfNEPointers();
	std::vector<std::string> GetNameOfFuncPointers();
	Func* GetFuncPointer(std::string name, size_t ModuleNo);
	Func* GetFuncPointer(MyFunc);
	SimB* GetP_Pointer(std::string name);
	Ps* GetPsPointer(std::string name);
	P* GetPPointer(std::string name);
    Gs* GetGsPointer(std::string name);
	bool IsMapSet();      
	CommonModelInfo* m_pCommonModelInfo;

private:
	bool m_ExternalModelMap;
};
