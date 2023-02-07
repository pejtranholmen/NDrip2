#include "SnowPack_Functions.h"
#include "../SOIL/SoilModel.h"
#include "../PLANT/PlantModel.h"
#include "../PhysConst.h"
#include "../../Simulator/Simulator.h"
SnowPack_Functions::SnowPack_Functions(void)
{
}
SnowPack_Functions::~SnowPack_Functions(void)
{
}



bool SnowPack_Functions::Def()
{
 	Radiation::Def();
	ModuleNo=14;
	ModuleConditions="SnowPack>0";
	

 	PlantModel* pPlant=(PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
	p_Plant=&pPlant->m_WaterUptake;	


//    ! Snow surface energy balance
	Sw* pSw;
	pSw= new Sw(&SnowSurEbal,SINGLE,SNOW, STORAGE,NOBPROC, NORMAL);
    Define(pSw  ,"SnowSurfTemperature",0,"Air Temperature|f(E-balance Solution)","WaterEq > 0|SnowPack >0",201);

	pSw= new Sw(&StabilitySnow ,SINGLE,SNOW, TURBULENCE,NOBPROC, NORMAL);
    Define(pSw    ,"StabilityCorrection",1,"f(Richardson Number)|f(Monin-Obukhov Length)","WaterEq > 0|SnowPack >0|SnowSurfTemperature >0");

//    ! Snow melt
	pSw= new Sw(&SnowMeltFunctionSwitch ,SINGLE,SNOW, SNOW_MELTING,NOBPROC, NORMAL);
     Define(pSw  	,"SnowMeltFunction",0,"Empirical|Heat balance","WaterEq > 0|SnowPack >0|SnowSurfTemperature >0",202);

//    ! Snow density
	pSw= new Sw(&NewSnowDensityModel ,SINGLE,SNOW, SHRINKING,NOBPROC, NORMAL);
    Define(pSw 	,"NewSnowDensity",0,"Linear f(air temp)|Exponential f(air temp)","WaterEq > 0|SnowPack >0");
 	
	pSw= new Sw(&SnowDensification ,SINGLE,SNOW, SHRINKING,NOBPROC, NORMAL);
    Define(pSw,"SnowDensification",0,"f(ice and liq. content)|f(compaction rate)","WaterEq > 0|SnowPack >0");

//    ! Snow adjustment
//    ! ---------------
 	pSw= new Sw(&SnowAdjustment ,SINGLE,SNOW, STORAGE,NOBPROC, NORMAL);
    Define(pSw 	,"SnowAdjustment",0,"No correction|Forced to match continous|Forced to match discrete","WaterEq > 0|SnowPack >0");

 //   ! Snow Roughness
 	pSw= new Sw(&SnowRoughness ,SINGLE,SNOW, TURBULENCE,NOBPROC, NORMAL);
    Define(pSw 	,"SnowRoughness",0,"Individual|Common roughness","WaterEq > 0|SnowPack >0|SnowSurfTemperature >0");

 //   ! Snow Ploughing
 	pSw= new Sw(&SnowPloughing ,SINGLE,SNOW, STORAGE,NOBPROC, NORMAL);
    Define(pSw 	,"SnowPloughing",0,"off|Driven by SnowMass","WaterEq > 0|SnowPack >0");
 	pSw= new Sw(&SnowPloughInput ,SINGLE,SNOW, STORAGE,NOBPROC, NORMAL);
    Define(pSw 	,"SnowPloughInput",0,"off|LateralInput File","WaterEq > 0|SnowPack >0|LateralInput = 1");
 	pSw= new Sw(&ChlorideMelt ,SINGLE,SNOW, SNOW_MELTING,NOBPROC, NORMAL);
    Define(pSw 	,"RoadSalt Melting",0,"off|on","WaterEq > 0|SnowPack >0|SaltRoadStorage >0");


	Ps* pPs;
 
//    ! Parameters

	Func *pFunc;
	string funcname;

	FunctorProp functorprop;

	vector<Ps*> ps_vec;
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



	ps_vec.clear();

 //   ! Snow surface age
 	pPs= new Ps(&SAgeZP ,SINGLE,SNOW, SNOW_MELTING,NOBPROC, NORMAL);
    Define(pPs	,"AgeUpdatePrec",5.,"Kg/m2day","SnowPack >= 1",0.,20.);	
	pPs= new Ps(&SAgeZQ ,SINGLE,SNOW, SNOW_MELTING,NOBPROC, NORMAL);
     Define(pPs			,"AgeUpdatePrecThQ",.9,"-","SnowPack >= 1", 0.0,1.);    	
 	pPs= new Ps(&AgeUpdateSnowDepthCorr ,SINGLE,SNOW, SNOW_MELTING,NOBPROC, NORMAL);
    Define(pPs  ,"AgeUpdateSDepthCorr",0.01,"m/day","SnowPack >= 1|SnowAdjustment=1",0.,0.1);

//    ! Snow mass balance

 	pPs= new Ps(&SRetens ,SINGLE,SNOW, STORAGE,NOBPROC, NORMAL);
    Define(pPs 			,"WaterRetention",.07,"-","SnowPack >= 1",0.001, 0.5, 0.02, 0.1);
	ps_vec.clear();

	functorprop.FunctorInitOption = string::npos;
	functorprop.FuncName = MyFunc::PRECCORR;	functorprop.Arg = MyArgType::D1; funcname = "Precipitation Correction Function";

	plotpar.Num_Of_Curves = 1;
	plotpar.Num_Of_Points = 300;
	plotpar.Num_Of_X = 1;
	plotpar.X_Min = -5;
	plotpar.X_Max = 5;
	plotpar.X_Label = "Temperature";  plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::TEMP_UNIT, true);
	plotpar.Y_Label = "Factor "; plotpar.Y_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::NO_UNIT, true);
 
	pPs= new Ps(&PRainLim ,SINGLE,SNOW, STORAGE,NOBPROC, NORMAL);	ps_vec.push_back(pPs);
	Define(pPs, "OnlyRainPrecTemp", 2., "C", "SnowPack >= 1", -4., 10.);
	pPs = new Ps(&PSnowLim, SINGLE, SNOW, STORAGE, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "OnlySnowPrecTemp", 0., "C", "SnowPack >= 1", -10., 4.);
	
	ps_vec.push_back(m_pModelMap->GetPsPointer("PrecA0Corr"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("PrecA1Corr"));

	pFunc = new Func(ATMBOUNDARY, PRECIPITATION, NOFPROC, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);	 	ps_vec.clear();

	ps_vec.push_back(m_pModelMap->GetPsPointer("OnlyRainPrecTemp"));

	pPs = new Ps(&SDensNew, SINGLE, SNOW, SHRINKING, NOBPROC, NORMAL); ps_vec.push_back(pPs); 
	Define(pPs, "DensityOfNewSnow", 100., "Kg/m3", "SnowPack >= 1|WaterEq > 0", 10., 300., 50., 200.);

	functorprop.FuncName = MyFunc::NEWSNOW_DENSITY_LINEAR; functorprop.Arg = MyArgType::D2;

	funcname = "Density of  Snow - Linear";
	pFunc = new Func(SINGLE, SNOW, TRANSPORT, NOBPROC, NORMAL);
	plotpar.X_Min = 0;
	plotpar.X_Max = 1.;
	plotpar.X_Label = "Degree of Ice in Snow";
	plotpar.Y_Label = "Density of Snow (kg/m3)";
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.Num_Of_Opt = 1;


	Define(pFunc, functorprop, funcname,  ps_vec, plotpar);

	functorprop.FuncName = MyFunc::NEWSNOW_DENSITY_EXP; functorprop.Arg = MyArgType::D2;
	funcname = "Density of New Snow - Exp";
	pFunc = new Func(SINGLE, SNOW, TRANSPORT, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname,  ps_vec); ps_vec.clear(); 



	pPs= new Ps(&SnowDepthInitial ,SINGLE,SNOW, STORAGE,NOBPROC, NORMAL);
    Define(pPs 	,"SnowDepthInitial", 0., "m", "SnowPack >= 1",0.,10.);
 	pPs= new Ps(&SnowMassInitial ,SINGLE,SNOW, STORAGE,NOBPROC, NORMAL);
    Define(pPs 	,"SnowMassInitial", 0., "mm", "SnowPack >= 1",0.,4000.);
 	
 	pPs= new Ps(&SDensNewImport ,SINGLE,SNOW, SHRINKING,NOBPROC, NORMAL);
    Define(pPs 	    ,"DensityOfImportSnow",300.,"Kg/m3","SnowPack >= 1|WaterEq > 0|SnowPloughInput>0",100.,920.);

 //   ! Snow heat balance
 	pPs= new Ps(&Temp0Li ,SINGLE,SNOW, LOWERBOUNDARY,NOBPROC, NORMAL);
    Define(pPs 				,"ZeroTemp_WaterLimit",3.,"Kg/m2","SnowPack >= 1",0.01, 1000.);

	ps_vec.clear();

functorprop.FuncName = MyFunc::SNOW_THERMALCOND; functorprop.Arg = MyArgType::D1;
	funcname = "Thermal Conductivity of Snow";
	plotpar.X_Min = 100;
	plotpar.X_Max = 900;
	plotpar.Y_Label = "Thermal Conductivity ";  plotpar.Y_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::THERMAL_CONDUCTIVY_UNIT, true);
	plotpar.X_Label = "Density "; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::KILO_MASSDENSITY_UNIT, true);

 	pPs= new Ps(&SThCon ,SINGLE,SNOW, HEAT_CONVECTION,NOBPROC, NORMAL); ps_vec.push_back(pPs); 
    Define(pPs,"SThermalCondCoef",2.86E-6,"Wm5/°C·Kg²","SnowPack >= 1", 1.E-9,1.E-5);
 	

	pFunc = new Func(SINGLE, SNOW, TRANSPORT, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname,  ps_vec, plotpar); ps_vec.clear(); 

	
	
	
	

