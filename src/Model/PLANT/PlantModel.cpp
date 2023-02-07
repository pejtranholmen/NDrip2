#include "PlantModel.h"

PlantModel::PlantModel(void)
{
 }
bool PlantModel::Def(ModelInfo* modelptr, ModelMap* mapptr)
{	

  	//m_WaterUptake.SetPtr2Module(this, modelptr, mapptr);
	//m_WaterUptake.Def();
	return true;
}
bool PlantModel::Ini()
{

	return true;
}
bool PlantModel::Dyn()
{

	 return true;
}

bool PlantModel::End()
{

	return true;
}

PlantModel::~PlantModel(void)
{
}
