#include "NC_GasFlux.h"
#include "../SOIL/SoilModel.h"
#include "../PLANT/PlantModel.h"
#include "../SOIL/Soil_HeatPump.h"
#include "./Functors/NC_GasFluxFunctors.h"
#include "../PLANT_NC/PlantOrgModel.h"
#include "../Atmosphere/AtmModel.h"

						   
NC_GasFlux::NC_GasFlux(void)
{
	ModuleName="Gas processes";
	ModuleNo=32;
	ModuleConditions="";
}

bool NC_GasFlux::Ini()	{
	NC_Organic::Ini();
	SoilModel* p_Soil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	Soil_HeatPump *pSoilHeat=&p_Soil->m_HeatPump;
	AtmModel* p_AtmModel = (AtmModel*)m_pModelInfo->GetModelPtr("Atm Model");
	double* pSoilEvap = &p_AtmModel->m_Irrigation.RaSoilSurf;
	double Rasurf = *pSoilEvap;

	T_Step = *pT_Step;
	NC_GasFlux::IniFunctors();
//	Use Parameters
//Use Hydraulic, Only	: M_Saturation
		for(size_t i=0; i<Init_Oxygen_Conc.size(); i++) {
			
			Oxygen_Conc[i]=Init_Oxygen_Conc[i]/100.;
			CO2_Conc[i]=Init_CO2_Conc[i]; //wenxin
			CO2_FluxSoil[i] = 0.0; //wenxin
			Air_Porosity[i]=max(0.01, p_Soil_WaterF->M_Saturation[i]-p_Soil_WaterF->ThetaTotal[i]);

			
			if (!m_pModelInfo->StartStateValue) {
				Oxygen[i] = Air_Porosity[i]/100. * p_Soil_WaterF->AThicknessLayers[i] *
					f_OxygenDensityFrom_Volume_Temp(Oxygen_Conc[i], pSoilHeat->Temp[i]);				
																									

				CO2Soil[i] = CO2_Conc[i] * Air_Porosity[i] * p_Soil_WaterF->AThicknessLayers[i] *
			 Fix::MOL_RATIO_CO2_AIR*AirDensityTempFunc(pSoilHeat->Temp[i]) / 1.E4; //wenxin density: kg/M-2

			}
			CH4[i]=0.;
	      
		}

		CH4_AccBubbles=CH4_AccPlant=CH4_AccEmission=CH4_AccProduction=CH4_AccOxidation=0.;

		OxygenMassConcAtm = 0.2095*Fix::MOL_RATIO_O2_AIR*AirDensityTempFunc(pSoilHeat->Temp.front());      //kg/m-3
		MethaneMassConcAtm = CH4_CONC_Atm*Fix::MOL_RATIO_CH4_AIR*AirDensityTempFunc(pSoilHeat->Temp.front());
		CO2MassConcAtm = CO2_CONC_Atm*Fix::MOL_RATIO_CO2_AIR*AirDensityTempFunc(pSoilHeat->Temp.front()); 
		
		An_VolFraction.assign(p_Soil_WaterF->NL,0.0001); //need to update
		
		if(Methane_Model_Sw(Methane_Model )> Methane_Model_Sw::off) {
		    
			for(size_t i=0; i<CH4.size(); i++) 
		        CH4[i]=CH4_InitConc*p_Soil_WaterF->WaterStorage[i]*1000.;

		    CH4_InitStorage=sum(CH4);
		    CH4_DiffTotStorage=sum(CH4)-CH4_InitStorage	;
		}

		if(N2O_YearlyFluxes_Sw(N2O_YearlyFluxes)> N2O_YearlyFluxes_Sw::off) {
				DailyValuesN2O.resize(365);
		        DailyValuesN2O.assign(365,0.0);
		        DailyValuesNO.resize(365);
		        DailyValuesNO.assign(365,0.0);
		}

		OxygenGradients.resize(p_Soil_WaterF->NL);
		
		DO2_SoilCoef.resize(p_Soil_WaterF->NL);
		DCO2_SoilCoef.resize(p_Soil_WaterF->NL);  //wenxin

		DO2_ExCoefSoil.resize(p_Soil_WaterF->NL);
		
		DenitGrowth_NO3.resize(p_Soil_WaterF->NL);
		DenitGrowth_NO2.resize(p_Soil_WaterF->NL);
		DenitGrowth_NO.resize(p_Soil_WaterF->NL);
		DenitGrowth_N2O.resize(p_Soil_WaterF->NL);
		DenitUptakeMin.resize(p_Soil_WaterF->NL);
		DenitUptakeOrg.resize(p_Soil_WaterF->NL);
		DenitUptakeFactor.resize(p_Soil_WaterF->NL);
		
		YearShift=false;
		FullYear=false;			



	//p_Soil_WaterF->WaterE
	return true;
}
bool NC_GasFlux::IniFunctors() {
	NC_Organic::IniFunctors();
	
	if (Methane_Model_Sw(Methane_Model) >= Methane_Model_Sw::on) {
		f_MethaneOxidation = Get_D1_Functor(MyFunc::METHANE_OXIDATION);
		f_MethaneOxidationRegulation = Get_D1_Functor(MyFunc::METHANE_OXIDATION_REGULATION_BY_CH4);
		f_PlantRootRespRegulation = Get_D1_Functor(MyFunc::METHANE_PLANTOXIDATION_REGULATION_BY_ROOTRESPIRATION);
		f_MethaneVolatility = Get_D2_Functor(MyFunc::METHANE_VOLATIVITY);
		f_MethaneProduction_TwoLitter = MethaneProduction(CH4_RateCoefRatio, RateCoefLitter1, RateCoefLitter2, RateCoefHumus);
		f_MethaneProduction_OneLitter = MethaneProduction(CH4_RateCoefRatio, RateCoefLitter1, RateCoefLitter2, RateCoefHumus);
		f_Ebullition = Get_D2_Functor(MyFunc::METHANE_EBULLITION);
		f_PlantEmissionbyWaterUptake = MethaneEmissionByTranspiration(CH4_LimitWaterUptake, CH4_WaterPowerCoef, CH4_WaterPlantCoef);
		if(Methane_SoilBoundary_Flux_Sw(Methane_SoilBoundary_Flux)== Methane_SoilBoundary_Flux_Sw::Fixed_Air_Resistance)
			f_MethaneBoundaryLayerFlux = MethaneBoundaryLayerFlux(CH4_CONC_Atm, CH4_ResistanceAir);
		else 
		    f_MethaneDynResBoundaryLayerFlux = MethaneBoundaryLayerFlux(CH4_CONC_Atm, CH4_ResistanceAir, CH4_ResistanceAirLogScale);
		f_MethaneOxidationInPlant = Get_D1_Functor(MyFunc::METHANE_PLANTOXIDATION_OF_WATERUPTAKE);
	}
	if (COS_Model_Sw(COS_Model) >= COS_Model_Sw::OnlyPlant) {
		if (COS_PlantModel_Sw(COS_PlantModel) == COS_PlantModel_Sw::Tranpiration_Base)
			f_COS_Plant = COS_PlantFlux(COS_TranspirationRatio);
		else {
			f_COS_Plant = COS_PlantFlux(COS_PhotoSynthesisRatio);
			f_COS_PlantTransp = COS_PlantFlux(COS_TranspirationRatio);
			f_COS_PlantTranspPot = COS_PlantFlux(COS_TranspirationPotRatio);
			f_COS_PlantTranspPotMax = COS_PlantFlux(COS_TranspirationPotMaxRatio);


		}

		if (COS_Model_Sw(COS_Model) == COS_Model_Sw::Plant_Soil)
			f_COS_Soil = COS_SoilFlux(COS_SoilRespiratonRatio);
	}


	f_AnaerobicFraction = Get_D1_Functor(MyFunc::ANAEROBIC_BALLON);
	f_AnaerobicExchangeEfficiency = Get_D1_Functor(MyFunc::EXCHANGE_EFFICIENY_ANAEROBIC);
	f_DenitrifierAnaerobicFracResponse= Get_D1_Functor(MyFunc::DENIT_ANAEROBIC_FRAC_RESPONSE);
	f_OxygenDiffuctionCoef = Get_D2_Functor(MyFunc::DiFFUSION_SOIL_OXYGEN);
	f_CarbonDiOxideDiffuctionCoef = Get_D2_Functor(MyFunc::DiFFUSION_SOIL_CO2);
	Ps* pP = (Ps*)m_pModelMap->GetPsPointer("AltSimPosition");
	f_OxygenDensityFrom_Volume_Temp = GasDensityTempFunction(GasElement::OXYGEN, pP->GetValue());
	f_OxygenDensityFrom_Temp = GasDensityTempFunction(GasElement::OXYGEN, pP->GetValue());
	f_CarbonDioOxideDensityFrom_Volume_Temp = GasDensityTempFunction(GasElement::CO2, pP->GetValue());
	f_CarbonDiOxideDensityFrom_Temp = GasDensityTempFunction(GasElement::CO2, pP->GetValue());

	f_NitrogenOxideProduction_Temp = Get_D1_Functor(MyFunc::NITRI_NO_PROD_TEMP_RESPONSE);
	f_NO_Production_Moist = Get_D1_Functor(MyFunc::NITRI_NO_PROD_MOIST_RESPONSE);
	f_N2O_Production_Moist = Get_D1_Functor(MyFunc::NITRI_N2O_PROD_MOIST_RESPONSE);
	f_NO_Production_PH = Get_D1_Functor(MyFunc::NITRI_NO_PROD_PH_RESPONSE);
	f_NO_Production = Get_D1_Functor(MyFunc::NITRI_NO_PROD_RESPONSE);
	f_N2O_Production = Get_D1_Functor(MyFunc::NITRI_N2O_PROD_RESPONSE);


	return true;
}
bool NC_GasFlux::Dyn()
{
	NC_Organic::Dyn();
	Flux();
	NC_Mineral::Integ();
	Integ();
	return true;
}

