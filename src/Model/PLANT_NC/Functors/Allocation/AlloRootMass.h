#pragma once
#include "../../../../ModelTypes/SimB.h"
#include "../../../../ModelTypes/FixedInteger_PlantComp.h"
struct AlloNewMobile {
	vector<double> Roots;
	vector<double> Leaf;
	vector<double> Stem;
	vector<double> CoarseRoots;
};

class CAlloRootMass_Fixed {
public:
	CAlloRootMass_Fixed(vector<double> AlloRootTotMass1):a(AlloRootTotMass1) {}
	double operator() (size_t index, double Mass=0) {
		return a[index];
}
private:
	vector <double> a;
};
class CAlloRootMass_Linear {
public:
	CAlloRootMass_Linear(vector<double> AlloRootTotMass1, vector<double> AlloRootTotMass2, double minimum) :a(AlloRootTotMass1), b(AlloRootTotMass2), minimum(minimum) {}
	double operator() (size_t index, double Mass) {
		return min(0.99, max(minimum, a[index]+b[index]*Mass));
	}
private:
	vector <double> a, b;
	double minimum;
};
class CAlloRootMass_Exp {
public:
	CAlloRootMass_Exp(vector<double> AlloRootTotMass1, vector<double> AlloRootTotMass2, vector<double> AlloRootTotMass3) :a(AlloRootTotMass1), b(AlloRootTotMass2), c(AlloRootTotMass3) {}
	double operator() (size_t index, double Mass) {
		return a[index]+b[index]*exp(c[index]*Mass);
	}
private:
	vector <double> a, b, c;
};
class CAlloRootWater_Fixed {
public:
	CAlloRootWater_Fixed(vector<double> AlloRootWater1) :a(AlloRootWater1) {}
	double operator() (size_t index, double EtRatio = 0) {
		return a[index];
	}
private:
	vector <double> a;
};
class CAlloRootWater_Linear {
public:
	CAlloRootWater_Linear(vector<double> AlloRootWater1, vector<double> AlloRootWater2) :a(AlloRootWater1), b(AlloRootWater2) {}
	double operator() (size_t index, double EtRatio) {
		return a[index] + b[index] * EtRatio;
	}
private:
	vector <double> a, b;
};
class CAlloRootWater_Exp {
public:
	CAlloRootWater_Exp(vector<double> AlloRootWater1, vector<double> AlloRootWater2, vector<double> AlloRootWater3) :a(AlloRootWater1), b(AlloRootWater2), c(AlloRootWater3) {}
	double operator() (size_t index, double EtRatio) {
		return a[index] + b[index] * (1-exp(c[index] * EtRatio));
	}
private:
	vector <double> a, b, c;
};
class CAlloRootNitrogen_Fixed {
public:
	CAlloRootNitrogen_Fixed(vector<double> AlloRoot_CN1) :a(AlloRoot_CN1) {}
	double operator() (size_t index, double N_Response = 0) {
		return a[index];
	}
private:
	vector <double> a;
};
class CAlloRootNitrogen_Linear {
public:
	CAlloRootNitrogen_Linear(vector<double> AlloRoot_CN1, vector<double> AlloRoot_CN2) :a(AlloRoot_CN1), b(AlloRoot_CN2) {}
	double operator() (size_t index, double N_Response) {
		return a[index] + b[index] * N_Response;
	}
private:
	vector <double> a, b;
};
class CAlloRootNitrogen_Exp {
public:
	CAlloRootNitrogen_Exp(vector<double> AlloRoot_CN1, vector<double> AlloRoot_CN2, vector<double> AlloRoot_CN3) :a(AlloRoot_CN1), b(AlloRoot_CN2), c(AlloRoot_CN3) {}
	double operator() (size_t index, double N_Response) {
		return a[index] + b[index] * (1.-exp(c[index] * N_Response));
	}
private:
	vector <double> a, b, c;
};
class CAlloRootPhosphorus_Fixed {
public:
	CAlloRootPhosphorus_Fixed(vector<double> AlloRoot_CP1) :a(AlloRoot_CP1) {}
	double operator() (size_t index, double P_Response = 0) {
		return a[index];
	}
private:
	vector <double> a;
};
class CAlloRootPhosphorus_Linear {
public:
	CAlloRootPhosphorus_Linear(vector<double> AlloRoot_CP1, vector<double> AlloRoot_CP2) :a(AlloRoot_CP1), b(AlloRoot_CP2) {}
	double operator() (size_t index, double P_Response) {
		return a[index] + b[index] * P_Response;
	}
private:
	vector <double> a, b;
};
class CAlloRootPhosphorus_Exp {
public:
	CAlloRootPhosphorus_Exp(vector<double> AlloRoot_CP1, vector<double> AlloRoot_CP2, vector<double> AlloRoot_CP3) :a(AlloRoot_CP1), b(AlloRoot_CP2), c(AlloRoot_CP3) {}
	double operator() (size_t index, double P_Response) {
		return a[index] + b[index] * exp(c[index] * P_Response);
	}
private:
	vector <double> a, b, c;
};

