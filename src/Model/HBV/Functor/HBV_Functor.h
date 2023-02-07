#pragma once
#include "../../../std.h"

class ActualEvaporation {
public: ActualEvaporation(double FieldCapacity, double CriticalUptakeFrac):
	FieldCapacity(FieldCapacity), CriticalUptakeFrac(CriticalUptakeFrac)
{}

//		std::string name() const { return "Actual Evaporation."}
	double operator () (double SoilStorage, double PotentialEvap) {
		return min(PotentialEvap*min(1., SoilStorage / (CriticalUptakeFrac*FieldCapacity)), SoilStorage);
	}
private: 
	double FieldCapacity, CriticalUptakeFrac;

};

class Infiltration {
public: Infiltration(double FieldCapacity, double BetaCoef) :
	FieldCapacity(FieldCapacity), BetaCoef(BetaCoef) {}
	double operator() (double SoilStorage, double Input, double Evaporation) {
			double value;
			value = Input*RelativeInfilFunc(SoilStorage);
			if (Evaporation - SoilStorage>value) value = Evaporation - SoilStorage;
			return value;
	}
private:
	double RelativeInfilFunc(double SoilStorage) {
		return max(1. - pow(max(SoilStorage, 0.00001) / FieldCapacity, BetaCoef),0.);
	}
	double FieldCapacity, BetaCoef;
};

class PeakRunOff {
public:PeakRunOff(double DisCoef,double DisCoefAlfa, double T_Step) : K1(DisCoef), Alfa(DisCoefAlfa), T_Step(T_Step) {}
	   double operator() (double Storage) {
		   double test = K1*pow(max(Storage, 0.0001), 1. + Alfa);
		   if (test>abs(Storage)*T_Step) return 0.6*abs(Storage)*T_Step;
		   else return test;
	   }
private:
	double K1, Alfa, T_Step;

};

class BaseRunOff {
public:BaseRunOff(double DisCoefk2 ): K2(DisCoefk2)  {}
	   double operator() (double Storage) {
		   return K2*max(Storage, 0.);
   }
private:
	double K2;
};

class GroundWaterLevel {
	public:GroundWaterLevel(double Min_GWLev, double Max_GWLev, double Sens_GWLev, double PowerCoef_GWLev)
		: Min(Min_GWLev), Max(Max_GWLev), Sens(Sens_GWLev), PowerCoef(PowerCoef_GWLev) {}
		   double operator() (double Storage) {
		   double firstvalue;
		   firstvalue = Min + Sens*pow(Storage / 1000., PowerCoef);
		   if (firstvalue>Max) return Max;
		   return firstvalue;
}

private:
	double Min,Max, Sens, PowerCoef;

};

class GroundWaterReDistribution {
public: GroundWaterReDistribution(double MaxPerc): MaxPerc(MaxPerc) {}
		double operator() (double PeakStorage) {
		return	max(0., min(MaxPerc, PeakStorage));
}
private:
	double MaxPerc;
};



