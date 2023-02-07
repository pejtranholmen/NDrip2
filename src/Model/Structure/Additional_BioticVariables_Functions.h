#pragma once
#include "./Additional_Variables.h"
class Additional_BioticVariables_Functions : public Additional_Variables
{
	public:
		Additional_BioticVariables_Functions(void);
		~Additional_BioticVariables_Functions(void);
		bool Def();

	//	Use CommonstrucN, Only  : AccCarbonHarvest,AccHarvest

//Use Profile, Only	: NL

	double AccFertilization, AccManure, AccDeposition, AccNFixation, AccExternalCarbon;

	double Acc_P_Fertilization, Acc_P_Manure, Acc_P_Deposition, Acc_P_Weathering;
	double Acc_P_Mineralization;

	double  AccDenitrification, AccMinLeaching[NUM_BIOELEM], AccDisLeaching[NUM_BIOELEM], AccSurfaceRunOffLeaching[NUM_BIOELEM];
	double TotalDiffPlant[NUM_BIOELEM];

	double TotalDiffOrgSoil[NUM_BIOELEM],  TotalDiffHumus[NUM_BIOELEM], TotalDiffLitter1[NUM_BIOELEM],TotalDiffLitter2[NUM_BIOELEM],  TotalDiffSoil[NUM_BIOELEM];
	double TotalDiffMinSoil[NUM_BIOELEM], TotalDiffSolidMineralPhosphorus, AccCarbonBalance, CarbonBalanceRate, AccNitrogenBalance, NitrogenBalanceRate;
	double TotalBalanceCheck[NUM_BIOELEM],  CarbonFlux;
    double Plant_BalanceCheck[NUM_BIOELEM], Soil_BalanceCheck[NUM_BIOELEM];

	double SoilRespOrg, SoilRespTot;
	double AccAboveLitter[NUM_BIOELEM],  AccBelowLitter[NUM_BIOELEM],  AccPlantLitter[NUM_BIOELEM];
	double TotalPhotoSynt, TotalRespiration;
   double N_SupplyRate, N_LossesRate, AccNInput, AccNOutput, TotalDiff_Eco[NUM_BIOELEM];
   double TotalDiff_Fungi[NUM_BIOELEM];
//!	Carbon Variables
	double AccPhotosyntheses, AccTotalRespiration, AccSoilRespiration, AccPlantRespiration;
	double AccFungiRespiration, AccFungiLitterFall, AccFungiGrowth;
	double AccSoilOrgRespiration, AccTotAdsRadiation, AccTotPlantUptake[NUM_BIOELEM];
	double AccRadEfficiency, AccWaterEfficiency, AccNitrogenEfficiency, AccPhosphorusEfficiency;

	double InitialStorageNPlant, InitialStorageCPlant;  //	! Local variable in module
	double InitialStorageOrgNSoil, InitialStorageOrgCSoil;  //	! Local variable in module
	double InitialStorage_SoilPMinerals;

   //double NIniSoil, NIniOrgSoil, NIniMinNSoil, CIniSoil, NIniPlant, CIniPlant;// !Local variables
   double IniPlant[NUM_BIOELEM], IniSoil[NUM_BIOELEM], IniOrgSoil[NUM_BIOELEM], IniMinSoil[NUM_BIOELEM], 
	   IniSoilHumus[NUM_BIOELEM], IniSoilLitter1[NUM_BIOELEM], IniSoilLitter2[NUM_BIOELEM], IniFungi[NUM_BIOELEM];
   double N_IrrigRateLocal;
   //double CIniSoilHumus, NIniSoilHumus;
   double TotAdsRadiation;
   double GWP_NEE[4], GWP_SOIL[2], GWP_PLANT;



};