functorprop.FuncName = MyFunc::RA_SNOW_NEUTRAL; functorprop.Arg = MyArgType::D2;
	funcname = "Aerodynamic Resistance Snow - Neutral ";
	functorprop.pSimModule = this;

	plotpar.X_Min = 0;
	plotpar.X_Max = 15.;
	plotpar.Y_Label = "Resistance ";  plotpar.Y_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::AERODYNAMIC_RESISTANCE_UNIT, true);
	plotpar.X_Label = "Wind Speed at 2m height"; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::WIND_SPEED_UNIT, true);
	plotpar.X2_Vec = { 2. };
	plotpar.LogYScale = true;

 	pPs= new Ps(&WindlessExChangeSnow ,SINGLE,SNOW, TURBULENCE,NOBPROC, NORMAL); 	ps_vec.push_back(pPs);
    Define(pPs 	,"WindlessExChangeSnow",0.,"s-1","SnowPack >=1|WaterEq > 0|SnowSurfTemperature>=1",0.,0.01);
	ps_vec.push_back(m_pModelMap->GetPsPointer("KBMinusOne"));
	pPs = new Ps(&RoughLMomSnow, SINGLE, SNOW, TURBULENCE, NOBPROC, NORMAL);  ps_vec.push_back(pPs);
	Define(pPs, "RoughLMomSnow", 0.001, "m", "SnowPack >=1|PlantType =< 2|WaterEq > 0|SnowSurfTemperature>=1", 0.00001, 5., 0.0001, .05);

	pFunc = new Func(SINGLE, SNOW, TRANSPORT, NOBPROC, NORMAL);
	
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);  ps_vec.clear();

	functorprop.FuncName = MyFunc::RA_SNOW_ANALYTICAL; functorprop.Arg = MyArgType::D4;
	funcname = "Aerodynamic Resistance Snow - Analytical Stability)";
	plotpar.Num_Of_Curves = 3;
	plotpar.X2_Vec = { 2., 2.,2, };
	plotpar.X3_Vec = { 1., 0., -1. };
	plotpar.X4_Vec = { 0., 10., 20. };
	plotpar.LegendsVec = { "Stable", "Neutral", "Forced" };

	ps_vec.push_back(m_pModelMap->GetPsPointer("WindlessExChangeSnow"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("KBMinusOne"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("RoughLMomSnow"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("StabCoefStableRich"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("StabCoefStableExp"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("StabCoefUnstableRich"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("StabCoefUnstableExp"));


	pFunc = new Func(SINGLE, SNOW, TRANSPORT, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);



	functorprop.FuncName = MyFunc::RA_SNOW_NUMERICAL; functorprop.Arg = MyArgType::D4;
	funcname = "Aerodynamic Resistance Snow - Monin-Obokuv Stability)";
	pFunc = new Func(SINGLE, SNOW, NOFPROC, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar); ps_vec.clear();


	functorprop.FuncName = MyFunc::BARESOIL_SNOWHEIGHT; functorprop.Arg = MyArgType::D1;
	funcname = "Degree of Bare Soil Function";
	plotpar.Num_Of_Curves = 1;
	plotpar.X_Min = 0;
	plotpar.X_Max = .2;
	plotpar.LogYScale = false;
	plotpar.LegendsVec = {""};
	plotpar.X_Label = "Snow Height "; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::LENGTH_UNIT, true);
	plotpar.Y_Label = "Bare Soil "; plotpar.Y_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::NO_UNIT, true);

	pPs = new Ps(&HCSnowCover, SINGLE, SNOW, LOWERBOUNDARY, NOBPROC, NORMAL); ps_vec.push_back(pPs);
    Define(pPs 	,"CritDepthSnowCover", .01, "m", "SnowPack >= 1",0.0001,3.,0.01,0.3);
	pFunc = new Func(SINGLEPLANT, PLANT_SHAPE, NOFPROC, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);



 //   ! Empirical snow melt model
	functorprop.FuncName = MyFunc::SNOW_MELTING_EMPIRICAL;
	functorprop.Arg = MyArgType::D5;
	funcname = "Empirical Snow Melt Function";

	plotpar.Num_Of_Curves = 3;
	plotpar.X_Min = -2;
	plotpar.X_Max = 5.;
	plotpar.X_Label = "Air Temperature "; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::TEMP_UNIT, true);
	plotpar.Y_Label = "Melting Rate "; plotpar.Y_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::WATERFLOW_UNIT, true);
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.Num_Of_Opt = 1;
	plotpar.LegendsVec = { "No Radiation", "Radiation 200 W/m2", "Radiation 400 W/m2" };
	plotpar.X2_Vec = { 0.f, 1.75E7f, 3.5E7f };
	plotpar.X3_Vec = { 10.f,10.f,10.f };
	plotpar.X4_Vec = { 0.f,0.f,0.f };



	ps_vec.clear(); 
	pPs = new Ps(&MeltRis, SINGLE, SNOW, SNOW_MELTING, NOBPROC, NORMAL); ps_vec.push_back(pPs); 
	Define(pPs, "MeltCoefGlobRad", 1.5E-7, "Kg/J", "SnowPack >= 1|SnowMeltFunction=0", 0., 3.E-6);
 	pPs= new Ps(&SAgeM1 ,SINGLE,SNOW, SNOW_MELTING,NOBPROC, NORMAL); ps_vec.push_back(pPs); 
    Define(pPs 		,"MeltCoefGlobRadAge1",2.,"-","SnowPack >= 1|SnowMeltFunction=0", 0.,10.);	
 	pPs= new Ps(&SAgeM2 ,SINGLE,SNOW, SNOW_MELTING,NOBPROC, NORMAL); ps_vec.push_back(pPs); 
    Define(pPs 		,"MeltCoefGlobRadAge2",0.1,"1/day","SnowPack >= 1|SnowMeltFunction=0",0.,1.);	
	pPs = new Ps(&MeltTemp, SINGLE, SNOW, SNOW_MELTING, NOBPROC, NORMAL); ps_vec.push_back(pPs); 
	Define(pPs, "MeltCoefAirTemp", 2., "Kg/Cm2day", "SnowPack >= 1|SnowMeltFunction=0", 0., 10.);
	pPs= new Ps(&MeltReFr ,SINGLE,SNOW, SNOW_MELTING,NOBPROC, NORMAL); ps_vec.push_back(pPs); 
    Define(pPs 	,"MeltCoefReFreeze",0.1,"m","SnowPack >= 1|SnowMeltFunction=0",0., 1.);
 	pPs= new Ps(&MeltFSoil ,SINGLE,SNOW, SNOW_MELTING,NOBPROC, NORMAL); ps_vec.push_back(pPs); 
    Define(pPs 	,"MeltCoefSoilHeatF",.5,"-","SnowPack >= 1|SnowMeltFunction=0",0., 1.);
	
	pFunc = new Func(SINGLE, SNOW, SNOW_MELTING, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();

 //   ! Compaction of snow: Jansson(1998)
	functorprop.FuncName = MyFunc::SIMPLE_SNOWDENSITY;
	functorprop.Arg = MyArgType::D3;
	funcname = "Simple Snow Density Function";

	plotpar.Num_Of_Curves = 3;
	plotpar.X_Min = 0;
	plotpar.X_Max = 300.;
	plotpar.X_Label = "Amount of Snow "; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::WATERMASS_UNIT, true);
	plotpar.Y_Label = "Snow Density "; plotpar.Y_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::KILO_MASSDENSITY_UNIT, true);
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.Num_Of_Opt = 1;
	plotpar.LegendsVec = { "Dry Snow", "Half Saturated", "Saturated"};
	plotpar.X2_Vec = { 0.f, .5f,	1.f };
	plotpar.X3_Vec = { 50.f, 50.f, 50.f };

	ps_vec.push_back(m_pModelMap->GetPsPointer("DensityOfNewSnow"));
 	pPs= new Ps(&SDensLi ,SINGLE,SNOW, SHRINKING,NOBPROC, NORMAL);
	Define(pPs, "DensityCoefWater", 200., "Kg/m3", "SnowPack >= 1|WaterEq > 0|SnowDensification=0", 0., 800., 50., 200.); ps_vec.push_back(pPs);
 	pPs= new Ps(&SDensMass ,SINGLE,SNOW, SHRINKING,NOBPROC, NORMAL);
    Define(pPs 	,"DensityCoefMass",0.5,"1/m","SnowPack >= 1|WaterEq > 0|SnowDensification=0", 0., 4.);  ps_vec.push_back(pPs);

	pFunc = new Func(SINGLE, SNOW, SHRINKING, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();




 //   ! Compaction of snow: Jordan(1991)
 	pPs= new Ps(&CRMetaMorphTemp1 ,SINGLE,SNOW, SHRINKING,NOBPROC, NORMAL);
    Define(pPs  	,"CRMetaMorphTemp1"	  ,2.777e-6	,"-","SnowPack >=1|WaterEq > 0|SnowDensification>=1",0.,1000.);
 	pPs= new Ps(&CRMetaMorphTemp2 ,SINGLE,SNOW, SHRINKING,NOBPROC, NORMAL);
    Define(pPs  	,"CRMetaMorphTemp2"	  ,0.04		,"-","SnowPack >=1|WaterEq > 0|SnowDensification>=1",0.,1000.);
 	pPs= new Ps(&CRMetaMorphLiq ,SINGLE,SNOW, SHRINKING,NOBPROC, NORMAL);
    Define(pPs  	,"CRMetaMorphLiq"	  ,2.0		,"-","SnowPack >=1|WaterEq > 0|SnowDensification>=1",0.,1000.);
 	pPs= new Ps(&CRMetaMorphDens ,SINGLE,SNOW, SHRINKING,NOBPROC, NORMAL);
    Define(pPs  	,"CRMetaMorphDens"	  ,46.0e-3	,"-","SnowPack >=1|WaterEq > 0|SnowDensification>=1",0.,1000.);
 	pPs= new Ps(&CRMetaMorphDensMin ,SINGLE,SNOW, SHRINKING,NOBPROC, NORMAL);
    Define(pPs  ,"CRMetaMorphDensMin" ,100.0	,"-","SnowPack >=1|WaterEq > 0|SnowDensification>=1",0.,1000.);
 	pPs= new Ps(&CROverburdenDens ,SINGLE,SNOW, SHRINKING,NOBPROC, NORMAL);
    Define(pPs  	,"CROverburdenDens"	  ,23.e-3	,"-","SnowPack >=1|WaterEq > 0|SnowDensification>=1",0.,1000.);
 	pPs= new Ps(&CROverburdenTemp ,SINGLE,SNOW, SHRINKING,NOBPROC, NORMAL);
    Define(pPs  	,"CROverburdenTemp"	  ,0.04		,"-","SnowPack >=1|WaterEq > 0|SnowDensification>=1",0.,1000.);
 	pPs= new Ps(&CROverburdenVisc ,SINGLE,SNOW, SHRINKING,NOBPROC, NORMAL);
    Define(pPs  	,"CROverburdenVisc"	  ,9.e5		,"-","SnowPack >=1|WaterEq > 0|SnowDensification>=1",0.,100000000.);
 	pPs= new Ps(&CRCompMeltCutOff ,SINGLE,SNOW, SHRINKING,NOBPROC, NORMAL);
    Define(pPs  	,"CRCompMeltCutOff"	  ,300.0	,"-","SnowPack >=1|WaterEq > 0|SnowDensification>=1",0.,1000.);

//    ! Snow Ploughing parameter
 	pPs= new Ps(&PloughSnowMass ,SINGLE,SNOW, STORAGE,NOBPROC, NORMAL);
    Define(pPs  	,"PloughSnowMass"	  ,3.0	,"-","SnowPack >=1|WaterEq > 0|SnowPloughing>=1",0.,1000.);
 	pPs= new Ps(&PloughEffSnow ,SINGLE,SNOW, STORAGE,NOBPROC, NORMAL);
    Define(pPs  		,"PloughEffSnow"	  ,0.9	,"-","SnowPack >=1|WaterEq > 0|SnowPloughing>=1",0.,1.);


	ps_vec.clear(); 
	functorprop.FuncName = MyFunc::ROADICE_SALTMELT;
	functorprop.Arg = MyArgType::D2;
	plotpar.Num_Of_Curves = 4;
	plotpar.X_Min = -5;
	plotpar.X_Max = 0.;
	plotpar.X_Label = "Air Temperature "; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::TEMP_UNIT, true);
	plotpar.Y_Label = "Snow Melting "; plotpar.Y_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::WATERMASS_UNIT, true);
	plotpar.LogXScale = false;
	plotpar.LogYScale = true;
	plotpar.Num_Of_Opt = 1;
	plotpar.LegendsVec = { "50 ", "100 ", "150 ","200 " };

	for (size_t i = 0; i < 4; ++i) plotpar.LegendsVec[i]+= p_ModelInfo->GetUnitString(UNIT_TYPES::mMASS_UNIT, true);
	plotpar.X2_Vec = { 50.f,	100.f , 150.f, 200.f};

 	pPs= new Ps(&ChlorideMeltConc ,SINGLE,SNOW, SNOW_MELTING,NOBPROC, NORMAL); ps_vec.push_back(pPs); 
    Define(pPs  		,"ChlorideMeltConc"	  ,900.,"mg/l","SnowPack >=1|WaterEq > 0|SnowPloughing>=1|RoadSalt Melting>0",0.,1.E5);

	funcname = "Road Ice Melting by Salt";
	pFunc = new Func(SINGLE, SNOW, SNOW_MELTING, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname,  ps_vec, plotpar);


 	pPs= new Ps(&SnowReduceLAIThreshold ,SINGLE,SNOW, INTERCEPTION,NOBPROC, NORMAL);
    Define(pPs ,"SnowReduceLAIThreshold",0.001,"-","SnowPack >= 1|WaterEq > 0|PlantType >= 2|Evaporation > 0",0.0001, 1., 0.001, .1);


	
	functorprop.FuncName = MyFunc::NO_PARFUNC; 
	functorprop.PhysFuncName = FUNC_CHOICE::SENSHEAT_AIRFLUX; functorprop.Arg = MyArgType::BLANK;
	funcname = "Sensible heat flux as Function Resistance";
	pFunc = new Func(SOILSURFACE, HEAT, SOILEVAPORATION, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname,  ps_vec);

	functorprop.FuncName = MyFunc::NO_PARFUNC;
	functorprop.PhysFuncName = FUNC_CHOICE::LATENTHEAT_AIRFLUX; functorprop.Arg = MyArgType::BLANK;
	funcname = "Latent heat flux as Function Resistance";
	pFunc = new Func(SOILSURFACE, HEAT, SOILEVAPORATION, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname,  ps_vec);


	Xs* pXs;
	pXs= new Xs(&WTotSnow,SINGLE,SNOW, STORAGE,NOBPROC, NORMAL);
		Define(pXs, "TotalSnowMass", 0.,"mm" );
	pXs= new Xs(&HSnow,SINGLE,SNOW, STORAGE,NOBPROC, NORMAL);
		Define(pXs,"Snow Depth"   ,0., "m");
	pXs= new Xs(&SWATS, SINGLE,	SNOW, STORAGE, NOBPROC, NORMAL);
		Define(pXs,"WaterInSnowPack"  , 0., "Kg/m³");
	pXs= new Xs(&WICE, SINGLE,	SNOW, EBAL_PARTITIONING, NOBPROC, NORMAL);
		Define(pXs,   "IceInSnowPack"  , 0.,"Kg/m³","SnowPack >= 1|HeatEq > 0|SnowMeltFunction>=1");