class AllocationToRoots {
public:AllocationToRoots(function<double(size_t, double)> f_Mass, function<double(size_t, double)> f_Nitrogen, function<double(size_t, double)> f_Phosphorus, function<double(size_t, double)> f_Water,
	vector<double> RateCoef, size_t option) :
	f_Mass(f_Mass), f_Nitrogen(f_Nitrogen), f_Phosphorus(f_Phosphorus), f_Water(f_Water), RateCoef(RateCoef), option(option) {};
	   double operator() (size_t index) {
		   return f_Mass(index, 0.);
	   };
	   vector<double> operator() (vector<double> &Leaf, vector<double>&Stem, vector<double> &Response_N, vector<double> &Response_P, vector<double> &Response_W, vector<double> &NewMobile, vector<double> &AlloToRoots) {
		   double MassResponse, NResponse, PResponse, WaterResponse, combined_response;
		   for (size_t index = 0; index < Leaf.size(); index++) {
			   MassResponse = max(0.001, f_Mass(index, Leaf[index] + Stem[index]));
			   NResponse = max(0.001, f_Nitrogen(index, Response_N[index]));
			   PResponse = max(0.001, f_Phosphorus(index, Response_P[index]));
			   WaterResponse = max(0.01, f_Water(index, Response_W[index]));

			   if (option==0)
				   combined_response = max(MassResponse, max(NResponse, max(PResponse, WaterResponse)));
			   else if (option==1)
				   combined_response = MassResponse * NResponse*WaterResponse*PResponse;
			   else
				   combined_response = (MassResponse + NResponse + PResponse + WaterResponse) / 4.;
			   
			   AlloToRoots[index] = combined_response * RateCoef[index]*NewMobile[index];
		   }
		   return AlloToRoots;
}
private:
	function<double(size_t, double)> f_Mass, f_Nitrogen, f_Phosphorus, f_Water;
	vector<double> RateCoef;
	size_t option;
};
class LeafResponseToMass {
public: LeafResponseToMass(vector<double> AlloLeaf1)
		: C1(AlloLeaf1) {
		option = 0;
	};
LeafResponseToMass(vector<double> AlloLeaf1, vector<double> AlloLeaf2)
		: C1(AlloLeaf1), C2(AlloLeaf2) {
		option = 1;
	};
 LeafResponseToMass(vector<double> AlloLeaf1, vector<double> AlloLeaf2, vector<double> AlloLeaf3)
		: C1(AlloLeaf1), C2(AlloLeaf2), C3(AlloLeaf3) {
		option = 2;
	};
	double operator() (size_t index, double mass) {
		if (option == 1)
			return C1[index] + C2[index] * (mass);
		else if (option == 2)
			return  C1[index] * exp(C3[index] * mass);
		else 
			return C1[index];
	};

private:
vector<double> C1, C2, C3;
size_t option;


};
class AllocationToLeaf {
public:AllocationToLeaf(vector<double> AlloLeaf1, vector<double> AlloLeaf2, vector<double> AlloLeaf3,
	vector<double> RateCoef, size_t option) :	RateCoef(RateCoef), option(option) {
	if (option == 0) f_mass = LeafResponseToMass(AlloLeaf1);
	else if (option == 1) f_mass = LeafResponseToMass(AlloLeaf1, AlloLeaf2);
	else f_mass = LeafResponseToMass(AlloLeaf1, AlloLeaf2, AlloLeaf3);
};
	   double operator() (size_t index) {
		   return f_mass(index, 0.);
	   };
   vector<double> operator() (vector<double> &Leaf, vector<double>&Stem, vector<double> &AlloToRoots, vector<double> &NewMobile, vector<double> &AlloToLeaf) {
		   double  combined_response;
		   for (size_t index = 0; index < Leaf.size(); index++) {
			   if (option == 0)
				   combined_response = f_mass(index, 0.);
			   else if (option == 1)
				   combined_response = f_mass(index, Leaf[index] + Stem[index]);
			   else if (option == 2)
				   combined_response = f_mass(index, Leaf[index] + Stem[index]); 
			   else
				   combined_response = (NewMobile[index] - AlloToRoots[index])*(f_mass(index,Leaf[index] + Stem[index]))/NewMobile[index];

			   double Frac = max(0., min(combined_response,1-AlloToRoots[index]/(RateCoef[index] * NewMobile[index])));
			   AlloToLeaf[index] = Frac * RateCoef[index] * NewMobile[index];
		   }
		   return AlloToLeaf;
   }
private:

