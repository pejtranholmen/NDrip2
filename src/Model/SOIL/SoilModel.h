#pragma once

#include "./Soil_Frost.h"
#include "./Soil_HeatPump.h"

#include "../../Simulator/ModelInfo.h"
#include "../GlobalInfo.h"
#include "../../NewBase/ModelMap.h"
class SoilModel	:public GlobalInfo
	{
	public:
		SoilModel(void);
		~SoilModel(void);
		bool Def(ModelInfo*, ModelMap*);
		bool Ini();
		bool Dyn();
		bool End();

		//Profile m_Profile;
		Soil_HeatPump m_HeatPump;
		Soil_Frost m_SoilFrost;
	};

