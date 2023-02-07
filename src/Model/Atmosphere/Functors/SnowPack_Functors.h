#pragma once
#include "../../../std.h"
#include "../../../NewBase/PhysFunc.h"
#include "../../Structure/Functors/Structure_Functors.h"
#include "../../Atmosphere/Functors/SoilEvaporation_Functors.h"
#include "./Radiation_Functors.h"

enum class SnowSurEbal_Sw{AIR_TEMP, E_BALANCE};
enum class StabilitySnow_Sw {RICHARDSON_NUMBER, MONIN_OBUKOV};
enum class SnowMeltFunctionSwitch_Sw { Empirical, Heat_Balance };
enum class NewSnowDensityModel_Sw{LINEAR_AIRTEMP, EXPONENTIAL_AIRTEMP};
enum class SnowDensification_Sw { ICE_LIQUID_CONTENT, COMPACTION_RATE };
enum class SnowAdjustment_Sw{NO, FORCED_TO_CONTINOUS, FORCED_TO_DISCRETE};
enum class SnowRoughness_Sw{INDIVUAL_PLANT_HEIGHT, COMMON_PLANT_HEIGHT };
enum class SnowPloughing_Sw { OFF, DRIVEN_BY_SNOWMASS };
enum class SnowPloughInput_Sw{ OFF, LATERAL_INPUTFILE};
enum class ChlorideMelt_Sw{OFF, ON};

class ThermalConductivitySnow {
public: ThermalConductivitySnow(double SthCon) : SThcon(SthCon) {}

		double operator() (double Density) {
			return SThcon*pow(Density, 2);
		}
private:
	double SThcon;
};
class SaltMeltOnRoad {
public: SaltMeltOnRoad(double ChlorideMeltConc) : a(ChlorideMeltConc) {}
		double operator() (double TempAir, double SaltOnRoad) {
			if (TempAir < 0)
				return SaltOnRoad / (a*abs(TempAir));
            return 0.0;
		}
private:
	double a;
};
class SnowMeltFunctor {
public: SnowMeltFunctor(double MeltRis, double SAgeM1, double SAgeM2, double MeltTemp, double MeltReFr, double MeltFSoil) :
	MeltRis(MeltRis), SAgeM1(SAgeM1), SAgeM2(SAgeM2), MeltTemp(MeltTemp), MeltReFr(MeltReFr), MeltFSoil(MeltFSoil) {}
		double operator() ( double Temp, double Rad, double Age, double Height, double SoilHeat) {
			double MRIS = MeltRis + SAgeM1*MeltRis*(1 - exp(-SAgeM2*Age));
			if (Temp<0 && Height>0.)
				return Temp*MeltTemp / (MeltReFr*Height) + MRIS*Rad - MeltFSoil*max(-0.2, SoilHeat / Fix::LATMELT);
			else
				return MeltTemp*Temp + MRIS*Rad - MeltFSoil*SoilHeat / Fix::LATMELT;
		}
private:
	double MeltRis, SAgeM1, SAgeM2, MeltTemp, MeltReFr, MeltFSoil;

};
class NewSnowDensity_Linear {
public:NewSnowDensity_Linear(double PRainLim, double SDensNew) : SDensNew(SDensNew), PRainLim(PRainLim) { FliqMax = 0.5; }
	   double operator() (double ThQualP, double Tair) {
		   if (Tair < PRainLim) {
			   double Smax = SDensNew / (67.92 + 51.25)*(67.92 + 51.25*exp(PRainLim / 2.59)) / (1. - FliqMax);
			   return SDensNew + (Smax - SDensNew)*(1 - ThQualP) / FliqMax;
		   }
		   else
			   return 1000.;
	   }
private:
	double SDensNew, PRainLim;
	double FliqMax;
};
class NewSnowDensity_Exponential {
public:NewSnowDensity_Exponential(double PRainLim, double SDensNew) : SDensNew(SDensNew), PRainLim(PRainLim) {}
	   double operator() (double ThQualP, double Tair) {
		   if (Tair < PRainLim)
			   return SDensNew / (67.92 + 51.25)*(67.92 + 51.25*exp(Tair / 2.59)) / ThQualP;
		   else
			   return 1000.0;
	   }
private:
	double SDensNew, PRainLim;
};
class RaHeatFunctionSnow_Numerical {
public:
	RaHeatFunctionSnow_Numerical(double WindlessExchangeSnow, double kb, double zom, double a, double b, double c, double d) :
		WindlessExchangeSoil(WindlessExchangeSnow), zo_Mom(zom) {
		f_zoheat = RoughnessHeatLengthFunction(kb, zom);
		f_StabilityCorr = StabilityCorrAnalytical(a, b, c, d);
		f_ResAirMom = Physf(FUNC_CHOICE::RES_AIR_MOM_HEAT);
		f_FrictionVelocity = FrictionVelocity(zom);
		f_WindProfile = WindProfile(zom);
		f_RichardsonToEta = RichardsonToEta(zom, f_zoheat());
		f_psi_h = Psi_h();

	}
	double operator() () {
		return f_zoheat();
	}

