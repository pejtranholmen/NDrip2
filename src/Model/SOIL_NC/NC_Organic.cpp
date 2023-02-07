#include "NC_Organic.h"
#include "../SOIL/SoilModel.h"
#include "../SOIL/Soil_Frost.h"
#include "../PLANT/PlantModel.h"
#include "./Functors/NC_Soil_Functors.h"
#include "./Functors/NC_Organic_Functors.h"
#include "../PLANT_NC/Functors/NC_Plant_Functors.h"
#include "../Structure/Structure.h"
						   
NC_Organic::NC_Organic(void)
{
	ModuleName="Soil organic processes";
	ModuleNo=25;
	ModuleConditions="";
}

bool NC_Organic::Ini()	{
//	Use Parameters
//Use Profile
//Use NC_Drivings, Only	:	NDrivTheta


	NC_Mineral::Ini();
	SoilModel* p_Soil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	Soil_Frost* pSoil=&p_Soil->m_SoilFrost;





	if (m_pModelStructure->m_NumActPlantElements < 3) {
		f_TurnOver_CN[_L1] = Get_OrgTurnOverFunctor(MyFunc::LITTER_TURNOVER);
		f_TurnOver_CN[_L2] = Get_OrgTurnOverFunctor(MyFunc::LITTER2_TURNOVER);
		f_TurnOver_CN[_F] = Get_OrgTurnOverFunctor(MyFunc::FAECES_TURNOVER);
		f_TurnOver_CN[_H] = Get_OrgTurnOverFunctor(MyFunc::HUMUS_TURNOVER);
	}
	else {
		f_TurnOver_CNP[_L1] = Get_D5_OrgTurnOverFunctor(MyFunc::LITTER_CNP_TURNOVER);
		f_TurnOver_CNP[_L2] = Get_D5_OrgTurnOverFunctor(MyFunc::LITTER2_CNP_TURNOVER);
		f_TurnOver_CNP[_F] = Get_D5_OrgTurnOverFunctor(MyFunc::FAECES_CNP_TURNOVER);
		f_TurnOver_CNP[_H] = Get_D5_OrgTurnOverFunctor(MyFunc::HUMUS_CNP_TURNOVER);
	}

	f_LitterTransSurf_L1 = Get_D1_Functor(MyFunc::LITTER_TRANS_SURFACE_L1);
	f_LitterTransSurf_L2 = Get_D1_Functor(MyFunc::LITTER_TRANS_SURFACE_L2);
	f_LitterTransSurf_Hum = Get_D1_Functor(MyFunc::LITTER_TRANS_SURFACE_HUM);

	
	CFungi_DepthLitter.resize(pSoil->NL);
	NFungi_DepthLitter.resize(pSoil->NL);  
	PFungi_DepthLitter.resize(pSoil->NL);
    
	CRootsSoilLitter.resize(pSoil->NL);
	CCoarseRootsSoilLitter.resize(pSoil->NL);
	TE_RootsSoilOldLitter.resize(pSoil->NL);
	TE_RootsSoilLitter.resize(pSoil->NL);
	NRootsSoilLitter.resize(pSoil->NL);
	NCoarseRootsSoilLitter.resize(pSoil->NL);
	PRootsSoilLitter.resize(pSoil->NL);
	PCoarseRootsSoilLitter.resize(pSoil->NL);
	CRootsSoilOldLitter.resize(pSoil->NL);
	NRootsSoilOldLitter.resize(pSoil->NL);
	PRootsSoilOldLitter.resize(pSoil->NL);
	COldCoarseRootsSoilLitter.resize(pSoil->NL);
	NOldCoarseRootsSoilLitter.resize(pSoil->NL);
	POldCoarseRootsSoilLitter.resize(pSoil->NL);

	Conc.Litter1.resize(pSoil->NL);
	Conc.Litter2.resize(pSoil->NL);
	Conc.Faeces.resize(pSoil->NL);

	for (size_t j = 0; j < NUM_ORG_FORMS; j++ ) {
		O_RespRate[j].resize(pSoil->NL);


	}
	for (size_t j = 0; j < NUM_ORG_TRANSFORMATION; j++) {
		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
			O_Transform[j][jj].resize(pSoil->NL);
			if(jj>0&&j<NUM_ORG_FORMS) O_Mineralisation[j][jj-1].resize(pSoil->NL);
		}
	}


	MicrobePotConsumptionRate.resize(pSoil->NL);

	if (Microbes_Sw(Microbes) >Microbes_Sw::off) {
		if(FaecesPool_Sw(FaecesPool)<= FaecesPool_Sw::no) {
			Mic_MortFrac_Fec=0.;
			Mic_ConsFrac_Fec=0.;
		}
		if(LitterPools_Sw(LitterPools)<= LitterPools_Sw::one) {
			Mic_MortFrac_L2=0.;
			Mic_ConsFrac_L2=0.;
		}
	
		f_Decomp_CN_Response = Get_D1_Functor(MyFunc::MICROBE_CN_RATIO_RESPONSE);
		f_Decomp_Conc_Response = Get_D1_Functor(MyFunc::MICROBE_CONC_RESPONSE);

		f_PotentialGrowthMicrobe = Get_D1_Functor(MyFunc::POT_GROWTHCONSUMPTION_MICROBE);
		f_GrowthConsumption = Get_D1_TableFunctor(MyFunc::GROWTH_CONSUMPTION_MICROBE);
		f_MicrobeRespiration = Get_D1_Functor(MyFunc::RESPIRATION_MICROBE);
		f_Humification = Get_D1_TableFunctor(MyFunc::HUMIFICATION_MICROBE);
		f_Mortality = Get_D1_TableFunctor(MyFunc::MORTALITY_MICROBE);





	}
	if(!m_pModelInfo->StartStateValue) {
		for(size_t j=0; j<NUM_ORG_FORMS;j++)
			for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
				O_State[j][jj].assign(pSoil->NL, 0.);
				Org_CarbonRatio[j][jj].assign(pSoil->NL, 10.);
			}
	
		DEPTH_type out;
		if (Initial_Soil_Organic_Sw(Initial_Soil_Organic) > Initial_Soil_Organic_Sw::Table) {
			for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
				for (size_t j = _L1; j <= _MIC; j++) {
					out = pSoil->DepthDist(Initial_Soil_Organic, Org_State_IniValue[j][jj].Tot_Profile, Org_State_IniValue[j][_C_].Depth, Org_State_IniValue[j][_C_].FracExpTail);
					O_State[j][jj] = out.Dist;
				}
			}
			if (InitialCarbonConditions_Sw(InitialCarbonConditions) == InitialCarbonConditions_Sw::As_Nitrogen) {
				for (size_t j = _L1; j <= _MIC; j++) {
					for (size_t i = 0; i < pSoil->NL; i++) {
						O_State[j][_C_][i] = O_State[j][_N_][i] * Org_State_IniCarbonRatio[j][_N_ - 1].Tot_Profile;
					}
				}
			}
		}
		else {
			if (Measured_Organic_Table_Sw(Measured_Organic_Table) == Measured_Organic_Table_Sw::As_measured_layers) {
				ReCalcOrgProfile();

			}
			else {
				for (size_t j = _L1; j <= _MIC; j++) {
					for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++)
						O_State[j][jj] = Org_State_IniValue[j][jj].Layer_Mass;
				}
				if (InitialCarbonConditions_Sw(InitialCarbonConditions) == InitialCarbonConditions_Sw::As_Nitrogen) {
					for (size_t j = _L1; j <= _MIC; j++) {
						for (size_t i = 0; i < pSoil->NL; i++) {
							O_State[j][_C_][i] = O_State[j][_N_][i] * Org_State_IniCarbonRatio[j][_N_ - 1].Layer_Mass[i];
						}
					}
				}
			}
		}
			//HH: check the initialPhosphoruscondition switch also
		if (InitialPhosphorusConditions_Sw(InitialPhosphorusConditions) == InitialPhosphorusConditions_Sw::As_CP_ratio_and_Carbon) {
				for (size_t j = _L1; j <= _MIC; j++) {
					for (size_t i = 0; i < pSoil->NL; i++) {
						O_State[j][_P_][i] = O_State[j][_C_][i] / Org_State_IniCarbonRatio[j][_P_ - 1].Layer_Mass[i];
					}
				}
		}


		
		if (DissolvedOrganics_Sw(DissolvedOrganics) > DissolvedOrganics_Sw::off) {

				
					for (size_t i = 0; i < pSoil->NL; i++) {
						O_State[_DIS][_C_][i] = Init_CDissConc * pSoil->AThicknessLayers[i] * pSoil->Theta[i] / 100.;
						O_State[_DIS][_N_][i] = O_State[_DIS][_C_][i] / Org_State_IniCarbonRatio[_H][_N_ - 1].Tot_Profile;
						if(m_pModelStructure->m_NumActPlantElements>2) O_State[_DIS][_P_][i] = O_State[_DIS][_C_][i] / Org_State_IniCarbonRatio[_H][_P_ - 1].Tot_Profile;
					}
					for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++)
						Dissolved[jj].Conc.resize(pSoil->NL);

	    }

		
		
	}

	for(size_t i=0; i<pSoil->NL; i++) 
		if(O_State[_H][_C_][i]<=0) 
			 O_State[_H][_C_][i]=0.1E-5;
		
	for (size_t j = 0; j < NUM_ORG_FORMS; j++)
		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++)
				O_State_Profile[j][jj] =sum(O_State[j][jj]);


