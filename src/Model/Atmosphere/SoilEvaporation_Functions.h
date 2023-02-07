#pragma once
#include "./SnowPack.h"
#include "../SOIL/Soil_Frost.h"
#include "../PLANT/Salt.h"


class SoilEvaporation_Functions : public SnowPack
{
	public:
		SoilEvaporation_Functions(void);
		~SoilEvaporation_Functions(void);
		bool Def();
		bool FunctionDef();

// Connection to other external modules

	Salt *mp_Plant;

	size_t  SurEbal,SoilRoughness;
	size_t	SurfaceCover;

	double RoughLBareSoilMom,WindLessExchangeSoil,EGPsi; 
	double PsiRs_1p, PsiRs_3pf1, PsiRs_3pf2, PsiRs_3pf3;
	double MaxSoilCondens, MaxSurfDeficit, MaxSurfExcess, RaLAI;
	double SoilCoverEvap, SoilCoverRunOff;


	//!	Auxiliaries
	double RaSoilSurf, SoilLatentF, SoilSensF, 	SoilEbal, PotEvapGround, VapourPSurf, EAvailSurf;
	double RsSoilSurf,RadNetBareSoil, RadNetShortBareSoil,RadNetLongBareSoil;// ! DG 080902
	double Surfmos;
	//! Introduced new variables for partioning of soilevaporation 15 October 2004

	double RaSoilSurf1, SoilLatentF1, SoilSensF1, SoilEBal1, 	RadNetBareSoil1, SurfMos1,  Fraction1;

	double RaSoilSurf2, SoilLatentF2, SoilSensF2, 	SoilEBal2, 	RadNetBareSoil2, SurfMos2;

	double SoilEvaporationFlux, SurfHeatflow1, SurfHeatFlow2, SoilEvaporation2, SoilEvaporation1;
	double SurfHeatFlow1, TempBareSoil1, TempBareSoil2;



	//! New variables and switches for new surface flux computations
	//!Switches
	size_t StabilitySoil;
	//!parameters
	//!double  RoughLBareSoilHeat
	//! Auxiliary variables
	//double EtaSoil;// !,WindspeedIterations,EtaSoilIterations,EbalSoilIterations;
	// Removed EtaSoil to Functors

	//! Local Defined Variables
	double		RoughLBareSoilHeat, SoilCoverEvapPar, SoilCoverRunoffPar;
	double     Thetau;//      ! Upperlayer moisture content Used both in _dyn and in Subroutine SurfaceHeatBalance
	                    //! Prior 4 sept 2008 declared both in dyn and subroutine locallay, causing errors in calculation
	                    //! in subroutine
	size_t SurfaceCoverSwitch;
    
    
    bool Pos, Neg;
    int Irep;
    double TDiffC;
	bool MulchInUse;

/*
	double RelHumidity(double PsiSoil, double Temp, double SurfMos);
	double Ecorr(double Surfmos);

	double RaBound(double LAIv);
	double RsSoil(double Psiv, double Surfmos);

	double RaFuncSoilNeutral(double WindSpeed);

	double RAFuncSoil(double WindSpeed, double Height) {return pSub->RAFunction(WindSpeed, Height, RoughLBareSoilMom);};

	double StabilityCorrAnalytical(double WindSpeed, double DiffHeightTemp, double TempDiff, double TempAir);
	*/
// functors

	function<double(double, double, double, double, double)> f_ResAirSoilSurface, f_PM_EquationSoil;
	function<double(double, double)> f_ResNeutralAirSoilSurface, f_TempDiffFromFlux, f_VapourDiffFromFlux;
	function<double(double, double, double)> f_RelHumSoilSurface;
	function<double(double, double)> f_SoilSurfaceResistance;
	function<double(double)> f_RaSoilFromVegCover;


private:

	
	//typedef double (PhysFunctions::*pFuncPhys0_3)( double, double, double);

	//vector<pFuncPhys0_3> m_fPhyspointer0_3;

};