	double operator() (double WindSpeed,double DiffHeight, double DiffTemp, double TempAir ) {
		double Ch;
		//! 2 Get Eta=Monin-Obukhov stabilityparameter from Richardson number and old Eta
		double		EtaSoil = f_RichardsonToEta(DiffHeight, DiffTemp, TempAir, WindSpeed);

		// ! ESTIMATE FRICTION VELOCITY (I.E. MOMENTUM TRANSFER)
		double Ustar = f_FrictionVelocity(EtaSoil, WindSpeed, DiffHeight);
		//! ESTIMATE TRANSFER COEFFICIENT FOR HEAT
		Ch = WindlessExchangeSoil + Ustar*Fix::KARMAN / (log(DiffHeight / f_zoheat()) - f_psi_h(EtaSoil) + f_psi_h(EtaSoil*f_zoheat() / DiffHeight));
		//! ESTIMATE AERODYNAMIC RESISTANCE

		return 1. / Ch;

	}

private:
	double WindlessExchangeSoil, zo_Mom;
	function<double()> f_zoheat;
	function<double(double, double, double, double)> f_StabilityCorr, f_ResAirMom, f_RichardsonToEta, f_WindProfile;
	function<double(double, double, double)>  f_FrictionVelocity;
	function<double(double)> f_psi_h;

};
class RaHeatFunctionSnow_Analytical {
public:
	RaHeatFunctionSnow_Analytical(double WindlessExchangeSnow, double kb, double zom, double a, double b, double c, double d) :
		WindlessExchangeSnow(WindlessExchangeSnow), zo_Mom(zom) {
		f_zoheat = RoughnessHeatLengthFunction(kb, zom);
		f_StabilityCorr = StabilityCorrAnalytical(a, b, c, d);
		f_ResAirMom = ResAirSoilNeutral(WindlessExchangeSnow, kb, zom);
		//Physf(FUNC_CHOICE::RES_AIR_MOM_HEAT);
	}

	double operator()  (double WindSpeed, double DiffHeight, double DiffTemp, double TempAir) {
		double Ch;
		Ch = WindlessExchangeSnow + 1 / (f_ResAirMom(WindSpeed, DiffHeight)*
			f_StabilityCorr(DiffHeight, DiffTemp, TempAir, WindSpeed));
		return 1. / Ch;

	}

private:
	double WindlessExchangeSnow, zo_Mom;
	function<double()> f_zoheat;
	function<double(double)> f_Rabound;
	function<double(double, double, double, double)> f_StabilityCorr;
	function<double(double, double)> f_ResAirMom;
};
class BareSoilCoverFromSnowHeight {
public:
	BareSoilCoverFromSnowHeight(double HCompleteCover) noexcept : hcomplete(HCompleteCover) {
		if (hcomplete <= 0.) hcomplete = .0001;
	}
	double operator()  (double HeightSnow) noexcept  {
		if (abs(HeightSnow) > 0)
			return  max(0., 1 - min(1., HeightSnow / hcomplete));
		else
			return 1.;
	}
private:
	double hcomplete;
};
class SimpleSnowDensity {
public:
	SimpleSnowDensity(double NewSnowDensity, double SnowDensityLiquid, double SnowDensityMass) noexcept: newsnow(NewSnowDensity), liquid(SnowDensityLiquid), mass(SnowDensityMass) {}
	double operator() ( double  TotalMass, double  liquidratio, double actualDensity) {
		return max(actualDensity, newsnow + liquidratio * liquid + TotalMass * mass);
	 }
private:
	double newsnow, liquid, mass;

};

