#pragma once
#include "./Surface_Water_Functions.h"
class Surface_Water : public Surface_Water_Functions
{
	public:
		Surface_Water(void);
		~Surface_Water(void);
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();

};
