#pragma once

#include "./Additional_Variables_Functions.h"
class Additional_Variables : public Additional_Variables_Functions
{
	public:
		Additional_Variables(void);
		~Additional_Variables(void);
		bool Ini();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();

	private:
		void AtmCanopyHeatFlux();

};
