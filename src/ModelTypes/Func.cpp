
#include "../Model/FunctionProperties.h"
#include "./FunctFactory.h"

#include "Func.h"
//#include "../Model/PLANT/WaterUptake_Functions.h"
#include "../Model/PLANT/Potential_Transpiration.h"
#include "../Model/Atmosphere/SoilEvaporation_Functions.h"
#include "../Model/SOIL_NC/AbioticResponses.h"
#include "../Model/SOIL/Thermal_Functions.h"
#include "../Model/SOIL_NC/NC_Organic_Functions.h"
#include "../Model/SOIL_NC/NC_GasFlux_Functions.h"
#include "../Model/HBV/HBV_Functions.h"
#include "../Model/SOIL/Hydraulic_Functions.h"

#include "../NewBase/ModelMap.h"

#include "../Model/SOIL/Functors/HydFunctors.h"



Func::~Func()
{
	m_FuncType=0;
}
//Func::Func(double (WaterUptake_Functions::*fpoint)(double),WaterUptake_Functions *mClass,  enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev)
Func::Func(enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev)

	:SimB(FUNCTION, data, elem, fproc, bproc, ulev)
{
	m_Class = nullptr;
	m_FuncType = 0;
	//double res;
	//res=(mClass->*fpoint)(3.);
	//SetPointer(ptr);
}
void Func::Def(int ModuleNo,size_t opt, size_t ftype,SimModule *mClass, string name, FunctionProperties* pFunctions, ModelMap* pModelMap)
{
	m_Index=pFunctions->GetIndex(name);
	PLOT_PARAM_DEP p;
	p=pFunctions->GetParamDep(m_Index);
	m_PlotParam=pFunctions->GetParam(m_Index);
	SetModuleNo(ModuleNo);
	SetName(name);
	m_FuncType=ftype;
	m_Opt=opt;

	m_Class=mClass;
	m_FunctorFunctions = false;
	//SetPointer(ptr);

	for(size_t i=0; i<p.Ps_Names.size(); i++) {	
		Ps* ptr=pModelMap->GetPsPointer(p.Ps_Names[i]);
		if(ptr!=nullptr)
			ptr->AddFuncPointer(this);
	}
	for(size_t i=0; i<p.P_Names.size(); i++) {
		P* ptr=pModelMap->GetPPointer(p.P_Names[i]);
		if(ptr!=nullptr)
			ptr->AddFuncPointer(this);

	}

}
void Func::Def(FunctorProp myf, SimModule *mClass, string name, FunctionProperties* pFunctions, ModelMap* pModelMap)
{
	m_Index = pFunctions->GetIndex(name);
	PLOT_PARAM_DEP p;
	p = pFunctions->GetParamDep(m_Index);
	m_PlotParam = pFunctions->GetParam(m_Index);
	SetModuleNo(0);
	SetName(name);
	m_FuncType = size_t(myf.Arg);
	m_Opt = 0;
	m_FunctorProp = myf;
	SetName(name);
	m_Class = mClass;
	m_FunctorFunctions = false;

	for (size_t i = 0; i<p.Ps_Names.size(); i++) {
		Ps* ptr = pModelMap->GetPsPointer(p.Ps_Names[i]);
		if (ptr != nullptr)
			ptr->AddFuncPointer(this);
	}
	for (size_t i = 0; i<p.P_Names.size(); i++) {
		P* ptr = pModelMap->GetPPointer(p.P_Names[i]);
		if (ptr != nullptr)
			ptr->AddFuncPointer(this);

	}

}
void Func::Def(FunctorProp myf, string name, vector<Ps*> Ps_Vector, FunctionProperties* pFunctions, ModelMap* pModelMap)
{
	m_Index = pFunctions->GetIndex(name);

	PLOT_PARAM_DEP p;
	if (m_Index != string::npos) {
		p = pFunctions->GetParamDep(m_Index);
		m_PlotParam = pFunctions->GetParam(m_Index);
	}
	else {
		m_PlotParam.Num_Of_Curves = 1;
		m_PlotParam.Num_Of_Points = 100;
		m_PlotParam.Num_Of_X = 1;
		m_PlotParam.X_Min = 0;
		m_PlotParam.X_Max = 100;
		m_PlotParam.X_Label = "X";
		m_PlotParam.Y_Label = "Y";
		m_PlotParam.LogXScale = false;
		m_PlotParam.LogYScale = false;
		m_PlotParam.Num_Of_Opt = 1;
		m_PlotParam.X2_Vec.assign(100, 100.);
	}
	auto SetPar = [&]() {
		m_FunctorPs_Par.clear();
		for (size_t i = 0; i < Ps_Vector.size(); i++) 
			m_FunctorPs_Par.push_back(((Ps*)Ps_Vector[i])->GetValue());
	};

	SetPar();
	m_Ps_Vector = Ps_Vector;
	SetModuleNo(0);
	SetName(name);
	m_FuncType = 0;
	m_FuncType = size_t(myf.Arg);

	m_Opt = 0;
	m_Class = nullptr;

	FunctFactory(myf, m_FunctorPs_Par);
	m_FunctorProp = myf;
	m_TableType = false;
	m_FunctorFunctions = true;
	for (size_t i = 0; i<Ps_Vector.size(); i++) {
		Ps* ptr = Ps_Vector[i];
		if (ptr != nullptr)
			ptr->AddFuncPointer(this);
	}
	if (m_Index != string::npos) pFunctions->SetParamDep(Ps_Vector, m_Index);
	

}
void Func::Def(FunctorProp myf, string name, vector<Ps*> Ps_Vector, vector<P*> P_Vector, FunctionProperties* pFunctions, ModelMap* pModelMap)
{
	m_Index = pFunctions->GetIndex(name);


	auto SetPar = [&]() {
		m_FunctorPs_Par.clear();
		for (size_t i = 0; i < Ps_Vector.size(); i++)
			m_FunctorPs_Par.push_back(((Ps*)Ps_Vector[i])->GetValue());
		m_FunctorP_Par.clear();
		for (size_t i = 0; i < P_Vector.size(); i++) {
			vector<double> p;
			for (size_t j = 0; j < P_Vector[i]->GetSize(); j++)
				p.push_back(((P*)P_Vector[i])->GetValue(j));
			m_FunctorP_Par.push_back(p);
		}


	};

	SetPar();
	m_Ps_Vector = Ps_Vector;
	m_P_vector = P_Vector;
	SetModuleNo(0);
	SetName(name);
	m_FuncType = size_t(myf.Arg);
	m_Opt = 0;
	m_Class = nullptr;

	FunctFactory(myf, m_FunctorPs_Par, m_FunctorP_Par);
	m_FunctorProp = myf;
	m_TableType = false;
	if (P_Vector.size() > 0) {
		if(myf.Arg!=MyArgType::D1&& myf.Arg != MyArgType::D2) m_TableType = true;
		FunctFactory(myf, m_FunctorPs_Par, m_FunctorP_Par);
	}
	else {
		if(myf.Arg==MyArgType::T_D3|| myf.Arg == MyArgType::T_D2||myf.Arg == MyArgType::T_D1) m_TableType = true;
		FunctFactory(myf, m_FunctorPs_Par);

	}

	m_FunctorFunctions = true;
	if (myf.Set_PlotPar) {
		for (size_t i = 0; i < Ps_Vector.size(); i++) {
			Ps* ptr = Ps_Vector[i];
			if (ptr != nullptr)
				ptr->AddFuncPointer(this);
		}

		m_FunctorFunctions = true;

		for (size_t i = 0; i < P_Vector.size(); i++) {
			if (P_Vector[i] != nullptr)
				P_Vector[i]->AddFuncPointer(this);
		}
	}
	

	if (m_Index != string::npos) pFunctions->SetParamDep(Ps_Vector,P_Vector, m_Index);

}
void Func::Def(FunctorProp myf, string name,  vector<P*> P_Vector, FunctionProperties* pFunctions, ModelMap* pModelMap)
{
	m_Index = pFunctions->GetIndex(name);

	PLOT_PARAM_DEP p;
	p = pFunctions->GetParamDep(m_Index);
	m_PlotParam = pFunctions->GetParam(m_Index);

	SetModuleNo(0);
	SetName(name);
	m_FuncType = size_t(myf.Arg) ;
	m_Opt = 0;
	auto SetPar = [&]() {
		m_FunctorP_Par.clear();
		for (size_t i = 0; i < P_Vector.size(); i++) {
			vector<double> p;
			for (size_t j = 0; j < P_Vector[i]->GetSize(); j++)
				p.push_back(((P*)P_Vector[i])->GetValue(j));
			m_FunctorP_Par.push_back(p);
		}
	};


	SetPar();


	FunctFactory(myf, m_FunctorP_Par);
	m_FunctorProp = myf;

	m_P_vector = P_Vector;
	m_TableType = true;
	m_FunctorFunctions = true;

	for (size_t i = 0; i<P_Vector.size(); i++) {
		if (P_Vector[i] != nullptr)
			P_Vector[i]->AddFuncPointer(this);
	}
	if (m_Index != string::npos) pFunctions->SetParamDep(P_Vector, m_Index);
	
}
void Func::Def(FunctorProp myf, string name, HYDRAULIC_COEF *pProp, vector<P*> P_Vector, FunctionProperties* pFunctions, ModelMap* pModelMap)
{
	m_Index = pFunctions->GetIndex(name);

	PLOT_PARAM_DEP p;
	p = pFunctions->GetParamDep(m_Index);
	m_PlotParam = pFunctions->GetParam(m_Index);

	SetModuleNo(0);
	SetName(name);
	m_FuncType = size_t(myf.Arg);
	m_Opt = 0;
//	Functp(myf, Par);
	m_FunctorProp = myf;
	m_pHydraulic_Coef = pProp;
	m_TableType = true;
	m_FunctorFunctions = true;
	m_P_vector = P_Vector;

	for (size_t i = 0; i<P_Vector.size(); i++) {
		if (P_Vector[i] != nullptr)
			P_Vector[i]->AddFuncPointer(this);
	}

}