//   ! Flow Variable	(August 2005)
	Ts* pTs;
	pTs= new Ts(&SnowExport,SINGLE,SNOW, TRANSPORT,NOBPROC, NORMAL);
	    Define(pTs,"Snow mass export", 0.0,UNIT_TYPES::WATERFLOW_UNIT, "SnowPack>=1|SnowPloughing>=1");
	pTs= new Ts(&SnowImport,SINGLE,SNOW, TRANSPORT,NOBPROC, NORMAL);    
		Define(pTs,"Snow mass import", 0.0,UNIT_TYPES::WATERFLOW_UNIT , "SnowPack>=1|SnowPloughinput>=1");

	pTs= new Ts(&SnowOutFlow, SINGLE,	SNOW, TRANSPORT, NOBPROC, NORMAL);
		Define(pTs,"SnowWaterOutflow",0.,"mm/day");
	pTs= new Ts(&SnowInFlow, SINGLE,	SNOW, TRANSPORT, NOBPROC, NORMAL);
		Define(pTs,"SnowInflow",0.,"mm/day");
	pTs= new Ts(&SnowEvaporation, SINGLE,	SNOW, EBAL_PARTITIONING, NOBPROC, NORMAL);
		Define(pTs ,"SnowEvaporation"  , 0.,"mm/day","SnowPack >= 1|HeatEq > 0|SnowSurfTemperature>=1");
 

	//    ! Always visible
    //! --------------
	Gs *pGs;
	pGs= new Gs(&DENSS, SINGLE,	SNOW, SHRINKING, NOBPROC, NORMAL);
		Define(pGs,"Snow Density"  , 0., "Kg/m³")	;
	pGs= new Gs(&SnowAge, SINGLE,	SNOW, SHRINKING, NOBPROC, NORMAL);
	    Define(pGs,"SnowSurfaceAge"   , 0., "days");
	pGs= new Gs(&ThQualP, SINGLE,	SNOW, SHRINKING, NOBPROC, NORMAL);
		Define(pGs ,"ThermQualOfThroughF" , 0., "-");
	pGs= new Gs(&FracBare, SINGLE,	SNOW, SHRINKING, NOBPROC, NORMAL);
		Define(pGs,"FracBareSoil",0.,"-");
	pGs= new Gs(&TempSnowSoil, SINGLE,	SNOW, SHRINKING, NOBPROC, NORMAL);
		Define(pGs,"TempSoilSUnderSnow",0.,"°C");
	
	G* pG;
	vector<double> initv;
	initv.assign(p_Plant->NumPlants,1.);
	NE *pNumPlants=m_pModelMap->GetNEPointer("Number of Plants");
	pG= new G(&SnowReduceLAIFactor,pNumPlants, PLANT,	SNOW, FYS_SHAPE, NOBPROC, NORMAL);
 //   ! Only if vegetation is explicit
    Define(pG, pNumPlants, "SnowReduceLAIFactor",initv,"-","SnowPack >= 1|WaterEq > 0|PlantType >= 2|Evaporation > 0");

