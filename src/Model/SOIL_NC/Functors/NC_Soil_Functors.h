#pragma once
#include "../../../std.h"
#include "./NC_Minerals_Functors.h"
#include "./P_Minerals_Functors.h"
#include "./NC_Organic_Functors.h"
#include "./NC_ExternalInputs_Functors.h"
#include "./NC_GasFluxFunctors.h"


enum class Common_Temp_Response_Sw {Q10_whole_range,Q10_above_threshold,Ratkowsky_function,O_Neill_function,  Arrhenius_function};
enum class Common_Moisture_Response_Sw{ Theta_Function,CO2_Function, CombinedThetaCO2};



class Q10 {
public: Q10(double Q10, double Bas, double Threshold) :q10(Q10), bas(Bas), Threshold(Threshold) { WithThreshold = true; }
public: Q10(double Q10, double Bas) :q10(Q10), bas(Bas) { WithThreshold = false; }
		double operator() (double Temp) {
			auto response = pow(q10, (Temp - bas) / 10.);
			if (WithThreshold) {
				if (Temp < Threshold&& Temp>0.)
					return Temp / Threshold*response;
				else if (Temp > Threshold)
					return response;
				else
					return 0.;
			}
			else
				return response;
		}
private:
	double q10, bas, Threshold;
	bool WithThreshold;
};
class Ratkowsky {
public : Ratkowsky(double min, double max): min1(min), max1(max) {
	if (max <= min) max = min + 0.1;
}
	 double operator () (double Temp) {
		 if (Temp > max1)
			 return 1.;
		 else if (Temp < min1)
			 return 0.;
		 else
			 return pow((Temp - min1) / (max1 - min1), 2.);
}
private:
	double min1, max1;
};
class Neill {
public: Neill( double Form, double Max, double Opt) : opt(Opt), max1(Max), form(Form) {}
		double operator() (double Temp) {
		if (Temp>max1)
				return 0.;
		else
			return pow((max1 - Temp) / (max1 - opt), form)
				*exp(form*(Temp - opt) / (max1 - opt));
}
private:
	double opt, max1, form;

};
class DenitTheta {
public: DenitTheta(double range, double PowerC, vector<double> saturation): range(range), PowerC(PowerC), saturation(saturation) {}
		double operator() (size_t index, double Theta) {
			if (Theta >= saturation[index])
				return 1.;
			else if (saturation[index] - Theta>range)
				return 0.;
			else
				return pow((Theta - (saturation[index] - range)) / range, PowerC);
}
private:
	double range, PowerC;
	vector<double> saturation;
};
class CommonThetaResponse {
public: CommonThetaResponse(double SatActivity, double LowerRange, double UpperRange, double PowerCoef,
	vector<double> saturation, vector<double> wilting) : SatActivity(SatActivity), LowerRange(LowerRange),
	UpperRange(UpperRange), PowerCoef(PowerCoef), saturation(saturation), wilting(wilting) {}
	double operator() (size_t index, double Theta) {
			if (Theta >= saturation[index])
				return SatActivity;
			else if (Theta<wilting[index])
				return 0.;
			else {
				double out;
				out = min(1., pow((saturation[index] - Theta) / UpperRange, PowerCoef)
					*(1. - SatActivity) + SatActivity);
				return min(out, pow((Theta - wilting[index]) / LowerRange, PowerCoef));
			}
}

private:
	double SatActivity, LowerRange, UpperRange, PowerCoef;
	vector<double> saturation, wilting;
};
class CommonCO2Response {
public: CommonCO2Response( double LowestImpact, double UpperThreshold): LowestImpact(LowestImpact), UpperThreshold(UpperThreshold) {}
	  double operator() (double CO2_Conc) {
		  if (CO2_Conc < LowestImpact)
			  return 1.;
		  else
			  return max(0., min(1., 1. - (CO2_Conc - LowestImpact) / (UpperThreshold - LowestImpact)));
	  }
private:
	double LowestImpact, UpperThreshold;

}; 
class CombinedCO2ThetaResponse {
public: CombinedCO2ThetaResponse(double UpperRange, double LowerRange, double PowerCoef, double LowestImpact, double UpperThreshold,
	vector<double> saturation, vector<double> wilting) : SatActivity(SatActivity), LowerRange(LowerRange),
	UpperRange(UpperRange), PowerCoef(PowerCoef), LowestImpact(LowestImpact), UpperThreshold(UpperThreshold), saturation(saturation), wilting(wilting) {}

	  double operator() (size_t index, double Theta, double CO2_Conc) {
		  if (Theta >= saturation[index]-0.01)  return 0.;
		  else if (Theta > saturation[index] - UpperRange) {
			  if (CO2_Conc < LowestImpact)
				  return 1.;
			  else
				  return max(0., min(1., 1. - (CO2_Conc - LowestImpact) / (UpperThreshold - LowestImpact)));
		  }
		  else if (Theta < wilting[index]) return 0.;
		  else {

			  ResponseCO2=max(0., min(1., 1. - (CO2_Conc - LowestImpact) / (UpperThreshold - LowestImpact)));
			  ResponseWater= min(1., pow((Theta - wilting[index]) / LowerRange, PowerCoef));

			  return  min(ResponseCO2, ResponseWater);
		  }
	  };

	private:
		double SatActivity, LowerRange, UpperRange, PowerCoef;
		vector<double> saturation, wilting;
		double LowestImpact, UpperThreshold;
		double ResponseCO2, ResponseWater;

};
class Arrhenius  {
public: Arrhenius(double Act, double Tem0) noexcept : act(Act), tem0(Tem0) {}
		double operator () (double Temp) noexcept {
			return exp(-(act / Fix::RGAS*((1 / (Temp - Fix::ABS_ZERO) - 1 / (tem0 - Fix::ABS_ZERO)))));
		}
private:
	double act, tem0;
};

