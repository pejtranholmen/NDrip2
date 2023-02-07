#pragma once
#include "../../../std.h"
#include "../../../NewBase/PhysFunc.h"
enum class RadiationInput_Sw {No, Yes};
enum class NetLongRad_Sw{One_Forumula, Two_Formulas};
enum class LongRadCanopy_Sw{Implicit, Explicit_Temp_Canopy};
enum class InLongRad_Sw { Konzelmann, Satterlund, Brunts };
enum class Turbidity_Sw{ Constant, Function_solar_angle};
enum class SplitGlobal_Sw{No, Yes};
enum class CanopyShade_Sw{ Constant, Function_solar_angle };




class AtmTransparency_FromRelSun {    //Angströns equation
public:
	AtmTransparency_FromRelSun(double ang1, double ang2) : a(ang1), b(ang2) { }
	double operator() (double x) {
		return a+x*b;
	}
private:
	double a,b;

};


class AlbedoSoil {
public:
	AlbedoSoil(double AlbedoDry, double AlbedoKExp, double AlbedoWet) :a(AlbedoDry), b(AlbedoKExp), c(AlbedoWet) { }
	double operator() (double PsiSoil) {
		double pfu;
		if (PsiSoil>10)
			pfu = log10(PsiSoil) - 1.;
		else
			pfu = 0;
		return a + exp(-b*pfu)*(c - a);
	}
private:
	double a, b, c;
};
class AlbedoSnowAgeTempSum {
public:
	AlbedoSnowAgeTempSum(double AlbSnowCoef_1, double AlbSnowCoef_2, double AlbSnowCoef_3, double AlbSnowMin) :a(AlbSnowCoef_1), b(AlbSnowCoef_2), c(AlbSnowCoef_3), d(AlbSnowMin) {
		Tsum_albedo = 0;
	}
	double operator() (double SnowAge, double Tsum_albedo) {
	
		return min(100., a*exp(b*SnowAge + c*Tsum_albedo) + d);
	}
private:
	double a, b, c, d;
	double Tsum_albedo;
};