void Func::ResizeDoubleParVector(vector<double> *pFirst, size_t NewSize)
{

	for(size_t i=0; i<m_FunctorP_Par.size(); i++) {

	/*	if (pFirst == m_FunctorP_Par[i][0]) {
			auto prevSize = m_FunctorP_Par[i].size();

			m_FunctorP_Par[i].resize(NewSize);
			if (NewSize > prevSize) {
				for (size_t jj = prevSize; jj < NewSize; jj++) {
					m_FunctorP_Par[i][jj] = m_FunctorP_Par[i][prevSize - 1] + jj + 1 - prevSize;
				}
			}
		}
		*/

	}
}
bool Func::MakeBool() {
	return m_pBoolFunctor();
}
bool Func::MakeOperation() {
	switch (m_FunctorProp.FuncName) {
	case MyFunc::PROFILE_RECALC:
		return ((Profile_Functions*)m_Class)->Profile_ReCalc();
	case MyFunc::HYDRAULIC_RECALC:
		return ((Hydraulic_Functions*)m_Class)->ReCalc();
	case MyFunc::BALLAND_ARP_RECALC:
		return ((Thermal_Functions*)m_Class)->Thermal_ReCalc();
	case MyFunc::DRAINAGE_RECALC:
		return ((Soil_Drainage_Functions*)m_Class)->SetDrainageFunctors();

	}

	
	switch(GetGroupNo()) {

		//case 4:	return ((Profile_Functions*)m_Class)->GetFunctionValue(m_Opt);
		//case 5:	return ((Hydraulic_Functions*)m_Class)->GetFunctionValue(m_Opt);
	//	case 6:	return ((Thermal_Functions*)m_Class)->GetFunctionValue(m_Opt);
	//	case 12: return ((Radiation_Functions*)m_Class)->GetFunctionValue(m_Opt);
		//case 25: return ((NC_Organic_Functions*)m_Class)->GetFunctionValue(m_Opt);

	}
	return false;
}
double Func::GetValue( double x1, bool check) {

	switch (m_FunctorProp.FuncName) {
	case MyFunc::HYDRAULIC_CONDTEMPFACTOR:
		return ((Hydraulic_Functions*)m_Class)->f_CondTempFactor(x1);
	case MyFunc::DRAINAGE_EMPIRICAL:
		return ((Soil_Drainage_Functions*)m_Class)->f_EmpiricalDrainage(x1);
	case MyFunc::DRAINAGE_SEEPAGE:
		return ((Soil_Drainage_Functions*)m_Class)->f_SeepageFlow(x1);
	case MyFunc::DRAINAGE_DITCHLEVEL:
		return ((Soil_Drainage_Functions*)m_Class)->f_DitchLevel(x1);
	case MyFunc::DRAINAGE_PHYSICAL:
		return ((Soil_Drainage_Functions*)m_Class)->f_PhysicalDrainage(x1);

	}

	if(m_FuncType>=100)
	//	return ((SimModule*)m_Class)->GetPhysFunctionValue(m_Opt, x1);
	switch(GetGroupNo()) {
	//case 5:	return ((Hydraulic_Functions*)m_Class)->GetFunctionValue(m_Opt, x1);
	//case 6:	return ((Thermal_Functions*)m_Class)->GetFunctionValue(m_Opt, x1);
	//case 7: return ((Meteorological_Data_Functions*)m_Class)->GetFunctionValue(m_Opt, x1);
//	case 12: return ((Radiation_Functions*)m_Class)->GetFunctionValue(m_Opt, x1);
//	case 9: return ((Plant_Functions*)m_Class)->GetFunctionValue(m_Opt, x1);
//	case 10: return ((Interception_Functions*)m_Class)->GetFunctionValue(m_Opt, x1);
//	case 11: return ((Potential_Transpiration_Functions*)m_Class)->GetFunctionValue(m_Opt, x1);
	//case 13: return ((SoilEvaporation_Functions*)m_Class)->GetFunctionValue(m_Opt, x1);
	//case 14: return ((SnowPack_Functions*)m_Class)->GetFunctionValue(m_Opt, x1);
//	case 15: return ((Soil_Frost_Functions*)m_Class)->GetFunctionValue(m_Opt, x1);
//	case 19: return ((Surface_Water_Functions*)m_Class)->GetFunctionValue(m_Opt, x1);
//	case 20: return ((WaterUptake_Functions*)m_Class)->GetFunctionValue(m_Opt, x1);
	//case 21: return ((Soil_Drainage_Functions*)m_Class)->GetFunctionValue(m_Opt, x1, check);
	//case 24: return ((NC_Plant_Functions*)m_Class)->GetFunctionValue(m_Opt, x1, check);
	//case 25: return ((NC_Organic_Functions*)m_Class)->GetFunctionValue(m_Opt, x1);
//	case 28: return ((AbioticResponses_Functions*)m_Class)->GetFunctionValue(m_Opt, x1);
	//case 32: return ((NC_GasFlux_Functions*)m_Class)->GetFunctionValue(m_Opt, x1);


	}
	return MISSING;
}

