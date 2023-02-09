#pragma once
#include <vector>
class PhoLeafResponse {
public: PhoLeafResponse(vector<double> RatioLeafOpt, vector<double> RatioLeafThres) : LeafThres(RatioLeafThres), LeafOpt(RatioLeafOpt) {}
		double operator() (size_t index, double RatioLeaf) {
		return max(0.001, min(1., (RatioLeaf - LeafThres[index]) / (LeafOpt[index] - LeafThres[index])));
}

private:
	vector<double> LeafThres, LeafOpt;
};
class PhotoResponsetoWaterAndNitrogen {
public:
	PhotoResponsetoWaterAndNitrogen(vector<double> CoefWater,  vector<double> MinResponse) :
		CoefWater(CoefWater),  MinRespons(MinResponse) {};
	double operator()(size_t index, double ResponseWater, double ResponseNitrogen) noexcept {
		return pow(CoefWater[index] * sqrt(ResponseWater) + (1 - CoefWater[index])*sqrt(ResponseNitrogen), 2) + MinRespons[index];
	};

private:
	vector<double> CoefWater,  MinRespons;

};

