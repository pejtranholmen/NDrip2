#pragma once
#include "./Profile_Functions.h"
class Profile : public Profile_Functions
{
	public:
		Profile(void);
		~Profile(void);
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();

};
