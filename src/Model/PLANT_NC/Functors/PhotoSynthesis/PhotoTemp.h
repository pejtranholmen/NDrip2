#pragma once
#include "../../../../std.h"

class PhoTempResponseDirect {
public: PhoTempResponseDirect(vector<double> TempResMin, vector<double> TempResOpt1, vector<double> TempResOpt2, vector<double> TempResMax) : TempResMin(TempResMin), TempResOpt1(TempResOpt1),
	TempResOpt2(TempResOpt2), TempResMax(TempResMax) {}
		double operator() (size_t index, double TempAir) {
			double out;
			if (TempAir < TempResOpt1[index])
				out = max(0.001, min(1.,
				(TempAir - TempResMin[index]) / (TempResOpt1[index] - TempResMin[index])));
			else
				out = max(0., min(1.,
				(TempResMax[index] - TempAir) / (TempResMax[index] - TempResOpt2[index])));
			return out;
		}
private:
	vector<double> TempResMin, TempResOpt1, TempResOpt2, TempResMax;
};
class PhotoGreenIndexResponse {
public: PhotoGreenIndexResponse(vector<double> MinFracGreen, vector<double>OptGreenIndex) :MinFracGreenResponse(MinFracGreen), OptGreenIndex(OptGreenIndex) {}
		double operator() (size_t index, double GreenIndex) {
			return MinFracGreenResponse[index] + min(1. - MinFracGreenResponse[index], max(0., GreenIndex / OptGreenIndex[index]));
		}
private:
	vector<double> MinFracGreenResponse, OptGreenIndex;
};

class PhoTempSumResponse {
public:PhoTempSumResponse(vector<double> TempSumResponseStart, vector<double> TempSumResponseOpt) :TempSumResponseStart(TempSumResponseStart), TempSumResponseOpt(TempSumResponseOpt) {}

	   double operator() (size_t index, double GrowthTempSum) {
		   return TempSumResponseStart[index] + (1 - TempSumResponseStart[index])*min(1.,
			   GrowthTempSum / TempSumResponseOpt[index]);
	   }
private:
	vector<double> TempSumResponseStart, TempSumResponseOpt;

};

class PhotoTempResponse {
public:PhotoTempResponse(function<double(size_t, double)> f_TempSum,
	function<double(size_t, double)> f_TempResponseDirect, function<double(size_t, double)> f_GreenAreaResponse):
	f_TempSum(f_TempSum), f_TempResponseDirect(f_TempResponseDirect), f_GreenAreaResponse(f_GreenAreaResponse)
	{ };
	PhotoTempResponse(function<double(size_t, double)> f_TempSum, function<double(size_t, double)> f_TempResponseDirect):
	   f_TempSum(f_TempSum), f_TempResponseDirect(f_TempResponseDirect)
	  { };

  double operator() (size_t index, double Temp, double SumGrowth_Green) {
		   if (f_GreenAreaResponse)
			   return f_TempResponseDirect(index, Temp)*f_GreenAreaResponse(index, SumGrowth_Green);
		   else
			   return f_TempResponseDirect(index, Temp)*f_TempSum(index, SumGrowth_Green);
   }

private:
	vector<double> TempSumResponseStart, TempSumResponseOpt;
	function <double(size_t, double)> f_TempSum, f_TempResponseDirect, f_GreenAreaResponse;
};