	function<double(size_t, double)> f_mass;
	vector<double> RateCoef;
	size_t option;
};

class AllocationToStemCoarseRoot {
public:
	AllocationToStemCoarseRoot(bool CoarseRoot, vector<double> RateCoef, vector<double> FractionCoarseRoot) :
		RateCoef(RateCoef), Fraction(FractionCoarseRoot) {
		if (CoarseRoot != true)
			for (size_t i = 0; i < Fraction.size(); i++) Fraction[i] = 1. - Fraction[i];
	};
	vector<double> operator() (vector<double> &AlloRoot, vector<double>&AlloLeaf, vector<double> &NewMobile, vector<double> &AlloToTarget) {
		for (size_t i = 0; i < AlloToTarget.size(); i++)
			AlloToTarget[i] = max(0., (NewMobile[i] * RateCoef[i] - AlloRoot[i] - AlloLeaf[i])*Fraction[i]);
		return AlloToTarget;
	}
private:

	vector<double> RateCoef, Fraction;
};
class AlloNewMobile_To_All {
public:
	AlloNewMobile_To_All(function<double(size_t, double)> f_Mass, function<double(size_t, double)> f_Nitrogen, function<double(size_t, double)> f_Phosphorus, function<double(size_t, double)> f_Water,
		vector<double> RateCoef, size_t optionRoot, vector<double> AlloLeaf1, vector<double> AlloLeaf2, vector<double> AlloLeaf3, size_t optionLeaf,
		vector<double> FractionCoarseRoot) :
		f_Root_Mass(f_Mass), f_Root_Nitrogen(f_Nitrogen), f_Root_Phosphorus(f_Phosphorus), f_Root_Water(f_Water), RateCoef(RateCoef), OptionRoot(optionRoot), OptionLeaf(optionLeaf),
		FractionCoarseRoot(FractionCoarseRoot)
	
