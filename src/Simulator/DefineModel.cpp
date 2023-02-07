#include "..//std.h"
#include "DefineModel.h"
#include "../NewBase/Doc.h"
#include "../SoilData/GetSoilStorage.h"
#include "../Model/HBV/HBV.h"
#include "../Model/HBV/HBVModel.h"

#include "../Model/PLANT/WaterUptake.h"

#include "../Model/Atmosphere/Irrigation.h"

#include "../Model/Structure/Structure.h"
#include "../Simulator/ModelInfo.h"


DefineModel::DefineModel () {

p_ModelInfo = nullptr;
p_ModelMap = nullptr;
p_ModelStructure = nullptr;
p_ModelInfo = new ModelInfo();
p_ModelMap = new ModelMap();
p_ModelStructure = &structuremodel.m_Structure; 


p_ModelInfo->AddModel("Soil Model", &soilmodel);
p_ModelInfo->AddModel("Plant Model", &plantmodel);
p_ModelInfo->AddModel("Atm Model", &atmmodel);
p_ModelInfo->AddModel("Structure Model", &structuremodel);
p_ModelInfo->AddModel("Plant Org Model", &plantorgmodel);
p_ModelInfo->AddModel("Soil Org Model", &soilorgmodel);




 atmmodel.m_Irrigation.setIrrigation(p_ModelInfo, p_ModelMap, p_ModelStructure);
 soilmodel.m_SoilFrost.setSoil_Frost(p_ModelInfo, p_ModelMap, p_ModelStructure);
 soilmodel.m_HeatPump.setSoil_HeatPump(p_ModelInfo, p_ModelMap, p_ModelStructure);

plantmodel.m_WaterUptake.setWaterUptake(p_ModelInfo, p_ModelMap, p_ModelStructure);
structuremodel.m_Additional.setAdditional_BioticVariables(p_ModelInfo, p_ModelMap, p_ModelStructure);
//plantorgmodel.m_NC_Drivings.setNC_Drivings(p_ModelInfo, p_ModelMap, p_ModelStructure);
soilorgmodel.m_NC_SoilOrg.setNC_SoilManagement(p_ModelInfo, p_ModelMap, p_ModelStructure);
plantorgmodel.m_NC_Plant.setNC_Plant(p_ModelInfo, p_ModelMap, p_ModelStructure);
structuremodel.m_Structure.setStructure(p_ModelInfo, p_ModelMap, p_ModelStructure);


soilmodel.m_SoilFrost.Def();
soilmodel.m_HeatPump.Def();
plantmodel.m_WaterUptake.Def();
atmmodel.m_Irrigation.Def();
//plantorgmodel.m_NC_Drivings.Def();
plantorgmodel.m_NC_Plant.Def();
soilorgmodel.m_NC_SoilOrg.Def();

structuremodel.m_Structure.Def();
structuremodel.m_Additional.Def();
size_t* pNE;
auto ne = p_ModelMap->GetNEPointer("Number of Layers");
pNE=ne->GetNE_Elem();
m_sizet.push_back(pNE);

};
DefineModel::~DefineModel() {
	auto koll = p_ModelInfo;
	if (koll != nullptr)		delete koll;
	auto koll1 = p_ModelMap;
	if (koll1 != nullptr)		delete koll1;

};