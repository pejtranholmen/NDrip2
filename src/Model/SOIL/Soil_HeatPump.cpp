#include "Soil_HeatPump.h"

void Soil_HeatPump::setSoil_HeatPump(ModelInfo* pInfo, ModelMap* pMap, Structure* pStruc)
{
	ModuleName = "Soil Heat Pump";
	ModuleNo = 17;
	ModuleConditions = "HeatPump > 0";
	m_pModelInfo = pInfo; m_pModelMap = pMap; m_pModelStructure = pStruc;
	pT_Step = &m_pModelInfo->T_Step;
	T_Step = *pT_Step;
}
Soil_HeatPump::Soil_HeatPump(void)
{
	ModuleName="Soil Heat Pump";
	ModuleNo=17;
	ModuleConditions="HeatPump > 0";
}

bool Soil_HeatPump::Ini()	{




	Soil_HeatF::Ini();
	return true;
}
bool Soil_HeatPump::IniFunctors() {




	Soil_HeatF::IniFunctors();
	return true;
}
bool Soil_HeatPump::Dyn()
{
	Soil_HeatF::Dyn();

	Flux();
	Integ();
	return true;
}

void Soil_HeatPump::Flux()
{
	
}
void Soil_HeatPump::Integ()
{
}

bool Soil_HeatPump::End()
{
return true;
}
Soil_HeatPump::~Soil_HeatPump(void)
{
}