//// Total Summary
	for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
		O_State_TotOrg[jj] = O_State_Profile[_L1][jj] + O_State_Profile[_H][jj] + O_State_Profile[_DIS][jj] + SurfaceLitter[_L1][jj];

		if (FungalGrowth_Sw(p_NC_Plant->FungalGrowth) > FungalGrowth_Sw::off) 
			O_State_TotOrg[jj] +=sum(p_NC_Plant->P_State[_FUNGI][jj]);
		if (LitterPools_Sw(LitterPools) > LitterPools_Sw::one) {
			O_State_Profile[_L2][jj] = sum(O_State[_L2][jj]);
			O_State_TotOrg[jj] += O_State_Profile[_L2][jj] + SurfaceLitter[_L2][jj];
		}
		if (FaecesPool_Sw(FaecesPool) > FaecesPool_Sw::no) {
			O_State_Profile[_F][jj] = sum(O_State[_F][jj]);
			O_State_TotOrg[jj] += O_State_Profile[_F][jj];
		}
		if (Microbes_Sw(Microbes) > Microbes_Sw::off) {
			O_State_Profile[_MIC][jj] = sum(O_State[_MIC][jj]);
			O_State_TotOrg[jj] += O_State_Profile[_MIC][jj];
		}
	}

	if(O_State_TotOrg[_N_]>0)	CN_Ratio_TotSoil=O_State_TotOrg[_C_]/O_State_TotOrg[_N_] ;
	if(O_State_Profile[_H][_N_]>0)	CN_Ratio_TotHumus=O_State_Profile[_H][_C_]/O_State_Profile[_H][_N_];
	if (O_State_TotOrg[_P_] > 0)	CP_Ratio_TotSoil = O_State_TotOrg[_C_] / O_State_TotOrg[_P_];
	if (O_State_Profile[_H][_P_] > 0)	CP_Ratio_TotHumus = O_State_Profile[_H][_C_] / O_State_Profile[_H][_P_];
	
	if(DisplacementOrganicLayers_Sw(DisplacementOrganicLayers)> DisplacementOrganicLayers_Sw::Static) {
		for (size_t j = 0; j < NUM_ORG_FORMS; j++) {
			for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++)
				Prev_O_State[j][jj].assign(O_State[j][jj].begin(), O_State[j][jj].end());
		}
		m_DisplacementDone=false;
		MakeDisplacement();
	}

	f_ConcFromAmountandWater = OrganicWaterConcentration(DocSolFraction, pSoil->AThicknessLayers);


	for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) 
		Dissolved[jj].Conc=f_ConcFromAmountandWater(O_State[_DIS][jj], p_NC_Plant->NDrivTheta, Dissolved[jj].Conc);

	if (p_NC_Plant->DeepPercolationInput > 0)
		f_VerticalSoluteFlowFromWaterFlowConcDeepPerc = VerticalSoluteWaterFlows(true);
	else
		f_VerticalSoluteFlowFromWaterFlowConcDeepPerc = VerticalSoluteWaterFlows(false);
	if (p_NC_Plant->SoilDrainageInput > 0)	f_DrainageSoluteFlowWithMeanConcFromWaterFlowConc = HorisontalSoluteWaterFlows();



	return true;
}
bool NC_Organic::IniFunctors() {

	NC_Mineral::IniFunctors();
	SoilModel* p_Soil = (SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	Soil_Frost* pSoil = &p_Soil->m_SoilFrost;

	if (m_pModelStructure->m_NumActPlantElements < 3) {
		f_TurnOver_CN[_L1] = Get_OrgTurnOverFunctor(MyFunc::LITTER_TURNOVER);
		f_TurnOver_CN[_L2] = Get_OrgTurnOverFunctor(MyFunc::LITTER2_TURNOVER);
		f_TurnOver_CN[_F] = Get_OrgTurnOverFunctor(MyFunc::FAECES_TURNOVER);
		f_TurnOver_CN[_H] = Get_OrgTurnOverFunctor(MyFunc::HUMUS_TURNOVER);
	}
	else {
		f_TurnOver_CNP[_L1] = Get_D5_OrgTurnOverFunctor(MyFunc::LITTER_CNP_TURNOVER);
		f_TurnOver_CNP[_L2] = Get_D5_OrgTurnOverFunctor(MyFunc::LITTER2_CNP_TURNOVER);
		f_TurnOver_CNP[_F] = Get_D5_OrgTurnOverFunctor(MyFunc::FAECES_CNP_TURNOVER);
		f_TurnOver_CNP[_H] = Get_D5_OrgTurnOverFunctor(MyFunc::HUMUS_CNP_TURNOVER);
	}

	f_LitterTransSurf_L1 = Get_D1_Functor(MyFunc::LITTER_TRANS_SURFACE_L1);
	f_LitterTransSurf_L2 = Get_D1_Functor(MyFunc::LITTER_TRANS_SURFACE_L2);
	f_LitterTransSurf_Hum = Get_D1_Functor(MyFunc::LITTER_TRANS_SURFACE_HUM);


	if (Microbes_Sw(Microbes) > Microbes_Sw::off) {
		f_Decomp_CN_Response = Get_D1_Functor(MyFunc::MICROBE_CN_RATIO_RESPONSE);
		f_Decomp_Conc_Response = Get_D1_Functor(MyFunc::MICROBE_CONC_RESPONSE);

		f_PotentialGrowthMicrobe = Get_D1_Functor(MyFunc::POT_GROWTHCONSUMPTION_MICROBE);
		f_GrowthConsumption = Get_D1_TableFunctor(MyFunc::GROWTH_CONSUMPTION_MICROBE);
		f_MicrobeGrowthRespiration = Get_D1_TableFunctor(MyFunc::RESP_GROWTH_CONSUMPTION_MICROBE);
		f_Humification = Get_D1_TableFunctor(MyFunc::HUMIFICATION_MICROBE);
		f_Mortality = Get_D1_TableFunctor(MyFunc::MORTALITY_MICROBE);
	}



	f_ConcFromAmountandWater = OrganicWaterConcentration(DocSolFraction, pSoil->AThicknessLayers);


	for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++)
		Dissolved[jj].Conc = f_ConcFromAmountandWater(O_State[_DIS][jj], p_NC_Plant->NDrivTheta, Dissolved[jj].Conc);

	if (p_NC_Plant->DeepPercolationInput > 0)
		f_VerticalSoluteFlowFromWaterFlowConcDeepPerc = VerticalSoluteWaterFlows(true);
	else
		f_VerticalSoluteFlowFromWaterFlowConcDeepPerc = VerticalSoluteWaterFlows(false);
	if (p_NC_Plant->SoilDrainageInput > 0)	f_DrainageSoluteFlowWithMeanConcFromWaterFlowConc = HorisontalSoluteWaterFlows();
	if (DissolvedOrganics_Sw(DissolvedOrganics) > DissolvedOrganics_Sw::off) {
		vector<double> RateCoef;
		RateCoef.push_back(RateCoefLitter1Dis); RateCoef.push_back(RateCoefLitter2Dis);
		RateCoef.push_back(RateCoefFaecesDis); RateCoef.push_back(RateCoefHumusDis); RateCoef.push_back(RateCoefMicrobeDis);
		f_DissolvedTransformbyTempThetaState = DissolvedTransformation(RateCoef, f_CommonTempResponse, f_Theta_Response);
		f_DissolvedHumusTransformbyTempThetaState = DissolvedTransformation(RateCoef, RateCoefDissolved, f_CommonTempResponse, f_Theta_Response);
	}

	return true;
}
bool NC_Organic::Dyn()
{
	NC_Mineral::Dyn();
	Flux();
	Integ();
	return true;
}

