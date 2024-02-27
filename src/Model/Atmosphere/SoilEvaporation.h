#pragma once
struct SOILEBAL_IN
{
		double SoilEbal;
		double AreaFraction;
		double TotalCoverDegree;
		double SurfaceMoistureDiff;
} ;
enum class soil_surfacetype {ONE, SPLIT};
struct SOILEBAL_OUT
{
	   double RadNet=0.;
	   double SoilEbal=0.;
	   double  LatentF=0.;
	   double  SensibleF=0.;
	   double  SoilSurfHeatFlow=0.;
	   double  Surface_Temp=0.;
	   double  Surface_VapourPressure=0.;
	   double  RaSoilSurf=10.;
};


#include "./SoilEvaporation_Functions.h"
class SoilEvaporation : public SoilEvaporation_Functions
{
	public:
		SoilEvaporation(void);
		~SoilEvaporation(void);
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();
		void SurfEvalSolution(SOILEBAL_IN , SOILEBAL_OUT &out, soil_surfacetype);

};
