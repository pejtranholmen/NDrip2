#include "Potential_Transpiration.h"
#include "../Atmosphere/SnowPack.h"
#include "../../Simulator/Simulator.h"
#include "../../Util/FUtil.hpp"
						   
Potential_Transpiration::Potential_Transpiration(void)
{
	ModuleName="Potential_Transpiration";
	ModuleNo=11;
	ModuleConditions="Potential_Transpiration SOIL Module >0";
}
bool Potential_Transpiration::Def()
{
	Plant::Def();

	ModuleNo = 11;
	ModuleConditions = "";
	SoilModel* pSoil = (SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	p_Soil_WaterF = &pSoil->m_SoilFrost;
	Sw* pSw;

	pSw = new Sw(&Roughness, SINGLE, ATMOSPHERE, TURBULENCE, NOBPROC, NORMAL);

	Define(pSw, "Roughness", 0, "Parameters|Driving variable|f(canopy)", "PlantType >= 2");
	pSw = new Sw(&Roughnessfunc, SINGLE, ATMOSPHERE, TURBULENCE, NOBPROC, NORMAL);
	Define(pSw, "Roughnessfunc", 0, "Shaw & Perreria|linear", "PlantType = 3");
	pSw = new Sw(&MultiRoughness, SINGLE, ATMOSPHERE, TURBULENCE, NOBPROC, NORMAL);
	Define(pSw, "MultiRoughness", 0, "No (common)|Individual", "PlantType = 3");

	pSw = new Sw(&Displacement, PLANT, ATMOSPHERE, TURBULENCE, NOBPROC, NORMAL);
	Define(pSw, "Displacement", 0, "Parameters|Driving variable|f(canopy)", "PlantType = 2");

	pSw = new Sw(&ConductanceCO2_Dependence, PLANT, ATMOSPHERE, POTTRANSPIRATION, PHOTOSYNTHESIS, NORMAL);
	Define(pSw, "CAtmDependence", 0, "Independent|Simple CO2 Response (atm)|CO2 Leaf Surface Response", "PlantType >=2");

	pSw = new Sw(&RSMethod, PLANT, VAPOUR, RESISTANCE, NOBPROC, NORMAL);
	Define(pSw, "RSMethod", 0, "Parameter|Driving variable|Lohammar Eq|Loh.Eq (T>DayNum)", "PlantType = 2", 111);
	pSw = new Sw(&ResAirStabCorr, PLANT, ATMOSPHERE, TURBULENCE, NOBPROC, NORMAL);
	Define(pSw, "Aerodynamic Resistance", 0, "Without stability correction|f(Richardson number)|f(Monin-Obukhov length)", "PlantType >= 2");

	Func *pFunc;
	string funcname;
	FunctorProp functorprop;
	vector<Ps*> ps_vec;
	vector<P*> p_vec;

	PLOT_PARAM plotpar;
	plotpar.Num_Of_Curves = 1;
	plotpar.Num_Of_Points = 300;
	plotpar.Num_Of_X = 1;
	plotpar.X_Min = 0;
	plotpar.X_Max = 366;
	plotpar.X_Label = "Day Number";
	plotpar.Y_Label = "Potential Transpiration (mm/day)";
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.Num_Of_Opt = 1;




	Ps* pPs;
	pPs = new Ps(&EPMaxDay, SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pPs, "EPMaxDay", 195., "Julian day", "PlantType = 1|PotTranspInput = 0", 1., 365.); ps_vec.push_back(pPs);
	pPs = new Ps(&EPPeriod, SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pPs, "EPPeriod", 200., "days", "PlantType = 1|PotTranspInput = 0", 10., 365.); ps_vec.push_back(pPs);
	pPs = new Ps(&EPMaxRate, SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pPs, "EPMaxRate", 4.,UNIT_TYPES::WATERFLOW_UNIT, "PlantType = 1|PotTranspInput = 0", 0.1, 20., 1., 10.); ps_vec.push_back(pPs);

	functorprop.FuncName = MyFunc::POTTRANSPIRATION_DAYOFYEAR; functorprop.Arg = MyArgType::D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Potential Transpiration - Day of year function";
	pFunc = new Func(SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec,plotpar); ps_vec.clear();




	pPs = new Ps(&CanDensMax, SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "CanDensMax", 0.7, "-", "Roughness = 2|PlantType = 2", 0.1, 0.9);
	pPs = new Ps(&PAddIndex, SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "PAddIndex", 1., "-", "Roughness = 2|PlantType = 2", 0., 5.);
	pPs = new Ps(&RoughLMin, SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "RoughLMin", 0.01, "s/m", "Roughness = 2|PlantType = 2", 0.00001, 1.);



	functorprop.FuncName = MyFunc::SHAWPERRERIA_ROUGHL; functorprop.Arg = MyArgType::D2;
	funcname = "Roughness Length - Shaw & Perreria - Single Plant";
	pFunc = new Func(SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec);



	functorprop.FuncName = MyFunc::SHAWPERRERIA_DISPLACEMENT; functorprop.Arg = MyArgType::D3;
	funcname = "Displacement Length - Shaw & Perreria - Single Plant";
	pFunc = new Func(SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec);

	ps_vec.push_back(m_pModelMap->GetPsPointer("RoughLMin"));

	functorprop.FuncName = MyFunc::NO_PARFUNC; functorprop.Arg = MyArgType::D4;
	functorprop.PhysFuncName = FUNC_CHOICE::RES_AIR_MOM_HEAT;
	funcname = "Max Aerodynamic Resistance by plant";
	pFunc = new Func(SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec); ps_vec.clear();





	pPs = new Ps(&CondRis, SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pPs, "CondRis", 5.E6,UNIT_TYPES::HEATFLOW_UNIT, "RSMethod >1|PlantType = 2", 1.E6, 500.E6);
	pPs = new Ps(&CondVPD, SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pPs, "CondVPD", 100., "Pa", "RSMethod >1|PlantType = 2", 10., 10000., 50., 4000.);

	ps_vec.clear(); p_vec.clear();
	pPs = new Ps(&CondMax, SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pPs, "CondMax", 0.02, "m/s", "RSMethod >1|PlantType = 2", 0.00005, 1., 0.001, .05); ps_vec.push_back(pPs);
	pPs = new Ps(&CondMaxWinter, SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pPs, "CondMaxWinter", 0.002, "m/s", "Winter regulation = 1", 0.00005, 1., 0.001, .05); ps_vec.push_back(pPs);

	functorprop.FuncName = MyFunc::MAXCONDUCTANCE; functorprop.Arg = MyArgType::BLANK;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Max Conductance - Winter - Summer";
	pFunc = new Func(SINGLEPLANT, PLANT_SHAPE, NOFPROC, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec);

	ps_vec.erase(ps_vec.begin());


	ModuleNo = 13;
	ModuleConditions = "Evaporation>0";

	pPs = new Ps(&KBMinusOne, ATMBOUNDARY, VAPOUR, TURBULENCE, NOBPROC, NORMAL);
	Define(pPs, "KBMinusOne", 0., "-", "WaterEq = 1|Surface Temperature >=2|Evaporation Method > 0", 0.0, 5.0, 0.0, 2.5);

	pPs = new Ps(&StabCoefStableRich, ATMBOUNDARY, VAPOUR, TURBULENCE, NOBPROC, NORMAL);
	Define(pPs, "StabCoefStableRich", 16., "-", "Evaporation > 0|Evaporation Method > 0|Surface Temperature > 1|WaterEq = 1|Stability Correction<1", 0., 30., 12., 20.);
	pPs = new Ps(&StabCoefStableExp, ATMBOUNDARY, VAPOUR, TURBULENCE, NOBPROC, NORMAL);
	Define(pPs, "StabCoefStableExp", 0.33333, "-", "Evaporation > 0|Evaporation Method > 0|Surface Temperature > 1|WaterEq = 1|Stability Correction<1", 0., 5., 0., 5.);
	pPs = new Ps(&StabCoefUnStableRich, ATMBOUNDARY, VAPOUR, TURBULENCE, NOBPROC, NORMAL);
	Define(pPs, "StabCoefUnstableRich", 16., "-", "Evaporation > 0|Evaporation Method > 0|Surface Temperature > 1|WaterEq = 1|Stability Correction<1", 0., 30., 12., 20.);
	pPs = new Ps(&StabCoefUnStableExp, ATMBOUNDARY, VAPOUR, TURBULENCE, NOBPROC, NORMAL);
	Define(pPs, "StabCoefUnstableExp", 0.33333, "-", "Evaporation > 0|Evaporation Method > 0|Surface Temperature > 1|WaterEq = 1|Stability Correction<1", 0., 5., 0., 5.);

	ModuleNo = 11;
	ModuleConditions = "";
	ps_vec.clear();
	pPs = new Ps(&WindLessExchangeCanopy, SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pPs, "WindLessExchangeCanopy", 0.001, "m/s", "PlantType > 1", 0.0, 1.); ps_vec.push_back(pPs);

	ps_vec.push_back(m_pModelMap->GetPsPointer("KBMinusOne"));

	functorprop.FuncName = MyFunc::RA_CANOPY_NEUTRAL; functorprop.Arg = MyArgType::D3;
	funcname = "Aerodynamic Resistance Canopy - Neutral";
	plotpar.Num_Of_Curves = 4;
	plotpar.Num_Of_Points = 300;
	plotpar.Num_Of_X = 3;
	plotpar.X_Min = 0;
	plotpar.X_Max = 10;
	plotpar.X_Label = "Wind Speed (m/s)";
	plotpar.Y_Label = "Aerodynamic Resistance (s/m)";
	plotpar.LogXScale = false;
	plotpar.LogYScale = true;
	plotpar.Num_Of_Opt = 1;
	plotpar.LegendsVec = { "Zo=0.01 m, H=2 m","Zo 0.1 m, H=2 m", "Zo=0.01 m, H= 10 m","Zo 0.1 m, H=10 m" };
	plotpar.X2_Vec = { 2.0f,2.0f, 10.0f,10.0f };
	plotpar.X3_Vec = { 0.01f,0.1f,  0.01f,0.1f };
	

	pFunc = new Func(SINGLEPLANT, PLANT_SHAPE, NOFPROC, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);  ps_vec.clear();

	ps_vec.push_back(m_pModelMap->GetPsPointer("WindLessExchangeCanopy"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("KBMinusOne"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("StabCoefStableRich"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("StabCoefStableExp"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("StabCoefUnstableRich"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("StabCoefUnstableExp"));
	
	plotpar.X3_Vec = { 0.0f,0.0f, 0.0f,0.0f };
	plotpar.X4_Vec = { 10.0f,10.0f, 10.0f,10.0f };
	plotpar.X5_Vec = { 0.01f,0.1f,  0.01f,0.1f };
	plotpar.Num_Of_X = 5;
	
	functorprop.FuncName = MyFunc::RA_CANOPY_ANALYTICAL; functorprop.Arg = MyArgType::D5;
	funcname = "Aerodynamic Resistance Canopy - Analytical Stability)";
	pFunc = new Func(SINGLEPLANT, PLANT_SHAPE, NOFPROC, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);

	functorprop.FuncName = MyFunc::RA_CANOPY_NUMERICAL; functorprop.Arg = MyArgType::D5;
	funcname = "Aerodynamic Resistance Canopy - Monin-Obokuv Stability)";
	pFunc = new Func(SINGLEPLANT, PLANT_SHAPE, NOFPROC, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);


	plotpar.LogYScale = false;



	ps_vec.clear(); pPs = new Ps(&CO2_BaseLevel, PLANT, ATMOSPHERE, POTTRANSPIRATION, PHOTOSYNTHESIS, NORMAL);
	Define(pPs, "CO2 Base Level", 354., "ppm", "PlantType>=3|CAtmDependence>0", 300., 600.); ps_vec.push_back(pPs);
	pPs = new Ps(&CO2_PowerCoef, PLANT, ATMOSPHERE, POTTRANSPIRATION, PHOTOSYNTHESIS, NORMAL);
	Define(pPs, "CO2 Power Coef", 1., "-", "CAtmDependence>0|PlantType>=3", 0.5, 1.5); ps_vec.push_back(pPs);

	functorprop.FuncName = MyFunc::CONDUCTANCE_CO2; functorprop.Arg = MyArgType::D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Canopy Conductance - CO2 influence";
	pFunc = new Func(SINGLEPLANT, PLANT_SHAPE, NOFPROC, NOBPROC, NORMAL);

	plotpar.Num_Of_Curves = 1;
	plotpar.Num_Of_Points = 300;
	plotpar.Num_Of_X = 1;
	plotpar.X_Min = 300;
	plotpar.X_Max = 450;
	plotpar.X_Label = "CO2 Conc (PPM)";
	plotpar.Y_Label = "Scaling on Canopy Conductance (-)";
	plotpar.LegendsVec = {};

	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.Num_Of_Opt = 1;




	Define(pFunc, functorprop, funcname, ps_vec, plotpar);


	P* pP;
	vector<double> initv;
	NE *pNumPlants, *pNoDayNum;
	pNumPlants = m_pModelMap->GetNEPointer("Number of Plants");
	pNoDayNum = m_pModelMap->GetNEPointer("Number of DayNumber for Single Plant Seasonality");

	pP = new P(&MC_CanDensMax, pNumPlants, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	initv.assign(NumPlants, 0.7);
	Define(pP, pNumPlants, "Canopy DensMax", initv, "-", "PlantType = 3|Roughnessfunc=0", 0.1, 0.9, 0.2, 0.8);
	mTab_EvapMulti.push_back(pP);

	pP = new P(&MC_PAddIndex, pNumPlants, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	initv.assign(NumPlants, 1.);
	Define(pP, pNumPlants, "Plant AddIndex", initv, "-", "PlantType >= 3|Roughnessfunc=0", 0., 5.);
	mTab_EvapMulti.push_back(pP);

	pP = new P(&MC_RoughLMin, pNumPlants, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	initv.assign(NumPlants, 0.01);
	Define(pP, pNumPlants, "Roughness Min", initv, "m", "PlantType >= 3", 0.00001, 1., 0.001, 0.5);
	mTab_EvapMulti.push_back(pP);



	pP = new P(&MC_RaLAI_Effect, pNumPlants, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	initv.assign(NumPlants, 20.);
	Define(pP, pNumPlants, "Air Resist LAI Effect", initv, "s/m", "PlantType >= 3", 0.00001, 1.);
	mTab_EvapMulti.push_back(pP);

	pP = new P(&MC_CondRis, pNumPlants, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	initv.assign(NumPlants, 5.E6);
	Define(pP, pNumPlants, "Conduct Ris", initv,UNIT_TYPES::HEATFLOW_UNIT, "PlantType >= 3", 1.E6, 500.E6);
	mTab_EvapMulti.push_back(pP);

	pP = new P(&MC_CondVPD, pNumPlants, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	initv.assign(NumPlants, 100.);
	Define(pP, pNumPlants, "Conduct VPD", initv, "Pa", "PlantType >= 3", 10., 10000., 50., 4000.);
	mTab_EvapMulti.push_back(pP);

	pP = new P(&MC_CondMax, pNumPlants, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	initv.assign(NumPlants, 0.02);
	Define(pP, pNumPlants, "Conduct Max", initv, "m/s", "PlantType >= 3", 0.00005, 1., 0.001, .05);
	mTab_EvapMulti.push_back(pP);



	pP = new P(&MC_RoughLMax, pNumPlants, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	initv.assign(NumPlants, 3.0);
	Define(pP, pNumPlants, "Roughness Max", initv, "m", "PlantType >= 3", 0.1, 10.);
	mTab_EvapMulti.push_back(pP);



	pP = (P*)m_pModelMap->GetP_Pointer("DayNumber"); if (pP != nullptr) mTab_EvapSingle.push_back(pP);
	pP = new P(&RoughLength, pNoDayNum, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	initv.assign(NoDayNum, 0.01);
	Define(pP, pNoDayNum, "Roughness Length", initv, "m", "Roughness = 0|Evaporation > 1|PlantType >0|PlantType <3", 0.000001, 10., 0.0001, 5.);
	mTab_EvapSingle.push_back(pP);

	p_vec.clear();
	p_vec.push_back((P*)m_pModelMap->GetP_Pointer("DayNumber"));
	p_vec.push_back((P*)m_pModelMap->GetP_Pointer("CForm"));
	p_vec.push_back(pP);

	functorprop.FuncName = MyFunc::ROUGHNESSLENGTH_DAYOFYEAR; functorprop.Arg = MyArgType::D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Canopy Resistance - Day of year function";
	pFunc = new Func(SINGLEPLANT, PLANT_SHAPE, NOFPROC, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);






	pP = new P(&Displace, pNoDayNum, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	initv.assign(NoDayNum, 0.01);
	Define(pP, pNoDayNum, "Displace", initv, "m", "Displacement = 0|PlantType > 1|PlantType <3", 0., 100., 0., 30.);
	mTab_EvapSingle.push_back(pP);

	p_vec.clear();
	p_vec.push_back((P*)m_pModelMap->GetP_Pointer("DayNumber"));
	p_vec.push_back((P*)m_pModelMap->GetP_Pointer("CForm"));
	p_vec.push_back(pP);

	functorprop.FuncName = MyFunc::DISPLACEMENT_DAYOFYEAR; functorprop.Arg = MyArgType::D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Displacement - Day of year function";
	pFunc = new Func(SINGLEPLANT, PLANT_SHAPE, NOFPROC, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);




	pP = new P(&ResSurface, pNoDayNum, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	initv.assign(NoDayNum, 100.);
	Define(pP, pNoDayNum, "Resistance Surface", initv, "s/m", "RSMethod = 0|PlantType > 1|PlantType <3", 10., 1.E6, 20., 1000.);
	mTab_EvapSingle.push_back(pP);
	// 
	p_vec.clear();
	p_vec.push_back((P*)m_pModelMap->GetP_Pointer("DayNumber"));
	p_vec.push_back((P*)m_pModelMap->GetP_Pointer("CForm"));
	p_vec.push_back(pP);

	functorprop.FuncName = MyFunc::CANOPYRESISTANCE_DAYOFYEAR; functorprop.Arg = MyArgType::D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Canopy Resistance - Day of year function";
	pFunc = new Func(SINGLEPLANT, PLANT_SHAPE, NOFPROC, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);






	pP = (P*)m_pModelMap->GetP_Pointer("AlbedoV"); if (pP != nullptr) mTab_EvapSingle.push_back(pP);
	pP = (P*)m_pModelMap->GetP_Pointer("CanopyHeight"); if (pP != nullptr) mTab_EvapSingle.push_back(pP);


	Tab *pTab;
	pTab = new Tab(&mTab_EvapSingle, SINGLEPLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pTab, "Evapotranspiration - single canopy", "DayNumber|Roughness Length|Displace|Resistance Surface|AlbedoV|CanopyHeight",
		"PlantType > 1|PlantType <3");

	pTab = new Tab(&mTab_EvapMulti, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pTab, "Evapotranspiration - multiple canopies", "Canopy DensMax|Plant AddIndex|Roughness Min|Air Resist LAI Effect|Conduct Ris|Conduct VPD|Conduct Max|Roughness Max",
		"PlantType =3");

	p_vec.clear();
	p_vec.push_back((P*)m_pModelMap->GetP_Pointer("Start DayNo"));
	p_vec.push_back((P*)m_pModelMap->GetP_Pointer("Optimum DayNo"));
	p_vec.push_back((P*)m_pModelMap->GetP_Pointer("End DayNo"));
	p_vec.push_back((P*)m_pModelMap->GetP_Pointer("Shape Start"));
	p_vec.push_back((P*)m_pModelMap->GetP_Pointer("Shape End"));



	pP = new P(&Z_StartValue, pNumPlants, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	initv.assign(NumPlants, 0.1); p_vec.push_back(pP);
	Define(pP, pNumPlants, "Z Start Value", initv, "-", "PlantType >= 3", 0.0001, 1., 0.01, 0.5);
	mTab_RoughMulti.push_back(pP);
	pP = new P(&Z_OptValue, pNumPlants, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	initv.assign(NumPlants, 0.1); p_vec.push_back(pP);
	Define(pP, pNumPlants, "Z Optimum Value", initv, "-", "PlantType >= 3", 0.0001, 1., 0.01, 0.5);
	mTab_RoughMulti.push_back(pP);
	pP = new P(&Z_EndValue, pNumPlants, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	initv.assign(NumPlants, 0.1); p_vec.push_back(pP);
	Define(pP, pNumPlants, "Z End Value", initv, "-", "PlantType >= 3", 0.0001, 1., 0.01, 0.5);
	mTab_RoughMulti.push_back(pP);

	p_vec.push_back((P*)m_pModelMap->GetP_Pointer("Roughness Min"));
	p_vec.push_back((P*)m_pModelMap->GetP_Pointer("Roughness Max"));
	functorprop.FuncName = MyFunc::ROUGHNESSFRACTION_SEASON; functorprop.Arg = MyArgType::D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Roughness Frac of Height - with season";
	pFunc = new Func(PLANT, PLANT_SHAPE, EVAPOTRANSPIRATION, PHOTOSYNTHESIS, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec); p_vec.clear();




	pTab = new Tab(&mTab_RoughMulti, PLANT, ATMOSPHERE, TURBULENCE, NOBPROC, NORMAL);
	Define(pTab, "Roughness coefficients - multiple canopies", "Start DayNo|Optimum DayNo|End DayNo|Shape Start|Shape End|Z Start Value|Z Optimum Value|Z End Value",
		"PlantType > 2|Roughnessfunc =1");

	p_vec.push_back((P*)m_pModelMap->GetP_Pointer("Start DayNo"));
	p_vec.push_back((P*)m_pModelMap->GetP_Pointer("Optimum DayNo"));
	p_vec.push_back((P*)m_pModelMap->GetP_Pointer("End DayNo"));
	p_vec.push_back((P*)m_pModelMap->GetP_Pointer("Shape Start"));
	p_vec.push_back((P*)m_pModelMap->GetP_Pointer("Shape End"));


	pP = new P(&D_StartValue, pNumPlants, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	initv.assign(NumPlants, 0.66);





	Define(pP, pNumPlants, "D Start Value", initv, "-", "PlantType >= 3", 0., 1.);
	mTab_DisplMulti.push_back(pP); p_vec.push_back(pP);
	pP = new P(&D_OptValue, pNumPlants, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	initv.assign(NumPlants, 0.66);
	Define(pP, pNumPlants, "D Optimum Value", initv, "-", "PlantType >= 3", 0., 1.);
	mTab_DisplMulti.push_back(pP); p_vec.push_back(pP);
	pP = new P(&D_EndValue, pNumPlants, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	initv.assign(NumPlants, 0.66); p_vec.push_back(pP);
	Define(pP, pNumPlants, "D End Value", initv, "-", "PlantType >= 3", 0., 1.);
	mTab_DisplMulti.push_back(pP);


	functorprop.FuncName = MyFunc::DISPLACEMENTFRACTION_SEASON; functorprop.Arg = MyArgType::D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Displacement Frac of Height - with season";
	pFunc = new Func(PLANT, PLANT_SHAPE, EVAPOTRANSPIRATION, PHOTOSYNTHESIS, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec); p_vec.clear();



	pTab = new Tab(&mTab_DisplMulti, PLANT, ATMOSPHERE, TURBULENCE, NOBPROC, NORMAL);
	Define(pTab, "Displacement coefficients - multiple canopies", "Start DayNo|Optimum DayNo|End DayNo|Shape Start|Shape End|D Start Value|D Optimum Value|D End Value",
		"PlantType > 2|Roughnessfunc=1");

	ps_vec.clear(); p_vec.clear();
	ps_vec.push_back(m_pModelMap->GetPsPointer("CondMaxWinter"));
	p_vec.push_back((P*)m_pModelMap->GetP_Pointer("Conduct Max"));

	functorprop.FuncName = MyFunc::MAXCONDUCTANCE_MC; functorprop.Arg = MyArgType::BLANK;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Canopy Resistance - Winter - Summer (Multi Plant Canopy)";
	pFunc = new Func(SINGLEPLANT, PLANT_SHAPE, NOFPROC, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, p_vec, plotpar); ps_vec.clear();




	Gs *pGs;
	pGs = new Gs(&CanopyHeightVar, SINGLE, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);


	Define(pGs, "CanopyHeight", 0., "m", "RSMethod >1|PlantType = 2");
	pGs = new Gs(&DisplVar, SINGLE, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pGs, "DisplacementHeight", 0., "m", "PlantType >= 2");
	pGs = new Gs(&RoughLVar, SINGLE, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pGs, "RoughLength", 0., "m", "PlantType = 2");
	pGs = new Gs(&ResSurfVar, SINGLE, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pGs, "ResSurfVegetation", 0., "s/m", "PlantType = 2");
	pGs = new Gs(&ResAir, SINGLE, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pGs, "Resist Air Mean", 0., "s/m", "Evaporation > 1|PlantType >1");
	G* pG;
	pG = new G(&MC_ResAir, pNumPlants, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	initv.assign(NumPlants, 0.);
	Define(pG, pNumPlants, "Resist Air Canopy", initv, "s/m", "Evaporation > 1|PlantType =3");
	pG = new G(&MC_RoughLVar, pNumPlants, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "Roughness Length", initv, "m", "PlantType = 3");
	pG = new G(&MC_DisplVar, pNumPlants, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "Displacement Height", initv, "m", "PlantType = 3");
	pG = new G(&MC_ResSurfVar, pNumPlants, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "Resistance Canopy", initv, "s/m", "PlantType = 3");
	pG = new G(&MC_PotTransp, pNumPlants, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "Pot Transpiration", initv, "-", "PlantType = 3");
	pG = new G(&MC_ResSurfVarMin, pNumPlants, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "Resistance Min Canopy", initv, "s/m", "PlantType = 3");
	pG = new G(&MC_PotTranspMax, pNumPlants, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "Pot MaxTranspiration", initv, "-", "PlantType = 3");
	pG = new G(&MC_ResponseAtm, pNumPlants, PLANT, ATMOSPHERE, POTTRANSPIRATION, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "Response Transp Atm", initv, "-", "PlantType = 3");

	return true;


}

bool Potential_Transpiration::Ini()	{

//	m_pModelStructure = ((Simulator*)m_pModelInfo)->GetSimStructurePointer();

	Plant::Ini();
	//if(LAIInput==1||LAIInput==3&&DrivPG2) return true;

	if(!DrivPG2) {
		if(Roughness==1||Displacement==1||RSMethod==1) 
			DrivPG2=true;
		else if(m_pModelStructure->PlantType<3&&(AlbedoVeg==2||Roughness==1)) 
			DrivPG2=true;
		
		if(DrivPG2)	{
			string filename;
			filename=m_PG_Crop.GetFileName();
			m_PG_Crop.Open(filename);
			m_pModelInfo->Run_Res_Id=CheckEcoDriv2();
			if(!m_pModelInfo->Run_Res_Id) return false;

		}
	}
	Winter=false;
    DisplVar=0.0;

	return true;
}
bool Potential_Transpiration::IniFunctors() {

	//m_pModelStructure = ((Simulator*)m_pModelInfo)->GetSimStructurePointer();
	SnowPack* pMet = (SnowPack*)p_Atm;
	Plant::IniFunctors();
	//if(LAIInput==1||LAIInput==3&&DrivPG2) return true;

	if (!DrivPG2) {
		if (Roughness == 1 || Displacement == 1 || RSMethod == 1)
			DrivPG2 = true;
		else if (m_pModelStructure->PlantType < 3 && (AlbedoVeg == 2 || Roughness == 1))
			DrivPG2 = true;

		if (DrivPG2) {
			string filename;
			filename = m_PG_Crop.GetFileName();
			m_PG_Crop.Open(filename);
			m_pModelInfo->Run_Res_Id = CheckEcoDriv2();
			if (!m_pModelInfo->Run_Res_Id) return false;

		}
	}
	Winter = false;
	DisplVar = 0.0;
	f_CanopyResistance_Time = Get_D1_Functor(MyFunc::CANOPYRESISTANCE_DAYOFYEAR);
	f_Roughness_Time = Get_D1_Functor(MyFunc::ROUGHNESSLENGTH_DAYOFYEAR);
	f_Displacement_Time = Get_D1_Functor(MyFunc::DISPLACEMENT_DAYOFYEAR);
	if (m_pModelStructure->PlantType == 1 && m_pModelStructure->Evaporation >= 1) f_PotTranspiration_Time = Get_D1_Functor(MyFunc::POTTRANSPIRATION_DAYOFYEAR);
	f_CanopyResistance = Get_D2_Functor(MyFunc::CANOPYRESISTANCE);
	f_Conductance_CO2 = Get_D1_Functor(MyFunc::CONDUCTANCE_CO2);


	f_RoughLengthLinear = Get_D3_TableFunctor(MyFunc::ROUGHNESSFRACTION_SEASON);
	f_DisplacementLinear = Get_D2_TableFunctor(MyFunc::DISPLACEMENTFRACTION_SEASON);
	if(pMet->CommonRefHeight==0 ) pMet->RefHeight = pMet->RefHeightWind;

	/*if (m_pModelStructure->PlantType == 2)*/ f_MaxConductance = Get_boolBlank_Functor(MyFunc::MAXCONDUCTANCE);
	/*if (m_pModelStructure->PlantType == 3)*/ f_MaxConductance_mc = Get_boolBlank_TableFunctor(MyFunc::MAXCONDUCTANCE_MC);
	f_Roughness_SingePlant = Get_D2_Functor(MyFunc::SHAWPERRERIA_ROUGHL);
	f_Displacement_SingePlant = Get_D3_Functor(MyFunc::SHAWPERRERIA_DISPLACEMENT);
	if (ResAirStabCorr == 0) { // No correction
		f_ResAirCanopy_Neutral = Get_D3_Functor(MyFunc::RA_CANOPY_NEUTRAL);
		f_ResAirCanopy = Get_D5_Functor(MyFunc::RA_CANOPY_ANALYTICAL);
	}
	else if (ResAirStabCorr == 1) //Richard Number Analytical
		f_ResAirCanopy = Get_D5_Functor(MyFunc::RA_CANOPY_ANALYTICAL);
	else if (ResAirStabCorr == 2) // Monin-Obov
		f_ResAirCanopy = Get_D5_Functor(MyFunc::RA_CANOPY_NUMERICAL);

	return true;
}
bool Potential_Transpiration::Dyn()
{
	Plant::Dyn();
	Flux();
	return true;
}

void Potential_Transpiration::Flux()
{
  double PAI,  MaxHeight, UsedPHeight;
  SnowPack *pMet=(SnowPack*)p_Atm;


   if(m_pModelStructure->PlantType==1&&m_pModelStructure->Evaporation>=1) {
      if(pMet->PotTranspInput==0) {
         pMet->PotTransp=f_PotTranspiration_Time(m_pModelInfo->JDayNum);
         return;
	  }
	  else if(pMet->PotTranspInput==1) {

		  pMet->PotTransp=max(GetEcoDat(&pMet->m_PGPOS_Meteor[pMet->PotEvapIpos.ivar],pMet->PotEvapIpos),0.1E-5);
         return;
      }
   }
   else if(m_pModelStructure->PlantType==2) {

      if(Roughness==2||Displacement==2) {
         if(CanopyHeightInput==0) 
            CanopyHeightVar=f_CanopyHeight_Time(m_pModelInfo->JDayNum);
         else if(CanopyHeightInput==1) 
            CanopyHeightVar=GetEcoDat(&m_PGPos_Crop,HeightIpos);
         
      }

      if(Roughness==0) 
         RoughLVar=f_Roughness_Time(m_pModelInfo->JDayNum);
      else if(Roughness==1) 
         RoughLVar=GetEcoDat(&m_PGPos_Crop,RoughIpos);
      else if(Roughness==2) {
         //!Snowcorrection by DG 29/9-97
        // ! Deleted this addition by PEJ - double correction of LAI !
         //!		  PAI=LAI*SnowReduceLAIFactor+PAddIndex
         PAI=LAI_Total+PAddIndex;
         //! Correction below better but what will happen when snow is more than vegetation ?
         //! when HSnow exceeds CanopyHeight RoughLVar=RoughLmin	and SnowReducaLAIFactor=0

         RoughLVar=ShawP_Rough(CanDensMax, PAI, CanopyHeightVar-pMet->HSnow, RoughLMin, 100.);
		 RoughLVar = f_Roughness_SingePlant(LAI_Total, CanopyHeightVar - pMet->HSnow);
      }

      if(Displacement==0) 
         DisplVar=max(pMet->HSnow,f_Displacement_Time(m_pModelInfo->JDayNum));
      else if(Displacement==1) 
         DisplVar=max(pMet->HSnow, GetEcoDat(&m_PGPos_Crop, DisplIpos));
      else if(Displacement==2) {
         //!Snowcorrection by DG 29/9-97
         //!
         //! Added if-statement to enable 10 to 2m windspeed corrections /DG 2002-06-02
		  if (pMet->CommonRefHeight == 1) {
			  DisplVar = ShawP_Displ(CanDensMax, PAI, pMet->RefHeight, CanopyHeightVar);
			  DisplVar = f_Displacement_SingePlant(LAI_Total, pMet->RefHeight, CanopyHeightVar);
		  }
		  else {
			  DisplVar = ShawP_Displ(CanDensMax, PAI, pMet->RefHeightTemp, CanopyHeightVar);
			  DisplVar = f_Displacement_SingePlant(LAI_Total, pMet->RefHeightTemp, CanopyHeightVar);
		  }
         
      }


	  if (RSMethod >= 2) {
		  //!C *	**** The lohammar equation for calculating surface resistance
		  if (ConductanceCO2_Dependence == 0)
			  ResSurfVar = f_CanopyResistance(f_Conductance(Winter, pMet->RadInShort, pMet->VapourPDef), LAI_Total);
		  else if (ConductanceCO2_Dependence == 1)
			  ResSurfVar = f_CanopyResistance(f_Conductance(Winter, pMet->RadInShort, pMet->VapourPDef)*f_Conductance_CO2(pNC_Plant->CO2_A), LAI_Total);
	  }
         //ResSurfVar=Lohammar(LAI_Total);
	  else if (RSMethod == 0) {
		  ResSurfVar = f_CanopyResistance_Time(m_pModelInfo->JDayNum);
	  }
	  else {
		  //! Driving
		  ResSurfVar = GetEcoDat(&m_PGPos_Crop, ResSurfIpos);
	  }
      
     // !Snowcorrection suggested by DG 29/9-97
      //! i am not sure that this make sens for the vegetation, if vegetation covered
      //! by snow there will be no air resistance defined for vegetation !

      //!	ResAir=RAFunctionPotTransp(RefHeight-(DisplVar+real(Hsnow)), RoughLVar, WindSpeed)
      //! 	ResAir=RAFunctionPotTransp(RefHeight-DisplVar, RoughLVar, WindSpeed)

      //! OBS! New version of RAFunctionPotTransp due to windspeed corrections /DG 2002-06-02
	  if(ResAirStabCorr==0)
		ResAir=f_ResAirCanopy_Neutral(pMet->WindSpeed, pMet->RefHeight - DisplVar, RoughLVar);
	  else
		ResAir = f_ResAirCanopy(pMet->RefHeight - DisplVar,pMet->TAir-TempCanopy, pMet->TAir, pMet->WindSpeed, RoughLVar);

    //  ResAir=RAFunctionPotTransp(pMet->RefHeight-DisplVar, RoughLVar, pMet->WindSpeed,TempCanopy);

     // ! DG 080902:
     // ! DG 2006-12-18 (at home with Jonna, one day befor the dissertation of Magnus Svensson, 
      //!                while stupidly trying to get the model working properoly for the Snowmip2 project)
      //!                Intrdocues a cutoff of Potential Transpiration for snow covered canopies!!!
      if(pMet->SnowReduceLAIFactor.front()>=pMet->SnowReduceLAIThreshold){
         pMet->PotTransp=PMEquation(pMet->TAir, pMet->RadNetTot-pMet->RadNetGround, pMet->VapourPDef, ResAir, ResSurfVar, LAI_Total);
         pMet->PotTransp=max(0.0001, pMet->PotTransp);
	  }
      else {
         pMet->PotTransp = 0.0;
         MC_PotTransp.front()=pMet->PotTransp;
	  }
   }
   else if(m_pModelStructure->PlantType==3) {
         MaxHeight=0.;
         MaxIndex=1;
		 UsedPHeight = MaxHeight;
         for(size_t i=0; i<size_t(NumPlants); i++) {
            if(PlantHeight[i]>= MaxHeight) {
               MaxHeight=PlantHeight[i];
               MaxIndex=i;
            }
		 }
         PAI=LAI[MaxIndex]+MC_PAddIndex[MaxIndex];
         if(MultiRoughness==0) // No - Common
            UsedPHeight=MaxHeight;
      
         

		 if (pMet->RefHeight - MaxHeight*1.2<0.) {

#ifndef COUPSTD
			 MFC_Util::MessageBox("Reference Height(Meteorological Data) too low, a forced adjustment made. \n"
				 "New Reference Height is 1.4 * Max Plant Heigth\n"
				 "Please consider a correct Reference Height above max plant height.\n");
#endif
			 pMet->RefHeight = MaxHeight * 1.4;
		 }
		 DisplVar = ShawP_Displ(MC_CanDensMax[MaxIndex], PAI, pMet->RefHeight, MaxHeight);


		 if(pMet->MC_LAIAbove.size()<NumPlants) {
			 pMet->MC_LAIAbove.resize(NumPlants);
		 }
		 if(pMet->MC_RntDist.size()<NumPlants) {
			 pMet->MC_RntDist.resize(NumPlants);
		 }
         for(size_t i=0; i<size_t(NumPlants); i++) {
            PAI=LAI[i]+MC_PAddIndex[i];
            if(MultiRoughness==1) UsedPHeight=PlantHeight[i]; // Individual
            if(Roughnessfunc==0) {
               MC_RoughLVar[i]=ShawP_Rough(MC_CanDensMax[i], PAI, UsedPHeight-pMet->HSnow, MC_RoughLMin[i], MC_RoughLMax[i]);
              // MC_ResAir[i]=RAFunctionPotTransp(pMet->RefHeight-DisplVar, MC_RoughLVar[i], pMet->WindSpeed, pMet->MC_TempCanopy[i])+pMet->MC_LAIAbove[i]*MC_RaLAI_Effect[i];
			   MC_ResAir[i] = f_ResAirCanopy(pMet->RefHeight - DisplVar, pMet->TAir - pMet->MC_TempCanopy[i], pMet->TAir, pMet->WindSpeed, MC_RoughLVar[i])+pMet->MC_LAIAbove[i] * MC_RaLAI_Effect[i];
			}
            else {
               MC_RoughLVar[i]=f_RoughLengthLinear(i,1.0, UsedPHeight-pMet->HSnow, m_pModelInfo->JDayNum);
             //  MC_DisplVar[i]=Displ_LinFunc(i, UsedPHeight-pMet->HSnow, m_pModelInfo->JDayNum);
			   MC_DisplVar[i] = f_DisplacementLinear(i, UsedPHeight - pMet->HSnow, m_pModelInfo->JDayNum);
              // MC_ResAir[i]=RAFunctionPotTransp(pMet->RefHeight-MC_DisplVar[i], MC_RoughLVar[i], pMet->WindSpeed,pMet->MC_TempCanopy[i])+pMet->MC_LAIAbove[i]*MC_RaLAI_Effect[i];
			   MC_ResAir[i] = f_ResAirCanopy(pMet->RefHeight - MC_DisplVar[i], pMet->TAir - pMet->MC_TempCanopy[i], pMet->TAir, pMet->WindSpeed, MC_RoughLVar[i]) + pMet->MC_LAIAbove[i] * MC_RaLAI_Effect[i];

            }

            
            MC_ResSurfVar[i]=Lohammar_LL(i);
            MC_ResSurfVarMin[i]=CanopyRSMax(i);
            if(pMet->SnowReduceLAIFactor[i]>=pMet->SnowReduceLAIThreshold){
               MC_PotTransp[i]=max(0., PMEquation(pMet->TAir, pMet->MC_RntDist[i], pMet->VapourPDef, MC_ResAir[i], MC_ResSurfVar[i], LAI[i]));
               MC_PotTranspMax[i]=max(0., PMEquation(pMet->TAir, pMet->MC_RntDist[i], pMet->VapourPDef, MC_ResAir[i], MC_ResSurfVarMin[i], LAI[i]));
			}
            else {
               MC_PotTransp[i]=0.0;
               MC_PotTranspMax[i]=0.0;
            }
            
            MC_ResponseAtm[i]=(MC_PotTransp[i]+0.001)/(MC_PotTranspMax[i]+.001);           
            
		 }

         RoughLVar=maximum(MC_RoughLVar);
        
         pMet->PotTransp=sum(MC_PotTransp);
         pMet->PotTranspMax=sum(MC_PotTranspMax);

        // ! DG 2003-08-18: introduced averaging of ResAir in relation to LAI for multiple plants
         //!                just check that sum(LAI) not eq 0!!
         //!	ResAir=Sum(MC_ResAir)/NumPlants
		 vector<double> mult; mult.resize(LAI.size());
		 for(size_t i=0; i<LAI.size(); i++) mult[i]=MC_ResAir[i]*LAI[i];

         ResAir=sum(mult)/max(0.00001,sum(LAI));
      }

}


bool Potential_Transpiration::End()
{
return true;
}
Potential_Transpiration::~Potential_Transpiration(void)
{
}
double Potential_Transpiration::Lohammar(double LAI) {
	double Conduct;
	SnowPack *pMet = (SnowPack*)p_Atm;


	//!  This subroutine calculate the surface resistance from the Lohammar
	//!  equation and the leaf area index of canopy

	if (RSMethod == 3 && Winter)
		Conduct = pMet->RadInShort / (pMet->RadInShort + CondRis)*CondMaxWinter / (1 + pMet->VapourPDef / CondVPD);
	else
		Conduct = pMet->RadInShort / (pMet->RadInShort + CondRis)*CondMax / (1 + pMet->VapourPDef / CondVPD);


	return 1 / max(Conduct*LAI, 0.001);

}




double Potential_Transpiration::CanopyRSMax(size_t Lindex) {
	double Conduct;
	if (Winter)
		Conduct = CondMaxWinter;
	else
		Conduct = MC_CondMax[Lindex];

	return 1 / max(Conduct*LAI[Lindex], 0.001);
}
double Potential_Transpiration::Lohammar_LL(size_t index) {

	double Conduct;
	SnowPack *pMet = (SnowPack*)p_Atm;

	//!  This subroutine calculate the surface resistance from the Lohammar
	//!  equation and the leaf area index of canopy
	if (Winter) {
		Conduct = pMet->RadInShort*pMet->RadFractions[index] / (pMet->RadInShort*pMet->RadFractions[index] + MC_CondRis[index])*
			CondMaxWinter / (1 + pMet->VapourPDef / MC_CondVPD[index]);
	}
	else {
		Conduct = pMet->RadInShort*pMet->RadFractions[index] / (pMet->RadInShort*pMet->RadFractions[index] + MC_CondRis[index])*
			MC_CondMax[index] / (1 + pMet->VapourPDef / MC_CondVPD[index]);
	}



	double scaleCoef = 1;
	if (ConductanceCO2_Dependence == 1) {
		scaleCoef = f_Conductance_CO2(pNC_Plant->CO2_A);
	}
	else if (ConductanceCO2_Dependence == 2) {
		scaleCoef = f_Conductance_CO2(pNC_Plant->PCO2_Surface[index]);
	}
	return 1 / max(Conduct*LAI[index] * scaleCoef, 0.001);
}

double Potential_Transpiration::Lohammar_Ris(double Radin) {
	return Radin / (Radin + CondRis);
}

double Potential_Transpiration::Lohammar_Vpd(double vpd) {

	return 1. / (1. + vpd / CondVPD);
}

double Potential_Transpiration::Lohammar_Ris_L(size_t index, double Radin) {
	return Radin / (Radin + MC_CondRis[index]);
}

double Potential_Transpiration::Lohammar_Vpd_L(size_t index, double vpd) {
	return 1. / (1. + vpd / MC_CondVPD[index]);
}
