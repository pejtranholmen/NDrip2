#pragma once
#include "../../../std.h"

enum class DissolvedOrganics_Sw { off, on };
enum class Microbes_Sw { off,on_substate_dependent,on_microbe_dependent };
enum class LitterPools_Sw { one,two };
enum class FaecesPool_Sw { no, one };
enum class CN_Ratio_Influence_Sw { no, on_litter_consumption };
enum class Microbial_Consumption_Sw{ Static, F_Temp, F_Temp_Moisture };
enum class Microbial_Mortality_Sw { Static, F_Temp, F_Temp_Moisture };
enum class Initial_Soil_Organic_Sw { Table, Linear_decrease, Constant, Exponential };
enum class OrganicUptake_Sw { off, on, on_with_threshold };
enum class InitialCarbonConditions_Sw { As_Nitrogen, As_Nitogen_and_Carbon };
enum class InitialPhosphorusConditions_Sw { As_Carbon, As_CP_ratio_and_Carbon };
enum class Measured_Organic_Table_Sw { As_model_layers, As_measured_layers };
enum class DisplacementOrganicLayers_Sw { Static, At_Year_Shift, At_Year_shift_Delay, At_week_Shift_At_week_shift_Delay };

class turnover {
public:
	turnover(double MicRatio, double Efficiency, double FracTrans, double AvailFracImmob, double RateCoef,  double P_AvailFracImmob, double Mic_PRatio)
		: RateCoef(RateCoef), CN_MicrobeRatio(MicRatio), Efficiency(Efficiency), FracTrans(FracTrans), N_FracImmob(AvailFracImmob),
		P_FracImmob(P_AvailFracImmob), CP_MicrobeRatio(Mic_PRatio) {};
	turnover(double MicRatio, double Efficiency,  double FracTrans, double AvailFracImmob, double RateCoef)
		: RateCoef(RateCoef), CN_MicrobeRatio(MicRatio), Efficiency(Efficiency), FracTrans(FracTrans), N_FracImmob(AvailFracImmob)
	{	};
	turnover(double MicRatio, double Efficiency, double AvailFracImmob, double RateCoef)
		: RateCoef(RateCoef), CN_MicrobeRatio(MicRatio), Efficiency(Efficiency), N_FracImmob(AvailFracImmob) {
		FracTrans = 0.;	};
	turnover(double MicRatio, double Efficiency, double AvailFracImmob, double RateCoef, double P_AvailFracImmob, double MicCPRatio)
		: RateCoef(RateCoef), CN_MicrobeRatio(MicRatio), Efficiency(Efficiency), N_FracImmob(AvailFracImmob), CP_MicrobeRatio(MicCPRatio), 
		P_FracImmob(P_AvailFracImmob)  {
		FracTrans = 0.;
	};
	OrgTurnOver operator()  (double DecompRate, double CN_Ratio, double Mineral) {
		OrgTurnOver out;
		DecompRate *= RateCoef;
		out.Respiration = (1 - Efficiency)*DecompRate;
		out.Trans_Org = Efficiency * FracTrans*DecompRate;
		out.N_Trans_Conv = out.Trans_Org / CN_MicrobeRatio;
		out.N_Mineralisation = Mineralisation(DecompRate, CN_Ratio, CN_MicrobeRatio, N_FracImmob, Mineral );
		return out;
	}
	OrgTurnOver operator()  (double DecompRate, double CN_Ratio, double Mineral, double CP_Ratio,  double P_Mineral) {
		OrgTurnOver out;
		DecompRate *= RateCoef;
		out.Respiration = (1 - Efficiency)*DecompRate;
		out.Trans_Org = Efficiency * FracTrans*DecompRate;
		out.N_Trans_Conv = out.Trans_Org / CN_MicrobeRatio;
		out.N_Mineralisation = Mineralisation(DecompRate, CN_Ratio,CN_MicrobeRatio, N_FracImmob, Mineral);
		out.P_Trans_Conv = out.Trans_Org / CP_Ratio;
		out.P_Mineralisation = Mineralisation(DecompRate, CP_Ratio, CP_MicrobeRatio, P_FracImmob, P_Mineral);

		return out;
	}


private:
	double Mineralisation(double DecompRate,double InputRatio, double MicrobeRatio, double FracImmob, double Mineral) {
		double MinRate = DecompRate * (1. / InputRatio - Efficiency / MicrobeRatio);
		if (MinRate < 0.) {
			if (Mineral > 0)
				return max(-FracImmob * Mineral, MinRate);
			else
				return  0.;
		}
		return MinRate;

	};
	double CN_MicrobeRatio;
	double CP_MicrobeRatio;
	double Efficiency;
	double FracTrans;
	double N_FracImmob;
	double P_FracImmob;
	double RateCoef;

};
class CN_RatioLinearResponse {
public:
		CN_RatioLinearResponse(double CN_Max, double CN_Min) : CN_Max(CN_Max), CN_Min(CN_Min) {}

