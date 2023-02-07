#pragma once
#include "Profile_Functors.h"
enum class HydFunc { BROOKoCOREY, GENUCH };
enum class CondFunc { MUALEM, POWER_REL_SAT, POWER_EFF_SAT };

enum class HydraulicFunc_Sw{ BROOKoCOREY, GENUCH };
enum class MatricCondFunc_Sw{ Independent, Same_as_Total_Conductivity,Function_of_total_conductivity };
enum class ConductivityFunc_Sw { Mualem,Power_relative_saturation, Power_effective_saturation };
enum class ScalingRetention_Sw { No, Yes };
enum class PedoFunction_Sw { Not_Used, Texture_parameters, Texture_database,pF_points,pF_points_fixed_residual };
enum class Replace_MissingLowCond {No, Yes};
enum class PedoType { AIRENTRY, LAMBDA, RESIDUAL, CONDUCTIVITY, WILTING };

struct RETENTION_COEF {
	double Saturation, Wilting, UpperBTheta, UpperBPsi, ResWater;
	double Galpha, GM, GN;
	double MacroPore, Lambda, PsiEntry;
	double ScalePsi;


};
struct CONDUCTIVITY_COEF {
	double NTortuosity, NSR, NSE, SatTotCond, SatMatCond;
};
struct HYDRAULIC_COEF {
	vector<RETENTION_COEF> ret;
	vector<CONDUCTIVITY_COEF> cond;
};
struct bc {
	bc (vector<double>& sat, vector<double>& macro, vector<double>& lambda, vector<double>& air, vector<double>& residual) : sat(sat), macro(macro), lambda(lambda), air(air), residual(residual) {
		for (size_t i = 0; i < sat.size(); i++) {
			psix.push_back(pow(1 - macro[i] / (sat[i] - residual[i]), -1. / lambda[i]) * air[i]);
			range.push_back(sat[i] - residual[i]);
   		}
	}
	double operator() (size_t i, double theta) {
		if (theta > sat[i] - macro[i] && sat[i] - residual[i] > macro[i]) {
			double func;


			if (psix[i] <= 100.)
				func = psix[i] - (theta + macro[i] - sat[i]) * psix[i] / macro[i];
			else {
				double psil = log10(psix[i]) - (theta + macro[i] - sat[i]) * log10(psix[i]) / macro[i];
				func = pow(10., psil);
				if (func < 1.1) func = 0;
			}
			return max(-50., func);
		}
		else {
			double se = (theta - residual[i])/range[i];
			return min(1.E6, air[i] /pow(10., log10(se) /lambda[i]));
		}
	}
	vector<double> sat, macro, psix, residual, range, lambda, air;
};


struct genuchten {
	genuchten(vector<double>&sat, vector<double>& alfa, vector<double>& n, vector<double>& m, vector<double>&residual): sat(sat), alfa(alfa), n(n), m(m), residual(residual)  {}
	double operator() (size_t i, double theta) {
		double se = max( min(0.99999, (theta - residual[i]) / (sat[i] - residual[i])), 0.0001);
		return  1./ alfa[i] * pow(pow(se, -1./m[i]) - 1., 1./n[i]);
	}
	vector<double> sat, alfa, n, m, residual;
};

class Psi_fromTheta {

public:
	Psi_fromTheta(vector<RETENTION_COEF> m, size_t hydfunc) noexcept  : m(m) {
		vector<double> sat;
		if (HydraulicFunc_Sw(hydfunc) == HydraulicFunc_Sw::GENUCH) {
			vector<double> alfa, gn, gm, residual;
			for (size_t i = 0; i < m.size(); ++i) {
				sat.push_back(m[i].Saturation);
				alfa.push_back(m[i].Galpha);
				gn.push_back(m[i].GN);
				gm.push_back(m[i].GM);
				residual.push_back(m[i].ResWater);
			}
			hyfunc = genuchten(sat, alfa, gn, gm, residual);
		}
		else {
			vector<double> macro, lambda, air, residual;
			for (size_t i = 0; i < m.size(); ++i) {
				sat.push_back(m[i].Saturation);
				lambda.push_back(m[i].Lambda);
				air.push_back(m[i].PsiEntry);
				macro.push_back(m[i].MacroPore);
				residual.push_back(m[i].ResWater);
			}
			hyfunc = bc(sat, macro, lambda, air, residual);
		}
	}

