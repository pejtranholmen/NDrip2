#pragma once

#include "./Soil_Frost_Functions.h"
class Soil_Frost : public Soil_Frost_Functions
{
	public:
		void setSoil_Frost(ModelInfo* pInfo, ModelMap* pMap, Structure* pStruc);
		Soil_Frost(void);
		~Soil_Frost(void);
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();



};