class AlbedoSnow_Tair {
public:
	AlbedoSnow_Tair(double AlbSnowCoef_1, double AlbSnowCoef_2, double AlbSnowCoef_3, double AlbSnowMin, double tstep) :a(AlbSnowCoef_1), b(AlbSnowCoef_2), c(AlbSnowCoef_3), d(AlbSnowMin), T_Step(tstep) {
		Tsum_albedo = 0;
	}
	double operator()  (double SnowAge, double Tair ) {
		if (SnowAge<1)
			Tsum_albedo = 0.;
		else if (Tair>0.)
			Tsum_albedo += Tair*T_Step;
		return min(100., a*exp(b*SnowAge + c*Tsum_albedo) + d);
	}
private:
	double a, b, c, d, T_Step;
	double Tsum_albedo;
};
class BeerLaw {
public:
	BeerLaw(double k) : _k(k) {}
	BeerLaw(vector<double> kv) : _kv(kv) {}
	double operator() (double LAI) { // Remaining at surface if Single Leaf
		return exp(-_k*LAI);
	}
	double operator() (size_t index, double LAI) { // Adsorbed per specific Leaf
		return 1. - exp(-_kv[index] * LAI);
	}
private:
	double _k;
	vector<double> _kv;
};
class RadLongOut_Em {
public:
	RadLongOut_Em(double Emissivity):_Emissivity(Emissivity){}
	double operator() (double TempSurf) {
		return _Emissivity * 86400. * Fix::STEFB*pow(TempSurf + 273.15, 4);
	}
private:
	double _Emissivity;
};
class RadNetLong {
public:
	RadNetLong(double _a, double _b, double c, double d) : a(_a), b(_b), c(c), d(d) {}
	double operator() (double Tair, double relsun) { // One single air temp
		double actv = ModelFunctions::SVP(Tair);
		return Fix::SECPERDAY*Fix::STEFB*pow(Tair + 273.15, 4)*(a - b*sqrt(actv))*
			(c + d*relsun);
	}
	double operator() (double Tair, double actv, double relsun) { // One single air temp
		return Fix::SECPERDAY*Fix::STEFB*pow(Tair + 273.15, 4)*(a - b*sqrt(actv))*
			(c + d*relsun);
	}
	double operator() (double Tair, double actv, double relsun, double TSurf) { // Two temp, air and surface
		return Fix::SECPERDAY*Fix::STEFB*pow(Tair + 273.15, 4)*(a - b*sqrt(actv))*
			(c + d*relsun);
	}

private:
	double a, b, c, d;
};
class RadLongIn {
public:
	RadLongIn(size_t choice, bool EmOnly, double a, double b) : Option(LONGF(choice)), EmissivityOnly(EmOnly), a(a), b(b) {}
	RadLongIn(size_t choice, bool EmOnly, double a, double b, double c) : Option(LONGF(choice)), EmissivityOnly(EmOnly), a(a), b(b), c(c) {}
	double operator() (double Tair,  double relsun, double actv) {
		double ECoef = 0.;
		switch (Option) {
		case LONGF::KONZELMAN:
			ECoef = ((a + b*pow(actv / (Tair + 273.15), 0.125))*(1 - pow(1 - relsun, 3)) + c*pow(1 - relsun, 3));
			break;
		case LONGF::SATTERLUND:
			ECoef = (1 - exp(-pow(actv / 100., (Tair + 273.15) / a)))*(1 + b*pow(1 - relsun, 2));
			break;
		case LONGF::BRUNT:
			ECoef = (a + b*sqrt(actv / 100.))*(1 + pow(1 - relsun, 2)*c);
			break;
		}
		if (EmissivityOnly) return ECoef;
		return Fix::SECPERDAY*Fix::STEFB*pow(Tair + 273.15, 4)*ECoef;
	}
	double operator() (double Tair, double relsun) {
		double ECoef = 0.;
		double actv = ModelFunctions::SVP(Tair);
		switch (Option) {
		case LONGF::KONZELMAN:
			ECoef = ((a + b*pow(actv / (Tair + 273.15), 0.125))*(1 - pow(1 - relsun, 3)) + c*pow(1 - relsun, 3));
			break;
		case LONGF::SATTERLUND:
			ECoef = (1 - exp(-pow(actv / 100., (Tair + 273.15) / a)))*(1 + b*pow(1 - relsun, 2));
			break;
		case LONGF::BRUNT:
			ECoef = (a + b*sqrt(actv / 100.))*(1 + pow(1 - relsun, 2)*c);
			break;
		}
		if (EmissivityOnly) return ECoef;
		return Fix::SECPERDAY*Fix::STEFB*pow(Tair + 273.15, 4)*ECoef;
	}

private:
	enum class LONGF { KONZELMAN, SATTERLUND, BRUNT };
	double a, b, c;
	enum LONGF Option;
	bool EmissivityOnly;
};
class DiffuseRadRatio {
public:
	DiffuseRadRatio(double DiffuseRadCoef) : Coef(DiffuseRadCoef) {}
	double operator() ( double Cloudiness, double ThetaAngle) {
		if (ThetaAngle<Fix::PI / 2.)
			return Coef + Coef*Cloudiness + (1. - 2.*Coef)*pow(Cloudiness, 2);
		else
			return 1.;
	}
private:
	double Coef;
};
class Angstrom {
public:
	Angstrom(double RadFracAng1, double RadFracAng2) : a(RadFracAng1), b(RadFracAng2) {}
	double operator() (double relsun) {
		return a + b*relsun;
	}
private:
	double a, b;
};
class CanopyShadeCover {  //THIS is a new VERSION with 2 parameter, using height and Solar angle input
						// note that previous version was not showing any sensivity for second parameters. 
						// The output from the function is a factor that enhance the shadow and extend the canopy cover.
						// Original version from David G new version created by PEJ December 29, 2020
						// New parameter CanopyShade_Base
public:
	CanopyShadeCover( double CanopyShadeCoef, double CanopyShadeBase) : a(CanopyShadeCoef), b(CanopyShadeBase) {}
	double operator() (double ThetaAngle, double height) {
		double elev = Fix::PI / 2. - ThetaAngle;
		if (ThetaAngle < Fix::PI / 2.)
			return min(10., max(1., (a * height + b) / tan(elev)));
		else
			return 1.0;
	} 
private:
	double a, b;
};

class RadGlobalTempAmplitude_SeasonalB {
public:
	RadGlobalTempAmplitude_SeasonalB(RadTempAmpPar par) : p(par) { f_sin = Physf(FUNC_CHOICE::AIRTEMP_YEAR); }
	double operator() (double TempDiff, double DayNo) {
		p.Bristow_B = Thornton_B(DailyTempRangeFunc(double(DayNo)));
		return p.Bristow_Turb*(1. - exp(-p.Bristow_B*pow(TempDiff, p.Bristow_C)));
	}
private:
	double Thornton_B(double DailyTempDiff) { return p.Thornton_B0 + p.Thornton_B1*exp(p.Thornton_B2*DailyTempDiff); }
	double DailyTempRangeFunc(double DayNo) { return f_sin(p.DTR_YEAR_MEAN, p.DTR_YEAR_AMP, DayNo, p.DTR_YEAR_SHIFT); }
	RadTempAmpPar p;
	function<double(double, double, double, double)> f_sin;
};
class RadGlobalTempAmplitude_FixedB {
public:
	RadGlobalTempAmplitude_FixedB(RadTempAmpPar par) : p(par) { }
	double operator() (double TempDiff, double DayNo) {
		return p.Bristow_Turb*(1. - exp(-p.Bristow_B*pow(TempDiff, p.Bristow_C)));
	}
private:

	RadTempAmpPar p;
};
