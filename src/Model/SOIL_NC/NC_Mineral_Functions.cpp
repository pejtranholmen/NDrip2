#include "NC_Mineral_Functions.h"
#include "../PLANT_NC/PlantOrgModel.h"
#include "../SOIL/SoilModel.h"
#include "../SOIL/Soil_Frost.h"
#include "../PLANT/PlantModel.h"
#include "./Functors/NC_Soil_Functors.h"


NC_Mineral_Functions::NC_Mineral_Functions(void)
{


}
NC_Mineral_Functions::~NC_Mineral_Functions(void)
{
}


bool NC_Mineral_Functions::Def()
{
	P_Mineral_Functions::Def();
	ModuleNo=26;
	SoilModel* p_Soil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	Soil_Frost* pSoil=&p_Soil->m_SoilFrost;
	NE *pNL=m_pModelMap->GetNEPointer("Number of Layers");
	PlantModel* pPlant=(PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");

	WaterUptake* p_Plant=&pPlant->m_WaterUptake;
	NE *pNumPlants=m_pModelMap->GetNEPointer("Number of Plants");

	Sw* pSw; Ps *pPs; P *pP; X *pX; T *pT; G *pG;  Gs *pGs;
	pSw= new Sw(&InitialMineral_N ,  SIMPROFILE,MINERAL_N, NOFPROC,INITB, NORMAL);
		Define(pSw, "Initial Mineral N",0,"Uniform concentration|Concentration(z)","Nitrogen and Carbon > 0");
	pSw= new Sw(&Denitrification ,  SIMPROFILE,MINERAL_N, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pSw, "Denitrification",1,"off|Old SOILN|Microbial based","Nitrogen and Carbon > 0",3102);
	pSw= new Sw(&DenitrifierGrowth ,  SIMPROFILE,MINERAL_N, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pSw, "Denitrifier Growth",0,"Mineral N|Organic N|Both Sources","Nitrogen and Carbon > 0|Denitrification=2|Dissolved Organics=1");
	pSw= new Sw(&NitrifierGrowth ,  SIMPROFILE,MINERAL_N, NOFPROC,NITRIFICATION, NORMAL);
		Define(pSw,"Nitrifier Growth",0,"Mineral N|Organic N","Nitrogen and Carbon > 0|Nitrification>0|Dissolved Organics=1");
	pSw= new Sw(&Denit_Temp_Response,  SIMPROFILE,MINERAL_N, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pSw, "Denit Temp Response",0,"Common|Q10 whole range|Q10 above threshold|Ratkowsky function","Nitrogen and Carbon > 0|Denitrification > 0");
	pSw= new Sw(&Nitrific_Temp_Response,  SIMPROFILE,MINERAL_N, NOFPROC,NITRIFICATION, NORMAL);
		Define(pSw, "Nitrific Temp Response",0,"Common|Q10 whole range|Q10 above threshold|Ratkowsky function","Nitrogen and Carbon > 0|Denitrification > 0");
	pSw= new Sw(&Denit_Depth_Distribution,  SIMPROFILE,MINERAL_N, NOFPROC,INITB, NORMAL);
		Define(pSw, "Denit Depth Distribution",1,"Table|Linear decrease|Constant|Exponential","Nitrogen and Carbon > 0|Denitrification > 0");
	pSw= new Sw(&Ammonium_Mobility,  SIMPROFILE,AMMONIUM, TRANSPORT,NOBPROC, NORMAL);
		Define(pSw,"Ammonium Mobility",0,"No|Linear isotherm","Nitrogen and Carbon > 0");
	pSw= new Sw(&Nitrification,  SIMPROFILE,MINERAL_N, NOFPROC,NITRIFICATION, NORMAL);
		Define(pSw, "Nitrification",0,"Old SOILN|Microbial based","Nitrogen and Carbon > 0");
	pSw= new Sw(&Initial_Nitrifier,  SIMPROFILE,MINERAL_N, NOFPROC,INITB, NORMAL);
		Define(pSw, "Initial Nitrifier",0,"Linear decrease|Constant|Exponential","Nitrogen and Carbon > 0|Nitrification >0");
//	pSw= new Sw(&AmmoniaVolatilisation,  SIMPROFILE,MINERAL_N, NOFPROC,INITB, NORMAL);
//		Define(pSw, "AmmoniaVolatilisation",0,"off|on","Nitrogen and Carbon > 0");
	pSw= new Sw(&Flexible_Uptake,  SIMPROFILE,MINERAL_N, TRANSPORT,PLANT_NUPTAKE, NORMAL);
		Define(pSw, "Flexible Uptake",0,"Common for all plants|Specific for each plant","Nitrogen and Carbon > 0");


		Func *pFunc;
		// Parameters;
		string funcname;
		FunctorProp functorprop;
		vector<Ps*> ps_vec;
		vector<P*> p_vec;
		ps_vec.clear();
		Tab *pTab;

		functorprop.FunctorInitOption = string::npos;
		functorprop.pModelInfo = m_pModelInfo;

		PLOT_PARAM plotpar;
		plotpar.Num_Of_Curves = 1;
		plotpar.Num_Of_Points = 100;
		plotpar.Num_Of_X = 1;
		plotpar.X_Min = 0;
		plotpar.X_Max = 100;
		plotpar.X_Label = "X";
		plotpar.Y_Label = "Y";
		plotpar.LogXScale = false;
		plotpar.LogYScale = false;
		plotpar.Num_Of_Opt = 1;

		string unit;




// Initial conditions 
	pPs= new Ps(&InitNitrateConc, TOTPROFILE,NITRATE, NOFPROC,INITB, NORMAL);
	Define(pPs,   "InitNitrateConc", 10., "mg-N/l","Initial Mineral N = 0|Nitrogen and Carbon > 0",0.0001, 100., 0.01, 50.);
	pPs= new Ps(&InitAmmoniumConc, TOTPROFILE,AMMONIUM, NOFPROC,INITB, NORMAL);
		Define(pPs,   "InitAmmoniumConc", 10., "mg-N/l","Initial Mineral N = 0|Nitrogen and Carbon > 0",0.0001, 100., 0.01, 50.);


		functorprop.FuncName = MyFunc::DENIT_THETA_RESPONSE; functorprop.Arg = MyArgType::T_D1;
		funcname = "Denitrification Water Content Response";
		ps_vec.clear();
		plotpar.X_Label = "Water Content "; plotpar.X_Label+= p_ModelInfo->GetUnitString(UNIT_TYPES::WATERCONTENT_UNIT,true);
		plotpar.Y_Label = "Relative Response "; 	plotpar.Y_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::NO_UNIT, true);


// Denitrification responses
	pPs = new Ps(&DenitThetaRange, TOTPROFILE, NITRATE, NOFPROC, DENITRIFICATION, NORMAL); ps_vec.push_back(pPs);
		Define(pPs,   "DenitThetaRange", 10., "vol %","Denitrification = 1|Nitrogen and Carbon > 0",0.0001, 100., 0.01, 50.);
	pPs= new Ps(&DenitThetaPowerC, TOTPROFILE,NITRATE, NOFPROC,DENITRIFICATION, NORMAL); ps_vec.push_back(pPs);
		Define(pPs,   "DenitThetaPowerC2", 10., "-","Denitrification = 1|Nitrogen and Carbon > 0",0.0001, 100., 0.01, 50.);

	p_vec.push_back(m_pModelMap->GetPPointer("mSaturation"));
	pFunc = new Func(SIMPROFILE, NITRATE, NOFPROC, DENITRIFICATION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, p_vec, plotpar);


	functorprop.FuncName = MyFunc::DENIT_TEMP_Q10_RESPONSE; functorprop.Arg = MyArgType::D1;
	funcname = "Q10 Temperature Response";
	plotpar.X_Min = -10;
	plotpar.X_Max = 30;
	plotpar.X_Label = "Temperature "; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::TEMP_UNIT, true);
	plotpar.Y_Label = "Relative Response "; 	plotpar.Y_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::NO_UNIT, true);
		ps_vec.clear();


	pPs = new Ps(&DenitTemQ10, TOTPROFILE, NITRATE, NOFPROC, DENITRIFICATION, NORMAL); ps_vec.push_back(pPs);
		Define(pPs,   "DenitTemQ10", 2., "-","Denitrification > 0|Nitrogen and Carbon > 0|Denit Temp Response < 3|Denit Temp Response > 0",0.0, 5., 0.5, 4.);
	pPs= new Ps(&DenitTemQ10Bas, TOTPROFILE,NITRATE, NOFPROC,DENITRIFICATION, NORMAL); ps_vec.push_back(pPs);
		Define(pPs,   "DenitTemQ10Bas", 20.,UNIT_TYPES::TEMP_UNIT,"Denitrification > 0|Nitrogen and Carbon > 0|Denit Temp Response < 3|Denit Temp Response > 0",0.0001, 100., 10., 30.);

	pFunc = new Func(SIMPROFILE, NITRATE, NOFPROC, DENITRIFICATION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);

	functorprop.FuncName = MyFunc::DENIT_TEMP_Q10THRESHOLD_RESPONSE; functorprop.Arg = MyArgType::D1;
	funcname = "Q10  with Threshold Temperature Response";
	pPs = new Ps(&DenitTemQ10Threshold, TOTPROFILE, NITRATE, NOFPROC, DENITRIFICATION, NORMAL); ps_vec.push_back(pPs);
		Define(pPs,   "DenitTemQ10Threshold", 5.,UNIT_TYPES::TEMP_UNIT,"Denitrification > 0|Nitrogen and Carbon > 0|Denit Temp Response = 2",0.0001, 20., 2., 10.);

	pFunc = new Func(SIMPROFILE, NITRATE, NOFPROC, DENITRIFICATION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);



	functorprop.FuncName = MyFunc::DENIT_TEMP_RATKOWSKY_RESPONSE; functorprop.Arg = MyArgType::D1;
	funcname = "Ratkowsky Temperature Response";
	ps_vec.clear();

	pPs = new Ps(&DenitTempMin, TOTPROFILE, NITRATE, NOFPROC, DENITRIFICATION, NORMAL); ps_vec.push_back(pPs);
		Define(pPs, "DenitTempMin", -8.,UNIT_TYPES::TEMP_UNIT, "Denitrification > 0|Nitrogen and Carbon > 0|Denit Temp Response = 3", -20., 10., -10., 5.);
	pPs= new Ps(&DenitTempMax, TOTPROFILE,NITRATE, NOFPROC,DENITRIFICATION, NORMAL); ps_vec.push_back(pPs);
		Define(pPs,   "DenitTempMax", 20.,UNIT_TYPES::TEMP_UNIT,"Denitrification > 0|Nitrogen and Carbon > 0|Denit Temp Response = 3",0.0001, 100., 10., 30.);



	pFunc = new Func(SIMPROFILE, NITRATE, NOFPROC, DENITRIFICATION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);

	functorprop.FuncName = MyFunc::DENIT_NITRATE_RESPONSE; functorprop.Arg = MyArgType::D1;
	funcname = "Denitrification Michaelis Menten Response";
	plotpar.X_Label = "Nitrate Concentration "; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::mMASS_LITER_UNIT, true);
	plotpar.X_Min = 0;
	plotpar.X_Max = 30;
	ps_vec.clear();

	pPs= new Ps(&DenitNitrateHalfSat, TOTPROFILE,NITRATE, NOFPROC,DENITRIFICATION, NORMAL);
	Define(pPs, "DenitNitrateHalfSat", 10., UNIT_TYPES::mMASS_LITER_UNIT, "Denitrification > 0|Nitrogen and Carbon > 0", 0.0001, 100., 0.01, 50.); ps_vec.push_back(pPs);

	pFunc = new Func(SIMPROFILE, NITRATE, NOFPROC, DENITRIFICATION, NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);


