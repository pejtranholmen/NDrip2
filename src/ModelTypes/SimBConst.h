#pragma once
namespace Fix {
	static constexpr double  DENSW = 1000.;
	static constexpr double DENSICE = 917.;   // Density of Ice
	static constexpr double LATMELT = 3.34E5;  //		! Heat of melting
	static constexpr double LATE = 2.45E6;  ///			! Latent heat of vaporization  * <J/KG>
	static constexpr double LATESUB = 2.84E6;//      ! Latent heat of sublimation   <J/kg>
	static constexpr double DENSA = 1.220;  //			! Density of air <KG/M**3> 	at 20 
	static constexpr double KARMAN = 0.41;  //			! Karmans constant <>                                            
	static constexpr double SPECA = 1004.; //			! Specific heat of air  <J/KG*C>                                           

	static constexpr double SPECI = 2.1E3;
	static constexpr double SPECW = 4.2E3;

	static constexpr double GAMMA = 66.;
	static constexpr double STEFB = 5.67E-8;
	static constexpr double SOLAR_CONST = 1361.;
	static constexpr double SECPERDAY = 86400.;

	// Vapour constants

	static constexpr double GRAVCONST = 9.80665;
	static constexpr double RGAS = 8.314;//	!  J/(molK)
	static constexpr double DNOT = 2.29E-5;///	  ! m2/s
	static constexpr double MOLW = 18.016;//
	static constexpr double RGASC = 47.04;//

	static constexpr double PI = 3.1415927;
	static constexpr double LOG_PI = 0.918939;
	static constexpr double RAD = 0.01745329;
	static constexpr double DensityofSolidSoil = 2.65;//	! DenSity of Soil Solid fraction
	static constexpr double HeatCapSolidSoil = 2.E6;//		! Heat capacity of Soil Solid fraction
	static constexpr double TempCompFrozenSoil = -6.;//		! Temperature for completly frozen soil
							 //!   Changed salt to be Cloride mass only
	static constexpr double MolMassSalt = 35.4527;	//			!	22.98977+35.4527=58.341
							//! Changed to 35.4527 by PEJ 22 oct 2004 after discover of error by Louise previous value 23.341 after 
	static constexpr double PatoCmWater = 0.1 / 9.80665;	//	! Conversion factor
	static constexpr double BiomassToCarbon = 0.45;	//	! Conversion factor 

	static constexpr double DNOT_O2 = 2.6E-5;//			! Diffusion of oxygen at 20 C (m2/s)
	static constexpr double DNOT_CO2 = 1.92E-5;//			! Diffusion of oxygen at 20 C (m2/s)

	static constexpr double DNOT_CH4 = 2.6E-5;//			! Diffusion of oxygen at 20 C (m2/s)
	static constexpr double MOL_C = 12.011;//				! Molecular weight of carbon
	static constexpr double MOL_CO2 = 44.0098;//			! Molecular weight of carbondioxide
	static constexpr double MOL_O2 = 31.998;//			! Molecular weight of oxygen
	static constexpr double MOL_AIR = 29.0;//				! Molecular weight of dry air
	static constexpr double MOL_RATIO_O2_C = 2.664058;//	! Mole ratio oxygen to carbon   
	static constexpr double MOL_RATIO_O2_AIR = 1.10;//! Mole ratio oxygen to Air
	static constexpr double MOL_RATIO_CO2_AIR = MOL_CO2 / MOL_AIR;//! Mole ratio oxygen to Air
	static constexpr double MOL_VOL_AIR = 44.6;//! Mole air per cubic meter (mol/m3)
	static constexpr double ABS_ZERO = 273.15;//! Absolute zero (negative sing removed)
	static constexpr double NORM_ATMPRES = 1.013E5;//	! Normal atmospheric pressure (Pa)
	static constexpr double PO2 = 20.5E3;//	! Oxygen partial pressure atmosphere
	static constexpr double MOL_RATIO_CH4_AIR = 0.552;//   ! Mole ratio CH4 to air
	static constexpr double TempToOsmoticP = -8.19E-5;
	static constexpr double ThermalCond_Air = 0.025;
	static constexpr double ThermalCond_Organic = 0.25;// Organic Matter
	static constexpr double ThermalCond_Mineral = 2.5;
	static constexpr double ThermalCond_Quarts = 8;
	static constexpr double ThermalCond_Water = 0.57;
	static constexpr double ThermalCond_Ice = 2.21;
	static constexpr double GWP_C_CO2 = 3.6666 * 1. * 3.6525; // to Kg CO2e /ha/yr
	static constexpr double GWP_C_CH4 = 1.3333 * 28. * 1. * 3.6525;// to CO2e/ha/yr
	static constexpr double GWP_N_N2O = 1.5714 * 265. * 1. * 3.6525;// tp CO2e/ha/yr



};