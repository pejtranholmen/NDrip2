#pragma once
#include "HBV.h"
#include "../Atmosphere/SnowPack.h"
#include "../PLANT/Interception.h"
#include "../Atmosphere/Meteorological_Data.h"
#include "../PLANT/Potential_Transpiration.h"
#include "../PLANT/WaterUptake.h"
#include "../../NewBase/ModelMap.h"

#include "../SOIL/Hydraulic.h"
#include "../SOIL/Profile.h"

class HBVModel	:public GlobalInfo
	{
	public:
		HBVModel(void);
		~HBVModel(void);
		bool Def(ModelInfo*, ModelMap*);
		bool Ini();
		bool Dyn();
		bool End();
 		HBV m_HBV;
		SnowPack m_Snow;
		Interception m_Interception;
		Meteorological_Data m_Meterological_Data;
		//Potential_Transpiration m_Potential_Transpiration;
		WaterUptake m_WaterUptake;
		Hydraulic m_Hydraulic;
		Profile m_Profile;
		
	};

