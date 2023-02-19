#pragma once
#include "./PhysConst.h"



class PhysFunctions: public PhysConst {
	public:

	PhysFunctions(){ px=py=Solar_Time_Adjust=m_Hour=m_ThetaAngle=0;};
	~PhysFunctions(){};
	
	double SNLT, CSLT, SSIN, CCOS, DEC;
	int INIT_CASUN;
	bool SLOPE_CORR;
	  
	int ICountErr;
	double Solar_Time_Adjust, px, py;
	double m_Altitude, m_Angstrom1, m_Angstrom2, m_Hour, m_ThetaAngle;


	// from General functions
	double SinTemp( double TAirMean, double TAirAmpl, double TAirPhas, double YCH, double Damp, double Time, double day, double depth);
	double AirDensityTempFunc(double Temp);
	double SVP(double Temp);
	double TempVapourPressure(double VapourPressure);
	double SVP_David(double Temp);
	double WetBulb(double Temp);
	double DSVP(double Temp);
	double Conc_Vap(double pressure, double Temp);
	bool CaSun_Init(double Lat, double Alt=0, double Ang1=0.22, double Ang2=0.5);
	bool SunDayInit(double Day);

	double ThetaAngle(double Hour=-1, double Day=-1.);

	double PotRisDaily(double Day);
	double DayLengthFunction(double Day);
	double SlopeCorr(double Hour=-1);
	double PotRisHour(double Hour=-1);
	double PMEquation(double Tair, double RadAvail, double VapourPDef, double ResAir, double ResSurf, double LAI=1);
	double PMRad(double Tair, double RadAvail, double VapourPDef, double ResAir, double ResSurf, double LAI=1);
	double PMVent(double Tair, double RadAvail, double VapourPDef, double ResAir, double ResSurf, double LAI=1);
	double ShawP_Rough(double CanDensMax,double PAI, double CanopyHeight, double RoughLmin, double RoughLmax);
	double ShawP_Displ(double CanDensMax, double PAI, double  RefHeight,double CanopyHeight);
	double RHPsi(double Psivalue, double Tempvalue, double EcorrValue=1);
	double AtmosphereCorrection( double temp, double relhum, double Hour);

	double D_vap(double DvapB,double AirP,double Temp);
	double D_oxygen(double DturtO2, double AirP, double Temp);
	double D_CO2(double DturtCO2, double AirP, double Temp);
	double D_NO(double DturtO2, double AirP, double Temp);
	double GasDiff_TempFunc(double Temp);
	double Sens_AirF(double TempDiff, double Ra);
	double Latent_AirF(double VapourDiff, double Ra);
	double RAFunction(double WindSpeed,double Height, double RoughLVar);
	double RAFunction(double WindSpeed, double Height, double RoughLVarMom,double RoughLVarHeat);
	double StabilityCorrAnalytical(double DiffHeight, double DiffTemp, double  TempAir, double WindSpeed, double StabCoef1, 
                                    double StabCoef2, double StabCoef3, double StabCoef4);
	double RichardsonToEta(double DiffZ, double TempAir, double DiffTemp, double WindSpeed,double zom,double zoh, double etaold,double maxitt);
	double psi_m(double xi);
	double psi_h(double xi);
	double FrictionVelocityFunction(double eta,double Wind, double DZ, double zom);
	double RaHeatfunctionGeneral(double MinExchangeCoef,double Ustar, double DiffHeight,double Zo, double Eta);
	double WindProfileFunction(double UU, double zU, double zT, double eta, double zom);
	double RadLongOut(double TempSurf, double Emissivity=1);


};
