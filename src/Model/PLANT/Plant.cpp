#include "Plant.h"
#include "../Atmosphere/SnowPack.h"
#include "../Atmosphere/Irrigation.h"
#include "../Structure/Structure.h"
#include "../../Simulator/Simulator.h"

						   
Plant::Plant(void)
{
	ModuleName="Plant";
	ModuleNo=9;
	ModuleConditions="PlantType >0";
}
bool Plant::Def()
{
	PLOT_PARAM plotpar;
	plotpar.Num_Of_Curves = 1;
	plotpar.Num_Of_Points = 300;
	plotpar.Num_Of_X = 1;
	plotpar.X_Min = 0;
	plotpar.X_Max = 1440;
	plotpar.X_Label = "Day Number";
	plotpar.Y_Label = "Potential Transpiration (mm/day)";
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.Num_Of_Opt = 1;

	ModuleNo = 9;
	ModuleConditions = "PlantType >0";
	SoilModel* pSoil = (SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	p_Soil_WaterF = &pSoil->m_SoilFrost;
	p_Soil_HeatF = &pSoil->m_HeatPump;
	AtmModel* pAtm = (AtmModel*)m_pModelInfo->GetModelPtr("Atm Model");
	p_Atm = &pAtm->m_Irrigation;
	PlantOrgModel* pPlantOrg = (PlantOrgModel*)m_pModelInfo->GetModelPtr("Plant Org Model");
	pNC_Plant = &pPlantOrg->m_NC_Plant;
	StructureModel* pStructure = (StructureModel*)m_pModelInfo->GetModelPtr("Structure Model");
	p_Structure = &pStructure->m_Structure;





	Sw* pSw;
	pSw = new Sw(&CanopyHeightInput, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pSw, "CanopyHeightInput", 0, "Parameters|Driving variable|Simulated", "Roughness = 2|PlantType > 1", 206);
	pSw = new Sw(&PlantDev, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);

	Define(pSw, "PlantDevelopment", 0, "Static|Start=f(TempSum)", "PlantType >= 2|Evaporation >= 1|PlantDrive < 1|WaterEq = 1");
	pSw = new Sw(&RootInput, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pSw, "RootInput", 0, "Parameters|Driving variable|Simulated", "PlantType > 0|WaterEq = 1|Evaporation >= 1|WUptake > 0", 200);
	pSw = new Sw(&RootDistribution, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pSw, "RootDistribution", 3, "Table|Linear|Constant|Exponential", "PlantType > 0|WUptake > 0|WaterEq = 1|Evaporation >= 1");
	pSw = new Sw(&LAIInput, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);

	Define(pSw, "LaiInput", 0, "Parameters|Driving variable|Simulated|PG File Dynamic Index style", "WUptake > 0|PlantType > 1|WaterEq = 1|Evaporation >= 1", 200);
	pSw = new Sw(&AlbedoVeg, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pSw, "AlbedoVeg", 0, "Static|Parameters|Driving variable|Simulated", "PlantType > 1", 200);
	pSw = new Sw(&SpatialPlantDist, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pSw, "SpatialDistribution", 0, "Random - Within unit area|Fixed - Within unit area|Fixed - Unrestricted area", "PlantType >= 3", 200);
	pSw = new Sw(&SLA_Dynamic, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pSw, "Dynamic Specific Leaf Area", 0, "off|on", "LaiInput = 2");

	pSw = new Sw(&AboveTable, SINGLE, GENERAL, NOFPROC, NOBPROC, NORMAL);
	Define(pSw, "AboveTable", 1, "No|Yes", "PlantType > 1");
	
	pSw = new Sw(&RootDistribution_Outputs, SINGLE, GENERAL, NOFPROC, NOBPROC, NORMAL);
	Define(pSw, "Selected Plant RootDistOutput", 0, "No Outputs|First Plant|Second Plant", "PlantType > 1");

	NE* pNumPlants;
	pNumPlants = new NE(&NumPlants, 1, 1, 10, 1, 4);
	Define(pNumPlants, "Number of Plants");

	Func *pFunc;
	string funcname;

	FunctorProp functorprop;



	vector<Ps*> ps_vec;
	vector<P*> p_vec;
	ps_vec.clear();




	Ps* pPs;
	pPs = new Ps(&PlantV_Index, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pPs, "Plant, Index in PG-file", 1., "-", "LaiInput=1", 1., 400.);
	pPs = new Ps(&PlantDynamic_IndexFile, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pPs, "Plant FileName Index", 1., "-", "LaiInput=3", 1., 4000.);


	pPs = new Ps(&RFracLow, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pPs, "RootFracExpTail", .1, "-", "PlantType > 0|RootDistribution = 3|WaterEq = 1", 0.001, 1., 0.01, 0.2);
	ps_vec.push_back(pPs);
	pFunc = new Func(PLANT, PLANT_SHAPE, NOFPROC, NOBPROC, NORMAL);
	functorprop.FuncName = MyFunc::ROOTDEPTH_EXP_DISTRIBUTION; functorprop.Arg = MyArgType::D2;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Root Depth Exp Function";

	Define(pFunc, functorprop, funcname, ps_vec);
	ps_vec.clear();




	pPs = new Ps(&TSumStart, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pPs, "TempSumStart", 50., UNIT_TYPES::TEMPSUM_UNIT, "PlantType > 0|PlantDevelopment = 1|WaterEq = 1", 10., 400., 20., 100.);
	pPs = new Ps(&TSumCrit, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pPs, "TempSumCrit", 5., UNIT_TYPES::TEMP_UNIT, "PlantType > 0|PlantDevelopment = 1|WaterEq = 1", 0., 10., 3., 7.);
	pPs = new Ps(&AlbedoLeaf, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pPs, "AlbedoLeaf", 25., "%", "AlbedoVeg = 0|PlantType >= 2", 1., 100., 8., 30.);
	pPs = new Ps(&RandomNumberSeed, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);

	Define(pPs, "RandomNumberSeed", 1., "-", "PlantType >= 3|SpatialDistribution=0", 1., 10000., 1., 10000.);

	NE *pNRootDayN, *pNoDayNum;

	pNRootDayN = new NE(&NRootDayN, 5, 1, 100, 1, 40);
	Define(pNRootDayN, "Number Root Day Layers");
	// call PnE(NRootDayN,5, 1,100,1, 10)
	// call PnE(NRootLay,10,1,100,1,40)				!Element is Table

	Tab *pTab;

	pNoDayNum = new NE(&NoDayNum, 5, 1, 20, 1, 20);
	Define(pNoDayNum, "Number of DayNumber for Single Plant Seasonality");
	P* pP;
	NE *pNL = m_pModelMap->GetNEPointer("Number of Layers");
	vector<double> initv;
	initv.resize(pNL->GetNEValue());
	for (size_t i = 0; i < pNL->GetNEValue(); i++)
		initv[i] = 1. / double(pNL->GetNEValue());
	pP = new P(&RootF, pNL, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pP, pNL, "RootFraction", initv, "-", "WUptake >= 1|RootDistribution = 0|WaterEq = 1", 0.0001, 1., 0.005, 1., 50);
	mTab_RootDist.push_back(pP);
	pTab = new Tab(&mTab_RootDist, SIMPROFILE, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pTab, "Root distribution with depth", "RootFraction", "WUptake >0|RootDistribution = 0|WaterEq = 1|PlantType > 0");

	pP = new P(&PRootDayNumber, pNRootDayN, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.resize(NRootDayN);
	initv[0] = 120.;
	for (size_t i = 1; i<initv.size(); i++)
		initv[i] = initv[i - 1] + 15;
	Define(pP, pNRootDayN, "pDay Number", initv, "#", "WUptake >= 1|RootInput = 0|WaterEq = 1|PlantType = 1", 1., 365., 1., 365.);
	mTab_RootDepth_t.push_back(pP);

	p_vec.push_back(pP);

	pP = new P(&PRootDepth, pNRootDayN, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NRootDayN, -0.1);
	Define(pP, pNRootDayN, "pRoot Depth", initv, "m", "WUptake >= 1|RootInput = 0|WaterEq = 1|PlantType > 0", -10., .0001, -3., 0.000001);
	mTab_RootDepth_t.push_back(pP);
	p_vec.push_back(pP);

	pFunc = new Func(SINGLEPLANT, PLANT_SHAPE, NOFPROC, NOBPROC, NORMAL);
	functorprop.FuncName = MyFunc::ROOTDEPTH_DAYOFYEAR; functorprop.Arg = MyArgType::D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Root Depth Day Number Function";

	Define(pFunc, functorprop, funcname, p_vec);
	p_vec.erase(p_vec.begin() + 1);


	pP = new P(&PRootLength, pNRootDayN, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NRootDayN, 10000.);
	Define(pP, pNRootDayN, "pRoot Length", initv, "m", "WUptake >= 1|RootInput = 0|WaterEq = 1|Basic equation = 1", 0., 1000., .01, 1.E6);
	mTab_RootDepth_t.push_back(pP);

	p_vec.push_back(pP);

	pFunc = new Func(SINGLEPLANT, PLANT_SHAPE, NOFPROC, NOBPROC, NORMAL);
	functorprop.FuncName = MyFunc::ROOTLENGTH_DAYOFYEAR; functorprop.Arg = MyArgType::D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Root Length Day Number Function";

	Define(pFunc, functorprop, funcname, p_vec);
	p_vec.clear();

	pTab = new Tab(&mTab_RootDepth_t, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pTab, "Root depths development with time", "pDay Number|pRoot Depth|pRoot Length", "WUptake >0|PlantType >= 1|PlantType <3|WaterEq = 1|RootInput = 0");





	initv.resize(NoDayNum);
	initv[0] = 120.;
	for (size_t i = 1; i<initv.size(); i++)
		initv[i] = initv[i - 1] + 15;

	pP = new P(&DayNum, pNoDayNum, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);  p_vec.push_back(pP);
	Define(pP, pNoDayNum, "DayNumber", initv, "#", "PlantType = 2|WaterEq = 1", 1., 365., 1., 365.);
	initv.assign(NoDayNum, 1.5);
	mTab_Above_t.push_back(pP);



	pP = new P(&LaiV, pNoDayNum, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pP, pNoDayNum, "LeafAreaIndex", initv, "-", "PlantType = 2|WaterEq = 1|LaiInput = 0", 0., 20., 0.01, 10.);
	mTab_Above_t.push_back(pP);
	pP = new P(&AlbedoV, pNoDayNum, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NoDayNum, 25.);
	Define(pP, pNoDayNum, "AlbedoV", initv, "%", "AlbedoVeg = 1|PlantType > 1", 0., 10.);
	mTab_Above_t.push_back(pP);

	pP = new P(&CanopyHeight, pNoDayNum, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NoDayNum, .1);
	Define(pP, pNoDayNum, "CanopyHeight", initv, "m", "Roughness = 2|PlantType > 1", 0., 150., 0., 40.);
	mTab_Above_t.push_back(pP);

	initv.assign(NoDayNum, 1.);
	pP = new P(&CForm, pNoDayNum, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);  p_vec.push_back(pP);
	Define(pP, pNoDayNum, "CForm", initv, "-", "PlantType = 2|WaterEq = 1", 0.01, 10., 0.1, 3.);

	mTab_Above_t.push_back(pP);

	pTab = new Tab(&mTab_Above_t, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pTab, "Above ground characteristics with time", "DayNumber|LeafAreaIndex|AlbedoV|CForm", "AboveTable = 1|PlantType = 2|WaterEq = 1");

	p_vec.push_back(mTab_Above_t[1]);
	pFunc = new Func(SINGLEPLANT, PLANT_SHAPE, NOFPROC, NOBPROC, NORMAL);
	functorprop.FuncName = MyFunc::LAI_DAYOFYEAR; functorprop.Arg = MyArgType::D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Leaf Area Index Day of year function";

	Define(pFunc, functorprop, funcname, p_vec);







	p_vec.erase(p_vec.end() - 1);  p_vec.push_back(mTab_Above_t[2]);




	functorprop.FuncName = MyFunc::ALBEDO_DAYOFYEAR; functorprop.Arg = MyArgType::D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Albedo Day of year function";
	pFunc = new Func(SINGLEPLANT, PLANT_SHAPE, NOFPROC, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);

	p_vec.erase(p_vec.end() - 1);  p_vec.push_back(mTab_Above_t[3]);

	functorprop.FuncName = MyFunc::CANOPYHEIGHT_DAYOFYEAR; functorprop.Arg = MyArgType::D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Canopy Height Day of year function";
	pFunc = new Func(SINGLEPLANT, PLANT_SHAPE, NOFPROC, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);




	pP = new P(&SpecificLeafAreaBegin, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 20.);
	Define(pP, pNumPlants, "Spec LeafArea Begin", initv, "gC/m2", "Nitrogen and Carbon >= 1|LaiInput = 2|Dynamic Specific Leaf Area=1", 0.01, 200.);//	!LK 010906 Changed from "Growth = 3".
	mTab_Size.push_back(pP);
	p_vec.clear();

	p_vec.push_back(pP);
	pP = new P(&SpecificLeafArea, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 30.);
	Define(pP, pNumPlants, "Specific LeafArea", initv, "gC/m2", "Nitrogen and Carbon >= 1|LaiInput = 2", 0.01, 200.);	//!LK 010906 Changed from "Growth = 3".
	mTab_Size.push_back(pP);
	p_vec.push_back(pP);

	functorprop.FuncName = MyFunc::LEAFAREAINDEX_PER_MASS; functorprop.Arg = MyArgType::T_D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Leaf Area Index - per mass";
	plotpar.Num_Of_Curves = 1;
	plotpar.Num_Of_Points = 100;
	plotpar.Num_Of_X = 1;
	plotpar.X_Min = 0;
	plotpar.X_Max = 300;
	plotpar.X_Label = "Leaf Carbon "; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::CARBON_MASS_UNIT, true);
	plotpar.Y_Label = "Leaf Area Index "; plotpar.Y_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::NO_UNIT, true);
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.Num_Of_Opt = 1;



	pFunc = new Func(PLANT, PLANT_SHAPE, NOFPROC, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);

	functorprop.FuncName = MyFunc::LEAFAREAINDEX_PER_MASS_STAGE; functorprop.Arg = MyArgType::T_D2;
	plotpar.Num_Of_Curves = 4;
	plotpar.X2_Vec = { 1.0f,1.25f, 1.5f, 2.f };
	plotpar.LegendsVec = { "GSI 1.0","GSI 1.25", "GSI 1.5", "GSI 2.0" };

	funcname = "LAI - per mass and development stage";
	pFunc = new Func(PLANT, PLANT_SHAPE, NOFPROC, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);




	pP = new P(&PlantMaxHeight, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 1.);
	Define(pP, pNumPlants, "Max Height", initv, "m", "Nitrogen and Carbon >= 1|CanopyHeightInput = 2", .01, 100.);
	mTab_Size.push_back(pP);

	pP = new P(&PlantHeightC1, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, .1);
	Define(pP, pNumPlants, "Height MassCoef", initv, "m2/g", "Nitrogen and Carbon >= 1|CanopyHeightInput = 2", .0, 1.);
	mTab_Size.push_back(pP);
	pP = new P(&PlantHeightC2, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pP, pNumPlants, "Height AgeCoef", initv, "1/days", "Nitrogen and Carbon >= 1|CanopyHeightInput = 2", 0., 1.);//	!LK 010906 Changed from "Growth = 3".
	mTab_Size.push_back(pP);


	p_vec.clear();

	functorprop.FuncName = MyFunc::ROOTDEPTH_PER_MASS; functorprop.Arg = MyArgType::T_D1;
	funcname = "Root Depth - per mass roots";
	plotpar.Num_Of_Curves = 1;
	plotpar.Num_Of_Points = 300;
	plotpar.Num_Of_X = 1;
	plotpar.X_Min = 0;
	plotpar.X_Max = 500.;
	plotpar.X_Label = "Below ground Carbon "; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::CARBON_MASS_UNIT, true);
	plotpar.Y_Label = "Root Depth "; plotpar.Y_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::LENGTH_UNIT, true);
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.LegendsVec = { };
	plotpar.Num_Of_Opt = 1;

	pP = new P(&RootLowestDepth, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, -1.);
	Define(pP, pNumPlants, "Root LowestDepth", initv, "m", "Nitrogen and Carbon >= 1", -5., -0.01);
	mTab_Size.push_back(pP); p_vec.push_back(pP);
	pP = new P(&RootIncDepth, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pP, pNumPlants, "Root IncDepth", initv, "m", "Nitrogen and Carbon >= 1", -100., -0.01);
	mTab_Size.push_back(pP); p_vec.push_back(pP);


	pFunc = new Func(PLANT, PLANT_SHAPE, NOFPROC, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar); p_vec.clear();





	pP = new P(&SpecificRootLength, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 0.0001);
	Define(pP, pNumPlants, "Specific RootLength", initv, "gC/m", "Nitrogen and Carbon >= 1|RootInput = 2|Basic equation = 1", 0.1E-6, 1.);
	mTab_Size.push_back(pP); p_vec.push_back(pP);

	functorprop.FuncName = MyFunc::ROOTLENGTH_PER_MASS; functorprop.Arg = MyArgType::D1;
	funcname = "Root Length - per mass roots";
	pFunc = new Func(PLANT, PLANT_SHAPE, NOFPROC, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec); p_vec.clear();




	pP = new P(&AlbedoVegStage, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 25.);
	Define(pP, pNumPlants, "AlbedoVegStage", initv, "%", "Nitrogen and Carbon >= 1|AlbedoVeg=3", 10., 30.);
	mTab_Size.push_back(pP); p_vec.push_back(pP);

	pP = new P(&AlbedoGrainStage, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 40.);
	Define(pP, pNumPlants, "AlbedoGrainStage", initv, "%", "Nitrogen and Carbon >= 1|AlbedoVeg=3", 20., 70.);
	mTab_Size.push_back(pP); p_vec.push_back(pP);

	functorprop.FuncName = MyFunc::ALBEDOVEG_PER_GROWTHSTAGE; functorprop.Arg = MyArgType::D1;
	funcname = "Albedo - per growth stage";
	
	pFunc = new Func(PLANT, PLANT_SHAPE, NOFPROC, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec); p_vec.clear();


	p_vec.clear();
	p_vec.push_back((P*)m_pModelMap->GetP_Pointer("Max Height"));
	p_vec.push_back((P*)m_pModelMap->GetP_Pointer("Height MassCoef"));
	p_vec.push_back((P*)m_pModelMap->GetP_Pointer("Height AgeCoef"));


	pP = new P(&PlantHeightC3, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 0.);
	Define(pP, pNumPlants, "Height GrainCoef", initv, "m2/g", "Nitrogen and Carbon >= 1|CanopyHeightInput = 2", 0., 1.);//	!LK 010906 Changed from "Growth = 3".
	mTab_Size.push_back(pP); p_vec.push_back(pP);
	pP = new P(&PlantHeightC4, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, .1); p_vec.push_back(pP);
	Define(pP, pNumPlants, "Height MaxGrain", initv, "", "Nitrogen and Carbon >= 1|CanopyHeightInput = 2", 0.01, 1.);//	!LK 010906 Changed from "Growth = 3".
	mTab_Size.push_back(pP);

	functorprop.FuncName = MyFunc::CANOPYHEIGHT_PER_MASS_AGE; functorprop.Arg = MyArgType::T_D3;
	funcname = "Plant Height - per mass and age";
	plotpar.Num_Of_Curves = 6;
	plotpar.Num_Of_Points = 300;
	plotpar.Num_Of_X = 1;
	plotpar.X_Min = 0;
	plotpar.X_Max = 5000;
	plotpar.X_Label = "Above ground Carbon "; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::CARBON_MASS_UNIT, true);
	plotpar.Y_Label = "Plant Height "; plotpar.Y_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::LENGTH_UNIT, true);
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.X2_Vec = { 1.0f,5.f, 10.f, 25.f, 50.f, 100.f };
	plotpar.X3_Vec = { .0f,0.f, 0.f, 0.f , 0.f, 0.f};
	plotpar.LegendsVec = { "Age 1.0","Age 5.", "Age 10.","Age 25.0","Age 50.", "Age 100." };
	plotpar.Num_Of_Opt = 1;


	pFunc = new Func(PLANT, PLANT_SHAPE, NOFPROC, NOBPROC, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);

	plotpar.Num_Of_Curves = 1;




	pTab = new Tab(&mTab_Size, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pTab, "Size and shape of growing plant", "Specific LeafArea|Max Height|Height MassCoef|Height AgeCoef|Root LowestDepth|Root IncDepth|Specific RootLength|AlbedoVegStage|AlbedoGrainStage|Height GrainCoef|Height MaxGrain|Spec LeafArea Begin",
		"Nitrogen and Carbon > 0|Growth != 0");
	p_vec.clear();
	pP = new P(&StartDayNo, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 121.);

	Define(pP, pNumPlants, "Start DayNo", initv, "#", "PlantType >= 3", 1., 365.);
	p_vec.push_back(pP);
	//Define(pP,pNoDayNum,"CanopyHeight"  ,initv,"m","Roughness = 2|PlantType > 1",0.,150., 0., 40.);
	mTab_LAI.push_back(pP);		mTab_Height.push_back(pP); mTab_Albedo.push_back(pP); mTab_RootDepth.push_back(pP); mTab_RootLength.push_back(pP);
	mTab_RoughMulti.push_back(pP); mTab_DisplMulti.push_back(pP);
	pP = new P(&OptDayNo, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 210.);
	Define(pP, pNumPlants, "Optimum DayNo", initv, "#", "PlantType >= 3", 1., 365.);
	p_vec.push_back(pP);
	mTab_LAI.push_back(pP);		mTab_Height.push_back(pP); mTab_Albedo.push_back(pP); mTab_RootDepth.push_back(pP); mTab_RootLength.push_back(pP);
	mTab_RoughMulti.push_back(pP); mTab_DisplMulti.push_back(pP);
	pP = new P(&EndDayNo, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 270.);
	Define(pP, pNumPlants, "End DayNo", initv, "#", "PlantType >= 3", 1., 365.);
	p_vec.push_back(pP);
	mTab_LAI.push_back(pP);		mTab_Height.push_back(pP); mTab_Albedo.push_back(pP); mTab_RootDepth.push_back(pP); mTab_RootLength.push_back(pP);
	mTab_RoughMulti.push_back(pP); mTab_DisplMulti.push_back(pP);
	pP = new P(&ShapeStart, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, .3);
	Define(pP, pNumPlants, "Shape Start", initv, "#", "PlantType >= 3", 0.01, 9.);
	p_vec.push_back(pP);
	mTab_LAI.push_back(pP);		mTab_Height.push_back(pP); mTab_Albedo.push_back(pP); mTab_RootDepth.push_back(pP); mTab_RootLength.push_back(pP);
	mTab_RoughMulti.push_back(pP); mTab_DisplMulti.push_back(pP);
	pP = new P(&ShapeEnd, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 3.);
	Define(pP, pNumPlants, "Shape End", initv, "#", "PlantType >= 3", 0.01, 9.);
	p_vec.push_back(pP);
	mTab_LAI.push_back(pP);		mTab_Height.push_back(pP); mTab_Albedo.push_back(pP); mTab_RootDepth.push_back(pP); mTab_RootLength.push_back(pP);
	mTab_RoughMulti.push_back(pP); mTab_DisplMulti.push_back(pP);


	pP = new P(&L_StartValue, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 0.);
	Define(pP, pNumPlants, "lStart Value", initv, "#", "PlantType >= 3|LaiInput =0", 0., 12.);
	p_vec.push_back(pP);
	mTab_LAI.push_back(pP);
	pP = new P(&L_OptValue, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 5.);
	Define(pP, pNumPlants, "lOptimum Value", initv, "#", "PlantType >= 3|LaiInput =0", 0., 20.);
	p_vec.push_back(pP);
	mTab_LAI.push_back(pP);
	pP = new P(&L_EndValue, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 0.);
	Define(pP, pNumPlants, "lEnd Value", initv, "#", "PlantType >= 3|LaiInput =0", 0., 12.);
	p_vec.push_back(pP);
	mTab_LAI.push_back(pP);

	pTab = new Tab(&mTab_LAI, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pTab, "Leaf Area Index - multiple canopies", "Start DayNo|Optimum DayNo|End DayNo|Shape Start|Shape End|lStart Value|lOptimum Value|lEnd Value",
		"PlantType > 2|LaiInput =0", 6);


	functorprop.FuncName = MyFunc::LEAFAREAINDEX_SEASON; functorprop.Arg = MyArgType::T_D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Leaf Area Index - with season";
	plotpar.X_Min = 0;
	plotpar.X_Max = 365;
	plotpar.X_Label = "Day Number";
	plotpar.Y_Label = "Leaf Area Index (-)";
	plotpar.LegendsVec = { "" };

	pFunc = new Func(PLANT, PLANT_SHAPE, EVAPOTRANSPIRATION, PHOTOSYNTHESIS, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec,plotpar);


	function<double(size_t, double)> f_test;
	f_test = Get_D1_TableFunctor(MyFunc::LEAFAREAINDEX_SEASON);
	auto s = f_test(0, 127.);

	for (size_t i = 0; i < 3; i++) { p_vec.erase(p_vec.begin() + 5); }

	pP = new P(&H_StartValue, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pP, pNumPlants, "hStart Value", initv, "#", "PlantType >= 3|CanopyHeightInput =0", 0., 120.);
	p_vec.push_back(pP);
	mTab_Height.push_back(pP);
	pP = new P(&H_OptValue, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, .5);
	Define(pP, pNumPlants, "hOptimum Value", initv, "#", "PlantType >= 3|CanopyHeightInput =0", 0., 120.);
	p_vec.push_back(pP);

	mTab_Height.push_back(pP);
	pP = new P(&H_EndValue, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 0.);
	Define(pP, pNumPlants, "hEnd Value", initv, "#", "PlantType >= 3|CanopyHeightInput =0", 0., 120.);
	p_vec.push_back(pP);
	mTab_Height.push_back(pP);
	functorprop.FuncName = MyFunc::CANOPYHEIGHT_SEASON; functorprop.Arg = MyArgType::T_D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Canopy Height - with season";
	plotpar.Y_Label = "Height (m)";
	pFunc = new Func(PLANT, PLANT_SHAPE, EVAPOTRANSPIRATION, PHOTOSYNTHESIS, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);


	f_test = Get_D1_TableFunctor(MyFunc::CANOPYHEIGHT_SEASON);
	s = f_test(0, 127.);
	for (size_t i = 0; i < 3; i++) { p_vec.erase(p_vec.begin() + 5); }


	pTab = new Tab(&mTab_Height, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pTab, "Canopy height - multiple canopies", "Start DayNo|Optimum DayNo|End DayNo|Shape Start|Shape End|hStart Value|hOptimum Value|hEnd Value",
		"PlantType > 2|CanopyHeightInput =0");

	pP = new P(&A_StartValue, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 25.);
	Define(pP, pNumPlants, "aStart Value", initv, "%", "PlantType >= 3|AlbedoVeg =1", 10., 50.);
	p_vec.push_back(pP);
	mTab_Albedo.push_back(pP);
	pP = new P(&A_OptValue, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 20.);
	Define(pP, pNumPlants, "aOptimum Value", initv, "%", "PlantType >= 3|AlbedoVeg =1", 5., 80.);
	p_vec.push_back(pP);
	mTab_Albedo.push_back(pP);
	pP = new P(&A_EndValue, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 40.);
	Define(pP, pNumPlants, "aEnd Value", initv, "%", "PlantType >= 3|AlbedoVeg =1", 5., 80.);
	p_vec.push_back(pP);
	mTab_Albedo.push_back(pP);

	pTab = new Tab(&mTab_Albedo, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	functorprop.FuncName = MyFunc::ALBEDOVEG_SEASON; functorprop.Arg = MyArgType::T_D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Albedo Vegetation - with season";
	plotpar.Y_Label = "Albedo (%)";
	pFunc = new Func(PLANT, PLANT_SHAPE, EVAPOTRANSPIRATION, PHOTOSYNTHESIS, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);


	f_test = Get_D1_TableFunctor(MyFunc::ALBEDOVEG_SEASON);
	s = f_test(0, 127.);
	for (size_t i = 0; i < 3; i++) { p_vec.erase(p_vec.begin() + 5); }




	Define(pTab, "Albedo vegetation - multiple canopies", "Start DayNo|Optimum DayNo|End DayNo|Shape Start|Shape End|aStart Value|aOptimum Value|aEnd Value",
		"PlantType > 2|AlbedoVeg =1");


	pP = new P(&R_StartValue, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 0.);
	mTab_RootDepth.push_back(pP);
	Define(pP, pNumPlants, "rStart Value", initv, "m", "PlantType >=2|RootInput =0", -20., 0.0);
	p_vec.push_back(pP);

	pP = new P(&R_OptValue, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, -0.5);
	Define(pP, pNumPlants, "rOptimum Value", initv, "m", "PlantType >= 3|RootInput =0", -20., 0.0);
	p_vec.push_back(pP);
	mTab_RootDepth.push_back(pP);
	pP = new P(&R_EndValue, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 0.0);
	Define(pP, pNumPlants, "rEnd Value", initv, "m", "PlantType >= 3|RootInput =0", -20., 0.0);
	p_vec.push_back(pP);
	mTab_RootDepth.push_back(pP);
	functorprop.FuncName = MyFunc::ROOTDEPTH_SEASON; functorprop.Arg = MyArgType::T_D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Root Depth - with season";
	plotpar.Y_Label = "Depth (m)";
	pFunc = new Func(PLANT, PLANT_SHAPE, EVAPOTRANSPIRATION, PHOTOSYNTHESIS, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar);


	f_test = Get_D1_TableFunctor(MyFunc::ROOTDEPTH_SEASON);
	s = f_test(0, 127.);
	for (size_t i = 0; i < 3; i++) { p_vec.erase(p_vec.begin() + 5); }


	pTab = new Tab(&mTab_RootDepth, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pTab, "Root depths - multiple canopies", "Start DayNo|Optimum DayNo|End DayNo|Shape Start|Shape End|rStart Value|rOptimum Value|rEnd Value",
		"PlantType > 2|RootInput =0");

	pP = new P(&RL_StartValue, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pP, pNumPlants, "rlStart Value", initv, UNIT_TYPES:: INVERSE_AREA_UNIT, "PlantType >=2|RootInput =0", -20., -0.01);
	mTab_RootLength.push_back(pP);
	p_vec.push_back(pP);

	pP = new P(&RL_OptValue, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 10000.);
	Define(pP, pNumPlants, "rlOptimum Value", initv, UNIT_TYPES::INVERSE_AREA_UNIT, "PlantType >= 3|RootInput =0", -20., -0.01);
	mTab_RootLength.push_back(pP);
	p_vec.push_back(pP);
	pP = new P(&RL_EndValue, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 0.);
	Define(pP, pNumPlants, "rlEnd Value", initv, UNIT_TYPES::INVERSE_AREA_UNIT, "PlantType >= 3|RootInput =0", -20., -0.01);
	mTab_RootLength.push_back(pP);
	p_vec.push_back(pP);
	functorprop.FuncName = MyFunc::ROOTLENGTH_SEASON; functorprop.Arg = MyArgType::T_D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Root Length - with season";
	plotpar.Y_Label = "Length ";	  plotpar.Y_Label += +"()";
	pFunc = new Func(PLANT, PLANT_SHAPE, EVAPOTRANSPIRATION, PHOTOSYNTHESIS, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec);


	f_test = Get_D1_TableFunctor(MyFunc::ROOTLENGTH_SEASON);
	p_vec.clear();

	pTab = new Tab(&mTab_RootLength, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pTab, "Root lengths - multiple canopies", "Start DayNo|Optimum DayNo|End DayNo|Shape Start|Shape End|rlStart Value|rlOptimum Value|rlEnd Value",
		"PlantType > 2|RootInput =0");


	pP = new P(&MaxPlantCover, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 1.);
	Define(pP, pNumPlants, "Max Cover", initv, UNIT_TYPES::NO_UNIT, "PlantType >=3", 0., 10., 0., 1.); p_vec.push_back(pP);
	mTab_CanCover.push_back(pP);
	pP = new P(&KExpPlantCover, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, .5);
	Define(pP, pNumPlants, "Area kExp", initv, "-", "PlantType >=3", 0., 10.); p_vec.push_back(pP);
	mTab_CanCover.push_back(pP);
	pTab = new Tab(&mTab_CanCover, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pTab, "Surface canopy cover - multiple canopies", "Max Cover|Area kExp", "PlantType > 2");

	functorprop.FuncName = MyFunc::CANOPYCOVER; functorprop.Arg = MyArgType::T_D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Canopy Cover Function";
	plotpar.X_Min = 0;
	plotpar.X_Max = 5;
	plotpar.Num_Of_Curves = 1;
	plotpar.X_Label = "LAI (-)";

	plotpar.Y_Label = "Degree of Cover (-)";
	plotpar.LegendsVec = {""};
	pFunc = new Func(PLANT, PLANT_SHAPE, EVAPOTRANSPIRATION, PHOTOSYNTHESIS, NORMAL);
	Define(pFunc, functorprop, funcname, p_vec, plotpar); p_vec.clear();




	pP = new P(&DiameterHeightRatio, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, .2);
	Define(pP, pNumPlants, "DiameterHeightRatio", initv, "-", "RadiationInput > 0|CanopyShade>0|PlantType >=3", 0.01, 10.);
	mTab_Diam.push_back(pP);

	pTab = new Tab(&mTab_Diam, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pTab, "Plant diameter - multiple canopies", "DiameterHeightRatio", "RadiationInput > 0|CanopyShade>0|PlantType > 12");

	pP = new P(&Xcenter, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	initv.assign(NumPlants, 0.5);
	Define(pP, pNumPlants, "XcenterPos", initv, "m", "RadiationInput > 0|PlantType >= 3|SpatialDistribution>=1", 0., 1.);
	mTab_Pos.push_back(pP);
	pTab = new Tab(&mTab_Pos, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pTab, "Spatial orientation - multiple canopies", "XcenterPos", "RadiationInput > 0|PlantType >=3|SpatialDistribution>=1");

	G* pG;
	initv.assign(NumPlants, 0);

	pG = new G(&SoilCoverFraction, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "Soil Cover Fraction", initv, "-", "WaterEq = 1|PlantType >= 2|Evaporation > 0");
	Gs* pGs;
	pGs = new Gs(&TotSoilCovFrac, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pGs, "Tot Soil Cover Fraction", 0., "-", "WaterEq = 1|PlantType = 3|Evaporation > 0");
	pG = new G(&PlantHeight, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "Canopy Height", initv, "m", "WaterEq = 1|PlantType >= 2|Evaporation > 0");
	pG = new G(&PlantAlbedo, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "Plant Albedo", initv, "%", "WaterEq = 1|PlantType >= 2|Evaporation > 0");
	pG = new G(&LAI, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "LeafAreaIndex", initv, "-", "WaterEq = 1|PlantType >= 2|Evaporation > 0");
	pGs = new Gs(&LAI_Total, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pGs, "LeafAreaIndexSum", 0., "-", "WaterEq = 1|PlantType = 3|Evaporation > 0");
	pG = new G(&RootDepth, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "RootDepth", initv, "m", "WUptake > 0|WaterEq = 1|Evaporation > 0|PlantType >0");
	pG = new G(&RootLength, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "RootLength", initv, "m", "WaterEq = 1|Evaporation > 0|Basic equation =1");
	pGs = new Gs(&RootLength_Total, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pGs, "RootLength_Total", 0., "-", "WaterEq = 1|PlantType = 3|Evaporation > 0|Basic equation =1");

	pG = new G(&SimRootDepth, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "SimRootDepth", initv, "m", "Nitrogen and Carbon >= 1|Growth >0");
	pG = new G(&SimRootLength, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "SimRootLength", initv, "m", "Nitrogen and Carbon >= 1|Growth >0");
	pG = new G(&SimPlantHeight, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "SimPlantHeight", initv, "m", "Nitrogen and Carbon >= 1|Growth >0");
	pG = new G(&SimPlantAlbedo, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "SimPlantAlbedo", initv, "%", "Nitrogen and Carbon >= 1|Growth >0");
	pG = new G(&SimLeafAreaIndex, pNumPlants, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pG, pNumPlants, "SimLeafAreaIndex", initv, "-", "Nitrogen and Carbon >= 1|Growth >0");


	pG = new G(&RelRootDist, pNL, SIMPROFILE, WATER, FYS_SHAPE, NOBPROC, NORMAL); initv.assign(pNL->GetNEValue(), 0.);
	Define(pG, pNL, "RootDepthDistribution", initv,UNIT_TYPES::NO_UNIT, "Nitrogen and Carbon >= 1|Growth >0|Selected Plant RootDistOutput>0");

	pGs = new Gs(&TsumPlant, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pGs, "TsumPlant", 0., "CDay", "PlantType > 0|PlantDevelopment = 1|WaterEq = 1");




	F* pF;
	pF = new F(&m_PG_Crop, PLANT, WATER, FYS_SHAPE, NOBPROC, NORMAL);
	Define(pF, "Crop data", "Crop.bin", "TypeOfDrivingFile >= 2");


	return true;
}

bool Plant::Ini()	{
	SnowPack *pMet = (SnowPack*)p_Atm;
	DrivPG2=false;
	LAI_Total = 0.;
	RelRootDist.resize(p_Soil_WaterF->NL);
	RRoot.resize(p_Soil_WaterF->NL);	


	if(RootInput==1||LAIInput==1||LAIInput==3||CanopyHeightInput==1||AlbedoVeg==2) {
		DrivPG2=true;
	}
	if(DrivPG2) {
		string filename;
		if(LAIInput==3) {
			filename=m_PG_Crop.GetFileName();
			size_t LL=filename.rfind("_");
			 if(LL!=string::npos) {
				string newname, num;
				num=FUtil::STD_ItoAscii(int(PlantDynamic_IndexFile));
				newname= filename.substr(0,LL+1)+num;
				newname+=".bin";
				m_PG_Crop.SetFileName(newname);
				m_PG_Crop.CloseFile();
			 } 

        }
        filename=m_PG_Crop.GetFileName();
		m_PG_Crop.CloseFile();
		m_PG_Crop.Open(filename);
		m_pModelInfo->Run_Res_Id=CheckEcoDriv2();
		m_PG_Crop.ResetPos();
		if(!m_pModelInfo->Run_Res_Id) return false;

	}



	if (PlantDev >= 1) {
		double DayLength = pMet->f_daylength(m_pModelInfo->JDayNum);
		if (DayLength<900) {
			Winter = true;
			DayNum.front() = 365.;
		}
		else
			Winter = false;

		if (m_pModelStructure->PlantType == 3) 	StartDayNo.assign(NumPlants, 365.);

		for (size_t i = NoDayNum - 1; i>NoDayNum; i--) {
			if (DayNum[i]>0.) {
				if (!m_pModelInfo->FIRST) DayNum[i] = 366.;
				m_pModelInfo->FIRST = true;
			}
		}
	}
	else
		Winter = false;


	if ((LAIInput == 0 && m_pModelStructure->PlantType == 1) || m_pModelStructure->PlantType == 2) {
		f_LAI_Time = Get_D1_Functor(MyFunc::LAI_DAYOFYEAR);
		f_Albedo_Time = Get_D1_Functor(MyFunc::ALBEDO_DAYOFYEAR);
		f_CanopyHeight_Time = Get_D1_Functor(MyFunc::CANOPYHEIGHT_DAYOFYEAR);
		f_RootDepth_Time = Get_D1_Functor(MyFunc::ROOTDEPTH_DAYOFYEAR);
		f_RootLength_Time = Get_D1_Functor(MyFunc::ROOTLENGTH_DAYOFYEAR);
	}
	else if (m_pModelStructure->PlantType == 3) {
		f_LAI_Season = Get_D1_TableFunctor(MyFunc::LEAFAREAINDEX_SEASON);
		f_CanopyHeight_Season = Get_D1_TableFunctor(MyFunc::CANOPYHEIGHT_SEASON);
		f_AlbedoVeg_Season = Get_D1_TableFunctor(MyFunc::ALBEDOVEG_SEASON);
		f_RootDepth_Season = Get_D1_TableFunctor(MyFunc::ROOTDEPTH_SEASON);
		f_RootLength_Season = Get_D1_TableFunctor(MyFunc::ROOTLENGTH_SEASON);
	}



	return true;
}
bool Plant::IniFunctors() {
	SnowPack *pMet = (SnowPack*)p_Atm;
	if ((LAIInput == 0 && m_pModelStructure->PlantType == 1) || m_pModelStructure->PlantType == 2) {
		f_Albedo_Time = Get_D1_Functor(MyFunc::ALBEDO_DAYOFYEAR);
		f_CanopyHeight_Time = Get_D1_Functor(MyFunc::CANOPYHEIGHT_DAYOFYEAR);
		f_RootDepth_Time = Get_D1_Functor(MyFunc::ROOTDEPTH_DAYOFYEAR);
		f_RootLength_Time = Get_D1_Functor(MyFunc::ROOTLENGTH_DAYOFYEAR);
	}
	else if (m_pModelStructure->PlantType == 3) {
		f_LAI_Season = Get_D1_TableFunctor(MyFunc::LEAFAREAINDEX_SEASON);
		f_CanopyHeight_Season = Get_D1_TableFunctor(MyFunc::CANOPYHEIGHT_SEASON);
		f_AlbedoVeg_Season = Get_D1_TableFunctor(MyFunc::ALBEDOVEG_SEASON);
		f_RootDepth_Season = Get_D1_TableFunctor(MyFunc::ROOTDEPTH_SEASON);
		f_RootLength_Season = Get_D1_TableFunctor(MyFunc::ROOTLENGTH_SEASON);
	}

	if (SLA_Dynamic == 0) 	f_LAI_Mass = Get_D1_TableFunctor(MyFunc::LEAFAREAINDEX_PER_MASS);
	else f_LAI_Mass_Stage = Get_D2_TableFunctor(MyFunc::LEAFAREAINDEX_PER_MASS_STAGE);
	if (m_pModelStructure->PlantType > 2)	f_CanopyCover = Get_D1_TableFunctor(MyFunc::CANOPYCOVER);
	f_RootDepthExpDistribution = Get_D2_Functor(MyFunc::ROOTDEPTH_EXP_DISTRIBUTION);
	f_RootDepth_Mass = Get_D1_TableFunctor(MyFunc::ROOTDEPTH_PER_MASS);
	f_CanopyHeight_Mass_Age = Get_D3_TableFunctor(MyFunc::CANOPYHEIGHT_PER_MASS_AGE);
	f_RootLength_Mass = Get_D1_TableFunctor(MyFunc::ROOTLENGTH_PER_MASS);
	f_Albedo_GrowthStage = Get_D1_TableFunctor(MyFunc::ALBEDOVEG_PER_GROWTHSTAGE);
	return true;
}
bool Plant::Dyn()
{
	if(m_pModelStructure->PlantType>0) {
		Flux();
		Integ();
	}
	return true;
}

void Plant::Flux()
{
	double TSTEP_PLANT;
	SnowPack *pMet=(SnowPack*)p_Atm;

	//if(pModelStructure->TimeResolution==0)	//	! Changed from 0 to 1 11 september 2005
		TSTEP_PLANT=T_Step;
//	else
//		TSTEP_PLANT=pModelStructure->TSTEP_SNOW;
	
	

	if (m_pModelStructure->PlantType <= 1) return;

	if(m_pModelStructure->PlantType<3) { // ! Single Canopy

	 if(PlantDev>=1) {
	    double DayLength=pMet->f_daylength(m_pModelInfo->JDayNum);
	    if(pMet->TAirDailyMean>TSumCrit&&DayLength>600) {
	       T5=T5+(pMet->TAirDailyMean-TSumCrit)*TSTEP_PLANT;
	       if(T5>=TSumStart&&Winter) {
	          DayNum.front()=m_pModelInfo->JDayNum;
	          Winter=false;
			  IniFunctors();
	       }
	       Islut=0;
		}
	    else if(pMet->TAirDailyMean<TSumCrit&&DayLength<720) {
	       Islut=Islut+TSTEP_PLANT;
	       if(Islut>=5.&&!Winter&&DayLength<720) {
	         Winter=true;
	         DayNum.front()=365.;
	         T5=0;
			 IniFunctors();
	       }
		}
	    else
	       Islut=0;
	     
	   }
	
		//TIME_INTP vint;
		//int=InterPlant(m_pModelInfo->JDayNum);
	

	
		if(LAIInput==1||LAIInput==3) 
				LAI.front()=GetEcoDat(&m_PGPos_Crop, LAI_Ipos);
		else  if (LAIInput == 0) {
			//LAI.front() = LeafAreaIndexTimeFunction(m_pModelInfo->JDayNum);
			LAI.front() = f_LAI_Time(m_pModelInfo->JDayNum);
		}
		else
			LAI.front()=SimLeafAreaIndex.front();
		
	
		//!SnowCorrection of leaf area index suggested by DG 29/9-97, update to MC 18/4-01
		if(pMet->FracBare<0.99) LAI.front()=LAI.front()*pMet->SnowReduceLAIFactor.front();

		LAI_Total=LAI.front();
		if (CanopyHeightInput == 0) {
			//PlantHeight.front() = CanopyHeightFunction(m_pModelInfo->JDayNum);
			PlantHeight.front() = f_CanopyHeight_Time(m_pModelInfo->JDayNum);
		}
		else if(CanopyHeightInput==1) 
			 PlantHeight.front()=GetEcoDat(&m_PGPos_Crop,HeightIpos);
		else if(CanopyHeightInput==2) 
			PlantHeight.front()=CanopyHeightVar;
		

		if(RootInput==0) {
			RootDepth.front()=f_RootDepth_Time(m_pModelInfo->JDayNum);
			if(WuptBasic_equation==1) RootLength.front()=f_RootLength_Time(m_pModelInfo->JDayNum);
		}
		else if(RootInput==1) {
			if(WuptBasic_equation==1) {
				if(RootLengthIpos.ivar>0) 
					 RootLength.front()=GetEcoDat(&m_PGPos_Crop,RootLengthIpos);
				 else
					 RootLength.front()=f_RootLength_Time(m_pModelInfo->JDayNum);
				 
			}
			RootDepth.front()=GetEcoDat(&m_PGPos_Crop,RootIpos);
		}
		else {
			RootDepth.front()=SimRootDepth.front();
			if(WuptBasic_equation==1) RootLength.front()=SimRootLength.front();
		}


	}
	else	{						//		! Multiple canopies
		if(PlantDev>=1) {
		    m_pModelInfo->DayLength=pMet->f_daylength(m_pModelInfo->JDayNum);
		    if(pMet->TAirDailyMean>TSumCrit&&m_pModelInfo->DayLength>600) {
		       T5=T5+(pMet->TAirDailyMean-TSumCrit)*TSTEP_PLANT;
		       if(T5>=TSumStart&&Winter) {
					for(size_t i=0; i<NumPlants;i++)
			          StartDayNo[i]=m_pModelInfo->JDayNum;

		          Winter=false;
				  IniFunctors();
		       }
		       Islut=0;
			 }
		     else if(pMet->TAirDailyMean<TSumCrit&&m_pModelInfo->DayLength<720) {
		       Islut=Islut+TSTEP_PLANT;
		       if(Islut>=5&&!Winter&&m_pModelInfo->DayLength<720) {
		         Winter=true;
				 for(size_t i=0; i<NumPlants;i++)
					StartDayNo[i]=365.;
		         T5=0;
		       }
			 }
		     else
		       Islut=0;
		}
		else
			Winter=false;

		LAI_Total=0.;
		
		DefineMultiplePlantShapes();
		RootLength_Total=sum(RootLength);

	}
	TsumPlant=T5;
}
void Plant::Integ()
{
}

void  Plant::DefineMultiplePlantShapes() {
		SnowPack *pMet=(SnowPack*)p_Atm;


		//m_pModelStructure = ((Simulator*)m_pModelInfo)->GetSimStructurePointer();
		size_t DRIVPG = m_pModelStructure->DRIVPG;

		for(size_t i=0; i<NumPlants; i++) {

			if(LAIInput==2) 
				LAI[i]=SimLeafAreaIndex[i];
			else  if(LAIInput==1||LAIInput==3&&DRIVPG==2&&i==0) {
		     //   CALL READECO(T_Current, IPLANT, NumValidInput2, VUTPG_File)
			//	LAI[i]=VUTPG_File(LAI_Ipos)
				if(PlantExtraPosIvar.size()>i)
					LAI[i]=GetEcoDat(&m_PGPos_Crop, LAI_Ipos, PlantExtraPosIvar[i]);
				else
					LAI[i]=GetEcoDat(&m_PGPos_Crop, LAI_Ipos);
			}
			else								
				LAI[i]=f_LAI_Season(i, m_pModelInfo->JDayNum);
			
			//!SnowCorrection of leaf area index suggested by DG 29/9-97, update to MC 18/4-01
			if(pMet->FracBare<0.99) LAI[i]=LAI[i]*pMet->SnowReduceLAIFactor[i];

			LAI_Total+=LAI[i];
			if(CanopyHeightInput==2) 
				PlantHeight[i]=SimPlantHeight[i];
			else  if(CanopyHeightInput==1&&m_pModelStructure->DRIVPG==2&&i==0) {
		        //CALL READECO(T_Current, IPLANT, NumValidInput2, VUTPG_File)
				//PlantHeight[i]=VUTPG_File(HeightIpos)
				PlantHeight[i]=GetEcoDat(&m_PGPos_Crop, HeightIpos);
			}
			else
				PlantHeight[i]=f_CanopyHeight_Season(i, m_pModelInfo->JDayNum);


			if(RootInput==2) {
				RootDepth[i]=SimRootDepth[i];
				if(WuptBasic_equation==1) RootLength[i]=SimRootLength[i];
			}
			else {
				RootDepth[i]=f_RootDepth_Season(i, m_pModelInfo->JDayNum);
				if(WuptBasic_equation==1) RootLength[i]=f_RootLength_Season(i, m_pModelInfo->JDayNum);
			}

			if (AlbedoVeg == 3)
				PlantAlbedo[i] = SimPlantAlbedo[i];
			else if (AlbedoVeg == 2) {
				//    CALL READECO(T_Current, IPLANT, NumValidInput2, VUTPG_File)
				PlantAlbedo[i] = GetEcoDat(&m_PGPos_Crop, AlbedoVegIpos);
			}
			else if (AlbedoVeg == 1)
				PlantAlbedo[i] = f_AlbedoVeg_Season(i, m_pModelInfo->JDayNum);
			else
				PlantAlbedo[i] = AlbedoLeaf;
			

		}

	}

bool Plant::End()
{
return true;
}
Plant::~Plant(void)
{
}
bool Plant::CheckEcoDriv2() 
{
	string str;

	int Imiss, CountHeight, CountLAI;
	CountHeight=CountLAI=0;
	if(!m_PG_Crop.ReOpen()) return false;
	m_PG_Crop.SetEndPoints_ValidValue();

   Imiss=0;
   LAI_Ipos.ivar=RoughIpos.ivar=DisplIpos.ivar=HeightIpos.ivar=ResSurfIpos.ivar=RootIpos.ivar=0;
   RootLengthIpos.ivar=AlbedoVegIpos.ivar=0;
   LAI_Ipos.pgfile=RoughIpos.pgfile=DisplIpos.pgfile=HeightIpos.pgfile=ResSurfIpos.pgfile=RootIpos.pgfile=&m_PG_Crop;
   RootLengthIpos.pgfile=AlbedoVegIpos.pgfile=&m_PG_Crop;
   LAI_Ipos.discrete=RoughIpos.discrete=DisplIpos.discrete=HeightIpos.discrete=ResSurfIpos.discrete=RootIpos.discrete=false;
   RootLengthIpos.discrete=AlbedoVegIpos.discrete=false;
   LAI_Ipos.shift=RoughIpos.shift=DisplIpos.shift=HeightIpos.shift=ResSurfIpos.shift=RootIpos.shift=false;
   RootLengthIpos.shift=AlbedoVegIpos.shift=false;

   PlantExtraPosIvar.resize(NumPlants);
   for(size_t i=0; i<m_PG_Crop.GetNumVariables(); i++) {
		 string name=m_PG_Crop.GetVarName(i+1);
		 string Id=m_PG_Crop.GetVarId(i+1);

		 for(size_t ii=0; ii<20;ii++) {
			if(int(name[ii])>=97) {
				name[ii]=char(int(name[ii])-32);
			}
		 }           
		if(name.find("LAI")!=string::npos) {
			  CountLAI+=1;
			  if(CountLAI==PlantV_Index) {
				  LAI_Ipos.ivar=i+1;
				  PlantExtraPosIvar[CountLAI-1]=0;
			  }
			  else if(LAI_Ipos.ivar-1<NumPlants) {
				  PlantExtraPosIvar[CountLAI-1]=i+1-LAI_Ipos.ivar;
			  }
		}  
		else if(name.find("LEAF")!=string::npos) {
			  CountLAI+=1;
			  if(CountLAI==PlantV_Index)  {
					LAI_Ipos.ivar=i+1;
					PlantExtraPosIvar[CountLAI-1]=0;
			  }
			  else if(LAI_Ipos.ivar-1<NumPlants) {
				  PlantExtraPosIvar[CountLAI-1]=i+1-LAI_Ipos.ivar;
			  }
		}
  		else if(name.find("ROUGH")!=string::npos&&RoughIpos.ivar==0) 
			  RoughIpos.ivar=i+1;
		else if(name.find("DISPL")!=string::npos&&DisplIpos.ivar==0)
			  DisplIpos.ivar=i+1;
		else if(name.find("HEIGHT")!=string::npos) {
			  CountHeight+=1;
			  if(CountHeight==PlantV_Index) 
					HeightIpos.ivar=i+1;
		}
		else if(name.find("ROOT")!=string::npos&&RootIpos.ivar==0)
			  RootIpos.ivar=i+1;
		else if(name.find("LROOT")!=string::npos&&RootLengthIpos.ivar==0) 
			  RootLengthIpos.ivar=i+1;
		else if(name.find("ALBEDO")!=string::npos&&AlbedoVegIpos.ivar==0)
			  AlbedoVegIpos.ivar=i+1;
		else if(name.find("RESSURF")!=string::npos&&ResSurfIpos.ivar==0) 
			ResSurfIpos.ivar=i+1;
		else if(name.find("                    ")!=string::npos) {
			 Imiss=Imiss+1;
			 CountLAI+=1;
			  if(CountLAI==PlantV_Index) {
				  LAI_Ipos.ivar=i+1;
				  PlantExtraPosIvar[CountLAI-1]=0;
			  }
			  else if(LAI_Ipos.ivar-1<NumPlants) {
				  PlantExtraPosIvar[CountLAI-1]=i+1-LAI_Ipos.ivar;
			  }


		}
		else {
			 Imiss=Imiss+1;


		}
    }

	 if(Imiss>0) {
	 		 Message(FUtil::STD_ItoAscii(Imiss)+" - Driving variable not identified in Crop Input- LAI assumed");


	 }
	 NumValidInput2=m_PG_Crop.GetNumVariables()-Imiss;

	return m_pModelInfo->Run_Res_Id;
}
bool Plant::Root_Check() {
	double SumRoot;

	if (RootDistribution == 0) {

		SumRoot = sum(RootF);

		if (SumRoot != 1 && SumRoot>0) {
			for (size_t i = 0; i<RootF.size(); i++)
				RootF[i] = RootF[i] / SumRoot;
			return false;
		}
		else
			return true;
	}
	return false;
}
