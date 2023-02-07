#include "Additional_BioticVariables_Functions.h"
#include "../PLANT_NC/PlantOrgModel.h"
#include "../PLANT_NC/NC_Plant.h"

Additional_BioticVariables_Functions::Additional_BioticVariables_Functions(void)
{


}
Additional_BioticVariables_Functions::~Additional_BioticVariables_Functions(void)
{
}


bool Additional_BioticVariables_Functions::Def()

{
	Additional_Variables::Def();
	ModuleNo=31;
	PlantOrgModel* pPlantOrg=(PlantOrgModel*)m_pModelInfo->GetModelPtr("Plant Org Model");
	NC_Plant *pNCPlant;
	pNCPlant=&pPlantOrg->m_NC_Plant;
	Xs *pXs; Ts *pTs; Gs *pGs;
	
	pXs= new Xs(&AccFertilization ,   ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,	"AccFertilization",		 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs= new Xs(&AccManure,  ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs, "AccNitrogenManure"	   	, 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs= new Xs(&AccExternalCarbon,  ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  "AccExternalCarbon"	   	, 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");

	pXs = new Xs(&Acc_P_Fertilization, ACCUMULATOR,	PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "Acc_P_Fertilization", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus> 0|N Fertilization >0");
	pXs = new Xs(&Acc_P_Manure, ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "Acc_P_Manure", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus> 0|N Fertilization >0");
	pXs = new Xs(&Acc_P_Weathering, ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "Acc_P_Weathering", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus> 0");

	pXs = new Xs(&AccTotPlantUptake[_N_], ACCUMULATOR, NITROGEN, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "AccTotNPlantUptake", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon > 0|Growth>1");
	pXs = new Xs(&AccTotPlantUptake[_P_], ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "AccTot_P_PlantUptake", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus> 0|Growth>1");



	pXs= new Xs(&AccAboveLitter[_C_],   ACCUMULATOR,CARBON, STORAGE,NOBPROC, NORMAL);
		Define(pXs,   "AccAboveLitterCarbon"	   	, 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs = new Xs(&AccAboveLitter[_N_], ACCUMULATOR, NITROGEN, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "AccAboveLitterN", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon > 0");
	pXs = new Xs(&AccAboveLitter[_P_], ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "AccAboveLitterP", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus> 0");
		
		
	pXs= new Xs(&AccBelowLitter[_C_],  ACCUMULATOR, CARBON, STORAGE,NOBPROC, NORMAL);
		Define(pXs,    "AccBelowLitterCarbon"	   	, 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs= new Xs(&AccBelowLitter[_N_],   ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  	    "AccBelowLitterN"	   	, 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs = new Xs(&AccBelowLitter[_P_], ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "AccBelowLitterP", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus> 0");
	
	pXs= new Xs(&AccPlantLitter[_N_],   ACCUMULATOR, CARBON, STORAGE,NOBPROC, NORMAL);
		Define(pXs,   "AccPlantLitterN"	   	, 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs= new Xs(&AccPlantLitter[_C_],   ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  	    "AccPlantLitterC"	   	, 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");	
	pXs = new Xs(&AccPlantLitter[_P_], ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "AccPlantLitterP", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus> 0");
		
		
		
	pXs= new Xs(&AccNInput,	ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  	"AccNitrogenInput"			, 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs= new Xs(&AccNOutput,  ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  		"AccNitrogenOutput"			, 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");

	pXs= new Xs(&AccDeposition,   ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  		"AccDeposition"			, 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs = new Xs(&Acc_P_Deposition, ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "Acc_P_Deposition", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus >0|Nitrogen and Carbon > 0");
	pXs = new Xs(&Acc_P_Mineralization, ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "Acc_P_Mineralization", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus >0|Nitrogen and Carbon > 0");




	pXs= new Xs(&AccNFixation,   ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  		"AccNFixation"			, 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");

	pXs= new Xs(&pNCPlant->AccHarvest[_N_]	, ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  		"AccNitrogenHarvest"  	, 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs = new Xs(&pNCPlant->AccHarvest[_C_], ACCUMULATOR, CARBON, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "AccCarbonHarvest", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon > 0");
	pXs = new Xs(&pNCPlant->AccHarvest[_P_], ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "Acc_P_Harvest", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus > 0");

	pXs= new Xs(&AccDenitrification,   ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  	"AccDenitrification"	, 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs= new Xs(&AccMinLeaching[_N_],   ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  "AccMinNLeaching"			, 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs = new Xs(&AccMinLeaching[_P_], ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "AccMin_P_Leaching", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus > 0");
	pXs = new Xs(&AccSurfaceRunOffLeaching[_P_], ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "Acc_P_SurfaceRunoff", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus >0");

		
	pXs= new Xs(&AccDisLeaching[_N_],   ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  		"AccDisNLeaching"		, 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs= new Xs(&AccDisLeaching[_C_],   ACCUMULATOR,CARBON, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  		"AccDisCLeaching"		, 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs = new Xs(&AccDisLeaching[_P_], ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "AccDis_P_Leaching", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus > 0");


	pXs= new Xs(&TotalDiffPlant[_N_],  ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  		"TotalDiffNPlant"		 , 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs= new Xs(&TotalDiffPlant[_C_],  ACCUMULATOR,CARBON, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  		"TotalDiffCPlant"		 , 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs = new Xs(&TotalDiffPlant[_P_], ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "TotalDiff_P_Plant", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus > 0");

	pXs = new Xs(&TotalDiffSoil[_C_], ACCUMULATOR, CARBON, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "TotalDiffCSoil", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon > 0");
	pXs= new Xs(&TotalDiffSoil[_N_],   ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
	    Define(pXs, "TotalDiffNSoil"		 , 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs = new Xs(&TotalDiffSoil[_P_], ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "TotalDiff_P_Soil", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus > 0");
	
	pXs = new Xs(&TotalDiffOrgSoil[_C_], ACCUMULATOR, CARBON, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "TotalDiffOrgCSoil", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon > 0");
	pXs= new Xs(&TotalDiffOrgSoil[_N_],   ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  	"TotalDiffOrgNSoil"		 , 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs = new Xs(&TotalDiffOrgSoil[_P_], ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "TotalDiffOrg_P_Soil", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus > 0");
	
	pXs = new Xs(&TotalDiffHumus[_C_], ACCUMULATOR, CARBON, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "TotalDiffCHumus", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon > 0");
	pXs= new Xs(&TotalDiffHumus[_N_],   ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  "TotalDiffNHumus"		 , 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs = new Xs(&TotalDiffHumus[_P_], ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "TotalDiff_P_Humus", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus > 0");

		pXs = new Xs(&TotalDiffLitter1[_C_], ACCUMULATOR, CARBON, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "TotalDiffCLitter1", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon > 0");
		pXs = new Xs(&TotalDiffLitter1[_N_], ACCUMULATOR, NITROGEN, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "TotalDiffNLitter1", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon > 0");
		pXs = new Xs(&TotalDiffLitter1[_P_], ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "TotalDiff_P_Litter1", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus > 0");

		pXs = new Xs(&TotalDiffLitter2[_C_], ACCUMULATOR, CARBON, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "TotalDiffCLitter2", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon > 0");
		pXs = new Xs(&TotalDiffLitter2[_N_], ACCUMULATOR, NITROGEN, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "TotalDiffNLitter2", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon > 0");
		pXs = new Xs(&TotalDiffLitter2[_P_], ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "TotalDiff_P_Litter2", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus > 0");

	pXs= new Xs(&TotalDiffMinSoil[_N_],   ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  	"TotalDiffMinNSoil"		 , 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs = new Xs(&TotalDiffMinSoil[_P_], ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "TotalDiffMin_P_Soil", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus > 0");

	pXs = new Xs(&TotalDiffSolidMineralPhosphorus, ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "TotalDiffSolidPMinerals", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus > 0");

	pXs = new Xs(&Soil_BalanceCheck[_C_], ACCUMULATOR, CARBON, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "Soil C BalanceCheck", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon > 0");
	pXs = new Xs(&Soil_BalanceCheck[_N_], ACCUMULATOR, NITROGEN, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "Soil N BalanceCheck", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon > 0");
	pXs = new Xs(&Soil_BalanceCheck[_P_], ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "Soil P BalanceCheck", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus > 0");

	pXs = new Xs(&TotalBalanceCheck[_C_], ACCUMULATOR, NITROGEN, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "CarbonBalanceCheck", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon > 0");
	pXs= new Xs(&TotalBalanceCheck[_N_],  ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  "NitrogenBalanceCheck"	 , 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs = new Xs(&TotalBalanceCheck[_P_], ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "PhosphorusBalanceCheck", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus > 0");

	pXs = new Xs(&Plant_BalanceCheck[_C_], ACCUMULATOR, CARBON, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "Plant C BalanceCheck", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon > 0");
	pXs= new Xs(&Plant_BalanceCheck[_N_],   ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  "Plant N BalanceCheck"	 , 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs = new Xs(&Plant_BalanceCheck[_P_], ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "Plant P BalanceCheck", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus> 0");

	pXs = new Xs(&TotalDiff_Eco[_C_], ACCUMULATOR, CARBON, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "TotalDiff C Eco", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon > 0");
	pXs= new Xs(&TotalDiff_Eco[_N_],   ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
	    Define(pXs,  	"TotalDiff N Eco"		 , 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs = new Xs(&TotalDiff_Eco[_P_], ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "TotalDiff P Eco", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus> 0");

		pXs = new Xs(&TotalDiff_Fungi[_C_], ACCUMULATOR, CARBON, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "TotalDiff C Fungi", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon > 0");
		pXs = new Xs(&TotalDiff_Fungi[_N_], ACCUMULATOR, NITROGEN, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "TotalDiff N Fungi", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon > 0");
		pXs = new Xs(&TotalDiff_Fungi[_P_], ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "TotalDiff P Fungi", 0.,UNIT_TYPES::MASS_UNIT, "Phosphorus> 0");




	pTs= new Ts(&NitrogenBalanceRate, ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pTs, 	"NitrogenBalanceRate"	 , 0.,UNIT_TYPES::MASSFLOW_UNIT,"Nitrogen and Carbon > 0");
 	pTs= new Ts(&N_SupplyRate, ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pTs, 	"N InputRate"	 , 0.,UNIT_TYPES::MASSFLOW_UNIT,"Nitrogen and Carbon > 0");
 	pTs= new Ts(&N_LossesRate, ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pTs, 	"N OutputRate"	 , 0.,UNIT_TYPES::MASSFLOW_UNIT,"Nitrogen and Carbon > 0");

	pTs= new Ts(&CarbonBalanceRate, ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
 		Define(pTs, 	"CarbonBalanceRate"	 , 0.,UNIT_TYPES::MASSFLOW_UNIT,"Nitrogen and Carbon > 0");
	pTs= new Ts(&CarbonFlux, ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
 		Define(pTs, 	"Carbon Flux"	 , 0.,UNIT_TYPES::MASSFLOW_UNIT,"Nitrogen and Carbon > 0");
	pTs= new Ts(&SoilRespOrg, ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
 		Define(pTs, 	"Soil Resp (no roots)"	 , 0.,UNIT_TYPES::MASSFLOW_UNIT,"Nitrogen and Carbon > 0");
	pTs= new Ts(&SoilRespTot, ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
 		Define(pTs, 	"Soil Respiration", 0.,UNIT_TYPES::MASSFLOW_UNIT,"Nitrogen and Carbon > 0");
	pTs= new Ts(&TotalPhotoSynt, ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
 		Define(pTs, 	"Total PhotoSynt", 0.,UNIT_TYPES::MASSFLOW_UNIT,"Nitrogen and Carbon > 0");
	pTs= new Ts(&TotalRespiration, ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
 		Define(pTs, 	"Total Respiration", 0.,UNIT_TYPES::MASSFLOW_UNIT,"Nitrogen and Carbon > 0");
	pXs= new Xs(&AccPhotosyntheses,   ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  	"AccPhotosyntheses"		 , 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs= new Xs(&AccTotalRespiration,   ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  	"AccTotalRespiration"	 , 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs= new Xs(&AccPlantRespiration,   ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  	"AccPlantRespiration"	 , 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs= new Xs(&AccSoilRespiration,   ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  	"AccSoilRespiration"	 , 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs = new Xs(&AccFungiRespiration, ACCUMULATOR, NITROGEN, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "AccFungiRespiration", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon > 0|Mycorrhiza Fungal Growth>0");
	pXs = new Xs(&AccFungiGrowth, ACCUMULATOR, NITROGEN, STORAGE, NOBPROC, NORMAL);
	Define(pXs, "AccFungiGrowth", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon > 0|Mycorrhiza Fungal Growth>0");
		pXs = new Xs(&AccFungiLitterFall, ACCUMULATOR, NITROGEN, STORAGE, NOBPROC, NORMAL);
		Define(pXs, "AccFungiLitterFall", 0.,UNIT_TYPES::MASS_UNIT, "Nitrogen and Carbon > 0|Mycorrhiza Fungal Growth>0");
	pXs= new Xs(&AccSoilOrgRespiration,   ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  	"AccSoilOrgRespiration"	 , 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs= new Xs(&AccCarbonBalance,   ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  	"AccCarbonBalance"		 , 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");
	pXs= new Xs(&AccNitrogenBalance,   ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  	"AccNitrogenBalance"		 , 0.,UNIT_TYPES::MASS_UNIT,"Nitrogen and Carbon > 0");

	pXs= new Xs(&AccTotAdsRadiation,   ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
		Define(pXs,  	"AccTotAdsRadiation"	 , 0.,"MJ/m2","Nitrogen and Carbon > 0|Growth>1");
		pGs= new Gs(&AccRadEfficiency, ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
 		Define(pGs, "AccRadEfficiency",0.0,"MJ/gC","Nitrogen and Carbon >0|Growth >=3");
	pGs= new Gs(&AccWaterEfficiency, ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
 		Define(pGs, "AccWaterEfficiency",0.0,"mm/gC","Nitrogen and Carbon >0|Growth >=3");
	pGs= new Gs(&AccNitrogenEfficiency, ACCUMULATOR,NITROGEN, STORAGE,NOBPROC, NORMAL);
 		Define(pGs, "AccNEfficiency",0.0,"gN/gC","Nitrogen and Carbon >0|Growth >=3");
	pGs = new Gs(&AccPhosphorusEfficiency, ACCUMULATOR, PHOSPHORUS, STORAGE, NOBPROC, NORMAL);
		Define(pGs, "Acc_P_Efficiency", 0.0, "gP/gC", "Phosphorus >0|Growth >=3");


	pGs = new Gs(&GWP_NEE[0], ATMBOUNDARY, GHG, EMISSION, LIVING, NORMAL);
	 Define(pGs, "GWP Eco - CO2", 0.0, "MgCO2e/ha/yr", "Nitrogen and Carbon > 0");
	pGs = new Gs(&GWP_NEE[1], ATMBOUNDARY, GHG, EMISSION, LIVING, NORMAL);
	 Define(pGs, "GWP - N2O", 0.0, "MgCO2e/ha/yr", "Nitrogen and Carbon > 0");
	pGs = new Gs(&GWP_NEE[2], ATMBOUNDARY, GHG, EMISSION, LIVING, NORMAL);
	 Define(pGs, "GWP - CH4", 0.0, "MgCO2e/ha/yr", "Nitrogen and Carbon > 0");
	pGs = new Gs(&GWP_NEE[3], ATMBOUNDARY, GHG, EMISSION, LIVING, NORMAL);
	 Define(pGs, "GWP Eco - All", 0.0, "MgCO2e/ha/yr", "Nitrogen and Carbon > 0");

	pGs = new Gs(&GWP_SOIL[0], ATMBOUNDARY, GHG, EMISSION, LIVING, NORMAL);
	 Define(pGs, "GWP Soil - CO2", 0.0, "MgCO2e/ha/yr", "Nitrogen and Carbon > 0");
	pGs = new Gs(&GWP_SOIL[1], ATMBOUNDARY, GHG, EMISSION, LIVING, NORMAL);
	 Define(pGs, "GWP Soil  - All", 0.0, "MgCO2e/ha/yr", "Nitrogen and Carbon > 0");
	
	 pGs = new Gs(&GWP_PLANT, ATMBOUNDARY, GHG, EMISSION, LIVING, NORMAL);
	 Define(pGs, "GWP Plant - CO2", 0.0, "MgCO2e/ha/yr", "Nitrogen and Carbon > 0");



return true;
}



