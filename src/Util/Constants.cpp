
#include "Constants.h"
Constants::Constants(void)
{

	DENSW = 1000.;
	DENSICE = 917.;   // Density of Ice
	LATMELT = 3.34E5;  //		! Heat of melting
	LATE = 2.45E6;  ///			! Latent heat of vaporization  * <J/KG>
	LATESUB = 2.84E6;//      ! Latent heat of sublimation   <J/kg>
	DENSA = 1.220;  //			! Density of air <KG/M**3> 	at 20 
	KARMAN = 0.41;  //			! Karmans constant <>                                            
	SPECA = 1004.; //			! Specific heat of air  <J/KG*C>                                           

	SPECI = 2.1E3;
	SPECW = 4.2E3;

	GAMMA = 66.;
	STEFB = 5.67E-8;
	SOLAR_CONST = 1361.;
	SECPERDAY = 86400.;

	// Vapour constants

	GRAVCONST = 9.80665;
	RGAS = 8.314;//	!  J/(molK)
	DNOT = 2.29E-5;///	  ! m2/s
	MOLW = 18.016;//
	RGASC = 47.04;//



	PI = 3.1415927;
	LOG_PI = 0.918939;
	RAD = 0.01745329;
	DensityofSolidSoil = 2.65;//	! DenSity of Soil Solid fraction
	HeatCapSolidSoil = 2.E6;//		! Heat capacity of Soil Solid fraction
	TempCompFrozenSoil = -6.;//		! Temperature for completly frozen soil
//!   Changed salt to be Cloride mass only
	MolMassSalt = 35.4527;	//			!	22.98977+35.4527=58.341
  //! Changed to 35.4527 by PEJ 22 oct 2004 after discover of error by Louise previous value 23.341 after 
	PatoCmWater = 0.1 / 9.80665;	//	! Conversion factor
	BiomassToCarbon = 0.45;	//	! Conversion factor 

  // Oxygen (O2) coefficients

	DNOT_O2 = 2.6E-5;//			! Diffusion of oxygen at 20 C (m2/s)
	DNOT_CO2 = 1.92E-5;//			! Diffusion of oxygen at 20 C (m2/s)

	DNOT_CH4 = 2.6E-5;//			! Diffusion of oxygen at 20 C (m2/s)
	MOL_C = 12.011;//				! Molecular weight of carbon
	MOL_CO2 = 44.0098;//			! Molecular weight of carbondioxide
	MOL_O2 = 31.998;//			! Molecular weight of oxygen
	MOL_AIR = 29.0;//				! Molecular weight of dry air
	MOL_RATIO_O2_C = 2.664058;//	! Mole ratio oxygen to carbon   
	MOL_RATIO_O2_AIR = 1.10;//! Mole ratio oxygen to Air
	MOL_RATIO_CO2_AIR = MOL_CO2 / MOL_AIR;//! Mole ratio oxygen to Air
	MOL_VOL_AIR = 44.6;//! Mole air per cubic meter (mol/m3)
	ABS_ZERO = 273.15;//! Absolute zero (negative sing removed)
	NORM_ATMPRES = 1.013E5;//	! Normal atmospheric pressure (Pa)
	PO2 = 20.5E3;//	! Oxygen partial pressure atmosphere
	MOL_RATIO_CH4_AIR = 0.552;//   ! Mole ratio CH4 to air
	TempToOsmoticP = -8.19E-5;





}
Constants::~Constants(void) {}