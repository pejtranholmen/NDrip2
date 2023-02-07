#pragma once
#include "../../../std.h"
#include "../../../NewBase/PhysFunc.h"
#include "../../Atmosphere/Functors/SoilEvaporation_Functors.h"

enum class Roughness_Sw {PARAMETERS,DRIVING_VARIABLE, F_CANOPY };
enum class Roughnessfunc_Sw {SHAW_PERRERIA, LINEAR};
enum class MultiRoughness_Sw { NO, INDIVIDUAL};
enum class Displacement_Sw { PARAMETERS, DRIVING_VARIABLE, F_CANOPY };
enum class CAtmDependence_SW{ INDEPENDENT, SIMPLE_ATM_RESPONSE, LEAF_SURFACE_RESPONSE };
enum class RSMethod_Sw{PARAMETER, DRIVING_VARIABLE, LOHAMMAR, LOHAMMAR_T_DAYNUM};
enum class ResAirStabCorr_Sw{ NO_CORRECTION, RICHARDSON,MONIN_OBUKHOV};




class SimplePotTranspiration {
public: SimplePotTranspiration(double EPMaxDay, double EPPeriod, double EPMaxRate): a(EPMaxDay), b(EPPeriod), c(EPMaxRate) {}
	double operator() (double daynum) {
			double Diff;
			if (a + 0.5*b - 365 > daynum)
				Diff = (daynum + 365. - a + 0.5*b) / b;
			else if (a - 0.5*b + 365<daynum)
				Diff = (daynum - 365. - a + 0.5*b) /b;
			else
				Diff = (daynum - a + 0.5*b) / b;

			if (Diff<0 || Diff>1)
				return 0.1E-5;
			else
				return max(c*sin(Diff*Fix::PI), 0.1E-5);
	}
private:
	double a, b,c;
};
class Conductance_Light {
public:
	Conductance_Light(vector<double> HalfSaturationLightRate): Saturation_50_V(HalfSaturationLightRate) {}
	Conductance_Light(double Saturation_50 ): Saturation_50(Saturation_50) {}
	double operator() (size_t i, double LightRate) {
		return LightRate / (LightRate + Saturation_50_V[i]);
	};
	double operator() (double LightRate) {
		return LightRate / (LightRate + Saturation_50);
	}
	private:
		vector<double> Saturation_50_V;
		double Saturation_50;
};
class Conductance_VPD {
public:
	Conductance_VPD(vector<double> VPD_50_V): VPD_50_v(VPD_50_V) {}
	Conductance_VPD(double VPD_50): VPD_50(VPD_50) {}
	double operator() (size_t i, double VPD) {
		return 1 / (1 + VPD / VPD_50_v[i]);
	}
	double operator() (double VPD) {
		return 1 / (1 + VPD / VPD_50);
	}
private:
	vector<double> VPD_50_v;
	double VPD_50;

};

class Conductance {
public: Conductance(vector<double> HalfSaturationLightRate, vector<double> VPD_50_V, vector<double> Cond_Max_V,double CondMaxWinter ):
	Cond_Max_V(Cond_Max_V), Cond_Max_Winter(CondMaxWinter) {
	f_Light_V = Conductance_Light(HalfSaturationLightRate); f_vpd_V = Conductance_VPD(VPD_50_V);
}
	 Conductance(double Saturation_50, double VPD_50, double CondMax, double Cond_Max_Winter) : CondMax(CondMax), Cond_Max_Winter(Cond_Max_Winter) {
	f_Light = Conductance_Light(Saturation_50); f_vpd = Conductance_VPD(VPD_50); }
	 
	 Conductance(double CondMax, double Cond_Max_Winter) : CondMax(CondMax), Cond_Max_Winter(Cond_Max_Winter) {}
	 Conductance(double Cond_Max_Winter, vector<double> Cond_Max_V) : Cond_Max_Winter(Cond_Max_Winter), Cond_Max_V(Cond_Max_V) {}

