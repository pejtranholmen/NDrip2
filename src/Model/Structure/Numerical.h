#pragma once

#include "./Numerical_Functions.h"
class Numerical : public Numerical_Functions
{
	public:
		Numerical(void);
		~Numerical(void);
		bool Ini();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();

};
