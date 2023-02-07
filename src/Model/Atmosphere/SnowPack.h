#pragma once
struct SnowPrec {
	double Dens_Ice;
	double Dens_Snow;
	double Dens_Liq;
	double Rate_Snow;
	double Rate_Rain;
	double Temperature;
};
#include "./SnowPack_Functions.h"
class SnowPack : public SnowPack_Functions
{
	public:
		SnowPack(void);
		~SnowPack(void);
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();


		bool Reset_SnowPack();
		bool Reset_SnowFluxes();
		double MeasuredSnowFunction();
		void snow_endstep();
		SnowPrec new_density(double prcp);
		double SnowPloughDistFunction();

		double Snow_Ebal_Temperatures(int Indicator);
		bool SnowSurfaceMassBalance();
		double LateSnowFunc();

		bool Actual_EmpiricalMelting();
		double Actual_Phys_PrecipMelting(double HeatContentSnow);
		double Actual_Phys_SurfaceMelting(double QMeltSurface);
		double Actual_Phys_SubSurfaceMelting(double HeatChangeSnow);


		
};
