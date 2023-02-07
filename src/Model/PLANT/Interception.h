#pragma once

#include "./Potential_Transpiration.h"
class Interception : public  Potential_Transpiration
{
	public:
		Interception(void);
		~Interception(void);
		bool Ini();
		bool IniFunctors();
		bool Def();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();


		vector<double> MC_PotWaterUptake;

		//!Switches
		size_t	PrecInterception; //!,SnowInterception   ! Interception
		size_t SnowIntUnload, InterceptionModel, SnowInterception;
		double PotWaterUptake; // From Water Uptake

		double IntCapacity, IntStorage, ThroughFall, IntThQual;

		vector<double>  MC_IntStorage, MC_EvaActInt;


		double  EPRatio, IntLAI, IntRS, IntBase;
		double IntLAIFrozen, IntSnowMeltScale, IntMaxCondensation;
		vector<double> MaxCover, LAICoverSens;
		//!Auxiliaries
		double EvaActInt, EvaPotInt, IntRate, DirectThroughfall, IntEvapFracMin;
		vector<double>	MC_EvaPotInt, MC_IntCapacity;

		//! Local Variables
		vector<size_t> IndexList;
		vector<double> Int_Filling;

		double TiStep;

		vector<P*> SurfaceCoverTab;



		function<double(double)> f_IntCapacity_Base;
		function<vector<double>(vector<double>)> f_IntCapacity_Base_MultiLayer;

		function<double(double, double)> f_IntCapacity_Snow;
		function<vector<double>(vector<double>, double)> f_IntCapacity_Snow_MultiLayer;
		function<double(vector<double> *, vector<size_t> *, vector<double> *, vector<double>, double, double)> f_MultiFill;

		function<double()> f_ThroughFall;
		function<double(double, double)> f_ThroughFallSinglePlant;
		function<double(double, double)> f_IntStorage;
		function<double(double, double, double)> f_EvapImplicitPlant;
		function<double(double, double, double, double)> f_EvapExplicitPlant;
		function<vector<double>(vector<double>, vector<double>, vector<double>)> f_EvapMultiPlant;
		function<vector<double>(vector<double>, vector<double>, vector<double>)> f_PotentialWaterUptakeMultiPlant;

		function<double(double, double)> f_PotentialWaterUptakeImplicitPlant;
		function<double(double, double, double)> f_PotentialWaterUptakeExplicitPlant;
		function<vector<double>(vector<double>, vector<double>)> f_PM_EquationPotentialIntercept_MultiLayer;

		function<double(double, double, double)> f_ThermalQualityInterCept;
		function<void(double, double, double)> f_ThermalQualityThroughFall;




};