// Denitification potential and distribution;
	pPs= new Ps(&DenitPotentialRate, TOTPROFILE,NITRATE, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pPs,   "DenitPotentialRate", 0.04,UNIT_TYPES::MASSFLOW_UNIT,"Denitrification > 0|Nitrogen and Carbon > 0",0., 1., 0.001, 0.4);


	vector<double> initv;

	pP=m_pModelMap->GetPPointer("mUpper Depth");
		mTab_InitMin.push_back(pP); mTab_DenitPot.push_back(pP);
	pP=m_pModelMap->GetPPointer("mLower Depth");
		mTab_InitMin.push_back(pP); mTab_DenitPot.push_back(pP);

	pP= new P(&InitNitrateConcZ,pNL,SIMPROFILE,NITRATE, NOFPROC,INITB, NORMAL);
		initv.assign(pSoil->NL,10.);
		Define(pP, pNL  ,"I Nitrate Conc",initv,"mg-N/l", "Initial mineral N Conditions = 1|Nitrogen and Carbon > 0",1.E-3,100.);
		mTab_InitMin.push_back(pP);
	pP= new P(&InitAmmoniumConcZ,pNL,SIMPROFILE,AMMONIUM, NOFPROC,INITB, NORMAL);
		Define(pP, pNL  ,"I Ammonium Conc",initv,"mg-N/l", "Initial mineral N Conditions = 1|Nitrogen and Carbon > 0",1.E-3, 100.);
		mTab_InitMin.push_back(pP);

	pTab = new Tab(&mTab_InitMin, SIMPROFILE,MINERAL_N, NOFPROC,INITB, NORMAL);
		Define(pTab, "Initial mineral-N concentrations",3);


	pP= new P(&DenitDistributionTab,pNL,SIMPROFILE,NITRATE, NOFPROC,DENITRIFICATION, NORMAL);
		initv.assign(pSoil->NL,0.1);
		Define(pP, pNL  ,"DenitDistZ",initv,"-", "Denit Depth Distribution = 0|Nitrogen and Carbon > 0");
		mTab_DenitPot.push_back(pP);

	pPs= new Ps(&DenitFracExpTail, TOTPROFILE,NITRATE, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pPs,  "DenitFracExpTail",0.1,"-", "Denit Depth Distribution = 3|Nitrogen and Carbon > 0",0.,1.);
	pPs= new Ps(&DenitDepth, TOTPROFILE,NITRATE, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pPs, "DenitDepth",-1.,"-", "Denit Depth Distribution > 0|Nitrogen and Carbon >0",-10.,-0.001,-4.,-0.01);
	pTab= new Tab(&mTab_DenitPot, SIMPROFILE,NITRATE, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pTab,"Potential denitrification distribution ",3);

	// Nitrification;
	functorprop.FuncName = MyFunc::NITRIFICATION_SIMPLE; functorprop.Arg = MyArgType::D2;
	funcname = "Nitrification Rate Function";
		plotpar.X_Label = "Ammonium "; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::MASS_UNIT, true);
		plotpar.X_Min = 0;
		plotpar.X_Max = 3.;
		plotpar.Y_Label = "Nitrification Rate "; plotpar.Y_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::MASSFLOW_UNIT, true);
		plotpar.Num_Of_Curves = 3;
		plotpar.Num_Of_X = 2;
		plotpar.X2_Vec={ .1,10.,20. };
		plotpar.LegendsVec = { "Nitrate .1 ", "Nitrate 10 ","Nitrate 20 " };
		for(size_t i=0; i<3; ++i) plotpar.LegendsVec[i]+= p_ModelInfo->GetUnitString(UNIT_TYPES::MASS_UNIT, true);
		ps_vec.clear();

	pPs= new Ps(&Nitr_Amm_Ratio, TOTPROFILE,MINERAL_N, NOFPROC,NITRIFICATION, NORMAL);
		Define(pPs, "NitrateAmmRatio", 8., UNIT_TYPES::MASS_RATIO_UNIT, "Nitrogen and Carbon > 0|Nitrification = 0", 0.1, 100., 1., 15.); ps_vec.push_back(pPs);
	pPs= new Ps(&Nitr_Specific_Rate, TOTPROFILE,MINERAL_N, NOFPROC,NITRIFICATION, NORMAL);
		Define(pPs,   "NitrificSpecificRate", .2, UNIT_TYPES::RATECOEF_UNIT,"Nitrogen and Carbon > 0|Nitrification =0",0.01, 1., .01, .8); ps_vec.push_back(pPs);

	pFunc = new Func(SIMPROFILE, NITRATE, NOFPROC, NITRIFICATION, NORMAL);
		Define(pFunc, functorprop, funcname, ps_vec, plotpar);

	functorprop.FuncName = MyFunc::NITRI_TEMP_Q10_RESPONSE; functorprop.Arg = MyArgType::D1;
	funcname = "Q10 Temperature Response";
	plotpar.X_Min = -10.;
	plotpar.X_Max = 30.;
	plotpar.X_Label = "Temperature "; plotpar.X_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::TEMP_UNIT, true);
	plotpar.Y_Label = "Relative Response "; plotpar.Y_Label += p_ModelInfo->GetUnitString(UNIT_TYPES::NO_UNIT, true);
	plotpar.Num_Of_Curves = 1;
	plotpar.Num_Of_X = 1;
	plotpar.X2_Vec = {};
	plotpar.LegendsVec = {};
	ps_vec.clear();

;
pPs = new Ps(&NitriTemQ10, TOTPROFILE, MINERAL_N, NOFPROC, NITRIFICATION, NORMAL); ps_vec.push_back(pPs);
		Define(pPs,   "NitriTemQ10", 2., "-","Nitrogen and Carbon > 0|Nitrific Temp Response < 3|Nitrific Temp Response > 0",0.0, 5., 0.5, 4.);
	pPs= new Ps(&NitriTemQ10Bas, TOTPROFILE,MINERAL_N, NOFPROC,NITRIFICATION, NORMAL); ps_vec.push_back(pPs);
		Define(pPs,   "NitriTemQ10Bas", 20.,UNIT_TYPES::TEMP_UNIT,"Nitrogen and Carbon > 0|Nitrific Temp Response < 3|Nitrific Temp Response > 0",0.0001, 100., 10., 30.);

		pFunc = new Func(PLANT, CARBON, NOFPROC, NITRIFICATION, NORMAL);
		Define(pFunc, functorprop, funcname, ps_vec, plotpar);

		functorprop.FuncName = MyFunc::NITRI_TEMP_Q10THRESHOLD_RESPONSE; functorprop.Arg = MyArgType::D1;
		funcname = "Q10  with Threshold Temperature Response";


	pPs= new Ps(&NitriTemQ10Threshold, TOTPROFILE,MINERAL_N, NOFPROC,NITRIFICATION, NORMAL); ps_vec.push_back(pPs);
		Define(pPs,   "NitriTemQ10Threshold", 5.,UNIT_TYPES::TEMP_UNIT,"Nitrogen and Carbon > 0|Nitrific Temp Response = 2",0.0001, 20., 2., 10.);
		pFunc = new Func(PLANT, CARBON, NOFPROC, NITRIFICATION, NORMAL);
		Define(pFunc, functorprop, funcname, ps_vec, plotpar);
		
		functorprop.FuncName = MyFunc::NITRI_TEMP_RATKOWSKY_RESPONSE; functorprop.Arg = MyArgType::D1;
		funcname = "Ratkowsky Temperature Response";
		ps_vec.clear();
		pPs = new Ps(&NitriTempMin, TOTPROFILE, MINERAL_N, NOFPROC, NITRIFICATION, NORMAL); ps_vec.push_back(pPs);
		Define(pPs, "NitriTempMin", -8.,UNIT_TYPES::TEMP_UNIT, "Nitrogen and Carbon > 0|Nitrific Temp Response = 3", -20., 10., -10., 5.);
	pPs= new Ps(&NitriTempMax, TOTPROFILE,MINERAL_N, NOFPROC,NITRIFICATION, NORMAL); ps_vec.push_back(pPs);
		Define(pPs,   "NitriTempMax", 20.,UNIT_TYPES::TEMP_UNIT,"Nitrogen and Carbon > 0|Nitrific Temp Response = 3",0.0001, 100., 10., 30.);
		pFunc = new Func(PLANT, CARBON, NOFPROC, NITRIFICATION, NORMAL);
		Define(pFunc, functorprop, funcname, ps_vec, plotpar);
	

	
	pP=m_pModelMap->GetPPointer("mUpper Depth");
		mTab_Nitrific.push_back(pP);mTab_AmIso.push_back(pP);
	pP=m_pModelMap->GetPPointer("mLower Depth");
		mTab_Nitrific.push_back(pP);mTab_AmIso.push_back(pP);	

	pP= new P(&Nitri_pH_Response,pNL,SIMPROFILE,MINERAL_N, NOFPROC, NITRIFICATION, NORMAL);initv.assign(pSoil->NL,1.);
		Define(pP, pNL  ,"RelScale Response",initv,"-","Nitrogen and Carbon > 0",0.,1.);
		mTab_Nitrific.push_back(pP);
	pTab= new Tab(&mTab_Nitrific,SIMPROFILE,MINERAL_N, NOFPROC, NITRIFICATION, NORMAL); 
		Define(pTab, "Nitrification pH-response distribution ",3);
