#pragma once

#include "./Technical_Functions.h"
class Technical : public Technical_Functions
{
	public:
		Technical(void);
		~Technical(void);
		bool Ini();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();

};
