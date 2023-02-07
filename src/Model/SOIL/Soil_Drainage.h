#pragma once

#include "./Soil_Drainage_Functions.h"
class Soil_Drainage : public Soil_Drainage_Functions
{
	public:
		Soil_Drainage(void);
		~Soil_Drainage(void);
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();

};
