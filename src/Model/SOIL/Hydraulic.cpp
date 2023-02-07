#include "Hydraulic.h"

						   
Hydraulic::Hydraulic(void)
{
	ModuleName="Soil Hydraulic";
	ModuleNo=5;
	ModuleConditions="Water Eq>0";
}

bool Hydraulic::Ini()	{
	Profile::Ini();
	ReCalc();
	return true;
}
bool Hydraulic::IniFunctors() {
	Profile::IniFunctors();
	return true;
}
bool Hydraulic::Dyn()
{
	Profile::Dyn();
	Flux();
	Integ();
	return true;
}

void Hydraulic::Flux()
{
	
}
void Hydraulic::Integ()
{
}

bool Hydraulic::End()
{
return true;
}
Hydraulic::~Hydraulic(void)
{
}