;	initv.assign(pSoil->NL,.99);

	pP= new P(&AmmoniumAdsFrac,pNL,SIMPROFILE,AMMONIUM, TRANSPORT,SORPTION, NORMAL);
		Define(pP, pNL  ,"NH4 Ads Frac",initv,"-","Nitrogen and Carbon > 0",0.,1.);
		mTab_AmIso.push_back(pP);
		pTab= new Tab(&mTab_AmIso, SIMPROFILE,AMMONIUM, TRANSPORT,SORPTION, NORMAL);
		Define(pTab,"Linear Ammonium isotherm",3);
;
// Uptake of mineral N and P;
	pPs= new Ps(&NUptMaxAvailFrac, TOTPROFILE,MINERAL_N, NOFPROC,PLANT_NUPTAKE, NORMAL);
		Define(pPs,   "NUptMaxAvailFrac", 0.08,UNIT_TYPES::TEMP_UNIT,"Nitrogen and Carbon > 0",0.001, 0.7, .01, 0.2);
	pPs= new Ps(&NUptFlexDegree, TOTPROFILE,MINERAL_N, NOFPROC,PLANT_NUPTAKE, NORMAL);
		Define(pPs,   	"NUptFlexibilityDeg", .5,UNIT_TYPES::TEMP_UNIT,"Nitrogen and Carbon > 0|Flexible Uptake = 0",0.0, 1.);
	pPs= new Ps(&NUptRootCoefMin, TOTPROFILE,MINERAL_N, NOFPROC,PLANT_NUPTAKE, NORMAL);
		Define(pPs,   	"NUptRootCoefMin", .01,UNIT_TYPES::TEMP_UNIT,"Nitrogen and Carbon > 0|Root_Density_Influence>0",0.0001, 1.);
	pPs = new Ps(&PUptMaxAvailFrac, TOTPROFILE, MINERAL_P, NOFPROC, PLANT_NUPTAKE, NORMAL);
		Define(pPs, "PUptMaxAvailFrac", 0.08,UNIT_TYPES::TEMP_UNIT, "Nitrogen and Carbon > 0|Phosphorus>0", 0.001, 0.7, .01, 0.2);
;
;
	pP= new P(&NUptPlantFlexDegree,pNumPlants,SIMPROFILE,AMMONIUM, TRANSPORT,SORPTION, NORMAL);
	initv.assign(p_Plant->NumPlants,0.5);
	Define(pP, pNumPlants ,"Flex Degree",initv,"-","Nitrogen and Carbon > 0|Flexible Uptake > 0",0.,1.);
	mTab_Flex.push_back(pP);
	pTab= new Tab(&mTab_Flex, SIMPROFILE,AMMONIUM, TRANSPORT,SORPTION, NORMAL);
	Define(pTab,"Plant specific flexible uptake","Flex Degree","Flexible Uptake > 0");
	
	pPs= new Ps(&DeniActivityRateCoef, TOTPROFILE,NITRATE, NOFPROC,DENITRIFICATION, NORMAL);
	Define(pPs,    "DeniActivityRateCoef", 0.5, "1/day","Nitrogen and Carbon > 0|Denitrification > 1",0.1, 1.);
	pPs= new Ps(&InitDenitBiomass, TOTPROFILE,DENITRIFIERS, NOFPROC,INITB, NORMAL);
	Define(pPs,  "InitDenitBiomass",2.,"gN/m2", "Denitrification =2",0.,500.);
	pPs= new Ps(&NXOY_HalfRateCoef, TOTPROFILE,DENITRIFIERS, NOFPROC,DENITRIFICATION, NORMAL);
	Define(pPs,  "NXOY_HalfRateCoef",87.,"mg/l", "Denitrification =2",0.,500.);
	pPs= new Ps(&NXOY_DOC_HalfRateCoef, TOTPROFILE,DENITRIFIERS, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pPs,  "NXOY_Doc_HalfRateCoef",17.,"mg/l", "Denitrification =2",0.,100.);
;
	pPs= new Ps(&DMic_GrowthCoef_NO3, TOTPROFILE,DENITRIFIERS, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pPs,  	"DMic_GrowthCoef_NO3",16.,"/day", "Denitrification =2",0.,100.);
	pPs= new Ps(&DMic_GrowthCoef_NO2, TOTPROFILE,DENITRIFIERS, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pPs,  	"DMic_GrowthCoef_NO2",16.,"/day", "Denitrification =2",0.,100.);
	pPs= new Ps(&DMic_GrowthCoef_NO, TOTPROFILE,DENITRIFIERS, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pPs,  	"DMic_GrowthCoef_NO",8.2,"/day", "Denitrification =2",0.,100.);
	pPs= new Ps(&DMic_GrowthCoef_N2O, TOTPROFILE,DENITRIFIERS, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pPs,  	"DMic_GrowthCoef_N2O",8.2,"/day", "Denitrification =2",0.,100.);
;
	pPs= new Ps(&DMic_RespCoef_NO3, TOTPROFILE,DENITRIFIERS, NOFPROC,DENITRIFICATION, NORMAL);
	 	Define(pPs,  	"Dmic_RespCoef_NO3",2.2,"/day", "Denitrification =2",0.,100.);
	pPs= new Ps(&DMic_RespCoef_NO2, TOTPROFILE,DENITRIFIERS, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pPs,  	"Dmic_RespCoef_NO2",0.84,"/day", "Denitrification =2",0.,100.);
	pPs= new Ps(&DMic_RespCoef_NO, TOTPROFILE,DENITRIFIERS, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pPs,  	"Dmic_RespCoef_NO",0.84,"/day", "Denitrification =2",0.,100.);
	pPs= new Ps(&DMic_RespCoef_N2O, TOTPROFILE,DENITRIFIERS, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pPs,  	"Dmic_RespCoef_N2O",1.9,"/day", "Denitrification =2",0.,100.);
;
	pPs= new Ps(&DMic_EffCoef_NO3, TOTPROFILE,DENITRIFIERS, NOFPROC,DENITRIFICATION, NORMAL);
	 	Define(pPs,  	"Dmic_EffCoef_NO3",0.401,"-", "Denitrification =2",0.1,1.);
	pPs= new Ps(&DMic_EffCoef_NO2, TOTPROFILE,DENITRIFIERS, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pPs,  	"Dmic_EffCoef_NO2",0.428,"-", "Denitrification =2",0.1,1.);
	pPs= new Ps(&DMic_EffCoef_NO, TOTPROFILE,DENITRIFIERS, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pPs,  	"Dmic_EffCoef_NO",0.428,"-", "Denitrification =2",0.1,1.);
	pPs= new Ps(&DMic_EffCoef_N2O, TOTPROFILE,DENITRIFIERS, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pPs,  	"Dmic_EffCoef_N2O",0.151,"-", "Denitrification =2",0.1,1.);
;
	pPs= new Ps(&D_InhiHalfRateNO3_N2O, TOTPROFILE,DENITRIFIERS, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pPs,  	"D_InhiHalfRateNO3_N2O",0.3,"mg/l", "Denitrification =2",0.,1.E5);
;
	pPs= new Ps(&DMic_DeathRateCoef, TOTPROFILE,DENITRIFIERS, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pPs,  	"DMic_DeathRateCoef",0.01,"/day", "Denitrification =2",0.,1.);
	pPs= new Ps(&DMic_DeathCDormancy, TOTPROFILE,DENITRIFIERS, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pPs,  	"DMic_DeathCDormancy",0.0001,"/day", "Denitrification =2",0.,0.01);
	pPs= new Ps(&DMic_DormancyThreshold, TOTPROFILE,DENITRIFIERS, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pPs,  "DMic_DormancyThreshold",0.1,"g/N/m", "Denitrification =2",0.,10.)	;