double Func::GetValue( double x1, double x2) {
//	if(m_FuncType>=100)
//		return ((SimModule*)m_Class)->GetPhysFunctionValue(m_Opt, x1,x2);
	switch(GetGroupNo()) {
		//case 7: return ((Meteorological_Data_Functions*)m_Class)->GetFunctionValue(m_Opt, x1, x2);
//		case 9: return ((Plant_Functions*)m_Class)->GetFunctionValue(m_Opt, x1, x2);
//		case 10: return ((Interception_Functions*)m_Class)->GetFunctionValue(m_Opt, x1, x2);
		//case 12: return ((Radiation_Functions*)m_Class)->GetFunctionValue(m_Opt, x1, x2);
		//case 13: return ((SoilEvaporation_Functions*)m_Class)->GetFunctionValue(m_Opt, x1, x2);
		//case 14: return ((SnowPack_Functions*)m_Class)->GetFunctionValue(m_Opt, x1);
	//	case 20: return ((WaterUptake_Functions*)m_Class)->GetFunctionValue(m_Opt, x1, x2);
//		case 24: return ((NC_Plant_Functions*)m_Class)->GetFunctionValue(m_Opt, x1, x2);
	//	case 32: return ((NC_GasFlux_Functions*)m_Class)->GetFunctionValue(m_Opt, x1, x2);

	}
	return MISSING;
}
double Func::GetValue( double x1, double x2, double x3) {

//	if(m_FuncType>=100)
//		return ((SimModule*)m_Class)->GetPhysFunctionValue(m_Opt, x1,x2,x3);
	switch(GetGroupNo()) {
	//	case 6:	return ((Thermal_Functions*)m_Class)->GetFunctionValue(m_Opt, x1, x2, x3);
//		case 9: return ((Plant_Functions*)m_Class)->GetFunctionValue(m_Opt, x1,x2, x3);
	//	case 10: return ((Interception_Functions*)m_Class)->GetFunctionValue(m_Opt, x1,x2,x3);
	//	case 12: return ((Radiation_Functions*)m_Class)->GetFunctionValue(m_Opt, x1, x2, x3);
	//	case 13: return ((SoilEvaporation_Functions*)m_Class)->GetFunctionValue(m_Opt, x1,x2,x3);


	}
	return MISSING;
}
double Func::GetValue(double x1, double x2, double x3, double x4) {
//	if(m_FuncType>=100)
//		return ((SimModule*)m_Class)->GetPhysFunctionValue(m_Opt, x1,x2,x3, x4);
	switch(GetGroupNo()) {
//		case 10: return ((Interception_Functions*)m_Class)->GetFunctionValue(m_Opt, x1,x2,x3, x4);
//		case 13: return ((SoilEvaporation_Functions*)m_Class)->GetFunctionValue(m_Opt, x1, x2, x3, x4);
	//	case 20: return ((WaterUptake_Functions*)m_Class)->GetFunctionValue(m_Opt, x1, x2, x3, x4);
	}
	return MISSING;
}
double Func::GetValue(size_t index) {
	switch(GetGroupNo()) {
	//case 7: return ((Meteorological_Data_Functions*)m_Class)->GetFunctionValue(m_Opt, index);
	//case 14: return ((SnowPack_Functions*)m_Class)->GetFunctionValue(m_Opt, index);
	}
	return MISSING;
}

