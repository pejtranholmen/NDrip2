#pragma once

#include "./WaterUptake.h"
#include "../../NewBase/ModelMap.h"	


class PlantModel	:public GlobalInfo
	{
	public:
		PlantModel(void);
		~PlantModel(void);
		bool Def(ModelInfo*, ModelMap*);
		bool Ini();
		bool Dyn();
		bool End();
		WaterUptake m_WaterUptake;

		
	};

