#include "HBVModel.h"

HBVModel::HBVModel(void)
{
   	AddModule("HBV Module", &m_HBV);
}
bool HBVModel::Def(ModelInfo* modelptr, ModelMap* mapptr)
{	//m_HBV.SetPtr2Module(this,modelptr, mapptr);
	//m_HBV.Def();

	return true;
}
bool HBVModel::Ini()
{
	m_HBV.Ini();

	return true;
}
bool HBVModel::Dyn()
{
	m_HBV.Dyn();

	 return true;
}

bool HBVModel::End()
{
	m_HBV.End();

	return true;
}

HBVModel::~HBVModel(void)
{
}
