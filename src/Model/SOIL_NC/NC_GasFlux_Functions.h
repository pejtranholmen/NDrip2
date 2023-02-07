#pragma once
#include "./NC_Organic.h"

class NC_GasFlux_Functions : public NC_Organic
{
	public:
		NC_GasFlux_Functions(void);
		~NC_GasFlux_Functions(void);
		bool Def();
		bool FunctionDef();


size_t  Trace_Gas_Emissions, Oxygen_Model, Methane_Model,Methane_Plant_Oxidation, Methane_Soil_Oxidation,
Methane_Plant_Emission, Methane_SoilBoundary_Flux;
size_t COS_Model, COS_PlantModel, COS_SoilModel;
size_t  N2O_YearlyFluxes/*, CO2_SoilModel Moved to AbioticResponses_Functions*/;  
vector<double> Oxygen, Oxygen_Flux, Oxygen_Consumption, CO2Soil, CO2_FluxSoil,CO2_Production;
vector<double> Init_Oxygen_Conc, Oxygen_Conc, Air_Porosity, Init_CO2_Conc/*, CO2_Conc*/;
double  DO2_Tortuosity , AnBal_ShapeCoef, IntDiffRedFrac, IntDiffRedFracBase, Oxygen_Flux_Infil;
double  DCO2_Tortuosity; 
double  DCO2_PowerCoefSoilDiff, DCO2_AirThreshold, DCO2Min_Soil;
double  NO_Rate, N2O_Rate, N2_Rate;
double  MaxFracN2O, MaxFracNO;
double  N2O_TempMax, N2O_TempOpt, N2O_TempShape, N2O_RelSatCrit, N2O_RelSatForm;
double  NO_RelSatCrit, NO_RelSatForm;
double  NO_pH_Coef;
double  Denit_Act_PowerCoef;
double  N2O_10Percentil, N2O_50Percentil, N2O_90Percentil, N2O_Ymean;
double  NO_10Percentil, NO_50Percentil, NO_90Percentil, NO_Ymean;
vector<double>  pH_Value;
vector<P*> mTab_Ox, mTab_Ph;

vector<double> NO, N2O, N2;
vector<double> NO_SoilFlux, N2O_SoilFlux, N2_SoilFlux, NO_NitriFlux, N2O_NitriFlux;
vector<double> NO_Exchange, N2O_Exchange, N2_Exchange;
double  NO_TotExchange, N2O_TotExchange, N2_TotExchange;

// New Methane Model
vector<double> CH4_Prod, CH4_Oxidation, CH4_SoilFlux, CH4;
double  CH4_RateCoefRatio;

vector<double> CH4_Ebullition,CH4_PlantDiffusion,CH4_SoilEmission, CH4_Conc_Water;

////Added by Marco
vector<double> CH4_OxidationSoilBacteria,  CH4_OxidationPlantRoot, MethaneBubbleFunc;
double  CH4_OxidationReactionRate, CH4_HalfRateOxConstant, CH4_PlantOxidationRate, CH4_Water_Diff_Coef;
double  CH4_EbullMax, CH4_BubbleRateConstant;
double  CH4_PlantOxidationEff, CH4_WaterPlantCoef, CH4_ResistanceAir;
double CH4_AccBubbles, CH4_AccPlant, CH4_AccEmission, CH4_DiffTotStorage, CH4_MassBalanceCheck;
double CH4_AccProduction, CH4_AccOxidation;
double CH4_ResistanceAirLogScale;

double CH4_TotPlantUptake, CH4_TotPlantEmission, CH4_TotPlantOxidation, CH4_TotSoilOxidation;
////Added by Marco III;


double  CH4_PlantOxShape, CH4_LimitWaterUptake, CH4_Air_Diff_Coef;
vector<double>  CH4_AirSoilFlux,  CH4_CONC_AIR;
double   CH4_HStandard, CH4_HTempSens, CH4_H_TRef, CH4_WaterPowerCoef, CH4_InitConc, CH4_SurfaceEmission;
;
double    CH4_TotSoilFlux, CH4_TotSysFlux, CH4_CONC_Atm, CO2_CONC_Atm;
double CO2_Flux_Out;


double COS_TranspirationRatio, COS_PhotoSynthesisRatio;	//Parameters
double COS_TranspirationPotMaxRatio, COS_TranspirationPotRatio;
double COS_AirResistCoef, COS_CanopyResistCoef, COS_RadHalfCoef, COS_RadScaleCoef;
double COS_HalfCondCanopy, COS_HalfCondAir;
double COS_SoilRespiratonRatio;							//Parameters
double COS_TotPlantUptake, COS_TotSoilUptake, COS_EcoSystemFlux;
double COS_Option1, COS_Option2, COS_Option3, COS_Option4, COS_Option5, COS_Option6;
;
;
// New CH4 model develop 2007;
//Other Existing variables to use: RootDensity;;
//			         GSI Common Response Function (GSIFunction) (do we have already one?);
//				 WaterFlow[i] ////Water vertical flow in soi;
;
;
;
;
//	LocalVariables;
//Real, Parameter	:: OxygenConcAtm = 0.2095	// Volumetric concentration of oxygen in atmosphere;
double OxygenMassConcAtm, MethaneMassConcAtm, CO2MassConcAtm;
vector<double> OxygenGradients, DO2_SoilCoef, DO2_ExCoefSoil, DCO2_SoilCoef;
vector<double> DenitGrowth_NO3, DenitGrowth_NO2, DenitGrowth_NO, DenitGrowth_N2O, DenitUptakeFactor;
vector<double> DailyValuesNO, DailyValuesN2O;
;
vector<double> DenitUptakeMin, DenitUptakeOrg;

function<double(double)> f_COS_Plant,f_COS_PlantTransp, f_COS_PlantTranspPotMax, f_COS_PlantTranspPot, f_COS_Soil;

function<double(double)> f_MethaneOxidation, f_MethaneOxidationRegulation, f_PlantRootRespRegulation;
function<double(double, double)> f_MethaneVolatility;
function<double(double, double)> f_Ebullition;
function<double(double, double)> f_MethaneProduction_OneLitter;
function<double(double, double)> f_PlantEmissionbyWaterUptake;
function<double(double, double)> f_MethaneBoundaryLayerFlux;
function<double(double, double, double)> f_MethaneDynResBoundaryLayerFlux;
function<double(double)> f_MethaneOxidationInPlant;
function<double(double, double, double)> f_MethaneProduction_TwoLitter;

function<double(double)> f_AnaerobicFraction;

function<double(double, double)> f_OxygenDiffuctionCoef;
function<double(double, double)> f_CarbonDiOxideDiffuctionCoef;
function<double(double, double)> f_OxygenDensityFrom_Volume_Temp;
function<double(double)> f_OxygenDensityFrom_Temp;
function<double(double, double)> f_CarbonDioOxideDensityFrom_Volume_Temp;
function<double(double)> f_CarbonDiOxideDensityFrom_Temp;

function<double(double)> f_NitrogenOxideProduction_Temp;
function<double(double)> f_NO_Production_Moist;
function<double(double)> f_N2O_Production_Moist;
function<double(double)> f_NO_Production_PH;
function<double(double)> f_NO_Production;
function<double(double)> f_N2O_Production;
function<double(double)> f_AnaerobicExchangeEfficiency;
function<double(double)> f_DenitrifierAnaerobicFracResponse;



double  CH4_InitStorage;
;
bool YearShift, FullYear;

/*
double N2O_TempFunctionL(size_t Layer);
double N2O_TempFunction(double Value);
double N2O_MoistureFunctionL(size_t Layer);
double N2O_MoistureFunction(double Value);
double NO_MoistureFunctionL(size_t Layer);
double NO_MoistureFunction(double Value);

double NO_pHFunctionL(size_t Layer);
double NO_pHFunction(double Value);
double ExCoefFunctionL(size_t Layer);
double ExCoefFunction(double AnVolFraction, double AirPorosity);*/
bool DenitrifierUpdate();


};

