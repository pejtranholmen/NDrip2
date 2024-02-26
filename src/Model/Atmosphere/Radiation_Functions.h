#pragma once

#include "./Meteorological_Data.h"
#include "../SOIL/Soil_Frost.h"



class Radiation_Functions : public Meteorological_Data
{
	public:
		Radiation_Functions(void);
		~Radiation_Functions(void);
		bool Def();
		bool FunctionDef();

  size_t  NetLongRad , InLongRad, Turbidity; 
  size_t  NumBrunt	 ,NumAlbSnow, NumKonz, NumBruntIn, NumSatterl;
  //! --David developement
  size_t  DistributeRadFunc,LongRadCanopy;
  size_t  SoilPartitioningArea;  // From Soil Evaporation


 // size_t RadiationInput;
  double FracBare, SnowAge, TempSnowSurf;
  vector <double> SnowReduceLAIFactor;
  vector<double> MC_TempCanopy;
  double SnowReduceLAIThreshold, SnowReduceLAIMax;

	
  double DripIrrigCover,DripIrrigXCenter; // From Irrigation
  vector<double> BruntAirCoef, BruntAirSoilCoef	,BruntCoef;
  double BruntAirCoef_1, BruntAirCoef_2, BruntAirCoef_3, BruntAirCoef_4;
  double BruntAirSoilCoef_1, BruntAirSoilCoef_2, BruntAirSoilCoef_3, BruntAirSoilCoef_4;
  double BruntCoef_1, BruntCoef_2, BruntCoef_3;
  vector<double> KonzelmannCoef, SatterlundCoef,	RntLAIv;
  double KonzelmannCoef_1, KonzelmannCoef_2, KonzelmannCoef_3;
  double SatterlundCoef_1, SatterlundCoef_2;
  double AlbedoVar,  RadNetShort, RadInLongGround, RadInLongGround1,RadInLongGround2;
  double Albedo, AlbLeafSnowCoef, Solar_Time_Adjust;
  double Emissivity;
  RadTempAmpPar ParB;

  double Bristow_Turb, Bristow_B, Bristow_C;
  double Thornton_B0, Thornton_B1, Thornton_B2;
  double DTR_YEAR_MEAN, DTR_YEAR_AMP, DTR_YEAR_SHIFT;

  double	RntLAI;	
  double AlbedoKExp, AlbedoDry, AlbedoWet;
  double RadFracAng1, RadFracAng2, AlbSnowMin;
  vector<double> AlbSnowCoef, MC_RntDist, MC_LAIAbove;
  double AlbSnowCoef_1, AlbSnowCoef_2, AlbSnowCoef_3;

  vector<P*> mTab_Brunt, mTab_SnowAlb, mTab_BruntInLong, mTab_Konzel, mTab_SatterL, mTab_Beer;

  //! Define new Local Variables
  //Real RelSun         ! Relative clear sky fraction

  //! David 2008-09-02 New shortwave radiation interception model for Manfreds Alptal Simulations (based on SNOWMIP2 work 2007)
  //! Switches
  size_t CanopyShade,SplitGlobal;//     ! Switch to select new radiation interception model
  //! variables
  double CanopyFracRadLong,RadNetLongCanopy,RadNetLongGround,RadLongOutVar,RadNetShortCanopy,RadInShortGround,RadNetShortGround,RadShortOut,RadNetCanopy;
  double SnowAlbedoVar, SoilAlbedoVar, GroundAlbedoVar;
  
  
  double CanopyFracRadDirect, CanopyFracRadDiffuse, RadInDiffuse, RadInDirect;
  double CanopyFracRad, CanopyFracRad1, CanopyFracRad2, RadNetGround1, RadNetGround2, RadNetGround;
  double DiffuseRadCoef, ViewScalingMax, CanopyShadeCoef, CanopyShadeCoef_Base;
  //! local variables
  bool DirectShortWaveSwitch,DistributedRadiationReset; //! local logical switches to control the flow of radiation interception model


 
 // double AlbedoSnowSumFunc(double TSumAlbedo, double SnowAge);
  /*double BeerLaw(double LAI);
  double BeerLaw(size_t index, double LAI);*/
  //double RadNetLong_RelSat_RelSun(double Tair, double relsat, double relsun) { return RadNetLong(Tair, SVP(Tair)*relsat, relsun);};
  //double RadNetLong(double Tair, double actv, double relsun, double tempsurf=0);
//  double RadLongIn_RelSat_RelSun(double Tair, double relsat, double relsun) { return RadLongIn(Tair, SVP(Tair)*relsat, relsun);}
/*  double Emmisivity_In_Konzelmann_Relsat(double Tair,double relsat, double relsun) {return Emmisivity_In_Konzelmann(Tair,SVP(Tair)*relsat,  relsun);  };
  double Emmisivity_In_Satterlund_Relsat(double Tair,double relsat, double relsun) {return Emmisivity_In_Satterlund(Tair,SVP(Tair)*relsat,  relsun);  };
  double Emmisivity_In_Brunt_Relsat(double Tair,double relsat, double relsun)  {return Emmisivity_In_Brunt(Tair,SVP(Tair)*relsat,  relsun);  };
  double Emmisivity_In_Konzelmann(double Tair,double actv, double relsun) ;
  double Emmisivity_In_Satterlund(double Tair,double actv, double relsun) ;
  double Emmisivity_In_Brunt(double Tair,double actv, double relsun);
  double RadLongOut(double Temp);


  //double Angstrom(double relsun);
  double BristowCampbell(double Tempdiff, bool Fixed_B);
  double Dtr_DayFunc();
  double Thornton_B(double tempdiff);
  double DiffuseGlobalRatio_Cloud(double Cloud) {return DiffuseGlobalRatio(0., Cloud);};
  double DiffuseGlobalRatio( double ThetaAngle, double Cloudiness);*/

 /* double CanopyShadeCoverFunction_SunAngle(double Angle) {return min(3., CanopyShadeCoverFunction(Fix::PI/2-Angle/90.*Fix::PI/2, 1.)); };
  double CanopyShadeCoverFunction(double ThetaAngle, size_t index);
  double CanopyShadeCoverFunction(double ThetaAngle, double height);*/
 // bool Init_Sunshine();

// Phys Functions

  function<double(double)> f_daylength, f_potrisday, f_radlongout;
  function<double(double, double)> f_radlongout_e, f_potrishour,  f_ThetaAngleHour;
  function<double(double, double, double)> f_atmcorrection;
// Local Functions

  function<double(double)> f_AlbedoSoil,f_Angstrom;
  function<double(double,double)> f_AlbedoSnow, f_DiffuseRadRatio, f_SlopeCorrDirectRad;

  function<double(double)> f_BeerSingleLeaf;
  function<double(size_t, double)> f_BeerAbsorbedPerLayer;
  function<double(double, double,double)> f_RadNetLong_Air, f_RadLongIn_Konz, f_RadLongIn_Satt, f_RadLongIn_Brunt;
  function<double(double, double, double)>  f_E_RadLongIn_Konz, f_E_RadLongIn_Satt, f_E_RadLongIn_Brunt;
  function<double(double, double)>  f_E_Cloud_RadLongIn_Konz, f_E_Cloud_RadLongIn_Satt, f_E_Cloud_RadLongIn_Brunt;

  function<double(double, double, double, double)> f_RadNetLong_AirSoil;

  function<double(double, double)> f_CanopyShadeCover;
  function<double(double, double)> f_RadGlobalTempAmplitude;

// Local Variables

  	vector<double> RadFractions,RadfractionsDirect;
	double RelSun;
	double Tsum_albedo;

// Connection to other external modules
 private:




};