;
	pPs= new Ps(&D_pH_HalfCoef, TOTPROFILE,DENITRIFIERS, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pPs,  	"D_pH_HalfCoef",4.25,"-", "Denitrification =2",0.,100.);
	pPs= new Ps(&D_pH_ShapeCoef, TOTPROFILE,DENITRIFIERS, NOFPROC,DENITRIFICATION, NORMAL);
		Define(pPs,  	"D_pH_ShapeCoef",0.5,"-", "Denitrification =2",0.,100.);
	
	pPs= new Ps(&NitriGrowthCoef, TOTPROFILE,NITRIFIERS, NOFPROC,NITRIFICATION, NORMAL);
		
	Define(pPs, "NitriGrowthCoef", .01, "1/day","Nitrogen and Carbon > 0|Nitrification > 0",0.00001, 10.);
	pPs= new Ps(&NitriHalfRateDOC, TOTPROFILE,NITRIFIERS, NOFPROC,NITRIFICATION, NORMAL);
		Define(pPs,     "NitriHalfRateDOC", 5.25, "mg/l","Nitrogen and Carbon > 0|Nitrification > 0",1.E-10, 10.);
	pPs= new Ps(&NitriHalfRateNO3, TOTPROFILE,NITRIFIERS, NOFPROC,NITRIFICATION, NORMAL);
		Define(pPs,     "NitriHalfRateNO3", 1.6, "mg/l","Nitrogen and Carbon > 0|Nitrification > 0",1.E-10, 10.);

	pPs= new Ps(&NitriDeathCoef, TOTPROFILE,NITRIFIERS, NOFPROC,NITRIFICATION, NORMAL);
		Define(pPs,    "NitriDeathCoef", .005, "1/day","Nitrogen and Carbon > 0|Nitrification > 0",1.E-7, 10.);

	pPs= new Ps(&InitNitriFracExpTail, TOTPROFILE,NITRIFIERS, NOFPROC,NITRIFICATION, NORMAL);
		Define(pPs,  "InitNitriFracExpTail",0.1,"-", "Initial Nitrifier = 2|Nitrification >0",0.,1.);
	pPs= new Ps(&NitrifierDepth, TOTPROFILE,NITRIFIERS, NOFPROC,NITRIFICATION, NORMAL);
		Define(pPs,  "NitrifierDepth",-1.,"m", "Nitrification >0",-5.,0.);
	pPs= new Ps(&InitNitrifier, TOTPROFILE,NITRIFIERS, NOFPROC,INITB, NORMAL);
		Define(pPs,  "InitNitrifier",1.,UNIT_TYPES::MASS_UNIT, "Nitrification >0",0.,100.);
	pPs= new Ps(&NitrificationRateCoef, TOTPROFILE,NITRIFIERS, NOFPROC,NITRIFICATION, NORMAL);
		Define(pPs,  "NitrificationRateCoef",.25,"mg ha/(l day kg)", "Nitrification >0",0.1E-5,1.);
	pPs= new Ps(&NitriHalfRateNH4, TOTPROFILE,NITRIFIERS, NOFPROC,NITRIFICATION, NORMAL);
		Define(pPs,  "NitriHalfRateNH4",6.18,"mg/l", "Nitrification >0",0.,50.);

		// To be used in Fungi Uptake function;
		pPs = new Ps(&FungiCoverRedCoef, TOTPROFILE, MYCORRHIZA, NOFPROC, DENITRIFICATION, NORMAL);
		Define(pPs, "FungiCoverRedCoef", 10., "-", "Denitrification > 0|Nitrogen and Carbon > 0|Mycorrhiza Fungal Growth>0", 0., 10.);
		pPs = new Ps(&FungiMinEnhanceCoef, TOTPROFILE, MYCORRHIZA, NOFPROC, DENITRIFICATION, NORMAL);
		Define(pPs, "FungiMinEnhanceCoef", 1., "-", "Denitrification > 0|Nitrogen and Carbon > 0|Mycorrhiza Fungal Growth>0", 0., 10.);
		pPs = new Ps(&FungiNH4_Enhance, TOTPROFILE, MYCORRHIZA, NOFPROC, DENITRIFICATION, NORMAL);
		Define(pPs, "Fungi NH4 Enhance", 2., "-", "Denitrification > 0|Nitrogen and Carbon > 0|Mycorrhiza Fungal Growth>0", 0., 5.);
		//Why Denitrification>0??? 
		pPs = new Ps(&FungiPO4EnhanceCoef, TOTPROFILE, MYCORRHIZA, NOFPROC, DENITRIFICATION, NORMAL);
		Define(pPs, "FungiPO4EnhanceCoef", 4., "-", "Nitrogen and Carbon > 0|Mycorrhiza Fungal Growth>0|Phosphorus>0", 0., 50.);


