#pragma once

#include "./Hydraulic_Functions.h"
class Hydraulic : public Hydraulic_Functions
{
	public:
		Hydraulic(void);
		~Hydraulic(void);
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();

};
