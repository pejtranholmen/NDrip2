#pragma once

#include "./Additional_BioticVariables_Functions.h"
class Additional_BioticVariables : public Additional_BioticVariables_Functions
{
	public:
		void setAdditional_BioticVariables(ModelInfo* pInfo, ModelMap* pMap, Structure* pStruc);
		Additional_BioticVariables(void);
		~Additional_BioticVariables(void);
		bool Ini();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();

};
