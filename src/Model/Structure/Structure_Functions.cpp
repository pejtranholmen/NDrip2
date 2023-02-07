#include "Structure_Functions.h"
#include "../SOIL/SoilModel.h"
#include "../Atmosphere/AtmModel.h"
#include "../HBV/HBVModel.h"
#include "../PLANT_NC/PlantOrgModel.h"
#include "../SOIL_NC/SoilOrgModel.h"

Structure_Functions::Structure_Functions(void)
{
	m_NumActPlantElements = 2;

}
Structure_Functions::~Structure_Functions(void)
{
}
bool Structure_Functions::Def()
{
	Numerical::Def();

	ModuleNo=2;
	ModuleConditions="";
 	SoilModel* pSoil=(SoilModel*)m_pModelInfo->GetModelPtr("Soil Model");
	p_Soil_WaterF=&pSoil->m_SoilFrost;


	p_Soil_HeatF=&pSoil->m_HeatPump;

 	AtmModel* pAtm=(AtmModel*)m_pModelInfo->GetModelPtr("Atm Model");
	p_Irrigation=&pAtm->m_Irrigation;


	Sw* pSw;
	pSw= new Sw(&SaltTracer,SINGLE,  SALT, TRANSPORT, NOBPROC, NORMAL);
    //Define(pSw, "CanopyHeightInput",0,"Parameters|Driving variable|Simulated","Roughness = 2|PlantType > 1")
	Define(pSw, "SaltTracer",0,"off|on","WaterEq > 0");
	pSw= new Sw(&GroundWaterFlow,SINGLE,  WATER, TRANSPORT, NOBPROC, NORMAL);
 	Define(pSw,"GroundWaterFlow",0,"off|on","WaterEq > 0",104);
	pSw= new Sw(&WaterEq,SINGLE,  WATER, NOFPROC, NOBPROC, NORMAL);
 	Define(pSw, "WaterEq",0,"off|On with complete soil profile|On but without Richards equation","",100);
	pSw= new Sw(&HeatEq,SINGLE,  HEAT, NOFPROC, NOBPROC, NORMAL);
 	Define(pSw, "HeatEq",1,"off|on","",101);
	pSw= new Sw(&Irrigation,SINGLE,  WATER, NOFPROC, NOBPROC, NORMAL);
 	Define(pSw,"Irrigation",0,"off|on","WaterEq > 0");
	pSw= new Sw(&SoilVapour,SINGLE,  VAPOUR, NOFPROC, NOBPROC, NORMAL);
 	Define(pSw, "SoilVapour",0,"off|Only SoilVapourflow|Soil- and SnowVapourflow|Only SnowVapourflow","WaterEq > 0|HeatEq > 0");
	pSw= new Sw(&SnowPack_Option,SINGLE,  SNOW, NOFPROC, NOBPROC, NORMAL);
 	Define(pSw, "SnowPack",0,"off|on","WaterEq > 0",108);
	pSw= new Sw(&PlantType,PLANT,  WATER, NOFPROC, NOBPROC, NORMAL);
 	Define(pSw, "PlantType",0,"No vegetation|Implicit big leaf|Explicit single big leaf|Explicit big leafes","WaterEq > 0",103);
	pSw= new Sw(&Evaporation,SINGLE,  WATER, NOFPROC, NOBPROC, NORMAL);
 	Define(pSw,"Evaporation",0,"off|Simple input style|Radiation input style","WaterEq > 0",102);
	pSw= new Sw(&Nitrogen_Carbon,SINGLE,  ORGANIC, NOFPROC, NOBPROC, NORMAL);
 	Define(pSw, "Nitrogen and Carbon",0,"off|Dynamic interaction with abiotics|Using abiotic driving variables","",109);
	pSw= new Sw(&LateralInput,SINGLE,  WATER, NOFPROC, NOBPROC, NORMAL);
 	Define(pSw, "LateralInput",0,"No lateral input|In driving file|WaterShed approach","WaterEq > 0",110);
	//Define(pSw,  Sw_Minteq, "Minteq", 0, "off|on","WaterEq >0")
	pSw= new Sw(&HBV_Soil,SINGLE,  WATER, NOFPROC, NOBPROC, NORMAL);
 	Define(pSw,"HBV Soil Module",0,"off|As a complementary model|Excluding Richard equation","",203);
	pSw= new Sw(&OnlyNC,SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
	Define(pSw, "OnlyNC",0,"No|Yes","");
	pSw = new Sw(&Phosphorus_Model, SINGLE, PHOSPHORUS, NOFPROC, NOBPROC, NORMAL);
	Define(pSw, "Phosphorus", 0, "off|Simple|Detailed", "Nitrogen and Carbon > 0");

	
	
	
return true;
}



