#include "Additional_BioticVariables.h"
#include "../PLANT_NC/PlantOrgModel.h"
#include "../SOIL_NC/SoilOrgModel.h"
#include "../Atmosphere/AtmModel.h"
#include "../Structure/Structure.h"
void Additional_BioticVariables::setAdditional_BioticVariables(ModelInfo* pInfo, ModelMap* pMap, Structure* pStruc)
{
	ModuleName = "Additional Biotic Variables";
	ModuleNo = 31;
	ModuleConditions = "";
	m_pModelInfo = pInfo; m_pModelMap = pMap; m_pModelStructure = pStruc;
	pT_Step = &m_pModelInfo->T_Step;
	T_Step = *pT_Step;
}
Additional_BioticVariables::Additional_BioticVariables(void)
{
	ModuleName="Additional Biotic Variables";
	ModuleNo=31;
	ModuleConditions="";
}

bool Additional_BioticVariables::Ini()	{
	
	PlantOrgModel* pPlantOrg=(PlantOrgModel*)m_pModelInfo->GetModelPtr("Plant Org Model");
	NC_Plant *pNCPlant;
	pNCPlant=&pPlantOrg->m_NC_Plant;
	SoilOrgModel* pSoilOrg=(SoilOrgModel*)m_pModelInfo->GetModelPtr("Soil Org Model");
	NC_SoilManagement *pNCSoil;
	pNCSoil=&pSoilOrg->m_NC_SoilOrg;


	Additional_Variables::Ini();
	T_Step=*pT_Step;

			AccFertilization	=0.;
			AccManure			=0.;
			Acc_P_Manure = 0.;
			AccExternalCarbon   =0.;
			AccNFixation		=0.;
			AccDeposition		=0.; 
			Acc_P_Deposition = 0.;//HH
			AccDenitrification	=0.;
			N_IrrigRateLocal = 0.;
		
			for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
				TotalDiffPlant[jj] = 0.;
				TotalDiffSoil[jj] = 0.;
				TotalDiffOrgSoil[jj] = 0.;
				TotalDiffHumus[jj] = 0.;
				TotalDiffMinSoil[jj] = 0.;
				TotalBalanceCheck[jj] = 0.;
				Plant_BalanceCheck[jj] = 0.;
				AccMinLeaching[jj] = 0.;
				AccDisLeaching[jj] = 0.;
				pNCPlant->AccHarvest[jj] = 0.;
				AccAboveLitter[jj] = 0.;
				AccBelowLitter[jj] = 0.;
				AccPlantLitter[jj] = 0.;
				AccTotPlantUptake[jj] = 0.;
			}
			AccPhotosyntheses	  =0.;
			AccTotalRespiration	  =0.;
			AccPlantRespiration	  =0.;
			AccSoilRespiration	  =0.;
			AccSoilOrgRespiration =0.;
			AccCarbonBalance	  =0.;
		;


            AccNitrogenBalance=0.;
            NitrogenBalanceRate=0.;
            N_SupplyRate=0.;
            N_LossesRate=0.;	
            AccNInput=0.;
            AccNOutput=0.;	
            AccNInput=0.;
            AccNOutput=0.;					

			for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
				IniSoil[jj] = pNCSoil->O_State_TotOrg[jj];
				IniOrgSoil[jj] = pNCSoil->O_State_TotOrg[jj];
				IniSoilHumus[jj] = pNCSoil->O_State_Profile[_H][jj];
				IniSoilLitter1[jj] = pNCSoil->O_State_Profile[_L1][jj];
				IniSoilLitter2[jj] = pNCSoil->O_State_Profile[_L2][jj];

				IniPlant[jj]= pNCPlant->P_AllPlants[_PLANT_WHOLE][jj];
				if (jj == 0) { //Carbon
					IniMinSoil[_C_] = 0.;
					IniFungi[_C_] = 0.;


				}
				else if(jj==_N_) {
					IniSoil[_N_] += pNCSoil->NTotMinN_Soil;
					IniMinSoil[_N_] = pNCSoil->NTotMinN_Soil;
					IniFungi[_N_] = 0.;

				}
				else if (jj == _P_) {
					InitialStorage_SoilPMinerals = pNCSoil->PTot_PSolidMinerals;
					IniMinSoil[_P_] = pNCSoil->PTotPhosphate_Soil;
					IniFungi[_P_] = 0.;
					
					Acc_P_Weathering= Acc_P_Mineralization = AccSurfaceRunOffLeaching[_P_]= 0.;
					TotalDiffSolidMineralPhosphorus = pNCSoil->PTot_PSolidMinerals- InitialStorage_SoilPMinerals;
				}
			}
	AccWaterEfficiency=	  0.	;
	AccRadEfficiency=	   0.	;
	AccNitrogenEfficiency=	0.;
	TotalDiffOrgSoil[_N_]=pNCSoil->O_State_TotOrg[_N_]+pNCSoil->N_Tot_DenitMicrobeStorage+pNCSoil->N_Tot_NitriMicrobeStorage-IniOrgSoil[_N_]-pNCSoil->IniGasMicrobeStorage;	
	return true;
}
bool Additional_BioticVariables::Dyn()
{

		//p_Soil_WaterF->Dyn();
	//if(pMod->T_CountIter%pMod->T_RatioOrgStep-1==0&&pMod->T_RatioOrgStep>1||pMod->T_RatioOrgStep==1) {
	T_Step=m_pModelInfo->T_Step;
		Additional_Variables::Dyn();
		Flux();
		Integ();
	//}
	TotalDiffStorage=GetTotalWaterStorage()-InitialTotalStorage;




	return true;
}

