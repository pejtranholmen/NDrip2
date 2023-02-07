#pragma once
#include "./Irrigation_Functions.h"
class Irrigation : public Irrigation_Functions
{
	public:
		void setIrrigation(ModelInfo* pInfo, ModelMap* pMap, Structure* pStruc);
		Irrigation(void);
		~Irrigation(void);
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();

};
