#pragma once

#include "./SimB.h"
#include "../PG/PG.H"
#include "../std.h"
#include "./FunctFactory.h"
class WaterUptake_Functions;
class SimModule;
class FunctionProperties;
class ModelMap;
struct HYDRAULIC_COEF;
struct CONDUCTIVITY_COMMON_PAR;



class Func :public SimB {

public:
	//Func(double (WaterUptake_Functions::*fpoint)(double), WaterUptake_Functions*, enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev);
	Func(enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev);
	Func(definitions def);
	~Func(void);
	void Def(int ModuleNo,size_t opt, size_t ftype,SimModule *mClass,string name, FunctionProperties* pFunc, ModelMap *pModelMap);
	void Def(FunctorProp prop, SimModule *mClass, string name, FunctionProperties* pFunc, ModelMap *pModelMap);
	void Def(function<double(double)> *pFunctor, string name, FunctionProperties* pFunc, ModelMap *pModelMap);
	void Def(FunctorProp prop, string name, vector<Ps*> Ps_Vector,  FunctionProperties* pFunctions, ModelMap* pModelMap);
	void Def(FunctorProp prop, string name, vector<Ps*> Ps_Vector, vector<P*> P_Vector, FunctionProperties* pFunctions, ModelMap* pModelMap);
	void Def(FunctorProp prop, string name, vector<P*> P_Vector, FunctionProperties* pFunctions, ModelMap* pModelMap);
	void Def(FunctorProp prop, string name, HYDRAULIC_COEF *pHyd, vector<P*> P_Vector, FunctionProperties* pFunctions, ModelMap* pModelMap);
	void SetPlotParam(PLOT_PARAM param) {m_PlotParam=param;};
	void ResizeDoubleParVector(vector<double> *pDouble, size_t NewSize);

	bool   MakeOperation();
	double GetValue(double x, bool check=false);
	double GetValue(double x, double y);
	double GetValue(double x, double y, double z);
	double GetValue(double x, double y, double z, double zz);
	double GetValue(size_t);
	double GetValue(size_t, double x);
	double GetValue(size_t, double x, double y);
	double GetValue(size_t, double x, double y, double z, bool check=false);
	double GetValue(size_t, double x, double y, double z, double zz);
	
	size_t GetFuncType() const {return m_FuncType;};
	function<double(bool)> Get_boolBlank_Functor();
	function<vector<double>()> GetVector_Blank_Functor();
	function<vector<double>(bool)> GetVector_boolBlank_Functor();
	function<double(double)> Get_D1_Functor();
	function<double(double,double)> Get_D2_Functor();
	function<double(double,double, double)> Get_D3_Functor();
	function<double(double,double, double, double)> Get_D4_Functor();
	function<double(double, double, double, double, double)> Get_D5_Functor();
	function<double(bool, size_t)> Get_boolBlank_TableFunctor();
	function<double(bool, size_t, double, double, double)> Get_boolD3_TableFunctor();
	function<double(bool, size_t, double, double, double, double)> Get_boolD4_TableFunctor();
	function<double(size_t, double)> Get_D1_TableFunctor();
	function<double(size_t, double, double)> Get_D2_TableFunctor();
	function<double(size_t, double, double, double)> Get_D3_TableFunctor();
	function<double(size_t, double, double, double, double)> Get_D4_TableFunctor();
	function<double(size_t, size_t, double, double, double)> Get_T_D3_TableFunctor();

	function<void(vector<double>*)>  GetVectorPointer_D1_Functor();
	function<void(vector<double>*, vector<double>*)>  GetVectorPointer_D2_Functor();
	function<void(vector<double>*, vector<double>*, vector<double>*)>  GetVectorPointer_D3_Functor();
	function<void(vector<double>*, vector<double>*, vector<double>*, vector<double>*)> GetVectorPointer_D4_Functor();
	function<void(vector<double>*, vector<double>*, vector<double>*, vector<double>*, vector<double>*)> GetVectorPointer_D5_Functor();
	function<OrgTurnOver(double, double, double)> Get_OrgTurnOverFunctor();
	function<OrgTurnOver(double, double, double, double, double)> Get_D5_OrgTurnOverFunctor();
	function < pair<double, double>( double, double)> Get_Pair_D2Functor();
	function < pair<double, double>(size_t, double, double)> Get_Pair_D2_TableFunctor();
	function < pair<double, double>(double, double, double)> Get_Pair_D3Functor();
	function < pair<double, double>(size_t, double, double, double)> Get_Pair_D3_TableFunctor();



    enum MyArgType GetArgumentTypes();
	bool IsTableType() { return m_TableType; };



	PLOT_PARAM GetPlotParam() {return m_PlotParam;};
	Hydraulic* Get_PointerHydraulic() { return m_FunctorProp.pHyd; };
	MyFunc Get_FunctorName() { return m_FunctorProp.FuncName; };

	double GetXMin() {return m_PlotParam.X_Min;};
	double GetXMax() {return m_PlotParam.X_Max;};
	string GetXLabel() {return m_PlotParam.X_Label;};
	string GetYLabel() {return m_PlotParam.Y_Label;};
	bool   GetLogScale() {return m_PlotParam.LogYScale;};
	bool   GetLogXScale() {return m_PlotParam.LogXScale;};
    void   SetFunctor(function<bool()> functor) { m_pBoolFunctor = functor; };
	void   SetFunctor(function<bool(bool)> functor) { m_D1Functor= functor; };
	bool   MakeBool();
	bool m_FunctorFunctions;
	vector<string> GetParDep_Ps_Names() { vector<string> out; 
		for (size_t i = 0; i < m_Ps_Vector.size(); i++) out.push_back(((SimB*)m_Ps_Vector[i])->GetName());
		return out;
	};
	vector<string> GetParDep_P_Names() {
		vector<string> out;
		for (size_t i = 0; i<m_P_vector.size(); i++) out.push_back(((SimB*)m_P_vector[i])->GetName());
		return out;
	};
	double Get(double x);
	double Get(size_t, double x);
	
protected:
	void UpdateP_Par();
	void UpdatePs_Par();
	size_t m_FuncType;
	size_t m_Opt;
	size_t m_Index;
	SimModule *m_Class;
	PLOT_PARAM m_PlotParam;
	function<bool()> m_pBoolFunctor;
	function<double(double)> m_D1Functor;
	function<double(double)> *m_pD1Functor;

	function<double(double)> *m_pFunctor;
	vector<Ps*> m_Ps_Vector;
	vector<P*> m_P_vector;
	vector<double> m_FunctorPs_Par;
	vector< vector<double>> m_FunctorP_Par;
	FunctorProp m_FunctorProp;
	bool m_TableType;
	HYDRAULIC_COEF *m_pHydraulic_Coef;
	CONDUCTIVITY_COMMON_PAR *m_pCondCommonPar;



};