void Additional_BioticVariables::Flux()
{
	
}
void Additional_BioticVariables::Integ()
{
	PlantOrgModel* pPlantOrg=(PlantOrgModel*)m_pModelInfo->GetModelPtr("Plant Org Model");
	NC_Plant *pNCPlant;
	pNCPlant=&pPlantOrg->m_NC_Plant;
	SoilOrgModel* pSoilOrg=(SoilOrgModel*)m_pModelInfo->GetModelPtr("Soil Org Model");
	NC_SoilManagement *pNCSoil;
	pNCSoil=&pSoilOrg->m_NC_SoilOrg;
	AtmModel* pAtm=(AtmModel*)m_pModelInfo->GetModelPtr("Atm Model");
	Irrigation *pIrrig;
	pIrrig=&pAtm->m_Irrigation;

 double	 PlantPhotoSynt, PlantResp, CarbonBalance, TotResp, TotalFixedCarbon;

 if (m_pModelStructure->m_NumActPlantElements >= 3) { //Mass balance test for phosphorus
	 TotalBalanceCheck[_P_] = pNCSoil->AccInflowP - Acc_P_Mineralization + Acc_P_Manure - pNCPlant->AccHarvest[_P_]
		 - AccMinLeaching[_P_] - AccDisLeaching[_P_] - AccSurfaceRunOffLeaching[_P_] - TotalDiffPlant[_P_]
		 - TotalDiffSoil[_P_];

	 Plant_BalanceCheck[_P_] = AccTotPlantUptake[_P_] - AccPlantLitter[_P_] - pNCPlant->AccHarvest[_N_] - TotalDiffPlant[_P_];
	 Soil_BalanceCheck[_P_] = AccPlantLitter[_P_] + pNCSoil->AccInflowP- Acc_P_Mineralization + Acc_P_Manure +
		 - AccTotPlantUptake[_P_] - AccMinLeaching[_P_] - AccSurfaceRunOffLeaching[_P_] - AccDisLeaching[_P_] - TotalDiffSoil[_P_] - pNCSoil->P_Fert;
 }




 for (size_t jj = 0; jj < m_pModelStructure->m_NumActPlantElements; jj++) {
	 TotalDiffOrgSoil[jj] = pNCSoil->O_State_TotOrg[jj]-IniOrgSoil[jj];//HH
	 TotalDiffHumus[jj] = pNCSoil->O_State_Profile[_H][jj]-IniSoilHumus[jj];
	 TotalDiffLitter1[jj] = pNCSoil->O_State_Profile[_L1][jj] - IniSoilLitter1[jj];
	 TotalDiffLitter2[jj] = pNCSoil->O_State_Profile[_L2][jj] - IniSoilLitter2[jj];


	 TotalDiffPlant[jj] = pNCPlant->P_AllPlants[_PLANT_WHOLE][jj]- IniPlant[jj];
	 if (FungalGrowth_Sw(pNCPlant->FungalGrowth) > FungalGrowth_Sw::off) {
		 TotalDiff_Fungi[jj] = pNCPlant->P_AllPlants[_FUNGI_][jj] - IniFungi[jj];
	 }
	 InFlow(pNCPlant->AccHarvest[jj], pNCPlant->TotHarvest[jj]);

	 InFlow(AccBelowLitter[jj], pNCPlant->TotPlantLitter[jj] - pNCPlant->TotAbovePlantLitter[jj]);
	 InFlow(AccAboveLitter[jj], pNCPlant->TotAbovePlantLitter[jj]);
	 InFlow(AccPlantLitter[jj], pNCPlant->TotPlantLitter[jj]);


	 if (jj == 0) { //Carbon
		 InFlow(AccExternalCarbon, pNCSoil->C_Manure_Litter);
		 InFlow(AccExternalCarbon, pNCSoil->C_Manure_Faeces);
		 TotalDiffMinSoil[jj] = 0.;
	 }
	 else if (jj == _N_) { //Nitrogen
		 
		 TotalDiffOrgSoil[_N_] = pNCSoil->O_State_TotOrg[jj]- IniOrgSoil[jj]; ;
		 TotalDiffMinSoil[_N_] = pNCSoil->NTotMinN_Soil - IniMinSoil[_N_];

		 InFlow(AccMinLeaching[jj], pNCSoil->NTotMinN_Drainage);
		 InFlow(AccMinLeaching[jj], pNCSoil->SoilNitrateFlow.back());
		 InFlow(AccMinLeaching[jj], pNCSoil->SoilAmmoniumFlow.back());

		 InFlow(AccDeposition, pNCSoil->N_Deposition_NO3);
		 InFlow(AccDeposition, pNCSoil->N_Deposition_NH4);

		 for (size_t i = 0; i < pNCPlant->NFixationPlant.size(); i++) {
			 InFlow(AccNFixation, pNCPlant->NFixationPlant[i]);
			 InFlow(AccNFixation, pNCPlant->NPlantAddSupply[i]);
			 InFlow(AccDeposition, pNCSoil->N_Deposition_Leaf[i]);
		 }

		 InFlow(AccFertilization, pNCSoil->N_Fert_App);

		 InFlow(AccManure, pNCSoil->N_Manure_Faeces);
		 InFlow(AccManure, pNCSoil->N_Manure_NH4);
		 InFlow(AccManure, pNCSoil->N_Manure_Litter);

		 InFlow(AccDenitrification, pNCSoil->NTotDenitrification);

		 if (pNCSoil->N_Irrigation > 0) {
			 if (pIrrig != NULL) {
				 InFlow(AccFertilization, pNCSoil->N_Irrig_NO3);
				 InFlow(AccFertilization, pNCSoil->N_Irrig_NH4);
				 N_IrrigRateLocal = pNCSoil->N_Irrig_NO3 + pNCSoil->N_Irrig_NH4;
			 }
			 else {
				 for (size_t i = 0; i < pNCSoil->SoilNitrateInflow.size(); i++) {
					 InFlow(AccFertilization, pNCSoil->SoilNitrateInflow[i]);
					 InFlow(AccFertilization, pNCSoil->SoilAmmoniumInflow[i]);
				 }
				 N_IrrigRateLocal = sum(pNCSoil->SoilNitrateInflow) + sum(pNCSoil->SoilAmmoniumInflow);
			 }
		 }
		 InFlow(AccTotPlantUptake[_N_], pNCPlant->Tot_PlantUptake[_N_ - 1]);

	 }
	 else if (jj == _P_) { // Phosphorus
		 TotalDiffMinSoil[_P_]= pNCSoil->PTotPhosphate_Soil- IniMinSoil[_P_];
		 TotalDiffSolidMineralPhosphorus = pNCSoil->PTot_PSolidMinerals - InitialStorage_SoilPMinerals;
		 InFlow(AccSurfaceRunOffLeaching[_P_], pNCSoil->PSurfRunoff);
		 InFlow(AccMinLeaching[jj], pNCSoil->PTotPO4_Drainage);
		 InFlow(Acc_P_Fertilization, pNCSoil->P_Fert_App);
		 InFlow(AccTotPlantUptake[_P_], pNCPlant->Tot_PlantUptake[_P_ - 1]);
		 InFlow(Acc_P_Weathering, pNCSoil->Tot_PWeatheringRate_Soil);
		 InFlow(Acc_P_Deposition, pNCSoil->Dep_P_DryRate);
		 InFlow(Acc_P_Mineralization, pNCSoil->PTotOrgPO4Rate);

	 }

	 if (jj == _P_) {
		 TotalDiffSoil[jj] = TotalDiffOrgSoil[jj] + TotalDiffMinSoil[jj]+ TotalDiffSolidMineralPhosphorus;
		 if (FungalGrowth_Sw(pNCPlant->FungalGrowth) > FungalGrowth_Sw::off)    TotalDiffSoil[jj] += TotalDiff_Fungi[jj];
		 TotalDiff_Eco[jj] = TotalDiffPlant[jj] + TotalDiffSoil[jj];
	 }
	 else {
		 TotalDiffSoil[jj] = TotalDiffOrgSoil[jj] + TotalDiffMinSoil[jj];
		 if (FungalGrowth_Sw(pNCPlant->FungalGrowth) > FungalGrowth_Sw::off) TotalDiffSoil[jj] += TotalDiff_Fungi[jj];
		 TotalDiff_Eco[jj] = TotalDiffPlant[jj] + TotalDiffSoil[jj];

	 }
	 InFlow(AccDisLeaching[jj], pNCSoil->Dissolved[jj].TotHFlow);
 }




  N_SupplyRate=pNCSoil->N_Fert_App+N_IrrigRateLocal+sum(pNCSoil->N_Deposition_Leaf)+sum(pNCPlant->NFixationPlant)+sum(pNCPlant->NPlantAddSupply)+
                pNCSoil->N_Manure_Faeces+pNCSoil->N_Manure_Litter+pNCSoil->N_Manure_NH4+pNCSoil->N_Deposition_NO3+pNCSoil->N_Deposition_NH4;
  N_LossesRate=pNCPlant->TotHarvest[_N_] +pNCSoil->NTotDenitrification+pNCSoil->Dissolved[_N_].TotHFlow +pNCSoil->NTotMinN_Drainage;
  NitrogenBalanceRate=N_SupplyRate-N_LossesRate;
   
  InFlow(AccNInput, N_SupplyRate);
  InFlow(AccNOutput, N_LossesRate);

  TotalBalanceCheck[_N_]=AccFertilization+AccManure+AccDeposition+AccNFixation- pNCPlant->AccHarvest[_N_]-AccDenitrification-AccMinLeaching[_N_]
					   -AccDisLeaching[_N_]-TotalDiffPlant[_N_]-TotalDiffMinSoil[_N_]-TotalDiffOrgSoil[_N_]-pNCSoil->N_Fert 
					   - pNCSoil->N_Tot_AnarobicGasStorage+pNCPlant->N_Seed_Added;			   //!N_Tot_NitriMicrobeStorage+

  Plant_BalanceCheck[_N_]=AccTotPlantUptake[_N_]-AccPlantLitter[_N_]- pNCPlant->AccHarvest[_N_]-TotalDiffPlant[_N_]+pNCPlant->N_Seed_Added;

  Soil_BalanceCheck[_N_]=AccPlantLitter[_N_]+AccFertilization+AccManure+AccDeposition-AccTotPlantUptake[_N_]
  - AccMinLeaching[_N_]-AccDisLeaching[_N_] -AccDenitrification-TotalDiffSoil[_N_] -pNCSoil->N_Fert 
  - pNCSoil->N_Tot_AnarobicGasStorage;

  TotalBalanceCheck[_C_]=AccPhotosyntheses-AccTotalRespiration- pNCPlant->AccHarvest[_C_]-TotalDiffPlant[_C_]-TotalDiffOrgSoil[_C_]
					-AccDisLeaching[_C_]+AccExternalCarbon+pNCPlant->C_Seed_Added;


  SoilRespOrg=sum(pNCSoil->O_RespRate[_L1])+sum(pNCSoil->O_RespRate[_L2])+sum(pNCSoil->O_RespRate[_H])+sum(pNCSoil->O_RespRate[_MIC]) 
			+sum(pNCSoil->O_RespRate[_F]);
//!			+DBLE(CTotDisOrgDecomp)

  double TotalFungiResp= sum(pNCPlant->P_Respiration[_FUNGI]);
  SoilRespTot=SoilRespOrg;
  SoilRespTot+=sum(pNCPlant->P_Respiration[_ROOT]);
 SoilRespTot+=sum(pNCPlant->P_Respiration[_OLD_ROOT]);
 SoilRespTot+=sum(pNCPlant->P_Respiration[_COARSE_ROOT]);
 SoilRespTot+=sum(pNCPlant->P_Respiration[_OLD_COARSE_ROOT]);
 SoilRespTot += TotalFungiResp; // Moved Fungi Respiration to be part of plant respiration instead of Soil Org.

 InFlow(AccFungiRespiration, TotalFungiResp);
  InFlow(AccSoilOrgRespiration, SoilRespOrg);

  double TotalFungiGrowth = sum(pNCPlant->P_ReAllocation[NEWMOBILE_FUNGI][_C_]);
  InFlow(AccFungiGrowth, TotalFungiGrowth);
  
  double TotalFungiLitter = sum(pNCPlant->P_LitterFall[_FUNGI][_C_]);
  InFlow(AccFungiLitterFall, TotalFungiLitter);

  PlantPhotoSynt=sum(pNCPlant->AtmNewMobile);
  TotalPhotoSynt=PlantPhotoSynt;

  InFlow(AccPhotosyntheses, PlantPhotoSynt);

  PlantResp=sum(pNCPlant->P_Respiration[_STEM])+sum(pNCPlant->P_Respiration[_LEAF])+sum(pNCPlant->P_Respiration[_ROOT])+sum(pNCPlant->P_Respiration[_GRAIN]) 
			+sum(pNCPlant->P_Respiration[_OLD_ROOT])+sum(pNCPlant->P_Respiration[_OLD_LEAF])+sum(pNCPlant->P_Respiration[_OLD_STEM])+sum(pNCPlant->P_Respiration[_COARSE_ROOT])+sum(pNCPlant->P_Respiration[_OLD_COARSE_ROOT]);

  InFlow(AccPlantRespiration, PlantResp);	
  
  CarbonBalance=PlantPhotoSynt-PlantResp-SoilRespOrg-pNCPlant->TotHarvest[_C_]-pNCSoil->Dissolved[_C_].TotHFlow;
  Plant_BalanceCheck[_C_]=AccPhotosyntheses-AccPlantLitter[_C_]- pNCPlant->AccHarvest[_C_]-TotalDiffPlant[_C_]+pNCPlant->C_Seed_Added-AccPlantRespiration-AccFungiGrowth;

  Soil_BalanceCheck[_C_] = AccPlantLitter[_C_] + AccExternalCarbon+ AccFungiGrowth -AccFungiRespiration
	  - AccDisLeaching[_C_] - AccSoilOrgRespiration -  TotalDiffOrgSoil[_C_];

  

  TotalRespiration=PlantResp+SoilRespOrg+TotalFungiResp;

  if(pNCSoil->FaecesPool>0) CarbonBalance+=pNCSoil->C_Manure_Litter+pNCSoil->C_Manure_Faeces;

  TotResp=PlantResp+SoilRespOrg+TotalFungiResp+ pNCPlant->GrazingRespiration+ pNCPlant->WoodEatingRespiration+ pNCPlant->WoodEaterRespFlux;
 
  TotAdsRadiation=sum(pNCPlant->RadPlantAdsorbed)/1.E6;
  InFlow(AccTotAdsRadiation,TotAdsRadiation );



  InFlow(AccCarbonBalance, CarbonBalance);	
  InFlow(AccTotalRespiration, TotResp);
  
  InFlow(AccNitrogenBalance, NitrogenBalanceRate);		
  InFlow(AccSoilRespiration, SoilRespTot);

  
  CarbonBalanceRate=PlantPhotoSynt-TotResp;//          !-CTotDisOrgDrainage REMOVED AND mOVED TO CarbonBalance 15/8 2006
  CarbonFlux=-CarbonBalanceRate;

  TotalFixedCarbon=AccPhotosyntheses-AccPlantRespiration;

  if(TotalFixedCarbon>0.01) {
		AccWaterEfficiency=(AccSoilEvapAndTransp-AccSoilEvap)/TotalFixedCarbon;
		AccRadEfficiency=AccTotAdsRadiation/TotalFixedCarbon;
		AccNitrogenEfficiency=AccTotPlantUptake[_N_]/TotalFixedCarbon;
		AccPhosphorusEfficiency = AccTotPlantUptake[_P_] / TotalFixedCarbon;
  }


  GWP_NEE[0] = CarbonFlux * Fix::GWP_C_CO2;
  GWP_NEE[1] = pNCSoil->N2O_Rate*Fix::GWP_N_N2O;
  GWP_NEE[2] = pNCSoil->CH4_TotSysFlux *Fix::GWP_C_CH4;
  GWP_NEE[3] = GWP_NEE[0] + GWP_NEE[1] + GWP_NEE[2];

  GWP_SOIL[0] = SoilRespOrg * Fix::GWP_C_CO2;
  GWP_SOIL[1] = GWP_SOIL[0] +GWP_NEE[1] + GWP_NEE[2];

  GWP_PLANT = (PlantResp-PlantPhotoSynt) * Fix::GWP_C_CO2;


}

bool Additional_BioticVariables::End()
{
return true;
}
Additional_BioticVariables::~Additional_BioticVariables(void)
{
}