	double operator() (size_t ipar, double Theta)  noexcept {
		double sex, psix, psid, psil, se, func;

		if (Theta >= m[ipar].Saturation)
			func = max(-500., -10. * (Theta - m[ipar].Saturation));
		else if (Theta <= m[ipar].Wilting && Theta <= m[ipar].UpperBTheta && m[ipar].UpperBPsi <= 150000.)
			func = pow(10., 4.2 + 2.8 * (1. - Theta / m[ipar].Wilting));
		else if (Theta <= m[ipar].UpperBTheta || (Theta <= m[ipar].ResWater && m[ipar].UpperBPsi <= 150000.)) {
			psid = min(1., max(0.001, (m[ipar].UpperBTheta - Theta) / (m[ipar].UpperBTheta - m[ipar].Wilting)));
			func = pow(10., log10(m[ipar].UpperBPsi) + psid * log10(15000. / m[ipar].UpperBPsi));
		}
		else
			func = hyfunc(ipar, Theta);
		if (m[ipar].ScalePsi != 0.)
			return pow(10., m[ipar].ScalePsi)*func;
		else
			return func;
	}
private:
	vector<RETENTION_COEF> m;
	function<double(size_t, double)> hyfunc;


};
class Theta_fromPsi {

public:
	Theta_fromPsi(vector<RETENTION_COEF> m, size_t Hydfunc) : m(m), HydFunc(Hydfunc) {}
	double operator() (size_t ipar, double psi) {
		double psix = 0;
		double sex;
		double pss = psi / pow(10., m[ipar].ScalePsi);
		if (m[ipar].Saturation - m[ipar].ResWater>m[ipar].MacroPore && HydraulicFunc_Sw(HydFunc) == HydraulicFunc_Sw::BROOKoCOREY) {
			sex = 1 - m[ipar].MacroPore / (m[ipar].Saturation - m[ipar].ResWater);
			psix = pow(sex, -1 / m[ipar].Lambda)*m[ipar].PsiEntry;
		}
		else
			psix = 0;

		if (pss<psix&&psix <= 100 && psi>0)
			return min(m[ipar].Saturation, m[ipar].Saturation - m[ipar].MacroPore + (psix - pss) / psix*m[ipar].MacroPore);
		else if (pss>0 && pss<psix&&psix>100 && psix>0) {
			return  min(m[ipar].Saturation, m[ipar].Saturation - m[ipar].MacroPore + m[ipar].MacroPore * (log10(pss / psix) / log10(1 / psix)));
		}
		else if (pss <= m[ipar].PsiEntry)
			return m[ipar].Saturation;
		else if (pss>m[ipar].UpperBPsi) {
			return m[ipar].UpperBTheta - min(1., log10(pss / m[ipar].UpperBPsi) /
				log10(15000 / m[ipar].UpperBPsi))*(m[ipar].UpperBTheta - m[ipar].Wilting);
		}
		else if (HydraulicFunc_Sw(HydFunc) == HydraulicFunc_Sw::BROOKoCOREY) // Brooks & Corey
			return pow(10, m[ipar].Lambda * log10(m[ipar].PsiEntry / pss))*(m[ipar].Saturation - m[ipar].ResWater) + m[ipar].ResWater;

		else {
			double se = pow(1 / (1 + pow(m[ipar].Galpha * pss, m[ipar].GN)), m[ipar].GM);
			return se*(m[ipar].Saturation - m[ipar].ResWater) + m[ipar].ResWater;
		}
	}
private:
	vector<RETENTION_COEF> m;
	size_t HydFunc;


};
struct CONDUCTIVITY_COMMON_PAR {
	double CMinUC, A0Temp, A1Temp;
	double FrCondImped;

};

struct se_rel {
    se_rel(vector<double>& Sat)	: range(Sat) {}
	double operator() (size_t index, double Theta)	  {  return min(1., max(0.001, Theta /range[index])); } 
	vector<double> range;
};
struct power_coef {
	power_coef(vector<double>& b) : b(b) {}
	double operator() (size_t index, double se) { return pow(se, b[index]); };
	vector<double> b;
};
struct powergm {
	powergm(vector<double>& n, vector<double>& gm) : n(n), gm(gm) {}
	double operator() (size_t index, double se) {
		return pow(se, n[index]) * pow((1 - pow(1 - pow(se, 1 / gm[index]), gm[index])), 2.);
	}
	vector<double> n, gm;
};
class se_eff {
public:
	se_eff(vector<double>& Sat, vector<double>& res) : res(res) {
		for (size_t i = 0; i < Sat.size(); i++) range.push_back(Sat[i] - res[i]); }
	double operator() (size_t index,double Theta) { return min(1., max(0.001, (Theta - res[index]) / range[index])); }
private:
	vector<double> res, range;
};



