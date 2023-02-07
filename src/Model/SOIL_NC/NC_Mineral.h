#pragma once

#include "./NC_Mineral_Functions.h"
class NC_Mineral : public NC_Mineral_Functions
{
	public:
		NC_Mineral(void);
		~NC_Mineral(void);
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();




};
