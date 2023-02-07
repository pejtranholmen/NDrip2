#pragma once

#include "./P_Mineral_Functions.h"
class P_Mineral : public P_Mineral_Functions
{
	public:
		P_Mineral(void);
		~P_Mineral(void);
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();




};