class Conductivity_fromTheta {
public:
	Conductivity_fromTheta(HYDRAULIC_COEF b, CONDUCTIVITY_COMMON_PAR cpar, size_t hydfunc, size_t condfunc) : b(b), p(cpar), HydFunc(hydfunc), condFunc(condfunc) {
		vector<double> sat, reswat, cond_power, tourtous, gm;
			for (size_t i = 0; i<b.ret.size(); ++i) {
				sat.push_back(b.ret[i].Saturation); reswat.push_back(b.ret[i].ResWater); if (ConductivityFunc_Sw(condFunc) == ConductivityFunc_Sw::Power_relative_saturation) cond_power.push_back(b.cond[i].NSR);
				else if (ConductivityFunc_Sw(condFunc) == ConductivityFunc_Sw::Power_effective_saturation) cond_power.push_back(b.cond[i].NSE);
				else if (ConductivityFunc_Sw(condFunc) == ConductivityFunc_Sw::Mualem && HydraulicFunc_Sw(HydFunc) == HydraulicFunc_Sw::BROOKoCOREY)
					cond_power.push_back(2. + b.cond[i].NTortuosity + 2./ b.ret[i].Lambda);
				else { tourtous.push_back(b.cond[i].NTortuosity); gm.push_back(b.ret[i].GM);}
			}
		if (ConductivityFunc_Sw(condFunc) == ConductivityFunc_Sw::Power_relative_saturation)
			f_se = se_rel( sat);
		else
			f_se = se_eff(sat, reswat);

		if (HydraulicFunc_Sw(HydFunc) == HydraulicFunc_Sw::GENUCH && ConductivityFunc_Sw(condFunc) == ConductivityFunc_Sw::Mualem)
			f_ff = powergm(tourtous, gm);
		else if (ConductivityFunc_Sw(condFunc) == ConductivityFunc_Sw::Mualem && HydraulicFunc_Sw(HydFunc) == HydraulicFunc_Sw::BROOKoCOREY)
			f_ff = power_coef(cond_power);
		else if (ConductivityFunc_Sw(condFunc) == ConductivityFunc_Sw::Power_relative_saturation)
			f_ff = power_coef(cond_power);
		else if (ConductivityFunc_Sw(condFunc) == ConductivityFunc_Sw::Power_effective_saturation)
			f_ff = power_coef(cond_power);
	}

