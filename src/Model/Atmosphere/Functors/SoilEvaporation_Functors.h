#pragma once
#include "../../../std.h"
#include "../../../NewBase/PhysFunc.h"
enum class SurEbal_Sw { AirTemperature, PM_equation, E_balance_Solution };
enum class StabilitySoil_Sw {Richardson_Number,Monin_Obukhov_Length};
enum class SoilRoughness_Sw { IndividualR,CommonR};
enum class SoilEvap_Sw { NOT_ESTIMATED, PM_EQ_1PAR, PM_EQ_3PAR, ITERATIVE_EBAL, K_FUNC };
enum class SoilPartitioningArea_Sw { NO,Dripp_Irrig,Drip_Irrig_and_Radiation };
enum class SurfaceCover_Sw {NO, DYN_PLASTIC_SHEET, PERMANENT_PLASTIC_SHEET};


class StabilityCorrAnalytical {
public:
	StabilityCorrAnalytical(double _a, double _b, double c, double d) : a(_a), b(_b), c(c), d(d) {
		f_Rich = RichardsonsNumber;
	}
	double operator() (double DiffHeight, double DiffTemp, double TempAir, double WindSpeed) { 
		double RICH = f_Rich(DiffHeight,DiffTemp,TempAir,WindSpeed);
		if (RICH>0)
			return pow((1 + a*RICH), b);	//! Stable conditions
		else
			return pow((1 - c*RICH), -d);
	}
private:
	function<double(double, double, double,double)> f_Rich;
	double a, b, c, d;
};
class Psi_m {
public: 
	Psi_m(){
		a = 1.; b = 2. / 3.; c = 5.; d = 0.35;
	}
	double operator() (double xi) {
		if (xi <= 0) {
			double x = pow(1. - 19.*xi, 0.25);   //    ! 19 from Högström, 1996
			return log(pow((1. + x), 2)*(1. + x*x) / 8.) - 2 * atan(x) + Fix::PI / 2.;
		}
		else {
			//!stable case from Holstlag and Bruin, following Beljaars & Holtslag 1991 * /
			return -(a*xi + b*(xi - c / d)*exp(-d*xi) + b*c / d);
		}
	}
private:
	double a,b,c,d;
};
class Psi_h {
public:
	Psi_h() {
		a = 1.; b = 2. / 3.; c = 5.; d = 0.35;
	}
	double operator() (double xi) {
		if (xi <= 0) {
			double x = pow((1. - 11.6*xi), 0.25);  //   ! 11.6 from Högström, 1996
			return 2.*log((1. + x*x) / 2.);
		}
		else {
			//!Stable case, func = 1, equation from Holtslag and De Bruin following Beljaars & Holstlag, 1991 * /
			return -(pow(1. + 2. / 3.*a*xi, 3. / 2.) + b*(xi - c / d)*exp(-d*xi) + b*c / d - 1.);
		}
	}
private:
	double a, b, c, d;
};
class RoughnessHeatLengthFunction {
public:
	RoughnessHeatLengthFunction(double kb, double zom) : kb(kb), zom(zom) {}
	RoughnessHeatLengthFunction(double kb) : kb(kb) {}
	double operator() () {
		return max(0.0001, zom / exp(kb));
	}
	double operator() (double zom) {
		return max(0.0001, zom / exp(kb));
	}
private:
	double kb, zom;
};
class RichardsonToEta {
public:
	RichardsonToEta(double zo_m, double zo_h) : zom(zo_m), zoh(zo_h) { max_itt = 10; f_Rich = RichardsonsNumber;
		f_psi_m = Psi_m(); f_psi_h = Psi_h(); eta_old = 10.;
	}
	RichardsonToEta(double kb)   {
		max_itt = 10; f_Rich = RichardsonsNumber;
		f_psi_m = Psi_m(); f_psi_h = Psi_h(); eta_old = 10.;
		f_RoughLHeat = RoughnessHeatLengthFunction(kb);
	}
	double operator() (double DiffHeight, double DiffTemp, double TempAir, double WindSpeed) {
		double Ri = f_Rich(DiffHeight, DiffTemp, TempAir, WindSpeed);
		int itt = 1;
		double acc = 0.0001;
		double RatioM = DiffHeight / zom;    if (RatioM <= 1) RatioM = 10.;
		double RatioH = DiffHeight / zoh;    if (RatioH <= 1) RatioH = 10.;

		//! STEP 1: Calculate function Ri2Xi(XiOld)
		double Ri2eta = pow((log(RatioM) - f_psi_m(eta_old) + f_psi_m(eta_old / RatioM)), 2) /
			(log(RatioH) - f_psi_h(eta_old) + f_psi_h(eta_old / RatioH));
		//! STEP 2: Calculate error in terms of Ri using etaOld and Ri2eta(etaOld)
		double Error = eta_old / Ri2eta - Ri;
		//! STEP 3: solve iteratively
		while (abs(Error)>acc&&itt<max_itt) {
			//! 3.1 new Xi
			auto value = Ri2eta*Ri;
			Ri2eta = pow((log(RatioM) - f_psi_m(value) +f_psi_m(value / RatioM)), 2) /
				(log(RatioH) - f_psi_h(value) + f_psi_h(value / RatioH));
			//! 3.2 error in terms of Richardson number
			Error = value / Ri2eta - Ri;
			//! 3.3 update number of iterations
			itt++;
		}
		//! STEP 4: Return new eta when convergance criteria is fullfilled
		return Ri2eta*Ri;
	};
	double operator() (double DiffHeight, double DiffTemp, double TempAir, double WindSpeed, double zom) {
		double Ri = f_Rich(DiffHeight, DiffTemp, TempAir, WindSpeed);
		int itt = 1;
		double acc = 0.0001;
		double RatioM = DiffHeight / zom;    if (RatioM <= 1) RatioM = 10.;
		double RatioH = DiffHeight / f_RoughLHeat(zom);    if (RatioH <= 1) RatioH = 10.;

		//! STEP 1: Calculate function Ri2Xi(XiOld)
		double Ri2eta = pow((log(RatioM) - f_psi_m(eta_old) + f_psi_m(eta_old / RatioM)), 2) /
			(log(RatioH) - f_psi_h(eta_old) + f_psi_h(eta_old / RatioH));
		//! STEP 2: Calculate error in terms of Ri using etaOld and Ri2eta(etaOld)
		double Error = eta_old / Ri2eta - Ri;
		//! STEP 3: solve iteratively
		while (abs(Error)>acc&&itt<max_itt) {
			//! 3.1 new Xi
			auto value = Ri2eta*Ri;
			Ri2eta = pow((log(RatioM) - f_psi_m(value) + f_psi_m(value / RatioM)), 2) /
				(log(RatioH) - f_psi_h(value) + f_psi_h(value / RatioH));
			//! 3.2 error in terms of Richardson number
			Error = value / Ri2eta - Ri;
			//! 3.3 update number of iterations
			itt++;
		}
		//! STEP 4: Return new eta when convergance criteria is fullfilled
		return Ri2eta*Ri;
	};
private:
	double zom, zoh, eta_old;
	size_t max_itt;
	function<double(double, double, double, double)> f_Rich;
	function<double(double)> f_psi_m, f_psi_h, f_RoughLHeat;;

};
class FrictionVelocity {
public:
	FrictionVelocity(double zom) : zom(zom) { f_psi_m = Psi_m(); }
	FrictionVelocity() { f_psi_m = Psi_m(); }

