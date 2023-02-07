#pragma once
#include "../../../../std.h"
#include "../../../../ModelTypes/SimB.h"
class CarbonRatio {
public:
	CarbonRatio(vector<double> Max_Plant_LifeTime)  {
		for (size_t i = 0; i < Max_Plant_LifeTime.size(); i++) {
			if (Max_Plant_LifeTime[i] > 1.) OldBioMass.push_back(true);
			else
				OldBioMass.push_back(false);
		}
	}
	vector<double> operator() (const vector<double> &MassCarbon, const vector<double> &MassCarbonOld, const vector<double> &Mass, const vector<double> &MassOld, vector<double> &MassRatio) {
		for (size_t i = 0; i < MassCarbon.size(); i++) {
				    if (OldBioMass[i]) {
						if ((Mass[i] + MassOld[i]) > 1.E-3) //for small P conditions HH
							MassRatio[i] = (MassCarbon[i] + MassCarbonOld[i]+1.E-5) / (Mass[i] + MassOld[i]+1.E-7);
						else
							MassRatio[i] = 1000.;//HH
					}
					else {
						if ((Mass[i]) > 1.E-3)
						MassRatio[i] = (MassCarbon[i] + 1.E-5) / (Mass[i] + 1.E-7);
						else
						MassRatio[i] = 1000.;
					}
		}
					
		return MassRatio;
	};
	vector<double> operator() (const vector<double> &MassCarbon,  const vector<double> &Mass,  vector<double> &MassRatio) {
		for (size_t i = 0; i < MassCarbon.size(); i++) {
			if ((Mass[i]) > 1.E-3)
				MassRatio[i] = (MassCarbon[i]+1.E-5) / (Mass[i]+1.E-7);
			else
				MassRatio[i] = 1000.;
		}
		return MassRatio;
	};
private:
	vector<bool> OldBioMass;
};

class CarbonRatioLeaf {
public:
	CarbonRatioLeaf(vector<double> Max_Plant_LifeTime , vector<double> CX_RatioMaxLeaf)   {
		DefaultLeafRatio.assign(CX_RatioMaxLeaf.size(), 100.);
		for (size_t i = 0; i < Max_Plant_LifeTime.size(); i++) {
			if (Max_Plant_LifeTime[i] > 1.) OldBioMass.push_back(true);
			else
				OldBioMass.push_back(false);
			DefaultLeafRatio[i]=CX_RatioMaxLeaf[i]*0.5;

		}
	}
	vector<double> operator() (const vector<double>& MassCarbon, const vector<double>& MassCarbonOld, const vector<double>& Mass, const vector<double>& MassOld, vector<double>& MassRatio) {
		for (size_t i = 0; i < MassCarbon.size(); i++) {
			if (OldBioMass[i]) {
				if ((Mass[i] + MassOld[i]) > 1.E-3) //for small P conditions HH
					MassRatio[i] = (MassCarbon[i] + MassCarbonOld[i] + 5.E-19) / (Mass[i] + MassOld[i] + 1.E-20);
				else
					MassRatio[i] = DefaultLeafRatio[i];
			}
			else {
				if ((Mass[i]) > 1.E-2)
					MassRatio[i] = (MassCarbon[i] + 5.E-19) / (Mass[i] + 1.E-20);
				else
					MassRatio[i] = DefaultLeafRatio[i];;
			}
		}

		return MassRatio;
	};
	vector<double> operator() (const vector<double>& MassCarbon, const vector<double>& Mass, vector<double>& MassRatio) {
		for (size_t i = 0; i < MassCarbon.size(); i++) {
			if ((Mass[i]) > 1.E-2)
					MassRatio[i] = (MassCarbon[i] + 5.E-19) / (Mass[i] + 1.E-20);
			else
					MassRatio[i] = DefaultLeafRatio[i];;
		}
		return MassRatio;
	};



private:
	vector<bool> OldBioMass; 
	vector<double> DefaultLeafRatio;

};
