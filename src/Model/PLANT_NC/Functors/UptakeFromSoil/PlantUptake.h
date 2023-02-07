#pragma once
#include "../../../../std.h"
class RootDensity {
public:
	RootDensity(vector<double> LayerThickness) : LayerThickness(LayerThickness) {};
	vector<double> operator() (vector<double> &Dist, double &Root, double &OldRoot, vector<double> &Density) {//HH
		//g/m3
		for (size_t ii = 0; ii < Dist.size(); ii++)
			Density[ii] = Dist[ii] * (Root + OldRoot) / LayerThickness[ii];
		return Density;
	};
private:
	vector<double> LayerThickness;
};
struct LayerOutput
{
	bool all=true;
	double upt=0.;
};
class PlantDemand {
public:
	PlantDemand(vector<double> CN_RatioMinRoots, vector<double> CN_RatioMinCoarseRoots, vector<double> CN_RatioMinLeaf, vector<double> CN_RatioMinStem)
		: CN_RatioRoot(CN_RatioMinRoots), CN_CoarseRoot(CN_RatioMinCoarseRoots),  CN_Stem(CN_RatioMinStem), CN_Leaf(CN_RatioMinLeaf)  {

	};
	PlantDemand(vector<double> CN_RatioMinRoots, vector<double> CN_RatioMinCoarseRoots,  vector<double> CN_RatioMinStem,
	vector<double> CN_LeafThres, vector<double> CN_LeafOpt, vector<double> MinDegreeSupply)
		: CN_RatioRoot(CN_RatioMinRoots), CN_CoarseRoot(CN_RatioMinCoarseRoots), CN_Stem(CN_RatioMinStem) {

		CN_Leaf.resize(CN_LeafOpt.size());
		for (size_t i = 0; i < CN_LeafThres.size(); i++)
			CN_Leaf[i] = CN_LeafThres[i] + MinDegreeSupply[i] * (CN_LeafOpt[i] - CN_LeafThres[i]);

	};
	vector<double> operator() (vector<double> &A1, vector<double> &A2, vector<double> &A3, vector<double> &A4,
		vector<double> &R1, vector<double> &R2, vector<double> &R3, vector<double> &R4, vector<double> &demand) {
		for (size_t i = 0; i < A1.size(); i++) {
			demand[i] = max(0., (A1[i] - R1[i]) / CN_RatioRoot[i]) + max(0., (A2[i] - R2[i]) / CN_CoarseRoot[i]) +
				max(0., (A3[i] - R3[i]) / CN_Stem[i]) + max(0., (A4[i] - R4[i]) / CN_Leaf[i]);
		}
		return demand;	
	};
private:
	vector<double> CN_RatioRoot, CN_CoarseRoot, CN_Leaf, CN_Stem;
};
class FungiDemand {
public:
	FungiDemand(vector<double> CN_RatioMinFungi): CN_RatioMinFungi(CN_RatioMinFungi) {};

	vector<double> operator() (vector<double>&amount_C, vector<double> &amount_N, vector<double> &Demand) {
		for (size_t i = 0; i < amount_C.size(); i++)
			Demand[i] = max(0., amount_C[i] / CN_RatioMinFungi[i] - amount_N[i]);
		return Demand;
}
private:
	vector<double> CN_RatioMinFungi;
};
class FungiPotentialSoilUptake {
public:
	FungiPotentialSoilUptake(vector<double> MyceliaCoverFrac, vector<double> OrgCoef, vector<double> NO3Coef, vector<double> NH4Coef)
	{
		for (size_t i = 0; i < MyceliaCoverFrac.size(); i++) {
			NRateCoef[0].push_back(MyceliaCoverFrac[i] * OrgCoef[i]/Fix::BiomassToCarbon);
			NRateCoef[1].push_back(MyceliaCoverFrac[i] * NO3Coef[i]/Fix::BiomassToCarbon);
			NRateCoef[2].push_back(MyceliaCoverFrac[i] * NH4Coef[i]/Fix::BiomassToCarbon);
		};
	};
	vector<double> operator() (size_t iform, vector<double> &State, vector<double> &PotentialUptake) {
		for (size_t i = 0; i < State.size(); i++)
			PotentialUptake[i] = NRateCoef[iform][i] * State[i];
		return PotentialUptake;
	};
private:
	vector<double> NRateCoef[3];
};

class FungiPotentialSoilPUptake {
public:
	FungiPotentialSoilPUptake(vector<double> MyceliaCoverFrac, vector<double> OrgPCoef, vector<double> PO4Coef)
	{
		for (size_t i = 0; i < MyceliaCoverFrac.size(); i++) {
			PRateCoef[0].push_back(MyceliaCoverFrac[i] * OrgPCoef[i] / Fix::BiomassToCarbon);
			PRateCoef[1].push_back(MyceliaCoverFrac[i] * PO4Coef[i] / Fix::BiomassToCarbon);
			
		};
	};
	vector<double> operator() (size_t iform, vector<double> &State, vector<double> &PotentialPUptake) {
		for (size_t i = 0; i < State.size(); i++)
			PotentialPUptake[i] = PRateCoef[iform][i] * State[i];
		return PotentialPUptake;
	};
private:
	vector<double> PRateCoef[2];
};

