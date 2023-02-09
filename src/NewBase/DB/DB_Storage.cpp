#pragma once
#include "../ModelMap.h"
#include "../../Model/ModelFunctions.h"
#include "DB_Storage.h"




DB_Storage::DB_Storage(void)
{
	m_pSimDoc=nullptr;
	m_IniDataBaseMade=false;
	
	for(size_t i=0; i<9;i++) {
		m_ReadInDone[i]=false;
		m_EditHeaders[i]=false;
		switch(i) {
			case 0:
			case 5: //Plant
				m_PlantTypeIndex[i]=0;
				break;
			case 1:
			case 7:
			case 3:
			case 8:
			default: //Soil
				m_PlantTypeIndex[i]=1;
				break;
			case 2:// Snow
				m_PlantTypeIndex[i]=2;
				break;
			case 4:  //Drainage
				m_PlantTypeIndex[i]=3;
				break;
			case 6:
				m_PlantTypeIndex[i] =4;
				break;
		}

	}
	//Plant
	m_PlantType[0].push_back("Coniferous Forest");
	m_PlantType[0].push_back("Decidous Forest");
	m_PlantType[0].push_back("Mixed Forest");
	m_PlantType[0].push_back("Natural Grassland");
	m_PlantType[0].push_back("Crops");
	m_PlantType[0].push_back("Shrubland");
	m_PlantType[0].push_back("Undefined");
	//Soil
	m_PlantType[1].push_back("Clayey soil");
	m_PlantType[1].push_back("Sandy soil");
	m_PlantType[1].push_back("Silty soil");
	m_PlantType[1].push_back("Loamy soil");
	m_PlantType[1].push_back("Organic soil");
	m_PlantType[1].push_back("Undefined");
	//Snow
	m_PlantType[2].push_back("Seasonal snow");
	m_PlantType[2].push_back("Permanent snow");
	m_PlantType[2].push_back("Undefined");
	//Water -Drainage
	m_PlantType[3].push_back("Natural drainage");
	m_PlantType[3].push_back("Tile drainage");
	m_PlantType[3].push_back("Ditches or canals");
	m_PlantType[3].push_back("Undefined");
	// Management
	m_PlantType[4].push_back("Ploughing");
	m_PlantType[4].push_back("Irrigated");
	m_PlantType[4].push_back("Fertilized");
	m_PlantType[4].push_back("Undefined");

	for(size_t i=0; i<9; i++) m_DB_KeyIndex[i]=0;
	m_DB_Index=0;

	m_Group[0].push_back(p_ModelInfo->GetGroupIndex("Potential Transpiration"));
	m_Group[0].push_back(p_ModelInfo->GetGroupIndex("Water uptake"));
	m_Group[0].push_back(p_ModelInfo->GetGroupIndex("Interception"));
	m_Group[0].push_back(p_ModelInfo->GetGroupIndex("Plant"));

	m_Group[1].push_back(p_ModelInfo->GetGroupIndex("Soil evaporation"));
	m_Group[1].push_back(p_ModelInfo->GetGroupIndex("Soil water flows"));

	m_Group[2].push_back(p_ModelInfo->GetGroupIndex("SnowPack"));

	m_Group[3].push_back(p_ModelInfo->GetGroupIndex("Soil frost"));

	m_Group[4].push_back(p_ModelInfo->GetGroupIndex("Soil water flows"));
	m_Group[4].push_back(p_ModelInfo->GetGroupIndex("Drainage and deep percolation"));
	m_Group[4].push_back(p_ModelInfo->GetGroupIndex("Surface water"));

	m_Group[5].push_back(p_ModelInfo->GetGroupIndex("Plant Growth"));
	m_Group[5].push_back(p_ModelInfo->GetGroupIndex("Potential Transpiration"));
	m_Group[5].push_back(p_ModelInfo->GetGroupIndex("Plant"));

	m_Group[6].push_back(p_ModelInfo->GetGroupIndex("Soil Managements"));
	m_Group[6].push_back(p_ModelInfo->GetGroupIndex("External N inputs"));
	m_Group[6].push_back(p_ModelInfo->GetGroupIndex("Meteorological Data"));
	m_Group[6].push_back(p_ModelInfo->GetGroupIndex("Radiation properties"));

	m_Group[7].push_back(p_ModelInfo->GetGroupIndex("Soil organic processes"));
	m_Group[7].push_back(p_ModelInfo->GetGroupIndex("Common abiotic responses"));

	m_Group[8].push_back(p_ModelInfo->GetGroupIndex("Soil mineral N processes"));
	m_Group[8].push_back(p_ModelInfo->GetGroupIndex("Gas processes"));

	//m_Group[0].push_back(p_ModelInfo->

}



DB_Storage::~DB_Storage(void)
{
}

string DB_Storage::GetGroupString(size_t db_index, size_t index) {
	CommonModelInfo *p_ModelInfo;
	p_ModelInfo=nullptr;
	return "";
	//return p_ModelInfo->GetGroupNames(m_Group[db_index][index]);
}

size_t DB_Storage::GetPlantTypeIndex(size_t db_index, string name)
{
	size_t ival = 0;
	while (m_PlantType[m_PlantTypeIndex[db_index]][ival] == name) {
		return ival;
	}
	return 0;
}