double Func::GetValue(size_t index, double x) {

	switch (m_FunctorProp.FuncName) {
		case MyFunc::PSI_THETA_MEASURED:
			if (m_Class == nullptr) return m_FunctorProp.pHyd->f_PsiTheta_Measured(index, x);
			return ((Hydraulic_Functions*)m_Class)->f_PsiTheta_Measured(index, x);
		case MyFunc::PSI_THETA:
			if (m_Class == nullptr) return m_FunctorProp.pHyd->f_PsiTheta_M(index, x);
			return ((Hydraulic_Functions*)m_Class)->f_PsiTheta_M(index, x);




	}
	switch(GetGroupNo()) {
//	case 9:	return ((Plant_Functions*)m_Class)->GetFunctionValue(m_Opt, index, x);
//	case 10: return ((Interception_Functions*)m_Class)->GetFunctionValue(m_Opt,index, x);
//	case 11: return ((Potential_Transpiration_Functions*)m_Class)->GetFunctionValue(m_Opt,index, x);
//	case 12: return ((Radiation_Functions*)m_Class)->GetFunctionValue(m_Opt, index,x);
	//case 14: return ((SnowPack_Functions*)m_Class)->GetFunctionValue(m_Opt, index, x);
//	case 20: return ((WaterUptake_Functions*)m_Class)->GetFunctionValue(m_Opt, index, x);
//	case 24: return ((NC_Plant_Functions*)m_Class)->GetFunctionValue(m_Opt, index, x);
	//case 28: return ((AbioticResponses_Functions*)m_Class)->GetFunctionValue(m_Opt, index, x);
	}
	return MISSING;
}
double Func::GetValue(size_t index, double x1, double x2) {

	switch (m_FunctorProp.FuncName) {
	case MyFunc::HYDRAULIC_COND_THETA:
		return ((Hydraulic_Functions*)m_Class)->f_CondTheta(index, x1, x2);
	case MyFunc::HYDRAULIC_COND_THETA_MEASURED:
		return ((Hydraulic_Functions*)m_Class)->f_CondTheta_Measured(index, x1, x2);
	case MyFunc::HYDRAULIC_COND_PSI:
		return ((Hydraulic_Functions*)m_Class)->f_CondPsi(index, x1, x2);
	case MyFunc::HYDRAULIC_COND_PSI_MEASURED:
		return ((Hydraulic_Functions*)m_Class)->f_CondPsi_Measured(index, x1, x2);



	}
	switch(GetGroupNo()) {

//	case 6:	return ((Thermal_Functions*)m_Class)->GetFunctionValue(m_Opt, index, x1, x2);
//	case 9:	return ((Plant_Functions*)m_Class)->GetFunctionValue(m_Opt, index, x1, x2);
//	case 10: return ((Interception_Functions*)m_Class)->GetFunctionValue(m_Opt,index, x1, x2);
//	case 20: return ((WaterUptake_Functions*)m_Class)->GetFunctionValue(m_Opt, index, x1, x2);
//	case 24: return ((NC_Plant_Functions*)m_Class)->GetFunctionValue(m_Opt, index, x1, x2);

	}
	return MISSING;
}
double Func::GetValue(size_t index, double x1, double x2, double x3, bool check) {
	//switch(GetGroupNo()) {
	//case 5:
//	return ((Hydraulic_Functions*)m_Class)->GetFunctionValue(m_Opt, index, x1, x2, x3);
//	case 11: return ((Potential_Transpiration_Functions*)m_Class)->GetFunctionValue(m_Opt,index, x1, x2, x3);
//	case 20: return ((WaterUptake_Functions*)m_Class)->GetFunctionValue(m_Opt, index, x1, x2, x3, check);

	//}
	return MISSING;
}
double Func::GetValue(size_t index, double x1, double x2, double x3, double x4) {
	switch(GetGroupNo()) {
	//	case 11: return ((Potential_Transpiration_Functions*)m_Class)->GetFunctionValue(m_Opt,index, x1, x2, x3, x4);
	//	case 20: return ((WaterUptake_Functions*)m_Class)->GetFunctionValue(m_Opt, index, x1, x2, x3, x4);

	}
	return MISSING;
}
function<double(bool)> Func::Get_boolBlank_Functor()
{
	if (m_TableType) {
		UpdateP_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorP_Par);// ->Get_D1_Functor();
		return koll.Get_D1_Functor();
	}
	else {
		UpdatePs_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);// ->Get_D1_Functor();
		return koll.Get_boolBlank_Functor();
	}
	//return *m_pFunctor;
}
function<vector<double>()> Func::GetVector_Blank_Functor()
{
	if (m_TableType) {
		UpdateP_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorP_Par);// ->Get_D1_Functor();
		return koll.GetVector_Blank_Functor();
	}
	else {
		UpdatePs_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);// ->Get_D1_Functor();
		return koll.GetVector_Blank_Functor();
	}
	//return *m_pFunctor;
}
function<vector<double>(bool)> Func::GetVector_boolBlank_Functor()
{
	if (m_TableType) {
		UpdateP_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorP_Par);// ->Get_D1_Functor();
		return koll.GetVector_boolBlank_Functor();
	}
	else {
		UpdatePs_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);// ->Get_D1_Functor();
		return koll.GetVector_boolBlank_Functor();
	}
	//return *m_pFunctor;
}
function<double(double)> Func::Get_D1_Functor()
{

	if (m_TableType) {

		UpdateP_Par();

		auto koll = FunctFactory(m_FunctorProp, m_FunctorP_Par);// ->Get_D1_Functor();
		return koll.Get_D1_Functor();
	}
	else {
		UpdatePs_Par();
		if (m_FunctorPs_Par.size() != 0) {
			auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);// ->Get_D1_Functor();
			return koll.Get_D1_Functor();
		}
		else {
			UpdateP_Par();
			auto koll = FunctFactory(m_FunctorProp, m_FunctorP_Par);
			return koll.Get_D1_Functor();
		}

		
	}
	//return *m_pFunctor;
}
function<double(double, double)> Func::Get_D2_Functor()
{
	if (m_TableType) {

		UpdateP_Par();

		auto koll = FunctFactory(m_FunctorProp, m_FunctorP_Par);// ->Get_D1_Functor();
		return koll.Get_D2_Functor();
	}
	else {
		UpdatePs_Par();
		if (m_FunctorPs_Par.size() != 0) {
			auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);
			return koll.Get_D2_Functor();// ->Get_D1_Functor();
		}
		else {
			UpdateP_Par();
			auto koll = FunctFactory(m_FunctorProp, m_FunctorP_Par);
			return koll.Get_D2_Functor();

		}
	}
	//return *m_pFunctor;
}
function<double(double, double, double)> Func::Get_D3_Functor()
{
	if (m_TableType) {
		UpdateP_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorP_Par);// ->Get_D1_Functor();
		return koll.Get_D3_Functor();
	}
	else {
		UpdatePs_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);// ->Get_D1_Functor();
		return koll.Get_D3_Functor();
	}
	//return *m_pFunctor;
}
function<double(double, double, double, double)> Func::Get_D4_Functor()
{
	if (m_TableType) {
		UpdateP_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorP_Par);// ->Get_D1_Functor();
		return koll.Get_D4_Functor();
	}
	else {
		UpdatePs_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);// ->Get_D1_Functor();
		return koll.Get_D4_Functor();
	}
	//return *m_pFunctor;
}
function<double(double, double, double, double, double)> Func::Get_D5_Functor()
{
		UpdatePs_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);// ->Get_D1_Functor();
		return koll.Get_D5_Functor();
}
function<double(bool, size_t)> Func::Get_boolBlank_TableFunctor()
{

	if (m_TableType) {
		UpdateP_Par();
		if (m_FunctorPs_Par.size() > 0) {
			UpdatePs_Par();
			auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par, m_FunctorP_Par);// ->Get_D1_Functor();
			return koll.Get_boolBlank_TableFunctor();


		}
		auto koll = FunctFactory(m_FunctorProp, m_FunctorP_Par);// ->Get_D1_Functor();
		return koll.Get_boolBlank_TableFunctor();
	}
	else {
		UpdatePs_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);// ->Get_D1_Functor();
		return koll.Get_boolBlank_TableFunctor();
	}
	//return *m_pFunctor;
}
function<double(bool, size_t, double, double, double)> Func::Get_boolD3_TableFunctor()
{

	if (m_TableType) {
		UpdateP_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorP_Par);// ->Get_D1_Functor();
		return koll.Get_D3_boolTableFunctor();
	}
	else {
		return nullptr;
	}
	//return *m_pFunctor;
}
function<double(bool, size_t, double, double, double, double)> Func::Get_boolD4_TableFunctor()
{

	if (m_TableType) {
		UpdateP_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorP_Par);// ->Get_D1_Functor();
		return koll.Get_D4_boolTableFunctor();
	}
	else {
		return nullptr;
	}
	//return *m_pFunctor;
}
function<double(size_t,double)> Func::Get_D1_TableFunctor()
{
	if (m_TableType) {
		UpdateP_Par();
		UpdatePs_Par();
		if (m_FunctorPs_Par.size() > 0) {
			auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par, m_FunctorP_Par);
			return koll.Get_D1_TableFunctor();
		}

		auto koll = FunctFactory(m_FunctorProp, m_FunctorP_Par);// ->Get_D1_Functor();
		return koll.Get_D1_TableFunctor();
	}
	else {
		UpdatePs_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);// ->Get_D1_Functor();
		return koll.Get_D1_TableFunctor();
	}
	//return *m_pFunctor;
}
function<double(size_t, double, double)> Func::Get_D2_TableFunctor()
{
	if (m_TableType) {
		UpdateP_Par();
		UpdatePs_Par();
		if (m_FunctorPs_Par.size() > 0) {
			auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par, m_FunctorP_Par);
			return koll.Get_D2_TableFunctor();
		}
		else {
			auto koll = FunctFactory(m_FunctorProp, m_FunctorP_Par);
			return koll.Get_D2_TableFunctor();


		}
	}
	else {
		UpdatePs_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);// ->Get_D1_Functor();
		return koll.Get_D2_TableFunctor();
	}
	//return *m_pFunctor;
}
function<double(size_t, double, double, double)> Func::Get_D3_TableFunctor()
{
	if (m_TableType) {
		UpdateP_Par();
		UpdatePs_Par();
		if (m_FunctorPs_Par.size() > 0) {
			auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par, m_FunctorP_Par);
			return koll.Get_D3_TableFunctor();
		}
		else {
			auto koll = FunctFactory(m_FunctorProp, m_FunctorP_Par);// ->Get_D1_Functor();
			return koll.Get_D3_TableFunctor();
		}
	}
	else {
		UpdateP_Par();
		UpdatePs_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);// ->Get_D1_Functor();
		return koll.Get_D3_TableFunctor();
	}
	//return *m_pFunctor;
}
function<double(size_t, size_t, double, double, double)> Func::Get_T_D3_TableFunctor()
{
	if (m_TableType) {
		UpdateP_Par();
		UpdatePs_Par();
		if (m_FunctorPs_Par.size() > 0) {
			auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par, m_FunctorP_Par);
			return koll.Get_T_D3_TableFunctor();
		}
		else {
			auto koll = FunctFactory(m_FunctorProp, m_FunctorP_Par);// ->Get_D1_Functor();
			return koll.Get_T_D3_TableFunctor();
		}
	}
	else {
		UpdateP_Par();
		UpdatePs_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);// ->Get_D1_Functor();
		return koll.Get_T_D3_TableFunctor();
	}
}//return *m_pFunctor;

