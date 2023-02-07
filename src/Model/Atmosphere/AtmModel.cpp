#include "AtmModel.h"
//Test of small change when sunshine at Tranholmen

AtmModel::AtmModel(void)
{
	AddModule("Irrigation",&m_Irrigation);
 }
bool AtmModel::Def(ModelInfo* modelptr, ModelMap* mapptr)
{	

  	//m_Irrigation.SetPtr2Module(this, modelptr, mapptr);
	//m_Irrigation.Def();
	return true;
}
bool AtmModel::Ini()
{

	return true;
}
bool AtmModel::Dyn()
{

	 return true;
}

bool AtmModel::End()
{

	return true;
}

AtmModel::~AtmModel(void)
{
}
