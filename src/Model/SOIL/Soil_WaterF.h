#pragma once

#include "./Soil_WaterF_Functions.h"
class Soil_WaterF : public Soil_WaterF_Functions
{
	public:
		Soil_WaterF(void);
		~Soil_WaterF(void);
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();



};
