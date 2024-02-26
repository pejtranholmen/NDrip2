#include "WaterUptake.h"
#include "../SOIL/Soil_HeatF.h"
#include "../../Simulator/Simulator.h"
#include "./Functors/WaterUptake_Functors.h"

void WaterUptake::setWaterUptake(ModelInfo* pInfo, ModelMap* pMap, Structure* pStruc)
{
	ModuleName = "Water uptake";
	ModuleNo = 20;
	ModuleConditions = "PlantType>0";
	m_pModelInfo = pInfo; m_pModelMap = pMap; m_pModelStructure = pStruc;
	pT_Step = &m_pModelInfo->T_Step;
	T_Step = *pT_Step;
}
WaterUptake::WaterUptake(void)
{
	ModuleName="Water uptake";
	ModuleNo=20;
	ModuleConditions="PlantType>0";
}
bool WaterUptake::Def()
{
	Salt::Def();

	ModuleNo = 20;

	ModuleConditions = "PlantType>0";
	Func *pFunc;
	string funcname;
	FunctorProp functorprop;
	vector<Ps*> ps_vec;
	vector<P*> p_vec;
	PLOT_PARAM plotpar;
	plotpar.Num_Of_Curves = 1;
	plotpar.Num_Of_Points = 100;
	plotpar.Num_Of_X = 1;
	plotpar.X_Min = 0;
	plotpar.X_Max = 4.2;
	plotpar.X_Label = "X";
	plotpar.Y_Label = "Y";
	plotpar.LogXScale = true;
	plotpar.LogYScale = false;
	plotpar.Num_Of_Opt = 1;


	Sw* pSw;
	pSw = new Sw(&WUptake, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pSw, "DemandRedistribution", 1, "Without flexible roots|With flexible roots", "WaterEq = 1|PlantType > 0|PlantWaterStorage=0");//
	pSw = new Sw(&Salt_Influence, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pSw, "Salt influence", 0, "Not considered|Added to pressure head|Add multiplicative response |Add minimum response", "WaterEq>0|PlantType >0|SaltTracer>0", 104);//
	pSw = new Sw(&WuptBasic_equation, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pSw, "Basic equation", 0, "Pressure head response|Darcy-based", "WaterEq = 1|PlantType > 1|TimeResolution!=1", 104);//
	pSw = new Sw(&PlantWaterStorage, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pSw, "PlantWaterStorage", 0, "Not considered|f(LAI )|f(LAI, height)", "WaterEq = 1|PlantType > 1|TimeResolution!=1|Basic equation >0");//
	pSw = new Sw(&PlantResistance, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pSw, "PlantResistance", 0, "No salt effect|Salt effect by osmotic pressure", "WaterEq>0|PlantType >1|SaltTracer>0|Basic equation >0", 104);//

	pSw = new Sw(&SaltCoefValid, SINGLE, GENERAL, NOFPROC, NOBPROC, NORMAL);
	Define(pSw, "SaltCoefValid", 0, "No salt effect|Salt effect", "WaterEq = 1|PlantType > 0");
	pSw = new Sw(&WupTempResponse, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pSw, "Temperature response", 1, "None|Double-exponential|Single-exponential|Polynomial", "WaterEq = 1|PlantType > 1|TimeResolution!=1"); //!,forced_recalc=104)
	pSw = new Sw(&MultiPlantWaterUptakeFunction, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pSw, "WaterUptakeFunction", 0, "Common for all plants|Specific for each plant", "WaterEq = 1|PlantType > 1|Basic equation = 0"); // introduced by PEJ, Oct 2019

	Ps *pPs;


	pPs = new Ps(&AirMinContent, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "AirMinContent", 5., UNIT_TYPES::VOLPERCENTAGE_UNIT, "PlantType > 0|WaterEq = 1|WaterUptakeFunction=0", 0., 20.);
	pPs = new Ps(&AirRedCoef, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "AirRedCoef", 4.,UNIT_TYPES::NO_UNIT, "PlantType > 0|WaterEq = 1|WaterUptakeFunction=0", 0., 20.);
	pPs = new Ps(&WupCri, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "CritThresholdDry", 400., UNIT_TYPES::PRESSUREHEAD_UNIT, "PlantType > 0|WaterEq = 1|Basic equation =0|WaterUptakeFunction=0", 0., 10000.);
	pPs = new Ps(&WupF, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "DemandRelCoef", 0.3, UNIT_TYPES::INVERSE_WATERFLOW_UNIT, "PlantType > 0|WaterEq = 1|Basic equation =0|WaterUptakeFunction=0", 0., 2.);
	//!010213	Define(pPs, WupFThres,"DemandRelThreshold"  ,0.3,"1/day", "PlantType > 0|WaterEq = 1|Demand influence=2|Basic equation =0",0.,2.,view=1901)	
	pPs = new Ps(&WupFB, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "NonDemandRelCoef", 0.1,UNIT_TYPES::NO_UNIT, "PlantType > 0|WaterEq = 1|Basic equation =0|WaterUptakeFunction=0", 0., 20.);
	
	pPs = new Ps(&DripScaleWUpt, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pPs, "DripScaleWupt", 2., "-", "Irrigation >0|Dripper>0", 0., 10000.);


	functorprop.FunctorInitOption = string::npos; functorprop.pHyd = p_Soil_WaterF;
	functorprop.FuncName = MyFunc::WATERUPTAKEMOISTURE; functorprop.Arg = MyArgType::T_D3;
	funcname = "Water Uptake Function";
	pFunc = new Func(SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);

	plotpar.Num_Of_Curves = 2;
	plotpar.X_Label = "Pressure Head (cm water)";
	plotpar.Y_Label = "Degree of Uptake (-)";
	plotpar.X2_Vec = { 2.f,6.f};
	plotpar.X3_Vec = { 0.f,0.f};
	plotpar.LegendsVec = { "demand 2 mm/day","demand 6 mm/day"};

	Define(pFunc, functorprop, funcname, ps_vec, p_vec,plotpar);


	pFunc = new Func(SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	
	functorprop.FuncName = MyFunc::WATERUPTAKEMOISTURE_DRIP; functorprop.Arg = MyArgType::T_D3;
	funcname = "Water Uptake drip irrigation";
	ps_vec.push_back(m_pModelMap->GetPsPointer("DripScaleWupt"));
	Define(pFunc, functorprop, funcname, ps_vec, p_vec, plotpar);

	
	ps_vec.clear();

	
	pPs = new Ps(&WupATe, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "TempCoefA", 0.8, "-", "PlantType > 0|WaterEq = 1|Temperature response=1|WaterUptakeFunction = 0", 0., 20.);
	pPs = new Ps(&WupBTe, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "TempCoefB", 0.4, "-", "PlantType > 0|WaterEq = 1|Temperature response=1|WaterUptakeFunction = 0", 0., 20.);
	pPs = new Ps(&WupDayOff, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "TempWupDayNoCut", 365., "#", "PlantType > 0|WaterEq = 1|Temperature response>=1", 0., 366.);

	functorprop.FunctorInitOption = string::npos;
	functorprop.FuncName = MyFunc::WATERUPTAKETEMP_ORIG; functorprop.Arg = MyArgType::D2;
	funcname = "Water Uptake Original TempFunction";
	plotpar.Num_Of_Curves = 2;
	plotpar.X_Max = 20;
	plotpar.LogXScale = false;
	plotpar.X2_Vec = {1.f,200.f};
	plotpar.X_Label = "Soil Temperature "; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::TEMP_UNIT, true);
	plotpar.LegendsVec = { "DayNo 1", "DayNo 200"};

	pFunc = new Func(SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar); ps_vec.clear();

	pPs = new Ps(&WupCTe, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "TempCoefC", 0.,UNIT_TYPES::TEMP_UNIT, "PlantType > 0|WaterEq = 1|Temperature response>=2|WaterUptakeFunction = 0", 0., 9.);
	pPs = new Ps(&WupDTe, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "TempCoefD", 15.,UNIT_TYPES::TEMP_UNIT, "PlantType > 0|WaterEq = 1|Temperature response>=2|WaterUptakeFunction = 0", 10., 50.);
	pPs = new Ps(&WupETe, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pPs, "TempCoefE", 1., "-", "PlantType > 0|WaterEq = 1|Temperature response=3|WaterUptakeFunction = 0", 0., 20.);


	ps_vec.push_back(m_pModelMap->GetPsPointer("TempWupDayNoCut"));

	functorprop.FunctorInitOption = string::npos;
	functorprop.FuncName = MyFunc::WATERUPTAKETEMP_TRIGTEMP; functorprop.Arg = MyArgType::D2;
	funcname = "Water Uptake Trig TempFunction";



	pFunc = new Func(SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar); ps_vec.clear();


	ps_vec.push_back(m_pModelMap->GetPsPointer("TempCoefC"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("TempCoefD"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("TempCoefE"));
	ps_vec.push_back(m_pModelMap->GetPsPointer("TempWupDayNoCut"));

	functorprop.FunctorInitOption = string::npos; functorprop.pHyd = nullptr;
	functorprop.FuncName = MyFunc::WATERUPTAKETEMP_TRIGTEMP_POLY; functorprop.Arg = MyArgType::D2;
	funcname = "Water Uptake Trig polynomial TempFunction";
	pFunc = new Func(SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar); ps_vec.clear();
 
 	pPs = new Ps(&UpMov, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pPs, "FlexibilityDegree", 0.6, "-", "PlantType > 0|DemandRedistribution > 0|WaterEq = 1|PlantWaterStorage=0", 0., 1.);
	
	pPs = new Ps(&RootDistMin, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "RootDistMin", .001, "m", "Basic equation =1", 1.E-9, .05);
	
	pPs = new Ps(&RootDistMax, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "RootDistMax", .01, "m", "Basic equation =1", 1.E-6, 0.1);

	pPs = new Ps(&RootDensityCoef, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);ps_vec.push_back(pPs);
	Define(pPs, "RootDensityCoef", .5, UNIT_TYPES::AREA_UNIT, "Basic equation =1", 0.00001, 5.);

	functorprop.FunctorInitOption = string::npos;
	functorprop.FuncName = MyFunc::ROOT_DISTANCE_FROM_DENSITY; functorprop.Arg = MyArgType::D1;
	funcname = "Root Distance to bulk soil";
	pFunc = new Func(SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar); ps_vec.clear();



	pPs = new Ps(&RootResistivity, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "ResistivityRoot", 1000., "mdays", "Basic equation =1", 1., 1.E9);
	pPs = new Ps(&XylemResistivity, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "ResistivityXylem", 1., "days/m", "Basic equation =1", 0.1E-3, 100.);

	functorprop.FunctorInitOption = string::npos;
	functorprop.FuncName = MyFunc::INTERNAL_PLANTRESISTANCE; functorprop.Arg = MyArgType::D2;
	funcname = "PlantWater Resistance by Height and Roots";
	pFunc = new Func(SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar); ps_vec.clear();



	pPs = new Ps(&PlantMaxSuction, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "PlantMaxSuction", 15000., "cm water", "Basic equation>0", 1000., 30000.);
	pPs = new Ps(&LeafThresholdSuction, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "LeafThresholdSuction", 1000., "cm water", "PlantWaterStorage>0", 100., 30000.);

	functorprop.FunctorInitOption = string::npos;
	functorprop.FuncName = MyFunc::STOMATA_RELATIVEOPENING_FROM_PLANTWATERPOTENTIAL; functorprop.Arg = MyArgType::D1;
	funcname = "Stomata Opening from PlantWater Potential";
	pFunc = new Func(SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar); ps_vec.clear();




	pPs = new Ps(&PlantWatRelLAI, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "PlantWatRelLAI", 1., "mm", "PlantWaterStorage=1", 0.01, 10.);


	functorprop.FunctorInitOption = string::npos;
	functorprop.FuncName = MyFunc::MAXPLANTWATER_FROM_LAI; functorprop.Arg = MyArgType::D1;
	funcname = "Max Plant Water Storage by LAI";
	pFunc = new Func(SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar); 

	pPs = new Ps(&PlantWatRelLAI_Height, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "PlantWatRelLAI_Height", 0.5, "mm/m", "PlantWaterStorage=2", 0.001, 5.);

	functorprop.FunctorInitOption = string::npos;
	functorprop.FuncName = MyFunc::MAXPLANTWATER_FROM_LAIHEIGHT; functorprop.Arg = MyArgType::D2;
	funcname = "Max Plant Water Storage by LAI and Heigth";
	pFunc = new Func(SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar); ps_vec.clear();


	
	
	
	
	
	pPs = new Ps(&Upt_Excess, SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pPs, "Upt_Excess", 2.0, "mm/day", "PlantWaterStorage>0", 0.00, 100.);

	NE* pNumPlants;
	pNumPlants = m_pModelMap->GetNEPointer("Number of Plants");
	vector<double> initv;
	functorprop.FunctorInitOption = string::npos;
	functorprop.FuncName = MyFunc::WATERUPTAKE_SALT; functorprop.Arg = MyArgType::T_D1;
	funcname = "Water Uptake SaltFunction";

	plotpar.Num_Of_Curves = 1;
	plotpar.X_Label = "Osmotic Pressure Head "; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::PRESSUREHEAD_UNIT, true);
	plotpar.Y_Label = "Degree of Uptake (-)";
	plotpar.X_Min = 1;
	plotpar.X_Max = 5.;
	plotpar.LogXScale = true;


	initv.assign(NumPlants, 3.);
	P* pP = new P(&SaltPowerCoef, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL); p_vec.push_back(pP);
	Define(pP, pNumPlants, "Salt p", initv, "-", "PlantType >= 3|SaltCoefValid > 0", .5, 10.);
	mTab_Response.push_back(pP);
	pP = new P(&SaltHalfReduction, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL); p_vec.push_back(pP);
	initv.assign(NumPlants, 5000.);
	Define(pP, pNumPlants, "Salt c50", initv, "cm", "PlantType >= 3|SaltCoefValid > 0", 5., 15000.);
	mTab_Response.push_back(pP);

	pFunc = new Func(SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar); p_vec.clear();


	Tab* pTab;
	pTab = new Tab(&mTab_Response, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pTab, "Salinity Response", "Salt p|Salt c50", "PlantType > 0|SaltCoefValid > 0|WaterEq = 1");



	functorprop.FuncName = MyFunc::WATERUPTAKEMOISTURE; functorprop.Arg = MyArgType::TT_D3;
	functorprop.pHyd = p_Soil_WaterF;
	funcname = "Water UptakeFunction";

	plotpar.Num_Of_Curves = 1;
	plotpar.X_Label = "Pressure Head "; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::PRESSUREHEAD_UNIT, true);
	plotpar.Y_Label = "Degree of Uptake (-)";
	plotpar.X_Min = 0;
	plotpar.X_Max = 5.;
	plotpar.LogXScale = true;


	pP = new P(&v_AirMinContent, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL); p_vec.push_back(pP); initv.assign(NumPlants, 5.);	mTab_WaterResponse.push_back(pP);
	Define(pP, pNumPlants, "v_AirMinContent", initv, UNIT_TYPES::VOLPERCENTAGE_UNIT, "PlantType > 0|WaterEq=1|WaterUptakeFunction > 0", 0., 20.);
	pP = new P(&v_AirRedCoef, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL); p_vec.push_back(pP); initv.assign(NumPlants, 4.);	mTab_WaterResponse.push_back(pP);
	Define(pP, pNumPlants, "v_AirRedCoef", initv,UNIT_TYPES::NO_UNIT, "PlantType > 0|WaterEq=1|WaterUptakeFunction > 0", 0., 20.);
	pP = new P(&v_WupCri, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL); p_vec.push_back(pP); initv.assign(NumPlants, 400.);	mTab_WaterResponse.push_back(pP);
	Define(pP, pNumPlants, "v_CritThresholdDry", initv, UNIT_TYPES::PRESSUREHEAD_UNIT, "PlantType > 0|WaterEq=1|WaterUptakeFunction > 0", 0., 10000.);
	pP = new P(&v_WupF, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL); p_vec.push_back(pP); initv.assign(NumPlants, 0.3);	mTab_WaterResponse.push_back(pP);
	Define(pP, pNumPlants, "v_DemandRelCoef", initv, UNIT_TYPES::INVERSE_WATERFLOW_UNIT, "PlantType > 0|WaterEq=1|WaterUptakeFunction > 0", 0., 2.);
	pP = new P(&v_WupFB, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL); p_vec.push_back(pP); initv.assign(NumPlants, .1);	mTab_WaterResponse.push_back(pP);
	Define(pP, pNumPlants, "v_NonDemandRelCoef", initv,UNIT_TYPES::NO_UNIT, "PlantType > 0|WaterEq=1|WaterUptakeFunction > 0", 0., 20.);	 

	pFunc = new Func(SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar); 

	 p_vec.clear();


	functorprop.FunctorInitOption = string::npos;
	functorprop.FuncName = MyFunc::WATERUPTAKETEMP_ORIG; functorprop.Arg = MyArgType::T_D2;
	funcname = "Water Uptake Original TempFunction";
	plotpar.Num_Of_Curves = 2;
	plotpar.X_Max = 20;
	plotpar.LogXScale = false;
	plotpar.X2_Vec = { 1.f,200.f };
	plotpar.X_Label = "Soil Temperature "; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::TEMP_UNIT, true);
	plotpar.LegendsVec = { "DayNo 1", "DayNo 200" };


	pP = new P(&v_WupATe, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL); p_vec.push_back(pP);	mTab_WaterResponse.push_back(pP);  initv.assign(NumPlants, .8);
		Define(pP, pNumPlants, "TempCoefA", initv,  "-", "PlantType > 0|WaterEq = 1|Temperature response=1|WaterUptakeFunction > 0", 0., 20.);
	pP = new P(&v_WupBTe, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL); p_vec.push_back(pP);	mTab_WaterResponse.push_back(pP);	  initv.assign(NumPlants, .4);
		Define(pP, pNumPlants, "TempCoefB", initv, "-", "PlantType > 0|WaterEq = 1|Temperature response=1|WaterUptakeFunction > 0", 0., 20.);
	pP = new P(&v_WupDayOff, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL); p_vec.push_back(pP); mTab_WaterResponse.push_back(pP);  initv.assign(NumPlants, 365);
		Define(pP, pNumPlants, "TempWupDayNoCut", initv, "#", "PlantType > 0|WaterEq = 1|Temperature response>=1|WaterUptakeFunction > 0", 0., 366.);

	pFunc = new Func(SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar); p_vec.clear();

	

	functorprop.FuncName = MyFunc::WATERUPTAKETEMP_TRIGTEMP; functorprop.Arg = MyArgType::T_D2;
	funcname = "Water Uptake Trig TempFunction";
	pP = new P(&v_WupCTe, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL); p_vec.push_back(pP);	 mTab_WaterResponse.push_back(pP);	 initv.assign(NumPlants, 0.);
		Define(pP, pNumPlants, "TempCoefC", initv, "-", "PlantType > 0|WaterEq = 1|Temperature response>=2|WaterUptakeFunction > 0", 0., 9.);
	pP = new P(&v_WupDTe, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL); p_vec.push_back(pP);	 mTab_WaterResponse.push_back(pP);	initv.assign(NumPlants, 15.);
		Define(pP, pNumPlants, "TempCoefD", initv, "-", "PlantType > 0|WaterEq = 1|Temperature response>=2|WaterUptakeFunction > 0", 10., 50.);
	p_vec.push_back(m_pModelMap->GetPPointer("TempWupDayNoCut"));
	
	pFunc = new Func(SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar); p_vec.clear();

	functorprop.FuncName = MyFunc::WATERUPTAKETEMP_TRIGTEMP_POLY; functorprop.Arg = MyArgType::T_D2;
	funcname = "Water Uptake Polynomial Trig TempFunction";

		p_vec.push_back(m_pModelMap->GetPPointer("TempCoefC"));
		p_vec.push_back(m_pModelMap->GetPPointer("TempCoefD"));
		
	pP = new P(&v_WupETe, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL);	p_vec.push_back(pP); mTab_WaterResponse.push_back(pP);	initv.assign(NumPlants, 1.);
		Define(pP, pNumPlants, "TempCoefE", initv, "-", "PlantType > 0|WaterEq = 1|Temperature response=3|WaterUptakeFunction > 0", 0., 20.);
	p_vec.push_back(m_pModelMap->GetPPointer("TempWupDayNoCut"));

	pFunc = new Func(SINGLE, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar); p_vec.clear();

	pTab = new Tab(&mTab_WaterResponse, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pTab, "Water Uptake Function", 1);



	X* pX;
	initv.assign(NumPlants, 0.);
	pX = new X(&PlantWater, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL);

	Define(pX, pNumPlants, "PlantWater", initv, "mm", "PlantWaterStorage>0");

	T* pT;
	NE *pNL = m_pModelMap->GetNEPointer("Number of Layers");
	pT = new T(&WUptakeRate, pNL, SIMPROFILE, WATER, WATERUPTAKE, NOBPROC, NORMAL);

	initv.assign(p_Soil_WaterF->NL, 0);
	Define(pT, pNL, "WUptakeRate", initv,UNIT_TYPES::WATERFLOW_UNIT, "PlantType > 0|WaterEq = 1");

	pT = new T(&Transpiration, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 0.);
	Define(pT, pNumPlants, "Transpiration", initv,UNIT_TYPES::WATERFLOW_UNIT, "PlantType > 0|WaterEq = 1");
	pT = new T(&PlantWaterUptake, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pT, pNumPlants, "PlantWaterUptake", initv,UNIT_TYPES::WATERFLOW_UNIT, "PlantType > 0|WaterEq = 1|PlantWaterStorage>0");

	Gs *pGs = new Gs(&PotWaterUptake, ALLPLANTS, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pGs, "PotWaterUptake", 0.,UNIT_TYPES::WATERFLOW_UNIT, "PrecInterception > 0|PlantType > 0|WaterEq = 1");

	pGs = new Gs(&Transpiration_All, ALLPLANTS, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pGs, "Transpiration all pl", 0.,UNIT_TYPES::WATERFLOW_UNIT, "PlantType = 3|WaterEq = 1");
	pGs = new Gs(&WupTrigTemp, ALLPLANTS, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pGs, "WaterUptake TrigTemp", 0., "°C", "Temperature response>0|WaterEq = 1");
	pGs = new Gs(&RedWupTotal_All, ALLPLANTS, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pGs, "RedCTotal all plant", 0., "mm", "PlantType > 3|WaterEq = 1|PlantWaterStorage <1");

	initv.assign(NumPlants, 0.);
	G *pG = new G(&RedWupPsi, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "RedCMoisture", initv, "-", "PlantType > 0|WaterEq = 1|PlantWaterStorage <1");
	pG = new G(&RedWupTemp, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "RedCTemperature", initv, "-", "PlantType > 0|WaterEq = 1|PlantWaterStorage <1");
	pG = new G(&RedWupTotal, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 1.);
	Define(pG, pNumPlants, "RedCTotal", initv, "-", "PlantType > 0|WaterEq = 1");
	pG = new G(&PlantWaterPotential, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 0.);
	Define(pG, pNumPlants, "PlantWaterPotential", initv, "cm water", "PlantWaterStorage>0");
	pG = new G(&MC_PotWaterUptake, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "Plant PotWaterUptake", initv,UNIT_TYPES::WATERFLOW_UNIT, "PrecInterception > 0|PlantType = 3|WaterEq = 1");

	pG = new G(&Resist_Plant, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "Resist Plant", initv, "days", "Basic equation >0");

	pG = new G(&Resist_SoilPlant, pNumPlants, PLANT, WATER, WATERUPTAKE, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "Resist Soil_Plant", initv, "days", "Basic equation >0");


	//Define(pFunc, fpoint, this, "WaterUptake","");
	//Define(pFunc, fpoint2, ((SimModule*)this), "WaterUptake","");






	return true;
}

