#pragma once
#include "../../../std.h"
#include "../../../NewBase/PhysFunc.h"
#include "./InterCeption_Functors.h"

enum class CanopyHeightInput_Sw{ Parameters,Driving_variable,Simulated};
enum class PlantDev_Sw { Static,Start_f_TempSum };
enum class RootInput_Sw { Parameters, Driving_variable, Simulated };
enum class RootDistribution_Sw { Table,Linear,Constant,Exponential };
enum class LAIInput_Sw { Parameters, Driving_variable, Simulated };
enum class AlbedoVeg_Sw {Static, Parameters, Driving_variable, Simulated };
enum class SpatialPlantDist_Sw {Random_Within, Fixed_Within, Fixed_UnRestricted};
enum class SLA_Dynamic_Sw {off, on};
enum class AboveTable_Sw {No, Yes};
enum class MultiPlantWaterUptakeFunction_Sw{Common, Plant_Specific};



class PlantInterPolate {
	public:PlantInterPolate(vector<double> DayNum, vector<double> CForm): DayNum(DayNum), CForm(CForm)  {}
	TIME_INTP operator() (double day) {
		TIME_INTP out;
		out.weight = 0;
		out.index = 0;
		for (size_t i = 0; i < DayNum.size() - 1; i++)
			if (day >= DayNum[i] && day <= DayNum[i + 1]) {
				out.weight = pow((day - DayNum[i]) / (DayNum[i + 1] - DayNum[i]), CForm[i]);
				out.index = i;
				return out;
			}
		return out;
	}
private :
	vector<double> DayNum, CForm;
};
class PlantTableSeasonTimeFunction {
public:PlantTableSeasonTimeFunction(vector<double> DayNum, vector<double> CForm, vector<double> PlantV) : DayNum(DayNum), PlantV(PlantV) { f_Int = PlantInterPolate(DayNum, CForm); }
	   double operator() (double Day) {
		   if (Day <= DayNum.front())
			   return PlantV.front();
		   else if (Day >= DayNum.back())
			   return PlantV.back();
		   else {
			   TIME_INTP v=f_Int(Day);
			   return PlantV[v.index] + v.weight*(PlantV[v.index + 1] - PlantV[v.index]);
		   }
		}
private:
	vector<double> PlantV, DayNum;
	function<TIME_INTP(double)> f_Int;
};
class RootTimeVarFunction {
public: RootTimeVarFunction(vector<double> DayNumber, vector<double> RootVar) : DayNumber(DayNumber), RootVar(RootVar) {}
		double operator() (double Day) {
		double out = RootVar.back();
		if (Day <= DayNumber.front())
			out = RootVar.front();
		else if (Day <= DayNumber.back()) {
			for (size_t i = 0; i<RootVar.size() - 1; i++)
				if (Day>DayNumber[i] && Day <= DayNumber[i + 1]) {
					out = RootVar[i] + (Day - DayNumber[i]) / (DayNumber[i + 1] - DayNumber[i])
						*(RootVar[i + 1] - RootVar[i]);
					break;
				}
		}
		return out;
}
private:
	vector<double> DayNumber, RootVar;

};

class PlantHeight {
public: PlantHeight(vector<double> PlantMaxHeight, vector<double> PlantHeightC1, vector<double> PlantHeightC2, vector<double> PlantHeightC3, vector<double> PlantHeightC4)
	:Max(PlantMaxHeight), c1(PlantHeightC1), c2(PlantHeightC2), c3(PlantHeightC3), c4(PlantHeightC4) {}
		double operator() (size_t index, double MassAboveGround, double Age, double MassGrain) {
			MassAboveGround = max(0., min(1.E10, MassAboveGround));
			double out = Max[index] * (1 - exp(-c1[index] * MassAboveGround))*(1 - exp(-c2[index] * Age))
				*(c4[index] + (1 - c4[index])*exp(-c3[index] * MassGrain));
			return max(0.001, out);
		}

private:
	vector<double> Max, c1, c2, c3, c4;
};

class LeafAreaIndex {
public: LeafAreaIndex(vector<double> SpecificLeafAreaBegin, vector<double> SpecificLeafArea) :
	StartValue(SpecificLeafAreaBegin), NormalValue(SpecificLeafArea) {}
		double operator() (size_t index, double Mass, double GrowthStage) {
			double SLA;
			double W = max(0., min(1., GrowthStage - 1.));
			SLA = (1 - W)*StartValue[index] + W*NormalValue[index];
			return max(0., Mass / SLA);
		}
		double operator() (size_t index, double Mass) {
			return max(0., Mass / NormalValue[index]);
		}

private:
	vector<double> StartValue, NormalValue;
};

class PlantAlbedo {
public: PlantAlbedo(vector<double> AlbedoVegStage, vector<double> AlbedoGrainStage) :
AVeg(AlbedoVegStage), AGrain(AlbedoGrainStage) {}
		double operator() (size_t index, double GrowthStage) {
			double AWeight, out;

			if (GrowthStage<1 || GrowthStage>4.)
				out = 0.;
			else if (GrowthStage<2)
				out = AVeg[index];
			else {

				AWeight = min(1., GrowthStage - 2.);
				out = (1. - AWeight)*AVeg[index] + AWeight*AGrain[index];
			}
			return out;
		}
private:
	vector<double> AVeg, AGrain;
};
class RootLength {
public: RootLength(vector<double> SpecificRootLength) : s(SpecificRootLength) {}
	double operator() (size_t index, double MassRoots) {
		return MassRoots / s[index];
	}
private:
	vector<double> s;

};
class RootDepth {
public: RootDepth(vector<double> RootLowestDepth, vector<double> RootIncDepth) :
	Lowest(RootLowestDepth), Inc(RootIncDepth){}
	double operator() (size_t index, double MassRoots) {
			return Lowest[index] * MassRoots / (MassRoots + Lowest[index] / Inc[index]);
	}
	vector<double> Lowest, Inc;
};
class CanopyCover {
public:
	CanopyCover(vector<double> MaxPlantCover, vector<double> KExpPlantCover): mcov(MaxPlantCover), senslai(KExpPlantCover) {}
	double operator() (size_t index, double lai) {
		return mcov[index] * (1. - exp(-senslai[index] * lai));
	}
private:
	vector<double> mcov, senslai;

};
class RootDepthExpDistribution {
public:
	RootDepthExpDistribution(double RFracLow): RFracLow(RFracLow) {}
	double operator() (double Depth, double MaxDepth) {
		double deck;
		deck = log(RFracLow) / MaxDepth;
		return (1 / (1 - RFracLow))*(exp(deck*Depth) - RFracLow);
	}
private:
	double RFracLow;
};
