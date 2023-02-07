#pragma once
#include "./Functors/Structure_Functors.h"
#include "./Structure_Functions.h"
class Structure : public Structure_Functions
{
	public:
		void setStructure(ModelInfo* pInfo, ModelMap* pMap, Structure* pStruc);
		Structure(void);
		~Structure(void);
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();

};
