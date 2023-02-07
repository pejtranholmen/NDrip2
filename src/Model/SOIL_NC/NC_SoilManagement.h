#pragma once

#include "./NC_SoilManagement_Functions.h"
class NC_SoilManagement : public NC_SoilManagement_Functions
{
	public:
		void setNC_SoilManagement(ModelInfo* pInfo, ModelMap* pMap, Structure* pStruc);
		NC_SoilManagement(void);
		~NC_SoilManagement(void);
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();


		
};
