#pragma once
#include "../../../std.h"
#include "../../SOIL/Hydraulic.h"
#include "../../Atmosphere/Irrigation.h"
#include "./Plant_Functors.h"
#include "../../Structure/Functors/Structure_Functors.h"
#include "../../SOIL/Functors/Profile_Functors.h"
#include <functional>


enum class WUptake_Sw { Without_flexible_roots, With_flexible_roots };
enum class Salt_Influence_Sw { Not_considered,Added_to_pressure_head,Add_multiplicative_response,Add_minimum_response };
enum class WUptBasic_equation_Sw { Pressure_head_response,Darcy_based };
enum class PlantWaterStorage_Sw { Not_considered,f_LAI,f_LAI_height};
enum class PlantResistance_Sw { No_salt_effect,Salt_effect_by_osmotic_pressure};
enum class SaltCoefValid_Sw { No_salt_effect,Salt_effect};
enum class WupTempResponse_Sw { None,Double_exponential,Single_exponential,Polynomial};

class WaterUptakeTempFunction {
public:
WaterUptakeTempFunction() { Option = 0; }

WaterUptakeTempFunction(double WupATe, double WupBTe, double WupDayOff) : a(WupATe), b(WupBTe), dayoff(WupDayOff) { Option = 1; }

WaterUptakeTempFunction(bool NotOrig, double WupCTe, double WupDTe,  double WupDayOff) :d(WupDTe), dayoff(WupDayOff) {

	TrigTemp = WupCTe;
	Option = 2;
}
WaterUptakeTempFunction(bool NotOrig, double WupCTe, double WupDTe, double WupETe, double WupDayOff) :d(WupDTe), e(WupETe), dayoff(WupDayOff) {
	
	TrigTemp = WupCTe;
	Option = 3;
}
WaterUptakeTempFunction(vector<double> WupATe, vector<double> WupBTe, vector<double> WupDayOff) : v_a(WupATe), v_b(WupBTe), v_dayoff(WupDayOff) {
	Option = 1; }

WaterUptakeTempFunction(bool NotOrig, vector<double> WupCTe, vector<double> WupDTe, vector<double> WupDayOff) : v_TrigTemp(WupCTe), v_d(WupDTe), v_dayoff(WupDayOff) {
	
	Option = 2;
}
WaterUptakeTempFunction(bool NotOrig, vector<double> WupCTe, vector<double> WupDTe, vector<double> WupETe, vector<double> WupDayOff) : v_TrigTemp(WupCTe), v_d(WupDTe), v_e(WupETe),  v_dayoff(WupDayOff) {
	Option = 3;
}

double operator() () {
		return 1;
};
double operator() (double Temp, double DayNum) {
	switch (Option) {
		case 0:
		return 1;
		case 1:
			if(DayNum<=dayoff) return 1 - exp(-a*pow(max(0.00001, Temp ), b));
			else return 1.;
		case 2:
			if (DayNum <= dayoff) return 1 - exp(max(0.00001, Temp - TrigTemp)*log(0.02) / (d - TrigTemp));
			else return 1.;
		case 3:
			if (DayNum <= dayoff) return min(1., pow(max((Temp - TrigTemp), 0.00001) / (d - TrigTemp), e));
			else return 1.;
		default:
			return 1.;
	}
};
double operator() (size_t index, double Temp, double DayNum) {
	switch (Option) {
	case 0:
		return 1;
	case 1:
		if (DayNum <= v_dayoff[index]) return 1 - exp(-v_a[index] * pow(max(0.00001, Temp), v_b[index]));
		else return 1.;
	case 2:
		if (DayNum <= v_dayoff[index]) return 1 - exp(max(0.00001, Temp - v_TrigTemp[index]) * log(0.02) / (v_d[index] - v_TrigTemp[index]));
		else return 1.;
	case 3:
		if (DayNum <= v_dayoff[index]) return min(1., pow(max((Temp - v_TrigTemp[index]), 0.00001) / (v_d[index] - v_TrigTemp[index]), v_e[index]));
		else return 1.;
	default:
		return 1.;
	}
};


private:
	double TrigTemp;
	double a, b, c, d, e;
	double dayoff;
	vector<double> v_TrigTemp;
	vector<double> v_a, v_b, v_c, v_d, v_e;
	vector<double> v_dayoff;