		double operator() (bool Winter, double LightRate, double vpd) {
			if (Winter)
				return Cond_Max_Winter*f_Light(LightRate)*f_vpd(vpd);
			else
				return CondMax*f_Light(LightRate)*f_vpd(vpd);
		}
		double operator() (bool Winter, size_t index, double LightRate, double vpd) {
			if (Winter)
				return Cond_Max_Winter*f_Light_V(index, LightRate)*f_vpd_V(index, vpd);
			else
				return Cond_Max_V[index]*f_Light_V(index, LightRate)*f_vpd_V(index, vpd);
		}
		double operator() (bool Winter) {
			if (Winter)
				return Cond_Max_Winter;
			else
				return CondMax;
		}
		double operator() (bool Winter, size_t index) {
			if (Winter)
				return Cond_Max_Winter;
			else
				return Cond_Max_V[index];
		}
private:
	function<double(double)> f_Light, f_vpd;
	function<double(size_t, double)> f_Light_V, f_vpd_V;
	vector<double> Cond_Max_V;
	double CondMax;
	double Cond_Max_Winter;
};
class Conductance_CO2 {
public: Conductance_CO2(double CO2_BaseLevel, double CO2_PowerCoef) : Base(CO2_BaseLevel), Coef(CO2_PowerCoef) {}
	double operator() (double Co2Conc) {
		return pow(Base / Co2Conc, Coef);
	}
private:
	double Base, Coef;
};
class CanopyResistance {
public:
	CanopyResistance(){}
	double operator() (double Conductance, double LAI, double CO2Factor) {
		return 1 / max(Conductance*LAI, 0.001);
	}
	double operator() (double Conductance, double LAI) {
		return 1 / max(Conductance*LAI, 0.001);
	}

};







class RoughnessLengthCanopy {
public:
	RoughnessLengthCanopy(double CanDensMax, double PAddIndex, double RoughLMin ):
		DensMax(CanDensMax), AddLAI(PAddIndex), RoughLMin(RoughLMin) {}
	RoughnessLengthCanopy(vector<double> MC_CanDensMax, vector<double> MC_PAddIndex, vector<double> MC_RoughLMin,
		vector<double> MC_RoughLMax) :
		DensMax_V(MC_CanDensMax), AddLAI_V(MC_PAddIndex), RoughL_Min_V(MC_RoughLMin), RoughL_Max_V(MC_RoughLMax) {}
	RoughnessLengthCanopy(struct PlantInterp L, vector<double> MC_RoughLMin,
		vector<double> MC_RoughLMax) :
		RoughL_Min_V(MC_RoughLMin), RoughL_Max_V(MC_RoughLMax) {
		f_PlantSeasonFracRough = PlantSeasonDev(L);
	}


	double operator() (double LAI, double Height) {
		return min(100., max(Height*FractionOfHeight(DensMax, LAI, AddLAI), RoughLMin));
	}
	double operator() (size_t index, double LAI, double Height) {
		return min(RoughL_Max_V[index], max(Height*FractionOfHeight(DensMax_V[index], LAI, AddLAI_V[index]), RoughL_Min_V[index]));
	}
	double operator() (size_t index, double dum, size_t DayOfYear, double Height) {
		return min(RoughL_Max_V[index], max(Height*FractionOfHeight(index, DayOfYear), RoughL_Max_V[index]));
	}
private:
	double FractionOfHeight(double DensMax, double LAI, double AddLAI) {
		double EQ1 = (0.175 - 0.098*DensMax) + (-0.098 + 0.045*DensMax)*log10(LAI + AddLAI);
		double EQ2 = (0.150 - 0.025*DensMax) + (0.122 - 0.0135*DensMax)*log10(LAI + AddLAI);
		return min(EQ1, EQ2);
	};
	double FractionOfHeight(size_t index, size_t DayOfYear) {
		return f_PlantSeasonFracRough(index, double(DayOfYear));

	};
	double DensMax, AddLAI, RoughLMin;
	vector<double> DensMax_V, AddLAI_V, RoughL_Min_V, RoughL_Max_V;
	vector<double> Z_Start, Z_Opt, Z_End;
	function<double(size_t, double)> f_PlantSeasonFracRough;

};

