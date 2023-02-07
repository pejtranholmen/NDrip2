#include "PlantOrgModel.h"

PlantOrgModel::PlantOrgModel(void)
{
 }
bool PlantOrgModel::Def(ModelInfo* modelptr, ModelMap* mapptr)
{	

	//m_NC_Plant.SetPtr2Module(this, modelptr, mapptr);
	//m_NC_Plant.Def();
	return true;
}
bool PlantOrgModel::Ini()
{

	return true;
}
bool PlantOrgModel::Dyn()
{

	 return true;
}

bool PlantOrgModel::End()
{

	return true;
}

PlantOrgModel::~PlantOrgModel(void)
{
}