//    ! Snow surface energy balance variables
 
//    ! snow surface temperature
	pGs= new Gs(&TempSnowSurf, SINGLE,	SNOW, EBAL_PARTITIONING, NOBPROC, NORMAL);
 
    Define(pGs ,"TempSnowSurface"  , 0.,"C","SnowPack >= 1|HeatEq > 0|SnowSurfTemperature>=1");

 //   ! H,LE,G & Rn
	 pGs= new Gs(&SnowSensF, SINGLE,	SNOW, EBAL_PARTITIONING, NOBPROC, NORMAL);
     Define(pGs ,"SnowSensibleFlow"  , 0.,UNIT_TYPES::HEATFLOW_UNIT,"SnowPack >= 1|HeatEq > 0|SnowSurfTemperature>=1");
 	 pGs= new Gs(&SnowLatentF, SINGLE,	SNOW, EBAL_PARTITIONING, NOBPROC, NORMAL);
    Define(pGs ,"SnowLatentFlow"  , 0.,UNIT_TYPES::HEATFLOW_UNIT,"SnowPack >= 1|HeatEq > 0|SnowSurfTemperature>=1");
 	 pGs= new Gs(&SnowSurfHeatF, SINGLE,	SNOW, EBAL_PARTITIONING, NOBPROC, NORMAL);
    Define(pGs ,"SnowSurfHeatFlow"  , 0.,UNIT_TYPES::HEATFLOW_UNIT,"SnowPack >= 1|HeatEq > 0|SnowSurfTemperature>=1");
 	 pGs= new Gs(&RadNetSnowCover, SINGLE,	SNOW, EBAL_PARTITIONING, NOBPROC, NORMAL);
    Define(pGs ,"RadNetSnowCover"  , 0.,UNIT_TYPES::HEATFLOW_UNIT,"SnowPack >= 1|HeatEq > 0|SnowSurfTemperature>=1");
 	 pGs= new Gs(&RadNetShortSnow, SINGLE,	SNOW, EBAL_PARTITIONING, NOBPROC, NORMAL);
    Define(pGs ,"RadNetShortSnow"  , 0.,UNIT_TYPES::HEATFLOW_UNIT,"SnowPack >= 1|HeatEq > 0|SnowSurfTemperature>=1");
 	 pGs= new Gs(&RadNetLongSnow, SINGLE,	SNOW, EBAL_PARTITIONING, NOBPROC, NORMAL);
    Define(pGs ,"RadNetLongSnow"  , 0.,UNIT_TYPES::HEATFLOW_UNIT,"SnowPack >= 1|HeatEq > 0|SnowSurfTemperature>=1");