void NC_Organic::Flux()
{
PlantModel* pPlant=(PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
WaterUptake* p_Plant=&pPlant->m_WaterUptake;
SoilModel* p_Soil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
Soil_Frost* pSoil=&p_Soil->m_SoilFrost;
/*	Use NC_Drivings, Only	:	NDrivTheta, NDrivWaterFlow, NDrivDrainage, NDrivDeepPercolation
Use NC_Drivings, Only	:	DeepPercolationInput, SoilDrainageInput
Use AbioticResponses
Use Plant, only		: RootDistribution, RFracLow, p_Plant->SimRootDepth, RootF
Use Profile, only	: DepthDist, AThicknessLayers
Use Timeinfo

Integer		II
Real		Cdependent
Real		RateL
Real*8		DecompHumus*/
//!	New local variable included when correcting bug on Carbon mineralisation 18/1 2004
//!	Flows from Surface Litter to uppermost soil compartments

auto OutflowFromSurfaceLittertoLitterUppermostLayer = [&]() {
	for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
		for (size_t i = _L1; i <= _L2; i++) {
			switch (i) {
			case _L1:
				if(Do_Cultivating) 
					SurfaceLitterMixingIntoUppermostlayer[i][jj] = SurfaceLitter[i][jj]/T_Step;
				else
					SurfaceLitterMixingIntoUppermostlayer[i][jj] = f_LitterTransSurf_L1(SurfaceLitter[i][jj]);
				break;
			case _L2:
				if (Do_Cultivating)
					SurfaceLitterMixingIntoUppermostlayer[i][jj] = SurfaceLitter[i][jj] / T_Step;
				else
					SurfaceLitterMixingIntoUppermostlayer[i][jj] = f_LitterTransSurf_L2(SurfaceLitter[i][jj]);
				break;

			}
		}
		if (Microbes_Sw(Microbes) > Microbes_Sw::off) {
			SurfaceLitterMixingIntoUppermostlayer[_H][jj] = f_LitterTransSurf_Hum(SurfaceLitter[_L1][jj]);
			//why ???
		}
	}
	if (p_Plant->TraceElementUptake) {
		p_Plant->TE_SurfaceLitter_L1 = RateCoefSurf_L1 * p_Plant->TE_SurfaceLitter;
		p_Plant->TE_RootsLitter1.assign(pSoil->NL, 0.);
	}
};

auto OutflowFromPlantLitterFalltoSoilLayers = [&]() {

	auto GetDepthDistributionofLitterFall = [&](size_t index_plant, size_t PlantComp, size_t PlantElement) {
		DEPTH_type out;
		out = p_Plant->f_WaterUptakeDistribution(p_NC_Plant->P_LitterFall[PlantComp][PlantElement][index_plant], p_Plant->SimRootDepth[index_plant]);
		if (PlantComp == _OLD_COARSE_ROOT) {
			auto koll = out;
		}
		return out.Dist;
	
	};

//Reset all Fluxes to zero
	for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++)
		for (size_t j = 0; j < 2; j++)
			LitterInputToSoilLayers[j][jj].assign(pSoil->NL,0.);

// Add contributions of all Plants with all plant components and all plant elements to to vector for two soil litters for all plant elements and soil layers
	for (size_t i = 0; i < p_Plant->NumPlants; i++) {
		vector<double> LitterFallToLayers;
		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
			for (size_t j = _ROOT; j <= _OLD_COARSE_ROOT; j++) {
				LitterFallToLayers = GetDepthDistributionofLitterFall(i, j, jj);
				if (j <= _OLD_COARSE_ROOT||LitterPools_Sw(LitterPools)== LitterPools_Sw::one) {
					for (size_t layer = 0; layer < pSoil->NL; layer++) {
						LitterInputToSoilLayers[_L1][jj][layer] += LitterFallToLayers[layer];
					}
				}
				else {
					for (size_t layer = 0; layer < pSoil->NL; layer++) {
						LitterInputToSoilLayers[_L1][jj][layer] += LitterFallToLayers[layer] * (1 - p_NC_Plant->Root_FracLitter2[i]);
						LitterInputToSoilLayers[_L2][jj][layer] += LitterFallToLayers[layer] * p_NC_Plant->Root_FracLitter2[i];
					}
				}
			}	//Fungi Component below
			if (FungalGrowth_Sw(p_NC_Plant->FungalGrowth) > FungalGrowth_Sw::off) {
				LitterFallToLayers = GetDepthDistributionofLitterFall(i, _FUNGI, jj);
				for (size_t layer = 0; layer < pSoil->NL; layer++) {
					LitterInputToSoilLayers[_L1][jj][layer] += LitterFallToLayers[layer];
				}
			}
		}
	}

};
auto Carbon_RatioSoilOrganics = [&]() {

	for (size_t jj = 1; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
		for (size_t ii = 0; ii < Org_CarbonRatio[_L1][jj - 1].size(); ii++) Org_CarbonRatio[_L1][jj-1][ii] = (O_State[_L1][_C_][ii] + 0.001) / (O_State[_L1][jj][ii] + 0.0001);
		if (LitterPools_Sw(LitterPools) > LitterPools_Sw::one)
			for (size_t ii = 0; ii < Org_CarbonRatio[_L2][jj - 1].size(); ii++) Org_CarbonRatio[_L2][jj - 1][ii] = (O_State[_L2][_C_][ii] + 0.001) / (O_State[_L2][jj][ii] + 0.0001);
		if (FaecesPool_Sw(FaecesPool) > FaecesPool_Sw::no)
			for (size_t ii = 0; ii < Org_CarbonRatio[_F][jj -1].size(); ii++) Org_CarbonRatio[_F][jj-1][ii] = (O_State[_F][_C_][ii] + 0.001) / (O_State[_F][jj][ii] + 0.0001);
		for (size_t ii = 0; ii < Org_CarbonRatio[_H][jj - 1].size(); ii++) {
			if (O_State[_H][jj][ii] > 0.) {
				Org_CarbonRatio[_H][jj - 1][ii] = (O_State[_H][_C_][ii] + 0.001) / (O_State[_H][jj][ii] + 0.0001);
			}
			else {
				Org_CarbonRatio[_H][jj - 1][ii] = 50.;

			}
		}
		for (size_t ii = 0; ii < Org_CarbonRatio[_MIC][jj-1].size(); ii++) Org_CarbonRatio[_MIC][jj-1][ii] = (O_State[_MIC][_C_][ii] + 0.001) / (O_State[_MIC][jj][ii] + 0.0001);
	}
};
auto ConcentrationofSoilOrganics = [&]() {
	for (size_t ii = 0; ii < Conc.Litter1.size(); ii++) Conc.Litter1[ii] = O_State[_L1][_C_][ii] / pSoil->AThicknessLayers[ii];
	if (LitterPools_Sw(LitterPools) > LitterPools_Sw::one)
		for (size_t ii = 0; ii < Conc.Litter2.size(); ii++) Conc.Litter2[ii] = O_State[_L2][_C_][ii] / pSoil->AThicknessLayers[ii];
	if (FaecesPool_Sw(FaecesPool) > FaecesPool_Sw::no)
		for (size_t ii = 0; ii < Conc.Faeces.size(); ii++) Conc.Faeces[ii] = O_State[_F][_C_][ii] / pSoil->AThicknessLayers[ii];

};
auto AbioticResponseOnDecomposition = [&]() {
	Response.Litter = 0.;
	Response.Humus = 0.;
	//Do i=1, NL
	for (size_t i = 0; i < Response.Temp.size(); i++) {
		if (Common_Moisture_Response_Sw(Common_Moisture_Response) == Common_Moisture_Response_Sw::CO2_Function)
			Response.Theta[i] = f_CO2_Response(CO2_Conc[i]);
		else if (Common_Moisture_Response_Sw(Common_Moisture_Response) == Common_Moisture_Response_Sw::CombinedThetaCO2)
			Response.Theta[i] = f_CombinedThetaCO2_Response(i,  p_NC_Plant->NDrivTheta[i], CO2_Conc[i]);
		else
			Response.Theta[i] = f_Theta_Response(i, p_NC_Plant->NDrivTheta[i]);
		Response.Temp[i] = f_CommonTempResponse(p_NC_Plant->NDrivTemperature[i]);
		Response.Temp_Theta[i] = Response.Temp[i] * Response.Theta[i];
		Response.Litter += Response.Temp_Theta[i] * O_State[_L1][_C_][i];
		Response.Humus += Response.Temp_Theta[i] * O_State[_H][_C_][i];
	}
	//! David, 2005-11-18, to avoid division by zeero with new compiler
	if (sum(O_State[_L1][_C_]) > 0.)
		Response.Litter = Response.Litter / sum(O_State[_L1][_C_]);

	if (O_State_Profile[_H][_C_] > 0.)
		Response.Humus = Response.Humus / O_State_Profile[_H][_C_];
};

	OutflowFromSurfaceLittertoLitterUppermostLayer();
	OutflowFromPlantLitterFalltoSoilLayers();
	Carbon_RatioSoilOrganics();
	ConcentrationofSoilOrganics();
	AbioticResponseOnDecomposition();





	if(Microbes_Sw(Microbes)==Microbes_Sw::off) {

		for (size_t j = _L1; j <= _H; j++) { // loop from Litter1 to Humus
			for (size_t i = 0; i < O_RespRate[_L1].size(); i++) { // Loop over all soil layers

				if (LitterPools_Sw(LitterPools) == LitterPools_Sw::one&&j == _L2) continue;
				if (FaecesPool_Sw(FaecesPool) == FaecesPool_Sw::no&&j == _F) continue;



				if (O_State[j][_C_][i] > 0.) {
					if (m_pModelStructure->m_NumActPlantElements <= 2) {	   // When C and N are the only elements
						OrgTurnOver a = f_TurnOver_CN[j](Response.Temp_Theta[i] *O_State[j][_C_][i], Org_CarbonRatio[j][_N_ - 1][i], SoilAmmonium[i]);
						O_RespRate[j][i] = a.Respiration;	O_Mineralisation[j][_N_ - 1][i] = a.N_Mineralisation;
						O_Transform[j][_C_][i] = a.Trans_Org;	O_Transform[j][_N_][i] = a.N_Trans_Conv;
					}
					else {
						// When also P exist, Note that the f_Turnover_CNP[j]
						// differs from the f_Turnover_CN[j] above
						OrgTurnOver a;
						double orginput;
						orginput = Response.Temp_Theta[i] *O_State[j][_C_][i];
						double c_n_ratio;
						c_n_ratio= Org_CarbonRatio[j][_N_ - 1][i];
						double c_p_ratio;
						c_p_ratio= Org_CarbonRatio[j][_P_ - 1][i];

						a= f_TurnOver_CNP[j](orginput, Org_CarbonRatio[j][_N_ - 1][i], SoilAmmonium[i],
							Org_CarbonRatio[j][_P_ - 1][i], Soil_PO4[i]);//Add to labile HH
						O_RespRate[j][i] = a.Respiration;	O_Mineralisation[j][_N_ - 1][i] = a.N_Mineralisation;
						O_Mineralisation[j][_P_ - 1][i] = a.P_Mineralisation;
						O_Transform[j][_C_][i] = a.Trans_Org;	O_Transform[j][_N_][i] = a.N_Trans_Conv;
						if (O_Transform[j][_P_].size() > i) O_Transform[j][_P_][i] = min(a.P_Trans_Conv, max(0., O_State[j][_P_][i]*0.5)); // Limited formation of P humus by available P of Litter
					}

				}
				else {
					O_RespRate[j][i] = 0.;	O_Mineralisation[j][_N_ - 1][i] = 0.; O_Mineralisation[j][_P_ - 1][i] = 0.;
					O_Transform[j][_C_][i] = 0.;	O_Transform[j][_N_][i] = 0.;
					if(O_Transform[j][_P_].size()>i) O_Transform[j][_P_][i] = 0.;
				}
			}
		};
		if(p_Plant->TraceElementUptake) {
			for (size_t i = 0; i < O_RespRate[_L1].size(); i++) {

				double RateL = RateCoefLitter1 * Response.Temp_Theta[i] *p_Plant->TE_Litter1[i];
				p_Plant->TE_Litter1MinRate[i] = RateL * (1 - HumFracLitter1);
				p_Plant->TE_Litter1HumusRate[i] = RateL * HumFracLitter1;
				p_Plant->TE_HumusMinRate[i] = RateCoefHumus * Response.Temp_Theta[i] *p_Plant->TE_Humus[i];
			}
		}

	}
  	else  {//! if microbes are explictely taking into account

		//Do i=1,NL
		double Cdependent;
		for(size_t i=0; i<O_State[_L1][_C_].size();i++) {
			if(Microbes==1) {
				if(LitterPools==0) 
					Cdependent=O_State[_L1][_C_][i];
				else
					Cdependent=O_State[_L1][_C_][i]+O_State[_L2][_C_][i];
				
				if(FaecesPool>0) 
					Cdependent=Cdependent+O_State[_F][_C_][i];
			}
			else	//! = 2 below
				Cdependent=O_State[_MIC][_C_][i];


			double AbioticRespons;

			if (Microbial_Consumption_Sw(Microbial_Consumption) == Microbial_Consumption_Sw::Static)
				AbioticRespons=1.;
			else if (Microbial_Consumption_Sw(Microbial_Consumption) == Microbial_Consumption_Sw::F_Temp)
				AbioticRespons= f_CommonTempResponse(p_NC_Plant->NDrivTemperature[i]);
			else if (Microbial_Consumption_Sw(Microbial_Consumption) == Microbial_Consumption_Sw::F_Temp_Moisture)
				AbioticRespons= Response.Temp_Theta[i];
			else
				AbioticRespons= 0.;

			double PotentialConsumptionRate= f_PotentialGrowthMicrobe( AbioticRespons*Cdependent);
//!			Litter1
			O_Transform[L1_MIC][_C_][i]=f_GrowthConsumption(_L1, f_Decomp_CN_Response(Org_CarbonRatio[_MIC][_N_-1][i])*f_Decomp_Conc_Response(Conc.Litter1[i])*PotentialConsumptionRate)
									-f_Mortality(_L1,AbioticRespons*O_State[_MIC][_C_][i]);

 			O_Transform[L1_MIC][_N_][i]= O_Transform[L1_MIC][_C_][i] / Org_CarbonRatio[_MIC][_N_ - 1][i];

			O_RespRate[_L1][i]=f_MicrobeGrowthRespiration(_L1, f_Decomp_CN_Response(Org_CarbonRatio[_L1][_N_ - 1][i])*f_Decomp_Conc_Response(Conc.Litter1[i])*PotentialConsumptionRate) ;
			O_Transform[_L1][_C_][i]= f_Humification(_L1,f_Decomp_CN_Response(Org_CarbonRatio[_L1][_N_ - 1][i])*f_Decomp_Conc_Response(Conc.Litter1[i])*PotentialConsumptionRate);
			O_Transform[_L1][_N_][i]=O_Transform[_L1][_C_][i]/ Org_CarbonRatio[_H][_N_-1][i];

//!			Litter2
			if(LitterPools_Sw(LitterPools)> LitterPools_Sw::one) {
				O_Transform[L2_MIC][_C_][i]= f_GrowthConsumption(_L2, f_Decomp_CN_Response(Org_CarbonRatio[_L2][_N_ - 1][i])*f_Decomp_Conc_Response(Conc.Litter2[i])*PotentialConsumptionRate)
					- f_Mortality(_L2,AbioticRespons*O_State[_MIC][_C_][i]);
		
				O_Transform[L2_MIC][_N_][i]= O_Transform[L2_MIC][_C_][i] / Org_CarbonRatio[_MIC][_N_ - 1][i];
				O_RespRate[_L2][i]=f_MicrobeGrowthRespiration(_L2,f_Decomp_CN_Response(Org_CarbonRatio[_L2][_N_ - 1][i])*f_Decomp_Conc_Response(Conc.Litter2[i])*PotentialConsumptionRate) ;
				O_Transform[_L2][_C_][i] = f_Humification(_L2,f_Decomp_CN_Response(Org_CarbonRatio[_L2][_N_ - 1][i])*f_Decomp_Conc_Response(Conc.Litter2[i])*PotentialConsumptionRate);

				O_Transform[_L2][_N_][i]=O_Transform[_L2][_C_][i]/ Org_CarbonRatio[_H][_N_ - 1][i];
			}
//!			Faeces
			if(FaecesPool_Sw(FaecesPool) >FaecesPool_Sw::no) {
				O_Transform[F_MIC][_C_][i]= f_GrowthConsumption(_F, f_Decomp_CN_Response(Org_CarbonRatio[_F][_N_ - 1][i])*f_Decomp_Conc_Response(Conc.Litter2[i])*PotentialConsumptionRate)
					- f_Mortality(_F,AbioticRespons*O_State[_MIC][_C_][i]);
					
					
					//Eff_Microbes*Mic_ConsFrac_Fec*CN_RateDepFunc(CN_Ratio_Faeces[i])*SubCo//ncRateDepFunc(C_Conc_Faeces[i])*MicrobePotConsumptionRate[i]
					//				-RateCoefMic_Mort*Mic_MortFrac_Fec*MicroMortAbioFunc(i)*O_State[_MIC][_C_][i];
				O_Transform[F_MIC][_N_][i]= O_Transform[F_MIC][_C_][i] / Org_CarbonRatio[_MIC][_N_ - 1][i];
				O_RespRate[_F][i]= f_MicrobeGrowthRespiration(_F, f_Decomp_CN_Response(Org_CarbonRatio[_F][_N_ - 1][i])*f_Decomp_Conc_Response(Conc.Litter2[i])*PotentialConsumptionRate);
					
					
					//(1-Eff_Microbes)*Mic_ConsFrac_Fec*CN_RateDepFunc(CN_Ratio_Litter2[i])*SubConcRateDepFunc(C_Conc_Litter2[i])*MicrobePotConsumptionRate[i];
	 			O_Transform[_F][_C_][i]= f_Humification(_F, f_Decomp_CN_Response(Org_CarbonRatio[_F][_N_ - 1][i])*f_Decomp_Conc_Response(Conc.Faeces[i])*PotentialConsumptionRate);
					
					
				//	Mic_HumFrac_L2*Mic_ConsFrac_L2*CN_RateDepFunc(CN_Ratio_Litter1[i])*SubConcRateDepFunc(C_Conc_Litter1[i])*MicrobePotConsumptionRate[i];
				O_Transform[_F][_N_][i]=O_Transform[_F][_C_][i]/ Org_CarbonRatio[_H][_N_ - 1][i];

			}

//!			Humus
			if(Microbes_Sw(Microbes)==Microbes_Sw::on_substate_dependent) PotentialConsumptionRate= f_PotentialGrowthMicrobe(AbioticRespons*O_State[_H][_C_][i]);

			O_Transform[H_MIC][_C_][i]=f_GrowthConsumption(_H, PotentialConsumptionRate)
									-f_Mortality(_H, AbioticRespons*O_State[_MIC][_C_][i]);
			O_Transform[H_MIC][_N_][i]= O_Transform[H_MIC][_C_][i] / Org_CarbonRatio[_MIC][_N_ - 1][i];



			O_RespRate[_H][i]=f_MicrobeGrowthRespiration(_H,PotentialConsumptionRate);

//!			Microbes
			O_RespRate[_MIC][i]=f_MicrobeRespiration(AbioticRespons*O_State[_MIC][_C_][i]);

			O_Mineralisation[_MIC][_N_-1][i] = (1-Org_CarbonRatio[_MIC][_N_ - 1][i]/CN_Ratio_Microbe)*O_State[_MIC][_N_][i];
			if (SoilAmmonium[i] > 0)
				O_Mineralisation[_MIC][_N_ - 1][i] = max(-N_ImmobMaxAvailFrac * SoilAmmonium[i], O_Mineralisation[_MIC][_N_ - 1][i]);
			else
				O_Mineralisation[_MIC][_N_ - 1][i] = max(0., O_Mineralisation[_MIC][_N_ - 1][i]);
		}
	
	  	O_RespRateProfile[_MIC] =sum(O_RespRate[_MIC]);
	  	NTotMicrobeNH4Rate =sum(O_Mineralisation[_MIC][_N_ - 1]);
 	}
	
	NTotOrgNH4Rate = 0.;
	PTotOrgPO4Rate = 0.;
	CTotSoilRespRate = 0.;

	for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
		Humusformation[jj] = 0.;
		O_State_TotOrg[jj] = SurfaceLitter[_L1][jj]+ SurfaceLitter[_L2][jj];

		if (Microbes > 0) {
			CTotSoilRespRate = O_RespRateProfile[_MIC];
			O_State_Profile[_MIC][jj] = sum(O_State[_MIC][jj]);
			O_State_TotOrg[jj] += O_State_Profile[_MIC][jj];
		}

	}
	for (size_t j = _L1; j <=_DIS; j++) {
		if ((j==_L2&&LitterPools == 0)||(j==_F&&FaecesPool == 0)) continue;
		if ((j == _MIC && Microbes == 0) || (j == _DIS && DissolvedOrganics == 0)) continue;

		O_RespRateProfile[j] = sum(O_RespRate[j]);
		CTotSoilRespRate += O_RespRateProfile[j];

		O_MineralisationProfile[j][_N_ - 1] = sum(O_Mineralisation[j][_N_ - 1]);

		if (m_pModelStructure->m_NumActPlantElements >= 3) {
			O_MineralisationProfile[j][_P_ - 1] = sum(O_Mineralisation[j][_P_ - 1]);
			PTotOrgPO4Rate += O_MineralisationProfile[j][_P_ - 1];
		}
		NTotOrgNH4Rate += O_MineralisationProfile[j][_N_ - 1];
		
		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
			O_State_Profile[j][jj]= sum(O_State[j][jj]);
			O_State_TotOrg[jj] += O_State_Profile[j][jj];
			if (j < _H) Humusformation[jj] += sum(O_Transform[j][jj]);
		}
	}
	if (p_NC_Plant->FungalGrowth > 0) {
		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
			O_State_TotOrg[jj] += sum(p_NC_Plant->P_State[_FUNGI][jj]);
		}
	}
	for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
		LitterInputProfile[jj] = sum(LitterInputToSoilLayers[_L1][jj]) + sum(LitterInputToSoilLayers[_L2][jj]);
	}


    //! AM 151211: add fungal uptake to make equal to NTotmin_PlantUptake
	NTotOrg_PlantUptake=sum(O_Transform[L1_PLANTUPTAKE][_N_])+sum(O_Transform[H_PLANTUPTAKE][_N_])+ sum(O_Transform[L1_FUNGIUPTAKE][_N_]) + sum(O_Transform[H_FUNGIUPTAKE][_N_]);
	if (m_pModelStructure->m_NumActPlantElements >= 3)
	PTotOrg_PlantUptake = sum(O_Transform[L1_PLANTUPTAKE][_P_]) + sum(O_Transform[H_PLANTUPTAKE][_P_]) + sum(O_Transform[L1_FUNGIUPTAKE][_P_]) + sum(O_Transform[H_FUNGIUPTAKE][_P_]);

	
