#include "NC_GasFlux_Functions.h"
#include "../PLANT_NC/PlantOrgModel.h"
#include "../SOIL/SoilModel.h"
#include "../SOIL/Soil_Frost.h"
#include "../PLANT/PlantModel.h"
#include "../PLANT/Salt.h"


NC_GasFlux_Functions::NC_GasFlux_Functions(void)
{


}
NC_GasFlux_Functions::~NC_GasFlux_Functions(void)
{
}


bool NC_GasFlux_Functions::Def()
{
	PLOT_PARAM plotpar;
	plotpar.Num_Of_Curves = 1;
	plotpar.Num_Of_Points = 300;
	plotpar.Num_Of_X = 1;
	plotpar.X_Min = 0;
	plotpar.X_Max = 10;
	plotpar.X_Label = "Independent(g/m2)";
	plotpar.Y_Label = "Dependent (g/m2/day)";
	plotpar.LogXScale = false;
	plotpar.LogYScale = false;
	plotpar.Num_Of_Opt = 1;
	
	Func* pFunc;
	string funcname;
	FunctorProp functorprop;

	vector<Ps*> ps_vec;

	NC_Organic::Def();
	ModuleNo=32;
	SoilModel* p_Soil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	Soil_Frost* pSoil=&p_Soil->m_SoilFrost;
	NE *pNL=m_pModelMap->GetNEPointer("Number of Layers");
	PlantModel* pPlant=(PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");

	Salt* p_Plant=&pPlant->m_WaterUptake;
	NE *pNumPlants=m_pModelMap->GetNEPointer("Number of Plants");
	Sw* pSw; Ps *pPs; P *pP; X *pX; T *pT; G *pG; Xs *pXs; Ts *pTs; Gs *pGs;

	pSw= new Sw(&Trace_Gas_Emissions ,  SIMPROFILE,AIR, NOFPROC,INITB, NORMAL);
		Define(pSw,"Trace Gas Emissions",0,"off|Direct Loss|Vertical fluxes in soil","Nitrogen and Carbon > 0", 3101);
	pSw= new Sw(&CO2_SoilModel,  SIMPROFILE,AIR, NOFPROC,SOIL_GAS_DYNAMICS, NORMAL);
		Define(pSw, "CO2 SoilModel",0,"Off|Steady state|Dynamic","Trace Gas Emissions >0");   //wenxin Added off by pej
	
	
	pSw= new Sw(&Oxygen_Model,  SIMPROFILE,AIR, NOFPROC,SOIL_GAS_DYNAMICS, NORMAL);
		Define(pSw, "Oxygen Model",0,"Steady state|Dynamic","Trace Gas Emissions >0");

	pSw = new Sw(&COS_Model, SIMPROFILE, AIR, NOFPROC, SOIL_GAS_DYNAMICS, NORMAL);
	Define(pSw, "COS Model", 0, "Off|Only Plant|Both Plant and Soil", "Nitrogen and Carbon > 0");
	
	pSw = new Sw(&COS_PlantModel, SIMPROFILE, AIR, NOFPROC, SOIL_GAS_DYNAMICS, NORMAL);
	Define(pSw, "COS PlantModel", 0, "Off|Tranpiration Base|PhotoSynthesis Base|Experimental", "Nitrogen and Carbon > 0|COS Model>0");

	pSw = new Sw(&COS_SoilModel, SIMPROFILE, AIR, NOFPROC, SOIL_GAS_DYNAMICS, NORMAL);
	Define(pSw, "COS SoilModel", 0, "Off|Heteorotrophic Respiration Base", "Nitrogen and Carbon > 0|COS Model>1");

	pGs = new Gs(&COS_TotPlantUptake, ALLPLANTS, COS, NOFPROC, COS_TRANSPORT, NORMAL);
	Define(pGs, "COS PlantUptake", 0.0,UNIT_TYPES::MIKRO_MASS_FLOW_UNIT, "Nitrogen and Carbon > 0|COS Model>0");
	pGs = new Gs(&COS_TotSoilUptake, TOTPROFILE, COS, NOFPROC, COS_TRANSPORT, NORMAL);
	Define(pGs, "COS SoilUptake", 0.0,UNIT_TYPES::MIKRO_MASS_FLOW_UNIT, "Nitrogen and Carbon > 0|COS Model>1");
	pGs = new Gs(&COS_EcoSystemFlux, TOTPROFILE, COS, NOFPROC, COS_TRANSPORT, NORMAL);
	Define(pGs, "COS EcoSystemFlux", 0.0,UNIT_TYPES::MIKRO_MASS_FLOW_UNIT, "Nitrogen and Carbon > 0|COS Model>1");

	pGs = new Gs(&COS_Option1, TOTPROFILE, COS, NOFPROC, COS_TRANSPORT, NORMAL);
	Define(pGs, "COS Opt1 Transp", 0.0,UNIT_TYPES::MIKRO_MASS_FLOW_UNIT, "Nitrogen and Carbon > 0|COS Model>1");

	pGs = new Gs(&COS_Option2, TOTPROFILE, COS, NOFPROC, COS_TRANSPORT, NORMAL);
	Define(pGs, "COS Opt2 PotTransp", 0.0,UNIT_TYPES::MIKRO_MASS_FLOW_UNIT, "Nitrogen and Carbon > 0|COS Model>1");
	pGs = new Gs(&COS_Option3, TOTPROFILE, COS, NOFPROC, COS_TRANSPORT, NORMAL);
	Define(pGs, "COS Opt3 MaxPotTransp", 0.0,UNIT_TYPES::MIKRO_MASS_FLOW_UNIT, "Nitrogen and Carbon > 0|COS Model>1");


	pGs = new Gs(&COS_Option4, TOTPROFILE, COS, NOFPROC, COS_TRANSPORT, NORMAL);
	Define(pGs, "COS Opt4 CanRes", 0.0,UNIT_TYPES::MIKRO_MASS_FLOW_UNIT, "Nitrogen and Carbon > 0|COS Model>1");

	pGs = new Gs(&COS_Option5, TOTPROFILE, COS, NOFPROC, COS_TRANSPORT, NORMAL);
	Define(pGs, "COS Opt5 AirRes", 0.0,UNIT_TYPES::MIKRO_MASS_FLOW_UNIT, "Nitrogen and Carbon > 0|COS Model>1");
	pGs = new Gs(&COS_Option6, TOTPROFILE, COS, NOFPROC, COS_TRANSPORT, NORMAL);
	Define(pGs, "COS Opt6 GlobalRad", 0.0,UNIT_TYPES::MIKRO_MASS_FLOW_UNIT, "Nitrogen and Carbon > 0|COS Model>1");


	pSw= new Sw(&Methane_Model,  SIMPROFILE,AIR, NOFPROC,SOIL_GAS_DYNAMICS, NORMAL);
	    Define(pSw, "Methane Model",0,"off|Simplified|Detailed","Trace Gas Emissions >0");
	pSw= new Sw(&Methane_Plant_Oxidation ,  SIMPROFILE,AIR, NOFPROC, METHANE_OXIDATION, NORMAL);
	    Define(pSw,"Methane oxidation by plants",0,"off|on","Nitrogen and Carbon > 0|Methane Model>0");
	pSw = new Sw(&Methane_Soil_Oxidation, SIMPROFILE, AIR, NOFPROC, METHANE_OXIDATION, NORMAL);
		Define(pSw, "Methane Soil oxidation", 1, "off|on original|New", "Nitrogen and Carbon > 0|Methane Model>0");
	pSw= new Sw(&Methane_Plant_Emission ,  SIMPROFILE,AIR, NOFPROC,METHANE_PRODUCTION, NORMAL);
	    Define(pSw,"Methane emission by plants",0,"off|on","Nitrogen and Carbon > 0|Methane Model>0");

	pSw = new Sw(&Methane_SoilBoundary_Flux, SIMPROFILE, AIR, NOFPROC, METHANE_PRODUCTION, NORMAL);
		Define(pSw, "Methane SoilBoundary", 0, "Fixed Air Resistance|Dynamic Air Resistance ", "Nitrogen and Carbon > 0|Methane Model>0");

	pSw= new Sw(&N2O_YearlyFluxes ,  SIMPROFILE,AIR, NOFPROC,SOIL_GAS_DYNAMICS, NORMAL);
	    Define(pSw,"N2O Yearly Flux Statistics",0,"off|on","Nitrogen and Carbon > 0|Trace Gas Emissions >0");

	vector<double> initv;
	pX= new X(&CH4,pNL,SIMPROFILE,METHANE, STORAGE,SOIL_GAS_DYNAMICS, NORMAL);
	initv.assign(pSoil->NL,0.);
    Define(pX, pNL  , "Methane content",initv,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions >= 1|Methane Model>0");


////State variables added by Marco
	pXs= new Xs(&CH4_AccBubbles,ACCUMULATOR,METHANE, STORAGE,METHANE_PRODUCTION, NORMAL);
		Define(pXs,   "Acc CH4 Bubble Emission",0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon >= 1|Methane Model>0");
	pXs= new Xs(&CH4_AccPlant, ACCUMULATOR,METHANE, STORAGE,METHANE_PRODUCTION, NORMAL);
		Define(pXs,   "Acc CH4 Plant Emission",0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon >= 1|Methane Model>0") ;
	pXs= new Xs(&CH4_AccEmission,ACCUMULATOR,METHANE, STORAGE,METHANE_TRANSPORT, NORMAL);
	    Define(pXs,  "Acc CH4 Emission",0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon >= 1|Methane Model>0") ;
	pXs= new Xs(&CH4_DiffTotStorage, ACCUMULATOR,METHANE, STORAGE,NOBPROC, NORMAL);
	    Define(pXs,   "Diff CH4 Storage",0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon >= 1|Methane Model>0") ;
	pXs= new Xs(&CH4_MassBalanceCheck,ACCUMULATOR,METHANE, STORAGE,NOBPROC, NORMAL);
	    Define(pXs,   "CH4 MassbalanceCheck",0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon >= 1|Methane Model>0") ;
	pXs= new Xs(& CH4_AccProduction,ACCUMULATOR,METHANE, STORAGE,METHANE_PRODUCTION, NORMAL);
	    Define(pXs,  "Acc CH4 Production",0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon >= 1|Methane Model>0") ;
	pXs= new Xs(&CH4_AccOxidation, ACCUMULATOR,METHANE, STORAGE,METHANE_OXIDATION, NORMAL);
	    Define(pXs,   "Acc CH4 Oxidation",0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon >= 1|Methane Model>0") ;
	initv.assign(pSoil->NL,0.);
	pT= new T(&CH4_Prod ,pNL,SIMPROFILE,METHANE, STORAGE,METHANE_PRODUCTION, NORMAL); 
		Define(pT, pNL  , "CH4 Source flux",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions >= 1|Methane Model>0");
	pT= new T(&CH4_Oxidation,pNL,SIMPROFILE,METHANE, STORAGE,METHANE_OXIDATION, NORMAL); 
	    Define(pT, pNL  , "CH4 Sink flux",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions >= 1|Methane Model>0");
	pT= new T(&CH4_SoilFlux,pNL,SIMPROFILE,METHANE, STORAGE,METHANE_TRANSPORT, NORMAL); 
		Define(pT, pNL  , "CH4 soil water flux",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions >= 1|Methane Model>0");
	;
////Vectors added by Marco
	pT= new T(&CH4_OxidationSoilBacteria ,pNL,SIMPROFILE,METHANE, STORAGE,METHANE_OXIDATION, NORMAL); 
		Define(pT, pNL  , "CH4 aerobic oxidation",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions >= 1|Methane Model>0");
	pT= new T(&CH4_OxidationPlantRoot ,pNL,SIMPROFILE,METHANE, STORAGE,METHANE_OXIDATION, NORMAL); 	
		Define(pT, pNL  , "CH4 plant oxidation",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions >= 1|Methane Model>0");
 ;
	pT= new T(&CH4_Ebullition ,pNL,SIMPROFILE,METHANE, EMISSION,METHANE_PRODUCTION, NORMAL); 
		Define(pT, pNL  , "CH4 ebullition rate",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions >= 1|Methane Model>0");
	pT= new T(&CH4_PlantDiffusion ,pNL,SIMPROFILE,METHANE, STORAGE,METHANE_OXIDATION, NORMAL); 
		Define(pT, pNL  , "CH4 plant diffusion",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions >= 1|Methane Model>0");
	pT= new T(&CH4_SoilEmission ,pNL,SIMPROFILE,METHANE, STORAGE,METHANE_TRANSPORT, NORMAL); 
		Define(pT, pNL  , "CH4 soil emission",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions >= 1|Methane Model>0");
	pTs= new Ts(&CH4_SurfaceEmission,SOILSURFACE,METHANE, STORAGE, METHANE_TRANSPORT, NORMAL);
	    Define(pTs,   "CH4 Surface Emission",0.,UNIT_TYPES::MASSFLOW_UNIT,"Nitrogen and Carbon >= 1|Methane Model>0");
	pT= new T(&CH4_AirSoilFlux ,pNL,SIMPROFILE,METHANE, STORAGE, METHANE_TRANSPORT, NORMAL);
	    Define(pT, pNL  , "CH4 soil air flux",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions >= 1|Methane Model>0");
  ;
// CH4 Parameters;
  functorprop.FuncName = MyFunc::METHANE_PRODUCTION_FROM_ONE_LITTER; functorprop.Arg = MyArgType::D2;
  functorprop.FunctorInitOption = string::npos;
  funcname = "Anaerobic Methane Production Function";
  plotpar.X_Label = "Amount Litter1 (g/m2)";
  plotpar.X_Min = 0;
  plotpar.X_Max = 10.;
  plotpar.Y_Label = "Potential Methane Production (";
  string unit = p_ModelInfo->GetUnitString(UNIT_TYPES::mMASSFLOW_UNIT) + ")"; plotpar.Y_Label += unit;

  plotpar.Num_Of_X = 2;
  plotpar.Num_Of_Opt = 1;
  plotpar.X2_Vec = { 0. };
  plotpar.Y_ScalingCoef = 3;
  plotpar.LegendsVec = { "Only Litter1"};


	pPs= new Ps(&CH4_RateCoefRatio,SIMPROFILE,METHANE, STORAGE,METHANE_PRODUCTION, NORMAL); 
	    Define(pPs,  "CH4 RateCoefRatio",.3, UNIT_TYPES::RATECOEF_UNIT,"Nitrogen and Carbon >=1|Methane Model>0",0.,50.); ps_vec.push_back(pPs);
    pFunc = new Func(TOTPROFILE, METHANE, NOFPROC, METHANE_PRODUCTION, NORMAL);
	ps_vec.push_back((Ps*)m_pModelMap->GetPsPointer("RateCoefLitter1"));
	ps_vec.push_back((Ps*)m_pModelMap->GetPsPointer("RateCoefLitter2"));
	ps_vec.push_back((Ps*)m_pModelMap->GetPsPointer("RateCoefHumus"));
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();

;
functorprop.FuncName = MyFunc::METHANE_PLANTOXIDATION_OF_WATERUPTAKE; functorprop.Arg = MyArgType::D1;
functorprop.FunctorInitOption = string::npos;
funcname = "Oxidation of Methane from Water Uptake Function";
plotpar.X_Label = "Methane uptake by waterflow (";
 unit = p_ModelInfo->GetUnitString(UNIT_TYPES::MIKRO_MASS_FLOW_UNIT)+")";
plotpar.X_Label += unit;
plotpar.X_Min = 0;
plotpar.X_Max = 1.E-4;
plotpar.Y_Label = "Plant Oxidation (";
plotpar.Y_Label += unit;
plotpar.Num_Of_X = 1;
plotpar.Num_Of_Opt = 0;
plotpar.X2_Vec = { };
plotpar.LegendsVec = { "" };
plotpar.X_ScalingCoef = 6;
plotpar.Y_ScalingCoef = 6;

	pPs= new Ps(&CH4_PlantOxidationEff ,SIMPROFILE,METHANE, STORAGE,METHANE_OXIDATION, NORMAL); 
	    Define(pPs,  "CH4 PlantOxidationEff",1.0, UNIT_TYPES::NO_UNIT,"Nitrogen and Carbon >=1|Methane Model>0|PlanType>0",0.,1.);	 ps_vec.push_back(pPs);

	pFunc = new Func(TOTPROFILE, METHANE, NOFPROC, METHANE_OXIDATION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();
	
	ps_vec.clear();
	functorprop.FuncName = MyFunc::METHANE_OXIDATION; functorprop.Arg = MyArgType::D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Maximum Oxidation Rate Function";
	plotpar.X_Label = "Amount of Methane (g/m2)";
	plotpar.X_Min = 0;
	plotpar.X_Max = 1.;
	plotpar.Y_Label = "Oxidation Rate (g/m2/day)";
	plotpar.Num_Of_X = 1;
	plotpar.Num_Of_Opt = 0;
	plotpar.X2_Vec = { };
	plotpar.LegendsVec = {""};
	plotpar.X_ScalingCoef = 0;
	plotpar.Y_ScalingCoef = 0;
		
	pPs= new Ps(&CH4_OxidationReactionRate,SIMPROFILE,METHANE, STORAGE,METHANE_OXIDATION, NORMAL); 
	Define(pPs, "CH4 AerobicOxRate", 0.1, UNIT_TYPES::RATECOEF_UNIT, "Nitrogen and Carbon >=1|Methane Model>0", 0., 50., 0., 1.); ps_vec.push_back(pPs);
	
	pFunc = new Func(TOTPROFILE, METHANE, NOFPROC, METHANE_OXIDATION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();


	functorprop.FuncName = MyFunc::METHANE_OXIDATION_REGULATION_BY_CH4; functorprop.Arg = MyArgType::D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Oxidation Regulation by CH4 concentration";
	plotpar.X_Label = "Methane soil Air (";
	unit = p_ModelInfo->GetUnitString(UNIT_TYPES::MIKRO_MASS_RATIO); unit += ")"; plotpar.X_Label += unit;
	plotpar.X_Min = 0;
	plotpar.X_Max = 1.E-6;
	plotpar.X_ScalingCoef = 6;
	plotpar.Y_Label = "Methane Oxidation(-)";




	pPs= new Ps(&CH4_HalfRateOxConstant,SIMPROFILE,METHANE, STORAGE,METHANE_OXIDATION, NORMAL); 
	Define(pPs,   "CH4 HalfRateOxConstant",2.3E-07, UNIT_TYPES::MASS_RATIO_UNIT,"Nitrogen and Carbon >=1|Methane Model>0",0.,10.,0.,1.E-5);	ps_vec.push_back(pPs);
	
	pFunc = new Func(TOTPROFILE, METHANE, NOFPROC, METHANE_OXIDATION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();



	functorprop.FuncName = MyFunc::METHANE_EBULLITION; functorprop.Arg = MyArgType::D1;
		funcname = "Methane Ebullition Rate Production";
		plotpar.X_Label = "Methane Water Concentration ("; unit = p_ModelInfo->GetUnitString(UNIT_TYPES::MIKRO_MASS_RATIO); unit += ")"; plotpar.X_Label += unit;
		plotpar.X_Min = 0;
		plotpar.X_Max = 1.E-5;
		plotpar.X_ScalingCoef = 6;
		plotpar.Y_ScalingCoef = 6;
		plotpar.Y_Label = "Methane Ebullition (";  unit = p_ModelInfo->GetUnitString(UNIT_TYPES::MIKRO_MASS_RATE); unit += ")"; plotpar.Y_Label += unit;
	
	pPs = new Ps(&CH4_BubbleRateConstant, SIMPROFILE, METHANE, STORAGE, METHANE_PRODUCTION, NORMAL);
		Define(pPs, "CH4 BubbleRateConstant", 2.8, "-", "Nitrogen and Carbon >=1|Methane Model>0", 0., 4.);	ps_vec.push_back(pPs);
	pPs= new Ps(&CH4_EbullMax,SIMPROFILE,METHANE, STORAGE,METHANE_PRODUCTION, NORMAL); 
		Define(pPs, "CH4 EbullThreshold", 5.E-06, "-", "Nitrogen and Carbon >=1|Methane Model>0", 0., 1.E-02);	ps_vec.push_back(pPs);
	
	pFunc = new Func(TOTPROFILE, METHANE, NOFPROC, METHANE_OXIDATION, NORMAL);
		Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();
	
		


;
functorprop.FuncName = MyFunc::METHANE_PLANTOXIDATION_REGULATION_BY_ROOTRESPIRATION; functorprop.Arg = MyArgType::D1;
functorprop.FunctorInitOption = string::npos;
funcname = "Plant Oxidation Regulation by root respiration";
plotpar.X_Label = "Root Respiration Rate (gC/m3/day)";
plotpar.X_Min = 0;
plotpar.X_Max = 1.;
plotpar.X_ScalingCoef = 0;
plotpar.Y_ScalingCoef = 0;
plotpar.Y_Label = "Relative Rate (-)";


	pPs= new Ps(&CH4_PlantOxShape,SIMPROFILE,METHANE, STORAGE,METHANE_OXIDATION, NORMAL); 
	    Define(pPs,   "CH4 PlantOxShapeCoef",34., UNIT_TYPES::INVERSE_SQUARED_FLUX_PER_VOLUME,"Nitrogen and Carbon >=1|Methane Model>0|PlantType>0",0.,200.);  ps_vec.push_back(pPs);

	pFunc = new Func(TOTPROFILE, METHANE, NOFPROC, METHANE_OXIDATION, NORMAL);
		Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();

	pPs = new Ps(&CH4_Water_Diff_Coef, SIMPROFILE, METHANE, STORAGE, METHANE_TRANSPORT, NORMAL);
		Define(pPs, "CH4 WaterDiffCoef", 1.29E-4, "m2/s", "Nitrogen and Carbon >= 1|Trace Gas Emissions >= 1|Methane Model>0", 1.E-5, 1.E-3);
	
	
	functorprop.FuncName = MyFunc::METHANE_PLANTOXIDATION_BY_ROOTRESPIRATION; functorprop.Arg = MyArgType::D1;
		functorprop.FunctorInitOption = string::npos;
		funcname = "Plant Oxidation Regulation by root respiration";
		plotpar.X_Label = "Root Respiration Rate (gC/m3/day)";
		plotpar.X_Min = 0;
		plotpar.X_Max = 1.;
		plotpar.Y_ScalingCoef = 3;
		plotpar.Y_Label = "Methane_Oxidation Rate (mg/m3/day)";

	pPs = new Ps(&CH4_PlantOxidationRate, SIMPROFILE, METHANE, STORAGE, METHANE_OXIDATION, NORMAL);
		Define(pPs, "CH4 PlantOxidationRate", 4.2E-05, UNIT_TYPES::MASS_RATIO_UNIT, "Nitrogen and Carbon >=1|Methane Model>0|PlantType>0", 0.,10., 0., 1.E-3); ps_vec.push_back(pPs);
		ps_vec.push_back((Ps*)m_pModelMap->GetPsPointer("CH4 PlantOxShapeCoef"));
	
	pFunc = new Func(TOTPROFILE, METHANE, NOFPROC, METHANE_OXIDATION, NORMAL);
		Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();




	
	functorprop.FuncName = MyFunc::METHANE_VOLATIVITY; functorprop.Arg = MyArgType::D2;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Methane Volatility Function";
	plotpar.X_Label = "Water Concentration (";
	
	auto k= p_ModelInfo->GetUnitString(UNIT_TYPES::MIKRO_MASS_RATIO);
	k += ")";
	plotpar.X_Label += k;	  ;
	plotpar.Y_Label = "Air Concentration (";
	plotpar.Y_Label += k;
	plotpar.X_Min = 0;
	plotpar.X_Max = 1.E-4;
	plotpar.Num_Of_Curves = 3;
	plotpar.Num_Of_X = 2;
	plotpar.Num_Of_Opt = 3;
	plotpar.X_ScalingCoef = 6;
	plotpar.Y_ScalingCoef = 6;
	plotpar.X2_Vec = { 0, 10.,20. };
	plotpar.LegendsVec.clear();
	string lstr =p_ModelInfo->GetUnitString(UNIT_TYPES::TEMP_UNIT);
	plotpar.LegendsVec.push_back("0 " + lstr);  plotpar.LegendsVec.push_back("10 " + lstr); plotpar.LegendsVec.push_back("20 " + lstr);
	
		
	pPs= new Ps(&CH4_HStandard,SIMPROFILE,METHANE, STORAGE, SOIL_GAS_EQUILIBRIUM, NORMAL);
	    Define(pPs,   "HenryStdCoef_cp",0.0013,"Mol/atm","Nitrogen and Carbon >=1|Methane Model>0",0.001,0.1);	 ps_vec.push_back(pPs);
	pPs= new Ps(&CH4_HTempSens,SIMPROFILE,METHANE, STORAGE, SOIL_GAS_EQUILIBRIUM, NORMAL);
	    Define(pPs,   "HenryCoefTempSens",-1700.,"K","Nitrogen and Carbon >=1|Methane Model>0",-10000.,10000.);	 ps_vec.push_back(pPs);
	pPs= new Ps(&CH4_H_TRef,SIMPROFILE,METHANE, STORAGE, SOIL_GAS_EQUILIBRIUM, NORMAL);
	    Define(pPs,   "H_RefTemp",25., UNIT_TYPES::TEMP_UNIT,"Nitrogen and Carbon >=1|Methane Model>0",-50.,50.);	 ps_vec.push_back(pPs);

	pFunc = new Func(TOTPROFILE, METHANE, NOFPROC, SOIL_GAS_EQUILIBRIUM, NORMAL);
		Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();
	
	functorprop.FuncName = MyFunc::METHANE_TRANSPIRATION_EFFICIENCY; functorprop.Arg = MyArgType::D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Transpiration Methane Emission Function";
		plotpar.X_Label = "Water Uptake Rate (mm/day)";
		plotpar.Y_Label = "Methane Emission Efficiency ()";
		plotpar.X_Min = 0;
		plotpar.X_Max = 8;
		plotpar.X_ScalingCoef = 0;
		plotpar.Y_ScalingCoef = 0;

		plotpar.Num_Of_Curves = 1;

		plotpar.Num_Of_X = 1;
		plotpar.Num_Of_Opt =0;
		plotpar.X2_Vec = { };
		plotpar.LegendsVec = {""};



	pPs= new Ps(&CH4_LimitWaterUptake,SIMPROFILE,METHANE, STORAGE,METHANE_OXIDATION, NORMAL); 
	    Define(pPs,   "CH4 LimitWaterUptake",4.,UNIT_TYPES::MASSFLOW_UNIT,"Nitrogen and Carbon >=1|Methane Model>0|PlantType>0",0.,10.); ps_vec.push_back(pPs);
	pPs= new Ps(&CH4_WaterPowerCoef,SIMPROFILE,METHANE, STORAGE,METHANE_OXIDATION, NORMAL); 
	    Define(pPs,   "CH4 Water Power Coef",7.,UNIT_TYPES::NO_UNIT,"Nitrogen and Carbon >=1|Methane Model>0|PlantType>0",0,10.); ps_vec.push_back(pPs);
	pPs= new Ps(&CH4_WaterPlantCoef,SIMPROFILE,METHANE, STORAGE,METHANE_OXIDATION, NORMAL); 
	    Define(pPs,  "CH4 Water Plant Coef",.66, UNIT_TYPES::MASS_RATIO_UNIT,"Nitrogen and Carbon >=1|Methane Model>0|PlantType>0",0.,1.); ps_vec.push_back(pPs);  //Changed default from 66 % to .66	  March 2 2020

	pFunc = new Func(TOTPROFILE, METHANE, NOFPROC, METHANE_PRODUCTION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();



	functorprop.FuncName = MyFunc::METHANE_BOUNDARYLAYER_FLUX; functorprop.Arg = MyArgType::D1;
	functorprop.FunctorInitOption = string::npos;
		funcname = "Methane Soil Surface Flux";
		plotpar.X_Label = "Air Surface Concentration (";
		unit = p_ModelInfo->GetUnitString(UNIT_TYPES::MIKRO_MASS_RATIO); unit += ")"; plotpar.X_Label += unit;
		plotpar.Y_Label = "Methane Flux (";	 unit = p_ModelInfo->GetUnitString(UNIT_TYPES::mMASSFLOW_UNIT); unit += ")"; plotpar.Y_Label += unit;
		plotpar.X_Min = 1.E-7;
		plotpar.X_Max = 5.E-6;

		plotpar.Num_Of_X = 1;
		plotpar.Num_Of_Opt = 0;
		plotpar.X2_Vec = {};
		plotpar.LegendsVec = {};
		plotpar.X_ScalingCoef = 6;
		plotpar.Y_ScalingCoef = 3;

	pPs = new Ps(&CH4_CONC_Atm, SIMPROFILE, METHANE, STORAGE, METHANE_TRANSPORT, NORMAL);
		Define(pPs, "CH4 Air Conc", 1.8E-06, "g/g", "Nitrogen and Carbon >= 1|Trace Gas Emissions >= 1|Methane Model>0", 1.E-09, 1.E-03);	ps_vec.push_back(pPs);
	pPs = new Ps(&CH4_ResistanceAir, SIMPROFILE, METHANE, STORAGE, METHANE_TRANSPORT, NORMAL);
		Define(pPs, "CH4 Resistance Air", 30., "-", "Nitrogen and Carbon >=1|Methane Model>0|Methane SoilBoundary=0", 1., 500.); ps_vec.push_back(pPs);

	
	pFunc = new Func(TOTPROFILE, METHANE, NOFPROC, METHANE_PRODUCTION, NORMAL);
		Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();

	pPs = new Ps(&CH4_ResistanceAirLogScale, SIMPROFILE, METHANE, STORAGE, METHANE_TRANSPORT, NORMAL);
		Define(pPs, "CH4 AirResistance LogScale", 0., "-", "Nitrogen and Carbon >=1|Methane Model>0|Methane SoilBoundary=1", -2., 2.); 


	pPs = new Ps(&CO2_CONC_Atm, SIMPROFILE, METHANE, STORAGE, INITB, NORMAL);
		Define(pPs, "CO2 Air Conc", 4.E-04, "g/g", "Nitrogen and Carbon >= 1|Trace Gas Emissions >= 1|Soil CO2Model>0", 2.E-04, 7.E-04);
		 //wenxin
	pPs= new Ps(&CH4_InitConc,SIMPROFILE,METHANE, STORAGE, METHANE_TRANSPORT, NORMAL);
	    Define(pPs,   "CH4 init Conc",2.E-4,"gC/g", "Nitrogen and Carbon >= 1|Trace Gas Emissions >= 1|Methane Model>0",1.E-09,1.E-03);


	pPs = new Ps(&COS_TranspirationRatio, ALLPLANTS, COS, NOFPROC, COS_TRANSPORT, NORMAL);
	  Define(pPs, "COS Transpiration Ratio", 15., UNIT_TYPES::MIKRO_MASS_RATIO, "Nitrogen and Carbon >= 1|COS PlantModel >= 1", 1.E-03, 1.E3);
	pPs = new Ps(&COS_TranspirationPotRatio, ALLPLANTS, COS, NOFPROC, COS_TRANSPORT, NORMAL);
	  Define(pPs, "COS Pot Transp Ratio", 10., UNIT_TYPES::MIKRO_MASS_RATIO, "Nitrogen and Carbon >= 1|COS PlantModel >= 1", 1.E-03, 1.E3);
	pPs = new Ps(&COS_TranspirationPotMaxRatio, ALLPLANTS, COS, NOFPROC, COS_TRANSPORT, NORMAL);
	  Define(pPs, "COS Pot MaxTransp Ratio", 4., UNIT_TYPES::MIKRO_MASS_RATIO, "Nitrogen and Carbon >= 1|COS PlantModel >= 1", 1.E-03, 1.E3);
  
  pPs = new Ps(&COS_CanopyResistCoef, ALLPLANTS, COS, NOFPROC, COS_TRANSPORT, NORMAL);
	  Define(pPs, "COS CanopyResistCoef", 8.E4, UNIT_TYPES::MIKRO_MASS_RATIO, "Nitrogen and Carbon >= 1|COS PlantModel >= 1", 1.E-03, 1.E6);
  pPs = new Ps(&COS_AirResistCoef, ALLPLANTS, COS, NOFPROC, COS_TRANSPORT, NORMAL);
	  Define(pPs, "COS AirResistCoef", 4800., UNIT_TYPES::MIKRO_MASS_RATIO, "Nitrogen and Carbon >= 1|COS PlantModel >= 1", 1.E-03, 1.E3);
  pPs = new Ps(&COS_HalfCondCanopy, ALLPLANTS, COS, NOFPROC, COS_TRANSPORT, NORMAL);
	  Define(pPs, "COS HalfCondCanopy", 30., UNIT_TYPES::WIND_SPEED_UNIT, "Nitrogen and Carbon >= 1|COS PlantModel >= 1", 1., 1.E3);
  pPs = new Ps(&COS_HalfCondAir, ALLPLANTS, COS, NOFPROC, COS_TRANSPORT, NORMAL);
	  Define(pPs, "COS HalfCondAir", 10., UNIT_TYPES::WIND_SPEED_UNIT, "Nitrogen and Carbon >= 1|COS PlantModel >= 1", 1., 1.E3);

	pPs = new Ps(&COS_RadHalfCoef, ALLPLANTS, COS, NOFPROC, COS_TRANSPORT, NORMAL);
	  Define(pPs, "COS RadHalfCoef", 5.E7,UNIT_TYPES::HEATFLOW_UNIT , "Nitrogen and Carbon >= 1|COS PlantModel >= 1", 1.E3, 1.E9);
  pPs = new Ps(&COS_RadScaleCoef, ALLPLANTS, COS, NOFPROC, COS_TRANSPORT, NORMAL);
	  Define(pPs, "COS RadScaleCoef", 100.,UNIT_TYPES::MIKRO_MASS_FLOW_UNIT, "Nitrogen and Carbon >= 1|COS PlantModel >= 1", 1., 1.E3);

	  
	  pPs = new Ps(&COS_PhotoSynthesisRatio, ALLPLANTS, COS, NOFPROC, COS_TRANSPORT, NORMAL);
	  Define(pPs, "COS Photosynthesis Ratio", 3., UNIT_TYPES::MIKRO_MASS_RATIO, "Nitrogen and Carbon >= 1|COS PlantModel == 2", 1.E-03, 1.E3);
	pPs = new Ps(&COS_SoilRespiratonRatio, TOTPROFILE, COS, NOFPROC, COS_TRANSPORT, NORMAL);
	  Define(pPs, "COS Soil Respiration Ratio", 1.6, UNIT_TYPES::MIKRO_MASS_RATIO, "Nitrogen and Carbon >= 1|COS Model >= 1", 1.E-03, 1.E3);
;
;	initv.assign(pSoil->NL,0.);
	pG= new G(&CH4_CONC_AIR,pNL,SIMPROFILE,METHANE, STORAGE,METHANE_TRANSPORT, NORMAL); 
	    Define(pG, pNL,   "CH4 air concentration",initv,"-", "Nitrogen and Carbon >= 1|Trace Gas Emissions > 0");
;
;
	pGs= new Gs(&CH4_TotSoilFlux,SIMPROFILE,METHANE, STORAGE,METHANE_TRANSPORT, NORMAL); 
	    Define(pGs,     "CH4 total soil flux ", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Methane Model>0")  ;
	pGs= new Gs(&CH4_TotPlantUptake,SIMPROFILE,METHANE, STORAGE,METHANE_OXIDATION, NORMAL); 
	    Define(pGs,     "CH4 total Plant Uptake", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Methane Model>0")  ;
	pGs= new Gs(&CH4_TotPlantEmission,SIMPROFILE,METHANE, STORAGE,METHANE_PRODUCTION, NORMAL); 
	    Define(pGs,    "CH4 total Plant Emission", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Methane Model>0")  ;
	pGs= new Gs(&CH4_TotPlantOxidation,SIMPROFILE,METHANE, STORAGE,METHANE_OXIDATION, NORMAL); 
	    Define(pGs,     "CH4 total Plant Oxidation", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Methane Model>0")  ;
	pGs = new Gs(&CH4_TotSoilOxidation, SIMPROFILE, METHANE, STORAGE, METHANE_OXIDATION, NORMAL);
		Define(pGs, "CH4 total Soil Oxidation", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Methane Model>0");

		
		;
	pGs= new Gs(&N2O_10Percentil, SIMPROFILE,METHANE, STORAGE,OXIDATION, NORMAL); 
	    Define(pGs,     "N2O 10 Percentil",0.0,UNIT_TYPES::MASSFLOW_UNIT,"Nitrogen and Carbon >= 1|Trace Gas Emissions > 0|N2O Yearly Flux Statistics>0");
	pGs= new Gs(&N2O_50Percentil,SIMPROFILE,METHANE, STORAGE,OXIDATION, NORMAL); 
	    Define(pGs,     "N2O 50 Percentil",0.0,UNIT_TYPES::MASSFLOW_UNIT,"Nitrogen and Carbon >= 1|Trace Gas Emissions > 0|N2O Yearly Flux Statistics>0");
	pGs= new Gs(&N2O_90Percentil,SIMPROFILE,METHANE, STORAGE,OXIDATION, NORMAL); 
	    Define(pGs,     "N2O 90 Percentil",0.0,UNIT_TYPES::MASSFLOW_UNIT,"Nitrogen and Carbon >= 1|Trace Gas Emissions > 0|N2O Yearly Flux Statistics>0");
	pGs= new Gs(&N2O_Ymean,SIMPROFILE,METHANE, STORAGE,OXIDATION, NORMAL); 
	    Define(pGs,     "N2O Yearly mean",0.0,UNIT_TYPES::MASSFLOW_UNIT,"Nitrogen and Carbon >= 1|Trace Gas Emissions > 0|N2O Yearly Flux Statistics>0");
    ;
	pGs= new Gs(&NO_10Percentil,SIMPROFILE,METHANE, STORAGE,OXIDATION, NORMAL); 
	    Define(pGs,     "NO 10 Percentil",0.0,UNIT_TYPES::MASSFLOW_UNIT,"Nitrogen and Carbon >= 1|Trace Gas Emissions > 0|N2O Yearly Flux Statistics>0");
	pGs= new Gs(&NO_50Percentil,SIMPROFILE,METHANE, STORAGE,OXIDATION, NORMAL); 
	    Define(pGs,     "NO 50 Percentil",0.0,UNIT_TYPES::MASSFLOW_UNIT,"Nitrogen and Carbon >= 1|Trace Gas Emissions > 0|N2O Yearly Flux Statistics>0");
	pGs= new Gs(&NO_90Percentil,SIMPROFILE,METHANE, STORAGE,OXIDATION, NORMAL); 
	    Define(pGs,     "NO 90 Percentil",0.0,UNIT_TYPES::MASSFLOW_UNIT,"Nitrogen and Carbon >= 1|Trace Gas Emissions > 0|N2O Yearly Flux Statistics>0");
	pGs= new Gs(& NO_Ymean,SIMPROFILE,METHANE, STORAGE,OXIDATION, NORMAL); 
	    Define(pGs,    "NO Yearly mean",0.0,UNIT_TYPES::MASSFLOW_UNIT,"Nitrogen and Carbon >= 1|Trace Gas Emissions > 0|N2O Yearly Flux Statistics>0");
;
 //;
   	pGs= new Gs(&CH4_TotSysFlux, TOTPROFILE,METHANE, STORAGE,METHANE_TRANSPORT, NORMAL); 
	Define(pGs,     "Methane ecosystem flux", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions > 0|Methane Model>0")  ;
;

	pG= new G(&CH4_Conc_Water,pNL,SIMPROFILE,METHANE, STORAGE,METHANE_TRANSPORT, NORMAL); 
	initv.assign(pSoil->NL,0.);
    Define(pG, pNL ,"CH4 water conc",initv,"gC/l", "Nitrogen and Carbon >= 1|Trace Gas Emissions > 0");

; initv.assign(pSoil->NL, .1);
	pX = new X(&CO2Soil, pNL, SIMPROFILE, SOILCO2, STORAGE, RESPIRATION, NORMAL);
		Define(pX, pNL, "CO2Soil", initv, UNIT_TYPES::GAS_MASS_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions > 0|CO2 SoilModel>0");
		//Wenxin SOILCO2 don't showup
;	initv.assign(pSoil->NL,20.);
	pX= new X(&Oxygen,pNL,SIMPROFILE,OXYGEN, STORAGE,OXIDATION, NORMAL); 
		Define(pX, pNL  , "Oxygen",initv,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions > 0");
	pX= new X(&NO,pNL,SIMPROFILE,ANAROBIC_GAS, STORAGE,OXIDATION, NORMAL); 
		Define(pX, pNL  , "Nitric Oxide",initv,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions > 1");
	pX= new X(&N2O,pNL,SIMPROFILE,ANAROBIC_GAS, STORAGE,OXIDATION, NORMAL); 
		Define(pX, pNL  , "Nitrous Oxide",initv,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions > 1");
	pX= new X(&N2 ,pNL,SIMPROFILE,ANAROBIC_GAS, STORAGE,OXIDATION, NORMAL); 
		Define(pX, pNL  , "Nitrogen gas",initv,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions > 1");
;	initv.assign(pSoil->NL,0.);
	pT= new T(& NO_SoilFlux ,pNL,SIMPROFILE,ANAROBIC_GAS, TRANSPORT,OXIDATION, NORMAL); 
		Define(pT, pNL , "Nitric oxide Soil Flux",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions > 1");
	pT= new T(& N2O_SoilFlux,pNL,SIMPROFILE,ANAROBIC_GAS, TRANSPORT,OXIDATION, NORMAL); 
		Define(pT, pNL , "Nitrous oxide Soil Flux",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions > 1");
	pT= new T(&N2_SoilFlux ,pNL,SIMPROFILE,ANAROBIC_GAS, TRANSPORT,OXIDATION, NORMAL); 
		Define(pT, pNL  , "Nitrogen Soil Flux",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions > 1");
;
	pT= new T(&NO_Exchange ,pNL,SIMPROFILE,ANAROBIC_GAS, TRANSPORT,OXIDATION, NORMAL); 
		Define(pT, pNL  , "Nitric oxide internal exchange",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions >= 1");
	pT= new T(&N2O_Exchange ,pNL,SIMPROFILE,ANAROBIC_GAS, TRANSPORT,OXIDATION, NORMAL); 
		Define(pT, pNL  , "Nitrous oxide internal exchange",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions >= 1");
	pT= new T(&N2_Exchange ,pNL,SIMPROFILE,ANAROBIC_GAS, TRANSPORT,OXIDATION, NORMAL); 
		Define(pT, pNL  , "Nitrogen internal exchange",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions >= 1");
;
  	pGs= new Gs(&NO_TotExchange ,SIMPROFILE,ANAROBIC_GAS, STORAGE,OXIDATION, NORMAL); 
		Define(pGs, "N Tot NO internal exchange", 0.,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions >= 1");
  	pGs= new Gs(&N2O_TotExchange,SIMPROFILE,ANAROBIC_GAS, STORAGE,OXIDATION, NORMAL); 
		Define(pGs,     "N Tot N2O internal exchange", 0.,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions >= 1");
  	pGs= new Gs(&N2_TotExchange,SIMPROFILE,ANAROBIC_GAS, STORAGE,OXIDATION, NORMAL); 
		Define(pGs,     "N Tot N2 internal exchange", 0.,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions >= 1");
;
	initv.assign(pSoil->NL,0.);
	pT= new T(&NO_NitriFlux, pNL,SIMPROFILE,ANAROBIC_GAS, TRANSPORT,OXIDATION, NORMAL); 
		Define(pT, pNL   ,"Nitric oxide (Nitrification)",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions >= 1");
	pT= new T(&N2O_NitriFlux ,pNL,SIMPROFILE,ANAROBIC_GAS, TRANSPORT,OXIDATION, NORMAL); 
		Define(pT, pNL  , "Nitrous oxide (Nitrification)",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions >= 1");
;
	pT= new T(&CO2_FluxSoil ,pNL,SIMPROFILE,SOILCO2, TRANSPORT,RESPIRATION, NORMAL); 
		Define(pT, pNL  , "CO2_ FluxSoil",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions > 0");


	pT= new T(&Oxygen_Flux ,pNL,SIMPROFILE,ANAROBIC_GAS, TRANSPORT,OXIDATION, NORMAL); 
		Define(pT, pNL  , "Oxygen Flux",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions > 0");
	pT= new T(&Oxygen_Consumption ,pNL,SIMPROFILE,ANAROBIC_GAS, TRANSPORT,OXIDATION, NORMAL); 
		Define(pT, pNL  , "Oxygen Consumption",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions > 0");
	
	pT = new T(&CO2_Production, pNL, SIMPROFILE, ANAROBIC_GAS, TRANSPORT, OXIDATION, NORMAL);
		Define(pT, pNL, "CO2 production", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions > 0|CO2 SoilModel>0");
	
	pTs= new Ts(&Oxygen_Flux_Infil ,SIMPROFILE,ANAROBIC_GAS, STORAGE,OXIDATION, NORMAL); 
		Define(pTs, "Oxygen Flux Infil", 0.,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions > 0");

	pTs = new Ts(&CO2_Flux_Out, SIMPROFILE, ANAROBIC_GAS, STORAGE, OXIDATION, NORMAL);
		Define(pTs, "CO2 Flux Out", 0.,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions > 0|CO2 SoilModel>0");
;
	pTs= new Ts(&N2O_Rate ,SIMPROFILE,ANAROBIC_GAS, STORAGE,OXIDATION, NORMAL); 
		Define(pTs, "N2O emission rate", 0.,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions > 0");
  	pTs= new Ts(&NO_Rate ,SIMPROFILE,ANAROBIC_GAS, STORAGE,OXIDATION, NORMAL); 
		Define(pTs,  "NO emission rate", 0.,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions > 0");
  	pTs= new Ts(&N2_Rate, SIMPROFILE,ANAROBIC_GAS, STORAGE,OXIDATION, NORMAL); 
		Define(pTs,   "N2 emission rate", 0.,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions > 0");
;
;
// CH4 model above;
; initv.assign(pSoil->NL, .0004);
	pG= new G(&CO2_Conc ,pNL,SIMPROFILE,SOILCO2, TRANSPORT,RESPIRATION, NORMAL); 
		Define(pG, pNL   , "CO2 Concentration",initv, UNIT_TYPES::GAS_CONC_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions > 0");
		//wenxin ppm = volume ratio?
	initv.assign(pSoil->NL, 20.);
	pG= new G(&Oxygen_Conc ,pNL,SIMPROFILE,AIR, TRANSPORT,OXIDATION, NORMAL); 
		Define(pG, pNL   , "Oxygen Concentration",initv,"-", "Nitrogen and Carbon >= 1|Trace Gas Emissions > 0");
	pG= new G(&Air_Porosity ,pNL,SIMPROFILE,AIR, TRANSPORT,OXIDATION, NORMAL); 
		Define(pG, pNL   , "Air Porosity",initv,"%", "Nitrogen and Carbon >= 1|Trace Gas Emissions > 0");
		initv.assign(pSoil->NL, .0);
	pG = new G(&DCO2_SoilCoef, pNL, SIMPROFILE, AIR, TRANSPORT, OXIDATION, NORMAL);
		Define(pG, pNL, "CO2 diffusion coefficient", initv, UNIT_TYPES::AREA_PER_TIME_UNIT, "Nitrogen and Carbon >= 1|Trace Gas Emissions > 0");
		
	initv.assign(pSoil->NL,0.);
	pG= new G(&An_VolFraction ,pNL,SIMPROFILE,ANAROBIC_GAS, TRANSPORT,OXIDATION, NORMAL); 
		Define(pG, pNL  , "An_VolFraction",initv,"-", "Nitrogen and Carbon >= 1|Trace Gas Emissions > 0");

	functorprop.FuncName = MyFunc::ANAEROBIC_BALLON; functorprop.Arg = MyArgType::D1;
		functorprop.FunctorInitOption = string::npos;
		funcname = "Anaerobic Ballon Function";
		plotpar.X_Label = "Oxygen content (";
		unit = p_ModelInfo->GetUnitString(UNIT_TYPES::mMASS_LITER_UNIT)+")";
		plotpar.X_Label += unit;
		plotpar.X_Min = 0;
		plotpar.X_Max = 0.20;
		plotpar.Y_Label = "Fraction of Anaerobic Volume (";
		unit = p_ModelInfo->GetUnitString(UNIT_TYPES::NO_UNIT) + ")";
		plotpar.Y_Label += unit;
		plotpar.Num_Of_X = 1;
		plotpar.Num_Of_Opt = 0;
		plotpar.X2_Vec = { };
		plotpar.LegendsVec = {};
		plotpar.X_ScalingCoef = 0;
		plotpar.Y_ScalingCoef = 0;

	pPs= new Ps(&AnBal_ShapeCoef ,SIMPROFILE,ANAROBIC_GAS, STORAGE,OXIDATION, NORMAL); 
		Define(pPs, "AnBal_ShapeCoef", 100., "-", "Trace Gas Emissions > 0", 0., 500.);  ps_vec.push_back(pPs);
	pFunc = new Func(TOTPROFILE, METHANE, NOFPROC, METHANE_PRODUCTION, NORMAL);
		Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();

		functorprop.FuncName = MyFunc::DiFFUSION_SOIL_OXYGEN; functorprop.Arg = MyArgType::D1;
		functorprop.FunctorInitOption = string::npos;
		funcname = "Oxygen Soil Air Diffusion Function";
		plotpar.Y_Label = "Soil Diffusion (";
		unit = p_ModelInfo->GetUnitString(UNIT_TYPES::AREA_PER_TIME_UNIT) + ")";
		plotpar.Y_Label += unit;
		plotpar.X_Min = 0;
		plotpar.X_Max = 100;
		plotpar.X_Label = "Percentage of total Volume (";
		unit = p_ModelInfo->GetUnitString(UNIT_TYPES::NO_UNIT) + ")";
		plotpar.X_Label += unit;
		plotpar.Num_Of_X = 1;
		plotpar.Num_Of_Opt = 0;
		plotpar.X2_Vec = { };
		plotpar.LegendsVec = {};
		plotpar.X_ScalingCoef = 0;
		plotpar.Y_ScalingCoef = 0;


		pPs = new Ps(&DO2_Tortuosity, SIMPROFILE, ANAROBIC_GAS, STORAGE, OXIDATION, NORMAL); ps_vec.push_back(pPs);
		Define(pPs,   "D O2 Tortuosity", 0.66, "-","Trace Gas Emissions > 0",0.3,1.0);

		pFunc = new Func(TOTPROFILE, OXYGEN,DIFFUSION ,SOIL_GAS_DYNAMICS, NORMAL);
		Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();
		functorprop.Arg = MyArgType::D2;
		pFunc = new Func(TOTPROFILE, OXYGEN, DIFFUSION, SOIL_GAS_DYNAMICS, NORMAL);
		Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();

		functorprop.FuncName = MyFunc::DiFFUSION_SOIL_CO2; functorprop.Arg = MyArgType::D1;
		functorprop.FunctorInitOption = string::npos;
		funcname = "CO2 Soil Air Diffusion Function";
		plotpar.Y_Label = "Soil Diffusion (";
		unit = p_ModelInfo->GetUnitString(UNIT_TYPES::AREA_PER_TIME_UNIT) + ")";
		plotpar.Y_Label += unit;
		plotpar.X_Min = 0;
		plotpar.X_Max = 100.;
		plotpar.X_Label = "Fraction of total Volume (";
		unit = p_ModelInfo->GetUnitString(UNIT_TYPES::WATERCONTENT_UNIT) + ")";
		plotpar.X_Label += unit;
		plotpar.Y_ScalingCoef = 0;

	pPs= new Ps(&DCO2_Tortuosity, SIMPROFILE,SOILCO2, TRANSPORT,RESPIRATION, NORMAL); 
		Define(pPs,   "D CO2 Tortuosity", 0.66, "-","Trace Gas Emissions > 0",0.3,1.0);	ps_vec.push_back(pPs);
	pPs = new Ps(&DCO2_PowerCoefSoilDiff, SIMPROFILE, SOILCO2, TRANSPORT, RESPIRATION, NORMAL);
		Define(pPs, "D CO2 PowerCoef", 1., "-", "Trace Gas Emissions > 0", 0.01, 100.0); ps_vec.push_back(pPs);
	pPs = new Ps(&DCO2_AirThreshold, SIMPROFILE, SOILCO2, TRANSPORT, RESPIRATION, NORMAL);
		Define(pPs, "D CO2 AirThreshold", 1., "vol %", "Trace Gas Emissions > 0", 0.01, 100.0); ps_vec.push_back(pPs);
	pPs = new Ps(&DCO2Min_Soil, SIMPROFILE, SOILCO2, TRANSPORT, RESPIRATION, NORMAL);
		Define(pPs, "D CO2 Min Value Soil", 1.E-9, UNIT_TYPES::AREA_PER_TIME_UNIT, "Trace Gas Emissions > 0", 1.E-10, 1.E-6); ps_vec.push_back(pPs);

	pFunc = new Func(TOTPROFILE, OXYGEN, DIFFUSION, SOIL_GAS_DYNAMICS, NORMAL);
		Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();
		functorprop.Arg = MyArgType::D2;
	pFunc = new Func(TOTPROFILE, OXYGEN, DIFFUSION, SOIL_GAS_DYNAMICS, NORMAL);
		Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();




	initv.assign(pSoil->NL,20.);

	functorprop.FuncName = MyFunc::NITRI_N2O_PROD_RESPONSE; functorprop.Arg = MyArgType::D1;
	functorprop.FunctorInitOption = string::npos;
	funcname = "Nitrous Oxide Production from Nitrifiers";
	plotpar.Y_Label = "Nitrous Oxide Production (";
	unit = p_ModelInfo->GetUnitString(UNIT_TYPES::MIKRO_MASS_FLOW_UNIT) + ")";
	plotpar.Y_Label += unit;
	plotpar.X_Min = 0;
	plotpar.X_Max = 1E-3;
	plotpar.X_Label = "Nitrification rate(";
	unit = p_ModelInfo->GetUnitString(UNIT_TYPES::mMASSFLOW_UNIT) + ")";
	plotpar.X_Label += unit;
	plotpar.Y_ScalingCoef = 6;
	plotpar.X_ScalingCoef = 3;

;
	pPs= new Ps(&MaxFracN2O,SIMPROFILE,SOILGAS_N2O, STORAGE,OXIDATION, NORMAL); 
	Define(pPs, "MaxFracN2O", 0.0006, "-", "Trace Gas Emissions > 0", 0., 0.1); ps_vec.push_back(pPs);
	pFunc = new Func(TOTPROFILE, OXYGEN, DIFFUSION, SOIL_GAS_DYNAMICS, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();

	functorprop.FuncName = MyFunc::NITRI_NO_PROD_RESPONSE;
	funcname = "Nitric Oxide Production from Nitrifiers";
	plotpar.Y_Label = "Nitric Oxide Production (";
	unit = p_ModelInfo->GetUnitString(UNIT_TYPES::MIKRO_MASS_FLOW_UNIT) + ")";
	plotpar.Y_Label += unit;
	
	pPs= new Ps(&MaxFracNO,SIMPROFILE,SOILGAS_NO, STORAGE,OXIDATION, NORMAL); 
		Define(pPs,   "MaxFracNO", 0.004, "-","Trace Gas Emissions > 0",0., 0.5); ps_vec.push_back(pPs);
	pFunc = new Func(TOTPROFILE, OXYGEN, DIFFUSION, SOIL_GAS_DYNAMICS, NORMAL);
		Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();
;


functorprop.FuncName = MyFunc::NITRI_NO_PROD_TEMP_RESPONSE; functorprop.Arg = MyArgType::D1;
functorprop.FunctorInitOption = string::npos;
funcname = "N oxide Production From Nitrification Temp Response";
plotpar.Y_Label = "Relative Response (";
unit = p_ModelInfo->GetUnitString(UNIT_TYPES::NO_UNIT) + ")";
plotpar.Y_Label += unit;
plotpar.X_Min = -5.;
plotpar.X_Max = 35.;
plotpar.X_Label = "Temperature (";
unit = p_ModelInfo->GetUnitString(UNIT_TYPES::TEMP_UNIT) + ")";
plotpar.X_Label += unit;
plotpar.Y_ScalingCoef = 0;
plotpar.X_ScalingCoef = 0;


	pPs= new Ps(&N2O_TempMax,SIMPROFILE,ANAROBIC_GAS, STORAGE,OXIDATION, NORMAL); 
	Define(pPs, "N2O TempMax", 33.5, UNIT_TYPES::TEMP_UNIT, "Trace Gas Emissions > 0", 10., 50.); ps_vec.push_back(pPs);
	pPs= new Ps(&N2O_TempOpt, SIMPROFILE,ANAROBIC_GAS, STORAGE,OXIDATION, NORMAL); 
		Define(pPs,   "N2O TempOpt", 23.5,UNIT_TYPES::TEMP_UNIT,"Trace Gas Emissions > 0",10., 50.); ps_vec.push_back(pPs);
	pPs= new Ps(&N2O_TempShape, SIMPROFILE,ANAROBIC_GAS, STORAGE,OXIDATION, NORMAL); 
		Define(pPs,   "N2O TempShape", 1.5, "-","Trace Gas Emissions > 0",0.001, 5.); ps_vec.push_back(pPs);

	pFunc = new Func(TOTPROFILE, OXYGEN, DIFFUSION, SOIL_GAS_DYNAMICS, NORMAL);
		Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();

;
functorprop.FuncName = MyFunc::NITRI_N2O_PROD_MOIST_RESPONSE; functorprop.Arg = MyArgType::D1;
functorprop.FunctorInitOption = string::npos;
funcname = "N20 Production From Nitrification Moisture Response";
plotpar.Y_Label = "Relative Response (";
unit = p_ModelInfo->GetUnitString(UNIT_TYPES::NO_UNIT) + ")";
plotpar.Y_Label += unit;
plotpar.X_Min = 0;
plotpar.X_Max = 1;
plotpar.X_Label = "Relative Saturation (";
unit = p_ModelInfo->GetUnitString(UNIT_TYPES::NO_UNIT) + ")";
plotpar.X_Label += unit;
plotpar.Y_ScalingCoef = 0;
	pPs= new Ps(&N2O_RelSatCrit,SIMPROFILE,SOILGAS_N2O, STORAGE,SOIL_GAS_DYNAMICS, NORMAL); 
		Define(pPs,  "N2O RelSatCrit", 0.55, "-","Trace Gas Emissions > 0",0.1, 1.); ps_vec.push_back(pPs);
	pPs= new Ps(&N2O_RelSatForm, SIMPROFILE,SOILGAS_N2O, STORAGE,SOIL_GAS_DYNAMICS, NORMAL); 
		Define(pPs,   "N2O RelSatForm", 0.24, "-","Trace Gas Emissions > 0",0.001, 1.);	ps_vec.push_back(pPs);
	
	pFunc = new Func(TOTPROFILE, OXYGEN, DIFFUSION, SOIL_GAS_DYNAMICS, NORMAL);
		Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();
;
functorprop.FuncName = MyFunc::NITRI_NO_PROD_MOIST_RESPONSE; functorprop.Arg = MyArgType::D1;
funcname = "N0 Production From Nitrification Moisture Response";
	pPs= new Ps(&NO_RelSatCrit,SIMPROFILE, SOILGAS_NO, STORAGE,SOIL_GAS_DYNAMICS, NORMAL);
		Define(pPs,  "NO RelSatCrit", 0.45, "-","Trace Gas Emissions > 0",0.1, 1.);	ps_vec.push_back(pPs);
	pPs= new Ps(&NO_RelSatForm ,SIMPROFILE,SOILGAS_NO, STORAGE, SOIL_GAS_DYNAMICS, NORMAL);
		Define(pPs,  "NO RelSatForm", 0.024, "-","Trace Gas Emissions > 0",0.001, 1.); ps_vec.push_back(pPs);
	pFunc = new Func(TOTPROFILE, SOILGAS_NO, DIFFUSION, SOIL_GAS_DYNAMICS, NORMAL);
		Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();

		functorprop.FuncName = MyFunc::NITRI_NO_PROD_PH_RESPONSE; functorprop.Arg = MyArgType::D1;
		functorprop.FunctorInitOption = string::npos;
		funcname = "N0 Production From Nitrification PH Response";
		plotpar.Y_Label = "Relative Response (";
		unit = p_ModelInfo->GetUnitString(UNIT_TYPES::NO_UNIT) + ")";
		plotpar.Y_Label += unit;
		plotpar.X_Min = 3;
		plotpar.X_Max = 8;
		plotpar.X_Label = "Soil PH (";
		unit = p_ModelInfo->GetUnitString(UNIT_TYPES::NO_UNIT) + ")";
		plotpar.X_Label += unit;
		plotpar.Y_ScalingCoef = 0;



	pPs = new Ps(&NO_pH_Coef, SIMPROFILE, SOILGAS_NO, STORAGE, SOIL_GAS_DYNAMICS, NORMAL);	ps_vec.push_back(pPs);
		Define(pPs, "NO pHCoef", 4.7, "-", "Trace Gas Emissions > 0", 2., 10.);

	pFunc = new Func(TOTPROFILE, SOILGAS_NO, DIFFUSION, SOIL_GAS_DYNAMICS, NORMAL);
		Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();

		functorprop.FuncName = MyFunc::DENIT_ANAEROBIC_FRAC_RESPONSE; functorprop.Arg = MyArgType::D1;
		functorprop.FunctorInitOption = string::npos;
		funcname = "Denitrifiers Response to Anaerobic fraction";
		plotpar.Y_Label = "Relative Response (";
		unit = p_ModelInfo->GetUnitString(UNIT_TYPES::NO_UNIT) + ")";
		plotpar.Y_Label += unit;
		plotpar.X_Min = 0;
		plotpar.X_Max = 1;
		plotpar.X_Label = "Aerobic Volume Frac (";
		unit = p_ModelInfo->GetUnitString(UNIT_TYPES::NO_UNIT) + ")";
		plotpar.X_Label += unit;
		plotpar.Y_ScalingCoef = 0;
	pPs= new Ps(&Denit_Act_PowerCoef, SIMPROFILE,ANAROBIC_GAS, STORAGE,OXIDATION, NORMAL); 
	Define(pPs, "Denit Act PowerCoef", 1., "-", "Trace Gas Emissions > 0", 1., 5.); ps_vec.push_back(pPs);
	
	pFunc = new Func(TOTPROFILE, ANAROBIC_GAS, DIFFUSION, SOIL_GAS_DYNAMICS, NORMAL);
		Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();

;
functorprop.FuncName = MyFunc::EXCHANGE_EFFICIENY_ANAEROBIC; functorprop.Arg = MyArgType::D1;
functorprop.FunctorInitOption = string::npos;
funcname = "Diffusion Exchange efficiency from Anaerobic to Aerobic";
plotpar.Y_Label = "Relative Response (";
unit = p_ModelInfo->GetUnitString(UNIT_TYPES::NO_UNIT) + ")";
plotpar.Y_Label += unit;
plotpar.X_Min = 0;
plotpar.X_Max = 1.;
plotpar.X_Label = "Relative anaerobic fraction (";
unit = p_ModelInfo->GetUnitString(UNIT_TYPES::NO_UNIT) + ")";
plotpar.X_Label += unit;
plotpar.Y_ScalingCoef = 0;
	pPs= new Ps(&IntDiffRedFrac, SIMPROFILE,ANAROBIC_GAS, STORAGE,OXIDATION, NORMAL); 
		Define(pPs,   "IntDiffRedFrac", 0.01, "-","Trace Gas Emissions > 0",0.1E-8, 1.);  ps_vec.push_back(pPs);
	pPs= new Ps(&IntDiffRedFracBase,SIMPROFILE,ANAROBIC_GAS, STORAGE,OXIDATION, NORMAL); 
		Define(pPs,   "IntDiffRedFracBase", 0.0001, "-","Trace Gas Emissions > 0",0.0, 1.);	ps_vec.push_back(pPs);
	pFunc = new Func(TOTPROFILE, ANAROBIC_GAS, DIFFUSION, SOIL_GAS_DYNAMICS, NORMAL);
		Define(pFunc, functorprop, funcname, ps_vec, plotpar);	ps_vec.clear();
;
	Tab *pTab;

		pP=m_pModelMap->GetPPointer("mUpper Depth");
		mTab_Ox.push_back(pP);mTab_Ph.push_back(pP);
		pP=m_pModelMap->GetPPointer("mLower Depth");
		mTab_Ox.push_back(pP);mTab_Ph.push_back(pP);

	pP= new P(&Init_Oxygen_Conc,pNL,SIMPROFILE,ANAROBIC_GAS, TRANSPORT, OXIDATION, NORMAL); 
		Define(pP, pNL,"Init Oxygen Conc", initv,"-","Nitrogen and Carbon >= 1|Trace Gas Emissions > 0",0.,30.);
		mTab_Ox.push_back(pP);
;	initv.assign(pSoil->NL, .0004);
	pP = new P(&Init_CO2_Conc, pNL, SIMPROFILE, ANAROBIC_GAS, TRANSPORT, OXIDATION, NORMAL);
		Define(pP, pNL, "Init CO2 Conc", initv, "-", "Nitrogen and Carbon >= 1|Trace Gas Emissions > 0", 0., 30.);
		mTab_Ox.push_back(pP); //wenxin

	pTab= new Tab(&mTab_Ox,  SIMPROFILE,ANAROBIC_GAS, INITF,NOBPROC, NORMAL); 
		Define(pTab, "Initial Gas Concentrations",3);
		initv.assign(pSoil->NL,5.);

	pP= new P(&pH_Value, pNL,SIMPROFILE,ANAROBIC_GAS, STORAGE,OXIDATION, NORMAL); 
		Define(pP, pNL,"pH value",initv,"-","Nitrogen and Carbon > 0|Trace Gas Emissions > 0",1.,10.);
		mTab_Ph.push_back(pP);

	pTab= new Tab(&mTab_Ph,  SIMPROFILE,ANAROBIC_GAS, INITF,NOBPROC, NORMAL);
		Define(pTab,"pH Soil Profile",3);


return FunctionDef();
}
bool NC_GasFlux_Functions::FunctionDef() {


	return true;
}

 /**
double NC_GasFlux_Functions::N2O_TempFunctionL(size_t Layer) {
	
	return N2O_TempFunction(p_NC_Plant->NDrivTemperature[Layer]);
}

double NC_GasFlux_Functions::N2O_TempFunction(double Value) {

	return pow((N2O_TempMax-Value)/(N2O_TempMax-N2O_TempOpt),N2O_TempShape)*
					  exp(N2O_TempShape*(Value-N2O_TempOpt)/(N2O_TempMax-N2O_TempOpt));
}

double NC_GasFlux_Functions::N2O_MoistureFunctionL(size_t Layer) {

	double RelSat;
	RelSat=p_NC_Plant->NDrivTheta[Layer]/p_Soil_WaterF->M_Saturation[Layer];
	return N2O_MoistureFunction(RelSat);
}

double NC_GasFlux_Functions::N2O_MoistureFunction(double Value) {
	return 1-1/(1+exp((Value-N2O_RelSatCrit)/N2O_RelSatForm));
}


double NC_GasFlux_Functions::NO_MoistureFunctionL(size_t Layer) {

	double RelSat;
	RelSat=p_NC_Plant->NDrivTheta[Layer]/p_Soil_WaterF->M_Saturation[Layer];

	return NO_MoistureFunction(RelSat);
}

double NC_GasFlux_Functions::NO_MoistureFunction(double Value) {
	
	return 1-1/(1+exp((Value-NO_RelSatCrit)/NO_RelSatForm));
}


double NC_GasFlux_Functions::NO_pHFunctionL(size_t Layer) {
	

	return NO_pHFunction(pH_Value[Layer]);
}

double NC_GasFlux_Functions::NO_pHFunction(double Value) {
	
	return min(1.,max(0.05, NO_pH_Coef-Value));
}


double NC_GasFlux_Functions::ExCoefFunctionL(size_t Layer) {
	return An_VolFraction[Layer]*(1-An_VolFraction[Layer])*DO2_SoilCoef[Layer]*IntDiffRedFrac+IntDiffRedFracBase*DO2_SoilCoef[Layer];
}


double NC_GasFlux_Functions::ExCoefFunction(double AnVolFraction, double AirPorosity) {
	//Use GenFunctions, only	:D_Oxygen

	//Real AnVolFraction, AirPorosity

	return	D_oxygen(DO2_Tortuosity, AirPorosity, 20.)*AnVolFraction*(1.-AnVolFraction)*IntDiffRedFrac*86.4E6 
	+IntDiffRedFracBase*D_oxygen(DO2_Tortuosity, AirPorosity, 20.)*86.4E6;
}		 */
bool NC_GasFlux_Functions::DenitrifierUpdate() {


	
    N_Tot_DenitGrowth_NO3=N_Tot_DenitGrowth_NO2=N_Tot_DenitGrowth_NO=N_Tot_DenitGrowth_N2O=0;
    
	for(size_t i=0; i<An_Cons_NO3.size(); i++) {
 
		if(An_Cons_NO3[i]>0) {
		        RatioNO3[i]=min(1., max(0.,  An_NO3[i]/(An_Cons_NO3[i]*T_Step) ));
			    An_Cons_NO3[i] = RatioNO3[i]*An_Cons_NO3[i];
		}
        else {
                RatioNO3[i]=1.;
            	An_Cons_NO3[i] = 0.			    		    ;
		}    			
			
		if(An_Cons_NO2[i]>0.) {
		        RatioNO2[i]=min(1., max(0.,  An_NO2[i]/(An_Cons_NO2[i]*T_Step) ));
    		    An_Cons_NO2[i] = RatioNO2[i]*An_Cons_NO2[i]	;
		}
        else {
                RatioNO2[i]=1.;
            	An_Cons_NO2[i] = 0.	        ;	    				
		}    	
			
		if(NO_Exchange[i]>0.) {	
				NO_Exchange[i]=RatioNO[i]*NO_Exchange[i];
			    An_Cons_NO[i]=RatioNO[i]*An_Cons_NO[i]			;
		}
		else {
				NO_Exchange[i]=0.0;
			    An_Cons_NO[i]=0.0;
	    }		    

		if(N2O_Exchange[i]>0.) {					
			    N2O_Exchange[i]=RatioN2O[i]*N2O_Exchange[i];
			    An_Cons_N2O[i]=RatioN2O[i]*An_Cons_N2O[i];
		}
        else {
			    N2O_Exchange[i]=0.;
			    An_Cons_N2O[i]=0.;
	    }	
        
        if(Denitrifier_Biomass[i]>=0.&&Denitrifier_Biomass[i]<10000.) ;
        
        
        else
            Denitrifier_Biomass[i]=0.00;
        
        if(RatioNO3[i]>=0.&&RatioNO2[i]>=0.&&RatioNO[i]>=0&&RatioN2O[i]>=0.) {
       
            DenitGrowth_NO3[i]    =  DMic_Growth_NO3[i]*RatioNO3[i]*Denitrifier_Activity[i]*Denitrifier_Biomass[i];
            DenitGrowth_NO2[i]   =   DMic_Growth_NO2[i]*RatioNO2[i]*Denitrifier_Activity[i]*Denitrifier_Biomass[i];
            DenitGrowth_NO[i]    =   DMic_Growth_NO[i]*RatioNO[i]*Denitrifier_Activity[i]*Denitrifier_Biomass[i];
            DenitGrowth_N2O[i]   =   DMic_Growth_N2O[i]*RatioN2O[i]*DenitriInhibitNO3L(i)*Denitrifier_Activity[i]*Denitrifier_Biomass[i];
		}
        else 
            DenitGrowth_NO3[i]=0.;

		Denitrifier_Growth[i]   =DenitGrowth_NO3[i] +DenitGrowth_NO2[i]+DenitGrowth_NO[i]+DenitGrowth_N2O[i];
	    Denitrifier_Death[i]    =DMic_DeathRateCoef*max(Denitrifier_Biomass[i]-DMic_DormancyThreshold/p_Soil_WaterF->AThicknessLayers[i],0.)+
	                                DMic_DeathCDormancy*Denitrifier_Biomass[i];
	                                
	    
	    
	    if(Denitrifier_Growth[i]>0.) {
	    
	        if(DenitrifierGrowth==0) {
	            double ThresholdUptake=NUptMaxAvailFrac*SoilAmmonium[i]/T_Step;
	            DenitUptakeFactor[i]=max(0., min(1.,ThresholdUptake/Denitrifier_Growth[i]));
	            Denitrifier_Growth[i]   =Denitrifier_Growth[i]*DenitUptakeFactor[i];
			}
	        else if(DenitrifierGrowth==1) {
	            double ThresholdUptake=NUptMaxAvailFrac*O_State[_DIS][_N_][i]/T_Step;
	            DenitUptakeFactor[i]=max(0., min(1.,ThresholdUptake/Denitrifier_Growth[i]));
	            Denitrifier_Growth[i]   =Denitrifier_Growth[i]*DenitUptakeFactor[i];
			}
	        else {
                double ThresholdUptakeMin=NUptMaxAvailFrac*SoilAmmonium[i]/T_Step;
                double ThresholdUptakeOrg=NUptMaxAvailFrac*O_State[_DIS][_N_][i]/T_Step            ;
                double ThresholdUptake=NUptMaxAvailFrac*(O_State[_DIS][_N_][i]+SoilAmmonium[i])/T_Step     ;
                DenitUptakeFactor[i]=max(0., min(1.,ThresholdUptake/Denitrifier_Growth[i]));
                Denitrifier_Growth[i]=DenitUptakeFactor[i]*Denitrifier_Growth[i]       ;
	            DenitUptakeMin[i]=ThresholdUptakeMin/(ThresholdUptakeMin+ThresholdUptakeOrg)*Denitrifier_Growth[i];
	            DenitUptakeOrg[i]=Denitrifier_Growth[i]-DenitUptakeMin[i]	           	         ;
	        }  
		}
        else {
            DenitUptakeMin[i]=0.;
            DenitUptakeOrg[i]=0.;
            DenitUptakeFactor[i]=0.;
	    }
	    
	    
	    if(DenitUptakeFactor[i]<1.) {
	        DenitGrowth_NO3[i]    =  DenitUptakeFactor[i]*DenitGrowth_NO3[i];
            DenitGrowth_NO2[i]   =  DenitUptakeFactor[i]*DenitGrowth_NO2[i];
            DenitGrowth_NO[i]    =  DenitUptakeFactor[i]*DenitGrowth_NO[i];
            DenitGrowth_N2O[i]   =  DenitUptakeFactor[i]*DenitGrowth_N2O[i];
           	An_Cons_NO3[i]		 = DenitUptakeFactor[i]*An_Cons_NO3[i];
		    An_Cons_NO2[i]		 = DenitUptakeFactor[i]*An_Cons_NO2[i];
 		    An_Cons_NO[i]		 = DenitUptakeFactor[i]*An_Cons_NO[i];
            An_Cons_N2O[i]	     = DenitUptakeFactor[i]*An_Cons_N2O[i];
         }
 		 An_Prod_NO2[i]	=An_Cons_NO3[i];
 		 An_Prod_NO[i]	=An_Cons_NO2[i]	;
	     An_Prod_N2O[i]  =An_Cons_NO[i];
	     An_Prod_N2[i]   =An_Cons_N2O[i];
		 if(T_Step==0.) 	T_Step = *pT_Step;
	
	    Denitrifier_ActivityRate[i]=DeniActivityRateCoef*(DMic_TempFunctionL(i)*DMic_pH_ResponseL(i)*DMic_N_ResponseL(i)*pow(An_VolFraction[i],Denit_Act_PowerCoef)-Denitrifier_Activity[i])/T_Step;
	
	}
	if (An_Cons_NO3[0] == 0.) {
		An_Cons_NO3[0] = 1.E-7;
	}
	
	N_Tot_DenitGrowth_NO3   =  sum(DenitGrowth_NO3);
    N_Tot_DenitGrowth_NO2   =  sum(DenitGrowth_NO2);
    N_Tot_DenitGrowth_NO    =  sum(DenitGrowth_NO);
    N_Tot_DenitGrowth_N2O   =  sum(DenitGrowth_N2O);
	return true;
	   
}