void NC_GasFlux::Flux()
{
	SoilModel* p_Soil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	Soil_HeatPump *pSoilHeat=&p_Soil->m_HeatPump;

	AtmModel* p_AtmModel = (AtmModel*)m_pModelInfo->GetModelPtr("Atm Model");
	double* pSoilEvap = &p_AtmModel->m_Irrigation.RaSoilSurf;
	double RaSurf = max(0.0001, *pSoilEvap);

/*	Use CommonStruc
Use Timeinfo
Use AbioticResponses
Use Nc_Plant, only	: SimRootDepth, NUM_NUPTMAX, CRootsLitter, NRootsLitter
Use Plant, only		: RootDistribution, RFracLow
Use NC_Organic, only	: CRootsSoilLitter, NRootsSoilLitter, LitterPools
Use GenFunctions, Only	:D_oxygen, D_NO
Use General, Only	: InflowScalar, OutFlowScalar, OutFlow,InFlow, Intflow
Use Ifport*/



double	Mean_Air_Porosity, Mean_Temp;
double OxidationRatio;

	for(size_t i=0; i<Init_Oxygen_Conc.size(); i++) {
		Oxygen_Consumption[i]=Fix::MOL_RATIO_O2_C*(p_NC_Plant->CRootsAtmLayer[i]+p_NC_Plant->CFungiAtmLayer[i]+p_NC_Plant->CCoarseRootsAtmLayer[i]
			+O_RespRate[_L1][i]+O_RespRate[_L2][i]+O_RespRate[_H][i]+O_RespRate[_MIC][i]+O_RespRate[_F][i]);   
		
		//Oxygen_Consumption.back() = .0;  //test boundary condition wenxin	  Removed this line excluding oxygen concumption of lowest layer
		
		CO2_Production[i] = Fix::MOL_CO2/Fix::MOL_C*(p_NC_Plant->CRootsAtmLayer[i] + p_NC_Plant->CFungiAtmLayer[i] + p_NC_Plant->CCoarseRootsAtmLayer[i]
			+ O_RespRate[_L1][i] + O_RespRate[_L2][i] + O_RespRate[_H][i] + O_RespRate[_MIC][i] + O_RespRate[_F][i]);

		Air_Porosity[i]=max(0.01,p_Soil_WaterF->M_Saturation[i]-p_Soil_WaterF->ThetaTotal[i]);
	}


	DO2_SoilCoef.front() = f_OxygenDiffuctionCoef(Air_Porosity.front(), pSoilHeat->Temp.front()) ;
	DCO2_SoilCoef.front() = f_CarbonDiOxideDiffuctionCoef(Air_Porosity.front(), pSoilHeat->Temp.front()) ;

	for(size_t i=1; i<Air_Porosity.size()-1; i++) {
		Mean_Air_Porosity=(Air_Porosity[i-1]+Air_Porosity[i])/2.;  // Convert to fraction
		Mean_Temp=(pSoilHeat->Temp[i-1]+pSoilHeat->Temp[i])/2.;
		DO2_SoilCoef[i] = f_OxygenDiffuctionCoef(Mean_Air_Porosity, Mean_Temp) ;
		DCO2_SoilCoef[i] = f_CarbonDiOxideDiffuctionCoef(Mean_Air_Porosity, Mean_Temp) ;
	}
	// Added bottom layer values Dec 1 PEJ
	DO2_SoilCoef.back() = f_OxygenDiffuctionCoef(Air_Porosity.back(), pSoilHeat->Temp.back());
	DCO2_SoilCoef.back() = f_CarbonDiOxideDiffuctionCoef(Air_Porosity.back(), pSoilHeat->Temp.back());


	if(Oxygen_Model_Sw(Oxygen_Model)== Oxygen_Model_Sw::Steady_state) {
		Oxygen_Flux.back()=0.;
		double OxygenConcAtm = Init_Oxygen_Conc[0] / 100.;
		OxygenMassConcAtm=f_OxygenDensityFrom_Volume_Temp(OxygenConcAtm,pSoilHeat->Temp.front());
		for (int i = Oxygen_Flux.size() - 2; i >= 0; i--)
			Oxygen_Flux[i] = Oxygen_Flux[i + 1] + Oxygen_Consumption[i + 1];

		Oxygen_Flux_Infil = Oxygen_Flux.front() + Oxygen_Consumption.front();

		Oxygen_Conc.front() = OxygenConcAtm - (Oxygen_Flux_Infil * p_Soil_WaterF->AThicknessLayers.front() / 2.)
			/ (DO2_SoilCoef.front()* f_OxygenDensityFrom_Volume_Temp(OxygenConcAtm, pSoilHeat->Temp.front()));
		Oxygen.front() = f_OxygenDensityFrom_Volume_Temp(Oxygen_Conc.front(), pSoilHeat->Temp.front()) * p_Soil_WaterF->AThicknessLayers.front()* Air_Porosity.front() / 100.;

			

		for(size_t i=1;i<Air_Porosity.size(); i++) {
			Oxygen_Conc[i]= Oxygen_Conc[i-1]-Oxygen_Flux[i-1]*p_Soil_WaterF->DistanceMidpLayers[i] /(DO2_SoilCoef[i]* f_OxygenDensityFrom_Volume_Temp(OxygenConcAtm, pSoilHeat->Temp[i]));
			Oxygen[i] = f_OxygenDensityFrom_Volume_Temp(Oxygen_Conc[i], pSoilHeat->Temp[i])* p_Soil_WaterF->AThicknessLayers[i]* Air_Porosity[i] / 100.;
		}

		for (size_t i = 0; i < Air_Porosity.size(); i++) {
			Oxygen_Conc[i] = max(0.0, Oxygen_Conc[i]);
			Oxygen[i] = max(0.0, Oxygen[i]);
		}

	}
	else {
		//oxygen dynamic

	 	OutFlow(Oxygen, Oxygen_Consumption);

		for (size_t i = 0; i < Oxygen_Conc.size(); i++) {
			Oxygen_Conc[i] = Oxygen[i] / (Fix::MOL_RATIO_O2_AIR*AirDensityTempFunc(pSoilHeat->Temp[i]));
			DO2_SoilCoef[i] = f_OxygenDiffuctionCoef( Air_Porosity[i], pSoilHeat->Temp[i]);//wenxin
		}
		
		OxygenMassConcAtm = 0.2095*Fix::MOL_RATIO_O2_AIR*AirDensityTempFunc(pSoilHeat->Temp.front());
		Oxygen_Flux_Infil=DO2_SoilCoef.front()*(OxygenMassConcAtm-Oxygen_Conc.front())/p_Soil_WaterF->AThicknessLayers.front()/2.;
		//Do i=2, NL,1
		for(size_t i=1; i<Oxygen_Flux.size(); i++) 
			Oxygen_Flux[i-1]=DO2_SoilCoef[i]*(Oxygen_Conc[i-1]-Oxygen_Conc[i])/p_Soil_WaterF->DistanceMidpLayers[i-1];
		

		InFlow(Oxygen.front(), Oxygen_Flux_Infil);
		IntFlow(Oxygen, Oxygen_Flux);

	}

	//added by Wenxin

	if (CO2_SoilModel_Sw(CO2_SoilModel) == CO2_SoilModel_Sw::Steady_state) {
		CO2_FluxSoil.back() = 0.;
		CO2MassConcAtm = f_CarbonDioOxideDensityFrom_Volume_Temp(CO2_CONC_Atm,pSoilHeat->Temp.front());

		for (int i = CO2_FluxSoil.size() - 2; i >= 0; i--)
			CO2_FluxSoil[i] = CO2_FluxSoil[i + 1] + CO2_Production[i + 1];

		CO2_Flux_Out = CO2_FluxSoil.front() + CO2_Production.front();

		CO2_Conc.front() = CO2_CONC_Atm+(CO2_Flux_Out * p_Soil_WaterF->AThicknessLayers.front() / 2.) /
			(DO2_SoilCoef.front()* f_CarbonDioOxideDensityFrom_Volume_Temp(CO2_CONC_Atm, pSoilHeat->Temp.front())); //Mass Per volume of soil.

		CO2Soil.front() =CO2_Conc.front()*f_CarbonDioOxideDensityFrom_Volume_Temp(CO2_Conc.front(), pSoilHeat->Temp.front()) * p_Soil_WaterF->AThicknessLayers.front() * Air_Porosity.front() / 100.;//Per volume of Air filled soil

		//Do i=2,NL,1
		for (size_t i = 1; i<Air_Porosity.size(); i++) {
			CO2_Conc[i] = CO2_Conc[i - 1] + (CO2_FluxSoil[i - 1]*p_Soil_WaterF->DistanceMidpLayers[i]) / (
				DCO2_SoilCoef[i]* f_CarbonDioOxideDensityFrom_Volume_Temp(CO2_Conc[i], pSoilHeat->Temp[i])); //Per volume soil air.
			CO2Soil[i] = CO2_Conc[i]*f_CarbonDioOxideDensityFrom_Volume_Temp(CO2_Conc[i], pSoilHeat->Temp[i])* p_Soil_WaterF->AThicknessLayers[i] * Air_Porosity[i] / 100.; //Per volume of CO2 of soil
		}
		for (size_t i = 0; i < Air_Porosity.size(); i++) {
			CO2_Conc[i] = max(0.0, min(1.E5, 1.E6*CO2_Conc[i]));
			CO2Soil[i] = max(0.0, CO2Soil[i]);

		}

	}
	else if (CO2_SoilModel_Sw(CO2_SoilModel) == CO2_SoilModel_Sw::Dynamic){

		OutFlow(CO2Soil, CO2_Production);

		for (size_t i = 0; i < CO2_Conc.size(); i++) {
			CO2_Conc[i] = CO2Soil[i] / (Fix::MOL_RATIO_CO2_AIR*AirDensityTempFunc(pSoilHeat->Temp[i]));
			DCO2_SoilCoef[i] = D_CO2(DCO2_Tortuosity, Air_Porosity[i], pSoilHeat->Temp[i]);//wenxin
		}
		CO2MassConcAtm = CO2_CONC_Atm*Fix::MOL_RATIO_CO2_AIR*AirDensityTempFunc(pSoilHeat->Temp.front());
		CO2_Flux_Out = DCO2_SoilCoef.front()*(CO2MassConcAtm - CO2_Conc.front()) / p_Soil_WaterF->AThicknessLayers.front() / 2.;
		for (size_t i = 1; i<CO2_FluxSoil.size(); i++)
			CO2_FluxSoil[i - 1] = DCO2_SoilCoef[i] * (CO2_Conc[i - 1] - CO2_Conc[i]) / p_Soil_WaterF->DistanceMidpLayers[i - 1];
 
		InFlow(CO2Soil.front(), CO2_Flux_Out);
		IntFlow(CO2Soil, CO2_FluxSoil);
  	}
	

//////////////////////////////////////////////////////////////////////



	for(size_t i=0; i<Oxygen_Conc.size();i++) {
	// New unit of oxygen conc as volume/volume instead of Kg/m3
	//	Oxygen_Conc[i]=Oxygen_Conc[i]/Fix::MOL_RATIO_O2_AIR/AirDensityTempFunc(pSoilHeat->Temp[i]); //Wenxin 
  		An_VolFraction[i] =f_AnaerobicFraction(Oxygen_Conc[i]);
	}


	if(Trace_Gas_Emissions_Sw(Trace_Gas_Emissions)== Trace_Gas_Emissions_Sw::direct_loos) {
		NO_Rate=N2O_Rate=N2_Rate=0.;
		for(size_t i=0; i<NitrificationRate.size();i++) {
			double relsat = p_NC_Plant->NDrivTheta[i] / p_Soil_WaterF->M_Saturation[i];

			NO_NitriFlux[i] = f_NO_Production(NitrificationRate[i]) * f_NO_Production_Moist(relsat) * f_NitrogenOxideProduction_Temp(p_NC_Plant->NDrivTemperature[i]) * f_NO_Production_PH(pH_Value[i]);
			N2O_NitriFlux[i] = f_N2O_Production(NitrificationRate[i]) * f_N2O_Production_Moist(relsat) * f_NitrogenOxideProduction_Temp(p_NC_Plant->NDrivTemperature[i]);

			N2O_Rate=N2O_Rate+N2O_NitriFlux[i];
			NO_Rate=NO_Rate+NO_NitriFlux[i];

		 //  Exchange with anaerobic sites

			DO2_ExCoefSoil[i] = f_AnaerobicExchangeEfficiency(relsat) * DO2_SoilCoef[i];
			


			NO_Exchange[i]	=	DO2_ExCoefSoil[i]*An_NO[i];
			
			if(NO_Exchange[i]>0.) 
                RatioNO[i]=min(1., max(0.,  An_NO[i]/((NO_Exchange[i]+An_Cons_NO[i])*T_Step)));
            else
                RatioNO[i]=0.;
 			
			N2O_Exchange[i]	=	DO2_ExCoefSoil[i]*An_N2O[i];
			if(N2O_Exchange[i]>0.) 
               RatioN2O[i]=min(1.,max(0.,An_N2O[i]/((N2O_Exchange[i]+An_Cons_N2O[i])*T_Step)));
            else
                RatioN2O[i]=0.;
	
			N2_Exchange[i]	=	min(DO2_ExCoefSoil[i]*An_N2[i], max(0., An_N2[i]/T_Step) );
		}
		
		bool test=DenitrifierUpdate(); 			
			
		for(size_t i=0; i<NO_Exchange.size(); i++) {
					NO_Rate			+=NO_Exchange[i];
					N2O_Rate		+=N2O_Exchange[i];
					N2_Rate			+=N2_Exchange[i];
		}
			
		An_Tot_NO3=sum(An_NO3);
		An_Tot_NO2=sum(An_NO2);
		An_Tot_NO=sum(An_NO);
		An_Tot_N2O=sum(An_N2O);
		An_Tot_N2=sum(An_N2);
		
		An_TotCons_NO3=sum(An_Cons_NO3);
		An_TotCons_NO2=sum(An_Cons_NO2);
		An_TotCons_NO=sum(An_Cons_NO);
		An_TotCons_N2O=sum(An_Cons_N2O);
		
		An_TotProd_NO2=sum(An_Prod_NO2);
		An_TotProd_NO=sum(An_Prod_NO);
		An_TotProd_N2O=sum(An_Prod_N2O);
		An_TotProd_N2=sum(An_Prod_N2);
	   	
	   	N_Tot_AnarobicGasStorage=An_Tot_NO2+An_Tot_NO+ An_Tot_N2O+ An_Tot_N2;
	   	
	   	N_Tot_DenitMicrobeStorage=sum(Denitrifier_Biomass);
	   	
	   	N_Tot_DenitDeathRate=sum(Denitrifier_Death);
   	    N_Tot_DenitGrowthRate=sum(Denitrifier_Growth);
		 		
		NTotDenitrification=NO_Rate+N2O_Rate+N2_Rate;
		
		NO_TotExchange=sum(NO_Exchange);
		N2O_TotExchange=sum(N2O_Exchange);
		N2_TotExchange=sum(N2_Exchange);

		OutFlow(An_NO, NO_Exchange);
		OutFlow(An_N2O, N2O_Exchange);
		OutFlow(An_N2,N2_Exchange);
		
 		OutFlow(SoilNitrate, NO_NitriFlux);
		OutFlow(SoilNitrate, N2O_NitriFlux);
	}
	else if(Trace_Gas_Emissions_Sw(Trace_Gas_Emissions)== Trace_Gas_Emissions_Sw::Vertical_fluxes_in_soil) {

		 //  Flux at soil surface
		NO_Rate	=DO2_SoilCoef.front()*NO.front()	/p_Soil_WaterF->AThicknessLayers.front()/2.;
		N2O_Rate=DO2_SoilCoef.front()*N2O.front()	/p_Soil_WaterF->AThicknessLayers.front()/2.;
		N2_Rate	=DO2_SoilCoef.front()*N2.front()	/p_Soil_WaterF->AThicknessLayers.front()/2.;
		NTotDenitrification=NO_Rate+N2O_Rate+N2_Rate;

		for(size_t i=0; i<NO_NitriFlux.size();i++) {
		 //	Production by Nitrification
			double relsat = p_NC_Plant->NDrivTheta[i] / p_Soil_WaterF->M_Saturation[i];

			NO_NitriFlux[i] = f_NO_Production(NitrificationRate[i]) * f_NO_Production_Moist(relsat) * f_NitrogenOxideProduction_Temp(p_NC_Plant->NDrivTemperature[i]) * f_NO_Production_PH(pH_Value[i]);
			N2O_NitriFlux[i] = f_N2O_Production(NitrificationRate[i]) * f_N2O_Production_Moist(relsat) * f_NitrogenOxideProduction_Temp(p_NC_Plant->NDrivTemperature[i]);

			DO2_ExCoefSoil[i] = f_AnaerobicExchangeEfficiency(relsat) * DO2_SoilCoef[i];

			NO_Exchange[i]	= -DO2_ExCoefSoil[i]*(NO[i]-An_NO[i]);
			N2O_Exchange[i]	= -DO2_ExCoefSoil[i]*(N2O[i]-An_N2O[i]);
			N2_Exchange[i]	= -DO2_ExCoefSoil[i]*(N2[i]-An_N2[i]);
			
			if(NO_Exchange[i]>0.) {
                RatioNO[i]=min(1., max(0.,  An_NO[i]/((NO_Exchange[i]+An_Cons_NO[i])*T_Step)));
		        NO_Exchange[i]=RatioNO[i]*NO_Exchange[i];
		        An_Cons_NO[i]=RatioNO[i]*An_Cons_NO[i];
		        An_Prod_N2O[i]=RatioNO[i]*An_Prod_N2O[i];
			}
			else {
			   	NO_Exchange[i]=0.0;
			    An_Cons_NO[i]=0.0;
			    An_Prod_N2O[i]=0.0;
			} 

			if(N2O_Exchange[i]>0.) {
                RatioN2O[i]=min(1.,max(0.,An_N2O[i]/((N2O_Exchange[i]+An_Cons_N2O[i])*T_Step)));
		        N2O_Exchange[i]=RatioN2O[i]*N2O_Exchange[i];
		        An_Cons_N2O[i]=RatioN2O[i]*An_Cons_N2O[i];
		        An_Prod_N2[i]=RatioN2O[i]*An_Prod_N2[i];
			}
			else {
			    N2O_Exchange[i]=0.;
			    An_Cons_N2O[i]=0.;
			    An_Prod_N2[i]=0.;
			} 
			
			if(i>=1) {
		 //  Vertical Flux between Layers
				NO_SoilFlux[i-1]	= -DO2_SoilCoef[i]*(NO[i]-NO[i-1])/p_Soil_WaterF->DistanceMidpLayers[i];
				N2O_SoilFlux[i-1]	= -DO2_SoilCoef[i]*(N2O[i]-N2O[i-1])/p_Soil_WaterF->DistanceMidpLayers[i];
				N2_SoilFlux[i-1]	= -DO2_SoilCoef[i]*(N2[i]-N2[i-1])/p_Soil_WaterF->DistanceMidpLayers[i];
			}
			
			
			
		}
		 
		bool test=DenitrifierUpdate();

		for(size_t i=0; i<NO_Exchange.size();i++) {
					NO_Rate	+=NO_Exchange[i];
					N2O_Rate+=N2O_Exchange[i];
					N2_Rate	+=N2_Exchange[i];
		}
		 
		 
		An_Tot_NO3=sum(An_NO3);
		An_Tot_NO2=sum(An_NO2);
		An_Tot_NO=sum(An_NO);
		An_Tot_N2O=sum(An_N2O);
		An_Tot_N2=sum(An_N2);
		
		An_TotCons_NO3=sum(An_Cons_NO3);
		An_TotCons_NO2=sum(An_Cons_NO2);
		An_TotCons_NO=sum(An_Cons_NO);
		An_TotCons_N2O=sum(An_Cons_N2O);
		
		An_TotProd_NO2=sum(An_Prod_NO2);
		An_TotProd_NO=sum(An_Prod_NO);
		An_TotProd_N2O=sum(An_Prod_N2O);
		An_TotProd_N2=sum(An_Prod_N2);
	   	
	   	N_Tot_AnarobicGasStorage=An_Tot_NO2+An_Tot_NO+ An_Tot_N2O+ An_Tot_N2;
	   	
	   	N_Tot_DenitMicrobeStorage=sum(Denitrifier_Biomass);

		 OutFlow(NO.front()	, NO_Rate);
		 OutFlow(N2O.front()	, N2O_Rate);
		 OutFlow(N2.front()	, N2_Rate);

		 InFlow(NO, NO_NitriFlux);
		 InFlow(N2O, N2O_NitriFlux);
		 OutFlow(SoilNitrate, NO_NitriFlux);
		 OutFlow(SoilNitrate, N2O_NitriFlux);

		 IntFlow(NO, NO_SoilFlux);
		 IntFlow(N2O, N2O_SoilFlux);
		 IntFlow(N2, N2_SoilFlux);

		 InFlow(NO, NO_Exchange);
		 InFlow(N2O, N2O_Exchange);
		 InFlow(N2, N2_Exchange);

		 OutFlow(An_NO, NO_Exchange);
		 OutFlow(An_N2O, N2O_Exchange);
		 OutFlow(An_N2, N2_Exchange);

	}
	
	if(Trace_Gas_Emissions_Sw(Trace_Gas_Emissions )> Trace_Gas_Emissions_Sw::off) {
		if(DenitrifierGrowth==1) 		
    	    OutFlow(O_State[_DIS][_N_], Denitrifier_Growth);	
		else if(DenitrifierGrowth==2) {
            if(DenitUptakeMin.front()>=0.) 
		      OutFlow(SoilAmmonium, DenitUptakeMin);
            else
                DenitUptakeMin.front()=0.;
            
		    OutFlow(O_State[_DIS][_N_], DenitUptakeOrg);			
        } 
        
   }     
	
	
	if(Methane_Model_Sw(Methane_Model )> Methane_Model_Sw::off) {
	 	CH4_DiffTotStorage=sum(CH4)-CH4_InitStorage;
        CH4_MassBalanceCheck=CH4_AccProduction-CH4_AccEmission-CH4_DiffTotStorage-CH4_AccOxidation;
	    //Do i=1, NL
		for(size_t i=0; i<CH4.size(); i++) {
		    CH4_Conc_Water[i]=0.001*(CH4[i]/p_Soil_WaterF->WaterStorage[i]);  ////// Unitless  g/m2/(g/m2)
  			CH4_CONC_AIR[i] = f_MethaneVolatility(CH4_Conc_Water[i], p_NC_Plant->NDrivTemperature[i]);
			if (LitterPools > 0)
				CH4_Prod[i]=f_MethaneProduction_TwoLitter(O_State[_L1][_C_][i], O_State[_L2][_C_][i], O_State[_H][_C_][i])* f_CommonTempResponse(p_NC_Plant->NDrivTemperature[i])* An_VolFraction[i];
			else 
				CH4_Prod[i]=f_MethaneProduction_OneLitter(O_State[_L1][_C_][i], O_State[_H][_C_][i])* f_CommonTempResponse(p_NC_Plant->NDrivTemperature[i])* An_VolFraction[i];

			if (Methane_Soil_Oxidation_Sw(Methane_Soil_Oxidation) == Methane_Soil_Oxidation_Sw::off)
				CH4_OxidationSoilBacteria[i] = 0;
			else if (Methane_Soil_Oxidation_Sw(Methane_Soil_Oxidation) == Methane_Soil_Oxidation_Sw::on_orignal) 
				CH4_OxidationSoilBacteria[i] = f_MethaneOxidation(CH4[i]) * f_MethaneOxidationRegulation(CH4_Conc_Water[i]) * (1 - An_VolFraction[i]);
			else 
				CH4_OxidationSoilBacteria[i] = f_MethaneOxidation(CH4[i]) * f_MethaneOxidationRegulation(CH4_CONC_AIR[i]) * f_CommonTempResponse(p_NC_Plant->NDrivTemperature[i])* (1 - An_VolFraction[i]);;

			if (Methane_Plant_Oxidation_Sw(Methane_Plant_Oxidation) > Methane_Plant_Oxidation_Sw::off) {
				double RespRate = p_NC_Plant->CRootsAtmLayer[i] + p_NC_Plant->CCoarseRootsAtmLayer[i];
				CH4_OxidationPlantRoot[i] = f_PlantRootRespRegulation(RespRate);
			}
            else
                CH4_OxidationPlantRoot[i]=0.;
   
	        CH4_Oxidation[i]=CH4_OxidationSoilBacteria[i]+CH4_OxidationPlantRoot[i];
	        if(CH4_Oxidation[i]>CH4[i]/T_Step&&CH4_Oxidation[i]>0.) {
	            OxidationRatio=(CH4[i]/T_Step)/CH4_Oxidation[i];
	            CH4_Oxidation[i]=OxidationRatio*CH4_Oxidation[i];
	            CH4_OxidationSoilBacteria[i]=OxidationRatio*CH4_OxidationSoilBacteria[i];
	            CH4_OxidationPlantRoot[i]=OxidationRatio*CH4_OxidationPlantRoot[i];
	        }
		    ////Ebullition
			if (p_NC_Plant->NDrivTemperature[i] > 0.)
				CH4_Ebullition[i] = f_Ebullition(CH4_Conc_Water[i], p_Soil_WaterF->WaterStorage[i]);
			else
				CH4_Ebullition[i] = 0.;
				//f_Ebullition(CH4_Conc_Water[i], p_Soil_WaterF->WaterStorage[i])* f_CommonTempResponse(p_NC_Plant->NDrivTemperature[i]);
		    ////Plants effect
			if (Methane_Plant_Emission_Sw(Methane_Plant_Emission) > Methane_Plant_Emission_Sw::off) 
				CH4_PlantDiffusion[i] = f_PlantEmissionbyWaterUptake(((WaterUptake*)pPlant)->WUptakeRate[i], CH4_Conc_Water[i]);
            else
        	    CH4_PlantDiffusion[i]=0.;	    	    
	        
		}
        
        // We might get negative values on the CH4 pools if any of the fluxes are too large...numerical problems
        
	    InFlow(CH4, CH4_Prod);	
        OutFlow(CH4,CH4_Oxidation);
        OutFlow(CH4, CH4_Ebullition);
        OutFlow(CH4, CH4_PlantDiffusion);
	    
	   for(size_t i=0; i<CH4_PlantDiffusion.size(); i++) {
	       	InFlow(CH4_AccBubbles, CH4_Ebullition[i]);
        	InFlow(CH4_AccPlant, CH4_PlantDiffusion[i]);	
       
		    CH4_SoilEmission[i]=CH4_Ebullition[i];

		 if(i>=1) {
            	   	CH4_SoilFlux[i-1]=-1.E6*CH4_Water_Diff_Coef*(CH4_Conc_Water[i]-CH4_Conc_Water[i-1])/p_Soil_WaterF->DistanceMidpLayers[i];
            	   	 if(p_Soil_WaterF->WaterFlow[i-1]>0) 
            	   	    CH4_SoilFlux[i-1]=CH4_SoilFlux[i-1]+p_Soil_WaterF->WaterFlow[i-1]*CH4_Conc_Water[i-1];
            	   	 else          
            	   	     CH4_SoilFlux[i-1]=CH4_SoilFlux[i-1]+p_Soil_WaterF->WaterFlow[i-1]*CH4_Conc_Water[i];
            	     
                     CH4_AirSoilFlux[i-1]=-DO2_SoilCoef[i]*(CH4_CONC_AIR[i]-CH4_CONC_AIR[i-1])/p_Soil_WaterF->DistanceMidpLayers[i]; 
                if(CH4_AirSoilFlux[i-1]<0) {
                    CH4_AirSoilFlux[i-1]=CH4_AirSoilFlux[i-1]*CH4_CONC_AIR[i];
                    CH4_SoilEmission[i]=CH4_SoilEmission[i]+CH4_AirSoilFlux[i-1];
					if(CH4_AirSoilFlux[i-1]>0)
						OutFlow(CH4[i], CH4_AirSoilFlux[i-1]);
					else
						InFlow(CH4[i],CH4_AirSoilFlux[i-1]);
				}
                else     
                    CH4_AirSoilFlux[i-1]=CH4_AirSoilFlux[i-1]*CH4_CONC_AIR[i-1];
                       	     
          }
          
          InFlow(CH4_AccEmission, CH4_SoilEmission[i]);
          InFlow(CH4_AccProduction, CH4_Prod[i]);
	   }
  	   
  
  	   //Uppermost layer

	   if (Methane_SoilBoundary_Flux_Sw(Methane_SoilBoundary_Flux) == Methane_SoilBoundary_Flux_Sw::Fixed_Air_Resistance)
			CH4_SurfaceEmission = f_MethaneBoundaryLayerFlux(CH4_CONC_AIR[0], pSoilHeat->Temp.front());
	   else
			CH4_SurfaceEmission = f_MethaneDynResBoundaryLayerFlux(CH4_CONC_AIR[0], pSoilHeat->Temp.front(), RaSurf);

		//RAFunction(RefHeight, RoughLBareSoilMom, ) Check which RA function to apply assuming a fixed aerodynamic resistance of 30
   
   
   	   CH4_TotSoilFlux=sum(CH4_SoilEmission)+CH4_SurfaceEmission;
   	   CH4_TotPlantUptake=sum(CH4_PlantDiffusion);
   	   CH4_TotPlantOxidation=f_MethaneOxidationInPlant(CH4_TotPlantUptake)+sum(CH4_OxidationPlantRoot);
	   CH4_TotSoilOxidation = sum(CH4_OxidationSoilBacteria);
   	   CH4_TotPlantEmission=CH4_TotPlantUptake- f_MethaneOxidationInPlant(CH4_TotPlantUptake);
   	   CH4_TotSysFlux=CH4_TotSoilFlux+CH4_TotPlantEmission;
   	   
  	   InFlow(CH4_AccEmission, CH4_SurfaceEmission);
  	   
  	   for(size_t i=0; i<CH4_Oxidation.size(); i++)
  	        InFlow(CH4_AccOxidation, CH4_Oxidation[i]);
 
  	   OutFlow(CH4.front(), CH4_SurfaceEmission);
	   IntFlow(CH4, CH4_SoilFlux);
	
	}


	if (COS_Model_Sw(COS_Model) >= COS_Model_Sw::OnlyPlant) {
		PlantOrgModel* pPlantOrg = (PlantOrgModel*)m_pModelInfo->GetModelPtr("Plant Org Model");
		NC_Plant* pNCPlant;
		pNCPlant = &pPlantOrg->m_NC_Plant;

		PlantModel* pPlant = (PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
		WaterUptake* p_Plant = &pPlant->m_WaterUptake;

		AtmModel* pAtm = (AtmModel*)m_pModelInfo->GetModelPtr("Atm Model");
		Irrigation* p_Irrig = &pAtm->m_Irrigation;


		if (COS_PlantModel_Sw(COS_PlantModel) == COS_PlantModel_Sw::Experimental) {
			COS_TotPlantUptake = f_COS_Plant(sum(pNCPlant->AtmNewMobile));
			COS_Option1 = f_COS_PlantTransp(p_Plant->Transpiration_All);
			COS_Option2 = f_COS_PlantTranspPot(p_Plant->MC_PotTransp[0]);
			COS_Option3 = f_COS_PlantTranspPotMax(p_Plant->MC_PotTranspMax[0]);

			COS_Option4 = COS_CanopyResistCoef*(1./( p_Plant->MC_ResSurfVar[0]*COS_HalfCondCanopy+1));
			COS_Option5 = COS_AirResistCoef*(1. /(p_Plant->MC_ResAir[0]*COS_HalfCondAir+1));
			COS_Option6 = COS_RadScaleCoef*p_Irrig->RadInShort/(COS_RadHalfCoef+ p_Irrig->RadInShort);
		}
		else if (COS_PlantModel_Sw(COS_PlantModel) == COS_PlantModel_Sw::PhotoSynthesis_Base)
			COS_TotPlantUptake = f_COS_Plant(sum(pNCPlant->AtmNewMobile));
		else
			COS_TotPlantUptake = f_COS_Plant(p_Plant->Transpiration_All);





		if (COS_Model_Sw(COS_Model) == COS_Model_Sw::Plant_Soil) {
			COS_TotSoilUptake = f_COS_Soil(O_RespRateProfile[_L1]);
			COS_EcoSystemFlux = COS_TotPlantUptake + COS_TotSoilUptake;
		}
	}
	
	if(N2O_YearlyFluxes>0) {
	
		if(YearShift&&m_pModelInfo->JDayNum==1) {
	            SortDArray(&DailyValuesNO[0],365);
	            SortDArray(&DailyValuesN2O[0],365);	            
	            if(FullYear) {
	                NO_10Percentil=DailyValuesNO[36];
	                NO_50Percentil=DailyValuesNO[182];
	                NO_90Percentil=DailyValuesNO[329];
	                NO_Ymean=sum(DailyValuesNO)/365.;
	                N2O_10Percentil=DailyValuesN2O[36];
	                N2O_50Percentil=DailyValuesN2O[182];
	                N2O_90Percentil=DailyValuesN2O[329];
	                N2O_Ymean=sum(DailyValuesN2O)/365.;
				}
	            else
	                FullYear=true;
	            
	            
	            DailyValuesNO.assign(365,0.0);
	            DailyValuesN2O.assign(365,0.0);
	            YearShift=false;
	        }
	        
	        if(m_pModelInfo->JDayNum==365) {
	            YearShift=true;        
	        }
	        if(m_pModelInfo->JDayNum>=1&&m_pModelInfo->JDayNum<=365) {
	            DailyValuesN2O[int(m_pModelInfo->JDayNum)-1]+=N2O_Rate*T_Step	;
	            DailyValuesNO[int(m_pModelInfo->JDayNum)-1]+=NO_Rate*T_Step;
	        }    
	}
	
	

}
void NC_GasFlux::Integ()
{
}

bool NC_GasFlux::End()
{
return true;
}
NC_GasFlux::~NC_GasFlux(void)
{
}