class FungiCoverage{
public:
	FungiCoverage(vector<double> AlloFungiC1, vector<double> AreaCoverFungiCoef) {
		for (size_t i = 0; i < AlloFungiC1.size(); i++)
			Coef.push_back(AlloFungiC1[i] * AreaCoverFungiCoef[i]);
	};
vector<double> operator() (vector<double> &StateFungi, vector<double> &CRootState, vector<double> &COldRootState, vector<double> &FungiRootCover) {
	double AllRoot;
	for (size_t i = 0; i < StateFungi.size(); i++) {
		AllRoot = CRootState[i] + COldRootState[i];
		if (AllRoot<= 0.) FungiRootCover[i] = 0.;
		else FungiRootCover[i] = min(1.,Coef[i]*StateFungi[i]/AllRoot);
	};
	return FungiRootCover;
};


private:
	vector<double> Coef;

};
class FungiNitrogenExcess {
public:
	FungiNitrogenExcess(vector<double> CN_RatioMaxFungi) :CN_RatioMaxFungi(CN_RatioMaxFungi) {
	};
	vector<double> operator() (vector<double> &StateN, vector<double> &StateC, vector<double> &FungiAvail) {
	
		for (size_t i = 0; i < StateN.size(); i++) {
			FungiAvail[i] = max(0., StateN[i] - StateC[i] / CN_RatioMaxFungi[i]);
		};
		return FungiAvail;
	};


private:
	vector<double> CN_RatioMaxFungi;

};
class ImpactOnFungiAvailability {
public:
	ImpactOnFungiAvailability(double fungiMinCoef, double fungiNH4EnhCoef) :fungiMinCoef(fungiMinCoef) {
		fungiNH4Coef = fungiMinCoef * fungiNH4EnhCoef;
	};

	double operator() (bool NH4 ) {
		if(NH4)
			return fungiNH4Coef;
		else
			return fungiMinCoef;
	};

private:
	double fungiMinCoef, fungiNH4Coef;
};
class ImpactOnFungiPAvailability {
public:
	ImpactOnFungiPAvailability(double FungiPO4EnhanceCoef, double FungiMinEnhanceCoef) :FungiPO4EnhanceCoef(FungiPO4EnhanceCoef) {}
		double operator() (double fungiMinEnhanceCoef){
			return fungiMinEnhanceCoef * FungiPO4EnhanceCoef;
		};

private:
	double FungiPO4EnhanceCoef;
};

class DensityImpactOnPlantAvailability {
public:
	DensityImpactOnPlantAvailability(double NUptRootCoefMin) :DensityCoef(NUptRootCoefMin) {};
	double operator() (double RootDensity) {
		return (1. - exp(-DensityCoef * RootDensity));
	};

private:
	double DensityCoef;
};
class FungiCoverImpactOnPlantAvailability {
public:
	FungiCoverImpactOnPlantAvailability(double FungiCoverRedCoef) :CoverCoef(FungiCoverRedCoef) {};
	double operator() (double FungiRootCover) {
		return exp(-CoverCoef * FungiRootCover);
	};

private:
	double CoverCoef;
};
class FractionOfAvailability  {
public:
	FractionOfAvailability(double MaxAvailFrac) :Frac(MaxAvailFrac)	{};

	double operator() () {
		return Frac;
	};
	LayerOutput operator() (double demand, double supply) {
		if (demand < supply*Frac) {
			out.all = true;
			out.upt = demand;
		}
		else {
			out.all = false;
			out.upt = supply * Frac;
		}
		return out;
	};

private:
	double Frac;
	LayerOutput out;
};
struct UptakeResults {
	vector<double> UptNO3;
	vector<double> UptNH4;
	double RemainFrac=0.;
};
struct PUptakeResults {
	vector<double> UptPhosphate;
	double RemainFrac=0.;
};

