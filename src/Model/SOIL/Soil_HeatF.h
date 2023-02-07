#pragma once

#include "./Soil_HeatF_Functions.h"
class Soil_HeatF : public Soil_HeatF_Functions
{
	public:
		Soil_HeatF(void);
		~Soil_HeatF(void);
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();

		void HeatWaterFlow();
		void HeatVapourFlow();

	private:
		void LowerBoundary();
		void ProfileFlux();
		void UpperBoundary();

		void WaterPipeModule();

		


};
