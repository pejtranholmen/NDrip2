#pragma once
#include "../../../std.h"

class N2O_TempResponse {
public:
	N2O_TempResponse(double N2O_TempMax, double N2O_TempOpt, double N2O_TempShape) noexcept :temp_max(N2O_TempMax), temp_opt(N2O_TempOpt), temp_shape (N2O_TempShape) {}
	double operator() (double Temp) noexcept {
		if (Temp > temp_max) 
			return 0.001;
		else
			return pow((temp_max - Temp) / (temp_max - temp_opt), temp_shape) *
				exp(temp_shape * (Temp - temp_opt) / (temp_max - temp_opt));
	}
private:
	double temp_max, temp_opt, temp_shape;
};
class NOxide_MoistureResponse {
public:
	NOxide_MoistureResponse(double RelSatCrit, double RelSatForm) noexcept :relsat_crit(RelSatCrit), relsat_shape(RelSatForm) {}
	double operator() (double RelSat) noexcept {
		return 1 - 1 / (1 + exp((RelSat - relsat_crit) / relsat_shape));
	}
private:
	double relsat_crit,relsat_shape;
};
class NOxide_PH_Response {
public:
	NOxide_PH_Response(double NO_PH_Coef) noexcept :no_ph_coef(NO_PH_Coef)  {}
	double operator()  (double Soil_PH) noexcept {
		return min(1., max(0.05, no_ph_coef - Soil_PH));
	}
private:
	double no_ph_coef;
};