	{
		if (OptionLeaf == 0) f_Leaf_Mass = LeafResponseToMass(AlloLeaf1);
		else if (OptionLeaf == 1) f_Leaf_Mass = LeafResponseToMass(AlloLeaf1, AlloLeaf2);
		else f_Leaf_Mass = LeafResponseToMass(AlloLeaf1, AlloLeaf2, AlloLeaf3);
	
	
	};
	AlloNewMobile operator() (vector<double> &Leaf, vector<double>&Stem, vector<double> &Response_N, vector<double> &Response_P, vector<double> &Response_W, vector<double> &NewMobile, AlloNewMobile &allo) {
		double MassResponse, NResponse, PResponse, WaterResponse, combined_response_root, combined_response_leaf, Amount_To_Allocate;
		for (size_t index = 0; index < Leaf.size(); index++) {
			MassResponse = max(0.001, f_Root_Mass(index, Leaf[index] + Stem[index]));
			NResponse = max(0.001, f_Root_Nitrogen(index, Response_N[index]));
			WaterResponse = max(0.01, f_Root_Water(index, Response_W[index]));
			Amount_To_Allocate= RateCoef[index] * NewMobile[index];
			if (f_Root_Phosphorus) {
				PResponse = max(0.001, f_Root_Phosphorus(index, Response_P[index]));
				if (OptionRoot == 0)
					combined_response_root = max(MassResponse, max(NResponse, max(PResponse, WaterResponse)));
				else if (OptionRoot == 1)
					combined_response_root = MassResponse * NResponse * PResponse * WaterResponse;
				else
					combined_response_root = (MassResponse + NResponse + PResponse + WaterResponse) / 4.;
			}
			else {
				if (OptionRoot == 0)
					combined_response_root = max(MassResponse, max(NResponse,  WaterResponse));
				else if (OptionRoot == 1)
					combined_response_root = MassResponse * NResponse *  WaterResponse;
				else
					combined_response_root = (MassResponse + NResponse + WaterResponse) / 3.;
			}
			allo.Roots[index] = combined_response_root *Amount_To_Allocate;
			if (OptionLeaf == 0)
				combined_response_leaf = f_Leaf_Mass(index, 0.);
			else if (OptionLeaf == 1)
				combined_response_leaf = f_Leaf_Mass(index, Leaf[index] + Stem[index]);
			else if (OptionLeaf == 2)
				combined_response_leaf = f_Leaf_Mass(index, Leaf[index] + Stem[index]);
			else
				combined_response_leaf = (NewMobile[index] - allo.Roots[index])*(f_Leaf_Mass(index, Leaf[index] + Stem[index])) / NewMobile[index];

			double FracLeaf = max(0., min(combined_response_leaf, 1 - combined_response_root));
			allo.Leaf[index] = FracLeaf * Amount_To_Allocate;

			double FracStem = max(0., min(1 - combined_response_root-FracLeaf,1.));
			allo.Stem[index]= FracStem*(1-FractionCoarseRoot[index])*Amount_To_Allocate;
			allo.CoarseRoots[index]= FracStem * FractionCoarseRoot[index]* Amount_To_Allocate;
		};
		return allo;
	};
private:
	function<double(size_t, double)> f_Root_Mass, f_Root_Nitrogen, f_Root_Phosphorus, f_Root_Water;
	function<double(size_t, double)> f_Leaf_Mass;
	vector<double> RateCoef, FractionCoarseRoot;
	size_t OptionRoot, OptionLeaf;
};

class AlloUptake_To_All {
public:
	AlloUptake_To_All(vector<double> RatioMinRoot, vector<double> RatioMinStem, vector<double> RatioMinCoarseRoot) :
		RatioMinRoot(RatioMinRoot), RatioMinStem(RatioMinStem), RatioMinCoarseRoot(RatioMinCoarseRoot)
	{
		FixedSupply = false;
	};
	AlloUptake_To_All(vector<double> RatioMinRoot, vector<double> RatioMinStem, vector<double> RatioMinCoarseRoot, 
		vector<double> Ratio_LeafThres, vector<double> Ratio_LeafOpt, vector<double> MinDegreeSupply) :
		RatioMinRoot(RatioMinRoot), RatioMinStem(RatioMinStem), RatioMinCoarseRoot(RatioMinCoarseRoot)
	{
		RatioLeaf_FixSupply.resize(Ratio_LeafOpt.size());
		for (size_t i = 0; i < Ratio_LeafThres.size(); i++)
			RatioLeaf_FixSupply[i] = Ratio_LeafThres[i] + MinDegreeSupply[i] * (Ratio_LeafOpt[i] - Ratio_LeafThres[i]);
		FixedSupply = true;
	};
	AlloNewMobile operator() (vector<double> &RespLeaf, vector<double> &RespStem, vector<double> &RespRoot, vector<double> &RespCoarseRoot, vector<double> &NewMobile, AlloNewMobile allo ) {
		for (size_t i = 0; i < allo.Leaf.size(); i++) {
			double demandroot = (allo.Roots[i]-RespRoot[i]) / RatioMinRoot[i];
			double demandcoarseroot = (allo.CoarseRoots[i]-RespCoarseRoot[i]) /RatioMinCoarseRoot[i];
			double demandstem = (allo.Stem[i]-RespStem[i])/RatioMinStem[i];
				double Remain = NewMobile[i];
			allo.Roots[i] = max(0., min(Remain, demandroot)); Remain -= allo.Roots[i];

			allo.CoarseRoots[i] = max(0., min(Remain, demandcoarseroot)); Remain -= allo.CoarseRoots[i];
			allo.Stem[i] = max(0., min(Remain, demandstem)); Remain -= allo.Stem[i];
			if (FixedSupply)
				allo.Leaf[i] = (allo.Leaf[i]-RespLeaf[i]) / RatioLeaf_FixSupply[i];
			else
				allo.Leaf[i] = max(0.,Remain);
		}
		return allo;
	}
private:
	vector<double> RatioMinRoot, RatioMinStem, RatioMinCoarseRoot, RatioLeaf_FixSupply;
	bool FixedSupply;

};