function<double(size_t, double, double, double, double)> Func::Get_D4_TableFunctor()
{
	if (m_TableType) {
		UpdateP_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorP_Par);// ->Get_D1_Functor();
		return koll.Get_D4_TableFunctor();
	}
	else {
		UpdatePs_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);// ->Get_D1_Functor();
		return koll.Get_D4_TableFunctor();
	}
	//return *m_pFunctor;
}
function<void(vector<double>*)>  Func::GetVectorPointer_D1_Functor() {
	if (m_TableType) {
		UpdateP_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorP_Par);// ->Get_D1_Functor();
		return koll.GetVectorPointer_D1_Functor();
	}
	else {
		UpdatePs_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);// ->Get_D1_Functor();
		return koll.GetVectorPointer_D1_Functor();
	}

}
function<void(vector<double>*, vector<double>*)>  Func::GetVectorPointer_D2_Functor() {
	if (m_TableType) {
		UpdateP_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorP_Par);// ->Get_D1_Functor();
		return koll.GetVectorPointer_D2_Functor();
	}
	else {
		UpdateP_Par();
		UpdatePs_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par, m_FunctorP_Par);// ->Get_D1_Functor();
		return koll.GetVectorPointer_D2_Functor();
	}

}
function<void(vector<double>*, vector<double>*, vector<double>*)>  Func::GetVectorPointer_D3_Functor() {
	UpdateP_Par();
	UpdatePs_Par();
	if (m_FunctorP_Par.size()>0&& m_FunctorPs_Par.size()>0) {
		auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par, m_FunctorP_Par);// ->Get_D1_Functor();
		return koll.GetVectorPointer_D3_Functor();
	}
	else if(m_FunctorP_Par.size()>0) {
		auto koll = FunctFactory(m_FunctorProp, m_FunctorP_Par);// ->Get_D1_Functor();
		return koll.GetVectorPointer_D3_Functor();
	}
	else {
		auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);// ->Get_D1_Functor();
		return koll.GetVectorPointer_D3_Functor();
	}

}
function<void(vector<double>*, vector<double>*, vector<double>*, vector<double>*)> Func::GetVectorPointer_D4_Functor() {
	if (m_TableType) {
		UpdateP_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorP_Par);// ->Get_D1_Functor();
		return koll.GetVectorPointer_D4_Functor();
	}
	else {
		UpdatePs_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);// ->Get_D1_Functor();
		return koll.GetVectorPointer_D4_Functor();
	}

}
function<void(vector<double>*, vector<double>*, vector<double>*, vector<double>*, vector<double>*)> Func::GetVectorPointer_D5_Functor() {
	if (m_TableType) {
		UpdateP_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorP_Par);// ->Get_D1_Functor();
		return koll.GetVectorPointer_D5_Functor();
	}
	else {
		UpdatePs_Par();
		auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);// ->Get_D1_Functor();
		return koll.GetVectorPointer_D5_Functor();
	}

}