;
// Parameters for NN3 volatilisation;
;
;
;
;
	pX= new X(&SoilNitrate ,pNL,SIMPROFILE,AMMONIUM, STORAGE,NOBPROC, NORMAL);
	initv.assign(pSoil->NL,10.);
	Define(pX, pNL ,			"SoilNitrate",initv,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1");
	pX= new X(&SoilAmmonium ,pNL,SIMPROFILE,AMMONIUM, STORAGE,NOBPROC, NORMAL);
		Define(pX, pNL ,			"SoilAmmonium",initv,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1");
	initv.assign(pSoil->NL,0.);
	pT= new T(&SoilNitrateFlow ,pNL,SIMPROFILE,NITRATE, TRANSPORT,NOBPROC, NORMAL);
		Define(pT, pNL ,		"SoilNitrateFlow",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1");
	pT= new T(&SoilNitrateDrainage ,pNL,SIMPROFILE,NITRATE, TRANSPORT,NOBPROC, NORMAL);
		Define(pT, pNL ,	"SoilNitrateDrainage",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|SoilDrainageInput > 0");
	pT= new T(&SoilNitrateInflow ,pNL,SIMPROFILE,NITRATE, TRANSPORT,NOBPROC, NORMAL);
		Define(pT, pNL ,	"SoilNitrateInflow",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Irrigation > 0|Dripper>0");
	pT= new T(&SoilNitrateUptake ,pNL,SIMPROFILE,NITRATE, TRANSPORT,NOBPROC, NORMAL);
		Define(pT, pNL ,	"SoilNitrateUptake",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1");
	
	pT= new T(&SoilAmmoniumFlow ,pNL,SIMPROFILE,NITRATE, TRANSPORT,NOBPROC, NORMAL);
	    Define(pT, pNL ,		"SoilAmmoniumFlow",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Ammonium Mobility > 0");
	pT= new T(&SoilAmmoniumDrainage ,pNL,SIMPROFILE,NITRATE, TRANSPORT,NOBPROC, NORMAL);
		Define(pT, pNL , "SoilAmmoniumDrainage",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|SoilDrainageInput > 0|Ammonium Mobility > 0");
	pT= new T(&SoilAmmoniumUptake ,pNL,SIMPROFILE,NITRATE, TRANSPORT,NOBPROC, NORMAL);
		Define(pT, pNL ,	"SoilAmmoniumUptake",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1");
	pT= new T(&SoilAmmoniumInflow ,pNL,SIMPROFILE,NITRATE, TRANSPORT,NOBPROC, NORMAL);
		Define(pT, pNL , "SoilAmmoniumInflow",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Irrigation >0|Dripper>0");
	
	pT= new T(&DenitrificationRate ,pNL,SIMPROFILE,NITRATE, TRANSPORT,NOBPROC, NORMAL);
	    Define(pT, pNL ,	"DenitrificationRate",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Denitrification > 0");
	pT= new T(&NitrificationRate ,pNL,SIMPROFILE,NITRATE, TRANSPORT,NOBPROC, NORMAL);
		Define(pT, pNL   ,	"NitrificationRate",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1");
;
	pG= new G(&DenitResponse ,pNL,SIMPROFILE,NITRATE, TRANSPORT,NOBPROC, NORMAL);
		Define(pG, pNL , "DenitResponse",initv,"-", "Nitrogen and Carbon >= 1|Denitrification > 0");
;
	pG= new G(&SoilNitrateConc ,pNL,SIMPROFILE,NITRATE, TRANSPORT,NOBPROC, NORMAL);
		Define(pG, pNL , "SoilNitrateConc",initv,"mg-N/l", "Nitrogen and Carbon >= 1");
	pG= new G(&SoilAmmoniumConc ,pNL,SIMPROFILE,AMMONIUM, TRANSPORT,NOBPROC, NORMAL);
		Define(pG, pNL , "SoilAmmoniumConc",initv,"mg-N/l", "Nitrogen and Carbon >= 1");
;
	pGs= new Gs(&NTotMinN_Drainage,TOTPROFILE,NITRATE, TRANSPORT,NOBPROC, NORMAL);
		Define(pGs,  "N Tot MinN Drainage", 0.,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >0");
	pGs= new Gs(&NTotNO3_Drainage,TOTPROFILE,NITRATE, TRANSPORT,NOBPROC, NORMAL);
		Define(pGs,  "N Tot NO3 Drainage", 0.,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >0");
	pGs= new Gs(&NTotNH4_Drainage ,TOTPROFILE,NITRATE, TRANSPORT,NOBPROC, NORMAL);
		Define(pGs, "N Tot NH4 Drainage", 0.,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >0");

	pGs= new Gs(&NTotMinNConc_Drainage ,TOTPROFILE,NITRATE, TRANSPORT,NOBPROC, NORMAL);
		Define(pGs, "N Conc MinN Drainage", 0.,"mg-N/l" , "Nitrogen and Carbon >0");
	pGs= new Gs(&NTotNO3Conc_Drainage ,TOTPROFILE,NITRATE, TRANSPORT,NOBPROC, NORMAL);
		Define(pGs,  "N Conc NO3 Drainage", 0.,"mg-N/l" , "Nitrogen and Carbon >0");
	pGs= new Gs(&NTotNH4Conc_Drainage,TOTPROFILE,AMMONIUM, TRANSPORT,NOBPROC, NORMAL);
		Define(pGs,  "N Conc NH4 Drainage", 0.,"mg-N/l" , "Nitrogen and Carbon >0");
	
	pGs= new Gs(&NTotDenitrification,TOTPROFILE ,NITRATE, TRANSPORT,NOBPROC, NORMAL);
		Define(pGs,  "N Tot Denitrification", 0.,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >0");
	pGs= new Gs(&NTotNitrification,TOTPROFILE ,MINERAL_N, TRANSPORT,NOBPROC, NORMAL);
		Define(pGs,  "N Tot Nitrification", 0.,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >0");
;
	pGs= new Gs(&NTotMin_PlantUptake,TOTPROFILE ,MINERAL_N, TRANSPORT,NOBPROC, NORMAL);
		Define(pGs,  "N Tot Min Plant Uptake", 0.,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >0");
	pGs= new Gs(&NTotNO3_PlantUptake,TOTPROFILE,NITRATE, TRANSPORT,NOBPROC, NORMAL);
		Define(pGs,  "N Tot NO3 Plant Uptake", 0.,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >0");
	pGs= new Gs(&NTotNH4_PlantUptake,TOTPROFILE ,AMMONIUM, TRANSPORT,NOBPROC, NORMAL);
		Define(pGs,  "N Tot NH4 Plant Uptake", 0.,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >0");
;
	pGs= new Gs(&NTotNitrate,TOTPROFILE ,NITRATE, TRANSPORT,NOBPROC, NORMAL);
		Define(pGs, "N Tot Nitrate Soil", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >0");
	pGs= new Gs(&NTotAmmonium,TOTPROFILE ,AMMONIUM, TRANSPORT,NOBPROC, NORMAL);
		Define(pGs, "N Tot Ammonium Soil", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >0");
	pGs= new Gs(&NTotMinN_Soil,TOTPROFILE ,MINERAL_N, TRANSPORT,NOBPROC, NORMAL);
		Define(pGs, "N Tot MinN Soil", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >0");
;
// Microbial based variables
	initv.assign(pSoil->NL, 10.);
	pX= new X(&Nitrifier_Biomass ,pNL,SIMPROFILE,NITRIFIERS, STORAGE,NOBPROC, NORMAL);
		Define(pX, pNL ,	"Nitrifier_Biomass",initv,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Nitrification > 0");
	pT= new T(&Nitrifier_GrowthRate ,pNL,SIMPROFILE,NITRIFIERS, STORAGE,NOBPROC, NORMAL);	
	initv.assign(pSoil->NL, 0.);
	Define(pT, pNL ,	"Nitrifier GrowthRate",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Nitrification > 0");
	pT= new T(&Nitrifier_DeathRate ,pNL,SIMPROFILE,NITRIFIERS, STORAGE,NOBPROC, NORMAL);	
		Define(pT, pNL ,	"Nitrifier DeathRate",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Nitrification > 0");
	pT= new T(&Nitrifier_RespRate ,pNL,SIMPROFILE,NITRIFIERS, STORAGE,NOBPROC, NORMAL);	
		Define(pT, pNL ,	"Nitrifier_RespRate",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Nitrification > 0");
;

// Microbial base Denitrification
	initv.assign(pSoil->NL,25.);
	pX= new X(&Denitrifier_Biomass ,pNL,SIMPROFILE,DENITRIFIERS, STORAGE,NOBPROC, NORMAL);
		Define(pX, pNL ,	"Denitrifier_Biomass",initv,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	initv.assign(pSoil->NL,0.5);
	pX= new X(&Denitrifier_Activity ,pNL,SIMPROFILE,DENITRIFIERS, STORAGE,NOBPROC, NORMAL);
		Define(pX, pNL ,	"Denitrifier_Activity",initv,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
;	initv.assign(pSoil->NL,0.);
	pX= new X(&An_NO3 ,pNL,SIMPROFILE,ANAROBIC_GAS, STORAGE,NOBPROC, NORMAL);
		Define(pX, pNL ,	"An_NO3",initv,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pX= new X(&An_NO2 ,pNL,SIMPROFILE,ANAROBIC_GAS , STORAGE,NOBPROC, NORMAL);
		Define(pX, pNL ,	"An_NO2",initv,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pX= new X(&An_NO ,pNL,SIMPROFILE,ANAROBIC_GAS , STORAGE,NOBPROC, NORMAL);
		Define(pX, pNL ,	"An_NO",initv,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pX= new X(&An_N2O ,pNL,SIMPROFILE,ANAROBIC_GAS , STORAGE,NOBPROC, NORMAL);
		Define(pX, pNL ,	"An_N20",initv,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pX= new X(&An_N2 ,pNL,SIMPROFILE,ANAROBIC_GAS , STORAGE,NOBPROC, NORMAL);
		Define(pX, pNL ,	"An_N2",initv,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pT= new T(&Denitrifier_Growth ,pNL,SIMPROFILE,DENITRIFIERS, STORAGE,DENITRIFICATION, NORMAL);	
	initv.assign(pSoil->NL,0.);
		Define(pT, pNL , "Denitrifier_Growth",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pT= new T(&Denitrifier_Death ,pNL,SIMPROFILE,DENITRIFIERS, STORAGE,DENITRIFICATION, NORMAL);	
		Define(pT, pNL , "Denitrifier_Death",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	initv.assign(pSoil->NL,0.);
	pT= new T(&Denitrifier_ActivityRate ,pNL,SIMPROFILE,DENITRIFIERS, STORAGE,DENITRIFICATION, NORMAL);	
		Define(pT, pNL , "Denitrifier_ActivityRate",initv,"-", "Nitrogen and Carbon >= 1|Denitrification = 2");

	pT= new T(&An_Cons_NO3 ,pNL,SIMPROFILE,DENITRIFIERS, STORAGE,DENITRIFICATION, NORMAL);	
		Define(pT, pNL , "An_Cons_NO3",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pT= new T(&An_Cons_NO2 ,pNL,SIMPROFILE,DENITRIFIERS, STORAGE,DENITRIFICATION, NORMAL);	
		Define(pT, pNL , "An_Cons_NO2",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pT= new T(&An_Cons_NO ,pNL,SIMPROFILE,DENITRIFIERS, STORAGE,DENITRIFICATION, NORMAL);	
		Define(pT, pNL , "An_Cons_NO",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pT= new T(&An_Cons_N2O ,pNL,SIMPROFILE,DENITRIFIERS, STORAGE,DENITRIFICATION, NORMAL);	
		Define(pT, pNL , "An_Cons_N20",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");

	pT= new T(&An_Prod_NO2 ,pNL,SIMPROFILE,DENITRIFIERS, STORAGE,DENITRIFICATION, NORMAL);	
		Define(pT, pNL , "An_Prod_NO2",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pT= new T(&An_Prod_NO ,pNL,SIMPROFILE,DENITRIFIERS, STORAGE,DENITRIFICATION, NORMAL);	
		Define(pT, pNL , "An_Prod_NO",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pT= new T(&An_Prod_N2O ,pNL,SIMPROFILE,DENITRIFIERS, STORAGE,DENITRIFICATION, NORMAL);	
		Define(pT, pNL , "An_Prod_N2O",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pT= new T(&An_Prod_N2 ,pNL,SIMPROFILE,DENITRIFIERS, STORAGE,DENITRIFICATION, NORMAL);	
		Define(pT, pNL , "An_Prod_N2",initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pGs= new Gs(&An_TotCons_NO3,TOTPROFILE ,ANAROBIC_GAS, TRANSPORT,NOBPROC, NORMAL);
	Define(pGs,   "NTot_An_Cons_NO3", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pGs= new Gs(&An_TotCons_NO2,TOTPROFILE ,ANAROBIC_GAS, TRANSPORT,NOBPROC, NORMAL);
		Define(pGs,   "NTot_An_Cons_NO2", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pGs= new Gs(&An_TotCons_NO,TOTPROFILE ,ANAROBIC_GAS, TRANSPORT,NOBPROC, NORMAL);
		Define(pGs,   "NTot_An_Cons_NO", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pGs= new Gs(&An_TotCons_N2O, TOTPROFILE ,ANAROBIC_GAS, TRANSPORT,NOBPROC, NORMAL);
		Define(pGs,   "NTot_An_Cons_N20", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");

	pGs= new Gs(&An_TotProd_NO2,TOTPROFILE ,ANAROBIC_GAS, TRANSPORT,DENITRIFICATION, NORMAL);
		Define(pGs,   "NTot_An_Prod_NO2",0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pGs= new Gs(&An_TotProd_NO,TOTPROFILE ,ANAROBIC_GAS, TRANSPORT,DENITRIFICATION, NORMAL);
		Define(pGs,   "NTot_An_Prod_NO", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pGs= new Gs(&An_TotProd_N2O,TOTPROFILE ,ANAROBIC_GAS, TRANSPORT,DENITRIFICATION, NORMAL);
		Define(pGs,   "NTot_An_Prod_N2O", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pGs= new Gs(&An_TotProd_N2,TOTPROFILE ,ANAROBIC_GAS, TRANSPORT,DENITRIFICATION, NORMAL);
		Define(pGs,   "NTot_An_Prod_N2", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");

	pGs= new Gs(&An_Tot_NO3,TOTPROFILE ,ANAROBIC_GAS, STORAGE,DENITRIFICATION, NORMAL);
		Define(pGs,   "NTot_An_NO3", 0.0,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pGs= new Gs(&An_Tot_NO2, TOTPROFILE ,ANAROBIC_GAS, STORAGE,DENITRIFICATION, NORMAL);
		Define(pGs,  "NTot_An_NO2", 0.0,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pGs= new Gs(&An_Tot_NO,TOTPROFILE ,ANAROBIC_GAS, STORAGE,DENITRIFICATION, NORMAL);
		Define(pGs,   "NTot_An_NO", 0.0,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pGs= new Gs(&An_Tot_N2O,TOTPROFILE ,ANAROBIC_GAS, STORAGE,DENITRIFICATION, NORMAL);
		Define(pGs,   "NTot_An_N20", 0.0,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pGs= new Gs(&An_Tot_N2,TOTPROFILE ,ANAROBIC_GAS, STORAGE,DENITRIFICATION, NORMAL);
	    Define(pGs,   "NTot_An_N2", 0.0,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	initv.assign(pSoil->NL,0.);
	pG= new G(&An_NO3_Conc,pNL,SIMPROFILE,ANAROBIC_GAS, STORAGE,DENITRIFICATION, NORMAL);	
	Define(pG, pNL, 	"An_NO3_Conc",initv,"mg/l", "Nitrogen and Carbon >= 1|Denitrification = 2");
	pG= new G(&An_NO2_Conc,pNL,SIMPROFILE,ANAROBIC_GAS, STORAGE,DENITRIFICATION, NORMAL);	
		Define(pG, pNL, 	"An_NO2_Conc",initv,"mg/l", "Nitrogen and Carbon >= 1|Denitrification = 2");
	pG= new G(&An_NO_Conc,pNL,SIMPROFILE,ANAROBIC_GAS, STORAGE,DENITRIFICATION, NORMAL);	
		Define(pG, pNL, 	"An_NO_Conc",initv,"mg/l", "Nitrogen and Carbon >= 1|Denitrification = 2");
	pG= new G(&An_N2O_Conc,pNL,SIMPROFILE,ANAROBIC_GAS, STORAGE,DENITRIFICATION, NORMAL);	
		Define(pG, pNL, 	"An_N2O_Conc",initv,"mg/l", "Nitrogen and Carbon >= 1|Denitrification = 2");
	pG= new G(&An_N2_Conc,pNL,SIMPROFILE,ANAROBIC_GAS, STORAGE,DENITRIFICATION, NORMAL);	
		Define(pG, pNL, 	"An_N2_Conc",initv,"mg/l", "Nitrogen and Carbon >= 1|Denitrification = 2");
	
	pGs= new Gs(&N_Tot_AnarobicGasStorage ,TOTPROFILE ,ANAROBIC_GAS, STORAGE,DENITRIFICATION, NORMAL);
		Define(pGs,  "NTotGasStorage", 0.0,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pGs= new Gs(&N_Tot_DenitMicrobeStorage,TOTPROFILE ,DENITRIFIERS, STORAGE,DENITRIFICATION, NORMAL);
		Define(pGs,  	"NTotDenitMicrobe", 0.0,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pGs= new Gs(&N_Tot_NitriMicrobeStorage ,TOTPROFILE ,NITRIFIERS, STORAGE,DENITRIFICATION, NORMAL);
		Define(pGs,  	"NTotNitriMicrobe", 0.0,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Nitrification = 1");
	pGs= new Gs(&N_Tot_DenitGrowthRate,TOTPROFILE ,ANAROBIC_GAS, STORAGE,DENITRIFICATION, NORMAL);
		Define(pGs,  	"NTotDenitMicGrowthRate", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pGs= new Gs(&N_Tot_NitriGrowthRate,TOTPROFILE ,ANAROBIC_GAS, STORAGE,DENITRIFICATION, NORMAL);
		Define(pGs,  	"NTotNitriMicGrowthRate", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Nitrification = 1");
	pGs= new Gs(&N_Tot_DenitDeathRate,TOTPROFILE ,ANAROBIC_GAS, STORAGE,DENITRIFICATION, NORMAL);
		Define(pGs,  	"NTotDenitMicDeathRate", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pGs= new Gs(&N_Tot_NitriDeathRate,TOTPROFILE ,ANAROBIC_GAS, STORAGE,DENITRIFICATION, NORMAL);
		Define(pGs,  	"NTotNitriMicDeathRate", 0.0,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Nitrification = 1");


	pGs= new Gs(&N_Tot_DenitGrowth_NO3,TOTPROFILE ,ANAROBIC_GAS, STORAGE,DENITRIFICATION, NORMAL);
	    Define(pGs,	"NTotDenitMicGrowth_NO3", 0.0,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pGs= new Gs(&N_Tot_DenitGrowth_NO2,TOTPROFILE ,ANAROBIC_GAS, STORAGE,DENITRIFICATION, NORMAL);
	    Define(pGs,  	"NTotDenitMicGrowth_NO2", 0.0,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pGs= new Gs(&N_Tot_DenitGrowth_NO,TOTPROFILE ,ANAROBIC_GAS, STORAGE,DENITRIFICATION, NORMAL);
	    Define(pGs,  	"NTotDenitMicGrowth_NO", 0.0,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	pGs= new Gs(&N_Tot_DenitGrowth_N2O,TOTPROFILE ,ANAROBIC_GAS, STORAGE,DENITRIFICATION, NORMAL);
	    Define(pGs,  	"NTotDenitMicGrowth_N2O", 0.0,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon >= 1|Denitrification = 2");
	
	return true;
}


double NC_Mineral_Functions::NitrificationFunction(double NH4, double N03) {

	//Real	NH4, N03

	return Nitr_Specific_Rate*max(0.,(NH4-N03/Nitr_Amm_Ratio));
}

//End Function NitrificationFunction

double NC_Mineral_Functions::NitrifierGrowthFunctionL(size_t Layer) {
	SoilModel* p_Soil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	Soil_Frost* pSoil=&p_Soil->m_SoilFrost;
//	Use NC_Organic,  Only: CDissolved
//	Use CommonStruc, Only: WaterStorage
//	Integer Layer
	double DissConc;
	DissConc=1000*O_State[_DIS][_C_][Layer]/pSoil->WaterStorage[Layer];
	return NitriGrowthCoef*NitriGrowthDOCFunction(DissConc)*NitriGrowthNitrateFunction(SoilNitrateConc[Layer]);

}
//End Function NitrifierGrowthFunctionL

double NC_Mineral_Functions::NitriGrowthDOCFunction(double Value) {
	//Real Value
    if(Value>0) 
	    return Value/(Value+NitriHalfRateDOC);
    else
        return 0.;
}
//End Function NitriGrowthDOCFunction

double NC_Mineral_Functions::NitriGrowthNitrateFunction(double Value) {
//!The Nitrate concentration is in mg/l of N
//	Real Value
    if(Value>0.) 
	    return Value/(Value+NitriHalfRateNO3);
    else
        return 0.;
    
}


double NC_Mineral_Functions::NitrifierDeathFunctionL(size_t Layer) {
//	Use NC_Organic,  Only: O_State[_DIS][_C_]
//	Use CommonStruc, Only: WaterStorage
//	Integer Layer
		SoilModel* p_Soil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	Soil_Frost* pSoil=&p_Soil->m_SoilFrost;
	double DissConc;
	DissConc=1000*O_State[_DIS][_C_][Layer]/pSoil->WaterStorage[Layer];

	return NitriDeathCoef*NitriGrowthDOCFunction(DissConc);
}

//End Function NitrifierDeathFunctionL

double NC_Mineral_Functions::NitrifierRespFunctionL(size_t Layer) {
//Use Nc_Organic, Only	: Eff_Microbes
//Integer Layer

	 return NitriDeathCoef*(1/Eff_Microbes-1);
}
//End Function NitrifierRespFunctionL
double NC_Mineral_Functions::NitrificationNH4FunctionL(size_t Layer) {

	return NitrificationNH4Function(SoilAmmoniumConc[Layer]);
}

//End Function NitrificationNH4FunctionL
double NC_Mineral_Functions::NitrificationNH4Function(double Value) {

    if(Value>0.) 
	    return Value/(NitriHalfRateNH4+Value);
    else
        return 0.0;
}

//End Function NitrificationNH4Function

double NC_Mineral_Functions::DenitriInhibitNO3L(size_t Layer) {
	//Integer Layer
	return DenitriInhibitNO3(An_NO3_Conc[Layer]);

}//End Function DenitriInhibitNO3L
 

double NC_Mineral_Functions::DenitriInhibitNO3(double Value) {
//	Real Value
		return D_InhiHalfRateNO3_N2O/(D_InhiHalfRateNO3_N2O+Value);
	
}

double NC_Mineral_Functions::LiquidConcL(size_t Layer,double Value) {
	SoilModel* p_Soil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	Soil_Frost* pSoil=&p_Soil->m_SoilFrost;
	//Use CommonStruc, Only: WaterStorage
	//Integer Layer
	//Real*8	Value
	return max(1.E-9,1000.*Value/pSoil->WaterStorage[Layer]);

}
//End Function LiquidConcL


double NC_Mineral_Functions::LiquidConcAnFractionL(size_t Layer,double Value) {
	SoilModel* p_Soil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	Soil_Frost* pSoil=&p_Soil->m_SoilFrost;
	//Use CommonStruc, Only: WaterStorage
	//Integer Layer
	//Real*8	Value
    if(An_VolFraction[Layer]>0.001) 
	    return max(0., 1000.*Value/(pSoil->WaterStorage[Layer]*An_VolFraction[Layer]));
    else
        return 0.;
}



double NC_Mineral_Functions::GrowthResponseNO3_L(size_t Layer) {
	//Integer Layer
	return GrowthResponseNXOY(An_NO3_Conc[Layer]);
}
//End Function GrowthResponseNO3_L

double NC_Mineral_Functions::GrowthResponseNO2_L(size_t Layer) {
	//Integer Layer
	return GrowthResponseNXOY(An_NO2_Conc[Layer]);
}
//End Function GrowthResponseNO2_L

double NC_Mineral_Functions::GrowthResponseNO_L(size_t Layer) {
//	Integer Layer
	return GrowthResponseNXOY(An_NO_Conc[Layer]);
}
//End Function GrowthResponseNO_L

double NC_Mineral_Functions::GrowthResponseN2O_L(size_t Layer) {
	//Integer Layer
	return GrowthResponseNXOY(An_N2O_Conc[Layer]);
}


double NC_Mineral_Functions::GrowthResponseDOC_L(size_t Layer) {
	SoilModel* p_Soil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	Soil_Frost* pSoil=&p_Soil->m_SoilFrost;
	//Use NC_Organic,  Only: O_State[_DIS][_C_]
	//Use CommonStruc, Only: WaterStorage
	//Integer Layer
	//Real DissConc
    if(DissolvedOrganics==0) 
        return 1.;
    else if(O_State[_DIS][_C_][Layer]>0.) {
	    double DissConc=1000*O_State[_DIS][_C_][Layer]/pSoil->WaterStorage[Layer];
	    return GrowthResponseDOC(DissConc);
	}
    else
        return 0.;
    
}
//End Function GrowthResponseDOC_L

double NC_Mineral_Functions::GrowthResponseDOC(double Value) {
	//real Value
    if(Value>0) 
	    return Value/(Value+NXOY_DOC_HalfRateCoef);
    else
        return 1.;
 }
//End Function GrowthResponseDOC

double NC_Mineral_Functions::GrowthResponseNXOY(double Value) {
	//Real Value
	if(Value<1.E-6) 
		return 0.;
	else if(Value>0.) 
		return Value/(Value+NXOY_HalfRateCoef);
    else 
		return 0.;
}
//End Function GrowthResponseNXOY

double NC_Mineral_Functions::DMic_pH_ResponseL(size_t Layer) {
	//Integer Layer
	//Real Value
	double value=5.;
	return DMic_pH_Response(value);
}

//End Function DMic_pH_ResponseL

double NC_Mineral_Functions::DMic_pH_Response(double value) {
	//Real Value
	return 1-(1/(1+exp((value-D_pH_HalfCoef)/D_pH_ShapeCoef) ) );
}
//End Function DMic_pH_Response

double NC_Mineral_Functions::DMic_N_ResponseL(size_t Layer) {
	//Integer Layer
	return DMic_N_Response(An_NXOY[Layer]);
}
//End Function DMic_N_ResponseL
double NC_Mineral_Functions::DMic_N_Response(double Value) {
	//Real Value

	if(Value<1.E-6) 
		return 0.;
	else
		return Value/(NXOY_HalfRateCoef+Value);
}

//End Function DMic_N_Response

double NC_Mineral_Functions::DMic_TempFunctionL(size_t Layer) {
	//Integer Layer
	return f_DenitTempResponse(p_NC_Plant->NDrivTemperature[Layer]);
}
//End Function DMic_TempFunctionL
/*
!     SUBROUTINE VOLAT(C,RVOL)
!C
!C======================================================================
!C
!C       PURPOSE:
!C
!C       REF:  CRANE, ET.AL. (1981) TRANS. ASAE 24:382-390.
!C           HOFF, ET.AL. (1981) JOURN OF ENV. QUAL. 10:90-95
!C
!C       VARIABLE DEFINITIONS:
!C       VARIABLE  i/O DESCRIPTION
!C       --------  --- -----------
!C       C      i  13-LONG OMNI STATE VECTOR: 1 -> 5 - STRUCTURAL,
!C                 METABOLIC, FAST, MED, & SLOW SOIL OM (OM UNITS
!C                 [UGC/G-SOIL]); 6 -> 9 - HETEROTROPHIC (SOIL
!C                 DECOMPOSERS), AUTOTROPHIC, & HETEROTROPHIC
!C                 (FACULTATIVE) BM (ALL BM HAVE UNITS
!C                 [# ORG'S/G-SOIL]); UREA, NO#, NH4, & N SINK
!C                 (ALL HAVE UNITS [MOLES/LITER PORE WATER]
!C       CONV   L  CONVERSION FACTOR FOR MG/L ==> UG/GM
!C       EK         P  EQUILIBRIUM CONSTANT
!C       H      i  HYDROGEN ION CONCENTRATION (PH)
!C       HM     L  HYDROGEN ION CONCENTRATION (MG/L)
!C       PNH3   L  PARTIAL PRESSURE NH3 GAS (ATM)
!C       PNH3A  P  PARTIAL PRESSURE NH3-AMBIENT (ATM)
!C       RHOB   i  BULK DENSITY (G/CM^3)
!C       RVOL   O  AMOUNT OF NH3-N VOLATILIZED (MOLES N/L) AT TIME STEP
!C       Theta  i  VOLUMETRIC WATER CONTENT (CM^3/CM^3)
!C       TEMP   i  TEMPERATURE OF LAYER (C)
!C       W      i  WIND RUN (KM/DAY)
!C       XK         P  DEPTH CONSTANT
!C       XK1        P  VOLATILIZATION CONSTANT
!C       XKG        L  EFFECT FROM WIND AND DEPTH
!C       ZN         i  DEPTH TO CENTER OF SOIL LAYER (CM)
!C
!C       COMMENTS:
!C
!C       EXTERNAL REFERENCES:
!C                 NONE
!C
!C       CALLED FROM:  PRCRAT
!C
!C       PROGRAMMER:  KEN ROJAS  MARVIN SHAFFER
!C
!C       VERSION:   1.0
!C
!C-----------------------------------------------------------------------
!C
!      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
!C
!      PARAMETER(XK1=1000.0D0) !LIWANG SUGGESTS -- WAS 4000.0
!      PARAMETER(XK=-0.25D0,PNH3A=2.45D-8,EK=8.7902D-12,TK=2.9447D4,
!     +    TK2=1.99D-3)
!C
!C ... BRING IN PARAMETERS BY LABELED COMMON
!C
!C      COMMON /NUTPAR/ R14,R23,R34,R43,R45,R53,EFFMAX,EFFNIT,
!C     A            EFFDEN,DENC,RPOP(3),ADCAY,ANIT,ADENIT,
!C     B            AUREA,ADEATH(3),CN(9),ADCAYV(5),XKP(3)
!C
!      COMMON /NUTINP/ H,PCO2,RHOB,RHOP,TEMP,U,Theta,W,ZN,PWFPC,
!     +    POPMIN(3),NHBFLG
!C
!C
!      DIMENSION C(20)
!C
!C     ..DETERMINE EFFECT FROM WIND AND DEPTH IN SOIL
!      XKG=XK1*LOG(W)*exp(XK*ZN)
!C
!      HM=10.0D0**(-H)
!C
!C     ..DETERMINE PARTIAL PRESSURE OF NH3
!      PNH3=EK*C(12)/HM
!      PNH3=MAX(PNH3,PNH3A)
!C
!C     ..CALCULATE VOLATIZATION
!      XMNH4=C(12)
!      TF=TK*exp(-6.0D0/(TK2*(TEMP+273.0D0)))
!      PRVOL=XKG*(PNH3-PNH3A)*TF
!      PRVOL=MAX(PRVOL,0.)
!      PRVOL=MIN(PRVOL,1.0D0)
!      RVOL=-XMNH4*PRVOL
!C
!C     WRITE (*,100) XMNH4*14000./RHOB*Theta,W,H,ZN,PRVOL*100.,
!C     1RVOL*14000./RHOB*Theta
!C
!C
!      RETURN
!C     1000 FORMAT (//' STARTING AMOUNT OF NH4 ==>',T30,G14.5,/' WIND ===>',
!C     +    T30,G14.5,/' PH ===>',T30,G14.5,/' ZN ===>',T30,G14.5,/
!C     +    ' PERCENT VOLATILIZED ==>',T30,G14.5,/
!C     +    ' AMOUNT VOLATILIZED ==>',T30,G14.5)
!      END
!C
!C
!C*/

double NC_Mineral_Functions::NH3_VolatL(size_t Layer) {
	//USE Profile, Only	: DepthMidpLayers
	//Use NC_Drivings, only	: NDrivTemperature
	SoilModel* p_Soil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	Soil_Frost* pSoil=&p_Soil->m_SoilFrost;

	return Conduct_NH3(p_NC_Plant->WindSpeed, pSoil->DepthMidpLayer[Layer])* 
		   (SoilPressureNH3(SoilAmmoniumConc[Layer], pSoil->DepthMidpLayer[Layer])-PNH3_Atm)* 
		   Volat_TempConductFunction(p_NC_Plant->NDrivTemperature[Layer]);
}//End Function NH3_VolatL

double NC_Mineral_Functions::Conduct_NH3(double Wind, double Depth) {
	//Real Wind, Depth

	return VolatRough*log(Wind)*exp(VolatDepth*Depth);

}

double NC_Mineral_Functions::Volat_TempConductFunction(double Temp) {
	//Real Temp

	return VolatTempC1*exp(-6.0/(VolatTempC2*(Temp+273.15)));
}


double NC_Mineral_Functions::SoilPressureNH3(double NH4Conc, double  Ph) {
	//Real NH4Conc, Ph

	return VolatEqCoef*NH4Conc/pow(10.0,-Ph);
}
//End Function SoilPressureNH3	


double NC_Mineral_Functions::NUptMaxAvailFrac_Function(double RootDensity, size_t PlantIndex) {
	//real*8 RootDensity ! root density in layer [gC/m3]
	//Integer PlantIndex
	double out;
	if(p_NC_Plant->Root_Density_Influence>0)
		out=NUptMaxAvailFrac*(1-exp(-NUptRootCoefMin*RootDensity));
	else
		out=NUptMaxAvailFrac;

	
	if(p_NC_Plant->FungalGrowth>0) 
	     out= out*  exp(-FungiCoverRedCoef*p_NC_Plant->FungiRootCover[PlantIndex]);
	//   ! NUptMaxAvailFrac_Function=NUptMaxAvailFrac_Function* 0.0 
	 
	
	
	return out;
}
	
//End Function NUptMaxAvailFrac_Function


double NC_Mineral_Functions::FungiNUptMaxAvailFrac_Function(double RootDensity,size_t PlantIndex, bool LAmmonium) {
	//real*8 RootDensity ! root density in layer [gC/m3]
	//Integer PlantIndex
	//Logical LAmmonium
	double out;
	if(p_NC_Plant->Root_Density_Influence>0)
		out=NUptMaxAvailFrac*(1-exp(-NUptRootCoefMin*RootDensity));
	else
		out=NUptMaxAvailFrac;
	

	
	if(p_NC_Plant->FungalGrowth>0) {
	    if(LAmmonium) 
	        out=min(1., out*FungiMinEnhanceCoef*FungiNH4_Enhance);
	    else
	        out=min(1., out*FungiMinEnhanceCoef);
	} 
    else
        out=0.;
	
	return out;
}
double NC_Mineral_Functions::DenitNitrateFunction(double Nitrate) {


	return Nitrate/(Nitrate+DenitNitrateHalfSat);
}

Plant_Uptake  NC_Mineral_Functions::NUptake(vector<double> Dist, size_t NUM, size_t index, double N_Demand, double N_Tot_Demand,  vector<double> Dist_Tot,double factNdemandFungi,double FungiNitrateUptpot,double FungiAmmoniumUptpot,double N_FungiDemand, double N_Tot_FungiDemand) {
	
	SoilModel* p_Soil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	Soil_Frost* pSoil=&p_Soil->m_SoilFrost;	
	Plant_Uptake out;
	double RootDens;

	//Real N_FungiDemand, N_Tot_FungiDemand,FungiNitrateUptpot,FungiAmmoniumUptpot,factNdemandFungi // Inputs
	

	
	double Ratio, SumR_Avail, CompFactorRoots, N_SecondDemand, Add;
	
	SumR_Avail=0.;
	UptNitrate.assign(pSoil->NL,0.);
	UptAmmonium.assign(pSoil->NL,0.);
	if(p_NC_Plant->FungalGrowth>0) {
	    FungiUptNitrate.assign(pSoil->NL,0.);
	    FungiUptAmmonium.assign(pSoil->NL,0.);
	}
//! 2005-11-29: Consistent scaling of Uptake of individual plants with regard to
//!             fraction of total demand in each soil layer

//!	First preliminary uptake
	      
	    //Do II=1, Num
		for(size_t ii=0; ii<NUM; ii++) {

	        // Ratio between Nitrate and Ammonium in actual Layer
	    	Ratio=(SoilNitrate[ii]+1.E-20)/(SoilNitrate[ii]+SoilAmmonium[ii]+1.E-20);
	        Ratio=max(0., min(1., Ratio));
            if(SoilNitrate[ii]<=0)  Ratio=0.;
            
	        // Root density in actual Layer of actual Plant
	        RootDens = Dist[ii]*(p_NC_Plant->P_State[_ROOT][_C_][index]+p_NC_Plant->P_State[_OLD_ROOT][_C_][index])/pSoil->ThicknessLayers[ii];
	        
	        if(Dist_Tot[ii]*N_Tot_Demand>0.0){      // May add demand from fungi
	       
            	   
	            // Nitrate Uptake of actual Plant in actual Layer: 
	            if(NUptMaxAvailFrac_Function(RootDens, index)*SoilNitrate[ii]>=Ratio*Dist_Tot[ii]*N_Tot_Demand) {
			            UptNitrate[ii]=Ratio*Dist[ii]*N_Demand;
			            SumR_Avail=SumR_Avail+Dist[ii]*Ratio;
				}
	            else if(SoilNitrate[ii]>0.) 
			            UptNitrate[ii]=max(0., NUptMaxAvailFrac_Function(RootDens, index)*SoilNitrate[ii]*Dist[ii]*N_Demand/(Dist_Tot[ii]*N_Tot_Demand));
	            
	                       
                // Ammonium Uptake of actual Plant in actual Layer: 
	            if(NUptMaxAvailFrac_Function(RootDens, index)*SoilAmmonium[ii]>=(1-Ratio)*Dist_Tot[ii]*N_Tot_Demand) {
			            UptAmmonium[ii]=(1-Ratio)*Dist[ii]*N_Demand;
			            SumR_Avail=SumR_Avail+Dist[ii]*(1-Ratio);
				}
	            else if(SoilAmmonium[ii]>0) 
			            UptAmmonium[ii]=max(0., NUptMaxAvailFrac_Function(RootDens, index)*SoilAmmonium[ii]*Dist[ii]*N_Demand/(Dist_Tot[ii]*N_Tot_Demand));
                else
                        UptAmmonium[ii]=0.;
	            
	        }
	        
	        //fungal N uptake
	        
	          if(factNdemandFungi>0&&p_NC_Plant->FungalGrowth>0){  
	      //  if(SNGL(Dist_tot[ii])*N_Tot_FungiDemand>0.0&&FungalGrowth>0){    
	 	        if(FungiNUptMaxAvailFrac_Function(RootDens, index,false)*SoilNitrate[ii]>=Ratio*FungiNitrateUptpot) {
			            FungiUptNitrate[ii]=Ratio*Dist[ii]*FungiNitrateUptpot*factNdemandFungi;
			            SumR_Avail=SumR_Avail+Dist[ii]*Ratio;
				}
	            else if(SoilNitrate[ii]>0) 
			            FungiUptNitrate[ii]=max(0., FungiNUptMaxAvailFrac_Function(RootDens, index, false)*SoilNitrate[ii]*Dist[ii]);
	            
	            
	            			      	            
                // Ammonium Uptake of fungi in actual Layer: 
	            if(FungiNUptMaxAvailFrac_Function(RootDens, index,true)*SoilAmmonium[ii]>=(1-Ratio)*FungiAmmoniumUptpot) {
			            FungiUptAmmonium[ii]=(1-Ratio)*Dist[ii]*FungiAmmoniumUptpot*factNdemandFungi;
			            SumR_Avail=SumR_Avail+Dist[ii]*(1-Ratio);
				}
	            else if(SoilAmmonium[ii]>0.) 
			            FungiUptAmmonium[ii]=max(0., FungiNUptMaxAvailFrac_Function(RootDens, index, true )*SoilAmmonium[ii]*Dist[ii]);
	                
	               
	        
	         }
		}
    
    if(SumR_Avail>0.0001)
	    CompFactorRoots=1./SumR_Avail;
	else
	    CompFactorRoots=0.;
	
	
	out.p_uptake=sum(UptAmmonium)+sum(UptNitrate);
	if(Flexible_Uptake==0) 
		N_SecondDemand=(N_Demand-out.p_uptake)*NUptFlexDegree;
	else
		N_SecondDemand=(N_Demand-out.p_uptake)*NUptPlantFlexDegree[index];
	

//	Second compensatory uptake
	if(N_SecondDemand>1.E-3) {
		//Do II=1, Num
		for(size_t ii=0; ii<NUM; ii++) {
		    if(Dist_Tot[ii]*N_Tot_Demand>0.0){
	            // Ratio between Nitrate and Ammonium in actual Layer
		        Ratio=(SoilNitrate[ii]+1.E-20)/(SoilNitrate[ii]+SoilAmmonium[ii]+1.E-20);
		        Ratio=max(0., min(1., Ratio));
        	   	   
	   	        // Root density in actual Layer of actual Plant
	   	        RootDens = Dist[ii]*(p_NC_Plant->P_State[_ROOT][_C_][index]+p_NC_Plant->P_State[_OLD_ROOT][_C_][index])/pSoil->ThicknessLayers[ii];
                            
		        if(NUptMaxAvailFrac_Function(RootDens, index)*SoilNitrate[ii]>=Ratio*Dist_Tot[ii]*N_Tot_Demand) {
			        Add=Ratio*Dist[ii]*N_SecondDemand*CompFactorRoots;
			        UptNitrate[ii]=max(0., min(UptNitrate[ii]+Add, NUptMaxAvailFrac_Function(RootDens, index)*SoilNitrate[ii]*Dist[ii]*N_Demand/(Dist_Tot[ii]*N_Tot_Demand))) ;
		        }

		        if(NUptMaxAvailFrac_Function(RootDens, index)*SoilAmmonium[ii]>=(1-Ratio)*Dist_Tot[ii]*N_Tot_Demand) {
			        Add=(1-Ratio)*Dist[ii]*N_SecondDemand*CompFactorRoots;
			        UptAmmonium[ii]=max(0., min(UptAmmonium[ii]+Add, NUptMaxAvailFrac_Function(RootDens, index)*SoilAmmonium[ii]*Dist[ii]*N_Demand/(Dist_Tot[ii])*N_Tot_Demand));
		        }
		    }
		}
	}

	out.p_uptake=sum(UptAmmonium)+sum(UptNitrate);
	
	if(p_NC_Plant->FungalGrowth>0) 
	    out.fungi_uptake=sum(FungiUptAmmonium)+sum(FungiUptNitrate);

//	Add uptake from present plant to next plant if more than one plant
	//Do II=1, NL
	for(size_t ii=0; ii<NUM;ii++) {
		if(index==0) {
		  if(p_NC_Plant->FungalGrowth>0) {    //add fungal uptake AM 171211
		     SoilNitrateUptake[ii]	=UptNitrate[ii] + FungiUptNitrate[ii];
			 SoilAmmoniumUptake[ii]	=UptAmmonium[ii]+ FungiUptAmmonium[ii];
		  }
		  else 	{
			 SoilNitrateUptake[ii]	=UptNitrate[ii];
			 SoilAmmoniumUptake[ii]	=UptAmmonium[ii];
		  }
		}
		else {
		  if(p_NC_Plant->FungalGrowth>0) {
			 SoilNitrateUptake[ii]	=SoilNitrateUptake[ii]	+UptNitrate[ii] + FungiUptNitrate[ii];
			 SoilAmmoniumUptake[ii]	=SoilAmmoniumUptake[ii]	+UptAmmonium[ii]+ FungiUptAmmonium[ii];
		  }
		  else {
			 SoilNitrateUptake[ii]	=SoilNitrateUptake[ii]	+UptNitrate[ii];
			 SoilAmmoniumUptake[ii]	=SoilAmmoniumUptake[ii]	+UptAmmonium[ii];
		  }
		}
  }
	
	NTotNO3_PlantUptake=sum(SoilNitrateUptake);
	NTotNH4_PlantUptake=sum(SoilAmmoniumUptake);
	
	NTotMin_PlantUptake=NTotNO3_PlantUptake+NTotNH4_PlantUptake;	// including fungi uptake

	return out;
}

void NC_Mineral_Functions::NitrogenUptakeResetForAllSoilLayers()
{

	SoilNitrateUptake.assign(SoilNitrateUptake.size(), 0.);
	SoilAmmoniumUptake.assign(SoilAmmoniumUptake.size(), 0.);
}


	
//End Function FungiNUptMaxAvailFrac_Function