//    ! Rn-H-LE-G=residual energy balance error
 	 pGs= new Gs(&SnowEbal, SINGLE,	SNOW, EBAL_PARTITIONING, NOBPROC, NORMAL);
    Define(pGs ,"SnowEbalClosure"  , 0.,UNIT_TYPES::HEATFLOW_UNIT,"SnowPack >= 1|HeatEq > 0|SnowSurfTemperature>=1");
 	 pGs= new Gs(&RaSnowSurf, SINGLE,	SNOW, EBAL_PARTITIONING, NOBPROC, NORMAL);
    Define(pGs ,"ResAirAboveSnow"  , 0.,"s/m","SnowPack >= 1|HeatEq > 0|SnowSurfTemperature>=1");
 	 pGs= new Gs(&VapourPSnowSurf, SINGLE,	SNOW, EBAL_PARTITIONING, NOBPROC, NORMAL);
    Define(pGs ,"VapourPSnowSurface"  , 0.,"PA","SnowPack >= 1|HeatEq > 0|SnowSurfTemperature>=1");

   // ! Auxiliaries in Monin-Obukhov stability correction
 	 pGs= new Gs(&EtaSnow, SINGLE,	SNOW, EBAL_PARTITIONING, NOBPROC, NORMAL);
     Define(pGs ,"MO-StabParSnow"  , 0.,"-","SnowPack >= 1|WaterEq > 0|SnowSurfTemperature>=1|StabilityCorrection>=1");
  //  !	call Gs(Group_14,WindspeedError,"WindSpeedErrorSnow"  , 0.,"-","SnowPack >= 1|WaterEq > 0|SnowSurfTemperature>=1|StabilityCorrection>=1")
  //  !	call Gs(Group_14,UstarSnow,"UstarSnow"  , 0.,"-","SnowPack >= 1|WaterEq > 0|SnowSurfTemperature>=1|StabilityCorrection>=1")
 	 pGs= new Gs(&WindSpeedSnow, SINGLE,	SNOW, EBAL_PARTITIONING, NOBPROC, NORMAL);
    Define(pGs ,"WindSpeedSnow"  , 0.,"-","SnowPack >= 1|WaterEq > 0|SnowSurfTemperature>=1|StabilityCorrection>=1|CommonRefHeight = 0");
 //   !	call Gs(Group_14,EtaSnowIterations,"EtaSnowIterations"  , 0.,"-","SnowPack >= 1|WaterEq > 0|SnowSurfTemperature>=1|StabilityCorrection>=1")

 //   ! Heat balance snow melt approach
 	 pGs= new Gs(&TempSnow, SINGLE,	SNOW, EBAL_PARTITIONING, NOBPROC, NORMAL);
    Define(pGs,  "TempSnowPack"   , 0.,"°C","SnowPack >= 1|HeatEq > 0|SnowMeltFunction>=1");

 	 pGs= new Gs(&QMeltSurface, SINGLE,	SNOW, EBAL_PARTITIONING, NOBPROC, NORMAL);
     Define(pGs ,"QMeltSurface"   , 0.,UNIT_TYPES::HEATFLOW_UNIT,"SnowPack >= 1|HeatEq > 0|SnowMeltFunction>=1");
   	 pGs= new Gs(&QSnowSoil, SINGLE,	SNOW, EBAL_PARTITIONING, NOBPROC, NORMAL);
   Define(pGs ,   "QSnowSoil"         , 0.,UNIT_TYPES::HEATFLOW_UNIT,"SnowPack >= 1|HeatEq > 0|SnowMeltFunction>=1");

