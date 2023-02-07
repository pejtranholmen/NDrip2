#pragma once
#include "../../../../std.h"

class LeafLitterFall_LAI {
public:
	LeafLitterFall_LAI(vector<double> LAI_Enh_Coef) : Coef(LAI_Enh_Coef) {};
	double operator() (size_t index, double LAI) {
		return exp(LAI*Coef[index]);
	}
private:
	vector<double> Coef;
};
class LitterFall_Tsum {
public:
	LitterFall_Tsum(vector<double> Leaf_LitterRateC1, vector<double>Leaf_LitterRateC2, vector<double> Leaf_TSum1, vector<double> Leaf_TSum2, vector<double> Dorm_FinalDayNo) 
		: RateCoef1(Leaf_LitterRateC1), RateCoef2(Leaf_LitterRateC2), TSum1(Leaf_TSum2), TSum2(Leaf_TSum2), Dorm_FinalDayNo(Dorm_FinalDayNo) { StaticFunc = false; };
	LitterFall_Tsum(vector<double> Leaf_LitterRateC1) : RateCoef1(Leaf_LitterRateC1) { StaticFunc = true; };

	double operator() (size_t index) {
		return RateCoef1[index];
	}
	double operator() (size_t index, double TSum, double DayNo) {
		if (StaticFunc) return RateCoef1[index];
		else {
			if (DayNo < Dorm_FinalDayNo[index]) {
				return RateCoef1[index] +
					(RateCoef2[index] - RateCoef1[index])*min(1., max(0., TSum - TSum1[index]) / max(1., TSum2[index] - TSum1[index]));
			}
			else
				return RateCoef2[index];
		}
	}
private:
	vector<double> RateCoef1, RateCoef2,TSum1,TSum2, Dorm_FinalDayNo;
	bool StaticFunc;
};
class LitterFallYear {

public://8
	LitterFallYear(vector<double> Leaf_LitterRateC1, vector<double> Leaf_LitterRateC2, vector<double> Leaf_TSum1,  vector<double> Leaf_TSum2, vector<double> Dorm_FinalDayNo, vector<double> LAI_Enh_Coef,
		vector<double> ScaleNew, vector<double> ScaleOld) :ScaleNew(ScaleNew), ScaleOld(ScaleOld)  { f_TSum = LitterFall_Tsum(Leaf_LitterRateC1, Leaf_LitterRateC2, Leaf_TSum1, Leaf_TSum2, Dorm_FinalDayNo);
	f_LAI = LeafLitterFall_LAI(LAI_Enh_Coef); StaticOption = false;
	}//7
	LitterFallYear(vector<double> Leaf_LitterRateC1, vector<double> Leaf_LitterRateC2, vector<double> Leaf_TSum1, vector<double> Leaf_TSum2, vector<double> Dorm_FinalDayNo, 
		vector<double> ScaleNew, vector<double> ScaleOld) :ScaleNew(ScaleNew), ScaleOld(ScaleOld) {
		f_TSum = LitterFall_Tsum(Leaf_LitterRateC1, Leaf_LitterRateC2, Leaf_TSum1, Leaf_TSum2, Dorm_FinalDayNo);
		 StaticOption = false;
	}//5
	LitterFallYear(vector<double> Leaf_LitterRateC1, vector<double> Leaf_LitterRateC2, vector<double> Leaf_TSum1, vector<double> Leaf_TSum2, vector<double> Dorm_FinalDayNo)  {
		f_TSum = LitterFall_Tsum(Leaf_LitterRateC1, Leaf_LitterRateC2, Leaf_TSum1, Leaf_TSum2, Dorm_FinalDayNo);
		StaticOption = false;
	}
	//4
	LitterFallYear(vector<double> Leaf_LitterRateC1, vector<double> LAI_Enh_Coef, vector<double> ScaleNew, vector<double> ScaleOld): ScaleNew(ScaleNew), ScaleOld(ScaleOld) {
		f_Static = LitterFall_Tsum(Leaf_LitterRateC1); f_LAI = LeafLitterFall_LAI(LAI_Enh_Coef);
		StaticOption = true;
	}//3
	LitterFallYear(vector<double> Leaf_LitterRateC1, vector<double> ScaleNew, vector<double> ScaleOld) : ScaleNew(ScaleNew), ScaleOld(ScaleOld) {
		f_Static = LitterFall_Tsum(Leaf_LitterRateC1);
		StaticOption = true;
	}//1
	LitterFallYear(vector<double> Leaf_LitterRateC1)  {
		f_Static = LitterFall_Tsum(Leaf_LitterRateC1);
		StaticOption = true;
	}

	double operator() (bool current_year, size_t index, double TSum, double DayNo, double LAI, double Mass) {
		if (current_year) {
			if(StaticOption)
				return f_LAI(index, LAI)*f_Static(index)*ScaleNew[index]*Mass;
			else
				return f_LAI(index, LAI)*f_TSum(index, TSum, DayNo)*ScaleNew[index] * Mass;

		}
		else {
			if(StaticOption)
				return f_LAI(index, LAI)*f_Static(index)*ScaleOld[index] * Mass;
			else	
				return f_LAI(index, LAI)*f_TSum(index, TSum, DayNo)*ScaleOld[index] * Mass;
		}
	}
	double operator() (bool current_year, size_t index, double TSum, double DayNo,  double Mass) {
		if (current_year) {
			if (StaticOption)
				return f_Static(index)*ScaleNew[index] * Mass;
			else
				return f_TSum(index, TSum, DayNo)*ScaleNew[index] * Mass;

		}
		else {
			if (StaticOption)
				return f_Static(index)*ScaleOld[index] * Mass;
			else
				return f_TSum(index, TSum, DayNo)*ScaleOld[index] * Mass;
		}
	}
	double operator() (size_t index, double TSum, double DayNo, double Mass) {
			if (StaticOption)
				return f_Static(index) * Mass;
			else
				return f_TSum(index, TSum, DayNo)* Mass;
	}

private:
	bool StaticOption;
	function <double(size_t)> f_Static;
	function <double(size_t, double)> f_LAI;
	function <double(size_t, double, double)> f_TSum;
	vector<double> ScaleNew, ScaleOld;
};




