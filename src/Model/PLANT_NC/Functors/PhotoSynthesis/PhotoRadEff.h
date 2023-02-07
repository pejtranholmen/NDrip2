#pragma once
#include "../../../../std.h"



class PhoRadEff {
public:
	PhoRadEff(vector<double> Eff) : a(Eff) { for (size_t i = 0; i < a.size(); i++) a[i]*= 1.E-6*Fix::BiomassToCarbon; }
	double operator() (size_t index) noexcept {
		return a[index];
	}
	double operator() (size_t index, double AdsorbedLight) noexcept {
		return a[index]*AdsorbedLight;
	}

private:
	vector<double> a;
};
class PhoSaturationResponse {
public:
	PhoSaturationResponse(vector<double> PhoMax): Coef(PhoMax) {}
	double operator() (size_t index, double PhotoSatRed) noexcept {
		return 1 - exp(-PhotoSatRed / Coef[index]);
	}
private:
	vector<double> Coef;
};
class PhoGrainFillResponse {
public:
	PhoGrainFillResponse(vector<double> PhoRadEff_Red): RedPercentage(PhoRadEff_Red) {
		for (size_t i = 0; i < PhoRadEff_Red.size(); i++) RedPercentage[i] /= 100.;
	}
	double operator() (size_t index, double DegreeGrainFilling) noexcept {
		return max(0., 1. - RedPercentage[index]*DegreeGrainFilling);
	}
private:
	vector<double> RedPercentage;
};