//! Dissolved Organic Model below

//!   Dissolved organics transfers
	auto DissolvedOrganicsConc = [&]() {
		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) 
		Dissolved[jj].Conc=f_ConcFromAmountandWater(O_State[_DIS][jj], p_NC_Plant->NDrivTheta, Dissolved[jj].Conc);
	};
	auto DissolvedOrganicsFlux = [&]() {
		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
			f_VerticalSoluteFlowFromWaterFlowConcDeepPerc(p_NC_Plant->NDrivWaterFlow, Dissolved[jj].Conc, p_NC_Plant->NDrivDeepPercolation, Dissolved[jj].VFlow);
			if (p_NC_Plant->SoilDrainageInput > 0) {
				pair<double, vector<double>> out;
				out = f_DrainageSoluteFlowWithMeanConcFromWaterFlowConc(p_NC_Plant->NDrivDrainage, Dissolved[jj].Conc, Dissolved[jj].HFlow);
				Dissolved[jj].HFlow = out.second;
			}
			Dissolved[jj].TotHFlow = sum(Dissolved[jj].HFlow);
			double sumdrainage = sum(p_NC_Plant->NDrivDrainage);
			if (sumdrainage > 0.00001) {
				Dissolved[jj].TotHFlowConc = sum(Dissolved[jj].HFlow) / sumdrainage*1000;
				if (jj = 2) Tot_P_DrainageConc = (Dissolved[_P_].TotHFlow + PSurfRunoff + PTotPO4_Drainage) / sumdrainage * 1000;
			}
			//if (jj = _P_) Tot_P_DrainageConc = (Dissolved[_P_].TotHFlow + PSurfRunoff + PTotPO4_Drainage) / sumdrainage * 1000;
		}
	};
	auto DissolvedOrganicsTransformation = [&]() {
		// !Equivilibrium transfers between organics and dissolved organics
			//Do i=1, NL
		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
			for (size_t j = _L1; j <= _MIC; j++) {
				if (j == _L2 && LitterPools_Sw(LitterPools) == LitterPools_Sw::two) continue; //HH
				if (j == _F && FaecesPool_Sw(FaecesPool) == FaecesPool_Sw::no) continue;
				if (j == _MIC && Microbes_Sw(Microbes) == Microbes_Sw::off) continue;
				if( j==_H)
					O_Transform[j + L1_DIS][jj]=f_DissolvedHumusTransformbyTempThetaState(j, p_NC_Plant->NDrivTemperature, p_NC_Plant->NDrivTheta, O_State[j][jj], O_State[_DIS][jj], O_Transform[j + L1_DIS][jj]);
				else
					O_Transform[j + L1_DIS][jj] = f_DissolvedTransformbyTempThetaState(j, p_NC_Plant->NDrivTemperature, p_NC_Plant->NDrivTheta, O_State[j][jj], O_Transform[j + L1_DIS][jj]);

			}
		}
	};

	auto SummationDissolvedOrganics = [&]() {

		// State  Variables
		for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
			Dissolved[jj].TotSourceFlow = sum(O_Transform[L1_DIS][jj]) + sum(O_Transform[H_DIS][jj]);
			if (LitterPools > 0) Dissolved[jj].TotSourceFlow += sum(O_Transform[L2_DIS]);
			if (FaecesPool > 0)  Dissolved[jj].TotSourceFlow += sum(O_Transform[F_DIS]);
			if (Microbes > 0)    Dissolved[jj].TotSourceFlow += sum(O_Transform[MIC_DIS]);
		}
		if (O_State_TotOrg[_N_] > 0)	CN_Ratio_TotSoil = O_State_TotOrg[_C_] / O_State_TotOrg[_N_];
		if (O_State_TotOrg[_P_] > 0)	CP_Ratio_TotSoil = O_State_TotOrg[_C_] / O_State_TotOrg[_P_];
		if (O_State_Profile[_H][_N_] > 0)	CN_Ratio_TotHumus = O_State_Profile[_H][_C_] / O_State_Profile[_H][_N_];
		if (O_State_Profile[_H][_P_] > 0)	CP_Ratio_TotHumus = O_State_Profile[_H][_C_] / O_State_Profile[_H][_P_];
	};


	if(DissolvedOrganics_Sw(DissolvedOrganics) > DissolvedOrganics_Sw::off) {
		DissolvedOrganicsConc();
		DissolvedOrganicsFlux();
		DissolvedOrganicsTransformation();
		SummationDissolvedOrganics();
	}

