#include "AbioticResponses.h"
#include "./Functors/NC_Soil_Functors.h"



						   
AbioticResponses::AbioticResponses(void)
{
	ModuleName="Common abiotic responses";
	ModuleNo=28;
	ModuleConditions="";
}

bool AbioticResponses::Ini()	{

	P_ExternalInputs::Ini();

	return true;
}
bool AbioticResponses::IniFunctors() {

	P_ExternalInputs::IniFunctors();



	return Ini_Responses();



	//p_Soil_WaterF->WaterE
	return true;
}
bool AbioticResponses::Ini_Responses()
{
	if (Common_Temp_Response_Sw(Common_Temp_Response) == Common_Temp_Response_Sw::Q10_whole_range)
		f_CommonTempResponse = Get_D1_Functor(MyFunc::ABIOTIC_TEMP_Q10_RESPONSE);
	else if (Common_Temp_Response_Sw(Common_Temp_Response) == Common_Temp_Response_Sw::Q10_above_threshold)
		f_CommonTempResponse = Get_D1_Functor(MyFunc::ABIOTIC_TEMP_Q10THRESHOLD_RESPONSE);
	else if (Common_Temp_Response_Sw(Common_Temp_Response) == Common_Temp_Response_Sw::Ratkowsky_function)
		f_CommonTempResponse = Get_D1_Functor(MyFunc::ABIOTIC_TEMP_RATKOWSKY_RESPONSE);
	else if (Common_Temp_Response_Sw(Common_Temp_Response) == Common_Temp_Response_Sw::O_Neill_function)
		f_CommonTempResponse = Get_D1_Functor(MyFunc::ABIOTIC_TEMP_NEILL_RESPONSE);
	else if (Common_Temp_Response_Sw(Common_Temp_Response) == Common_Temp_Response_Sw::Arrhenius_function)
		f_CommonTempResponse = Get_D1_Functor(MyFunc::ABIOTIC_TEMP_ARRHENIUS_RESPONSE);
	if (Common_Moisture_Response_Sw(Common_Moisture_Response) == Common_Moisture_Response_Sw::Theta_Function)
		f_Theta_Response = Get_D1_TableFunctor(MyFunc::ABIOTIC_THETA_RESPONSE);
	else if (Common_Moisture_Response_Sw(Common_Moisture_Response) == Common_Moisture_Response_Sw::CombinedThetaCO2)
		f_CombinedThetaCO2_Response= Get_D2_TableFunctor(MyFunc::ABIOTIC_COMBINED_THETA_CO2_RESPONSE);
	else if (Common_Moisture_Response_Sw(Common_Moisture_Response) == Common_Moisture_Response_Sw::CO2_Function)
		f_CO2_Response = Get_D1_Functor(MyFunc::ABIOTIC_CO2_RESPONSE);



	return false;
}
bool AbioticResponses::Dyn()
{
	P_ExternalInputs::Dyn();
	Flux();
	Integ();
	return true;
}
bool AbioticResponses::Def()
{
	P_ExternalInputs::Def();

	ModuleNo = 28;
	ModuleConditions = "";
	Sw* pSw;



	pSw = new Sw(&Common_Temp_Response, SIMPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pSw, "Temp Response", 0, "Q10 whole range|Q10 above threshold|Ratkowsky function|O Neill function|Arrhenius function", "Nitrogen and Carbon > 0");

	pSw = new Sw(&Common_Moisture_Response, SIMPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pSw, "Moisture Response", 0, "Theta Function|CO2 Function|Combined ThetaCO2", "Nitrogen and Carbon > 0|CO2 SoilModel>0");

	Func *pFunc;
	// Parameters;
	string funcname;
	FunctorProp functorprop;
	vector<Ps*> ps_vec;
	vector<P*> p_vec;
	ps_vec.clear();
	Ps *pPs; P *pP;
	Tab *pTab;

	functorprop.FunctorInitOption = string::npos;
	functorprop.pModelInfo = m_pModelInfo;

	PLOT_PARAM plotpar;
	plotpar.Num_Of_Curves = 1;
	plotpar.Num_Of_Points = 100;
	plotpar.Num_Of_X = 1;
	plotpar.X_Min = -10;
	plotpar.X_Max = 30;
	plotpar.X_Label = "Temperature (C)";
	plotpar.Y_Label = "Response ()";
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.Num_Of_Opt = 1;


	functorprop.FuncName = MyFunc::ABIOTIC_TEMP_Q10_RESPONSE; functorprop.Arg = MyArgType::D1;
	funcname = "Q10 Temperature Response";
	ps_vec.clear();

	pPs = new Ps(&CommonTemQ10, SIMPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "TemQ10", 2., "-", "Nitrogen and Carbon > 0|Temp Response < 2", 0.0, 5., 0.5, 4.); ps_vec.push_back(pPs);
	pPs = new Ps(&CommonTemQ10Bas, SIMPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "TemQ10Bas", 20.,UNIT_TYPES::TEMP_UNIT, "Nitrogen and Carbon > 0|Temp Response < 2", 0.0001, 100., 10., 30.); ps_vec.push_back(pPs);

	pFunc = new Func(PLANT, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);

	functorprop.FuncName = MyFunc::ABIOTIC_TEMP_Q10THRESHOLD_RESPONSE; functorprop.Arg = MyArgType::D1;
	funcname = "Q10  with Threshold Temperature Response";

	pPs = new Ps(&CommonTemQ10Threshold, SIMPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "TemQ10Threshold", 5.,UNIT_TYPES::TEMP_UNIT, "Nitrogen and Carbon > 0|Temp Response = 1", 0.0001, 20., 2., 10.); ps_vec.push_back(pPs);

	pFunc = new Func(PLANT, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);


	functorprop.FuncName = MyFunc::ABIOTIC_TEMP_RATKOWSKY_RESPONSE; functorprop.Arg = MyArgType::D1;
	funcname = "Ratkowsky Temperature Response";
	ps_vec.clear();
	pPs = new Ps(&CommonTempMin, SIMPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "TempMin", -8.,UNIT_TYPES::TEMP_UNIT, "Nitrogen and Carbon > 0|Temp Response = 2", -20., 10., -10., 5.); ps_vec.push_back(pPs);
	pPs = new Ps(&CommonTempMax, SIMPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "TempMax", 20.,UNIT_TYPES::TEMP_UNIT, "Nitrogen and Carbon > 0|Temp Response = 2", 0.0001, 100., 10., 30.);

	pFunc = new Func(PLANT, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);

	functorprop.FuncName = MyFunc::ABIOTIC_TEMP_NEILL_RESPONSE; functorprop.Arg = MyArgType::D1;
	funcname = "O Neill Temperature Response";
	ps_vec.clear();
	plotpar.X_Max = 45.;
	//! O"Neill function
	pPs = new Ps(&CommonNeillForm, SIMPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "Neill Form", 4.28, "-", "Temp Response = 3", 0.1, 100.); ps_vec.push_back(pPs);
	pPs = new Ps(&CommonNeillMax, SIMPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "Neill Max Temp", 42.,UNIT_TYPES::TEMP_UNIT, "Temp Response = 3", 20., 100.); ps_vec.push_back(pPs);
	pPs = new Ps(&CommonNeillOpt, SIMPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "Neill Opt Temp", 27.5,UNIT_TYPES::TEMP_UNIT, "Temp Response = 3", 10., 80.); ps_vec.push_back(pPs);

	pFunc = new Func(PLANT, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);

	// Arrhenius_function add by Hongxing
	functorprop.FuncName = MyFunc::ABIOTIC_TEMP_ARRHENIUS_RESPONSE; functorprop.Arg = MyArgType::D1;
	funcname = "Arrhenius Temperature Response";
	plotpar.X_Max = 30;
	plotpar.X_Label = "Temperature (C)";
	plotpar.Y_Label = "Response ()";
	ps_vec.clear();
	pPs = new Ps(&CommonArrheniusActWea, SIMPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "Arrhenius Act Wea", 47300., UNIT_TYPES::ENERGY_MOLE, "Nitrogen and Carbon > 0|Temp Response = 4", 10000., 200000., 20000., 100000.); ps_vec.push_back(pPs);
	pPs = new Ps(&CommonArrheniusTemp0, SIMPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "Arrhenius Temp0", 10.,UNIT_TYPES::TEMP_UNIT, "Nitrogen and Carbon > 0|Temp Response = 4", 1., 100., 5., 30.); ps_vec.push_back(pPs);


	pFunc = new Func(PLANT, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);


	functorprop.FuncName = MyFunc::ABIOTIC_THETA_RESPONSE; functorprop.Arg = MyArgType::T_D1;
	funcname = "Decomposition Water Content Response";
	ps_vec.clear();
	p_vec.clear();

	pPs = new Ps(&SaturationActivity, SIMPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "SaturationActivity", 0.6, "-", "Nitrogen and Carbon > 0|Moisture Response=0", 0., 1.); ps_vec.push_back(pPs);
	pPs = new Ps(&ThetaLowerRange, SIMPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "ThetaLowerRange", 13., "vol %", "Nitrogen and Carbon > 0", 0., 100., 3., 20.); ps_vec.push_back(pPs);
	pPs = new Ps(&ThetaUpperRange, SIMPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "ThetaUpperRange", 8., "vol %", "Nitrogen and Carbon > 0", 0., 100., 3., 20.); ps_vec.push_back(pPs);
	pPs = new Ps(&ThetaPowerCoef, SIMPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pPs, "ThetaPowerCoef", 1., "-", "Nitrogen and Carbon > 0", 0.01, 10., 0.1, 3.); ps_vec.push_back(pPs);

	p_vec.push_back(m_pModelMap->GetPPointer("mSaturation"));
	p_vec.push_back(m_pModelMap->GetPPointer("mWilting Point"));
	pFunc = new Func(PLANT, CARBON, NOFPROC, DECOMPOSITION, NORMAL);


	plotpar.X_Min = 0;
	plotpar.X_Max = 100;
	plotpar.X_Label = "Water Content (vol %)";
	plotpar.Y_Label = "Response ()";


	Define(pFunc, functorprop, funcname, ps_vec,p_vec, plotpar);


	functorprop.FuncName = MyFunc::ABIOTIC_CO2_RESPONSE; functorprop.Arg = MyArgType::D1;
	funcname = "Decomposition CO2 Response";
	plotpar.X_Min = 0;
	plotpar.X_Max = 3000;
	plotpar.X_Label = "CO2 Concentration (PPM)";
	ps_vec.clear();
	p_vec.clear();
	pPs = new Ps(&CO2LowestImpact, SIMPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "CO2 Lowest Impact", 500., "PPM", "Nitrogen and Carbon > 0|Moisture Response>0", 10., 200000, 400., 20000.);

	pPs = new Ps(&CO2CutOffThreshold, SIMPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "CO2 CutOff Threshold", 2000., "PPM", "Nitrogen and Carbon > 0|Moisture Response>0", 10., 200000, 400., 5000.);

	pFunc = new Func(PLANT, CARBON, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec,  plotpar);
	ps_vec.clear();

	functorprop.FuncName = MyFunc::ABIOTIC_COMBINED_THETA_CO2_RESPONSE; functorprop.Arg = MyArgType::T_D2;
	funcname = "Decomposition Response Water and CO2";
	pFunc = new Func(PLANT, CARBON, NOFPROC, DECOMPOSITION, NORMAL);

	ps_vec.push_back(m_pModelMap->GetPsPointer("ThetaUpperRange"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("ThetaLowerRange"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("ThetaPowerCoef"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("CO2 Lowest Impact"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("CO2 CutOff Threshold"));


	p_vec.push_back(m_pModelMap->GetPPointer("mSaturation"));
	p_vec.push_back(m_pModelMap->GetPPointer("mWilting Point"));
	Define(pFunc, functorprop, funcname, ps_vec, p_vec, plotpar);


	//!	Response function
	G* pG;
	vector<double> initv;
	initv.assign(p_Soil_WaterF->NL, 0.);
	NE *pNL; pNL = m_pModelMap->GetNEPointer("Number of Layers");

	pG = new G(&Response.Temp, pNL, SIMPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pG, pNL, "Response(c) Temp", initv, "-", "Nitrogen and Carbon >= 1");
	pG = new G(&Response.Theta, pNL, SIMPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pG, pNL, "Response(c) Theta", initv, "-", "Nitrogen and Carbon >= 1");
	pG = new G(&Response.Temp_Theta, pNL, SIMPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pG, pNL, "Response(c) TempTheta", initv, "-", "Nitrogen and Carbon >= 1");

	Gs *pGs;
	pGs = new Gs(&Response.Litter, TOTPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pGs, "Response(c) Litter", 0.0, "-", "Nitrogen and Carbon >= 1");
	pGs = new Gs(&Response.Humus, TOTPROFILE, ORGANIC, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pGs, "Response(c) Humus", 0.0, "-", "Nitrogen and Carbon >= 1");

	return FunctionDef();

}
bool AbioticResponses::FunctionDef() {
	Func *pFunc;
/*	double (AbioticResponses_Functions::*fpoint)(double);
	fpoint = &AbioticResponses_Functions::CommonTempFunction;
	pFunc = new Func(SIMPROFILE, GENERAL, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, m_fpointer0_1.size(), 0, this, "Common Temperature Response"); m_fpointer0_1.push_back(fpoint);
	double res = pFunc->GetValue(3.);
	double (AbioticResponses_Functions::*fpoint1_1)(size_t, double);
	fpoint1_1 = &AbioticResponses_Functions::CommonThetaFunction;
	pFunc = new Func(SIMPROFILE, GENERAL, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, m_fpointer1_1.size(), 10, this, "Common Soil Moisture Response");	m_fpointer1_1.push_back(fpoint1_1);
	res = pFunc->GetValue(size_t(1), 700.);

	fpoint = &AbioticResponses_Functions::DenitTempFunction;
	pFunc = new Func(SIMPROFILE, GENERAL, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, m_fpointer0_1.size(), 0, this, "Denitrification Temperature Response"); m_fpointer0_1.push_back(fpoint);
	res = pFunc->GetValue(3.);


	fpoint = &AbioticResponses_Functions::RespTempFunction;
	pFunc = new Func(PLANT, CARBON, TRANSPORT, RESPIRATION, NORMAL);
	Define(pFunc, m_fpointer0_1.size(), 0, this, "Plant Respiration Temperature Function"); m_fpointer0_1.push_back(fpoint);
	res = pFunc->GetValue(1.);

	fpoint = &AbioticResponses_Functions::CommonCO2ResponseFunction;
	pFunc = new Func(SIMPROFILE, GENERAL, NOFPROC, DECOMPOSITION, NORMAL);
	Define(pFunc, m_fpointer0_1.size(), 0, this, "CO2 Response Function"); m_fpointer0_1.push_back(fpoint);
	res = pFunc->GetValue(1000.);
	*/
	return true;
}






void AbioticResponses::Flux()
{
	
}
void AbioticResponses::Integ()
{
}

bool AbioticResponses::End()
{
return true;
}
AbioticResponses::~AbioticResponses(void)
{
}