//    ! Snow adjustment variables
    pGs= new Gs(&PrecAdjustSnow, SINGLE,	SNOW, EBAL_PARTITIONING, NOBPROC, NORMAL);
	Define(pGs ,"PrecAdjustSnow"  , 0.,UNIT_TYPES::WATERFLOW_UNIT,"SnowPack >= 1|SnowAdjustment > 0");
	Ds *pDs;
	pDs= new Ds(&SnowMeasured, SINGLE,	SNOW, EBAL_PARTITIONING, NOBPROC, NORMAL);

	Define(pDs,"SnowMeasured"  , 0.,"m","SnowPack >= 1|SnowAdjustment > 0");

 //   ! Snow Reduce LAI parameter and variable
     pGs= new Gs(&SnowReduceLAIMax, SINGLE,	SNOW, EBAL_PARTITIONING, NOBPROC, NORMAL);

     Define(pGs ,"SnowReduceLAIMax"  , 1.,"-","SnowPack >= 1|WaterEq > 0|PlantType >= 2|Evaporation > 0");


	return FunctionDef();


}
bool SnowPack_Functions::FunctionDef() {


		return true;
}

double SnowPack_Functions::ThCond_R20(double Theta, double Thqualv) {

	/*		Real	Thetav, ThQualv
	Integer I
	I=1
	Thcond_R20=0.
	Do While (DepthMidpLayers(i).Lt.ResIntegDepth.And.I.lt.NL)
	Thcond_R20=Thcond_R20+DistanceMidpLayers(I)/THCOND(I,THETAv,THQUAlv)
	I=I+1
	EndDo	*/

	//if(I.EQ.1) Then	   ! First layer bigger than 40 cm 
	return p_Soil_WaterF->DistanceMidpLayers.front() / ((Soil_HeatF*)p_Soil_HeatF)->f_ThermalCond(0, Theta, Thqualv);
	//Endif

}
double SnowPack_Functions::ThCond_Upper(double Theta, double Thqualv) {
	return	((Soil_HeatF*)p_Soil_HeatF)->f_ThermalCond(0, Theta, Thqualv);
}


