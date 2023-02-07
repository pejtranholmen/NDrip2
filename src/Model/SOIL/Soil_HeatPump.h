#pragma once

#include "./Soil_HeatPump_Functions.h"
class Soil_HeatPump : public Soil_HeatPump_Functions
{
	public:
		void setSoil_HeatPump(ModelInfo* pInfo, ModelMap* pMap, Structure* pStruc);
		Soil_HeatPump(void);
		~Soil_HeatPump(void);
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();

};
