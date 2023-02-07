#pragma once

#include "./Thermal_Functions.h"
class Thermal : public Thermal_Functions
{
	public:
		Thermal(void);
		~Thermal(void);
		bool Ini();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();

};
