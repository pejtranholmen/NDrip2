#pragma once

#include "./Meteorological_Data_Functions.h"
class Meteorological_Data : public Meteorological_Data_Functions
{
	public:
		Meteorological_Data(void);
		~Meteorological_Data(void);
		bool Ini();
		bool IniFunctors();
		bool Dyn();
		void Flux();
		void Integ();
		bool End();


		bool CheckDriveHorizontal();
		bool CheckEcoDriv1();

};
