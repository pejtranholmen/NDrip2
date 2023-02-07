#pragma once
#include "../../../../std.h"


class LogisticGrowth {
public:LogisticGrowth(vector<double> UptakeStart, vector<double> UptakeEnd, vector<double> UptakeACoef, vector<double> UptakeBCoef, vector<double> UptakeCCoef, vector<double> UptakeCNRatio, ModelInfo *pModelInfo):
	a(UptakeACoef), b(UptakeACoef), c(UptakeACoef), start(UptakeStart), end(UptakeEnd), pModelInfo(pModelInfo), c_n(UptakeCNRatio) {}
   double operator() (size_t index, double Day) {
		   double TimePeriod = Day - start[index];
		   if (TimePeriod>0.&&Day<end[index]) {
			   return c_n[index]*a[index] * (a[index] - b[index]) /b[index] * c[index] * exp(-TimePeriod*c[index])
				   / pow(1 + (a[index] - b[index]) / b[index] * exp(-c[index] * TimePeriod), 2);
		   }
		   else
			   return 0.;
}
   double operator() (size_t index) {
	   double  TimePeriod;
	   bool WithinPeriod;
	   double JDay = pModelInfo->JDayNum;

	   TimePeriod = JDay - start[index];
	   if (TimePeriod > 0 && JDay < end[index])
		   WithinPeriod = true;
	   else
		   WithinPeriod = false;
	   if (end[index] < start[index] && JDay < end[index] && TimePeriod <= 0.) {
		   TimePeriod = TimePeriod + 365.;
		   WithinPeriod = true;
	   }
	   else if (end[index]<start[index] && JDay>start[index] && TimePeriod > 0.)
		   WithinPeriod = true;

	   if (WithinPeriod) {
		   return c_n[index]*a[index] * (a[index] - b[index]) / b[index] * c[index] * exp(-TimePeriod*c[index])
			   / pow(1 + (a[index] - b[index]) / b[index]
				   * exp(-c[index] * TimePeriod), 2.);
	   }
	   else
		   return 0.;
   }

private:
	vector<double> a,b,c, start, end, c_n;
	ModelInfo *pModelInfo;

};