class MineralUptakeByRootsAndFungi {
public:
	// 3 Constructors for definition with various combination of functions
	//add on for fungi without root density impact by HH
	MineralUptakeByRootsAndFungi(function<LayerOutput(double, double)> f_Avail, function<double(double)> f_Density, function<double(double)> f_FungiImpactOnPlant,
		function<double(bool)> f_ImpactonFungiMinUptake, size_t MaxLayers, double FracAvail, vector<double> FlexDegree):
		f_Avail(f_Avail), f_Density(f_Density), 
		f_FungiImpactOnPlant(f_FungiImpactOnPlant),
		f_ImpactonFungiByAmmonium(f_ImpactonFungiMinUptake), FracAvail(FracAvail), FlexDegree(FlexDegree)
	{
		 ScaledAvail.assign(MaxLayers, 1.0); Ratio.resize(MaxLayers);
	};
	MineralUptakeByRootsAndFungi(function<LayerOutput(double, double)> f_Avail, function<double(double)> f_Density, size_t MaxLayers, double FracAvail, vector<double> FlexDegree) :
		 FracAvail(FracAvail),f_Avail(f_Avail), f_Density(f_Density), FlexDegree(FlexDegree)
	{
		ScaledAvail.assign(MaxLayers, 1.0); Ratio.resize(MaxLayers); 
	};
	MineralUptakeByRootsAndFungi(function<LayerOutput(double, double)> f_Avail, size_t MaxLayers, double FracAvail, vector<double> FlexDegree) : 
		f_Avail(f_Avail), FracAvail(FracAvail), FlexDegree(FlexDegree) {
		ScaledAvail.assign(MaxLayers, 1.0); Ratio.resize(MaxLayers);
	};
	MineralUptakeByRootsAndFungi(function<LayerOutput(double, double)> f_Avail,  function<double(double)> f_FungiImpactOnPlant,
		function<double(bool)> f_ImpactonFungiMinUptake, size_t MaxLayers, double FracAvail, vector<double> FlexDegree) :
		f_Avail(f_Avail), 
		f_FungiImpactOnPlant(f_FungiImpactOnPlant),
		f_ImpactonFungiByAmmonium(f_ImpactonFungiMinUptake), FracAvail(FracAvail), FlexDegree(FlexDegree)
	{
		ScaledAvail.assign(MaxLayers, 1.0); Ratio.resize(MaxLayers);
	};
	// 4 operators for combination of RootDensity and Fungi Impacts on Plant Min uptake
	UptakeResults operator() (vector<double> &DistWithDepth,  size_t PlantIndex, double N_DemandPlant, double N_Tot_Demand,
		vector<double> Dist_Tot, vector<double> &SoilNitrate, vector<double> &SoilAmmonium, vector<double> &RootDensity, UptakeResults &out) {
		out.RemainFrac = 0.;
		for (size_t ii = 0; ii < out.UptNH4.size(); ii++) {
			Ratio[ii] = (SoilNitrate[ii] + 1.E-20) / (SoilNitrate[ii] + SoilAmmonium[ii] + 1.E-20);
			Ratio[ii] = max(0., min(1., Ratio[ii]));
			ScaledAvail[ii] = f_Density(RootDensity[ii]);
		}
		out=GetNitrate(DistWithDepth, N_DemandPlant, N_Tot_Demand, Dist_Tot, SoilNitrate, out);
		out=GetAmmonium(DistWithDepth, N_DemandPlant, N_Tot_Demand, Dist_Tot, SoilAmmonium, out);
		if (out.RemainFrac > 0.0001) {
			double ScaleComp = 1 / out.RemainFrac*FlexDegree[PlantIndex];
			out=GetAdditionalCompUptake(DistWithDepth, N_DemandPlant, N_Tot_Demand, Dist_Tot, SoilNitrate, SoilAmmonium, ScaleComp, out);
		}
		return out;
	}
	UptakeResults operator() (vector<double> &DistWithDepth,  size_t PlantIndex, double N_DemandPlant, double N_Tot_Demand,
		vector<double> Dist_Tot, vector<double> &SoilNitrate, vector<double> &SoilAmmonium, UptakeResults &out) {
		out.RemainFrac = 0.;

		if (N_DemandPlant<=0.) {
			out.UptNH4.assign(out.UptNH4.size(), 0.);
			out.UptNO3.assign(out.UptNO3.size(), 0.);
			out.RemainFrac = 0.;
			return out;
		}
		for (size_t ii = 0; ii < out.UptNO3.size(); ii++) {
			Ratio[ii] = (SoilNitrate[ii] + 1.E-20) / (SoilNitrate[ii] + SoilAmmonium[ii] + 1.E-20);
			Ratio[ii] = max(0., min(1., Ratio[ii]));
		}
		out=GetNitrate(DistWithDepth,  N_DemandPlant, N_Tot_Demand, Dist_Tot, SoilNitrate, out);
		out=GetAmmonium(DistWithDepth, N_DemandPlant, N_Tot_Demand, Dist_Tot, SoilAmmonium, out);
		if (out.RemainFrac > 0.0001) {
			double ScaleComp = 1 / out.RemainFrac*FlexDegree[PlantIndex];
			out=GetAdditionalCompUptake(DistWithDepth,  N_DemandPlant, N_Tot_Demand, Dist_Tot, SoilNitrate, SoilAmmonium, ScaleComp, out);
		}

		return out;
	}
	UptakeResults operator() (vector<double> &DistWithDepth,  size_t PlantIndex, double N_DemandPlant, double N_Tot_Demand,
		vector<double> Dist_Tot, vector<double> &SoilNitrate, vector<double> &SoilAmmonium, vector<double> &FungiRootCover, bool Fungi, UptakeResults &out) {//HH

		out.RemainFrac = 0.; 
		for (size_t ii = 0; ii < out.UptNO3.size(); ii++) {
			Ratio[ii] = (SoilNitrate[ii] + 1.E-20) / (SoilNitrate[ii] + SoilAmmonium[ii] + 1.E-20);
			Ratio[ii] = max(0., min(1., Ratio[ii]));
			double ScaledFungi = f_FungiImpactOnPlant(FungiRootCover[PlantIndex]);//*f_ImpactonFungiMinUptake;//HH
			ScaledAvail.assign(ScaledAvail.size(), ScaledFungi);//HH
		}
		out=GetNitrate(DistWithDepth, N_DemandPlant, N_Tot_Demand, Dist_Tot, SoilNitrate, out);
		out=GetAmmonium(DistWithDepth, N_DemandPlant, N_Tot_Demand, Dist_Tot, SoilAmmonium, out);
		if (out.RemainFrac > 0.0001) {
			double ScaleComp = 1 / out.RemainFrac*FlexDegree[PlantIndex];
			out=GetAdditionalCompUptake(DistWithDepth,  N_DemandPlant, N_Tot_Demand, Dist_Tot, SoilNitrate, SoilAmmonium, ScaleComp, out);
		}
		return out;
	}
	UptakeResults operator() (vector<double> &DistWithDepth, size_t PlantIndex, double N_DemandPlant, double N_Tot_Demand,
		vector<double> Dist_Tot, vector<double> &SoilNitrate, vector<double> &SoilAmmonium, vector<double> &RootDensity, vector<double> &FungiRootCover, bool Fungi, UptakeResults &out) {
		out.RemainFrac = 0.;
		for (size_t ii = 0; ii < out.UptNO3.size(); ii++) {
			Ratio[ii] = (SoilNitrate[ii] + 1.E-20) / (SoilNitrate[ii] + SoilAmmonium[ii] + 1.E-20);
			Ratio[ii] = max(0., min(1., Ratio[ii]));
			ScaledAvail[ii] = f_Density(RootDensity[ii])*f_FungiImpactOnPlant(FungiRootCover[PlantIndex]);
		}
		out=GetNitrate(DistWithDepth,  N_DemandPlant, N_Tot_Demand, Dist_Tot, SoilNitrate, out);
		out=GetAmmonium(DistWithDepth,  N_DemandPlant, N_Tot_Demand, Dist_Tot, SoilAmmonium, out);
		if (out.RemainFrac > 0.0001) {
			double ScaleComp = 1 / out.RemainFrac*FlexDegree[PlantIndex];
			out=GetAdditionalCompUptake(DistWithDepth,  N_DemandPlant, N_Tot_Demand, Dist_Tot, SoilNitrate, SoilAmmonium, ScaleComp, out);
		}
		return out;
	}
	// 1 operator for Compensatory Uptake by roots
	UptakeResults operator() (vector<double> &DistWithDepth, size_t NumLayers,  double N_DemandPlant, double N_Tot_Demand,
		vector<double> Dist_Tot, vector<double> &SoilNitrate, vector<double> &SoilAmmonium, UptakeResults &uptprev, double ScaleComp, UptakeResults &out) {
		for (size_t ii = 0; ii < out.UptNO3.size(); ii++) {
			Ratio[ii] = (SoilNitrate[ii] + 1.E-20) / (SoilNitrate[ii] + SoilAmmonium[ii] + 1.E-20);
			Ratio[ii] = max(0., min(1., Ratio[ii]));
		}
		out = uptprev;
		return GetAdditionalCompUptake(DistWithDepth,  N_DemandPlant, N_Tot_Demand, Dist_Tot, SoilNitrate, SoilAmmonium, ScaleComp, out);
	}


private:
	UptakeResults GetNitrate(vector<double> &DistWithDepth, double N_DemandPlant, double N_Tot_Demand,
		vector<double> &Dist_Tot, vector<double> &SoilNitrate, UptakeResults &out) {
		for (size_t ii = 0; ii < out.UptNO3.size(); ii++) {

				double Demand = Ratio[ii] * Dist_Tot[ii] * N_Tot_Demand;
				// Nitrate Uptake of actual Plant in actual Layer: 
				auto s = f_Avail(Demand, SoilNitrate[ii] * ScaledAvail[ii]);
				if (s.all) {
					out.UptNO3[ii] = s.upt;
					out.RemainFrac += DistWithDepth[ii] * Ratio[ii];
				}
				else if (SoilNitrate[ii] > 0.)
					out.UptNO3[ii] = s.upt;
		}
		return out;
	};
	UptakeResults GetAdditionalCompUptake(vector<double> &DistWithDepth, double N_DemandPlant, double N_Tot_Demand,
		vector<double> &Dist_Tot, vector<double> &SoilNitrate, vector<double> &SoilAmmonium, double ScaleComp, UptakeResults &out) {
		for (size_t ii = 0; ii < out.UptNH4.size(); ii++) {
			if (Dist_Tot[ii] * N_Tot_Demand > 0.) {
				double Demand = Ratio[ii] * Dist_Tot[ii] * N_Tot_Demand;
				// Nitrate Uptake of actual Plant in actual Layer: 
				auto s = f_Avail(Demand, SoilNitrate[ii] * ScaledAvail[ii]);
				if (s.all) {
					out.UptNO3[ii]+= max(0., min(s.upt*ScaleComp, FracAvail*SoilNitrate[ii]));
				}
				Demand = (1-Ratio[ii]) * Dist_Tot[ii] * N_Tot_Demand;
				// Nitrate Uptake of actual Plant in actual Layer: 
				s = f_Avail(Demand, SoilAmmonium[ii]);
				if (s.all) {
					out.UptNH4[ii] += max(0., min(s.upt*ScaleComp, FracAvail*SoilAmmonium[ii]));
				}

			}

		};
		return out;
	};
	UptakeResults GetAmmonium(vector<double> &DistWithDepth,  double N_DemandPlant, double N_Tot_Demand,
		vector<double> &Dist_Tot, vector<double> &SoilAmmonium, UptakeResults &out) {
		double Demand;

		for (size_t ii = 0; ii < out.UptNH4.size(); ii++) {
			Demand = (1-Ratio[ii]) * Dist_Tot[ii] * N_Tot_Demand;
			auto s = f_Avail(Demand, SoilAmmonium[ii] * ScaledAvail[ii]);
			if (s.all) {
				out.UptNH4[ii] = s.upt;
				out.RemainFrac += DistWithDepth[ii] * (1 - Ratio[ii]);
			}
			else if (SoilAmmonium[ii] > 0.)
				out.UptNH4[ii] = s.upt;
		}
		return out;
	};