	double operator()(double eta, double Wind, double Height) {
		if (zom <= 0) return .01;
		double Ratio = Height / zom;
		if (Ratio <= 2) Ratio = 2;
		return Wind*Fix::KARMAN / (log(Ratio) - f_psi_m(eta) + f_psi_m(eta / Ratio));
	}
	double operator()(double eta, double Wind, double Height, double zom) {
		if (zom <= 0) return .01;
		double Ratio = Height / zom;
		if (Ratio <= 2) Ratio = 2;
		return Wind*Fix::KARMAN / (log(Ratio) - f_psi_m(eta) + f_psi_m(eta / Ratio));
	}
private:
		double zom;
		function<double(double)> f_psi_m;
};
class ResAirHeatFunctionGeneral {
public:
	ResAirHeatFunctionGeneral(double MinExchangeCoef,  double zom ) : MinExchangeCoef(MinExchangeCoef), Zom(zom) {
		f_psi_h = Psi_h();
	}
	double operator() (double Ustar, double DiffHeight, double Eta) {
		double Ratio = DiffHeight / Zom;
		if (Ratio <= 1) Ratio = 10.;
		return 1 / (MinExchangeCoef + Ustar*Fix::KARMAN / (log(Ratio) - f_psi_h(Eta) + f_psi_h(Eta / Ratio)));
	}
private:
	double MinExchangeCoef, Zom ;
	function<double(double)>  f_psi_h;
};
class ResAirSoilNeutral {
public:
	ResAirSoilNeutral(double MinExchangeCoef,  double kb, double zom) : MinExchangeCoef(MinExchangeCoef), Zom(zom)
	{
		f_RoughLHeat = RoughnessHeatLengthFunction(kb, zom);
	}
	double operator() (double WindSpeed, double Height) {
		WindSpeed = max(WindSpeed, 0.1);
		return  max(0.001, min(log(Height / Zom) * log(Height / f_RoughLHeat()) / (WindSpeed * Fix::KARMAN * Fix::KARMAN), 1 / MinExchangeCoef));
	}

private:
	double MinExchangeCoef, Zom;
	function<double()> f_RoughLHeat;
};
class WindProfile {
public: WindProfile(double zom)   : zom(zom) { f_psi_m = Psi_m(); }
		double operator()(double UU, double zU, double zT, double eta) {
			//! input Windspeed, zUheight Wind,height temp,M-O stabilityparameter=zT/L,roughness momentum 
			/*real UU,zU,zT,eta,zom, RatioU, RatioT
			integer psifunc
			!Local FrictionVelocity/Karman
			real UstarKarman
			! Get Friction velocity over Karman, Ustar/Karman*/

			double RatioU = zU / zom;
			double RatioT = zT / zom;
			if (RatioU <= 1 || RatioT <= 1)
				return UU;
			double UstarKarman = UU / (log(zU / zom) - f_psi_m(eta*zU / zT) + f_psi_m(eta*zom / zT));
			//! Windspeed at height of temperature measurements, zT
			return UstarKarman*(log(zT / zom) - f_psi_m(eta) + f_psi_m(eta*zom / zT));
		}
private:
	double zom;
	function<double(double)> f_psi_m;
};
class RaBoundaryLAIFunction {
public:
	RaBoundaryLAIFunction(double RaLAI) : RaLAI(RaLAI) {}
	double operator() (double LAIv) {
		return RaLAI*LAIv;
	}
private:
	double RaLAI;
};
class SurfaceResistanceSoilSurface_3Par {
public:
	SurfaceResistanceSoilSurface_3Par(double PsiRs_3pf1, double PsiRs_3pf2, double PsiRs_3pf3)
		: a(PsiRs_3pf1), b(PsiRs_3pf2), c(PsiRs_3pf3) {}
	double operator() (double Psiv, double Surfmos) {
			return max(0., a*max(0., Psiv - b) - c*Surfmos);
	}
private:
	double a, b, c;
};
class SurfaceResistanceSoilSurface_1Par {
public:
	SurfaceResistanceSoilSurface_1Par( double PsiRs_1p)
		: a(PsiRs_1p){}
	double operator() (double Psiv, double Surfmos) {
		return a*(log10(max(100., Psiv)) - 1 - Surfmos); 
	}
private:
	double a;
};
class ECorrSoilSurface {
public:
	ECorrSoilSurface(double EGPsi) : a(EGPsi) {}
	double operator() (double SurfMoisture) {
		return pow(10., -SurfMoisture*a);
	}
private:
	double a;
};
class RelHumiditySoilSurface {
public:
	RelHumiditySoilSurface(double EGPsi) { f_ECorr = ECorrSoilSurface(EGPsi); f_RelHumPsi = Physf(FUNC_CHOICE::RH_PSI); }
	double operator() (double PsiSoil, double Temp, double SurfMostuire) {
		return f_RelHumPsi(PsiSoil, Temp, f_ECorr(SurfMostuire));
	}
private:
	function<double(double)> f_ECorr;
	function<double(double, double, double)> f_RelHumPsi;
};
class RaHeatFunctionSoil_Numerical {
public:
	RaHeatFunctionSoil_Numerical (double RaLAI, double WindlessExchangeSoil,  double kb, double zom, double a, double b, double c, double d) :
		WindlessExchangeSoil(WindlessExchangeSoil), zo_Mom(zom) {
		f_zoheat = RoughnessHeatLengthFunction(kb, zom);
		f_Rabound = RaBoundaryLAIFunction(RaLAI);
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
	double operator() (double LAIv) {
		return f_Rabound(LAIv);
	}
	double operator() ( double DiffHeight, double DiffTemp, double TempAir, double WindSpeed ) {
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
	double operator() ( double DiffHeight, double DiffTemp, double TempAir, double WindSpeed, double LAIv) {
		double Ch;
		//! 2 Get Eta=Monin-Obukhov stabilityparameter from Richardson number and old Eta
		double		EtaSoil = f_RichardsonToEta(DiffHeight, DiffTemp, TempAir, WindSpeed);

		// ! ESTIMATE FRICTION VELOCITY (I.E. MOMENTUM TRANSFER)
		double Ustar = f_FrictionVelocity(EtaSoil, WindSpeed, DiffHeight);
		//! ESTIMATE TRANSFER COEFFICIENT FOR HEAT
		Ch = WindlessExchangeSoil + Ustar*Fix::KARMAN / (log(DiffHeight / f_zoheat()) - f_psi_h(EtaSoil) + f_psi_h(EtaSoil*f_zoheat() / DiffHeight));
		//! ESTIMATE AERODYNAMIC RESISTANCE
		return 1. / Ch+ f_Rabound(LAIv);

	}

private:
	double WindlessExchangeSoil, zo_Mom;
	function<double()> f_zoheat;
	function<double(double)> f_Rabound;
	function<double(double, double, double, double)> f_StabilityCorr, f_ResAirMom, f_RichardsonToEta,f_WindProfile;
	function<double(double, double, double)>  f_FrictionVelocity;
	function<double(double)> f_psi_h;

};
class RaHeatFunctionSoil_Analytical {
public:
	RaHeatFunctionSoil_Analytical( double RaLAI, double WindlessExchangeSoil, double kb, double zom, double a, double b, double c, double d) :
		WindlessExchangeSoil(WindlessExchangeSoil), zo_Mom(zom) {
		f_zoheat = RoughnessHeatLengthFunction(kb, zom);
		f_Rabound = RaBoundaryLAIFunction(RaLAI);
		f_StabilityCorr = StabilityCorrAnalytical(a, b, c, d);
		f_ResAirMom = ResAirSoilNeutral(WindlessExchangeSoil, kb, zom);
			//Physf(FUNC_CHOICE::RES_AIR_MOM_HEAT);
	}
	double operator() (double LAIv) {
		return f_Rabound(LAIv);
	}
	double operator() (double DiffHeight, double DiffTemp, double TempAir, double WindSpeed) {
		double Ch;
		Ch = WindlessExchangeSoil + 1 / (f_ResAirMom(WindSpeed, DiffHeight)*
				f_StabilityCorr(DiffHeight, DiffTemp, TempAir, WindSpeed));
		return 1. / Ch;

	}
	double operator() (double DiffHeight, double DiffTemp, double TempAir, double WindSpeed, double LAIv) {
		double Ch;
		Ch = WindlessExchangeSoil + 1 / (f_ResAirMom(WindSpeed, DiffHeight)*
			f_StabilityCorr(DiffHeight, DiffTemp, TempAir, WindSpeed));
		return 1. / Ch + f_Rabound(LAIv);

	}

private:
	double WindlessExchangeSoil, zo_Mom;
	function<double()> f_zoheat;
	function<double(double)> f_Rabound;
	function<double(double, double, double, double)> f_StabilityCorr;
	function<double(double, double)> f_ResAirMom;
};
class TempDifferenceFromSensibleFlow {
	public:
	TempDifferenceFromSensibleFlow() {}
	double operator() (double Sens_AirF, double Ra) {

		if (Sens_AirF > -1.E8&&Sens_AirF < 1.E8)
			return max(-5., min(5., Sens_AirF*Ra / (Fix::DENSA*Fix::SPECA*Fix::SECPERDAY)));
		else
			return 0.;
	}
private:


};
