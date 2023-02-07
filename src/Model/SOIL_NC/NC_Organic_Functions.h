#pragma once
#include "./NC_Mineral.h"
#include "./NC_Mineral_Functions.h"



class Soil_Frost;
class NC_Organic_Functions : public NC_Mineral
{
	public:
		NC_Organic_Functions(void);
		~NC_Organic_Functions(void);
		bool Def();
		bool FunctionDef();


		bool ReCalcOrgProfile();	
SimModule *pPlant, *pSoil;
Soil_Frost *m_pSoil;
size_t  Microbes, LitterPools,  Initial_Soil_Organic, CN_Ratio_Influence;
size_t  Microbial_Mortality, Microbial_Consumption,  Q_Model, InitialCarbonConditions, InitialPhosphorusConditions;
size_t  OrganicUptake;
size_t Measured_Organic_Table; // New switch to have layers corresponding to other measured horizons June 2014
size_t DisplacementOrganicLayers; // New switch to move organics between layers corresponding to annual change of storage


vector<double>  Prev_O_State[NUM_ORG_FORMS][NUM_BIOELEM];

vector<double>  CTOTQ;

// Input to Soil pools
//double  CSurfaceLitter_L1, CSurfaceLitter_L2, CSurfaceLitter_Hum;
//double  NSurfaceLitter_L1, NSurfaceLitter_L2, NSurfaceLitter_Hum;
//
//vector<double> NRootsLitter1,NRootsLitter2,CRootsLitter1, CRootsLitter2;
//vector<double> NFungiLitter1,NFungiLitter2,CFungiLitter1, CFungiLitter2;



double SurfaceLitter[2][NUM_BIOELEM];

double SurfaceLitterMixingIntoUppermostlayer[4][NUM_BIOELEM];




// MF
//vector<double>  CLitter1Dissolved, CLitter2Dissolved, CFaecesDissolved, CHumusDissolved, CMicrobesDissolved;
//vector<double>  NLitter1Dissolved, NLitter2Dissolved, NFaecesDissolved, NHumusDissolved, NMicrobesDissolved;

vector<double>  DissolvedFlow[NUM_BIOELEM-1], DissolvedDrainage[NUM_BIOELEM - 1], NDissolvedDecomp;

double DissolvedCDrainageConc, DissolvedNDrainageConc;

// N Organic Uptake
double		NTotOrg_PlantUptake, CN_Threshold_Org, PTotOrg_PlantUptake;


// Respiration rates

vector<double> O_RespRate[NUM_ORG_FORMS];
double O_RespRateProfile[NUM_ORG_FORMS];

// Transformation rates

vector<double> O_Transform[NUM_ORG_TRANSFORMATION][NUM_BIOELEM];
vector<double> O_Mineralisation[NUM_ORG_FORMS][NUM_BIOELEM-1];
double O_MineralisationProfile[NUM_ORG_FORMS][NUM_BIOELEM - 1];
vector<double> LitterInputToSoilLayers[2][NUM_BIOELEM];
double LitterInputProfile[NUM_BIOELEM];

pair<size_t, size_t> Org_Link[NUM_ORG_TRANSFORMATION];

double  CTotSoilRespRate;
double  Humusformation[NUM_BIOELEM - 1];
double  NTotLitter1NH4Rate,NTotLitter2NH4Rate,NTotHumusNH4Rate, NTotMicrobeNH4Rate ;
double  NTotFaecesNH4Rate, NTotOrgNH4Rate, PTotOrgPO4Rate;
;

double  NTotSoilMicrobes, CTotSoilMicrobes, NTotSoilLitter1, CTotSoilLitter1, NTotSoilLitter2, CTotSoilLitter2 ;
double  NTotSoilFaeces, CTotSoilFaeces;



// Soil Organics;
double  CTotOrg_DisOrg, NTotOrg_DisOrg;
double  CTotDisOrgDecomp;
;
// Auxiliaries;
//vector<double>   CLitter1Decomp, CFaecesDecomp, CLitter2Decomp;
struct C_Content {
	vector<double> Litter1;
	vector<double> Litter2;
	vector<double> Faeces;


};
C_Content Conc;
//vector<double> C_Conc_Litter1, C_Conc_Litter2, C_Conc_Faeces;
;
double    CN_Ratio_TotSoil, CN_Ratio_TotHumus, CP_Ratio_TotSoil, CP_Ratio_TotHumus;
//vector<double>   ResponseTemp, ResponseTheta, ResponseTemp_Theta;
;
// Initial Conditions;

Soil_Org_Ini Org_State_IniValue[NUM_ORG_FORMS][NUM_BIOELEM - 1];  //Parameters

Soil_Org_Ini Org_State_IniCarbonRatio[NUM_ORG_FORMS][NUM_BIOELEM - 1]; //Parameters
vector<double> Org_CarbonRatio[NUM_ORG_FORMS][NUM_BIOELEM - 1];// Auxiliary
//Soil_Org Org_Tot[NUM_BIOELEM - 1];// Auxiliary
//Soil_Org Org_CarbonRatioTot[NUM_BIOELEM - 1];// Auxiliary

double    Init_L1_C_Tot,	  	Init_L2_C_Tot,		  	Init_F_C_Tot			;
vector<double> Init_Measured_C_Cont,Init_Measured_N_Cont, Init_Measured_BD;
vector<double> Init_Assumed_CN_Humus, Init_Assumed_Frac_Litter2, Init_Assumed_CN_Litter;


//	Parameters;
double    RateCoefLitter1, RateCoefFaeces, RateCoefHumus, RateCoefLitter2;
double    HumFracLitter1, HumFracLitter2, HumFracFaeces;
double    Eff_Litter1, Eff_Litter2, Eff_Faeces, Eff_Humus, CN_Ratio_Microbe, CP_Ratio_Microbe;
;
double    RateCoefSurf_L1, RateCoefSurf_L2, RateCoefSurf_Hum;
;
double     RateCoefMic_Cons, RateCoefMic_Mort, RateCoefMic_Resp;
double    Mic_ConsFrac_L1, Mic_ConsFrac_L2, Mic_ConsFrac_Fec;
double    Mic_HumFrac_L1, Mic_HumFrac_L2, Mic_HumFrac_Fec;
double    Mic_MortFrac_L1, Mic_MortFrac_L2, Mic_MortFrac_Fec;
double    RateResponseCN_Max, RateResponseCN_Min, PO4ConcentrationResponsePO4_Max, PO4ConcentrationResponsePO4_Min;
double    SubStrate_HalfRateConc, N_ImmobMaxAvailFrac, P_ImmobMaxAvailFrac;
//MF;

double    RateCoefLitter1Dis, RateCoefLitter2Dis, RateCoefHumusDis;
double	  DocSolFraction; // Added 6 june 2015
double    RateCoefFaecesDis, RateCoefMicrobeDis, Init_CDissConc;
vector<double>   DissolvedConc[NUM_BIOELEM - 1], RateCoefDissolved;
FLUX Dissolved[NUM_BIOELEM - 1];
;
//org Uptake 8/10-2000 PEJ

double    UptOrgRateCoef_L, UptOrgRateCoef_H, NUptRootCoefOrg;

double    P_UptOrgRateCoef_L, P_UptOrgRateCoef_H;

double FungiOrgEnhanceCoef, FungiOrgPEnhanceCoef;
;
vector<double>   ResponseTempD, ResponseThetaD, ResponseTemp_ThetaD;

//Tables
vector<P*> mTab_OrgN, mTab_OrgCN, mTab_OrgC, mTab_Diss, mTab_MeasOrg, mTab_OrgP, mTab_OrgCP;
;
;
;
// Local Variables;
;
vector<double>  CRootsSoilLitter,NRootsSoilLitter, PRootsSoilLitter, MicrobePotConsumptionRate;
;
vector<double>  CFungi_DepthLitter,NFungi_DepthLitter, PFungi_DepthLitter;
;
vector<double>  CCoarseRootsSoilLitter,NCoarseRootsSoilLitter, PCoarseRootsSoilLitter;
vector<double>  TE_RootsSoilLitter,TE_RootsSoilOldLitter;
vector<double>  CRootsSoilOldLitter,NRootsSoilOldLitter,PRootsSoilOldLitter;
vector<double>  COldCoarseRootsSoilLitter,NOldCoarseRootsSoilLitter, POldCoarseRootsSoilLitter;

vector<double> OrgTotLayer;
;

;
bool MakeDisplacement();
bool MakeDisplacement2();


function<OrgTurnOver(double, double, double)> f_TurnOver_CN[4];
function<OrgTurnOver(double, double, double, double, double)> f_TurnOver_CNP[4];

function<double(double)> f_Decomp_CN_Response, f_Decomp_Conc_Response;
function<double(double)> f_PotentialGrowthMicrobe, f_MicrobeRespiration;
function<double(size_t, double)> f_MicrobeGrowthRespiration, f_GrowthConsumption, f_Humification, f_Mortality;
function<double(double)> f_LitterTransSurf_L1, f_LitterTransSurf_L2, f_LitterTransSurf_Hum;


// Functors as Templates for fast processing
function <vector<double>(vector<double>&, vector<double>&, vector<double>)> f_ConcFromAmountandWater;
function <vector<double>(vector<double>&, vector<double>&, double, vector<double>&)> f_VerticalSoluteFlowFromWaterFlowConcDeepPerc;
function <pair<double,vector<double>>(vector<double>, vector<double>&, vector<double>&)> f_DrainageSoluteFlowWithMeanConcFromWaterFlowConc;
function <vector<double>(size_t, vector<double>&, vector<double>&, vector<double>&, vector<double>&, vector<double> &)> f_DissolvedHumusTransformbyTempThetaState;
function <vector<double>(size_t, vector<double>&, vector<double>&, vector<double>&, vector<double> &)> f_DissolvedTransformbyTempThetaState;
	

bool Do_Cultivating;

};

