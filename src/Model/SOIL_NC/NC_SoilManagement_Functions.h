#pragma once
#include "./NC_GasFlux.h"

class NC_SoilManagement_Functions : public NC_GasFlux
{
	public:
		NC_SoilManagement_Functions(void);
		~NC_SoilManagement_Functions(void);
		bool Def();

   size_t SurfaceCultivation, Ploughing;
   double SurfaceCultDay, SurfaceCultDepth, PloughingDepth, PloughingDay;

   bool	Do_Ploughing;
   vector<double> DistFrac;
   bool MixFrac(vector<double> *Substrate, size_t Num);

};