// Shift of Organic Growth of Layers
	size_t cycle;
	if(DisplacementOrganicLayers<3)
		cycle=366;
	else
		cycle=7;


	if(int(m_pModelInfo->JDayNum)%cycle==1&&DisplacementOrganicLayers>0&&!m_DisplacementDone) {
		if(DisplacementOrganicLayers==1||DisplacementOrganicLayers==3)
			MakeDisplacement();
		else
			MakeDisplacement2();
		m_DisplacementDone=true;
	}
	else if (int(m_pModelInfo->JDayNum)%cycle==2)
		m_DisplacementDone=false;


}
void NC_Organic::Integ()
{
	PlantModel* pPlant = (PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
	WaterUptake* p_Plant = &pPlant->m_WaterUptake;
	SoilModel* p_Soil = (SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	Soil_Frost* pSoil = &p_Soil->m_SoilFrost;
	/*	Use Timeinfo
		Use Profile, Only	: NL
		Use General, Only	: InflowScalar, OutFlowScalar, OutFlow,InFlow, Intflow
		Use CommonStrucN, Only	: SoilAmmonium, Denitrification, Denitrifier_Death

	!	Flows from Surface Litter to uppermost soil compartments*/

	for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
		for (size_t i = _L1; i <= _L2; i++) {
			InFlow(O_State[i][jj].front(), SurfaceLitterMixingIntoUppermostlayer[i][jj]);
			OutFlow(SurfaceLitter[i][jj], SurfaceLitterMixingIntoUppermostlayer[i][jj]);
		}
		if (Microbes_Sw(Microbes) > Microbes_Sw::off) {
			InFlow(O_State[_H][jj].front(), SurfaceLitterMixingIntoUppermostlayer[_H][jj]);
			OutFlow(SurfaceLitter[_L1][jj], SurfaceLitterMixingIntoUppermostlayer[_H][jj]);
		}
	}

	for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
		for (size_t i = _L1; i <= _L2; i++)
			InFlow(O_State[i][jj], LitterInputToSoilLayers[i][jj]);
	}
	/*
	//!   Flow from roots to Soil Litter 1
	InFlow(O_State[_L1][_C_], CRootsLitter1	);
	InFlow(O_State[_L1][_N_], NRootsLitter1	);
	InFlow(O_State[_L1][_C_], CFungiLitter1	);
	InFlow(O_State[_L1][_N_], NFungiLitter1	);
	

//!   Flow from death denitrifiers to Soil Litter 1
	

	if(LitterPools>0) {
		InFlow(O_State[_L2][_C_], CRootsLitter2	);
		InFlow(O_State[_L2][_N_], NRootsLitter2	);
	}
	*/
	if(p_Plant->TraceElementUptake) {
		InFlow(p_Plant->TE_Litter1.front(), p_Plant->TE_SurfaceLitter_L1);
		OutFlow(p_Plant->TE_SurfaceLitter, p_Plant->TE_SurfaceLitter_L1);

		InFlow(p_Plant->TE_Litter1	, p_Plant->TE_RootsLitter1		);
		OutFlow(p_Plant->TE_Litter1	, p_Plant->TE_Litter1HumusRate	);
		OutFlow(p_Plant->TE_Litter1	, p_Plant->TE_Litter1MinRate		);
		InFlow (p_Plant->TE_Humus	, p_Plant->TE_Litter1HumusRate	);
		OutFlow(p_Plant->TE_Humus	, p_Plant->TE_HumusMinRate		);
		InFlow(p_Plant->SaltStorage, p_Plant->TE_Litter1MinRate		);
		InFlow(p_Plant->SaltStorage, p_Plant->TE_HumusMinRate		);
 	}

/*
	OutFlow(SurfaceLitter[_L1][_C_], CSurfaceLitter_L1);
	OutFlow(SurfaceLitter[_L2][_C_], CSurfaceLitter_L2);
	OutFlow(SurfaceLitter[_L1][_C_], CSurfaceLitter_Hum);

	OutFlow(SurfaceLitter[_L1][_N_], NSurfaceLitter_L1);
	OutFlow(SurfaceLitter[_L2][_N_], NSurfaceLitter_L2);
	OutFlow(SurfaceLitter[_L1][_N_], NSurfaceLitter_Hum);
	*/
//! Organic N uptake

	if(OrganicUptake>=1) {
		OutFlow(O_State[_L1][_N_], O_Transform[L1_PLANTUPTAKE][_N_]	);
		OutFlow(O_State[_H][_N_],O_Transform[H_PLANTUPTAKE][_N_]	);
		if (m_pModelStructure->m_NumActPlantElements >= 3) {
			OutFlow(O_State[_L1][_P_], O_Transform[L1_PLANTUPTAKE][_P_]);
			OutFlow(O_State[_H][_P_], O_Transform[H_PLANTUPTAKE][_P_]);
		}
		if (p_NC_Plant->FungalGrowth > 0) {
		OutFlow(O_State[_L1][_N_], O_Transform[L1_FUNGIUPTAKE][_N_]);//HH
		OutFlow(O_State[_H][_N_], O_Transform[H_FUNGIUPTAKE][_N_]	);
		if (m_pModelStructure->m_NumActPlantElements >= 3) {
			OutFlow(O_State[_L1][_P_], O_Transform[L1_FUNGIUPTAKE][_P_]);
			OutFlow(O_State[_H][_P_], O_Transform[H_FUNGIUPTAKE][_P_]);
			}
		}
	}
	


	if(Microbes==0) {
//	Litter1
//		Carbon
		OutFlow(O_State[_L1][_C_], O_RespRate[_L1]	);
		OutFlow(O_State[_L1][_C_], O_Transform[_L1][_C_]	);

 //		Nitrogen
		OutFlow(O_State[_L1][_N_], O_Mineralisation[_L1][_N_-1]	);
		InFlow(SoilAmmonium, O_Mineralisation[_L1][_N_ - 1]);

		OutFlow(O_State[_L1][_N_], O_Transform[_L1][_N_]	);
// P
		OutFlow(O_State[_L1][_P_], O_Mineralisation[_L1][_P_ - 1]);
		InFlow(Soil_PO4, O_Mineralisation[_L1][_P_ - 1]);
		OutFlow(O_State[_L1][_P_], O_Transform[_L1][_P_]);




//	Litter2
//		Carbon
		if(LitterPools>0) {
			OutFlow(O_State[_L2][_C_], O_RespRate[_L2]	);
			OutFlow(O_State[_L2][_C_], O_Transform[_L2][_C_]	);

 //		Nitrogen
			OutFlow(O_State[_L2][_N_], O_Mineralisation[_L2][_N_-1]	);
			InFlow(SoilAmmonium, O_Mineralisation[_L2][_N_ - 1]);
			OutFlow(O_State[_L2][_N_], O_Transform[_L2][_N_]	);

	//		P
			OutFlow(O_State[_L2][_P_], O_Mineralisation[_L2][_P_ - 1]);
			InFlow(Soil_PO4, O_Mineralisation[_L2][_P_ - 1]);
			OutFlow(O_State[_L2][_P_], O_Transform[_L2][_P_]);
		}


//	Faeces
		if(FaecesPool>0) {
//			Carbon
			OutFlow(O_State[_F][_C_], O_RespRate[_F]	);
			OutFlow(O_State[_F][_C_], O_Transform[_F][_C_]	);
//		Nitrogen
			OutFlow(O_State[_F][_N_], O_Mineralisation[_F][_N_ - 1]);
			InFlow(SoilAmmonium, O_Mineralisation[_F][_N_ - 1]);
			OutFlow(O_State[_F][_N_], O_Transform[_F][_N_]	);
			//		P
			OutFlow(O_State[_F][_P_], O_Mineralisation[_F][_P_ - 1]);
			InFlow(Soil_PO4, O_Mineralisation[_F][_P_ - 1]);
			OutFlow(O_State[_F][_P_], O_Transform[_F][_P_]);
		}

//	Humus
//		Nitrogen
		InFlow(O_State[_H][_N_]	, O_Transform[_L1][_N_]	);	
		InFlow(O_State[_H][_C_]	, O_Transform[_L1][_C_]	);	
		InFlow(O_State[_H][_P_], O_Transform[_L1][_P_]);

		if(LitterPools>0) {
	 		InFlow(O_State[_H][_N_]	, O_Transform[_L2][_N_]	);	
			InFlow(O_State[_H][_C_]	, O_Transform[_L2][_C_]	);	
			InFlow(O_State[_H][_P_], O_Transform[_L2][_P_]);
		}
		if(FaecesPool>0) {
			InFlow(O_State[_H][_N_]	, O_Transform[_F][_N_]	);	
			InFlow(O_State[_H][_C_]	, O_Transform[_F][_C_]	);	
			InFlow(O_State[_H][_P_], O_Transform[_F][_P_]);
		}

		OutFlow(O_State[_H][_C_]	, O_RespRate[_H]  	);
		OutFlow(O_State[_H][_N_]	, O_Mineralisation[_H][_N_ - 1]);

		

		InFlow(SoilAmmonium, O_Mineralisation[_H][_N_ - 1]);
		if (m_pModelStructure->m_NumActPlantElements >= 3) {
			OutFlow(O_State[_H][_P_], O_Mineralisation[_H][_P_ - 1]);
			InFlow(Soil_PO4, O_Mineralisation[_H][_P_ - 1]);
			InFlow(AccInflowP, PTotOrgPO4Rate);
		}

	}
	else  {	// Microbes are included
//	Litter1
//		Carbon
		OutFlow(O_State[_L1][_C_], O_RespRate[_L1]	);
		OutFlow(O_State[_L1][_C_], O_Transform[L1_MIC][_C_]	);
		OutFlow(O_State[_L1][_C_], O_Transform[_L1][_C_]	);
//		Nitrogen
		OutFlow(O_State[_L1][_N_], O_Mineralisation[_L1][_N_ - 1]);
		OutFlow(O_State[_L1][_N_], O_Transform[L1_MIC][_N_]);
		OutFlow(O_State[_L1][_N_], O_Transform[_L1][_N_]	);

		if(LitterPools>0) {
			OutFlow(O_State[_L2][_C_], O_RespRate[_L2]		);
			OutFlow(O_State[_L2][_C_], O_Transform[L2_MIC][_C_]	);
			OutFlow(O_State[_L2][_N_], O_Transform[L2_MIC][_N_]	);
			OutFlow(O_State[_L2][_C_], O_Transform[_L2][_C_]	);
			OutFlow(O_State[_L2][_N_], O_Transform[_L2][_N_]	);
		}

//	Faeces
		if(FaecesPool>0) {
//		Carbon
			OutFlow(O_State[_F][_C_], O_RespRate[_F]		);
			OutFlow(O_State[_F][_C_], O_Transform[F_MIC][_C_]	);
			OutFlow(O_State[_F][_C_], O_Transform[_F][_C_]		);
//		Nitrogen
			OutFlow(O_State[_F][_N_], O_Transform[F_MIC][_N_]	);
			OutFlow(O_State[_F][_N_], O_Transform[_F][_N_]		);
		}

//	Humus
//		Nitrogen
		InFlow(O_State[_H][_N_]	, O_Transform[_L1][_N_]	);	
		InFlow(O_State[_H][_N_]	, O_Transform[_L2][_N_]	);	
		InFlow(O_State[_H][_N_]	, O_Transform[_F][_N_]	);	
		InFlow(O_State[_H][_C_]	, O_Transform[_L1][_C_]	);	
		InFlow(O_State[_H][_C_]	, O_Transform[_L2][_C_]	);	
		InFlow(O_State[_H][_C_]	, O_Transform[_F][_C_]	);	

		OutFlow(O_State[_H][_C_]	, O_RespRate[_H]  	);
		OutFlow(O_State[_H][_C_]	, O_Transform[H_MIC][_C_] );
		OutFlow(O_State[_H][_N_]	, O_Transform[H_MIC][_N_]	);

//	Microbes
//		Carbon
		InFlow(O_State[_MIC][_C_], O_Transform[L1_MIC][_C_]);
		InFlow(O_State[_MIC][_C_], O_Transform[H_MIC][_C_]);
		if(LitterPools>0)	InFlow(O_State[_MIC][_C_], O_Transform[L2_MIC][_C_]);
		if(FaecesPool>0)		InFlow(O_State[_MIC][_C_], O_Transform[F_MIC][_C_]);
		OutFlow(O_State[_MIC][_C_], O_RespRate[_MIC]);
//		Nitrogen
		InFlow(O_State[_MIC][_N_], O_Transform[L1_MIC][_N_]);
		InFlow(O_State[_MIC][_N_], O_Transform[H_MIC][_N_]);
		if(LitterPools>0)	InFlow(O_State[_MIC][_N_], O_Transform[L2_MIC][_N_]);
		if(FaecesPool>0)	InFlow(O_State[_MIC][_N_], O_Transform[F_MIC][_N_]);

		OutFlow(O_State[_MIC][_N_], O_Mineralisation[_MIC][_N_ - 1]);
		InFlow(SoilAmmonium, O_Mineralisation[_MIC][_N_ - 1]);
	}

//Dissolved organics
		if(DissolvedOrganics>0) {
			InFlow(O_State[_DIS][_C_], O_Transform[L1_DIS][_C_]);
			InFlow(O_State[_DIS][_N_], O_Transform[L1_DIS][_N_]);

			OutFlow(O_State[_L1][_C_], O_Transform[L1_DIS][_C_]);
			OutFlow(O_State[_L1][_N_], O_Transform[L1_DIS][_N_]);
			if (m_pModelStructure->m_NumActPlantElements >= 3) {
				InFlow(O_State[_DIS][_P_], O_Transform[L1_DIS][_P_]);
				OutFlow(O_State[_L1][_P_], O_Transform[L1_DIS][_P_]);
			}

			if(LitterPools>0) {
				InFlow(O_State[_DIS][_C_], O_Transform[L2_DIS][_C_]);
				OutFlow(O_State[_L2][_C_], O_Transform[L2_DIS][_C_]);
				InFlow(O_State[_DIS][_N_], O_Transform[L2_DIS][_N_]);
				OutFlow(O_State[_L2][_N_], O_Transform[L2_DIS][_N_]);
				if (m_pModelStructure->m_NumActPlantElements >= 3) {
					InFlow(O_State[_DIS][_P_], O_Transform[L2_DIS][_P_]);
					OutFlow(O_State[_L2][_P_], O_Transform[L2_DIS][_P_]);
				}
			}

 			if(FaecesPool>0) {
				InFlow(O_State[_DIS][_C_], O_Transform[F_DIS][_C_]);
				OutFlow(O_State[_L2][_C_], O_Transform[F_DIS][_C_]);
				InFlow(O_State[_DIS][_N_], O_Transform[F_DIS][_N_]);
				OutFlow(O_State[_L2][_N_], O_Transform[F_DIS][_N_]);
				if (m_pModelStructure->m_NumActPlantElements >= 3) {
					InFlow(O_State[_DIS][_P_], O_Transform[F_DIS][_P_]);
					OutFlow(O_State[_L2][_P_], O_Transform[F_DIS][_P_]);
				}
			}

  			if(Microbes>0) {
				InFlow(O_State[_DIS][_C_], O_Transform[MIC_DIS][_C_]);
				OutFlow(O_State[_MIC][_C_], O_Transform[MIC_DIS][_C_]);
				InFlow(O_State[_DIS][_N_], O_Transform[MIC_DIS][_N_]);
				OutFlow(O_State[_MIC][_N_], O_Transform[MIC_DIS][_N_]);
				if (m_pModelStructure->m_NumActPlantElements >= 3) {
					InFlow(O_State[_DIS][_P_], O_Transform[MIC_DIS][_P_]);
					OutFlow(O_State[_MIC][_P_], O_Transform[MIC_DIS][_P_]);
				}
			}


 			InFlow(O_State[_DIS][_C_], O_Transform[H_DIS][_C_]);
			OutFlow(O_State[_H][_C_], O_Transform[H_DIS][_C_]);
 			InFlow(O_State[_DIS][_N_], O_Transform[H_DIS][_N_]);
			OutFlow(O_State[_H][_N_], O_Transform[H_DIS][_N_]);
			if (m_pModelStructure->m_NumActPlantElements >= 3) {
				InFlow(O_State[_DIS][_P_], O_Transform[H_DIS][_P_]);
				OutFlow(O_State[_H][_P_], O_Transform[H_DIS][_P_]);
			}

			for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
				IntFlow(O_State[_DIS][jj], Dissolved[jj].VFlow);
				OutFlow(O_State[_DIS][jj], Dissolved[jj].HFlow);
			}
		}
}

bool NC_Organic::End()
{
return true;
}
NC_Organic::~NC_Organic(void)
{
}
void  NC_Organic::UptakeOrgReset() {
	for (size_t jj = 1; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
		SoilModel* p_Soil = (SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
		Soil_Frost* pSoil = &p_Soil->m_SoilFrost;
		O_Transform[L1_PLANTUPTAKE][jj].assign(pSoil->NL, 0.);
		O_Transform[H_PLANTUPTAKE][jj].assign(pSoil->NL, 0.);
		O_Transform[L1_FUNGIUPTAKE][jj].assign(pSoil->NL, 0.);
		O_Transform[H_FUNGIUPTAKE][jj].assign(pSoil->NL, 0.);
	}
}