bool WaterUptake::Ini()	{
	//m_pModelStructure = ((Simulator*)m_pModelInfo)->GetSimStructurePointer();
	Salt::Ini();
	if (m_pModelStructure->PlantType == 0) return true;
	WTemp.resize(p_Soil_WaterF->NL);
	RedWupPsiDepth.resize(p_Soil_WaterF->NL);	
	SingleUptake.resize(p_Soil_WaterF->NL);
	ThetaWetCrit.resize(p_Soil_WaterF->NL);
	RedGrowthSalt.resize(NumPlants);

	if(TraceElementUptake) 
		TE_SingleUptake.resize(p_Soil_WaterF->NL);

	for(size_t i=0; i<ThetaWetCrit.size();i++) {
		WUptakeRate[i]=0.;
		ThetaWetCrit[i]=p_Soil_WaterF->M_Saturation[i]-AirMinContent;
	}

	WupCri_Initial=WupCri;
	

	if (m_pModelStructure->PlantType == 3) {
		DefineMultiplePlantShapes();
		if(PlantWaterStorage_Sw(PlantWaterStorage)==PlantWaterStorage_Sw::f_LAI)
			f_MaxStorage_LAI = Get_D1_Functor(MyFunc::MAXPLANTWATER_FROM_LAI);
		else if (PlantWaterStorage_Sw(PlantWaterStorage) == PlantWaterStorage_Sw::f_LAI_height)
			f_MaxStorage_LAI_Height = Get_D2_Functor(MyFunc::MAXPLANTWATER_FROM_LAIHEIGHT);

		if (PlantWaterStorage > 0 && !m_pModelInfo->StartStateValue) {
			for (size_t i = 0; i < PlantWater.size(); i++)
				PlantWater[i] = MaxPlantWaterStorage(i)*0.9;

		}
		RedWupTotal.assign(NumPlants, 1.);
	}
	


	return true;
}
bool WaterUptake::IniFunctors() {
	Irrigation* pMet = (Irrigation*)p_Atm;

	Salt::IniFunctors();

	if (RootDistribution_Sw(RootDistribution) == RootDistribution_Sw::Table)
		f_WaterUptakeDistribution = FixedDepthDistribution(RootF, p_Soil_WaterF->DepthLayers, p_Soil_WaterF->ThicknessLayers);
	else if (RootDistribution_Sw(RootDistribution) == RootDistribution_Sw::Linear)
		f_WaterUptakeDistribution = LinearDepthDistribution(p_Soil_WaterF->DepthLayers, p_Soil_WaterF->ThicknessLayers);
	if (RootDistribution_Sw(RootDistribution) == RootDistribution_Sw::Constant)
		f_WaterUptakeDistribution = UniformDepthDistribution(p_Soil_WaterF->DepthLayers, p_Soil_WaterF->ThicknessLayers);
	if (RootDistribution_Sw(RootDistribution) == RootDistribution_Sw::Exponential)
		f_WaterUptakeDistribution = ExponentialDepthDistribution(RFracLow, p_Soil_WaterF->DepthLayers, p_Soil_WaterF->ThicknessLayers);

	if (WupTempResponse_Sw(WupTempResponse) == WupTempResponse_Sw::None)
		f_WaterUptakeTemp_No = WaterUptakeTempFunction();
	if (MultiPlantWaterUptakeFunction_Sw(MultiPlantWaterUptakeFunction) == MultiPlantWaterUptakeFunction_Sw::Common) {
		if (WupTempResponse_Sw(WupTempResponse) == WupTempResponse_Sw::Double_exponential)
			f_WaterUptakeTemp = Get_D2_Functor(MyFunc::WATERUPTAKETEMP_ORIG);
		else if (WupTempResponse_Sw(WupTempResponse) == WupTempResponse_Sw::Single_exponential)
			f_WaterUptakeTemp = Get_D2_Functor(MyFunc::WATERUPTAKETEMP_TRIGTEMP);
		else if (WupTempResponse_Sw(WupTempResponse) == WupTempResponse_Sw::Polynomial)
			f_WaterUptakeTemp = Get_D2_Functor(MyFunc::WATERUPTAKETEMP_TRIGTEMP_POLY);
	}
	else {
		if (WupTempResponse_Sw(WupTempResponse) == WupTempResponse_Sw::Double_exponential)
			f_MultiWaterUptakeTemp = WaterUptakeTempFunction(v_WupATe, v_WupBTe, v_WupDayOff);
		else if (WupTempResponse_Sw(WupTempResponse) == WupTempResponse_Sw::Single_exponential)
			f_MultiWaterUptakeTemp = WaterUptakeTempFunction(true, v_WupCTe, v_WupDTe, v_WupDayOff);
		else if (WupTempResponse_Sw(WupTempResponse) == WupTempResponse_Sw::Polynomial)
			f_MultiWaterUptakeTemp = WaterUptakeTempFunction(true, v_WupCTe, v_WupDTe, v_WupETe, v_WupDayOff);
	}


	for (size_t i = 0; i<pNC_Plant->Drip.size(); i++)	   {
		if (pNC_Plant->Drip[i] > 0) {
			pMet->DripInUse = true;
		}
	}


	if (MultiPlantWaterUptakeFunction_Sw(MultiPlantWaterUptakeFunction) == MultiPlantWaterUptakeFunction_Sw::Common) {
		if (pMet->DripInUse)	  f_WaterUptakeMoisture = Get_D3_TableFunctor(MyFunc::WATERUPTAKEMOISTURE_DRIP);
		else f_WaterUptakeMoisture = Get_D3_TableFunctor(MyFunc::WATERUPTAKEMOISTURE);
	}
	else {
		SoilModel* pSoil = (SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
		Hydraulic* pHyd = &pSoil->m_SoilFrost;
		f_MultiWaterUptakeMoisture = WaterUptakeMoistureFunction(v_AirMinContent, v_AirRedCoef, v_WupCri, v_WupF, v_WupFB, pHyd);
	}
	if (Salt_Influence_Sw(Salt_Influence) > Salt_Influence_Sw::Added_to_pressure_head) {
		f_WaterUptakeSalt = Get_D1_TableFunctor(MyFunc::WATERUPTAKE_SALT);
	}
	else if(Salt_Influence_Sw(Salt_Influence)!=Salt_Influence_Sw::Not_considered)
		f_WaterUptakeSalt = Get_D1_TableFunctor(MyFunc::WATERUPTAKE_SALT);

	f_WaterFlowDistanceInSoil = Get_D1_Functor(MyFunc::ROOT_DISTANCE_FROM_DENSITY);

	//	if(PlantWaterStorage_Sw(PlantWaterStorage)==PlantWaterStorage_Sw::f_LAI)
	f_MaxStorage_LAI = Get_D1_Functor(MyFunc::MAXPLANTWATER_FROM_LAI);
	//	else if (PlantWaterStorage_Sw(PlantWaterStorage) == PlantWaterStorage_Sw::f_LAI_height)
	f_MaxStorage_LAI_Height = Get_D2_Functor(MyFunc::MAXPLANTWATER_FROM_LAIHEIGHT);
	f_PlantResistance = Get_D2_Functor(MyFunc::INTERNAL_PLANTRESISTANCE);
	f_RelativeStomata_PlantPotential = Get_D1_Functor(MyFunc::STOMATA_RELATIVEOPENING_FROM_PLANTWATERPOTENTIAL);
	return true;
}
bool WaterUptake::Dyn()
{
	//Salt::Dyn();
	Flux();
	Integ();
	return true;
}

void WaterUptake::Flux()
{
	Soil_HeatF* pSoilHeat=(Soil_HeatF*)p_Soil_HeatF;
	double Local_OPressure, WupPot2;
	WUptakeRate.assign(p_Soil_WaterF->NL,0.);
	RedWupTotal_All=0.;
	NNUP_MAX=0;
    Transpiration.assign(NumPlants,0.);


 //  ! Trace Element Section

   if(TraceElementUptake_Sw(TraceElementUptake)!= TraceElementUptake_Sw::off) {
		SaltPlantUptake.assign(NumPlants,0.);
		TE_PlantLeafUptake.assign(NumPlants,0.);
		TE_PlantRootUptake.assign(NumPlants,0.);
		TE_PlantStemUptake.assign(NumPlants,0.);
  }

   for(size_t IP=0; IP<size_t(NumPlants);IP++) { //loop over plants
	if(RootDepth[IP]>=0.) break; //Cycle Loop_over_plants
    Transpiration[IP]=0.;
	SingleUptake.assign(p_Soil_WaterF->NL,0.);
    double ETemp=0.;
	SaltAccLayers=0.;
    double WupLoss=0.;
    double SumR_Avail=0.;
	DEPTH_type out;
	out.NumLay=0;
	if(PlantType_Sw(m_pModelStructure->PlantType)>=PlantType_Sw::Explicit_Single_Leaf) { // ! Estimate root distribution for multiple plants
		out=p_Soil_WaterF->DepthDist(RootDistribution,  1., RootDepth[IP], RFracLow);
		out = f_WaterUptakeDistribution(1., RootDepth[IP]);
		RRoot=out.Dist;
		if (IP + 1 == RootDistribution_Outputs) RelRootDist = RRoot;
	}
	NNUP_MAX=max(RRoot.size(), out.NumLay);//  ! Introduced NNUP_MAX the highest number of layers 030506

//Loop_over_layers:      DO i=1,NNUP
   for(size_t i=0; i<RRoot.size();i++) {	
		   if(PlantWaterStorage_Sw(PlantWaterStorage)> PlantWaterStorage_Sw::Not_considered) {
				if(Salt_Influence==1) 
					Local_OPressure=OsmoticPressure[i];
				else
					Local_OPressure=0.;
				PlantWaterPotential[IP] = PlantWaterPotentialFuncL(IP);
				double flux=10.*RRoot[i]*(abs(PlantWaterPotential[IP])-10.*Fix::GRAVCONST*(PlantHeight[IP]+p_Soil_WaterF->DepthLayers[i])-p_Soil_WaterF->Psi[i]-Local_OPressure)/   
				(PlantResistanceFuncL(i,IP)+RhizoSphereResistanceFuncL(i, IP)) ;	
				flux=min(flux,(MaxPlantWaterStorage(IP)-PlantWater[IP])*RRoot[i]/T_Step);
				flux=min(flux, RRoot[i]*(Upt_Excess+ MC_PotWaterUptake[IP]));
				SingleUptake[i]=max(0.,flux);

				if(Salt_Influence==2) 
 					SingleUptake[i]=SingleUptake[i]*WaterUptakeSaltFuncL(i, IP);
				else if (Salt_Influence==3) 
					SingleUptake[i]=min(SingleUptake[i],WaterUptakeSaltFuncL(i, IP))*RRoot[i]*MC_PotWaterUptake[IP];
		   }
		   else {

		//! REDUCTION DUE TO LOW SOIL TEMPERATURE 
 
			   if (HeatEq_Sw(m_pModelStructure->HeatEq) == HeatEq_Sw::on && WUptBasic_equation_Sw(WuptBasic_equation) == WUptBasic_equation_Sw::Pressure_head_response) { //! Added condition on Darcybased approach 1 febr 2001
				   if (MultiPlantWaterUptakeFunction == 0) RedWupTempLocal = f_WaterUptakeTemp(pSoilHeat->Temp[i], m_pModelInfo->JDayNum);
				   else    RedWupTempLocal = f_MultiWaterUptakeTemp(IP, pSoilHeat->Temp[i], m_pModelInfo->JDayNum);

			   }
			  else 
				 RedWupTempLocal=1;
 
		//! REDUCTION DUE TOO LOW OR TOO HIGH WATER CONTENTS 
			  RedWupPsiDepth[i]=WaterUptakeMoistureFuncL(i, IP, MC_PotWaterUptake[IP]);

			  if(RedWupPsiDepth[i]>=0.9999) 
				  SumR_Avail+=RRoot[i];
			  double RedWupTotalLocal=RedWupTempLocal*RedWupPsiDepth[i];
 
		//! DISTRIBUTED Uptake AND TOTAL WATER UPTAKE 
			  SingleUptake[i]=RRoot[i]*MC_PotWaterUptake[IP]*RedWupTotalLocal;
			  WTemp[i]=RRoot[i]*MC_PotWaterUptake[IP]*RedWupTempLocal;
			  WupLoss+= max(0., RRoot[i]*MC_PotWaterUptake[IP]*RedWupTempLocal -SingleUptake[i]);
			  ETemp+=WTemp[i];
		   }
		   Transpiration[IP]=Transpiration[IP]+SingleUptake[i];
		   if(Salt_Influence_Sw(Salt_Influence)> Salt_Influence_Sw::Not_considered)
				SaltAccLayers+=RRoot[i]*WaterUptakeSaltFuncL(i, IP);

   }//EndDo	Loop_over_layers
//!
   if(PlantWaterStorage_Sw(PlantWaterStorage)> PlantWaterStorage_Sw::Not_considered) {

	   for(size_t i=0; i<WUptakeRate.size(); i++) WUptakeRate[i]+=SingleUptake[i];
	   PlantWaterUptake[IP]=sum(SingleUptake);
	   PlantWaterPotential[IP]=PlantWaterPotentialFuncL(IP);
   	   Transpiration[IP]=MC_PotWaterUptake[IP]*f_RelativeStomata_PlantPotential(PlantWaterPotential[IP]);
	   if(MC_PotWaterUptake[IP]>0) 
		   RedWupTotal[IP]=Transpiration[IP]/MC_PotWaterUptake[IP];
	   else
		   RedWupTotal[IP]=1.;	
	   
   }
   else	{

      if(WupLoss>5.E-2&&WUptake>=1&&SumR_Avail>0.) {
        WupPot2=WupLoss*UpMov;
        Transpiration[IP]=0.;
        for(size_t i=0; i<RRoot.size();i++) {
          if(p_Soil_WaterF->Psi[i]>1&&RedWupPsiDepth[i]>=1.) { 	 //! DISTRIBUTION OF COMPENSATORY WATER UPTAKE 
            SingleUptake[i]=min( max(0., (p_Soil_WaterF->Theta[i]-p_Soil_WaterF->M_Wilting[i])*p_Soil_WaterF->AThicknessLayers[i]/T_Step), 
				RRoot[i]/SumR_Avail*WupPot2 + SingleUptake[i]);
          }
	      Transpiration[IP]+=SingleUptake[i];      
		}
      }

      if(MC_PotWaterUptake[IP]>0.01) {
		RedWupTotal[IP]=Transpiration[IP]/MC_PotWaterUptake[IP];
		RedWupTemp[IP]=ETemp/MC_PotWaterUptake[IP];
	  }
	  else {
		RedWupTotal[IP]=1.;
		RedWupTemp[IP]=1.;
	  }
//!C920906 PEJ
//!c calculate integrated temp (RedWupTemp) and moisture (RedWupPsi) effect
//!
       if(ETemp>0.) 
          RedWupPsi[IP]=1-(ETemp-Transpiration[IP])/ETemp;
       else
          RedWupPsi[IP]=1;

        for(size_t i=0; i<WUptakeRate.size(); i++) WUptakeRate[i]+=SingleUptake[i];
  }

  RedWupTotal_All+=(1-RedWupPsi[IP])*MC_PotWaterUptake[IP];

  if(WUptBasic_equation_Sw(WuptBasic_equation) > WUptBasic_equation_Sw::Pressure_head_response) {//	! Resistance to mid of Root zone
  		Resist_Plant[IP]=PlantResistanceFuncL(RRoot.size()/2, IP);
  		Resist_SoilPlant[IP]=RhizoSphereResistanceFuncL(RRoot.size()/2, IP);
  }


   RedGrowthSalt[IP]=SaltAccLayers;

   if(PassiveUptake_Sw(PassiveUptake)> PassiveUptake_Sw::off)
		TE_PassiveUptake( SingleUptake, IP);
   

   if(ActiveUptake_Sw(ActiveUptake)> ActiveUptake_Sw::off) 
//! Calculated active Uptake of TraceElements
   		TE_ActiveUptake(RRoot, IP);
   


  } //EndDo	Loop_over_plants



   Transpiration_All=sum(Transpiration);

}
double WaterUptake::WaterUptakeMoistureFuncL(size_t index, size_t IndexP, double PotWaterUptake) {


	double out;


	if (Salt_Influence_Sw(Salt_Influence) == Salt_Influence_Sw::Not_considered || Salt_Influence_Sw(Salt_Influence) >= Salt_Influence_Sw::Add_multiplicative_response) { //! Salt is neglected or independent response 
		if (WUptBasic_equation_Sw(WuptBasic_equation) == WUptBasic_equation_Sw::Pressure_head_response) {
			// !PressureheadResponse
				if (MultiPlantWaterUptakeFunction == 0)
					out = f_WaterUptakeMoisture(index, p_Soil_WaterF->Psi[index], PotWaterUptake, p_Soil_WaterF->Theta[index]);
				else
					out = f_MultiWaterUptakeMoisture(IndexP, index, p_Soil_WaterF->Psi[index], PotWaterUptake, p_Soil_WaterF->Theta[index]);
		}
		else  if (MC_PotWaterUptake[IndexP]>0.)
			out = min(MaxUptakeRate(index, IndexP), MC_PotWaterUptake[IndexP]) / (MC_PotWaterUptake[IndexP] * RedWupTempLocal);
		else
			out = 1.;

		if (Salt_Influence_Sw(Salt_Influence) == Salt_Influence_Sw::Add_multiplicative_response)
			out *= WaterUptakeSaltFuncL(index, IndexP);
		else if (Salt_Influence_Sw(Salt_Influence) == Salt_Influence_Sw::Add_minimum_response)
			out = min(out, WaterUptakeSaltFuncL(index, IndexP));

	}
	else if (Salt_Influence_Sw(Salt_Influence) == Salt_Influence_Sw::Added_to_pressure_head) {
		if (WUptBasic_equation_Sw(WuptBasic_equation) == WUptBasic_equation_Sw::Pressure_head_response)  //  ! Salt is incorporated in normal response
			out = f_WaterUptakeMoisture(Index, p_Soil_WaterF->Psi[index] + OsmoticPressure[index], PotWaterUptake, p_Soil_WaterF->Theta[index]);
		else if (MC_PotWaterUptake[IndexP] > 0.)
			out = min(MaxUptakeRate(index, IndexP), MC_PotWaterUptake[IndexP]) / (MC_PotWaterUptake[IndexP] * RedWupTempLocal);
		else
			out = 1.;

	}
	else
		out = 1.;
	return out;
}
double WaterUptake::RhizoSphereResistanceFuncL(size_t Index, size_t IndexP) {

	return RootDistFuncL(Index, IndexP) / (p_Soil_WaterF->f_CondThetaMatric(true, Index, p_Soil_WaterF->Theta[Index], ((Soil_HeatF*)p_Soil_HeatF)->Temp[Index])*RRoot[Index]);
}
double WaterUptake::RootDistFuncL(size_t Index, size_t IndexP) {
	double Rdens;
	Rdens = RRoot[Index] * RootLength[IndexP] / p_Soil_WaterF->AThicknessLayers[Index];  // ! m/m3
	return f_WaterFlowDistanceInSoil(Rdens*1.E-4);					 // ! cm/cm
}
double WaterUptake::MaxUptakeRate(size_t Index, size_t IndexP) {
	double PsiSoil;
	if (Salt_Influence == 1)
		PsiSoil = p_Soil_WaterF->Psi[Index] + OsmoticPressure[Index];
	else
		PsiSoil = p_Soil_WaterF->Psi[Index];

	return max(PlantMaxSuction - PsiSoil - (PlantHeight[IndexP] + p_Soil_WaterF->DepthMidpLayer[Index])*Fix::GRAVCONST*10., 0.)
		/ (PlantResistanceFuncL(Index, IndexP) + RhizoSphereResistanceFuncL(Index, IndexP));

}
double WaterUptake::PlantResistanceFuncL(size_t Index, size_t IndexP) {

	double	OsmoticPressureScaling=1.;
	if (PlantResistance_Sw(PlantResistance) == PlantResistance_Sw::No_salt_effect)
		OsmoticPressureScaling = 1.;
	else
		OsmoticPressureScaling = f_WaterUptakeSalt(0,OsmoticPressure[Index]);

	double Temperature = ((Soil_HeatF*)p_Soil_HeatF)->Temp[Index];
	return f_PlantResistance(PlantHeight[IndexP], RootLength[IndexP] )/
		max(0.0000001, OsmoticPressureScaling*f_WaterUptakeTemp(Temperature, m_pModelInfo->JDayNum));
}
double WaterUptake::MaxPlantWaterStorage(size_t IndexP) {
	if (PlantWaterStorage_Sw(PlantWaterStorage) == PlantWaterStorage_Sw::f_LAI)
		return f_MaxStorage_LAI(LAI[IndexP]);
	else
		return f_MaxStorage_LAI_Height(LAI[IndexP],PlantHeight[IndexP]);
}
double WaterUptake::PlantWaterPotentialFuncL(size_t IndexP) {
	return min(1., max(0., 1. - PlantWater[IndexP] / MaxPlantWaterStorage(IndexP)))*
		(PlantMaxSuction - PlantHeight[IndexP] * Fix::GRAVCONST * 10) + PlantHeight[IndexP] * Fix::GRAVCONST*10.;
}
double WaterUptake::WaterUptakeSaltFuncL(size_t Index, size_t IndexPlant) {
	return f_WaterUptakeSalt(IndexPlant, OsmoticPressure[Index]);
}

void WaterUptake::Integ()
{
	OutFlow(p_Soil_WaterF->WaterStorage,WUptakeRate);
	if(PlantWaterStorage_Sw(PlantWaterStorage)> PlantWaterStorage_Sw::Not_considered) {
		InFlow(PlantWater, PlantWaterUptake);
		OutFlow(PlantWater, Transpiration);
	}

}

bool WaterUptake::End()
{
return true;
}
WaterUptake::~WaterUptake(void)
{
}
