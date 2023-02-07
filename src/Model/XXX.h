#pragma once
#include <string>
//#include "../Base/ModelType.h"
#include "./XXX_Functions.h"
class XXX : public XXX_Functions
{
	public:
		XXX(void);
		~XXX(void);
		bool Ini();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();

};
