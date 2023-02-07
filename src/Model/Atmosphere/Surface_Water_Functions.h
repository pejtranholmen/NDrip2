#pragma once
#include "./SoilEvaporation.h"



class Surface_Water_Functions : public SoilEvaporation
{
	public:
		Surface_Water_Functions(void);
		~Surface_Water_Functions(void);
		bool Def();

	size_t  Furrow,  Dripper, PumpStation;
	double SoilIrrigF;
			//	! Storage
	double Spoolinflow, SpoolSoilInfil, SpoolRunoff;
	double FurrowInfil, FurrowPrec;
	double SpoolCover;	//				! Auxiliary

	double PumpPool, PumpInFlow, PumpOutFlow, SpoolDelayRunoff;

	double SurfCoef; //		! P -Previous SURDEL 
	double SurfPoolMax;	//! P -Previous SPOOLMAX
	double SPCovTot;	//	! P -Previous SPCovTot
	double InfFurrow;	//! P -Previous INFBYFRAC
	double SoilCover;	//! P -Previous SOILCOVER
	double SurfPoolInit; //	! P - New introduced 2003-03-14 PEJ
	double SPCovMax;	//	! P - New introduced 2004-10-07 PEJ
	double SPCovPot;	//	! P - New introduced 2004-10-07 PEJ

    double PumpRate, PumpLevelStart, PumpLevelStop, PumpPartitioning;

	double FracInfiltrationHighFlow;

	function<double(double)> f_SurfaceRunOff;
	function<double(double)> f_SurfaceWaterCover;
};