	function<LayerOutput(double, double)> f_Avail;
	function<double(double)> f_Density;
	function<double(double)> f_FungiImpactOnPlant;
	function<double(bool)> f_ImpactonFungiByAmmonium;
	vector<double> ScaledAvail, Ratio;
	double FracAvail;
	vector<double> FlexDegree;
};
class PMineralUptakeByRootsAndFungi {
public:
	// 3 Constructors for definition with various combination of functions + 1 by HH
	PMineralUptakeByRootsAndFungi(function<LayerOutput(double, double)> f_AvailP, function<double(double)> f_Density, function<double(double)> f_FungiImpactOnPlant,
		function<double(double)> f_ImpactOnFungiPhosphateUptake, size_t MaxLayers, double FracAvail, vector<double> FlexDegree) :
		f_AvailP(f_AvailP), f_Density(f_Density),
		f_FungiImpactOnPlant(f_FungiImpactOnPlant),
		f_ImpactOnFungiPhosphateUptake(f_ImpactOnFungiPhosphateUptake), FracAvail(FracAvail), FlexDegree(FlexDegree)
	{
		ScaledAvail.assign(MaxLayers, 1.0);
	};
	PMineralUptakeByRootsAndFungi(function<LayerOutput(double, double)> f_AvailP, function<double(double)> f_Density, size_t MaxLayers, double FracAvail, vector<double> FlexDegree) :
		FracAvail(FracAvail), f_AvailP(f_AvailP), f_Density(f_Density), FlexDegree(FlexDegree)
	{
		ScaledAvail.assign(MaxLayers, 1.0); 
	};
	PMineralUptakeByRootsAndFungi(function<LayerOutput(double, double)> f_AvailP, size_t MaxLayers, double FracAvail, vector<double> FlexDegree) : 
		f_AvailP(f_AvailP), FracAvail(FracAvail), FlexDegree(FlexDegree) {
		ScaledAvail.assign(MaxLayers, 1.0); 
	};
	PMineralUptakeByRootsAndFungi(function<LayerOutput(double, double)> f_AvailP,  function<double(double)> f_FungiImpactOnPlant,
		function<double(double)> f_ImpactOnFungiPhosphateUptake, size_t MaxLayers, double FracAvail, vector<double> FlexDegree) :
		f_AvailP(f_AvailP), 
		f_FungiImpactOnPlant(f_FungiImpactOnPlant),
		f_ImpactOnFungiPhosphateUptake(f_ImpactOnFungiPhosphateUptake), FracAvail(FracAvail), FlexDegree(FlexDegree)
	{
		ScaledAvail.assign(MaxLayers, 1.0);
	};
	// 4 operators for combination of RootDensity and Fungi Impacts on Plant Min uptake
	PUptakeResults operator() (vector<double> &DistWithDepth, size_t PlantIndex, double N_DemandPlant, double N_Tot_Demand,
		vector<double> Dist_Tot, vector<double> &Phosphate, vector<double> &RootDensity, PUptakeResults &out) {
		out.RemainFrac = 0.;
		for (size_t ii = 0; ii < out.UptPhosphate.size(); ii++) {
	
			ScaledAvail[ii] = f_Density(RootDensity[ii]);
		}
		out=GetPhosphate(DistWithDepth,  N_DemandPlant, N_Tot_Demand, Dist_Tot, Phosphate, out);
		if (out.RemainFrac > 0.0001) {
			double ScaleComp = 1 / out.RemainFrac*FlexDegree[PlantIndex];
			out=GetAdditionalCompUptake(DistWithDepth,  N_DemandPlant, N_Tot_Demand, Dist_Tot, Phosphate, ScaleComp, out);
		}
		return out;
	}
	PUptakeResults operator() (vector<double> &DistWithDepth,  size_t PlantIndex, double DemandPlant, double Tot_Demand,
		vector<double> &Dist_Tot, vector<double> &Phosphate, PUptakeResults &out) {
		
		if (DemandPlant <= 0.) {
			out.UptPhosphate.assign(out.UptPhosphate.size(), 0.);
			out.RemainFrac = 0.;
			return out;
		}
		out.RemainFrac = 0.;

		out=GetPhosphate(DistWithDepth,  DemandPlant, Tot_Demand, Dist_Tot, Phosphate, out);
		if (out.RemainFrac > 0.0001) {
			double ScaleComp = 1 / out.RemainFrac*FlexDegree[PlantIndex];
			out=GetAdditionalCompUptake(DistWithDepth,  DemandPlant, Tot_Demand, Dist_Tot, Phosphate, ScaleComp, out);
		}

		return out;
	}
	PUptakeResults operator() (vector<double> &DistWithDepth,  size_t PlantIndex, double DemandPlant, double Tot_Demand,
		vector<double> Dist_Tot,  vector<double> &Phosphate, vector<double> &FungiRootCover, bool Fungi, PUptakeResults &out) {
		if (DemandPlant <= 0.) {
			out.UptPhosphate.assign(out.UptPhosphate.size(), 0.);
			out.RemainFrac = 0.;
			return out;
		}
		out.RemainFrac = 0.;
		for (size_t ii = 0; ii < out.UptPhosphate.size(); ii++) {
			ScaledAvail[ii] = f_FungiImpactOnPlant(FungiRootCover[PlantIndex]);
		}
		out=GetPhosphate(DistWithDepth,  DemandPlant, Tot_Demand, Dist_Tot, Phosphate, out);
		if (out.RemainFrac > 0.0001) {
			double ScaleComp = 1 / out.RemainFrac*FlexDegree[PlantIndex];
			out=GetAdditionalCompUptake(DistWithDepth, DemandPlant, Tot_Demand, Dist_Tot, Phosphate, ScaleComp, out);

		}
		return out;
	}
	PUptakeResults operator() (vector<double> &DistWithDepth, size_t PlantIndex, double DemandPlant, double Tot_Demand,
		vector<double> Dist_Tot, vector<double> &Phosphate, vector<double> &RootDensity, vector<double> &FungiRootCover, bool Fungi, PUptakeResults &out) {
		out.RemainFrac = 0.;
		for (size_t ii = 0; ii < out.UptPhosphate.size(); ii++) {
			ScaledAvail[ii] = f_Density(RootDensity[ii])*f_FungiImpactOnPlant(FungiRootCover[PlantIndex]);
		}
		out=GetPhosphate(DistWithDepth,  DemandPlant, Tot_Demand, Dist_Tot, Phosphate, out);
		if (out.RemainFrac > 0.0001) {
			double ScaleComp = 1 / out.RemainFrac*FlexDegree[PlantIndex];
			out=GetAdditionalCompUptake(DistWithDepth, DemandPlant, Tot_Demand, Dist_Tot, Phosphate, ScaleComp, out);

		}

		return out;
	}
	// 1 operator for Compensatory Uptake by roots
	PUptakeResults operator() (vector<double> &DistWithDepth,  double DemandPlant, double Tot_Demand,
		vector<double> Dist_Tot, vector<double> &Phosphate,  PUptakeResults &uptprev, double ScaleComp, PUptakeResults &out) {

		out = uptprev;
		out=GetAdditionalCompUptake(DistWithDepth, DemandPlant, Tot_Demand, Dist_Tot, Phosphate, ScaleComp, out);
		return out;
	}


private:
	PUptakeResults GetPhosphate(vector<double> &DistWithDepth,  double DemandPlant, double Tot_Demand,
		vector<double> &Dist_Tot, vector<double> &Phosphate, PUptakeResults &out) {
		for (size_t ii = 0; ii < out.UptPhosphate.size(); ii++) {

			double Demand = Dist_Tot[ii] * Tot_Demand;
			// Nitrate Uptake of actual Plant in actual Layer: 
			auto s = f_AvailP(Demand, Phosphate[ii] * ScaledAvail[ii]);
			if (s.all) {
				out.UptPhosphate[ii] = s.upt;
				out.RemainFrac += DistWithDepth[ii];
			}
			else if (Phosphate[ii] > 0.)
				out.UptPhosphate[ii] = s.upt;
		}
		return out;
	};
	PUptakeResults GetAdditionalCompUptake(vector<double> &DistWithDepth, double N_DemandPlant, double N_Tot_Demand,
		vector<double> &Dist_Tot, vector<double> &Phosphate, double ScaleComp, PUptakeResults &out) {
		for (size_t ii = 0; ii < out.UptPhosphate.size(); ii++) {
			if (Dist_Tot[ii] * N_Tot_Demand > 0.) {
				double Demand =  Dist_Tot[ii] * N_Tot_Demand;
				// Nitrate Uptake of actual Plant in actual Layer: 
				auto s = f_AvailP(Demand, Phosphate[ii] * ScaledAvail[ii]);
				if (s.all) {
					out.UptPhosphate[ii] += max(0., min(s.upt*ScaleComp, FracAvail*Phosphate[ii]));
				}
				Demand =  Dist_Tot[ii] * N_Tot_Demand;
				// Nitrate Uptake of actual Plant in actual Layer: 
				s = f_AvailP(Demand, Phosphate[ii]);
				if (s.all) {
					out.UptPhosphate[ii] += max(0., min(s.upt*ScaleComp, FracAvail*Phosphate[ii]));
				}

			}

		};
		return out;
	};

