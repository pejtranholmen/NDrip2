#pragma once
#include "./P_Mineral.h"

class NC_Mineral_Functions : public P_Mineral
{
	public:
		NC_Mineral_Functions(void);
		~NC_Mineral_Functions(void);
		bool Def();

size_t 	InitialMineral_N,  Denit_Depth_Distribution, Ammonium_Mobility;
size_t		Nitrification, AmmoniaVolatilisation, Denitrification, Denit_Temp_Response, Nitrific_Temp_Response;
size_t	Initial_Nitrifier, NitrifierGrowth;
size_t  Flexible_Uptake, DenitrifierGrowth;
size_t DissolvedOrganics;

double DenitThetaRange, DenitThetaPowerC                    ;
double DenitTemQ10, DenitTemQ10Bas, DenitTemQ10Threshold      ;
double DenitTempMax, DenitTempMin      ;
double DenitNitrateHalfSat;
double RespTemQ10, RespTemQ10Bas, RespTemQ10Threshold      ;
double NitriTemQ10, NitriTemQ10Bas, NitriTemQ10Threshold      ;
double NitriTempMax, NitriTempMin ;

double  	InitNitrateConc,InitAmmoniumConc;
vector<double>   An_VolFraction;
double Eff_Microbes;



vector<double> 	SoilNitrateFlow,SoilNitrateDrainage ,InitNitrateConcZ,InitAmmoniumConcZ;//;
vector<double> 	SoilAmmoniumDrainage,SoilAmmoniumFlow,  SoilNitrateUptake, SoilAmmoniumUptake; //, SoilAmmoniumUptake;
vector<double>  	SoilNitrateConc,SoilAmmoniumConc,AmmoniumAdsFrac;
double 	NTotMinN_Drainage, NTotNO3_Drainage, NTotNH4_Drainage, NTotDenitrification;
double   NTotMin_PlantUptake, NTotNO3_PlantUptake, NTotNH4_PlantUptake;
double 	NTotNitrate, NTotAmmonium, NTotMinN_Soil, NTotNitrification;

double 	NTotMinNConc_Drainage, NTotNO3Conc_Drainage, NTotNH4Conc_Drainage;
//Denitrification parameters

vector<double>    DenitDistributionTab,DenitResponse,DenitrificationRate;
double 	DenitPotentialRate, DenitFracExpTail, DenitDepth;

// NH3 Volatilisation parameters

double  VolatRough, VolatDepth, PNH3_Atm, VolatEqCoef, VolatTempC1, VolatTempC2;

//// New Denitrification parameters

vector<double> 	Denitrifier_Biomass, Denitrifier_Growth, Denitrifier_Death;
vector<double>  Denitrifier_Activity, Denitrifier_ActivityRate;

vector<double>  An_Cons_NO3, An_Cons_NO2, An_Cons_NO, An_Cons_N2O ;
vector<double>  An_Prod_NO2, An_Prod_NO, An_Prod_N2O, An_Prod_N2 ;
vector<double>  An_NO3, An_NO2, An_NO, An_N2O, An_N2 ;

vector<double>  An_NO3_Conc, An_NO2_Conc, An_NO_Conc, An_N2O_Conc,An_N2_Conc;
double  N_Tot_AnarobicGasStorage, N_Tot_DenitMicrobeStorage, N_Tot_NitriMicrobeStorage;

double  N_Tot_DenitGrowthRate,N_Tot_NitriGrowthRate, N_Tot_DenitDeathRate,N_Tot_NitriDeathRate;
double   An_TotCons_NO3, An_TotCons_NO2, An_TotCons_NO, An_TotCons_N2O ;
double   An_TotProd_NO2, An_TotProd_NO, An_TotProd_N2O, An_TotProd_N2;
double   An_Tot_NO2, An_Tot_NO, An_Tot_N2O, An_Tot_N2, An_Tot_NO3;
double  N_Tot_DenitGrowth_NO3,N_Tot_DenitGrowth_NO2, N_Tot_DenitGrowth_NO,N_Tot_DenitGrowth_N2O;


double  DMic_GrowthCoef_NO3,DMic_GrowthCoef_NO2, DMic_GrowthCoef_NO, DMic_GrowthCoef_N2O;
double  NXOY_HalfRateCoef, NXOY_DOC_HalfRateCoef, InitDenitBiomass;
double  DMic_RespCoef_NO3,DMic_RespCoef_NO2, DMic_RespCoef_NO, DMic_RespCoef_N2O;
double  DMic_EffCoef_NO3,DMic_EffCoef_NO2, DMic_EffCoef_NO, DMic_EffCoef_N2O;

double  DMic_DeathRateCoef, DeniActivityRateCoef, DMic_DeathCDormancy;
double  DMic_DormancyThreshold;


double  D_pH_HalfCoef, D_pH_ShapeCoef;

double  D_InhiHalfRateNO3_N2O;
;
;
;
// Nitrification 
double 	Nitr_Amm_Ratio, Nitr_Specific_Rate;

vector<double>   Nitri_pH_Response,NitrificationRate ;
// Nitrification microbial base approach


vector<double>	Nitrifier_Biomass;	// State
vector<double>	Nitrifier_GrowthRate, Nitrifier_DeathRate, Nitrifier_RespRate;	// Fluxes
double 	NitriGrowthCoef, NitriHalfRateDOC, NitriHalfRateNO3;	  // Activity parameters
double 	NitriDeathCoef;	  // Activity parameters
double 	InitNitrifier, InitNitriFracExpTail, NitrifierDepth;	  // Initial Nitrifier parameters
double   NitrificationRateCoef, NitriHalfRateNH4;
// Tables

vector<P*> mTab_DenitPot, mTab_InitMin, mTab_Nitrific, mTab_AmIso, mTab_Flex;

// Mineral N Uptake
double  NUptMaxAvailFrac, NUptFlexDegree,NUptRootCoefMin, PUptMaxAvailFrac;

vector<double> NUptPlantFlexDegree;
double   FungiMinEnhanceCoef, FungiMinPEnhanceCoef, FungiNH4_Enhance, FungiCoverRedCoef;
double   FungiPO4EnhanceCoef;

//	Local Variable

vector<double> DenitDistribution, UptNitrate, UptAmmonium, FungiUptNitrate, FungiUptAmmonium;

vector<double> An_NXOY;

vector<double> DMic_Growth_NO3, DMic_Growth_NO2, DMic_Growth_NO, DMic_Growth_N2O, DMic_Growth_NXOY;

vector<double> DMic_RespGrowth_NO3, DMic_RespGrowth_NO2, DMic_RespGrowth_NO, DMic_RespGrowth_N2O;
vector<double> DMic_RespMaint_NO3, DMic_RespMaint_NO2, DMic_RespMaint_NO, DMic_RespMaint_N2O;

size_t NUMDENIT;

//Local Variables for Denitrification Submodel added 28 sept 2008 /PEJ
vector<double> RatioNO3,    RatioNO2,   RatioNO,    RatioN2O;
double      IniGasMicrobeStorage;


double NitrificationFunction(double NH4, double N03);
double NitrifierGrowthFunctionL(size_t Layer);
double NitriGrowthDOCFunction(double Value);
double NitriGrowthNitrateFunction(double Value);
double NitrifierDeathFunctionL(size_t Layer);
double NitrifierRespFunctionL(size_t Layer);
double NitrificationNH4FunctionL(size_t Layer);
double NitrificationNH4Function(double Value);
double DenitriInhibitNO3L(size_t Layer);
double DenitriInhibitNO3(double value);
double LiquidConcL(size_t Layer,double Value);
double LiquidConcAnFractionL(size_t Layer,double Value);
double GrowthResponseNO3_L(size_t Layer);
double GrowthResponseNO2_L(size_t Layer);
double GrowthResponseNO_L(size_t Layer);
double GrowthResponseN2O_L(size_t Layer);
double GrowthResponseDOC_L(size_t Layer);
double GrowthResponseDOC(double value);
double GrowthResponseNXOY(double Value);
double DMic_pH_ResponseL(size_t Layer);
double DMic_pH_Response(double value);
double DMic_N_ResponseL(size_t Layer);
double DMic_N_Response(double Value);
double DMic_TempFunctionL(size_t Layer);
double NH3_VolatL(size_t Layer);
double Conduct_NH3(double Wind, double Depth);
double Volat_TempConductFunction(double Temp);
double SoilPressureNH3(double NH4Conc, double  Ph);
double NUptMaxAvailFrac_Function(double RootDensity, size_t PlantIndex);
double FungiNUptMaxAvailFrac_Function(double RootDensity,size_t PlantIndex, bool LAmmonium) ;
double DenitNitrateFunction(double Nitrate);
Plant_Uptake NUptake(vector<double> Dist, size_t NUM, size_t index, double N_demand, double N_Tot_Demand,  vector<double> Dist_Tot,double factNdemandFungi,double FungiNitrateUptpot,double FungiAmmoniumUptpot,double N_FungiDemand, double N_Tot_FungiDemand);
void NitrogenUptakeResetForAllSoilLayers();


function<double(double)> f_DenitTempResponse, f_NitriTempResponse;
function<double(size_t, double)> f_DenitThetaResponse;


};

