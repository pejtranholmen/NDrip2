#pragma once
#include "./../std.h"

class FirstOrderResponse {
public:
	FirstOrderResponse(double Coef) noexcept : Coef(Coef) {};
	FirstOrderResponse(vector<double> v_Coef) noexcept : v_Coef(v_Coef) {};
	double operator()(double State)  noexcept { return Coef * State; };
	double operator()(size_t i, double State)  noexcept { return v_Coef[i] * State; };
private:
	double Coef;
	vector<double> v_Coef;
};
class Michaelis_Menten_Response {
public:
	Michaelis_Menten_Response(double Coef) noexcept : c_half_rate(Coef) {};
	double operator()(double Conc) noexcept { return  Conc / (Conc + c_half_rate);}
private:
	double c_half_rate;
};
class ExpDecline_Response {
public:
	ExpDecline_Response(double Coef) noexcept : c (Coef) {};
	double operator()(double factor) noexcept { return  1-exp(-c*factor); }
private:
	double c;
};
class ExpSquaredDecline_Response {
public:
	ExpSquaredDecline_Response(double Coef) noexcept : c(Coef) {};
	double operator()(double factor) noexcept { return  1 - exp(-c * factor*factor); }
private:
	double c;
};
class ExpSquared_Response {
public:
	ExpSquared_Response(double Coef) noexcept : c(Coef) {};
	double operator()(double factor) noexcept { return  exp(-c * factor * factor); }
private:
	double c;
};
