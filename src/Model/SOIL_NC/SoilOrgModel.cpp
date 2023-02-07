#include "SoilOrgModel.h"

SoilOrgModel::SoilOrgModel(void)
{
 }
bool SoilOrgModel::Def(ModelInfo* modelptr, ModelMap* mapptr)
{	

	//m_NC_SoilOrg.SetPtr2Module(this, modelptr, mapptr);
	//m_NC_SoilOrg.Def();
	return true;
}
bool SoilOrgModel::Ini()
{

	return true;
}
bool SoilOrgModel::Dyn()
{

	 return true;
}

bool SoilOrgModel::End()
{

	return true;
}

SoilOrgModel::~SoilOrgModel(void)
{
}