class DisplacementLength {
public:
	DisplacementLength(double CanDensMax, double PAddIndex): DensMax(CanDensMax), AddLAI(PAddIndex) {}
	DisplacementLength(struct PlantInterp L)  {
		f_PlantSeasonDiplacement = PlantSeasonDev(L);
	}
	double operator() (double LAI, double RefHeight, double CanopyHeight) {
		return max(0., min(RefHeight-0.5, FractionOfHeight(DensMax, LAI, AddLAI)*CanopyHeight));
	}
	double operator() (size_t index, double Height, double DayNum) {
		return f_PlantSeasonDiplacement(index, DayNum)*Height;
	}

private:
	double FractionOfHeight(double DensMax, double LAI, double AddLAI) {
		return 0.803 + 0.108*DensMax - (0.462 - 0.086*DensMax)*
			exp(-(0.163 + 0.283*DensMax)*(LAI + AddLAI));
	};
	double DensMax, AddLAI;
	function<double(size_t, double)> f_PlantSeasonDiplacement;
};
class ResAirCanopyNeutral {
public:
	ResAirCanopyNeutral(double MinExchangeCoef, double kb) : MinExchangeCoef(MinExchangeCoef)
	{
		f_RoughLHeat = RoughnessHeatLengthFunction(kb);
	}
	double operator() (double WindSpeed, double Height, double Zom) {
		WindSpeed = max(WindSpeed, 0.1);
		return min(log(Height / Zom)*log(Height / f_RoughLHeat(Zom)) / (WindSpeed*Fix::KARMAN*Fix::KARMAN), 1 / MinExchangeCoef);
	}

private:
	double MinExchangeCoef;
	function<double(double)> f_RoughLHeat;
};
class ResAirCanopy_Numerical {
public:
	ResAirCanopy_Numerical(double WindlessExchangeCanopy, double kb,  double a, double b, double c, double d) :
		WindlessExchange(WindlessExchangeCanopy) {
		f_zoheat = RoughnessHeatLengthFunction(kb);
		f_StabilityCorr = StabilityCorrAnalytical(a, b, c, d);
		f_ResAirMom = Physf(FUNC_CHOICE::RES_AIR_MOM_HEAT);
		f_FrictionVelocity = FrictionVelocity();
		f_RichardsonToEta = RichardsonToEta(kb);
		f_psi_h = Psi_h();

	}
	double operator() (double DiffHeight, double DiffTemp, double TempAir, double WindSpeed, double zom) {
		double Ch;
		//! 2 Get Eta=Monin-Obukhov stabilityparameter from Richardson number and old Eta
		double		Eta = f_RichardsonToEta(DiffHeight, DiffTemp, TempAir, WindSpeed, zom);

		// ! ESTIMATE FRICTION VELOCITY (I.E. MOMENTUM TRANSFER)
		double Ustar = f_FrictionVelocity(Eta, WindSpeed, DiffHeight, zom);
		//! ESTIMATE TRANSFER COEFFICIENT FOR HEAT
		Ch = WindlessExchange + Ustar*Fix::KARMAN / (log(DiffHeight / f_zoheat(zom)) - f_psi_h(Eta) + f_psi_h(Eta*f_zoheat(zom) / DiffHeight));
		//! ESTIMATE AERODYNAMIC RESISTANCE

		return 1. / Ch;

	}

private:
	double WindlessExchange, zo_Mom;
	function<double(double)> f_zoheat;
	function<double(double, double, double, double, double)> f_RichardsonToEta;
	function<double(double, double, double, double)> f_StabilityCorr, f_ResAirMom,  f_WindProfile;
	function<double(double, double, double, double)>  f_FrictionVelocity;
	function<double(double)> f_psi_h;

};
class ResAirCanopy_Analytical {
public:
	ResAirCanopy_Analytical(double WindlessExchangeCanopy, double kb, double a, double b, double c, double d) :
		WindlessExchange(WindlessExchangeCanopy) {
		f_zoheat = RoughnessHeatLengthFunction(kb);
		f_StabilityCorr = StabilityCorrAnalytical(a, b, c, d);
		f_ResAirMom = Physf(FUNC_CHOICE::RES_AIR_MOM_HEAT);
		f_FrictionVelocity = FrictionVelocity();
		f_RichardsonToEta = RichardsonToEta(kb);
		f_psi_h = Psi_h();

	}
	double operator() (double DiffHeight, double DiffTemp, double TempAir, double WindSpeed, double zom) {
		double Ch;
		Ch = WindlessExchange + 1 / (f_ResAirMom(WindSpeed, DiffHeight, zom, f_zoheat(zom) )*
			f_StabilityCorr(DiffHeight, DiffTemp, TempAir, WindSpeed));
		return 1. / Ch;
	}

private:
	double WindlessExchange, zo_Mom;
	function<double(double)> f_zoheat;
	function<double(double, double, double, double, double)> f_RichardsonToEta;
	function<double(double, double, double, double)> f_StabilityCorr, f_ResAirMom, f_WindProfile;
	function<double(double, double, double, double)>  f_FrictionVelocity;
	function<double(double)> f_psi_h;

};