function<OrgTurnOver(double, double, double)> Func::Get_OrgTurnOverFunctor()
{
	UpdatePs_Par();
	auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);
	return koll.Get_OrgTurnOverFunctor();
}
function<OrgTurnOver(double, double, double, double, double)> Func::Get_D5_OrgTurnOverFunctor()
{
	UpdatePs_Par();
	auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);
	return koll.Get_D5_OrgTurnOverFunctor();
}
function<pair<double, double>( double, double)> Func::Get_Pair_D2Functor()
{
	UpdatePs_Par();
	auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);
	return koll.Get_Pair_D2Functor();
}
function<pair<double, double>(double, double, double)> Func::Get_Pair_D3Functor()
{
	UpdatePs_Par();
	auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);
	return koll.Get_Pair_D3Functor();
}
function < pair<double, double>(size_t, double, double) > Func::Get_Pair_D2_TableFunctor()
{
	UpdatePs_Par();
	auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);
	return koll.Get_Pair_D2_TableFunctor();
}
function<pair<double, double>(size_t, double, double, double)> Func::Get_Pair_D3_TableFunctor()
{
	UpdatePs_Par();
	auto koll = FunctFactory(m_FunctorProp, m_FunctorPs_Par);
	return koll.Get_Pair_D3_TableFunctor();
}


MyArgType Func::GetArgumentTypes()
{
	return 	m_FunctorProp.Arg;
}

double Func::Get(double x)
{
	return m_D1Functor(x);

}

void Func::UpdateP_Par()
{
	m_FunctorP_Par.clear();
	for (size_t i = 0; i < m_P_vector.size(); i++) {
		vector<double> p;
		for (size_t j = 0; j < m_P_vector[i]->GetSize(); j++)
			p.push_back(m_P_vector[i]->GetValue(j));
		m_FunctorP_Par.push_back(p);
	}
}
void Func::UpdatePs_Par()
{
	m_FunctorPs_Par.clear();
	for (size_t i = 0; i < m_Ps_Vector.size(); i++) {
		m_FunctorPs_Par.push_back(m_Ps_Vector[i]->GetValue());
	}
}