class FungiPlantDemand {
public:
	FungiPlantDemand(vector<double> FungiMinSupplyCoef) : FungiMinSupplyCoef(FungiMinSupplyCoef) {};

	double operator() (size_t index, double NFungiPlantSupply, double NRootUptakeSupply, double PlantDemand) {
		double out;
		double   MinPlantSupply;
		MinPlantSupply = FungiMinSupplyCoef [index]* (NRootUptakeSupply + NFungiPlantSupply);

		if (NFungiPlantSupply>= MinPlantSupply)
			out = 1.;
		else
			out = max(0.0, NFungiPlantSupply / (NRootUptakeSupply+ NFungiPlantSupply));
		if (PlantDemand< 0.0)
			out = 1;
		return out;
}

private:
	vector<double> FungiMinSupplyCoef;
};


class NitrogenAvailableforFungi {
public:
	NitrogenAvailableforFungi(vector<double> NavailCoef) : coef(NavailCoef) {};
	double operator() (size_t index, double TotalMinSoil) {
		return pow(exp(-coef[index] * (pow(TotalMinSoil, 2))), 3);
	};
private:
	vector<double> coef;
};
class PhosphorusAvailableforFungi {
public:
	PhosphorusAvailableforFungi(vector<double> PavailCoef) : coef(PavailCoef) {};
	double operator() (size_t index, double TotalPhosphateSoil) {
		return pow(exp(-coef[index] * (pow(TotalPhosphateSoil, 2))), 3);
	};
private:
	vector<double> coef;
};

class AllocationToFungi {
public:
	AllocationToFungi(function<double(size_t, double)> f_Available, function<double(size_t, double, double, double)> f_Supply, vector<double> AlloMaxC1, vector<double> CAlloFungiC1) :
		f_Available(f_Available), f_Supply(f_Supply), AlloMaxC1(AlloMaxC1), CAlloFungiC1(CAlloFungiC1) {};

	vector<double> operator() (vector<double > &NO3, vector<double> &NH4,
		vector<double> &root, vector<double> &oldroot, vector<double> &fungi,
		vector<double> &NFungiPlant, vector<double> &NRootUptake, vector<double> &PlantDemand,vector<double> &AllotoRoot, vector<double> &CFungiAtm, vector<double> &AllotoFungi) {
		double demand, supply;
		double TotalMinSoil = 0;
		for (size_t i = 0; i < NO3.size(); i++) // loop over layers
			TotalMinSoil += NO3[i] + NH4[i];
		for (size_t i = 0; i < AllotoFungi.size(); i++) {
			demand = AllotoRoot[i] * AlloMaxC1[i] * f_Available(i, TotalMinSoil);
			supply = (((root[i] + oldroot[i])*CAlloFungiC1[i]) - fungi[i])*f_Supply(i, NFungiPlant[i], NRootUptake[i], PlantDemand[i]);
			AllotoFungi[i] = max(CFungiAtm[i], min(demand, supply));
		}
		return AllotoFungi;
	};

private:

	vector<double> AlloMaxC1, CAlloFungiC1;
	function<double(size_t, double)> f_Available;
	function<double(size_t, double, double, double)> f_Supply;
};

