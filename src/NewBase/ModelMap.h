#pragma once
#include "./CommonModelInfo.h"
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
    vector<float> val;
};

//#include "../ModelTypes/FunctFactory.h"
enum class  MyFunc;

class ModelMap  {
	public:
	ModelMap(void);	
	virtual ~ModelMap();
	bool SetModelMap(ModelMap* setmap);

	R RunOpt;
	multimap<string, SimB*> GlobalMap;
	map<string, NE*> NE_Map;
	map<string, Sw*> MapSw[NUMGROUP];
	map<string, Ps*> MapP[NUMGROUP];
	map<string, P*>  MapPt[NUMGROUP];
	map<string, Tab*> MapTab[NUMGROUP];
	map<string, OutSingle*> MapXs[NUMGROUP], MapTs[NUMGROUP], MapGs[NUMGROUP], MapDs[NUMGROUP];
	map<string, OutVector*> MapX[NUMGROUP], MapT[NUMGROUP], MapG[NUMGROUP], MapD[NUMGROUP];
	map<string, F*> MapF[NUMGROUP];
	map<string, FCSV*>  MapCSV;
	map<string, Func*> MapFunc[NUMGROUP];
	map<enum   MyFunc, Func*> MapMyFunc;
	map<int, Func*> MapPhysFunc;

	map<string, int> MapErrors;

	map<size_t, size_t> MapFlexVal[8];
	map<string, size_t> MapFlexId[8];

	NE* GetNEPointer(string name); 
	vector<SimB*> GetAll_NEPointers();

	vector<string> GetNameOfNEPointers();
	vector<string> GetNameOfFuncPointers();
	Func* GetFuncPointer(string name, size_t ModuleNo);
	Func* GetFuncPointer(MyFunc);
	SimB* GetP_Pointer(string name);
	Ps* GetPsPointer(string name);
	P* GetPPointer(string name);
    Gs* GetGsPointer(string name);
	bool IsMapSet();      
	CommonModelInfo* m_pCommonModelInfo;

private:
	bool m_ExternalModelMap;
};