	double operator() (size_t ipar, double Theta, double Temp) {
		if (Theta >= b.ret[ipar].Saturation - b.ret[ipar].MacroPore && b.ret[ipar].Saturation - b.ret[ipar].ResWater>b.ret[ipar].MacroPore) {
			return WCondMacroPore(ipar, Theta, Temp); // MacroPore
		}
		else
			return WCondMatric(ipar, Theta, Temp);

	}
	double operator() (bool MatricRange, size_t ipar, double Theta, double Temp) {
		if (MatricRange)
			return WCondMatric(ipar, Theta, Temp);
		else if (Theta >= b.ret[ipar].Saturation - b.ret[ipar].MacroPore && b.ret[ipar].Saturation - b.ret[ipar].ResWater>b.ret[ipar].MacroPore) {
			return WCondMacroPore(ipar, Theta, Temp); // MacroPore
		}
		else
			return WCondMatric(ipar, Theta, Temp);

	}
	double operator() (size_t ipar) { return WCondSat(ipar, 10.); };
 	double operator() (double Temp) {return TempCondFactor(Temp);}
private:
	double WCondMatric(size_t ipar, double Theta, double Temp) {
		double se, ff = 1., conduct;
		double ctemp = TempCondFactor(Temp);
		se = f_se(ipar, Theta);
		ff = f_ff(ipar, se);
	/*	if (condFunc == ConductivityFunc_Sw::Power_relative_saturation)
			se = min(1., max(0.001, Theta / b.ret[ipar].Saturation));
		else
			se = min(1., max(0.001, (Theta - b.ret[ipar].ResWater) / (b.ret[ipar].Saturation - b.ret[ipar].ResWater)));

		if (HydFunc == HydraulicFunc_Sw::GENUCH && condFunc == ConductivityFunc_Sw::Mualem)
			ff = pow(se, b.cond[ipar].NTortuosity)*pow((1 - pow(1 - pow(se, 1 / b.ret[ipar].GM), b.ret[ipar].GM)), 2.);
		else if (condFunc == ConductivityFunc_Sw::Mualem && HydFunc == HydraulicFunc_Sw::BROOKoCOREY)
			ff = pow(se, 2 + b.cond[ipar].NTortuosity + 2 / b.ret[ipar].Lambda);
		else if (condFunc == ConductivityFunc_Sw::Power_relative_saturation)
			ff = pow(se, b.cond[ipar].NSR);
		else if (condFunc == ConductivityFunc_Sw::Power_effective_saturation)
			ff = pow(se, b.cond[ipar].NSE);*/

		if (ff>-20.) {
			if (b.ret[ipar].MacroPore <= 0)
				conduct = b.cond[ipar].SatTotCond * ff*ctemp;
			else
				conduct = b.cond[ipar].SatMatCond * ff*ctemp;
			conduct = max(p.CMinUC, conduct);
		}
		else
			conduct = p.CMinUC;
		return conduct;
	}
	double WCondSat(size_t ipar, double Temp) {
		return b.cond[ipar].SatTotCond;
	}
	double WCondMacroPore(size_t ipar, double Theta, double Temp) {
		double thet_mat = b.ret[ipar].Saturation - b.ret[ipar].MacroPore;
		if (b.ret[ipar].MacroPore>.00001) {
			double ucon4 = WCondMatric(ipar, thet_mat, Temp);
			double sse = min(1., (Theta - b.ret[ipar].Saturation + b.ret[ipar].MacroPore) / b.ret[ipar].MacroPore);
			return pow(10., log10(ucon4) + sse*log10(WCondSat(ipar, Temp) / ucon4));
		}
		else
			return WCondSat(ipar, Temp);
	}
	double TempCondFactor(double Temp) {
		return max(0.1, p.A0Temp + p.A1Temp*Temp);
	}
	
	HYDRAULIC_COEF b;
	size_t HydFunc;
	size_t condFunc;
	CONDUCTIVITY_COMMON_PAR p;
	function<double(size_t, double)> f_se, f_ff;
};
class Conductivity_fromPsi {
public:
	Conductivity_fromPsi(HYDRAULIC_COEF b, CONDUCTIVITY_COMMON_PAR cpar,   size_t hydfunc, size_t condfunc) : hydfunc(hydfunc) {
		f_CondFromTheta = Conductivity_fromTheta(b, cpar, hydfunc, condfunc);
		f_ThetaFromPsi = Theta_fromPsi(b.ret, hydfunc);
	}
	double operator() (size_t ipar, double psi, double temp) {
		return f_CondFromTheta(ipar, f_ThetaFromPsi(ipar, psi), temp);
	}
private:
	size_t hydfunc;
	function<double(size_t, double, double)> f_CondFromTheta;
	function<double(size_t, double)> f_ThetaFromPsi;
};
class Conductivity_fromPartlyFrozen {
public:
	Conductivity_fromPartlyFrozen(HYDRAULIC_COEF b, CONDUCTIVITY_COMMON_PAR cpar, size_t hydfunc, size_t condfunc) : cpar(cpar) {
		f_CondFromTheta = Conductivity_fromTheta(b, cpar, hydfunc, condfunc);
	}
	double operator() (size_t ipar, double Theta, double Temp, double ThQual) {
		double imped = pow(10., -ThQual*cpar.FrCondImped);
		return min(cpar.CMinUC, imped*f_CondFromTheta(ipar, Theta, Temp));
	}
private:
	CONDUCTIVITY_COMMON_PAR cpar;
	function<double(size_t, double, double)> f_CondFromTheta;
};
class Conductivity_HighFlowDamp {
public:
	Conductivity_HighFlowDamp(double HighFlowDampC) :HighFlowDampC(HighFlowDampC) {}
	double operator() (double IceContent) {
		return exp(-IceContent / HighFlowDampC);
	}
private:
	double HighFlowDampC;
};
class MatricConductivityFunctor {
public:
	MatricConductivityFunctor(double CommonConductivity, double SensAboveThreshold) : CommonConductivity(CommonConductivity), SensAboveThreshold(SensAboveThreshold) {}
	double operator() (double value) {
		if (value<CommonConductivity)
			return value;
		else {
			double diff = (log10(value) - log10(CommonConductivity))*SensAboveThreshold + log10(CommonConductivity);
			return pow(10., diff);
		}
	}
private:
	double CommonConductivity, SensAboveThreshold;

};
class WaterContent_FlowRate {
public:
	WaterContent_FlowRate(HYDRAULIC_COEF par) : par(par) {}
	double operator() (size_t i, double flowrate, double Thickness) {
		double  PSB, PSI, SE;
		PSB = par.ret[i].PsiEntry * pow(flowrate / par.cond[i].SatMatCond, 1 / (-2 - par.ret[i].Lambda * (2 + par.cond[i].NTortuosity)));
		PSI = PSB + Thickness * 25;
		SE = pow(PSI / par.ret[i].PsiEntry, -par.ret[i].Lambda);
		return SE*(par.ret[i].Saturation - par.ret[i].ResWater) + par.ret[i].ResWater;
	}
private:
	HYDRAULIC_COEF par;
};
class AirEntry_fromTexture {
public:

	AirEntry_fromTexture() : C() {
		double k[13] = { 5.3396738,   0.1845038, -2.48394546, -0.00213853,
			-0.04356349, -0.61745089, 0.00143598, -0.00855375,
			-0.00001282,  0.00895359, -0.00072472, 0.0000054, 0.50028060 };
		C.assign(k, k + 13);
	}
	double operator() (double SAND, double CLAY, double PORO) {
		double A = C[0] + C[1] * CLAY + C[2] * PORO + C[3] * CLAY*CLAY + C[4] * SAND*PORO
			+ C[5] * PORO*CLAY + C[6] * SAND*SAND*PORO*PORO + C[7] * CLAY*CLAY*PORO*PORO
			+ C[8] * SAND*SAND*CLAY + C[9] * CLAY*CLAY*PORO
			+ C[10] * SAND*SAND*PORO + C[11] * CLAY*CLAY*SAND + C[12] * PORO*PORO*CLAY;
		return exp(A);
	}

private:
	vector<double> C;
	vector<double> SandContent, ClayContent, Saturation;

};
class Lambda_fromTexture {
public:
	Lambda_fromTexture() : C() {
		double k[12] = { -0.7842831, 0.0177544, -1.062468, -0.00005304,
			-0.00273493, 1.11134946, -0.03088295, 0.00026587,
			-0.00610522, -0.00000235, 0.00798746,-0.00674491 };
		C.assign(k, k + 12);
	}
	double operator() (double SAND, double CLAY, double PORO) {
		double A;
		A = C[0] + C[1] * SAND + C[2] * PORO + C[3] * SAND*SAND + C[4] * CLAY*CLAY
			+ C[5] * PORO*PORO + C[6] * SAND*PORO + C[7] * SAND*SAND*PORO*PORO
			+ C[8] * CLAY*CLAY*PORO*PORO + C[9] * SAND*SAND*CLAY
			+ C[10] * CLAY*CLAY*PORO + C[11] * CLAY*PORO*PORO;
		return exp(A);

	}
private:
	vector<double> C;

};
class Residual_fromTexture {
public:
	Residual_fromTexture() : C() {
		double k[9] = { -0.0182482 , 0.00087269, 0.00513488, 0.02939286,
			-0.00015395,-0.0010827 ,-0.00018233, 0.00030703,
			-0.0023584 };
		C.assign(k, k + 9);
	}
	double operator() (double SAND, double CLAY, double PORO) {
		double  A;
		A = C[0];
		A += C[1] * SAND;
		A += C[2] * CLAY;
		A += C[3] * PORO;
		A += C[4] * CLAY*CLAY;
		A += C[5] * SAND*PORO;
		A += C[6] * CLAY*CLAY*PORO*PORO;
		A += C[7] * CLAY*CLAY*PORO;
		A += C[8] * CLAY*PORO*PORO;
		A = max(0., 100.*A);
		return A;
	}
private:
	vector<double> C;

};
class HydraulicCond_fromTexture {
public:
	HydraulicCond_fromTexture() : C() {
		double k[13] = { 19.52348, -8.96847 , -0.028212, 0.00018107,
			-0.0094125, -8.395215, 0.077718, -0.00298,
			-0.019492,  0.0000173, 0.02733, 0.001434, -0.0000035 };
		C.assign(k, k + 13);
	}
	double operator() (double SAND, double CLAY, double PORO) {
		double A;
		A = PORO;	  A = A*C[0];	  A = A + C[1];	  A = A + C[2] * CLAY;  A = A + C[3] * SAND*SAND;
		A = A + C[4] * CLAY*CLAY;	  A = A + C[5] * PORO*PORO;
		A = A + C[6] * SAND*PORO;	  A = A + C[7] * SAND*SAND*PORO*PORO;
		A = A + C[8] * CLAY*CLAY*PORO*PORO;	  A = A + C[9] * SAND*SAND*CLAY;
		A = A + C[10] * CLAY*CLAY*PORO;	  A = A + C[11] * SAND*SAND*PORO;
		A = A + C[12] * CLAY*CLAY*SAND;
		return 240.*exp(A);
	}
private:
	vector<double> C;

};
class WiltingPoint_fromClay {
public:
	WiltingPoint_fromClay() : C() {
		double k[2] = { 0.45, 2. };
		C.assign(k, k + 2);
	}
	double operator() (double CLAY) {
		return C[0] + CLAY*C[1];
	}
private:
	vector<double> C;

};