	function<LayerOutput(double, double)> f_AvailP;
	function<double(double)> f_Density;
	function<double(double)> f_FungiImpactOnPlant;
	function<double(double)> f_ImpactOnFungiPhosphateUptake;
	vector<double> ScaledAvail,FlexDegree;
	double FracAvail;
};

struct OrganicUptakeResults {
	vector<double> Litter1;
	vector<double> Humus;

};
class OrganicUptakeByRootsAndFungi {
public:
	// 
	OrganicUptakeByRootsAndFungi(double UptOrgRateCoef_L, double UptOrgRateCoef_H, size_t MaxLayers):
		FracL1(UptOrgRateCoef_L) , FracH(UptOrgRateCoef_H)
	{
		ScaledAvail.resize(MaxLayers); Demand.resize(MaxLayers); Ratio.resize(MaxLayers); out.resize(MaxLayers);
		soil_zero_out.Humus = out; soil_zero_out.Litter1 = out;
	
	};
	OrganicUptakeByRootsAndFungi(double UptOrgRateCoef_L, double UptOrgRateCoef_H,
		double NUptRootCoefOrg,  size_t MaxLayers) :FracL1(UptOrgRateCoef_L), FracH(UptOrgRateCoef_H)
	{
		ScaledAvail.resize(MaxLayers);Demand.resize(MaxLayers); Ratio.resize(MaxLayers); out.resize(MaxLayers);
		soil_zero_out.Humus = out; soil_zero_out.Litter1 = out;
		f_Density = DensityImpactOnPlantAvailability(NUptRootCoefOrg);
	};
	OrganicUptakeByRootsAndFungi(bool Fungi, double UptOrgRateCoef_L, double UptOrgRateCoef_H,
		double NUptRootCoefOrg, double FungiOrgEnhanceCoef, size_t MaxLayers) :
		FracL1(UptOrgRateCoef_L), FracH(UptOrgRateCoef_H), EnhanceFungi(FungiOrgEnhanceCoef)
	{
		ScaledAvail.resize(MaxLayers); Demand.resize(MaxLayers); Ratio.resize(MaxLayers); out.resize(MaxLayers);
		soil_zero_out.Humus = out; soil_zero_out.Litter1 = out;
		f_Density = DensityImpactOnPlantAvailability(NUptRootCoefOrg);
	};
	OrganicUptakeByRootsAndFungi(bool Fungi, double UptOrgRateCoef_L, double UptOrgRateCoef_H,
		 double FungiOrgEnhanceCoef, size_t MaxLayers) :FracL1(UptOrgRateCoef_L), FracH(UptOrgRateCoef_H)
		,EnhanceFungi(FungiOrgEnhanceCoef)
	{
		ScaledAvail.resize(MaxLayers); Demand.resize(MaxLayers); Ratio.resize(MaxLayers); out.resize(MaxLayers);
		soil_zero_out.Humus = out; soil_zero_out.Litter1 = out;
	
	};

	
	// 4 operators for combination of RootDensity and Fungi Impacts on Plant Min uptake
	OrganicUptakeResults operator() (vector<double> &DistWithDepth, size_t NumLayers,  double N_DemandPlant,
		vector<double> Dist_Tot, vector<double> &Litter1, vector<double> &Humus, vector<double> &RootDensity) {
		for (size_t ii = 0; ii < NumLayers; ii++) {
			ScaledAvail[ii] =FracL1*f_Density(RootDensity[ii])*Litter1[ii]/(FracH*f_Density(RootDensity[ii])*Humus[ii]+ FracL1 * f_Density(RootDensity[ii])*Litter1[ii]);
			Demand[ii] = N_DemandPlant * DistWithDepth[ii] * ScaledAvail[ii];
		}
		soil_out.Litter1 = GetOrgUptake(NumLayers,FracL1, Litter1);
		for (size_t ii = 0; ii < NumLayers; ii++) {
			ScaledAvail[ii] = FracH * f_Density(RootDensity[ii])*Humus[ii] / (FracH*f_Density(RootDensity[ii])*Humus[ii] + FracL1 * f_Density(RootDensity[ii])*Litter1[ii]);
			Demand[ii] = N_DemandPlant * DistWithDepth[ii] * ScaledAvail[ii];
		}
		soil_out.Humus = GetOrgUptake(NumLayers,FracH, Humus);
		return soil_out;
	}
	OrganicUptakeResults operator() (vector<double> &DistWithDepth, size_t NumLayers, double N_DemandPlant,
		vector<double> Dist_Tot, vector<double> &Litter1, vector<double> &Humus) {
		if (N_DemandPlant <= 0.) {
			return soil_zero_out;
		}
		for (size_t ii = 0; ii < NumLayers; ii++) {
			ScaledAvail[ii] = FracL1 * Litter1[ii] / (FracH*Humus[ii] + FracL1 * Litter1[ii]);
			Demand[ii] = N_DemandPlant * DistWithDepth[ii] * ScaledAvail[ii];
		}
		soil_out.Litter1 = GetOrgUptake(NumLayers, FracL1, Litter1);
		for (size_t ii = 0; ii < NumLayers; ii++) {
			ScaledAvail[ii] = FracH * Humus[ii] / (FracH*Humus[ii] + FracL1 *Litter1[ii]);
			Demand[ii] = N_DemandPlant * DistWithDepth[ii] * ScaledAvail[ii];
		}
		soil_out.Humus = GetOrgUptake(NumLayers,FracH, Humus);
		return soil_out;
	}
	OrganicUptakeResults operator() (bool Fungi, vector<double> &DistWithDepth, size_t NumLayers, double N_DemandPlant,
		vector<double> Dist_Tot, vector<double> &Litter1, vector<double> &Humus, vector<double> &RootDensity) {
		for (size_t ii = 0; ii < NumLayers; ii++) {
			ScaledAvail[ii] = FracL1 * f_Density(RootDensity[ii])*Litter1[ii] / (FracH*f_Density(RootDensity[ii])*Humus[ii] + FracL1 * f_Density(RootDensity[ii])*Litter1[ii]);
			Demand[ii] = N_DemandPlant * DistWithDepth[ii] * ScaledAvail[ii];
		}
		soil_out.Litter1 = GetOrgUptake(NumLayers,FracL1*EnhanceFungi, Litter1);//HH add fungi enhance uptake*
		for (size_t ii = 0; ii < NumLayers; ii++) {
			ScaledAvail[ii] = FracH * f_Density(RootDensity[ii])*Humus[ii] / (FracH*f_Density(RootDensity[ii])*Humus[ii] + FracL1 * f_Density(RootDensity[ii])*Litter1[ii]);
			Demand[ii] = N_DemandPlant * DistWithDepth[ii] * ScaledAvail[ii];
		}
		soil_out.Humus = GetOrgUptake(NumLayers,FracH*EnhanceFungi, Humus);//HH
		return soil_out;
	}
	OrganicUptakeResults operator() (bool Fungi, vector<double> &DistWithDepth, size_t NumLayers, double N_DemandPlant,
		vector<double> Dist_Tot, vector<double> &Litter1, vector<double> &Humus) {
		for (size_t ii = 0; ii < NumLayers; ii++) {
			ScaledAvail[ii] = FracL1 * Litter1[ii] / (FracH*Humus[ii] + FracL1 * Litter1[ii]);
			Demand[ii] = N_DemandPlant * DistWithDepth[ii] * ScaledAvail[ii];
		}
		soil_out.Litter1 = GetOrgUptake(NumLayers, FracL1*EnhanceFungi, Litter1);//HH  
		for (size_t ii = 0; ii < NumLayers; ii++) {
			ScaledAvail[ii] = FracH * Humus[ii] / (FracH*Humus[ii] + FracL1 * Litter1[ii]);
			Demand[ii] = N_DemandPlant * DistWithDepth[ii] * ScaledAvail[ii];
		}
		soil_out.Humus = GetOrgUptake(NumLayers,FracH*EnhanceFungi, Humus);//HH
		return soil_out;
	}
	

private:
	vector<double> GetOrgUptake(size_t NumLayers, double Frac, vector<double> &SoilOrgN) {
		for (size_t ii = 0; ii < NumLayers; ii++)
			out[ii] = max(0., min(Demand[ii], Frac*ScaledAvail[ii] * SoilOrgN[ii]));
		return out;
	
	};

