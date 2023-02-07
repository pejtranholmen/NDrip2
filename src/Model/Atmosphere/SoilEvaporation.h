#pragma once
struct SOILEBAL_IN
{
		double SoilEbal;
		double AreaFraction;
		double TotalCoverDegree;
		double SurfaceMoistureDiff;
} ;
enum soil_surfacetype {ONE, SPLIT};
struct SOILEBAL_OUT
{
	   double RadNet;
	   double SoilEbal;
	   double  LatentF;
	   double  SensibleF;
	   double  SoilSurfHeatFlow;
	   double  Surface_Temp;
	   double  Surface_VapourPressure;
	   double  RaSoilSurf;
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
		SOILEBAL_OUT SurfEvalSolution(SOILEBAL_IN , enum soil_surfacetype);

};