class AllocationToFungiwithP {
public:
    AllocationToFungiwithP(){};
    AllocationToFungiwithP(function<double(size_t, double)> f_Available, function<double(size_t, double)> f_AvailableP, function<double(size_t, double, double, double)> f_Supply, function<double(size_t, double, double, double)> f_SupplyP, vector<double> AlloMaxC1, vector<double> CAlloFungiC1) :
        f_Available(f_Available), f_AvailableP(f_AvailableP), f_Supply(f_Supply), f_SupplyP(f_SupplyP), AlloMaxC1(AlloMaxC1), CAlloFungiC1(CAlloFungiC1) {};

    vector<double> operator() (vector<double > &NO3, vector<double> &NH4, vector<double> &Soil_PO4,
        vector<double> &root, vector<double> &oldroot, vector<double> &fungi,
        vector<double> &NFungiPlant, vector<double> &NRootUptake, vector<double> &PlantDemandN,
        vector<double> &PFungiPlant, vector<double> &PRootUptake, vector<double> &PlantDemandP, vector<double> &AllotoRoot, vector<double> &CFungiAtm, vector<double> &AllotoFungi) {
        double demand, supply;
        double TotalMinSoil = 0;
        double TotalPhosphateSoil = 0.;
        for (size_t i = 0; i < NO3.size(); i++)  // loop over layers
            TotalMinSoil += NO3[i] + NH4[i];
        for (size_t i = 0; i < Soil_PO4.size(); i++)
            TotalPhosphateSoil += Soil_PO4[i];
        
        for (size_t i = 0; i < AllotoFungi.size(); i++) {
            demand = AllotoRoot[i] * AlloMaxC1[i] * f_AvailableP(i, TotalPhosphateSoil)*f_Available(i, TotalMinSoil);
            demand = AllotoRoot[i] * AlloMaxC1[i] * min(f_AvailableP(i, TotalPhosphateSoil), f_Available(i, TotalMinSoil));//Law of Minimum by HH
            supply = (((root[i] + oldroot[i])*CAlloFungiC1[i]) - fungi[i])*min(f_Supply(i, NFungiPlant[i], NRootUptake[i], PlantDemandN[i]), f_SupplyP(i, PFungiPlant[i], PRootUptake[i], PlantDemandP[i]));//HH
            AllotoFungi[i] = max(CFungiAtm[i], min(demand, supply));
        }
        return AllotoFungi;
    };

private:

    vector<double> AlloMaxC1, CAlloFungiC1;
    function<double(size_t, double)> f_Available, f_AvailableP;
    function<double(size_t, double, double, double)> f_Supply, f_SupplyP;
};

class FromRootToLeafAfterHarvest {
public:FromRootToLeafAfterHarvest(vector<double> AlloFromRootLeafCoef, vector<double> AlloFromRootLeafRatio) :
	Coef(AlloFromRootLeafCoef), Ratio(AlloFromRootLeafRatio) {};
	   double operator() (size_t index, double mass_root, double mass_leaf) {
		   if ((mass_root + 0.01) / (mass_leaf + 0.01) > Ratio[index])
			   return Coef[index] * (mass_root - mass_leaf * Ratio[index]);
		   else
			   return 0.;
}
	   vector<double> operator() (vector<bool> &HarvestDone, vector<double> &root, vector<double> &Leaf, vector<double> &AlloRootToLeaf) {
		   for (size_t i = 0; i < root.size(); i++) {
			   if (HarvestDone[i]&&(root[i] + 0.001) / (Leaf[i] + 0.001) > Ratio[i])
				   AlloRootToLeaf[i]= Coef[i] * (root[i] - Leaf[i] * Ratio[i]);
			   else
				   AlloRootToLeaf[i]=0.;
		   }
		   return AlloRootToLeaf;
	   }

private:
	vector<double> Coef, Ratio;

};