double  SnowPack_Functions::SteadyStateSol(double Upperheight, double UpperTh, double Lowerheight, double LowerTh, double  Tempabove, double Tempbelow) {

	double ALFA = (UpperTh*Lowerheight) / (Upperheight*LowerTh);

	ALFA = max(min(1., ALFA), 0.);
	return min(0.0001, (Tempbelow + ALFA*Tempabove) / (1. + ALFA));
}

double SnowPack_Functions::SnowSoilTemp_Function()
{
	double ThetaU = p_Soil_WaterF->WaterStorage.front() / p_Soil_WaterF->ThicknessLayers.front() / 10.;

	if (m_pModelStructure->TimeResolution == 1) {
		Rsoil = ThCond_R20(ThetaU, ((Soil_HeatF*)p_Soil_HeatF)->ThQual.front());
		KSOIL = p_Soil_WaterF->ThicknessLayers.front() / (2.*Rsoil);
	}
	else
		KSOIL = ThCond_Upper(ThetaU, ((Soil_HeatF*)p_Soil_HeatF)->ThQual.front());


	KSNOW = f_ThCond_Snow(DENSS);

	return SteadyStateSol(HSnow, KSNOW, p_Soil_WaterF->ThicknessLayers.front() / 2., KSOIL, TempSnowSurf, ((Soil_HeatF*)p_Soil_HeatF)->Temp.front());


}
double SnowPack_Functions::RetentionFunction(double SnowAmount) {
	return SRetens*SnowAmount;
}
DENS_SNOW SnowPack_Functions::SimpleDensityFunction(double LiquidRatio, double TotalMass) {
	DENS_SNOW out;
	out.Density=max(DENSS, SDensNew+(LiquidRatio)*SDensLi+TotalMass*SDensMass);
	out.Height=WTotSnow/out.Density ;
	out.CR=(out.Height/HSnow-1.)/(TISTEP*m_pModelInfo->SecPerDay);
	return out;

}



