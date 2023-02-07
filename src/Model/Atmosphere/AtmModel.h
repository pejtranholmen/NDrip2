#pragma once

#include "./Irrigation.h"
#include "../../NewBase/ModelMap.h"	


class AtmModel	:public GlobalInfo
	{
	public:
		AtmModel(void);
		~AtmModel(void);
		bool Def(ModelInfo*, ModelMap*);
		bool Ini();
		bool Dyn();
		bool End();
		Irrigation m_Irrigation;

		
	};

