#pragma once

#include "./NC_Drivings.h"
#include "../SOIL/Soil_Frost.h"
#include "../PLANT/Plant.h"
#include "./Functors/Respiration/RespirationTemplate.h"
#include "../../PG/PGUtil.h"
#include "./Functors/NC_Plant_Functors.h"


class ZadokOutCreateFile;
struct GRespons{
	double Temp;
	double N;
	double W;
};
struct Photo{
	double Gross;
	double Net;
	double Rubisco;
	double Light;
	double Sink;
};


class NC_Plant_Functions : public NC_Drivings
{
	public:
		NC_Plant_Functions(void);
		~NC_Plant_Functions(void);
		bool Def();
		bool Def_GrowthStages();
		bool Def_PhotoSynthesis();
		bool Def_Allocation();
		bool Def_LitterFall();
		bool Def_Respiration();
		bool Def_Fungi();

		void Def_Grazing();
		void Def_WoodEating();
		void Def_NFixation();
		bool FunctionDef();






	
 Soil_Frost *p_Soil_WaterF; // Soil Water Flow module
 SimModule *mp_Plant;




 /*Use Plant, only		: LeafAreaFunctionL, RootDepthFunctionL, PlantAlbedoFunctionL, PlantHeightFunctionL, LAI, PlantHeight, RootF
use Plant, only		: SimPlantHeight, SimLeafAreaIndex, SimRootDepth, SimPlantAlbedo, SimRootLength, RootLengthFunctionL
Use PotTranspiration, only	: MC_ResSurfVar, MC_DisplVar, MC_ResAir, Lohammar_L
Use Radiation, only	: DistributeRad, RntLAIv
Use NC_Drivings, only	: NDrivWaterStress, NDrivTranspiration, NDrivRadInShort, NDrivTAir, NDrivSaltStress
Use NC_Organic, only	: CTotSoilRespRate, LitterPools
Use Salt
//, Only		: TraceElementUptake, TE_ActiveUptake, TE_UptPassiveReset,TE_UptActiveReset, SaltPlantUptake, TE_UptReset
//Use Salt, Only		: TE_PlantLeafUptake, TE_PlantRootUptake, TE_PlantStemUptake, ActiveUptake*/

size_t Root_Allocation_Combination, Growth, Resp_Temp_Response, FungalGrowth, Root_Density_Influence;      
; //, Grain_Development;
size_t Root_Alloc_TotMass, Root_Alloc_N_Leaf, Root_Alloc_WaterStress, Leaf_Alloc_TotMass, Root_Alloc_P_Leaf;
size_t GrainFillAllocation;
size_t N_Fixation, Harvest_Day, LitterFall_Dynamics;		// LK removed Growth_Start 020828
size_t Winter_Regulation, Salinity_Stress, PlantStress, PhoSaturation;
size_t PlantRespiration, N_Fixed_Supply, GrowthResponseVar, RadInterception, PhotoInput, N_ReAllocation, Phos_ReAllocation; //// new ReAllocation 30/11 05
size_t InitialPlantConditions, Grazing, GrazingClosure, InitialPlantPConditions;
size_t TerminatingHarvest, ResetGSI_AtHarvest;
size_t Sowing_Switch,Emergence_Switch, StartSeasonTableExist;
size_t Seedling_Management;

size_t ZadokScale, ZadokForcingFromFileInput;
size_t CanopyCO2_Storage;
CPG m_PG_Zadok;

// State variables;


vector<double> GrowthStageIndex, SimPlantAge;


vector<double> P_State[NUM_PLANT_FUNGI_COMP][NUM_BIOELEM];
vector<double> P_Respiration[NUM_PLANT_FUNGI_COMP];
vector<double> P_LitterFall[NUM_PLANT_FUNGI_COMP][NUM_BIOELEM];
vector<double> P_Harvest[NUM_PLANT_COMP][NUM_BIOELEM];
vector<double> P_ReAllocation[NUM_REALLOCATION][NUM_BIOELEM];
AlloNewMobile P_CarbonAlloNewMobile;
AlloNewMobile P_UptakeNewMobile[NUM_BIOELEM - 1];
vector<double> P_Uptake[NUM_UPTAKE][NUM_BIOELEM-1];
double AccHarvest[NUM_BIOELEM];

vector<double> P_State_IniValue[NUM_PLANT_FUNGI_COMP][NUM_BIOELEM];  //Parameters
vector<double> P_State_IniCarbonRatio[NUM_PLANT_FUNGI_COMP][NUM_BIOELEM - 1]; //Parameters
vector<double> P_State_CarbonRatio[NUM_PLANT_FUNGI_COMP][NUM_BIOELEM - 1];// Auxiliary


vector<double> P_SumPlant[NUM_SUM_STATES][NUM_BIOELEM], C_RatioPlant[NUM_BIOELEM - 1];//HH

double P_AllPlants[NUM_SUM_STATES][NUM_BIOELEM];
double AllFungi[NUM_BIOELEM];


pair<size_t, size_t> P_ReLink[NUM_REALLOCATION];
string P_StateCompName[NUM_PLANT_FUNGI_COMP];
string P_BioElementName[NUM_BIOELEM];

// Flow variables;
vector<double> NPlantAddSupply;
vector<double> AtmNewMobile;//, CNewMobileLeaf,CNewMobileStem, CNewMobileRoots, CNewMobileCoarseRoots;
//vector<double> CStemGrain, CLeafGrain, CRootsGrain;
//vector<double> CStemHarvest, CLeafHarvest, CGrainHarvest, CRootsHarvest;
//vector<double> COldStemHarvest, COldLeafHarvest, COldRootsHarvest;
vector<double>  CNewMobileFungi;
//vector<double> CStemAtm, CLeafAtm, CRootsAtm, CGrainAtm;
vector<double> CStemOldStem, CLeafOldLeaf;
//vector<double> COldStemAtm, COldLeafAtm, COldRootsAtm;
vector<double>   NFungiPlant, PFungiPlant;
vector<double> CRootsLeaf, NRootsLeaf, COldRootsLitter, NOldRootsLitter; // Reallocation During conditions of small leaf Christine;
;


vector<double> CLeafGrazing, COldLeafGrazing, CStemGrazing;
vector<double> NLeafGrazing, NOldLeafGrazing, NStemGrazing;
vector<double> PLeafGrazing, POldLeafGrazing, PStemGrazing;




vector<double> CCoarseRootsAtm, COldCoarseRootsAtm;
vector<double> CCoarseRootsHarvest, COldCoarseRootsHarvest;
vector<double> NCoarseRootsHarvest, NOldCoarseRootsHarvest;
vector<double> SimPlantYearAge;
;
vector<double> CCoarseRootsGrain, COldCoarseRootsGrain,CCoarseRootsOldRoots;
;
;
vector<double> NCoarseRootsGrain, NOldCoarseRootsGrain, CRootsLitter;
vector<double> NRootsLitter, NCoarseRootsLitter, CCoarseRootsLitter, NOldCoarseRootsLitter, COldCoarseRootsLitter;
;
vector<double> Cflux;
double GrazingRespiration, C_GrazingRate, N_GrazingRate, P_GrazingRate;
// New Flows 8 nov 00;
vector<double> NMobileLeaf, CMobileLeaf, NFixationPlant;
vector<double> NLeafReturnMobile, CLeafReturnMobile, NOldLeafReturnMobile,COldLeafReturnMobile;
;
// New Flow 30 nov 05;
vector<double> NLeafMobile;
vector<double> NStemMobile;
vector<double> NRootsMobile;
vector<double> NCoarseRootsMobile;
vector<double> NOldLeafMobile;
vector<double> NOldStemMobile;
vector<double> NOldRootsMobile;
vector<double> NOldCoarseRootsMobile;

;
// New flow June 17 2007;
vector<double> CLeafStem, NLeafStem;
;
vector<double> Mobile_Plant_Flux[NUM_BIOELEM];
double Mobile_TotPlant_Flux[NUM_BIOELEM];

vector<double>FungiRootCover, factNSupplyFungi, factPSupplyFungi ;//HH


/*
double N_Mobile_TotPlant_Flux;

;
;

;
;
vector<double> P_ReAllocation[NEWMOBILE_LEAF][_N_], P_ReAllocation[NEWMOBILE_STEM][_N_], N_NewMobileRoots,P_ReAllocation[NEWMOBILE_COARSEROOT][_N_];
;
vector<double> NStemGrain, NLeafGrain, NRootsGrain;
vector<double> NStemHarvest, NLeafHarvest, NGrainHarvest, NRootsHarvest;
vector<double> NOldStemHarvest, NOldLeafHarvest, NOldRootsHarvest;
vector<double> P_LitterFall[_LEAF][_N_],NStemSurfaceLitter, NGrainSurfaceLitter, N_NewMobileSurfaceLitter,NMobileSurfaceLitter,
vector<double> P_ReAllocation[_OLD_LEAF][_N_], COldLeafSurfaceLitter;
vector<double> NOldStemSurfaceLitter, P_LitterFall[_OLD_STEM][_C_];
vector<double> P_ReAllocation[STEM_OLD_STEM][_N_], P_ReAllocation[LEAF_OLD_LEAF][_N_], P_ReAllocation[ROOT_OLD_ROOT][_N_];
v*/
vector<double>  NPlantUptake, NRootUptake, PPlantUptake, PRootUptake;
;
// Auxiliary variable;
;
DemandUptakeSoil DemandUptake[2], DemandFungiUptake[2];
double C_RatioTotPlant[NUM_BIOELEM-1], C_RatioTotRoots[NUM_BIOELEM - 1];
vector<double>  RadPlantAdsorbed,  CP_RatioLeaf;
vector<double> GrowthTempSum, DormingTempSum,  CP_RatioGrain, CP_RatioStem, CP_RatioRoots, CP_RatioCoarseRoots;
vector<double>  ZadokIndex, CP_RatioFungi;
vector<double> SpringDevDayNum, DormingStartDayNum, GrowingStage;

;
vector<double> UptDeficit_1[2], UptDeficit_2[2], UptFungiDeficit_1[2], UptFungiDeficit_2[2],factCNFungi,  factNdemandFungi, factNavailFungi, factCPFungi, factPdemandFungi, factPavailFungi;//HH
vector<double> UptakeFracCoefDef, Root_NoduleRatio, StemLeaf_NoduleRatio;
vector<double> SpecificMaxNFixation,SpecificMinNFixation, NFixationSensitivity;
;
double TotHarvest[NUM_BIOELEM], TotPlantLitter[NUM_BIOELEM], TotFungiLitter[NUM_BIOELEM];
;
vector<double> C_Roots, N_Roots, C_Plant, N_Plant, C_AllLeaf, N_AllLeaf, C_Leaf, N_Leaf;
double Tot_PlantUptake[NUM_BIOELEM-1], Tot_FungiUptake[NUM_BIOELEM - 1];
vector<double> CPlantAtm,  CPlantAtm_AboveG;

vector<double> TotalPlantLitterFall[NUM_BIOELEM];

double TotAbovePlantLitter[NUM_BIOELEM];
;
// Parameters;
vector<double> UptakeStart, UptakeEnd, UptakeACoef, UptakeBCoef, UptakeCCoef, UptakeCNRatio;
vector<double> SowingDayNo, EmergenceDayNo, C_Seed;


// Seedling Management Parameters
vector<double> Seedling_Year, Seedling_YearAfterFinalHarvest, Seedling_DayNo, Seedling_CRoot, Seedling_CStem, Seedling_CLeaf, Seedling_Age;
vector<int> Estimated_Seedling_Year;
;
vector<double> HarvestDayNo, HarvFracGrain, LitterFracGrain, HarvFracLeaf, LitterFracLeaf;
vector<double> HarvFracStem,LitterFracStem, HarvFracRoots, LitterFracRoots;
vector<double> HarvFracCoarseRoots, LitterFracCoarseRoots;
;
vector<double> P_HarvFracGrain, P_LitterFracGrain, P_HarvFracLeaf, P_LitterFracLeaf;
vector<double> P_HarvFracStem,P_LitterFracStem, P_HarvFracRoots, P_LitterFracRoots;
vector<double> P_HarvFracCoarseRoots, P_LitterFracCoarseRoots;
;
vector<double> StubbleLeaf, StubbleStem, Dorm_FinalDayNo ;
vector <double> MinFracGreenResponse, OptGreenIndex;
double GrazingOfftake, GrazingRespFraction;
double C_GrazingExportFraction, C_GrazingManureFraction, C_GrazingGrowthFraction, C_GrazingCH4Fraction;
double N_GrazingExportFraction, CN_GrazingOrganic, N_GrazingGrowthFraction, N_GrazingManureAmmoniaFraction, N_GrazingManureFraction;
double P_GrazingExportFraction, CP_GrazingOrganic, P_GrazingGrowthFraction, P_GrazingManureFraction;
double C_GrazingExport, C_GrazingManure, C_GrazingGrowth, C_GrazingMethaneEmission;
double N_GrazingExport, N_GrazingManure, N_GrazingGrowth, N_GrazingFaeces, N_GrazingAmmonia, N_GrazingAmmonium;
double P_GrazingExport, P_GrazingManure, P_GrazingGrowth, P_GrazingFaeces;

double Pot_C_GrazingRate, Max_AvailGrazingFraction, GrazingScalingCoef;

double C_GrazingOrganism, N_GrazingOrganism, P_GrazingOrganism;

;
vector<double> InitPlantAge//, InitNGrain, InitNLeaf,InitNStem, InitNRoots,InitNOldLeaf,InitNOldStem, InitNOldRoots;
//vector<double> InitCNGrain, InitCNLeaf,InitCNStem, P_State_IniCarbonRatio[_ROOT][_N_],InitCNOldLeaf,InitCNOldStem, InitCNOldRoots;
;
//vector<double> InitCGrain, InitCLeaf,InitCStem, InitCRoots,InitCOldLeaf,InitCOldStem, InitCOldRoots;
//vector<double> InitCCoarseRoots,InitCOldCoarseRoots;
;
vector<double> InitNCoarseRoots,InitNOldCoarseRoots;
vector<double> InitCNCoarseRoots,InitCNOldCoarseRoots;
;
;
vector<double> CN_RatioMinLeaf, CN_RatioMinStem, CN_RatioMinRoots, CN_RatioMinCoarseRoots;
vector<double> CP_RatioMinLeaf, CP_RatioMinStem, CP_RatioMinRoots, CP_RatioMinCoarseRoots;
vector<double> CN_RatioMaxLeaf, CP_RatioMaxLeaf;
;
vector<double> AlloLeaf1, AlloLeaf2, AlloLeaf3, AlloRootWater1, AlloRootWater2, AlloRootWater3;
vector<double> AlloRoot_CN1, AlloRoot_CN2, AlloRoot_CN3, AlloRootTotMass1, AlloRootTotMass2, AlloRootTotMass3;
vector<double> AlloRoot_CP1, AlloRoot_CP2, AlloRoot_CP3;
vector<double> AlloRateCoef, Allo_C_LeafStem;

double AlloFracMinimumRoots;
;
// Parameter to Fungi Models            */;
;
// Parameter to Fungi Models            */;
;
vector<double>  AlloFungiC1,AlloMaxC1, RespMCoefFungi, Fungi_LitterRateC1, Fungi_LitterRateC2, FungiCarbonDemandCoef, FungiMinNSupplyCoef, FungiMinPSupplyCoef;
vector<double>  Fungi_MycFrac, Fungi_MycRate, Fungi_MantleRate, CN_RatioMinFungi, AreaCoverFungiCoef, CN_RatioMaxFungi;
vector<double>  Fungi_NO3rate,Fungi_NH4rate,Fungi_Norgrate,NavailMaxCoef,NavailCoef,NretFungi    ;
vector<double>  Fungi_PO4rate, Fungi_Porgrate, PavailMaxCoef, PavailCoef, PretFungi, CP_RatioMinFungi, CP_RatioMaxFungi;//HH
;
;
size_t ReAllocationToLeaf;  // Suggested by Christine;
vector<double>  AlloFromRootLeafCoef, AlloFromRootLeafRatio;  // New Christine reallocation;

double MoistureRangeEmergence, ZadokRangeGSI2, GSI_DeltaZadok30, GSI_DeltaZadok55;

// Parameter Tables
vector<P*> mTab_Nup,mTab_Start, mTab_GSI, mTab_Photo, mTab_PhotoW, mTab_PhotoRad, mTab_GSI2 ;
vector<P*> mTab_LitterA, mTab_LitterB, mTab_LitterBC, mTab_Harvest, mTab_HarvestPrior, mTab_Graz;
vector<P*> mTab_PlantInit[3], mTab_PlantInitRatio[2], mTab_CNLitter, mTab_PlantCP, mTab_PlantCN, mTab_CPLitter;
vector<P*> mTab_PResp, mTab_PRespN,mTab_Allo, mTab_PlantInitCP;
vector<P*> mTab_FungiGrowth, mTab_FungiTrans, mTab_FungiTransP, mTab_AlloStem, mTab_AlloLeaf, mTab_AlloCoarseRoots, mTab_AlloGrain;
vector<P*> mTab_LitterScale, mTab_NFix, mTab_Farq, mTab_PlantMan;
vector<P*> mTab_CleanD, mTab_CleanY, mTab_HarvD, mTab_HarvY;
vector<P*> mTab_Seedlings;

vector<P*> mTab_StartYear;
// Auxiliary to Fungi Model;
vector<double>  NFungiDemand,  NFungiAvail, FungiNorgUptpot,FungiNitrateUptpot,FungiAmmoniumUptpot;
vector<double>  PFungiDemand, PFungiAvail, FungiPorgUptpot, FungiPhosphateUptpot;//HH
double N_Tot_FungiDemand,N_Tot_FungiUptake;
double P_Tot_FungiDemand, P_Tot_FungiUptake;
;
;
vector<double> Allo_C_LeafGrain,Allo_C_StemGrain,Allo_C_RootsGrain;
vector<double> Allo_N_LeafGrain,Allo_N_StemGrain,Allo_N_RootsGrain;
vector<double> Allo_P_LeafGrain, Allo_P_StemGrain, Allo_P_RootsGrain;

vector<double> Allo_GrainReductionRange;
;
vector<double> ScaleOldLeaf, ScaleNewLeaf,ScaleOldRoots, ScaleNewRoots, ScaleOldStem, ScaleNewStem;
vector<double> ScaleNewCoarseRoots, ScaleOldCoarseRoots;
;
vector<double>  MinimumP_Response;
vector<double> Min_GSI, Max_GSI, Initial_GSI, Max_Leaf_Lifetime, Max_Plant_LifeTime, GSI_PostHarvest;
vector<double> MobileAlloCoef, /*MobileAlloCoef_P,*/ ShootCoef;
vector<double>  C_Plant_AboveG, N_Plant_AboveG;

DRIVERSOnGPP GPP_Driver, GPP_Current, GPP_Potential, GPP_Deficit;
;
vector<double> CNLeafOpt, CNLeafThres, CPLeafOpt, CPLeafThres;
vector<double> TempResMin, TempResOpt1, TempResOpt2, TempResMax;
vector<double>  RelWeightWater, TempResponseCurrent;
vector<double>  TempSumResponseOpt, TempSumResponseStart;
;
vector<double> PhoRadEfficiency, PhoRadEff_Reduc, PhoMax, N_MinDegreeSupply;
vector<double> PhoWaterEfficiency;
;
vector<double> RespMCoefLeaf, RespMCoefStem, RespMCoefGrain, RespMCoefRoot,RespMCoefCoarseRoot, RespGCoef, RespNO3Coef, RespNH4Coef, RespNOrgCoef;
;
vector<double> GrainSI_Step, GrainSI_StepDay, GrainSI_ThresDay, GrainSI_StepTemp, GrainSI_ThresTemp;
vector<double> T_Sum_Emerg, T_Thres_Emerg, T_Sum_Sowing, T_Thres_Sowing, T_Thres_Dormancy,T_Sum_Initial;
vector<double> T_Thres_GrainFill, T_Sum_GrainFill;
vector<double> T_Thres_Ripening, T_Sum_Ripening;
;
vector<double> Leaf_LitterRateC1, Leaf_LitterRateC2, Leaf_LitterTsum1, Leaf_LitterTsum2, Leaf_FracLitter2;
vector<double> Stem_LitterRateC1, Stem_LitterRateC2, Stem_LitterTsum1, Stem_LitterTsum2, Stem_FracLitter2;
vector<double> Grain_LitterRateC1, Grain_LitterRateC2, Grain_LitterTsum1, Grain_LitterTsum2, Grain_FracLitter2;
vector<double> Root_LitterRateC1, Root_LitterRateC2, Root_LitterTsum1, Root_LitterTsum2,Root_FracLitter2, CoarseRoot_FracLitter2;
vector<double> CoarseRoot_LitterRateC1, CoarseRoot_LitterRateC2, CoarseRoot_LitterTsum1, CoarseRoot_LitterTsum2;
vector<double> AboveGroundLitterFall_Position4Target;
;

;
vector<double> AlloCoarseRoots;
;
// Farquhar parameters;
vector<double> C3Type, Vcmax, QEff, RBoundary;
double P_ATheta, P_BTheta, P_Surface, CO2_A, PCO2_Boundary;
vector<double> PCO2_Internal, PCO2_Surface ;
vector<double> PRubOut, PLightOut, PSinkOut;
;
;
// NPP auxiliaries;
;
vector<double>  NPP_Leaf, NPP_Stem, NPP_CoarseRoots, NPP_Roots, NPP_Plant;
vector<double>  AccNPP_Leaf, AccNPP_Stem, AccNPP_CoarseRoots, AccNPP_Roots, AccNPP_Plant;
vector<double>  CRootsAtmLayer, CCoarseRootsAtmLayer, CFungiAtmLayer;
;
// Yied auxiliaries to keep latest Yield until next Yield appear. Use for for Validation if date of Harvest in unknown;
;
double  YearGrainHarvest[NUM_BIOELEM], YearTotalHarvest[NUM_BIOELEM];
// Common parameters for different plants;
double RespRateCoef;
double RespTemQ10, RespTemQ10Bas, RespTemQ10Threshold;
;
// Totals for output;
double Tot_Response_N, Tot_Response_Temp, Tot_Response_Water, C_TotPlant_AboveG, N_TotPlant_AboveG;
double Tot_Response_N_Water;
;
// Auxialiaries for Harvest Cycles;
;
double  C_TotHarvest_1, C_TotHarvest_2;
double  C_Thinning_1, C_Thinning_2;
double  N_TotHarvest_1, N_TotHarvest_2;
double  N_Thinning_1, N_Thinning_2;
;
//Auxiliaries for Zadok


vector<double> ZadokDayNumShift;
// Local variables;
//vector<double>  SowingSum, EmergenceSum, GrainStartSum, GrainFillSum, HarvestSum;
vector<double>  GrowingSum, DormingSum  , RemainOldLeaf[NUM_BIOELEM],  ResponseWeightCoef;
bool ZadokShift;
;
;
;
vector<double>  NUptakeDeficitTotal, PUptakeDeficitTotal;
vector<vector<double> > UptakeDistWithDepth;  //Relative distribution first index Plant, second index SoilLayer
vector<double> UptakeDistTotal[NUM_BIOELEM - 1], UptakeFungiDistTotal[NUM_BIOELEM-1];
;
;
size_t	Growing, SpringDev, ResetYearShiftDay;
double ContinStartSum, ContinStopSum;
;
bool DoHarvest, DoFinalHarvest;;
;
vector<bool> HarvestToBeDone, YearShiftToBeDone, HarvestDoneThisYear;
vector<size_t> ResetHarvestToBeDoneDay;
;
;
// Farquhar Variables;
vector<vector <double>>  EYY, PCo2y, ASSIMY, ASSIMNY;
vector<double>  PCo2s,  PCO2B, Vmax, FarqRange, Gammas, VmaxRubisco, Jmax;
vector<double>  PCo2I, GBoundary, GStomata, GAir, Qt, MMO2V;
;

vector<bool> Emergence_Done;
vector<size_t> ICConv, IGath ;
;
size_t m_ICountDay;
bool m_ResetMade;

;
//Logical Emergence_done;
;
//Real, Dimension, Allocatable :: VUTPG_PhotoFile	// Variables in PG-file PhotoInput;
;
;
// New Harvest variables 26 Dec 2005;
;
;
;
vector<double> Mulch, Drip;
size_t NUM_HarvestDays, NUM_HarvestYears;
size_t PlantsToHarvest;
vector<double>  HarvestYears,HarvestDays, HarvestYearPlantIndex, HarvestDayPlantIndex;
vector<double>  Harvest_Type_Years, Harvest_Type_Days;
vector<double>  HarvestLAIValue;
;
// New Harvest variables 21 november 2009;
size_t         NumValidHarvest, IHarvestInput,ICountHarvest;

;
;
// New option How to Harvest 28 March 2006;
vector<size_t> IHarvestDates ;// Variables in PG-file PhotoInput;
vector<SIMB> ChangeParAtHarvestDate;
vector< vector<double>> ChangeParAtHarvestValues;


// New for Sowing and Emergence from PG file reading
vector<size_t> ISowingDates, IEmergenceDates;
size_t ISowingCount, IEmergenceCount;
size_t ISowingIndex, IEmergenceIndex;


;
size_t HarvestStyle;
;
;
// New Cleaning variables;
size_t NUM_CleaningDays, NUM_CleaningYears;
size_t Cleaning;
vector<double>  CleaningYears,CleaningDays, CleaningPlantIndex, CleaningDegree;
size_t NUM_DynamicYearStart;
vector<double> m_StartYears, m_SowingDays, m_EmergenceDays, m_HarvestDays;
bool m_YearTest;
;
vector<double>  LAI_Enh_Coef, Annual_D_CPlant;
;
vector<double> PreviousCPlant;	// ;
;
bool DoCleaning, YearToClean, CleaningToBeDone;
size_t ResetCleaningDay, IndexClean;
// New Enhanced Litter Fall Because of LAI value;
;
//// Moved from Dyn below;
size_t Layer, OK, JJP;
double	C_Seed_Avail, NOrgUptake, C_Seed_Added, N_Seed_Added, P_Seed_Added;
;
double	AccResponseTemp, AccResponseWater, AccResponseWaterN, AccResponseN, Deficit, UptDeficitTotal[NUM_BIOELEM-1], UptFungiDeficitTotal[NUM_BIOELEM-1];
;
bool yearshift;
;
//// Moved from View below