DENS_SNOW SnowPack_Functions::SNTermDensityFunction() {
	DENS_SNOW out;

			
	/*! old name			=new name			symbol			default hidden
	! -----------------	-------------------	---------------	-------	------	
	! MetaMorph_c3		= CRMetaMorphTemp1	(c1)			2.77e-6 no
	! MetaMorph_c4		= CRMetaMorphTemp2	(c2)			0.04	no
	! MetaMorphDens		= CRMetaMorphDens	(c3)			0.046	no
	! 100.0				= CRMetaMorphDensMin(gammalim,max)	100.0	yes
	! MetaMorphLiq_c5	= CRMetaMorphLiq	(c4)			2		no
	! OverburdenTemp	= CROverburdenTemp  (c5)			0.04	no
	! OverburdenDens_c2	= CROverburdenDens	(c6)			0.023	no
	! overburden_eta0	= CROverburdenVisc	(eta0)			9e5		no
	! CompMeltCutOff	= CRCompMeltCutOff	(no symbol)		300		no

	! Compaction due to metamorphosis*/
		CR1=-CRMetaMorphTemp1*exp(CRMetaMorphTemp2*TempSnow);
		DM=min(CRMetaMorphDensMin,1.15*DENSS_N);
		if(BULKDENS_ICE > DM) CR1=CR1*exp(-CRMetaMorphDens*(BULKDENS_ICE-DM));
		//! Liquid water term
		if(BULKDENS_LIQ > 0.01) CR1=CR1*CRMetaMorphLiq;
						
						
					
	//! Compaction due to overburden
		CR2=-WTotSnow*exp(CROverburdenTemp*TempSnow-CROverburdenDens*BULKDENS_ICE)/CROverburdenVisc;
					
	//! Compaction occurring during melt
		TOTALMELT=MELT+MELTSURF+MELTPREC;
		if(TOTALMELT>0. && BULKDENS_ICE*HSnow > 0.){
			if(BULKDENS_ICE < CRCompMeltCutOff)
				CR3=-TOTALMELT/(BULKDENS_ICE*HSnow);
			else
				CR3=-MELTSURF/(BULKDENS_ICE*HSnow);
		}
		else if(MELTSURF>0. && BULKDENS_ICE*HSnow > 0.)
			CR3=-MELTSURF/(BULKDENS_ICE*HSnow);
		else
			CR3=0.;
	
		//!! Why adjusting units for CR3 beneath Moved above
		CR3=CR3/(TISTEP*m_pModelInfo->SecPerDay);						
		//! Total Compaction
		out.CR=CR1+CR2+CR3;
		//! Checking compaction rate to make sure that negative snow Height are avoided
		FactorOfHeightChange=max(0.1, (1+CR*TISTEP*m_pModelInfo->SecPerDay)); // !  Introduced limits 0.1
		//! by PEJ to exclude negative values of HSnow Sept 2007
						
						
		//! New snow depth
		out.Height=HSnow*FactorOfHeightChange;					
		out.Density=WTotSnow/out.Height;

			
		if(out.Density>900.) { //! Introduced as upper bound by PEJ Sept 2007
			out.Density=900.;
			out.Height=WTotSnow/out.Density;									
		}
	return out;
}
double SnowPack_Functions::SnowHeatContentFunction(double Temp, double ice, double water) {
		return Temp*(Fix::SPECI*ice+Fix::SPECW*water);  
}
double SnowPack_Functions::SnowTempFunction(double HeatContentSnow, double ice, double water) {
	if (ice>0. && water<=0.)
		return HeatContentSnow/(Fix::SPECI*ice);
	else
		return 0.;	
}
double SnowPack_Functions::ThermQualPrecFunction(double _TAir) {
	double FliqMaxP = 0.5;
	if (_TAir<PRainLim)
		return min(1., (1 - FliqMaxP) + FliqMaxP*(_TAir - PRainLim) / (PSnowLim - PRainLim));
	else
		return 0.;

}
double SnowPack_Functions::UpdateSnowDensity() {

	if (HSnow <= 0)
		return f_NewSnowDensity(1., PSnowLim);
	else {
		if (SnowAdjustment == 2)
			epsilon = AgeUpdateSnowDepthCorr;
		else
			epsilon = AgeUpdateSnowDepthCorr*TISTEP;

		if (SnowMeasured >= HSnow + epsilon&&SnowMeasured > OldSnowDepth) {
			ThQualP = ThermQualPrecFunction(TAir);
			SnowAge = 0.;
			return  f_NewSnowDensity(ThQualP, TAir);
			
		}
		else
			return DENSS;
	}
}
double SnowPack_Functions::AdjustedSnowPrecAndSnow() {
	double PrecAdjustSnow;
	if (TAir > 5.) return 0.;
	PrecAdjustSnow = (SnowMeasured - HSnow)*SnowDensity / TISTEP;
	HSnow = SnowMeasured;
	Temp_WtotSnow = WTotSnow;
	WTotSnow = max(WTotSnow + PrecAdjustSnow*TISTEP, 0.0);
	if (Temp_WtotSnow > 0.) {
		WICE = WICE*WTotSnow / Temp_WtotSnow;
		SWATS = SWATS*WTotSnow / Temp_WtotSnow;
	}
	else {
		WICE = WTotSnow;
		SWATS = 0.;
	}
	if (HSnow > 0.)
		DENSS = WTotSnow / HSnow;
	else
		DENSS = 0;

	return (WTotSnow - Temp_WtotSnow) / TISTEP;
}