	vector<double> out;
	OrganicUptakeResults soil_out, soil_zero_out;
	function<LayerOutput(double, double)> f_Avail;
	function<double(double)> f_Density;
	function<double(double)> f_FungiImpactOnPlant;
	function<double(bool)> f_ImpactonFungiByAmmonium;
	vector<double> ScaledAvail, Ratio, Demand;
	double FracL1, FracH, EnhanceFungi=0.;
};

class NitrogenFixationbyDemand {
public:
	NitrogenFixationbyDemand(vector<double> Coef) : Coef(Coef) {};
	vector<double> operator() (vector<double> Demand) {
		for (size_t i = 0; i < Demand.size(); i++)
			Demand[i] *= Coef[i];
		return Demand;
	};


private:
	vector<double> Coef;
};
class NitrogenFixationResponseToMinNSoil {
public:
	NitrogenFixationResponseToMinNSoil(vector<double> MaxNFix, vector<double> MinNFix, vector<double> SensFix) :
		MaxNFix(MaxNFix), MinNFix(MinNFix), SensFix(SensFix) {};
	double operator() (size_t index, double conc) {
		return (MaxNFix[index] - MinNFix[index])*exp(SensFix[index] * conc) + MinNFix[index];
	};
private:
	vector<double> MaxNFix, MinNFix, SensFix;
};


