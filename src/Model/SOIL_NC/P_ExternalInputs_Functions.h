#pragma once
#include "./NC_ExternalInputs.h"

class P_ExternalInputs_Functions : public NC_ExternalInputs
{
	public:
		P_ExternalInputs_Functions(void);
		~P_ExternalInputs_Functions(void);
		bool Def();

		double	P_Fert, P_Fert_App, P_Deposition,Dep_P_DryRate;
		double P_Manure, P_Irrig;
		double P_Fert_Sol, P_FertDiss_k;
		vector <double> SoilPhosphate;

		vector<double> P_Fert_Rate;
		vector<P*> mTab_PFert;
		vector<double> Soil_PO4;
};

