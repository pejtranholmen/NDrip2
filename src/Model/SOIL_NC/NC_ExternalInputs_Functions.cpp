#include "NC_ExternalInputs_Functions.h"
#include "../SOIL/SoilModel.h"
#include "../PLANT/PlantModel.h"
#include "../PLANT_NC/PlantOrgModel.h"
#include "../PLANT/WaterUptake.h"


NC_ExternalInputs_Functions::NC_ExternalInputs_Functions(void)
{


}
NC_ExternalInputs_Functions::~NC_ExternalInputs_Functions(void)
{
}


bool NC_ExternalInputs_Functions::Def()
{
	ModuleNo=29;
	ModuleConditions="";
	SoilModel* pSoil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	p_Soil_WaterF=&pSoil->m_SoilFrost;
	PlantModel* pPlant=(PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
	WaterUptake* p_Plant=&pPlant->m_WaterUptake;

	PlantOrgModel* pPlantOrg = (PlantOrgModel*)m_pModelInfo->GetModelPtr("Plant Org Model");
	p_NC_Plant = &pPlantOrg->m_NC_Plant;

	Sw* pSw; Xs* pXs; Ts *pTs; Gs *pGs; T* pT; P* pP; Ps *pPs; FCSV *pFCSV;
	NE *pNumPlants=m_pModelMap->GetNEPointer("Number of Plants");
	pSw= new Sw(&N_Fertilization, ATMBOUNDARY, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL);
		Define(pSw,   "N Fertilization", 0, "off|Parameters|PG-file|Plant Stage Table|Year Based Table - 1 app|Year Based Table - 3 app","Nitrogen and Carbon >0", 117);
	pSw= new Sw(&FertilizationIndexFileName, ATMBOUNDARY,  CLIMATE, NOFPROC, NOBPROC, NORMAL);
	Define(pSw, "Dynamic indexed input file",0,"off|Using Single PG file|Using Multi PG file","N Fertilization=2");
	pSw= new Sw(&DepositionIndexFileName, ATMBOUNDARY,  CLIMATE, NOFPROC, NOBPROC, NORMAL);
	Define(pSw, "Dynamic indexed deposition file",0,"off|Using Single PG file|Using Multi PG file","N Deposition=2");	
		
	pSw= new Sw(&N_Irrigation, ATMBOUNDARY, NITROGEN, TRANSPORT, FORCING_BIOLOGY,NORMAL);
 	    Define(pSw,  "N Irrigation",0,"off|on","Nitrogen and Carbon >0", 118);
	pSw= new Sw(&N_Deposition, ATMBOUNDARY, NITROGEN, TRANSPORT,  FORCING_BIOLOGY, NORMAL);
 		Define(pSw,   "N Deposition", 0, "off|Parameters|PG-file","Nitrogen and Carbon >0");
	pSw= new Sw(&ManureInput, ATMBOUNDARY, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL);
 		Define(pSw,   "Manure Input", 0, "Parameters|PG-file","Nitrogen and Carbon >0|FaecesPool>0", 117);
	pSw = new Sw(&ReadYearBasedManagementTable, ATMBOUNDARY, NITROGEN, NOFPROC, NOBPROC, NORMAL);
		Define(pSw, "Read Values from Dynamic indexed csv-file", 0, "off|on", "N Fertilization=6",12000);
	pXs= new Xs(&N_Fert , SOILSURFACE, NITROGEN, STORAGE, FORCING_BIOLOGY, NORMAL);
		Define(pXs, "N Fertilizer", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|N Fertilization >0");
	pTs= new Ts(&N_Fert_App, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); 
		Define(pTs, "N Fert Appl Rate", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|N Fertilization >0");
	pTs= new Ts(&N_Fert_NO3, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); 
		Define(pTs,   "N Fert NO3 Dis Rate", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|N Fertilization >0");
	pTs= new Ts(&N_Fert_NH4, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); 
		Define(pTs,   "N Fert NH4 Dis Rate", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|N Fertilization >0");
	pTs= new Ts(&N_Manure_NH4,  SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); 
		Define(pTs,  	"N Manure NH4 Rate", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|FaecesPool >0");
	pTs= new Ts(&N_Manure_Litter,  SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); 
		Define(pTs,   "N Manure Litter Rate", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|FaecesPool >0");
	pTs= new Ts(&N_Manure_Faeces,  SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); 
		Define(pTs,   "N Manure Faeces Rate", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|FaecesPool >0");
	pTs= new Ts(&C_Manure_Litter,  SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); 
		Define(pTs,   "C Manure Litter Rate", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|FaecesPool >0");
	pTs= new Ts(&C_Manure_Faeces,  SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); 
		Define(pTs,   "C Manure Faeces Rate", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|FaecesPool >0");
	pTs= new Ts(&N_Deposition_NO3,  SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); 
		Define(pTs,   "Deposition NO3 Rate", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|N Deposition >0");
	pTs= new Ts(&N_Deposition_NH4,  SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); 
		Define(pTs,   "Deposition NH4 Rate", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|N Deposition >0");
		vector<double> initv;
	pT= new T(&N_Deposition_Leaf, pNumPlants,  SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL);initv.assign(p_Plant->NumPlants,0.); 
		Define(pT, pNumPlants,  "Deposition N Leaf",initv,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|N Deposition >0");
	pGs= new Gs(&Total_N_DepositionLeaf, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); 
		Define(pGs,   "Total Deposition N Leaf", 0.0,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|N Deposition >0");
	pGs= new Gs(&N_Irrig_App, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); 
	    Define(pGs,   "N Irrig Appl Rate", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|N Irrigation >0");
	pPs= new Ps(&N_FertDiss_k, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); 
		Define(pPs,  "N Fert Dis k",0.15,"-", "Nitrogen and Carbon >= 1|N Fertilization >=1",0.001,1.0);
	pPs= new Ps(&N_Fert_NH4_Frac, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); 
		Define(pPs,  "N Fert NH4 Frac",0.15,"-", "Nitrogen and Carbon >= 1|N Fertilization >=1",0.,1.0);
	pPs= new Ps(&N_Irrig_NH4_Conc,  SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); 
		Define(pPs,  "N Irrig NH4 Conc",0.15,"mgN/l", "Nitrogen and Carbon >= 1|Irrigation>0",0.,500.);
	pPs= new Ps(&N_Irrig_NO3_Conc,  SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); 
		Define(pPs,  "N Irrig NO3 Conc",0.15,"mgN/l", "Nitrogen and Carbon >= 1|Irrigation>0",0.,500.);

 
return FunctionDef();
}


bool NC_ExternalInputs_Functions::FunctionDef()
{
	PlantModel* pPlant = (PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
	WaterUptake* p_Plant = &pPlant->m_WaterUptake;
	// Parameters;
	string funcname;
	FunctorProp functorprop;
	vector<Ps*> ps_vec;
	vector<P*> p_vec;
	ps_vec.clear();
	Func *pFunc;
	functorprop.FunctorInitOption = string::npos;
	functorprop.pModelInfo = m_pModelInfo;

	PLOT_PARAM plotpar;
	plotpar.Num_Of_Curves = 1;
	plotpar.Num_Of_Points = 366;
	plotpar.Num_Of_X = 1;
	plotpar.X_Min = 0;
	plotpar.X_Max = 366;
	plotpar.X_Label = "Day Number";
	plotpar.Y_Label = "Fetilization Rate (gN/m2/day)";
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.Num_Of_Opt = 1;


	P* pP; Ps *pPs; FCSV *pFCSV;
	NE *pNumPlants = m_pModelMap->GetNEPointer("Number of Plants");
		vector<double> initv;


	NE *pNFertDayNo;


	pNFertDayNo = new NE(&NFertDayNo, 1, 1, 100, 1, 10);
	Define(pNFertDayNo, "Num of Fert Days");
	functorprop.FuncName = MyFunc::N_FERTILIZATION_DAY;
	functorprop.Arg = MyArgType::D1;
	functorprop.Set_PlotPar = true;
	funcname = "Daily Fertilization within years";


	pP = new P(&N_Fert_DayNo, pNFertDayNo, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); initv.assign(NFertDayNo, 121.);
	Define(pP, pNFertDayNo, "Fert DayNo", initv, "#", "Nitrogen and Carbon >= 1", 1., 366.);
	mTab_Fert.push_back(pP); p_vec.push_back(pP);
	pP = new P(&N_Fert_Rate, pNFertDayNo, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); initv.assign(NFertDayNo, 12.);
	Define(pP, pNFertDayNo, "N Fert Rate", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1", 0., 100.);
	mTab_Fert.push_back(pP); p_vec.push_back(pP);
	pFunc = new Func(SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL);
	Define(pFunc, functorprop, funcname,ps_vec, p_vec, plotpar); p_vec.clear();	


	Tab *pTab;
	pTab = new Tab(&mTab_Fert, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL);
	Define(pTab, "N Fertilization", "Fert DayNo|N Fert Rate", "Nitrogen and Carbon > 0|N Fertilization =1");

	NE *pNFertYearTableNo;
	pNFertYearTableNo = new NE(&NFertYearTableNo, 1, 1, 5, 1, 100);
	Define(pNFertYearTableNo, "Num of Fert Years");

	functorprop.FuncName = MyFunc::N_FERTILIZATION_TABLE_YEAR_1APP;
	functorprop.Arg = MyArgType::D2;
	functorprop.Set_PlotPar = true;
	plotpar.Num_Of_Points = 100;
	plotpar.Num_Of_X = 2;
	plotpar.X2_Vec = { 121. };
	plotpar.X_Min = 1920;
	plotpar.X_Max = 2020;
	plotpar.X_Label = "Year";


	funcname = "Yearly Fertilization 1 App";
	ps_vec.clear(); p_vec.clear();

	pP = new P(&N_Fert_Year, pNFertYearTableNo, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); initv.assign(NFertYearTableNo, 2015.);
	Define(pP, pNFertYearTableNo, "Fert Year", initv, "#", "Nitrogen and Carbon >= 1|N Fertilization >3", 1900., 2100.);
	mTab_Fert_YearTable.push_back(pP); p_vec.push_back(pP);

	pP = new P(&N_Fert_DayNo1, pNFertYearTableNo, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); initv.assign(NFertYearTableNo, 121.);
	Define(pP, pNFertYearTableNo, "Fert DayNo_1 ", initv, "#", "Nitrogen and Carbon >= 1|N Fertilization >3", 1., 366.);
	mTab_Fert_YearTable.push_back(pP); p_vec.push_back(pP);
	pP = new P(&N_Fert_Rate1, pNFertYearTableNo, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); initv.assign(NFertYearTableNo, 12.);
	Define(pP, pNFertYearTableNo, "N Fert Rate1", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|N Fertilization >3", 0., 100.);
	mTab_Fert_YearTable.push_back(pP); p_vec.push_back(pP);

	pFunc = new Func(SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, p_vec, plotpar);
	functorprop.FuncName = MyFunc::N_FERTILIZATION_TABLE_YEAR_3APP;
	funcname = "Yearly Fertilization 3 App";

	pP = new P(&N_Fert_DayNo2, pNFertYearTableNo, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); initv.assign(NFertYearTableNo, 151.);
	Define(pP, pNFertYearTableNo, "Fert DayNo_2 ", initv, "#", "Nitrogen and Carbon >= 1|N Fertilization >4", 1., 366.);
	mTab_Fert_YearTable.push_back(pP); p_vec.push_back(pP);
	pP = new P(&N_Fert_Rate2, pNFertYearTableNo, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); initv.assign(NFertYearTableNo, 12.);
	Define(pP, pNFertYearTableNo, "N Fert Rate2", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|N Fertilization >4", 0., 100.);
	mTab_Fert_YearTable.push_back(pP); p_vec.push_back(pP);
	pP = new P(&N_Fert_DayNo3, pNFertYearTableNo, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); initv.assign(NFertYearTableNo, 181.);
	Define(pP, pNFertYearTableNo, "Fert DayNo_3 ", initv, "#", "Nitrogen and Carbon >= 1|N Fertilization >4", 1., 366.);
	mTab_Fert_YearTable.push_back(pP); p_vec.push_back(pP);
	pP = new P(&N_Fert_Rate3, pNFertYearTableNo, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); initv.assign(NFertYearTableNo, 12.);
	Define(pP, pNFertYearTableNo, "N Fert Rate3", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|N Fertilization >4", 0., 100.);
	mTab_Fert_YearTable.push_back(pP); p_vec.push_back(pP);
	functorprop.FuncName = MyFunc::N_FERTILIZATION_TABLE_YEAR_3APP;
	pFunc = new Func(SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, p_vec, plotpar);


	pTab = new Tab(&mTab_Fert_YearTable, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL);
	Define(pTab, "N Fertilization Year Table", "Fert Year|Fert DayNo1|N Fert Rate1|Fert DayNo2|N Fert Rate2", "Nitrogen and Carbon > 0|N Fertilization >3");
	
	
	
	NE *pNFertStageNo;
	pNFertStageNo = new NE(&NFertStageNo, 1, 1, 5, 1, 5);
	Define(pNFertStageNo, "Num of Fert Stages");

	functorprop.FuncName = MyFunc::N_FERTILIZATION_STAGE_YEAR;
	functorprop.Arg = MyArgType::D2;
	functorprop.Set_PlotPar = true;
	plotpar.Num_Of_Points = 100;
	plotpar.Num_Of_X = 2;
	plotpar.X2_Vec = { 121. };
	plotpar.X_Min = 0;
	plotpar.X_Max = 100;
	plotpar.X_Label = "Plant Age (Year)";


	funcname = "Yearly Fertilization - Plant Age";
	ps_vec.clear(); p_vec.clear();
	pP = new P(&N_Fert_YearStage, pNFertStageNo, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); initv.assign(NFertStageNo, 12.);
	Define(pP, pNFertStageNo, "Max Age Plant Stage", initv, "Year", "Nitrogen and Carbon >= 1", 0., 100.);
	mTab_FertStage.push_back(pP); p_vec.push_back(pP);
	pP = new P(&N_Fert_YearRate, pNFertStageNo, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); initv.assign(NFertStageNo, 12.);
	Define(pP, pNFertStageNo, "Yearly N Rate", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1", 0., 100.);
	mTab_FertStage.push_back(pP); p_vec.push_back(pP);
	pP = new P(&N_Fert_YearFreq, pNFertStageNo, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); initv.assign(NFertStageNo, 1.);
	Define(pP, pNFertStageNo, "Yearly N Frequency", initv, "-", "Nitrogen and Carbon >= 1", 0., 100.);
	mTab_FertStage.push_back(pP); p_vec.push_back(pP);
	pTab = new Tab(&mTab_FertStage, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL);
	
	p_vec.push_back(m_pModelMap->GetPPointer("Fert DayNo"));
	pFunc = new Func(SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, p_vec, plotpar); p_vec.clear();
	
	Define(pTab, "N Plant Stage Fertilization", "Max Age Plant Stage|Yearly N Rate|Yearly N Frequency", "Nitrogen and Carbon > 0|N Fertilization =3");
	
	
	
	
	pP = new P(&Dep_N_toLeaf, pNumPlants, PLANT, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); initv.assign(p_Plant->NumPlants, 1.E-5);
	Define(pP, pNumPlants, "Dep N to Leaf", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|N Deposition >0", 0., 0.01);
	mTab_NDep.push_back(pP);
	pTab = new Tab(&mTab_NDep, PLANT, NITROGEN, TRANSPORT, NFIXATION, NORMAL);
	Define(pTab, "Specific N Deposition uptake leaf", "Dep N to Leaf", "Nitrogen and Carbon > 0|N Deposition>0");

	pPs = new Ps(&Dep_N_DryRate, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL);
	Define(pPs, "Dep N DryRate", 0.001,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|N Deposition >0", 0., 0.1);
	pPs = new Ps(&Dep_N_WetConc, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL);
	Define(pPs, "Dep N WetConc", 0.1, "mgN/l", "Nitrogen and Carbon >= 1|N Deposition >0", 0., 50.);
	pPs = new Ps(&Dep_NH4_FracW, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL);
	Define(pPs, "Dep NH4 FracWet", 0.5, "-", "Nitrogen and Carbon >= 1|N Deposition >0", 0., 1.);
	pPs = new Ps(&Dep_NH4_FracD, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL);
	Define(pPs, "Dep NH4 FracDry", 0.5, "-", "Nitrogen and Carbon >= 1|N Deposition >0", 0., 1.);
	pPs = new Ps(&CsvFileManagementIndex, ATMBOUNDARY, NITROGEN, NOFPROC, NOBPROC, NORMAL);
	Define(pPs, "Index Fert Table CsvFile", 1., "", "Nitrogen and Carbon >= 1|N Fertilization >3", 1., 10000.);

	NE *pNManureDayNo;
	pNManureDayNo = new NE(&NManureDayNo, 1, 1, 100, 1, 10);
	Define(pNManureDayNo, "Num of Manure Days");
	pP = new P(&Manure_DayNo, pNManureDayNo, ATMBOUNDARY, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); initv.assign(NManureDayNo, 151);
	Define(pP, pNManureDayNo, "ManDayNo", initv, "#", "Nitrogen and Carbon >= 1|FaecesPool >0", 1., 365.);
	mTab_Manure.push_back(pP);
	pP = new P(&Manure_Faeces, pNManureDayNo, ATMBOUNDARY, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); initv.assign(NManureDayNo, 2.);
	Define(pP, pNManureDayNo, "N Faeces", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|FaecesPool >0", 0., 30.);
	mTab_Manure.push_back(pP);
	pP = new P(&Manure_Litter, pNManureDayNo, ATMBOUNDARY, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); initv.assign(NManureDayNo, 2.);
	Define(pP, pNManureDayNo, "N Litter", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|FaecesPool >0", 0., 30.);
	mTab_Manure.push_back(pP);
	pP = new P(&Manure_NH4, pNManureDayNo, ATMBOUNDARY, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); initv.assign(NManureDayNo, 6.);
	Define(pP, pNManureDayNo, "N NH4", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|FaecesPool >0", 0., 50.);
	mTab_Manure.push_back(pP);
	pP = new P(&Manure_CN_Litter, pNManureDayNo, ATMBOUNDARY, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); initv.assign(NManureDayNo, 30.);
	Define(pP, pNManureDayNo, "CN Litter", initv, "-", "Nitrogen and Carbon >= 1|FaecesPool >0", 10., 100.);
	mTab_Manure.push_back(pP);
	pP = new P(&Manure_CN_Faeces, pNManureDayNo, ATMBOUNDARY, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); initv.assign(NManureDayNo, 20.);
	Define(pP, pNManureDayNo, "CN Faeces", initv, "-", "Nitrogen and Carbon >= 1|FaecesPool >0", 10., 40.);
	mTab_Manure.push_back(pP);
	pP = new P(&Manure_Depth, pNManureDayNo, ATMBOUNDARY, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); initv.assign(NManureDayNo, -0.3);
	Define(pP, pNManureDayNo, "ManDepth", initv, "m", "Nitrogen and Carbon >= 1|FaecesPool >0", -.5, 0.);
	mTab_Manure.push_back(pP);
	pP = new P(&Manure_YearFreq, pNFertDayNo, ATMBOUNDARY, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL); initv.assign(NFertDayNo, 1.);
	Define(pP, pNFertDayNo, "M YearFrequency", initv, "-", "Nitrogen and Carbon >= 1|FaecesPool >0", 1., 10.);
	mTab_Manure.push_back(pP);
	pTab = new Tab(&mTab_Manure, ATMBOUNDARY, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL);
	Define(pTab, "N Manure Application", "ManDayNo|N Faeces|N Litter|N NH4|CN Litter|CN Faeces|ManDepth|M YearFrequency", "Nitrogen and Carbon > 0|FaecesPool >0");
	pPs = new Ps(&ManureYearStart, SOILSURFACE, NITROGEN, TRANSPORT, FORCING_BIOLOGY, NORMAL);
	Define(pPs, "Manure YearStart", 0., "-", "Nitrogen and Carbon >= 1|FaecesPool >0", 0., 1.);
	F* pF = new F(&m_PG_Manure, SOILSURFACE, MANURE, NOFPROC, FORCING_BIOLOGY, NORMAL);
	Define(pF, "Manure", "Manure.bin", "EnableInputN > 0");
	pF = new F(&m_PG_Deposition, SOILSURFACE, MANURE, NOFPROC, FORCING_BIOLOGY, NORMAL);
	Define(pF, "N Deposition File", "NDeposition.bin", "N Deposition > 1");
	pFCSV = new FCSV(&m_FertTableStream, ATMBOUNDARY, NITROGEN, NOFPROC, FORCING_BIOLOGY, NORMAL);
	Define(pFCSV, "FertTable File", "BeckumManagement_xxx.csv", "Read Values from Dynamic indexed csv-file>0|N Fertilization =6");
	m_pFertTableStream = pFCSV;

	return true;
}
