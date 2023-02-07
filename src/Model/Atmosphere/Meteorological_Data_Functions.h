#pragma once
#include "../SimModule.h"
#include "../SOIL/Soil_Frost.h"
#include "../PLANT/WaterUptake.h"
class Additional_Variables;





class Meteorological_Data_Functions : public SimModule
{
	public:
		Meteorological_Data_Functions(void);
		~Meteorological_Data_Functions(void);
		bool Def();
		bool FunctionDef();
		double PotTransp;


size_t SoilEvap ;    // From Soil evaporation
double TempBareSoil;
size_t SnowPloughInput;  // From SnowPack
double SnowImport;
double SoilInfil, SurfacePool;	 // From Surface Water

double  PSnowLim, PRainLim, ThQualP;    //from SnowPack
size_t  TairCycle, TAirGlobRad;
size_t  PrecInput, TAirInput,HumRelInput,WSpeedInput,
						RadNetInput,RadGlobInput,CloudInput,DBSunInput,
						PotTranspInput,TSurfInput , RadInLongInput, VapourAirInput, 
						CommonRefHeight, TempDayDiffInput, TrafficInput;
//! Heat Island
size_t  SweClimScenarious, SweClimRegions;// !, UrbanHeatIsland
size_t ClimIndexFileName, TempCycleInput, RunOnInput;

double  PrecA0Corr, PrecA1Corr, PrecRate, PrecFreq, PrecStartDay;
double  TAirMean, TAirAmpl, TAirPhase,   RefHeight;
double  WSpeedMean, HumRelMean, CloudFMean;
double CloudMinTemp, CloudMaxTemp, TempDayDiff_Fixed;

vector<double>  Wsource;

double  IrrigV_Index;// ! To Identify which of many Irrigation variables
//! -- David 1999-04-25 --
//! -- New parameters for topografic corrections of global radiation and temp. --
double  AltMetStation, AltSimPosition, TairLapseRate;

double  TAir, RadInShort;	//! Driving variables
double  PrecMeas, HumRel   ,  VapourPAir, VapourPDef;	
double  WindSpeed ,	 RadNetTot	;//! David 19/6
double  RadInLong ,CloudFrac ,	DurBriSun;
double IrrigationRate,	   TempDayDifference;
double PrecCorr;					  
//! PEJ 9/8 2005
double PotTranspMax;
double  SpoolRunOn;
double px,py;// Check that those are not in the radiation

double  PrecScaleF, TempAddCoef ;//!, UrbanSmoothFrac

double RadInLong_Index, RadGlob_Index, Prec_Index, TAir_Index, HumRel_Index, VapourAir_Index;
double WSpeed_Index, RNT_Index, Cloud_Index, DBSun_Index, PGFileName_Index, TempDiff_Index, TrafficIntensity;

double Latitude; // moved from Radiation module

//! Local variables

vector <double> Ecodat,TairMeanVector;
double RefHeightWind,RefHeightTemp,TAirDailyMean;

double RefHeightTemperature;

// Pg Files

	CPG m_PG_Meteor, m_PG_HeatPump, m_PG_Salt;
	CPG m_PG_LaterWater, m_PG_Snow, m_PG_Source;
	PGPOS m_PGPOS_Meteor[10];
	PGPOS m_PGPOS_HeatPump, m_PGPOS_Salt;
	PGPOS m_PGPOS_LaterWater, m_PGPOS_AdjustSnow, m_PGPOS_Source;

	
	
	
	//!	Positions in Driving variable file
ININFO	TairIpos, TSurfIpos, PrecIpos, RadGlobIpos, HumRelIpos, 
	RadNetIpos, CloudFIpos, DBSunIpos, WSpeedIpos, IrrigIpos, PotEvapIpos, 
	RadInLongIpos, VapourAirIpos, WetBulbIpos , TempDiff_Ipos, TrafficIpos;


ININFO    LateralIpos, RunOnIpos, DrainageCount, SnowImportIpos, SaltDrainCount, SaltDrainIpos;

ININFO IDisPrec, IDisRadNet, IDisRadGlob;
ININFO ISnowAdjust;
size_t NumValidInput1 , NumValidInput3 ;
	
bool DrivPG1, MISSRIS, MISSRNT , MISSCLOUD, MISSRIL;

int ISOURCE;

double YCH;

vector<double> DT_A, DT_B, PrecCoef;


 Soil_Frost *p_Soil_WaterF; // Soil Water Flow module
 WaterUptake* p_Plant;
 SimModule* p_Soil_HeatF;
 Additional_Variables *p_AdditionalVar;

 double AirTempDayFunction(double fractOfDay);
 double AirTempYearFunction(double Day);
 double Precgenerator(size_t DayNum);
 double Precgenerator(double DayNum);
 bool OpenSweClimFile();  
 double NewTemp(double temp, double Iday);
 double NewVapourP(double VapourP, double Tair, double Iday);
 double NewPrec(double prec, double Iday);

 double PrecipitationCorrectionFactor(double Temp);
// double VapourPressureAirTemp(double Temp, double RelHum);
 //Phys Functors
 function<double(double)>  f_SinDay, f_SinYear;
 function<double(double, double)> f_avp_rh;
 function<double(double)> f_svp;
// Local Functors
 function<double(double)> f_pgen, f_TempAlt, f_MeanDailyTemp, f_PrecCorrFactor;
 
//Integer IUnit

 private:



};

