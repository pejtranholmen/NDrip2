#include "Soil_HeatPump_Functions.h"

Soil_HeatPump_Functions::Soil_HeatPump_Functions(void)
{
}
Soil_HeatPump_Functions::~Soil_HeatPump_Functions(void)
{
}
bool Soil_HeatPump_Functions::Def()
{
	Soil_HeatF::Def();
	
	ModuleName="Soil Heat Pump";
	ModuleNo=17;
	ModuleConditions="HeatPump > 0";

	Ts *pTs= new Ts(&HeatPumpFlow, SINGLE, HEAT, EXTRACTION, NOBPROC, NORMAL);
	Define(pTs, "HeatPumpFlow", 0.,UNIT_TYPES::HEATFLOW_UNIT,"Heat Pump >= 1");
	
	Ps *pPs= new Ps(&HPBase, SINGLE, HEAT, EXTRACTION, NOBPROC, NORMAL);
	Define(pPs, "HPBase", 0.,UNIT_TYPES::HEATFLOW_UNIT ,	"Heat Pump = 1",-1.E7,1.E7);
	pPs= new Ps(&HPAmp, SINGLE, HEAT, EXTRACTION, NOBPROC, NORMAL);
	Define(pPs,  "HPAmp ", 1.E5,"J/m2/day/C","Heat Pump = 1",0., 1.E6);
	pPs= new Ps(&HPCut, SINGLE, HEAT, EXTRACTION, NOBPROC, NORMAL);
	Define(pPs,  "HPCut", -5.,UNIT_TYPES::TEMP_UNIT,			"Heat Pump = 1", -50., 20.);
	pPs= new Ps(&HPLayer, SINGLE, HEAT, EXTRACTION, NOBPROC, NORMAL);
	Define(pPs, "HPLayer", 4.,"#",		 "Heat Pump >= 1", 0., 100.);
	pPs= new Ps(&HPMax  , SINGLE, HEAT, EXTRACTION, NOBPROC, NORMAL);
	Define(pPs, "HPMax",   -10.,UNIT_TYPES::TEMP_UNIT ,		"Heat Pump = 1",-30., 0.);
	pPs= new Ps(&HPZero  , SINGLE, HEAT, EXTRACTION, NOBPROC, NORMAL);
	Define(pPs, "HPZero",   -10.,UNIT_TYPES::TEMP_UNIT ,	"Heat Pump = 1", -100., 10.);

	return true;
}