class NitrogenFixationIndependentDemand {
public:
	NitrogenFixationIndependentDemand(
		function<double(size_t, double)> f_Theta, function<double(double)> f_Temp,
		function<double(size_t, double)> f_MinN, vector<double> NoduleRatio) :
		f_Theta(f_Theta), f_Temp(f_Temp), f_MinN(f_MinN),
		NoduleRatio(NoduleRatio) {};


	vector<double> operator() (vector<vector<double>> RDist, vector<double> &Nitrate, vector<double> &Ammonium, 
		vector<double> &Stem, vector<double> &OldStem, vector<double> &Leaf, vector<double> OldLeaf,
		vector<double> &Theta, vector<double> &TempSoil, vector<double> &NFixation) {
		for (size_t i = 0; i < NFixation.size(); i++) {
			double NFixProf = 0;
			double biomass=Stem[i]+OldStem[i]+Leaf[i]+OldLeaf[i];
			double ratio = biomass / NoduleRatio[i];
			for (size_t ii = 0; ii < TempSoil.size(); ii++)
				NFixProf += RDist[i][ii] * ratio*f_Theta(ii, Theta[ii])*f_Temp( TempSoil[ii])
				*f_MinN(i, Nitrate[ii] + Ammonium[ii]);
			NFixation[i] = NFixProf;
		}
		return NFixation;

	};
	vector<double> operator() (vector<vector<double>> RDist, vector<double> &Nitrate, vector<double> &Ammonium,
		vector<double> &Root, vector<double> &OldRoot,
		vector<double> &Theta, vector<double> &TempSoil, vector<double> &NFixation) {
		for (size_t i = 0; i < NFixation.size(); i++) {
			double NFixProf = 0;
			double biomass = Root[i] + OldRoot[i];
			double ratio = biomass / NoduleRatio[i];
			for (size_t ii = 0; ii < TempSoil.size(); ii++)
				NFixProf += RDist[i][ii] * ratio*f_Theta(ii, Theta[ii])*f_Temp( TempSoil[ii])
				*f_MinN(i, Nitrate[ii] + Ammonium[ii]);
			NFixation[i] = NFixProf;
		}
		return NFixation;

	};


private:
	vector<double> NoduleRatio;
	function<double(double)> f_Temp;
	function<double(size_t, double)> f_Theta,  f_MinN;
};
