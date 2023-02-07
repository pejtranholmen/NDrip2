#pragma once
#include "../../../std.h"
#include "../../../ModelTypes/FunctFactory.h"

class PH_Response_Weathering {
public:
	PH_Response_Weathering(double PH_Shape, double PH_Opt) noexcept : shape(PH_Shape), opt(PH_Opt) {};
	double operator() (double pH) noexcept {
		if (pH <= opt)
			return pow(10., -shape * (opt - pH));
		else
			return pow(10., shape * (opt - pH));
	};
private:
	double shape, opt;
};

class LangmuirAdsorption {
public:
	LangmuirAdsorption(double max_cap, double c50): max_cap(max_cap), c50(c50) {
	}
	double operator() (double PO4_Sol) {
		return c50 * PO4_Sol / (max_cap - PO4_Sol);
}	
private:
	double max_cap, c50;
};

class P_Surfacerunoff_Conc {
public:
	P_Surfacerunoff_Conc (double Deltaconc, double Baseconc, double ThreFlow) : Deltaconc (Deltaconc), Baseconc (Baseconc), ThreFlow (ThreFlow){
	}
	double operator() (double SurfaceOutFlow) {
		return min(1., SurfaceOutFlow / ThreFlow)*(Deltaconc - Baseconc)+Baseconc;
	}
private:
	double Baseconc, Deltaconc, ThreFlow;
};