	size_t Icase, II;
// Local control of Thinning support variable	
size_t	PreviousThinningYear, ICountHarvestCycle;
// Local control of YearStart support variables;
size_t  m_CountStartYear;

double m_LatestSowingDay;
bool m_NewDay;


CPG m_PG_HarvestDate, m_PG_PlantBiomass, m_PG_Grazing, m_PG_Sowing, m_PG_Emergence;

//   Positions in Additional Driving variable file for Nitrogen and Carbon, allocatable for NumPlants
vector<ININFO> CAtmNewMobile_Position, LAI_Position, RootDepth_Position;
size_t NumValidInputPhotoInput,NumVariablesPhotoInput;
PGPOS m_PGPos_PlantBiomass;

ININFO GrazingRate_Pos;
size_t NumValidInputGrazingInput, NumVariablesGrazingInput;
vector<size_t> GrazingDatesPGMin;
PGPOS m_PGPos_GrazingRate;

// Wood Eating Process
size_t WoodEating;
double C_WoodEater, N_WoodEater, P_WoodEater;
double WoodEaterRateCoef;
double WoodEatingStartAge, WoodEatingStartPeriod, WoodEatingOptPeriod, WoodEatingEndPeriod;
double WoodEaterEfficiency, WoodEaterCN_Ratio, WoodEaterCP_Ratio;

double C_WoodEatingRate, N_WoodEatingRate, P_WoodEatingRate, WoodEatingRespiration;
double WoodEaterFaecesRateCoef, WoodEaterAmmoniumRateCoef, WoodEaterRespRateCoef;;
double WoodEaterRespFlux, WoodEaterAmmoniumFlux;
double C_WoodEaterFaecesFlux, N_WoodEaterFaecesFlux, P_WoodEaterFaecesFlux;
double C_WoodEaterGrowthFlux;

vector<double> COldStemEating, NOldStemEating, POldStemEating;
vector<double> COldCoarseRootEating, NOldCoarseRootEating, POldCoarseRootEating;

//ININFO 

double WoodEatingRateCoefFunction(double time_sincestart);

GRespons P_Responses(size_t index);
Photo PhoSib(size_t index);
double P_PhoSiB(size_t index);
double PMinFunc(double P_Rub, double P_light,double  P_Sink, double Atheta, double Btheta);
double TP_Corr();
double MMO2(double Qtemp);
double P_RubiscolimitedC3(double Vmax, double Pco2, double Gamma, double MMO2);
double P_RubiscolimitedC4(double Vmax);
double P_LightLimitedC3(double Jm,double  Pco2, double Gamma);
double P_LightLimitedC4(double Jm);
double P_SinkLimitedC3(double Vmax);
double P_SinkLimitedC4_CO2(double PSink1, double Pco2);
double P_SinkLimitedC4(double Vmax);
double CasCo2cap(double displ,double  tpcorr,double TempAir);
double GAirFunc(double ResAir,double  TP, double Temp);
double LogisticGrowth(size_t index, double Day);
double LogisticGrowthRateFunc(size_t index, double JDay);
double CN_RatioLeafFixed(size_t index,double Response);
double PhoSoilWaterResponse(double EtaEtpRatio) ;
double GBoundaryFunc(double RBoundary, double TP, double Temp);
double GStomataFunc(double RStomata,double TP,double  Temp, double WStress);
void SortIn(vector<double> FarqRange, vector<double> Gammas, size_t i_step, size_t NumPlants, size_t index);

double CAlloFungi(size_t index);
double CAlloMaxFungi(size_t index);
double CAlloRootNitrogen(size_t index, double CN_RatioLeaf);
double CAlloRootWater(size_t index, double EtRatio);
double CAlloRootMass(size_t index,double Mass);
double AlloFromRootToLeaf(size_t index, double mass_root, double mass_leaf); 
double PhoRadEffResponseL(size_t index);
double PhoRadEffResponse(size_t index, double GrainFilling);
double PhoRadGrainFillResponse(size_t index, double GrainFilling);
double PhoSatResponseL(size_t index);
double PhoSatResponse(size_t index, double PhoLevel);


double CN_MinLeafFunc(size_t index);
double CN_MinStemFunc(size_t index);
double CN_MinRootsFunc(size_t index);
double CN_MinCoarseRootsFunc(size_t index);

double CP_MinLeafFunc(size_t index);
double CP_MinStemFunc(size_t index);
double CP_MinRootsFunc(size_t index);
double CP_MinCoarseRootsFunc(size_t index);

double LeafLitterFallCoefFuncL(size_t index);
double LeafLitterFallLAIFunc(double LAI, size_t index);
double LeafLitterFallCoefFunc(double Tsum, size_t index);
double StemLitterFallCoefFuncL(size_t index);
double StemLitterFallCoefFunc(double Tsum, size_t index);
double GrainLitterFallCoefFuncL(size_t index);
double GrainLitterFallCoefFunc(double Tsum, size_t index);
double RootLitterCoefFuncL(size_t index);
double FungiLitterCoefFuncL(size_t index);
double FungiLitterCoefFunc(double Tsum, size_t index);
double RootLitterCoefFunc(double Tsum,size_t index);
double CoarseRootLitterCoefFuncL(size_t index);
double CoarseRootLitterCoefFunc(double Tsum, size_t index);
bool KillAboveGroundPlant(size_t index);
bool KillBelowGroundPlant(size_t index);

double StomataCO2Func(double PCO2A, double PCO2Bp, double Assim, double RespSoil, double RespMPlant, double GB, double GS, double GA, double Vmax, double co2cap);
bool CheckFromPhotoFile();
bool CheckFromGrazingFile();


bool TerminatingHarvestFunction(size_t i);
double CoverFungiAreaFuncL(size_t i);
double CoverFungiFunc(double C_Fungi, double C_Root, double C_OldRoot, double AlloFungiC1, double Coef);

function < vector<double>(vector<double> &, vector<double>&, vector<double>&, vector<double>&, vector<double>&)> f_CarbonRatioWithOldBiomass, f_CarbonNitrogenRatioWithOldBiomass, f_CarbonPhosporousRatioWithOldBiomass;
function < vector<double>(vector<double> &, vector<double>&, vector<double>&)> f_CarbonRatio, f_CarbonNitrogenRatio, f_CarbonPhosporousRatio;

function<double(size_t, double)> f_PhotoTempDirect, f_PhotoTempSum, f_PhotoNLeaf, f_LogisticGrowth, f_PhotoPLeaf;
function<double(size_t, double)> f_MaxGPP, f_PhotoSaturation, f_PhotoGrainFill, f_PhotoGreenIndex;
function<double(size_t, double, double)> f_Photo_Combined_N_Water_Response, f_PhotoTemp;
function<DRIVERSOnGPP(DRIVERSOnGPP&, vector<double> &)> f_IntegratedAuxGPPResponse;

function<double(size_t, double)> f_CAlloRootWater, f_CAlloRootNitrogen, f_CAlloRootPhosphorus, f_CAlloRootMass;
function<double(size_t, double)> f_CAlloLeaf;
function < vector<double>(vector<double> &, vector<double>&, vector<double>&, vector<double>&, vector<double>&, vector<double>&)> f_AlloRoots;
function <double(size_t)> f_AlloCoefLeaf, f_AlloCoefRoot;

function < vector<double>(vector<double> &, vector<double>&, vector<double>&, vector<double>&, vector<double>&)> f_AlloLeaf;
function < vector<double>(vector<double> &, vector<double>&, vector<double>&, vector<double>&)> f_AlloCoarseRoot;
function < vector<double>(vector<double> &, vector<double>&, vector<double>&, vector<double>&)> f_AlloStem;

function <vector<double>(vector<double> &, vector<double>&, vector<double>&, vector<double>&, vector<double>&, vector<double>&,
	vector<double> &, vector<double>&, vector<double>&, vector<double>&, vector<double>&)> f_AlloFungi;

AllocationToFungiwithP f_AlloFungiwithP;//HH
    
//function <vector<double>(vector<double> &, vector<double>&, vector<double>&, vector<double>&, vector<double>&, vector<double>&,
//    vector<double> &, vector<double>&, vector<double>&, vector<double>&, vector<double> &, vector<double>&, vector<double>&, vector<double>&, vector<double>)> f_AlloFungiwithP;//HH
function < vector<double>(vector<bool>&, vector<double> &, vector<double>&, vector<double>&)> f_RootToLeaf;
function <vector<double>(size_t Comp, size_t Elem, vector<double> &GSI, vector<double> &mass, vector<double>&flux)> f_GrainSupply;
function < vector<double>(vector<double> &, vector<double>&, vector<double>&)> f_ReAlloCarbonToStemFromLeaf;
function < vector<double>(vector<double> &, vector<double>&, vector<double>&)> f_ReAlloNitrogenToStemFromLeaf;
function < vector<double>(vector<double> &, vector<double>&, vector<double>&)> f_ReAlloPhosphorusToStemFromLeaf;

function < AlloNewMobile(vector<double> &, vector<double>&, vector<double>&, vector<double>&, vector<double>&, vector<double>&, AlloNewMobile&)> f_AlloCarbonPlant;
function < AlloNewMobile(vector<double> &, vector<double>&, vector<double>&, vector<double>&, vector<double>&, AlloNewMobile)> f_AlloUptakePlant[2];

function<double(bool, size_t, double, double, double, double)> f_LeafLitterFall;
function<double(bool, size_t, double, double, double)> f_StemLitterFall, f_RootLitterFall, f_CoarseRootLitterFall;
function<double(size_t, double, double, double)> f_GrainLitterFall, f_FungiLitterFall;
function<double(size_t, double)> f_GrowthRespiration;
function<double(double)> f_TempResponseRespiration;
function<double(PlantComp, size_t, double)> f_MaintenanceRespiration;
function<vector<double>(PlantComp, vector<double> &Mass, double &Temp,vector<double> &Resp)> f_PlantRespiration;
function<vector<double>(PlantComp, vector<double> &Mass, vector<double> &Photo, double &Temp, vector<double> &Resp)> f_PlantPhotoRespiration;
function<PlantSoilLayers(PlantComp, vector<vector<double>> &DistPlantSoil, vector<double> &Mass, vector<double> &Temp)> f_PlantBelowGroundRespiration;
function<PlantSoilLayers(PlantComp, vector<vector<double>> &DistPlantSoil, vector<double> &Mass, vector<double> &Photo, vector<double> &Temp)> f_PlantPhotoBelowGroundRespiration;
function<double(PlantComp, size_t)> f_SaltImpactOnGrowthRespiration;
function<double(size_t, size_t)> f_NuptRespiration;
function<double(size_t, double, double)> f_Sowing, f_Emergence, f_GrainFill;
function<double(size_t, double, double, double)> f_EmergenceWithMoistControl;
function<double(size_t, double, double, double)> f_GrainStart;
function<double(size_t, double, double)> f_Harvest; 
function<double(size_t, double, double, double, double &DormSum, size_t &SpringDev)> f_DormingAndGrowthTempSum;
function<double(double)> f_Zadok;



function<bool(size_t year, size_t dayno)> f_sow, f_emergence, f_harvest;
vector<double> m_RootDensity;	// Local auxiliary variable

function <vector<double>(vector<double>&, vector<double>&, vector<double>&, vector<double>&, vector<double>&, vector<double>&, vector<double>&, vector<double>&, vector<double>&)> f_PlantDemand[2];
function < vector<double>(vector<double>&, vector<double>&, vector<double>&)> f_FungiDemand;
function < vector<double>(vector<double>&, vector<double>&, vector<double>&)> f_FungiPDemand;
function <vector<double>(size_t index, vector<double>&, vector<double>&)> f_FungiPotentialUptake;
function <vector<double>(size_t index, vector<double>&, vector<double>&)> f_FungiPotentialPUptake;
    
function <vector<double>(vector<double>&, vector<double>&, vector<double>&, vector<double>&)> f_FungiCover;
          
function <vector<double>(vector<double>&, vector<double>&, vector<double>&)> f_FungiNAvail;
    
function <vector<double>(vector<double>&, vector<double>&, vector<double>&)> f_FungiPAvail;
    
function <vector<double>(vector<double>&, double&, double&, vector<double>&)> f_RootDensity;
    
function<LayerOutput(double, double)> f_Avail;
function<LayerOutput(double, double)> f_AvailP;
function<double(double)> f_Density; //f_FungiCoverImpact; HH
function<double(double)> f_FungiImpactOnPlant;
function<double(bool)> f_ImpactOnFungiMinUptake;
function<double(double)> f_ImpactOnFungiPhosphateUptake;
function<UptakeResults(vector<double> &DistWithDepth, size_t PlantIndex, double N_DemandPlant, double N_Tot_Demand,
	vector<double> Dist_Tot, vector<double> &SoilNitrate, vector<double> &SoilAmmonium, UptakeResults &out)> f_MineralUptake;
function<PUptakeResults(vector<double> &DistWithDepth,  size_t PlantIndex, double P_DemandPlant, double P_Tot_Demand,
	vector<double>& Dist_Tot, vector<double> &Soilphosphate, PUptakeResults &out)> f_PMineralUptake;
        
function<UptakeResults(vector<double> &DistWithDepth, size_t PlantIndex, double N_DemandPlant, double N_Tot_Demand,
	vector<double> Dist_Tot, vector<double> &SoilNitrate, vector<double> &SoilAmmonium, vector<double> &RootDensity, UptakeResults &out)> f_MineralUptakeWithDensity;
    
function<PUptakeResults(vector<double> &DistWithDepth,  size_t PlantIndex, double N_DemandPlant, double N_Tot_Demand,
	vector<double> Dist_Tot, vector<double> &SoilPhosphate, vector<double>& RootDensity, PUptakeResults &out)> f_PMineralUptakeWithDensity;
    
    
function<UptakeResults(vector<double> &DistWithDepth, size_t PlantIndex, double N_DemandPlant, double N_Tot_Demand,
	vector<double> Dist_Tot, vector<double> &SoilNitrate, vector<double> &SoilAmmonium, vector<double> &RootDensity, vector<double> &FungiCover,bool, UptakeResults &out)> f_MineralUptakeWithDensityImpactWithFungi;
function<PUptakeResults(vector<double> &DistWithDepth, size_t PlantIndex, double N_DemandPlant, double N_Tot_Demand,
	vector<double> Dist_Tot, vector<double> &SoilPhosphate,  vector<double> &RootDensity, vector<double> &FungiCover, bool, PUptakeResults &out)> f_PMineralUptakeWithDensityImpactWithFungi;
function<UptakeResults(vector<double> &DistWithDepth, size_t PlantIndex, double N_DemandPlant, double N_Tot_Demand,
	vector<double> Dist_Tot, vector<double> &SoilNitrate, vector<double> &SoilAmmonium,  vector<double> &FungiCover, UptakeResults &out)> f_MineralUptakeWithFungi;
function<PUptakeResults(vector<double> &DistWithDepth, size_t PlantIndex, double N_DemandPlant, double N_Tot_Demand,
	vector<double> Dist_Tot, vector<double> &SoilPhosphate, vector<double> &FungiCover, bool FungiUptake, PUptakeResults &out)> f_PMineralUptaketoFungi;//HH
function<UptakeResults(vector<double> &DistWithDepth,size_t PlantIndex, double N_DemandPlant, double N_Tot_Demand,
	vector<double> Dist_Tot, vector<double> &SoilNitrate, vector<double> &SoilAmmonium, vector<double> &RootDensity, vector<double> &FungiCover, bool FungiUptake, UptakeResults &out)> f_MineralUptaketoFungiWithDensityImpact;
function<UptakeResults(vector<double> &DistWithDepth, size_t PlantIndex, double N_DemandPlant, double N_Tot_Demand,
	vector<double> Dist_Tot, vector<double> &SoilNitrate, vector<double> &SoilAmmonium, vector<double>& FungiCover, bool FungiUptake, UptakeResults &out)> f_MineralUptaketoFungi;
    
function <UptakeResults(vector<double> &DistWithDepth, size_t NumLayers, double N_DemandPlant, double N_Tot_Demand,
	vector<double> Dist_Tot, vector<double> &SoilNitrate, vector<double> &SoilAmmonium, UptakeResults &uptprev, double ScaleComp, UptakeResults &out)> f_CompUptake;

function < OrganicUptakeResults(vector<double> &DistWithDepth, size_t NumLayers, double N_DemandPlant, vector<double> Dist_Tot, vector<double>& Litter1, vector<double>& Humus)> f_OrganicUptake[NUM_BIOELEM-1];
    
function < OrganicUptakeResults(vector<double> &DistWithDepth, size_t NumLayers, double N_DemandPlant, vector<double> Dist_Tot, vector<double>& Litter1, vector<double>& Humus,  vector<double>& RootDensity)> f_OrganicUptakeWithDensity[NUM_BIOELEM - 1];
    
function < OrganicUptakeResults(bool Fungi, vector<double> &DistWithDepth, size_t NumLayers, double N_DemandPlant, vector<double> Dist_Tot, vector<double>& Litter1, vector<double>& Humus)> f_OrganicUptaketoFungi[NUM_BIOELEM - 1];
    
function < OrganicUptakeResults(bool Fungi, vector<double> &DistWithDepth, size_t NumLayers, double N_DemandPlant, vector<double> Dist_Tot, vector<double>& Litter1, vector<double>& Humus, vector<double>& RootDensity)> f_OrganicUptakeWithDensitytoFungi[NUM_BIOELEM - 1];
    

function <vector<double>(vector<double>)> f_DemandDrivenNFixation;
function < vector<double>(vector<vector<double>>&, vector<double>&, vector<double>&, vector<double>&, vector<double>&, vector<double>&, vector<double>&, vector<double>&, vector<double>&, vector<double>&)> f_AboveGroundDrivenNFixation;
function < vector<double>(vector<vector<double>>&,  vector<double>&, vector<double>&, vector<double>&, vector<double>&, vector<double>&, vector<double>&, vector<double>&)> f_BelowGroundDrivenNFixation;

ZadokOutCreateFile* pZadokOutCreateFile{ nullptr };
vector<string> zadokoutstrings, ZadokDates, sitename;
bool shift;
double z0;
fstream outfile;






};

