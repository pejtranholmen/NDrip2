#pragma once

struct DENS_SNOW {
	double CR; // compaction rate
	double Height;
	double Density;
};

#include "./Radiation.h"
#include "../SOIL/Soil_Frost.h"
#include "../PLANT/Plant.h"


class SnowPack_Functions : public Radiation
{
	public:
		SnowPack_Functions(void);
		~SnowPack_Functions(void);
		bool Def();
		bool FunctionDef();
// Switches
	size_t SnowSurEbal, SnowAdjustment, SnowRoughness, SnowPloughing, ChlorideMelt;
	size_t StabilitySnow, SnowMeltFunctionSwitch, NewSnowDensityModel, SnowDensification;

//Parameters
double	SAgeM1, SAgeM2, SAgeZP, SAgeZQ;   
double	SDensLi,SDensMass, SDensNew, SDensNewImport; 
double	Temp0Li;  
double	MeltReFr, MeltRis, MeltTemp, MeltFSoil, MeltChlorideConc;
double	SRetens;  
double	SThCon;
double	HCSnowCover;   
double	RoughLMomSnow;
double	AgeUpdateSnowDepthCorr;
double ChlorideMeltConc;

double PloughSnowMass, PloughEffSnow;


//!double	OverburdenDens_c2,OverburdenTemp, MetaMorph_c3,MetamorphTemp_c4, & 
//!					MetaMorphLiq_c5,MetaMorphDens,CompMeltCutOff, Overburden_eta0
double	CRMetaMorphTemp1,CRMetaMorphTemp2,CRMetaMorphDens,CRMetaMorphDensMin, 
				CRMetaMorphLiq,CROverburdenTemp,CROverburdenDens,CROverburdenVisc,CRCompMeltCutOff;

double SnowDepthInitial, SnowMassInitial,WindlessExChangeSnow;




// State Variables
  	double WTotSnow, HSnow;
// Flow Variables
	double SnowExport;
	double DENSS, SWATS,  SnowOutFlow, SnowInFlow, TempSnowSoil;
// Auxiliary Variables

double SnowSensF, SnowLatentF, SnowSurfHeatF, RadNetSnowCover, 
				 RaSnowSurf, SnowEbal ,VapourPSnowSurf,RadNetShortSnow,RadNetLongSnow;

double PrecAdjustSnow, SnowMeasured;

double	SnowEvaporation;
double EtaSnow,UstarSnow,WindSpeedSnow ;
double WICE,TempSnow, QMeltSurface,QSnowSoil;





// Connection to other external modules

//	Plant *mp_Plant; 

// Connection to Snow local variables

	double RoughLHeatSnow, LateVapSnow;

	bool reset_snow;
	double OldSnowDepth;
	double TISTEP;


double MPOT, MRIS;

double DENSICE_N, DENSLIQ_N, DENSS_N;
double SNOWP, RAIN;
double WICEOLD, SWATSOLD, WTotSnowOld;
double HSNOW_NEW;
double POTMELT,MELT,MELTHEAT,KSNOW;
double PotSublimation,Sublimation,DELTAH,Condensation;
double POTMELTSURF,MELTSURF,MELTHEATSURF;
double ThCondUpper,KSNOWSOIL,RSNOWSOIL,HeatChangeSnow,TCHANGE,RETENS;
double DENSINF,HINF,DENSO,HOLD,CR,david,Rsnow,DENS_RIME;
double BULKDENS_ICE,BULKDENS_LIQ;
double POTMELTPREC;
double MELTPREC;
double MELTHEATPREC;
double POROSITY;
double SATURATION;
double TOTALMELT;
double CR1,CR2,CR3,DM, FactorOfHeightChange;
double Snow_Increase, Snow_Prev;


//




//!  End Snow_Dyn


//! Snow Temp i

	double	Tcond,  ConcSurf, ConcSurf1,  VapourPSoilU, 
		AirP, DatMU, VapGra, VapourPAirSat,   Change  
		,EtaLow,RaSnowUncorrected,Tbelow,ZSnow;

	double  Rsoil, ThetaU, KSOIL;
	int Irep;
	bool POS, NEG;
//!! Removed RSNOW, KSNOW, KSNOWSOIL, RSNOWSOIL	
	
	
//! Snow Temp	


//! Adjust Snow Depth

	double SnowDensity;
	double Temp_WtotSnow,epsilon,OldSDM;
	
	
// New check for ebal snow
       double SnowSensF_Old, SnowLatentF_Old;


// Functions


	double SnowSoilTemp_Function();
	double RetentionFunction(double SnowAmount);
	DENS_SNOW SimpleDensityFunction(double liquidRation, double TotalMass);
	DENS_SNOW SNTermDensityFunction();
	double SnowHeatContentFunction( double TempSnow, double ice, double water);
	double SnowTempFunction(double HeatContentSnow, double ice, double water);
	double BareSoilFunction(double SnowHeight);
	double UpdateSnowDensity();
	double AdjustedSnowPrecAndSnow();
	double ThermQualPrecFunction(double _TAir);
	double ThCond_R20(double Theta, double Thqualv);
	double ThCond_Upper(double Theta, double Thqualv);
	double SteadyStateSol(double UppperHeight, double UpperTh, double LowerHeight, double LowerTh, double TempAbove, double TempBelow);
// Functors
	function<double(double, double, double, double)> f_StabCorrAnalytical;
	function<double(double, double, double, double)> f_ResAirMom;// FUNC_CHOICE::RES_AIR_MOM_HEAT

	function<double(double, double)> f_SensibleFlowByResistance, f_LatentFlowByResistance;

	function<double(double)> f_ThCond_Snow;
	function<double(double)> f_BareSoilCover_Snow;

	function<double(double, double)> f_SaltMelt_Road, f_NewSnowDensity;
	function<double(double, double, double, double, double)> f_SnowMelt;
	function<double(double, double)> f_ResAir_SnowNeutral;
	function<double(double, double, double, double)> f_ResAirSnow;

};

