#pragma once
#include "../../../../std.h"
enum class PlantComp { NEWMOBILE, LEAF, STEM,GRAIN,MOBILE, OLD_LEAF, OLD_STEM, ROOT, COARSEROOT, OLD_ROOT, OLD_COARSE_ROOT, FUNGI };




class FirstOrderFunc {
public:
	FirstOrderFunc(vector<double> Coef) : Coef(Coef) {}
	double operator() (size_t i, double Mass) { return Coef[i] * Mass; }
private:
	vector<double> Coef;
};
class newtest {
public:
	newtest(double &&koll) : myk(koll) {}
	newtest(vector<double> &&koll): k(koll) {}

	double operator() () {
		return myk;
	}

private:
		double myk=0.;
public: vector<double> k;

};




class MaintenanceRespiration {
public:
	MaintenanceRespiration(vector< vector<double>> MCoef): Coef(MCoef) { LumpedPlant = false; }
	MaintenanceRespiration(double CommonCoef) : CommonCoef(CommonCoef) { LumpedPlant = true; }
	double operator() (PlantComp comp, size_t i, double Mass) {
		if (LumpedPlant)
			return CommonCoef*Mass;
		else
			return Coef[size_t(comp)][i] * Mass;
	}

	double operator() (double Mass) {
			return CommonCoef*Mass;
	}
private:
	vector< vector<double>> Coef;
	double CommonCoef=0.;
	bool LumpedPlant;

};
struct PlantSoilLayers {
	vector<double> Plants;
	vector<double> Layers;
};

class PlantRespirationFunctions {
public:
	PlantRespirationFunctions(function<double(PlantComp, size_t, double)> f_Main, function<double(double)> f_Temp,
		function<double(size_t, double)> f_Growth, function<double(PlantComp, size_t)> f_SaltImpact
		):	f_Main(f_Main), f_Temp(f_Temp), f_Growth(f_Growth), f_SaltImpact(f_SaltImpact) {};

	PlantRespirationFunctions(function<double(PlantComp, size_t, double)> f_Main, function<double(double)> f_Temp,
		function<double(size_t, double)> f_Growth): f_Main(f_Main), f_Temp(f_Temp), f_Growth(f_Growth)
	{};

	PlantRespirationFunctions(function<double(PlantComp, size_t, double)> f_Main, function<double(double)> f_Temp) :
		f_Main(f_Main), f_Temp(f_Temp)  {};

	vector<double> operator() (PlantComp icomp,  vector<double> &Mass, vector<double> &Temp, vector<double> &Resp) {
		for (size_t i = 0; i < Mass.size(); i++)
			Resp[i] = f_Main(icomp, i, Mass[i])*f_Temp(Temp[i]);
		return Resp;
	};
	PlantSoilLayers operator() (PlantComp icomp, vector<vector<double>> &Dist, vector<double> &Mass, vector<double> &Temp) {
		PlantSoilLayers out;
		out.Plants.assign(Mass.size(), 0.);
		out.Layers.assign(Temp.size(), 0.);
		for (size_t i_plant = 0; i_plant < Mass.size(); i_plant++) {
			double sumplant = 0.; double layer;
			for (size_t ii = 0; ii < Temp.size(); ii++) {
				layer= f_Main(icomp, i_plant, Dist[i_plant][ii] * Mass[i_plant])*f_Temp(Temp[ii]);
				sumplant += layer;
				out.Layers[ii] += layer;
			}
			out.Plants[i_plant] = sumplant;
		}
		return out;
	}
	PlantSoilLayers operator() (PlantComp icomp, vector<vector<double>> &Dist, vector<double> &Mass, vector<double> &Photo, vector<double> &Temp) {
		PlantSoilLayers out;
		out.Plants.assign(Mass.size(), 0.);
		out.Layers.assign(Temp.size(), 0.);
		for (size_t i_plant = 0; i_plant < Mass.size(); i_plant++) {
			double sumplant = 0.; double layer;
			for (size_t ii = 0; ii < Temp.size(); ii++) {
				layer = f_Main(icomp, i_plant, Dist[i_plant][ii] * Mass[i_plant])*f_Temp(Temp[ii])+ f_Growth(i_plant, Dist[i_plant][ii] * Photo[i_plant]);
				sumplant += layer;
				out.Layers[ii] += layer;
			}
			out.Plants[i_plant] = sumplant;
		}
		return out;
	}
	vector<double> operator() (PlantComp icomp, vector<double> &Mass, double &Temp, vector<double> &Resp) {
		for (size_t i = 0; i < Mass.size(); i++)
			Resp[i] = f_Main(icomp, i, Mass[i])*f_Temp(Temp);
		return Resp;
	};
	vector<double> operator() (PlantComp icomp, vector<double> &Mass, vector<double> &Photo, double &Temp, vector<double> &Resp) {
		for (size_t i = 0; i < Mass.size(); i++)
			Resp[i] = f_Growth(i,  Photo[i])+f_Main(icomp,i, Mass[i])*f_Temp(Temp);
		if(f_SaltImpact)
			for (size_t i = 0; i < Mass.size(); i++)
				Resp[i]+= f_SaltImpact(icomp, i);
		return Resp;
	};



private:
	function<double(PlantComp,size_t, double)> f_Main;
	function<double(size_t, double)> f_Growth;
	function<double(PlantComp,size_t)> f_SaltImpact;
	function<double(double)> f_Temp;

};



