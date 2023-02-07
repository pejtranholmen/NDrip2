#pragma once

#include "./NC_SoilManagement.h"
#include "../../NewBase/ModelMap.h"	


class SoilOrgModel	:public GlobalInfo
	{
	public:
		SoilOrgModel(void);
		~SoilOrgModel(void);
		bool Def(ModelInfo*, ModelMap*);
		bool Ini();
		bool Dyn();
		bool End();
		NC_SoilManagement m_NC_SoilOrg;

		
	};

