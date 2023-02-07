#pragma once
#include "../../../std.h"
#include "./../../Basic_Functors.h"
#include "./BasicGas_Functors.h"

#include "./NOxide_Functors.h"
#include "./Methane_Functors.h"		
enum class COS_Model_Sw { off, OnlyPlant, Plant_Soil };
enum class COS_PlantModel_Sw { Off, Tranpiration_Base, PhotoSynthesis_Base, Experimental };
enum class COS_SoilMoidel_Sw { Off, Heteotrophic_Respiration_Base };

class FromAnaerobicToAerobicDiffusion {
public:
	FromAnaerobicToAerobicDiffusion(double DiffRedFrac, double DiffRedFracBase) :a(DiffRedFrac), b(DiffRedFracBase) {}
	double operator () (double anaerobic_frac) {
		return anaerobic_frac * max(0., (1 - anaerobic_frac)) * a + b;
}
private:
	double a, b;
};
class DenitrifiersAnaerobicMoistureResponse {
public:
	DenitrifiersAnaerobicMoistureResponse(double Denit_Act_PowerCoef): powercoef(Denit_Act_PowerCoef)  {}
	double operator() (double anaerobicfrac) {
		return pow(anaerobicfrac, powercoef);

}
private:
	double powercoef;
};
class COS_PlantFlux {
	public:
	COS_PlantFlux(double Cos_Ratio) : Cos_Ratio(Cos_Ratio) {}

	double operator() (double FluxBase) {
		return 	FluxBase * Cos_Ratio;

	}
	private:
		double Cos_Ratio;
};
class COS_SoilFlux {
public:
	COS_SoilFlux(double Cos_Ratio) : Cos_Ratio(Cos_Ratio) {}

	double operator() (double FluxBase) {
		return 	FluxBase * Cos_Ratio;

	}
private:
	double Cos_Ratio;
};



