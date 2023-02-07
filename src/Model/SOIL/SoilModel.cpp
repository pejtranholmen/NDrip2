#include "SoilModel.h"
#include "../GlobalInfo.h"

SoilModel::SoilModel(void)
{

	AddModule("Soil Heat Pump",&m_HeatPump);
	AddModule("Soil Frost",&m_SoilFrost);
}
bool SoilModel::Def(ModelInfo* modelptr, ModelMap* mapptr)
{
	//m_Profile.SetPtr2Module(this, modelptr, mapptr);
//	m_Profile.Def();
	//m_Hydraulic.SetPtr2Module(this, modelptr, mapptr);
	//m_Hydraulic.Def();
	//m_SoilFrost.SetPtr2Module(this, modelptr, mapptr);
	//m_SoilFrost.Def();
	//m_HeatPump.SetPtr2Module(this, modelptr, mapptr);
	//m_HeatPump.Def();

	return true;
}
bool SoilModel::Ini()
{
	//m_Profile.Ini();
	return true;
}
bool SoilModel::Dyn()
{
	 return true;
}

bool SoilModel::End()
{

	return true;
}

SoilModel::~SoilModel(void)
{
}