	double operator() (double CN_Ratio) {
		return 1. - min(1., max(0., (CN_Ratio - CN_Min) / CN_Max - CN_Min));
	}

private:
	double CN_Max, CN_Min;
};
class SubstrateConcentrationResponse {
public:
	SubstrateConcentrationResponse(double HalfRateConc) : HalfRateConc(HalfRateConc) {};
	double operator () (double SubConc) {
		if (SubConc <= 0.01*HalfRateConc)
			return 0.;
		else
			return SubConc / (SubConc + HalfRateConc);
	}
private:
	double HalfRateConc;

};
class PO4ConcentrationResponse {
public:
	PO4ConcentrationResponse(double PO4ConcMin, double PO4ConcMax) : PO4ConcMin(PO4ConcMin), PO4ConcMax(PO4ConcMax) {};
	double operator () (double PO4Conc) {
		return 1. - min(1., max(0., (PO4Conc - PO4ConcMin) / PO4ConcMax - PO4ConcMin));

	}
private:
		double PO4ConcMin, PO4ConcMax;
};
class FirstOrderRateResponse {
public:
	FirstOrderRateResponse(vector<double> v1) : av(v1) {
		if (av.size() == 8) {
			for (size_t i = 0; i < 4; i++) {
				factorv.push_back(av[i + 4] * av[i]);
			}
		}
		else {
			for (size_t i = 0; i < av.size() - 1; i++) {
				factorv.push_back(av[i + 1] * av[0]);
			}
		}
	};
	FirstOrderRateResponse(double Eff, double Frac, double ComplementValue) : a(Eff), b(Frac) { factor =(ComplementValue- a) * b; };
	FirstOrderRateResponse(double Frac, double RateCoef) :  b(Frac), c(RateCoef) { factor = b*c; };
	FirstOrderRateResponse(double RateCoef) : c(RateCoef) { factor =c; };
	FirstOrderRateResponse(vector<double> v1, vector<double> v2) : av(v1), bv(v2) {
		for (size_t i = 0; i < av.size(); i++) {
			factorv.push_back(av[i] * bv[i]);
		}
	};


	double operator() (double Input) {
		return Input*factor;
	};
	double operator() (size_t i, double Input) {
		return Input*factorv[i];
	};
	private:
		double a, b, c, factor;
		vector<double> av, bv, factorv;

};
class OrganicWaterConcentration {
public:
	OrganicWaterConcentration(double DocSolFraction, vector<double> &ThicknessLayers) :Frac(DocSolFraction) {
		for (size_t i = 0; i < ThicknessLayers.size(); i++) {
			CoefSum.push_back(Frac/ThicknessLayers[i]);// Frac=1 in default, by HH
		};
	};
	vector<double> operator() (vector<double> &State, vector<double> &Theta, vector<double> Conc) {
		for (size_t i = 0; i < State.size(); i++) {
			Conc[i]=CoefSum[i] * State[i] / Theta[i];
			if (State[i]<1.E-5 || Conc[i]>1.E3) Conc[i] = 1.E-7;
			else if (State[i] > 0.);
			else Conc[i] = 1.E-7;
		}
		return Conc;
	};


private:
	double Frac;
	vector<double> CoefSum;
};
class VerticalSoluteWaterFlows {
public:
	VerticalSoluteWaterFlows(bool DeepPercolation) : DeepPercolation(DeepPercolation) {};
	vector<double> operator() (vector<double> &WaterFlow, vector<double> &Conc, double DeepPercolationFlow, vector<double> &ItemFlow) {
		for (size_t i = 0; i < Conc.size(); i++) {
			if (WaterFlow[i] > 0.)
				ItemFlow[i] = WaterFlow[i] * Conc[i] / 1000.;
			else if (i < Conc.size() - 1)
				ItemFlow[i] = WaterFlow[i] * Conc[i + 1] / 1000.;
			else
				ItemFlow[i] = 0.;
		}
		if (DeepPercolation) ItemFlow.back() += DeepPercolationFlow * Conc.back() / 1000.;
		return ItemFlow;
	};

private:
	bool DeepPercolation;
};
class HorisontalSoluteWaterFlows {
public:
	HorisontalSoluteWaterFlows() {};
	pair<double,vector<double>> operator() (vector<double> DrainageFlow, vector<double> &Conc, vector<double> &ItemDrainage) {
		auto cflux = 0.;
		auto wflux = 0.;
		for (size_t i = 0; i < Conc.size(); i++) {
			ItemDrainage[i] = max(0.,DrainageFlow[i] * Conc[i] / 1000.);
			wflux += DrainageFlow[i];
			cflux += ItemDrainage[i];
		}
		if (wflux > 1.E-4)
			return pair<double, vector<double>>(1000 * cflux / wflux, ItemDrainage);
		else
			return pair<double, vector<double>>(0.,ItemDrainage);
	}
private:
};
class DissolvedTransformation {
public:
	DissolvedTransformation(vector<double> RateCoef, function<double(double)> f_Temp, function<double(size_t, double)> f_Theta) : RateCoef(RateCoef), f_Temp(f_Temp), f_Theta(f_Theta) {};
	DissolvedTransformation(vector<double> RateCoef, vector<double> RateCoefDis, function<double(double)> f_Temp, function<double(size_t, double)> f_Theta) : RateCoef(RateCoef), RateCoefDis(RateCoefDis), f_Temp(f_Temp), f_Theta(f_Theta) {};

		vector<double> operator() (size_t i_substrate,  vector<double> &Temp, vector<double> &Theta, vector<double> &State, vector<double> &TransformFlux) {
			for (size_t i = 0; i < Theta.size(); i++) 
				TransformFlux[i] =RateCoef[i_substrate]* f_Temp(Temp[i])*f_Theta(i, Theta[i])*State[i];
			return TransformFlux;
		};
		vector<double> operator() (size_t i_substrate, vector<double> &Temp, vector<double> &Theta, vector<double> &StateOrg, vector<double> &StateDisOrg, vector<double> &TransformFlux) {
			for (size_t i = 0; i < Theta.size(); i++)
				TransformFlux[i] = RateCoef[i_substrate] * f_Temp(Temp[i])*f_Theta(i, Theta[i])*StateOrg[i] -
				RateCoefDis[i] * f_Temp(Temp[i])*f_Theta(i, Theta[i])*StateDisOrg[i];
			return TransformFlux;
		};

private:
	function<double(double)> f_Temp;
	function<double(size_t, double)> f_Theta;
	vector<double> RateCoef, RateCoefDis;
};
