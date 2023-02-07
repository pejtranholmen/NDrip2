#include "Thermal.h"
#include "./Functors/Thermal_Functors.h"

						   
Thermal::Thermal(void)
{
	ModuleName="Soil Thermal";
	ModuleNo=6;
	ModuleConditions="HeatEq > 0";
}

bool Thermal::Ini()	{


	f_ThermalCond = ThCond_KerstenEq(Clay.UC1, Clay.UC2, Clay.UC3,
		Clay.FC1, Clay.FC2, Clay.FC3, Clay.FC4,	BulkDenS);

	//f_ThermalCond = ThCond_BallarpArp()
	return true;
}
bool Thermal::Dyn()
{
	Flux();
	Integ();
	return true;
}

void Thermal::Flux()
{
	
}
void Thermal::Integ()
{
}

bool Thermal::End()
{
return true;
}
Thermal::~Thermal(void)
{
}