class GrainFillingSupply {
public:
    GrainFillingSupply() { range = 0.2; threshold = 3.; };
	GrainFillingSupply(vector<vector<double>> C_coef, vector<vector<double>> N_coef, vector<vector<double>> P_coef) :
		C_coef(C_coef), N_coef(N_coef), P_coef(P_coef) {
		range = 0.2; threshold = 3.;
	};
	vector<double> operator() (size_t P_Comp, size_t P_Elem,vector<double> &GSI, vector<double> &mass, vector<double> &flux) {
		
		for (size_t i = 0; i < mass.size(); i++) {
			if (GSI[i] <= 2.00001 || GSI[i]>3) flux[i] = 0;
			else {
				if (P_Elem == _C_) {
					switch (P_Comp) {
					case _LEAF:
						flux[i] = C_coef[0][i] * mass[i];
						break;
					case _STEM:
						flux[i] = C_coef[1][i] * mass[i];
						break;
					case _ROOT:
						flux[i] = C_coef[2][i] * mass[i];
						break;
					default:
						flux[i] = 0;
					}
				}		
				else if (P_Elem == _N_) {
					switch (P_Comp) {
					case _LEAF:
						flux[i] = N_coef[0][i] * mass[i];
						break;
					case _STEM:
						flux[i] = N_coef[1][i] * mass[i];
						break;
					case _ROOT:
						flux[i] = N_coef[2][i] * mass[i];
						break;
					default:
						flux[i] = 0;
					}
				}
				else if (P_Elem == _P_) {
					switch (P_Comp) {
					case _LEAF:
						flux[i] = P_coef[0][i] * mass[i];
						break;
					case _STEM:
						flux[i] = P_coef[1][i] * mass[i];
						break;
					case _ROOT:
						flux[i] = P_coef[2][i] * mass[i];
						break;
					default:
						flux[i] = 0;
					}
				}
			}
		}
		return flux;
}

private:
	vector<vector<double>> C_coef, N_coef, P_coef;
	double threshold, range;
};
class GrainFillingSupply_WithReduction {
public:
	GrainFillingSupply_WithReduction(vector<vector<double>> C_coef, vector<vector<double>> N_coef, vector<vector<double>> P_coef, vector<double> Range_Red) :
		Range_Red(Range_Red)
	{
        f_GrainFill = GrainFillingSupply(C_coef, N_coef, P_coef);
    };
	
	vector<double> operator() (size_t P_Comp, size_t P_Elem, vector<double>& GSI, vector<double>& mass, vector<double>& flux) {
		flux = f_GrainFill(P_Comp, P_Elem, GSI, mass, flux);
		for (size_t i = 0; i < flux.size(); ++i) {
			double scale = min(1., (3.- GSI[i]) / Range_Red[i]);
			flux[i] *= scale;
		}	
		return flux;
	}

private:
	vector<double> Range_Red;
	GrainFillingSupply f_GrainFill;
	
};
class ReAlloCarbonToStemFromLeaf {
public:
	ReAlloCarbonToStemFromLeaf(vector<double> Allo_C_LeafStem) :
		Coef(Allo_C_LeafStem) {};
	vector<double> operator() (vector<double> &GSI, vector<double> &mass, vector<double> &flux) {
		for (size_t i = 0; i < GSI.size(); i++) {
			if (GSI[i] < 4)
				flux[i] = 0.;
			else
				flux[i] = Coef[i] * mass[i];
		}
		return flux;
	};
private:
	vector<double> Coef;
};
class ReAlloNitrogenToStemFromLeaf {
public:
	ReAlloNitrogenToStemFromLeaf(vector<double> CN_RatioMinStem) :
		 Ratio(CN_RatioMinStem) {};
	vector<double> operator() (vector<double> &GSI, vector<double> &carbonflux, vector<double> &flux) {
		for (size_t i = 0; i < GSI.size(); i++) {
			if (GSI[i] < 3)
				flux[i] = 0.;
			else
				flux[i] = carbonflux[i]/Ratio[i];
		}
		return flux;
	};

private:
	vector<double>  Ratio;

};

class ReAlloPhosphorusToStemFromLeaf {
public:
	ReAlloPhosphorusToStemFromLeaf(vector<double> CP_RatioMinStem) :
		Ratio(CP_RatioMinStem) {};
	vector<double> operator() (vector<double> &GSI, vector<double> &carbonflux, vector<double> &flux) {
		for (size_t i = 0; i < GSI.size(); i++) {
			if (GSI[i] < 3)
				flux[i] = 0.;
			else
				flux[i] = carbonflux[i] / Ratio[i];
		}
		return flux;
	};

private:
	vector<double>  Ratio;

};


