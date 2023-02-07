#include "P_Mineral_Functions.h"
#include "../PLANT_NC/PlantOrgModel.h"
#include "../SOIL/SoilModel.h"
#include "../SOIL/Soil_Frost.h"
#include "../PLANT/PlantModel.h"
#include "./Functors/NC_Soil_Functors.h"
#include "../../Simulator/Simulator.h"


P_Mineral_Functions::P_Mineral_Functions(void)
{
	m_pModelStructure = nullptr;
	pSoil = nullptr;
}
P_Mineral_Functions::~P_Mineral_Functions(void)
{
}


bool P_Mineral_Functions::Def()
{
	AbioticResponses::Def();
	ModuleNo=36;
	SoilModel* p_Soil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	pSoil=&p_Soil->m_SoilFrost;
	NE *pNL=m_pModelMap->GetNEPointer("Number of Layers");
	PlantModel* pPlant=(PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");

	WaterUptake* p_Plant=&pPlant->m_WaterUptake;
	NE *pNumPlants=m_pModelMap->GetNEPointer("Number of Plants");
	//m_pModelStructure = ((Simulator*)m_pModelInfo)->GetSimStructurePointer();



	Sw* pSw; Ps *pPs; P *pP; X *pX; T *pT; G *pG;  Gs *pGs; Xs *pXs; Ts *pTs;

	pSw = new Sw(&InitialMineral_P, SIMPROFILE, MINERAL_N, NOFPROC, INITB, NORMAL);
	Define(pSw, "Initial Mineral P", 0, "Uniform concentration|Concentration(z)", "Phosphorus > 0");

	pSw = new Sw(&Weath_Temp_Response, SIMPROFILE, PHOSPHORUS, NOFPROC, WEATHERING, NORMAL);
	Define(pSw, "Weath Temp Response", 0, "Common|Q10 whole range|Q10 above threshold|Ratkowsky function|Arrhenius function", "Nitrogen and Carbon > 0|Phosphorus > 0");
	pSw = new Sw(&Flexible_Uptake_P, SIMPROFILE, MINERAL_N, TRANSPORT, PLANT_NUPTAKE, NORMAL);
	Define(pSw, "Flexible Uptake P", 0, "Common for all plants|Specific for each plant", "Nitrogen and Carbon > 0|Phosphorus>0");
//	pSw= new Sw(&AmmoniaVolatilisation,  SIMPROFILE,MINERAL_N, NOFPROC,INITB, NORMAL);
//		Define(pSw, "AmmoniaVolatilisation",0,"off|on","Nitrogen and Carbon > 0");



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

		ps_vec.clear();

	functorprop.FuncName = MyFunc::PH_WEATHERING_RESPONSE; functorprop.Arg = MyArgType::D1;
	funcname = "PH Weathering Response";
	plotpar.X_Label = "pH (-)";
	plotpar.Y_Label = "Response (-)";
	plotpar.X_Min = 3;
	plotpar.X_Max = 10;

	pPs = new Ps(&Weath_pH_Shape, TOTPROFILE, MINERAL_P, NOFPROC, WEATHERING, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "Delta Power Coef pH", 0.27,UNIT_TYPES::NO_UNIT, "Phosphorus > 0", 0.027, 1.); 
	
	pPs = new Ps(&Weath_pH0, TOTPROFILE, MINERAL_P, NOFPROC, WEATHERING, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "pH Optimal", 7,UNIT_TYPES::NO_UNIT, "Phosphorus > 0", 5, 10., 3., 12.); 
	pFunc = new Func(TOTPROFILE, MINERAL_P, NOFPROC, WEATHERING , NORMAL);
	Define(pFunc, functorprop, funcname, ps_vec, plotpar);


	ps_vec.clear();
	functorprop.FuncName = MyFunc::WEATHERINGRATECOEF; functorprop.Arg = MyArgType::D1;
	funcname = "P Weathering Rate Function";
	plotpar.X_Label = "Amount of Phosphorus (g/m2)";
	plotpar.X_Min = 0;
	plotpar.X_MaxPar = "Inital P content";
	plotpar.Y_Label = "Weathering Rate (g/m2/day)";
	pPs = new Ps(&Weath_RateCoef, TOTPROFILE, MINERAL_P, NOFPROC, WEATHERING, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "Weathering Rate Coef", 1.E-6,UNIT_TYPES::NO_UNIT, "Phosphorus > 0", 0., 0.001);

	pFunc = new Func(TOTPROFILE, MINERAL_P, ADSORPTION, NOBPROC, NORMAL);

	Define(pFunc, functorprop, funcname, ps_vec, plotpar);

	ps_vec.clear();
	
	functorprop.FuncName = MyFunc::PO4_ADSORPTION; functorprop.Arg = MyArgType::D1;
	funcname = "LangmuirAdsorption";
	plotpar.X_Label = "Amount of Phosphate (g P/g soil)";
	plotpar.X_Min = 0;
	plotpar.X_MaxPar = "Inital P content";
	plotpar.Y_Label = "Phosphate in solution (g P/m2)";

	pPs = new Ps(&Langmuir_max, TOTPROFILE, MINERAL_P, ADSORPTION , NOBPROC,NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "Langmuir max", 10.,UNIT_TYPES::NO_UNIT, "Phosphorus > 0", 5.E-6, 1.E4, 3., 12.); 

	pPs = new Ps(&Langmuir_c50, TOTPROFILE, MINERAL_P, ADSORPTION,NOBPROC, NORMAL); ps_vec.push_back(pPs);
	Define(pPs, "Langmuir half sat", 1.E-3,UNIT_TYPES::NO_UNIT, "Phosphorus > 0", 0., 10.); 

	pFunc = new Func(TOTPROFILE, MINERAL_P, SURFACERUNOFF, NOBPROC, NORMAL);

	Define(pFunc, functorprop, funcname, ps_vec, plotpar);

	ps_vec.clear();
	functorprop.FuncName = MyFunc::P_SURFACERUNOFF_CONC; functorprop.Arg = MyArgType::D1;
	funcname = "P Conc response to surfacerunoff";
	plotpar.X_Label = "Surfacerunoff flow rate (mm/day)";
	plotpar.X_Min = 0.;
	plotpar.X_Max = 50.;
	plotpar.Y_Label = "P Conc (mg/1)";
	pPs = new Ps(&Psurf_Conc2, SINGLE, MINERAL_P, SURFACERUNOFF, NOBPROC, NORMAL); ps_vec.push_back(pPs);
		Define(pPs, "DeltaConc", 0.07, "mg-P/l", "Phosphorus > 0", 0., 10.); 
	pPs = new Ps(&Psurf_Conc1, SINGLE, MINERAL_P, SURFACERUNOFF, NOBPROC, NORMAL); ps_vec.push_back(pPs);
		Define(pPs, "BaseConc", 0.027, "mg-P/l", "Phosphorus > 0", 0., 10.); 
	pPs = new Ps(&Psurf_ThresFlow, SINGLE, MINERAL_P, SURFACERUNOFF, NOBPROC, NORMAL); ps_vec.push_back(pPs);
		Define(pPs, "Psurf_ThresFlow", 10.,UNIT_TYPES::WATERFLOW_UNIT, "Phosphorus > 0", 0., 100.); 
		
	pFunc = new Func(TOTPROFILE, MINERAL_P, SURFACERUNOFF, NOBPROC, NORMAL);

	Define(pFunc, functorprop, funcname, ps_vec, plotpar);


// Initial conditions 
	pPs = new Ps(&InitPhosphateConc, TOTPROFILE, MINERAL_P, NOFPROC, INIT_P, NORMAL);
		Define(pPs, "InitPhosphateConc", 0.0001, "mg-P/l", "Initial Mineral P = 0|Phosphorus > 0", 0.000001, 100., 0.0001, 50.);
	pPs = new Ps(&InitMineral_P_Content, TOTPROFILE, MINERAL_P, NOFPROC, INIT_P, NORMAL);
		Define(pPs, "Inital P content", 0.001, UNIT_TYPES::MASSDENSITY_UNIT, "Initial Mineral P = 0|Phosphorus > 0", 0.0000001, 0.1, 0.000001, 0.01);

		

	
	vector<double> initv;

	pP=m_pModelMap->GetPPointer("mUpper Depth");
		 mTab_InitConcP.push_back(pP);
	pP=m_pModelMap->GetPPointer("mLower Depth");
		 mTab_InitConcP.push_back(pP);
 
	pP = new P(&InitPhosphateConcZ, pNL, SIMPROFILE, MINERAL_P, NOFPROC, INIT_P, NORMAL); initv.assign(pSoil->NL, 0.0001);
		Define(pP, pNL, "I Labile Phosphate Conc", initv, "mg-P/l", "Initial mineral P = 1|Phosphorus > 0",0., 10.);
		mTab_InitConcP.push_back(pP);
	
	pP = new P(&InitMineral_P_ContentZ, pNL, SIMPROFILE, MINERAL_P, NOFPROC, INIT_P, NORMAL); initv.assign(pSoil->NL, 0.001);
		Define(pP, pNL, "Inital P content", initv, UNIT_TYPES::MASS_RATIO_UNIT , "Initial mineral P = 1|Phosphorus > 0", 0., 10.);
	mTab_InitConcP.push_back(pP);


	pTab = new Tab(&mTab_InitConcP, SIMPROFILE,MINERAL_P, NOFPROC,INIT_P, NORMAL);
		Define(pTab, "Initial mineral-P concentrations",3);

	pXs = new Xs(&PMineral_MassBalanceCheck,SINGLE , PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
	Define(pXs, "P Min Balance Check",0.,UNIT_TYPES::MASS_UNIT, "Phosphorus > 0");


	initv.assign(pSoil->NL,0.);
	pX = new X(&Soil_PO4, pNL, SIMPROFILE, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
	Define(pX, pNL, "Soil Phosphate", initv,UNIT_TYPES::MASS_UNIT, "Phosphorus > 0");
	initv.assign(pSoil->NL, 0.001);
	pX = new X(&Soil_Mineral_P, pNL, SIMPROFILE, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
	Define(pX, pNL, "Soil mineral P", initv,UNIT_TYPES::MASS_UNIT, "Phosphorus > 0");

	pG = new G(&Soil_PO4_Sol, pNL, SIMPROFILE, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
	Define(pG, pNL, "Soil P Solution", initv,UNIT_TYPES::MASS_UNIT, "Phosphorus > 0");
	pG = new G(&Soil_PO4_Conc, pNL, SIMPROFILE, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
	Define(pG, pNL, "Soil P conc", initv, UNIT_TYPES::mMASS_LITER_UNIT, "Phosphorus > 0");
	// Phosphorus added 
	initv.assign(pSoil->NL, 0.);
	pT = new T(&WeatheringRate, pNL, SIMPROFILE, PHOSPHORUS, TRANSPORT, WEATHERING, NORMAL);
	Define(pT, pNL, "Weathering Rate", initv,UNIT_TYPES::MASSFLOW_UNIT, "Phosphorus > 0");

	pT = new T(&SoilPhosphateFlow, pNL, SIMPROFILE, PHOSPHORUS, TRANSPORT, NOBPROC, NORMAL);
		Define(pT, pNL, "SoilPhosphateFlow", initv,UNIT_TYPES::MASSFLOW_UNIT, "Phosphorus > 0");
	pT = new T(&SoilPhosphateDrainage, pNL, SIMPROFILE, PHOSPHORUS, TRANSPORT, NOBPROC, NORMAL);
		Define(pT, pNL, "SoilPhosphateDrainage", initv,UNIT_TYPES::MASSFLOW_UNIT, "SoilDrainageInput > 0|Phosphorus > 0");
	pT = new T(&SoilPhosphateUptake, pNL, SIMPROFILE, PHOSPHORUS, TRANSPORT, NOBPROC, NORMAL);
		Define(pT, pNL, "SoilPhosphateUptake", initv,UNIT_TYPES::MASSFLOW_UNIT, "Phosphorus > 0");
	pTs = new Ts(&PSurfRunoff, SINGLE, PHOSPHORUS, TRANSPORT, NOBPROC, NORMAL);
		Define(pTs, "PSurfaceRunoff", 1.,UNIT_TYPES::MASSFLOW_UNIT, "Phosphorus > 0");
//	pT = new T(&SoilPhosphateInflow, pNL, SIMPROFILE, PHOSPHORUS, TRANSPORT, NOBPROC, NORMAL);
//		Define(pT, pNL, "SoilPhosphateInflow", initv,UNIT_TYPES::MASSFLOW_UNIT, "Nitrogen and Carbon >= 1|Irrigation >0|Dripper>0|Phosphorus > 0");


;
	pGs = new Gs(&PTotPO4_Drainage, TOTPROFILE, PHOSPHORUS, TRANSPORT, NOBPROC, NORMAL);
		Define(pGs, "P Tot PO4 Drainage", 0.,UNIT_TYPES::MASSFLOW_UNIT, "Phosphorus > 0");
	pGs = new Gs(&PTotPO4Conc_Drainage, TOTPROFILE, PHOSPHORUS, TRANSPORT, NOBPROC, NORMAL);
		Define(pGs, "P Conc PO4 Drainage", 0., UNIT_TYPES::mikro_MASS_LITER_UNIT, "Phosphorus > 0");

	pGs = new Gs(&Tot_MinP_DrainageConc, TOTPROFILE, PHOSPHORUS, TRANSPORT, NOBPROC, NORMAL);// Tot MinP conc by HH
		Define(pGs, "Tot MinP Drainage Conc", 0.0, "mg/l", "Nitrogen and Carbon >= 1|Phosphorus>0");

	
	pGs = new Gs(&PTotPO4_PlantUptake, TOTPROFILE, PHOSPHORUS, TRANSPORT, NOBPROC, NORMAL);
		Define(pGs, "P Tot PO4 Plant Uptake", 0.,UNIT_TYPES::MASSFLOW_UNIT, "Phosphorus>0");
;
	pGs = new Gs(&PTot_PSolidMinerals, TOTPROFILE, PHOSPHORUS, TRANSPORT, NOBPROC, NORMAL);
		Define(pGs, "P Tot Solid Pminerals", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus>0");

	pGs = new Gs(&PTotPhosphate_Soil, TOTPROFILE, PHOSPHORUS, TRANSPORT, NOBPROC, NORMAL);
		Define(pGs, "P Tot Phosphate Soil", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus>0");
	pGs = new Gs(&Tot_PWeatheringRate_Soil, TOTPROFILE, PHOSPHORUS, TRANSPORT, WEATHERING, NORMAL);
		Define(pGs, "P Tot Weathering Soil", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus>0");

	return true;
}

double P_Mineral_Functions::Weath_pH_Response(double value) {
	//Real Value
	return pow(10., -Weath_pH_Shape * (Weath_pH0 - value));
}
	//End Function Weath_pH_Response


double P_Mineral_Functions::PUptMaxAvailFrac_Function(double RootDensity, size_t PlantIndex) {
	//real*8 RootDensity ! root density in layer [gC/m3]
	//Integer PlantIndex
	double out;
	if (p_NC_Plant->Root_Density_Influence > 0)
		out = PUptMaxAvailFrac * (1 - exp(-PUptRootCoefMin * RootDensity));
	else
		out = PUptMaxAvailFrac;


	if (p_NC_Plant->FungalGrowth > 0)
		out = out * exp(-FungiCoverRedCoefP * p_NC_Plant->FungiRootCover[PlantIndex]);
	//   ! NUptMaxAvailFrac_Function=NUptMaxAvailFrac_Function* 0.0 



	return out;
}

double P_Mineral_Functions::FungiPUptMaxAvailFrac_Function(double RootDensity, size_t PlantIndex, bool LPhosphate) {
	//real*8 RootDensity ! root density in layer [gC/m3]
	//Integer PlantIndex
	//Logical LAmmonium
	double out;
	if (p_NC_Plant->Root_Density_Influence > 0)
		out = PUptMaxAvailFrac * (1 - exp(-PUptRootCoefMin * RootDensity));
	else
		out = PUptMaxAvailFrac;
	

	if (p_NC_Plant->FungalGrowth > 0) {
		if (LPhosphate)
			out = min(1., out*FungiPEnhanceCoef*FungiPO4_Enhance);
		else
			out = min(1., out*FungiPEnhanceCoef);
	}
	else
		out = 0.;

	return out;
}

Plant_Uptake  P_Mineral_Functions::PUptake(vector<double> Dist, size_t NUM, size_t index, double P_Demand, double P_Tot_Demand, vector<double> Dist_Tot, double factPdemandFungi, double FungiPhosphateUptpot, double P_FungiDemand, double P_Tot_FungiDemand) {

	SoilModel* p_Soil = (SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	Soil_Frost* pSoil = &p_Soil->m_SoilFrost;
	Plant_Uptake out;
	double RootDens;

	//Real N_FungiDemand, N_Tot_FungiDemand,FungiNitrateUptpot,FungiAmmoniumUptpot,factNdemandFungi // Inputs



	double SumR_Avail, CompFactorRoots, P_SecondDemand, Add;

	SumR_Avail = 0.;
	UptPhosphate.assign(pSoil->NL, 0.);
	
	if (p_NC_Plant->FungalGrowth > 0) {
		FungiUptPhosphate.assign(pSoil->NL, 0.);
			}
	//! 2005-11-29: Consistent scaling of Uptake of individual plants with regard to
	//!             fraction of total demand in each soil layer

	//!	First preliminary uptake

			//Do II=1, Num
	for (size_t ii = 0; ii < NUM; ii++) {

	
		// Root density in actual Layer of actual Plant
		RootDens = Dist[ii] * (p_NC_Plant->P_State[_ROOT][_C_][index] + p_NC_Plant->P_State[_OLD_ROOT][_C_][index]) / pSoil->ThicknessLayers[ii];

		if (Dist_Tot[ii] * P_Tot_Demand > 0.0) {      // May add demand from fungi


			// Phosphate Uptake of actual Plant in actual Layer: 
			if (PUptMaxAvailFrac_Function(RootDens, index)*SoilPhosphate[ii] >=  Dist_Tot[ii] * P_Tot_Demand) {
				UptPhosphate[ii] = Dist[ii] * P_Demand;
				SumR_Avail = SumR_Avail + Dist[ii] ;
			}
			else if (SoilPhosphate[ii] > 0.)
				UptPhosphate[ii] = max(0., PUptMaxAvailFrac_Function(RootDens, index)*SoilPhosphate[ii] * Dist[ii] * P_Demand / (Dist_Tot[ii] * P_Tot_Demand));

		}

		//fungal P uptake

		if (factPdemandFungi > 0 && p_NC_Plant->FungalGrowth > 0) {
			//  if(SNGL(Dist_tot[ii])*N_Tot_FungiDemand>0.0&&FungalGrowth>0){    
			if (FungiPUptMaxAvailFrac_Function(RootDens, index, false)*SoilPhosphate[ii] >= FungiPhosphateUptpot) {
				FungiUptPhosphate[ii] = Dist[ii] * FungiPhosphateUptpot*factPdemandFungi;
				SumR_Avail = SumR_Avail + Dist[ii] ;
			}
			else if (SoilNitrate[ii] > 0)
				FungiUptPhosphate[ii] = max(0., FungiPUptMaxAvailFrac_Function(RootDens, index, false)*SoilPhosphate[ii] * Dist[ii]);


		}
	}

	if (SumR_Avail > 0.0001)
		CompFactorRoots = 1. / SumR_Avail;
	else
		CompFactorRoots = 0.;


	out.p_uptake = sum(UptPhosphate);
	if (Flexible_Uptake_P==0) 
		P_SecondDemand = (P_Demand - out.p_uptake)*PUptFlexDegree;
	else
		P_SecondDemand = (P_Demand - out.p_uptake)*PUptPlantFlexDegree[index];


	//	Second compensatory uptake
	if (P_SecondDemand > 1.E-6) { 
		//Do II=1, Num
		for (size_t ii = 0; ii < NUM; ii++) {
			if (Dist_Tot[ii] * P_Tot_Demand > 0.0) {
				
								// Root density in actual Layer of actual Plant
				RootDens = Dist[ii] * (p_NC_Plant->P_State[_ROOT][_C_][index] + p_NC_Plant->P_State[_OLD_ROOT][_C_][index]) / pSoil->ThicknessLayers[ii];

				if (PUptMaxAvailFrac_Function(RootDens, index)*SoilPhosphate[ii] >=  Dist_Tot[ii] * P_Tot_Demand) {
					Add = Dist[ii] * P_SecondDemand*CompFactorRoots;
					UptPhosphate[ii] = max(0., min(UptPhosphate[ii] + Add, PUptMaxAvailFrac_Function(RootDens, index)*SoilPhosphate[ii] * Dist[ii] * P_Demand / (Dist_Tot[ii] * P_Tot_Demand)));
				}

			}
		}
	}

	out.p_uptake = sum(UptPhosphate);

	if (p_NC_Plant->FungalGrowth > 0)
		out.fungi_uptake = sum(FungiUptPhosphate) ;

	//	Add uptake from present plant to next plant if more than one plant
		//Do II=1, NL
	for (size_t ii = 0; ii < NUM; ii++) {
		if (index == 0) {
			if (p_NC_Plant->FungalGrowth > 0) {    
				SoilPhosphateUptake[ii] = UptPhosphate[ii] + FungiUptPhosphate[ii];
			}
			else {
				SoilPhosphateUptake[ii] = UptPhosphate[ii];
			}
		}
		else {
			if (p_NC_Plant->FungalGrowth > 0) {
				SoilPhosphateUptake[ii] = SoilPhosphateUptake[ii] + UptPhosphate[ii] + FungiUptPhosphate[ii];
				
			}
			else {
				SoilPhosphateUptake[ii] = SoilPhosphateUptake[ii] + UptPhosphate[ii];
				
			}
		}
	}

	PTotPO4_PlantUptake = sum(SoilPhosphateUptake);
	
	return out;
}



void P_Mineral_Functions::PhosphorusUptakeResetForAllSoilLayers()
{

	SoilPhosphateUptake.assign(SoilPhosphateUptake.size(), 0.);
}
