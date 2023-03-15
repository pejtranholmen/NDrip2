#pragma once
#include "./NC_GasFlux.h"
enum class Deep_Ploughing_Sw { off, Parameter, PG_File_Specified };
enum class SurfaceCultivation_Sw { off, Parameter, PG_File_Specified };

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

   CPG m_PG_SurfaceCult, m_PG_Deep_Ploughing;


   // New for Sowing and Emergence from PG file reading
   vector<size_t> ISurfaceCultDates, IPloughingDates;
   size_t ISurfaceCultCount, IPloughingCount;



   bool MixFrac(vector<double> *Substrate, size_t Num);

};

