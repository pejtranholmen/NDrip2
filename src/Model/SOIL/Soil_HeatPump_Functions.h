#pragma once

#include "./Soil_HeatF.h"


class Soil_HeatPump_Functions : public Soil_HeatF
{
	public:
		Soil_HeatPump_Functions(void);
		~Soil_HeatPump_Functions(void);

		 bool Def();

		 double HPBase, HPAmp, HPCut, HPLayer, HPMax, HPZero;
		 double HeatPumpFlow;

	
};

