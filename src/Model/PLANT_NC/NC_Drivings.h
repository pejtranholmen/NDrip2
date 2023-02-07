#pragma once

#include "./NC_Drivings_Functions.h"
class NC_Drivings : public NC_Drivings_Functions
{
	public:
		NC_Drivings(void);
		~NC_Drivings(void);
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();

};
