#pragma once




#include "../NewBase/PhysFunc.h"
#include "../ModelTypes/NewModelType.h"

#include "./GlobalInfo.h"
#include "../Simulator/ModelInfo.h"
#include "../NewBase/ModelMap.h"
#include "../Util/StatUtil.h"
#include "./Units.h"
using namespace StatUtil;
using namespace ModelFunctions;

struct DRIVERSOnGPP {
	vector<double> Light;
	vector<double> Temp;
	vector<double> Water;
	vector<double> Nitrogen;
	vector<double> Phosphorus;
	vector<double> Nutrient_Water;
	vector<double> Salt;
	vector<double> AllCombined;
};
struct DemandUptakeSoil {
	vector<double> Demand;
	vector<double> DemandSoil;
	vector<double> MinUptake;
	vector<double> OrgUptake;
	double TotalDemand;
	double TotalDemandSoil;
};
struct Soil_Org_Ini {
	vector<double> Layer_Mass;
	vector<double> Layer_Conc;
	double Tot_Profile;
	double Depth;
	double FracExpTail;
};

struct FLUX {
	vector<double> Conc;
	vector<double> HFlow;
	vector<double> VFlow;
	double	TotHFlow;
	double  TotHFlowConc;
	double  TotSourceFlow;
};
struct Dec_Response {
	vector<double> Temp;
	vector<double> Theta;
	vector<double> Temp_Theta;
	double Litter;
	double Humus;
};

struct ININFO
{
	size_t ivar;
	CPG *pgfile;
	bool discrete;
	int shift;
};

class Structure;
class ModelInfo;
class SimModule {

public:
	~SimModule();
	SimModule();

	void Define(Ps*,  string Name, double value, string unit, string conditions, double error_min, double error_max, double warn_min=float(MISSING), double warn_max=float(MISSING), int recalc=0, int UserLevel=0, int view=0);
	void Define(Ps*, string Name, double value, UNIT_TYPES, string conditions, double error_min, double error_max, double warn_min = float(MISSING), double warn_max = float(MISSING), int recalc = 0, int UserLevel = 0, int view = 0);

	void Define(Xs*,string Name, double value, string unit, string conditions="");
	void Define(Xs*, string Name, double value, UNIT_TYPES, string conditions = "");
	void Define(Ts*,string Name, double value, string unit, string conditions="");
	void Define(Ts*, string Name, double value, UNIT_TYPES, string conditions = "");
	void Define(Gs*,string Name, double value, string unit, string conditions="");
	void Define(Gs*, string Name, double value, UNIT_TYPES, string conditions = "");
	void Define(Ds*,string Name, double value, string unit, string conditions="");
	void Define(Ds*, string Name, double value, UNIT_TYPES, string conditions = "");

//	void SetPtr2Module(GlobalInfo *pGlob, ModelInfo *pModel, ModelMap *pMap) {pSub=pGlob; m_pModelInfo=pModel;m_pModelMap=pMap; pT_Step=&pModel->T_Step;T_Step=*pT_Step;}; 
 
	void Define(Sw*,string Name, int ,string options, string condition="", int recalc=0);
	void Define(NE*, string Name);

	void Define(P*, NE*, string Name, vector<double>, string unit, string conditions = "", double error_min = float(MISSING), double error_max = float(MISSING), double warn_min = float(MISSING), double warn_max = float(MISSING), int recalc = 0, int views = 0, int view = 0);
	void Define(P*, NE*, string Name, vector<double>, UNIT_TYPES, string conditions = "", double error_min = float(MISSING), double error_max = float(MISSING), double warn_min = float(MISSING), double warn_max = float(MISSING), int recalc = 0, int views = 0, int view = 0);
	

	void Define(Tab*, string Name, size_t minPtoEnable);
	void Define(Tab* pTab, string Name, string pname, string cond, size_t min_valid=1) { pTab->SetConditions(cond); Define(pTab, Name, min_valid);};
	
	void Define(Func* ptr,  FunctorProp myf, SimModule *mClass, string name, PLOT_PARAM plotpar);
	void Define(Func* ptr, FunctorProp myf, SimModule *mClass, string name);
	void Define(Func* ptr, FunctorProp myf, string name);
	void Define(Func* ptr, FunctorProp myf, string name,  vector<Ps*> PsV);
	
	void Define(Func* ptr, FunctorProp myf, string name, vector<Ps*> PsV, PLOT_PARAM plotpar);
	void Define(Func* ptr, FunctorProp myf, string name, vector<Ps*> PsV, vector<P*> PV, PLOT_PARAM plotpar);
	void Define(Func* ptr, FunctorProp myf, string name,  vector<P*> PsV);
	void Define(Func* ptr, FunctorProp myf, string name, vector<P*> PsV, PLOT_PARAM plotpar);

	void Define(X*, NE* pne, string Name, vector<double> value, string unit, string conditions="");
	void Define(X*, NE* pne, string Name, vector<double> value, UNIT_TYPES, string conditions = "");

	void Define(T*, NE* pne, string Name, vector<double> value, string unit, string conditions="");
	void Define(T*, NE* pne, string Name, vector<double> value, UNIT_TYPES, string conditions = "");

	void Define(G*,NE* pNE, string Name, vector<double> value, string unit, string conditions="");
	void Define(G*, NE* pNE, string Name, vector<double> value, UNIT_TYPES, string conditions = "");

	void Define(D*,NE* pNE, string Name, vector<double> value, string unit, string conditions="");
	void Define(D*, NE* pNE, string Name, vector<double> value, UNIT_TYPES, string conditions = "");
	
	
	
	
	void Define(F*, string Name, string FileName, string conditions="");
	void Define(FCSV*, string Name, string FileName, string conditions = "", size_t NumHeaders=1);