	size_t Option;

};
class WaterUptakeMoistureFunction {
public:
	WaterUptakeMoistureFunction(vector<double> AirMinContent, vector<double> AirRedCoef, vector<double> WupCri, vector<double> WupF, 
		vector<double> WupFB, Hydraulic* HydPointer) :
		v_AirMinContent(AirMinContent), v_AirRedCoef(AirRedCoef), v_WupCri(WupCri), v_WupF(WupF), v_WupFB(WupFB), phyd(HydPointer)
	{
		v_ThetaWetCrit.resize(v_AirRedCoef.size());
		for (size_t i = 0; i < v_AirRedCoef.size(); ++i) {
			v_ThetaWetCrit[i].resize(phyd->NL);
			for (size_t j = 0; j < v_ThetaWetCrit[i].size(); j++)
				v_ThetaWetCrit[i][j] = phyd->M_Saturation[j] - v_AirMinContent[i];
		}
	}
	WaterUptakeMoistureFunction(double AirMinContent, double AirRedCoef, double WupCri, double WupF, double WupFB, Hydraulic *HydPointer) :
	AirMinContent(AirMinContent), AirRedCoef(AirRedCoef), WupCri(WupCri), WupF(WupF), WupFB(WupFB),phyd(HydPointer)
	{
		ThetaWetCrit.resize(phyd->NL);
		for (size_t i = 0; i<ThetaWetCrit.size(); i++)
			ThetaWetCrit[i] = phyd->M_Saturation[i] - AirMinContent;
		
	}
	WaterUptakeMoistureFunction(double AirMinContent, double AirRedCoef, double Wup, double WupF, double WupFB, double DripScale, Hydraulic* HydPointer) :
		AirMinContent(AirMinContent), AirRedCoef(AirRedCoef), WupCri(Wup), WupF(WupF), WupFB(WupFB), phyd(HydPointer)
	{
		ThetaWetCrit.resize(phyd->NL);
		for (size_t i = 0; i < ThetaWetCrit.size(); i++)
			ThetaWetCrit[i] = phyd->M_Saturation[i] - AirMinContent;
		WupCri*=DripScale;
	}
	double operator() (size_t index_plant, size_t index, double PsiValue, double PotWaterUptake, double ThetaValue) {
		if (ThetaValue > v_ThetaWetCrit[index_plant][index]) {
			if (v_AirMinContent[index_plant] < 0.01) return 1.;
			return pow(10., (-v_AirRedCoef[index_plant] * (ThetaValue - v_ThetaWetCrit[index_plant][index]) / v_AirMinContent[index_plant]));
		}
		else if (PsiValue > 1.5E4)
			return 0;
		else if (PsiValue <= v_WupCri[index_plant])
			return 1;
		else
			return pow(v_WupCri[index_plant] / PsiValue, PotWaterUptake * v_WupF[index_plant] + v_WupFB[index_plant]);
	}

	double operator() (size_t index, double PsiValue, double PotWaterUptake, double ThetaValue) {
		if (ThetaValue > ThetaWetCrit[index]) {
			if (AirMinContent < 0.01) return 1.;
			return pow(10., (-AirRedCoef*(ThetaValue - ThetaWetCrit[index]) / AirMinContent ));
		}
		else if (PsiValue>1.5E4)
			return 0;
		else if (PsiValue <= WupCri)
			return 1;
		else
			return pow(WupCri / PsiValue, PotWaterUptake*WupF + WupFB);
	}

private:

	vector<double> ThetaWetCrit; 
	double WupCri, WupF, WupFB, AirMinContent, AirRedCoef;
	vector<double> v_WupCri, v_WupF, v_WupFB, v_AirMinContent, v_AirRedCoef;
	vector<vector<double>> v_ThetaWetCrit;
	Hydraulic *phyd;
};
class WaterUptakeSaltFunction {
public:
	WaterUptakeSaltFunction( vector<double> SaltPowerCoef, vector<double> SaltHalfReduction ) :
		SaltHalfReduction(SaltHalfReduction), SaltPowerCoef(SaltPowerCoef) {}
	double operator() ( size_t IndexPlant, double PsiValue) {
		return max(0., 1. / (1. + pow(abs(PsiValue) / SaltHalfReduction[IndexPlant], SaltPowerCoef[IndexPlant])));
	}
private:
	vector<double> SaltHalfReduction, SaltPowerCoef;
};
class WaterUptakeDistance_FromDensity {
public: WaterUptakeDistance_FromDensity( double RootDistMin, double RootDistMax, double RootDensitityCoef ):
	RootDistMin(RootDistMin), RootDistMax(RootDistMax), RootDensityCoef(RootDensitityCoef)
{}
		double operator() (double Density) {
			//!	The density is root length cm per cm3 of soil.
			//!	The function return a value in m which is the distance from root surface to nearby bulk soil (rhizosfere)

			return RootDistMin + (RootDistMax - RootDistMin)*exp(-RootDensityCoef*Density);
}

private:
	double RootDistMin, RootDistMax, RootDensityCoef;
};
class PlantResistance {
public: PlantResistance(double RootResistivity, double XylemResistivity):
	RootResistivity(RootResistivity), XylemResistivity(XylemResistivity) {}
	double operator() (double PlantHeight, double RootLength) {	return PlantHeight*XylemResistivity + RootResistivity / RootLength;}

private:
	double RootResistivity, XylemResistivity;
};
class MaxWaterStorage {
public:
	MaxWaterStorage(double PlantWatRelLAI) :
		PlantWatRelLAI(PlantWatRelLAI) {}
	MaxWaterStorage(double PlantWatRelLAI, double PlantWatRelLAI_Height) :
		PlantWatRelLAI(PlantWatRelLAI), PlantWatRelLAI_Height(PlantWatRelLAI_Height) {}
	double operator() (double LAI) {return LAI*PlantWatRelLAI;}
	double operator() (double LAI, double PlantHeight) { return LAI*PlantHeight*PlantWatRelLAI_Height; }


private:
	double PlantWatRelLAI, PlantWatRelLAI_Height;

};
class RelativeStomataClosure {
public: RelativeStomataClosure(double PlantMaxSuction, double LeafThresholdSuction) :
	PlantMaxSuction(PlantMaxSuction), LeafThresholdSuction(LeafThresholdSuction) {}
	double operator() (double PlantWaterPotential) {
			return max(0., min(1., (PlantWaterPotential - PlantMaxSuction) / (LeafThresholdSuction - PlantMaxSuction)));
	};
private:
	double PlantMaxSuction, LeafThresholdSuction;
};