class PedoFunc_fromIndex {
public:
	PedoFunc_fromIndex(enum  PedoType Opt, vector<double> Sand, vector<double> Clay, vector<double> Sat) :
		Option(Opt), SandContent(Sand), ClayContent(Clay), Saturation(Sat) {
		switch (Opt) {
		case PedoType::AIRENTRY:
			f_Pedo = AirEntry_fromTexture();
			break;
		case PedoType::LAMBDA:
			f_Pedo = Lambda_fromTexture();
			break;
		case PedoType::RESIDUAL:
			f_Pedo = Residual_fromTexture();
			break;
		case PedoType::CONDUCTIVITY:
			f_Pedo = HydraulicCond_fromTexture();
			break;
		case PedoType::WILTING:
			f_PedoWilt = WiltingPoint_fromClay();
			break;
		}
	}
	double operator() (size_t index) {
		if (Option == PedoType::WILTING) return f_PedoWilt(ClayContent[index]);
		else return f_Pedo(SandContent[index], ClayContent[index], Saturation[index] / 100.);
	}
private:
	enum  PedoType Option;
	vector<double> SandContent, ClayContent, Saturation;
	function<double(double, double, double)> f_Pedo;
	function<double(double)> f_PedoWilt;
};

struct SOILPROPEVAPORATION {
	RETENTION_COEF ret_ulayer;
	CONDUCTIVITY_COEF cond_ulayer;
	double Thickness_ulayer;
};
class MaxSoilEvaporationRate_PsiUpperlayer {
public:
	MaxSoilEvaporationRate_PsiUpperlayer(SOILPROPEVAPORATION p, size_t HydFuncOpt) : par(p), HydFuncOption(HydFuncOpt) {}
	double operator() (double PsiUpperMidLayer) {
		double b, PsiNullMax;
		if (HydraulicFunc_Sw(HydFuncOption) == HydraulicFunc_Sw::BROOKoCOREY)
			b = -((par.cond_ulayer.NTortuosity + 2)*par.ret_ulayer.Lambda + 2);
		else
			b = -((par.cond_ulayer.NTortuosity + 2)*(par.ret_ulayer.GN - 1) + 2);
		PsiNullMax = ((((par.Thickness_ulayer * 100)*b) / 2) - (PsiUpperMidLayer*(1 - b))) / (1 + b);
		if (HydraulicFunc_Sw(HydFuncOption) == HydraulicFunc_Sw::BROOKoCOREY)
			return par.cond_ulayer.SatMatCond * pow((PsiNullMax + PsiUpperMidLayer) / (2 * par.ret_ulayer.PsiEntry), b);
		else {
			double a = pow((PsiNullMax + PsiUpperMidLayer) / 2, par.ret_ulayer.GN - 1);
			double b = pow(1 + (1 / par.ret_ulayer.PsiEntry)*((PsiNullMax + PsiUpperMidLayer) / 2), par.ret_ulayer.GN);
			return par.cond_ulayer.SatMatCond * pow(1 - ((1 / par.ret_ulayer.PsiEntry)*a*pow(b, -par.ret_ulayer.GM)), 2)
				/ pow(b, par.ret_ulayer.GM * par.cond_ulayer.NTortuosity);
		}
	}
private:
	SOILPROPEVAPORATION par;
	size_t HydFuncOption;
};
