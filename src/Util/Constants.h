#pragma once
class Constants
{
public:
Constants();
~Constants();

public:
	 double DENSW;
	 double LATMELT, LATE, DENSA, KARMAN, SPECA, LATESUB;
	double	GAMMA, STEFB;
	 double	GRAVCONST, RGAS, RGASC, DNOT, DNOT_O2, DNOT_CO2, MOLW;
	 double SPECI, SPECW;
	 double	PI, LOG_PI, RAD,DENSICE;
	  double SOLAR_CONST, SECPERDAY;

	 double DNOT_CH4,MOL_CO2, MOL_C, MOL_O2, MOL_AIR;
	 double MOL_RATIO_O2_C, MOL_RATIO_O2_AIR, MOL_RATIO_CO2_AIR, RATIO_C_O;
	 double MOL_VOL_AIR, ABS_ZERO, NORM_ATMPRES;
	 double PO2, MOL_RATIO_CH4_AIR;

	 double DensityofSolidSoil, HeatCapSolidSoil, TempCompFrozenSoil;
	 double MolMassSalt, PatoCmWater, BiomassToCarbon, TempToOsmoticP;
	static Constants& C() {
		static Constants mySingle;
		return mySingle;
	}

};
