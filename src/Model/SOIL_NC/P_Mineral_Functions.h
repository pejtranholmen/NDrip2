#pragma once
#include "./AbioticResponses.h"
struct Plant_Uptake {
	double p_uptake; 
	double fungi_uptake;
};
class P_Mineral_Functions : public AbioticResponses
{
public:
	P_Mineral_Functions(void);
	~P_Mineral_Functions(void);
	bool Def();

	//add Phosphorus by Hongxing 18-10-25
	size_t 	  InitialMineral_P, Flexible_Uptake_P;
	size_t		 Weathering, Weath_Temp_Response;

	double Weath_RateCoef; 
	double Weath_pH_Shape, Weath_pH0;
	double Langmuir_max, Langmuir_c50;

	double  	InitPhosphateConc, InitMineral_P_Content;


	vector<double>  Soil_PO4_Sol, Soil_PO4_Conc, Soil_Mineral_P, UptPhosphate, FungiUptPhosphate;

	vector<double> 	SoilPhosphateFlow, SoilPhosphateDrainage, InitPhosphateConcZ, InitMineral_P_ContentZ;//;
	vector<double> 	 SoilPhosphateUptake, SoilPAdsorptionFlux; //, SoilAmmoniumUptake;
	vector<double> PUptPlantFlexDegree;


	vector<P*> mTab_InitConcP;
	double 	PTotPO4_Drainage;
	double   PTotPO4_PlantUptake;
	double 	 PTotPhosphate_Soil, PTot_PSolidMinerals;

	double PSurfRunoff;

	double  PTotPO4Conc_Drainage, Psurf_Conc1, Psurf_Conc2, Psurf_ThresFlow, Tot_MinP_DrainageConc;//HH
	double  Tot_P_DrainageConc;
	double  PUptMaxAvailFrac, PUptFlexDegree, PUptRootCoefMin, FungiCoverRedCoefP, FungiPEnhanceCoef, FungiPO4_Enhance;

	double PMineral_MassBalanceCheck, AccInflowP, AccOutFlowP, TotDiffStorageP, InitialStorageP;

	vector<double>     WeatheringResponse, WeatheringRate;
	double Tot_PWeatheringRate_Soil;
	// local

	vector<double> MassPerLayer;

	double Weath_pH_Response(double value);
	double PUptMaxAvailFrac_Function(double RootDensity, size_t PlantIndex);
	double FungiPUptMaxAvailFrac_Function(double RootDensity, size_t PlantIndex, bool LPhosphate);
	Plant_Uptake PUptake(vector<double> Dist, size_t NUM, size_t index, double P_demand, double P_Tot_Demand, vector<double> Dist_Tot, double factPdemandFungi, double FungiPhosphateUptpot, double P_FungiDemand, double P_Tot_FungiDemand);
	void PhosphorusUptakeResetForAllSoilLayers();

	function<double(double)> f_FirstOrder, f_pH_Response, f_Adsorption, f_PSurfaceRunoff_Conc;

	Structure *m_pModelStructure;
	Soil_Frost* pSoil;

}
;