	void SetTimeStep(double tstep) { T_Step=tstep;};
	void SetRatioToGlobalStep(size_t ratio) {Ratio_ToGlobalTimeStep=ratio;};

	NE* GetNEPointer(string Name) {return m_pModelMap->GetNEPointer(Name);};


	bool NumTest(const double &T) {
		if (T > -1.E20&&T < 1.E20) return true;
		else {
			Error();
			return false;
		}
	};



	void InFlow(vector<double> &X, const vector<double> &T) { 
		for (size_t i = 0; i < X.size(); i++) {
			if(NumTest(T[i]))	X[i] += T[i] * T_Step;
		}
	};
	void InFlow(double &X, const double &T) { 
		if(NumTest(T)) X+= T*T_Step;};
	
	void OutFlow(double &X, const double &T) {/*if(NumTest(T))*/ X -= T*T_Step; };
	void OutFlow(vector<double> &X, const vector<double> &T) { 
		for (size_t i = 0; i < X.size(); i++) {
			 if(NumTest(T[i]))	X[i] -= T[i] * T_Step;

		}
	};

	
	void IntFlow(vector<double> &X, const vector<double> &T) { for (size_t i = 1; i<X.size(); i++) { X[i - 1] -= T[i - 1]*T_Step; X[i] += T[i - 1]*T_Step; } };

	void TransFlow(vector<double> &X_out,vector<double> &X_in, const vector<double> &T)
	{for(size_t i=0; i<X_out.size(); i++) {X_out[i]-=T[i]*T_Step; X_in[i]+=T[i]*T_Step;} };
	void TrIntFlow(vector<double> &X_out,vector<double> &X_in, const vector<double> &T)
	{for(size_t i=1; i<X_out.size(); i++) {X_out[i-1]-=T[i-1]*T_Step; X_in[i]+=T[i-1]*T_Step;} };


	double GetEcoDat(PGPOS *pos, ININFO, size_t AdjustPosIvar=0);
	vector<double> GetEcoDat(PGPOS *pos, size_t NVar, ININFO);
	void Message(string str);
	int Error();

	function<double(double)> Get_D1_PhysFunctor(enum  FUNC_CHOICE);
	function<double(double, double)> Get_D2_PhysFunctor(enum  FUNC_CHOICE);
	function<double(double, double, double)> Get_D3_PhysFunctor(enum  FUNC_CHOICE);
	function<double(double, double, double, double)> Get_D4_PhysFunctor(enum  FUNC_CHOICE);
	function<double(double, double, double, double, double)> Get_D5_PhysFunctor(enum  FUNC_CHOICE);

	function<double(bool)> Get_boolBlank_Functor(enum  MyFunc);
	function<vector<double>()> GetVector_Blank_Functor(enum  MyFunc);
	function<vector<double>(bool)> GetVector_boolBlank_Functor(enum  MyFunc);

	function<double(double)> Get_D1_Functor(enum  MyFunc);
	function<double(double, double)> Get_D2_Functor(enum  MyFunc);
	function<double(double, double, double)> Get_D3_Functor(enum  MyFunc);
	function<double(double, double, double, double)> Get_D4_Functor(enum  MyFunc);
	function<double(double, double, double, double, double)> Get_D5_Functor(enum  MyFunc);

	function<void(vector<double>*)>  GetVectorPointer_D1_Functor(enum  MyFunc);
	function<void(vector<double>*, vector<double>*)>  GetVectorPointer_D2_Functor(enum  MyFunc);
	function<void(vector<double>*, vector<double>*, vector<double>*)>  GetVectorPointer_D3_Functor(enum  MyFunc);
	function<void(vector<double>*, vector<double>*, vector<double>*, vector<double>*)> GetVectorPointer_D4_Functor(enum  MyFunc);
	function<void(vector<double>*, vector<double>*, vector<double>*, vector<double>*, vector<double>*)> GetVectorPointer_D5_Functor(enum  MyFunc);


	function<double(bool, size_t)> Get_boolBlank_TableFunctor(enum  MyFunc);

	function<double(bool, size_t, double, double, double, double)> Get_boolD4_TableFunctor(enum  MyFunc);
	function<double(bool, size_t, double, double, double)> Get_boolD3_TableFunctor(enum  MyFunc);



	function<double(size_t, double)> Get_D1_TableFunctor(enum  MyFunc);
	function<double(size_t, double, double)> Get_D2_TableFunctor(enum  MyFunc);
	function<double(size_t, double, double, double)> Get_D3_TableFunctor(enum  MyFunc);
	function<double(size_t, double, double, double, double)> Get_D4_TableFunctor(enum  MyFunc);

	function<OrgTurnOver(double, double, double)> Get_OrgTurnOverFunctor(enum  MyFunc);
	function<OrgTurnOver(double, double, double, double, double)> Get_D5_OrgTurnOverFunctor(enum  MyFunc);

	//void Define(Functions*, string Name, string unit);

	GlobalInfo* pSub;
	double T_Step; //Time Step of the particular module
	
protected:


	string ModuleName, ModuleConditions;
	size_t ModuleNo;
	

	Structure*  m_pModelStructure;
	ModelMap* m_pModelMap;

	double *pT_Step; // Pointer to Global Time Step of Model
	size_t Ratio_ToGlobalTimeStep; 
	size_t Count_Call;
	size_t m_NumErrors;
	ModelInfo* m_pModelInfo;
private:
	size_t NE_elem[10];
	
};
