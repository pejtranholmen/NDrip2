#include "Structure.h"

void Structure::setStructure(ModelInfo* pInfo, ModelMap* pMap, Structure* pStruc)
{
	ModuleName = "Model Structure";
	m_pModelInfo = pInfo; m_pModelMap = pMap;
	m_pModelStructure = pStruc;
	pT_Step = &m_pModelInfo->T_Step;
	T_Step = *pT_Step;
}
Structure::Structure(void)
{
	ModuleName="Model Structure";

}

bool Structure::Ini()	{
	
	Numerical::Ini();
	T_Step=*pT_Step;
	if(!p_Irrigation->Ini()) return false;
	if(!p_Plant->Ini()) return false;
	if(!p_Soil_WaterF->Ini()) return false;
	if(!p_Soil_HeatF->Ini()) return false;
	if(!p_NC_Plant->Ini()) return false;
	if(!p_NC_Soil->Ini()) return false;
	p_Irrigation->SetTimeStep(m_pModelInfo->T_StepSnow);

	p_NC_Plant->SetTimeStep(m_pModelInfo->T_Step_NC);
	p_NC_Soil->SetTimeStep(m_pModelInfo->T_Step_NC);

	return true;
}
bool Structure::IniFunctors() {

	if (!p_Irrigation->IniFunctors()) return false;
	if (!p_Plant->IniFunctors()) return false;
	if (!p_Soil_WaterF->IniFunctors()) return false;
	if (!p_Soil_HeatF->IniFunctors()) return false;
	if (!p_NC_Plant->IniFunctors()) return false;
	if (!p_NC_Soil->IniFunctors()) return false;

	return true;
}
bool Structure::Dyn()
{
	Count_Call++;
	((Soil_Drainage_Functions*)p_Soil_WaterF)->SetDrainageFunctors();

	if(m_pModelInfo->T_CountIterOrg%m_pModelInfo->T_RatioInput==0&&m_pModelInfo->T_CountAdd%m_pModelInfo->T_RatioOrgStep==0) {
		m_pModelInfo->FIRST=true;
		p_Irrigation->SetTimeStep(m_pModelInfo->T_StepSnow);
		p_Plant->SetTimeStep(m_pModelInfo->T_StepSnow);
		p_Irrigation->Dyn();   //Call to Irrigation, radiation, Met data
		((Salt*)p_Plant)->Dyn(); // Call to Interception
		((SnowPack*)p_Irrigation)->Dyn();  // Call to Snow
		((Soil_Drainage_Functions*)p_Soil_WaterF)->SetDrainLevel();
		Numerical::Dyn();
		p_Irrigation->SetTimeStep(m_pModelInfo->T_Step);
		p_Soil_WaterF->SetTimeStep(m_pModelInfo->T_Step);
		p_Soil_HeatF->SetTimeStep(m_pModelInfo->T_Step);
		p_Plant->SetTimeStep(m_pModelInfo->T_Step);

	}
	p_Soil_WaterF->ThetaMMM();
	((Surface_Water*)p_Irrigation)->Dyn(); // Call to Soil Evaporation and Surface Water
	
	if (HBV_Soil_Sw(m_pModelStructure->HBV_Soil) != HBV_Soil_Sw::ExcludingRichardEquation) {
		p_Soil_HeatF->Dyn();
		p_Soil_WaterF->Dyn();
		((WaterUptake*)p_Plant)->Dyn();
	}

	

	if(m_pModelStructure->Nitrogen_Carbon>0)
		p_NC_Plant->NC_Drivings_Update();
	if(NitrogenCarbonStep==2) {

		if(m_pModelInfo->T_CountAdd%m_pModelInfo->T_RatioOrgStep==m_pModelInfo->T_RatioOrgStep-1) {
			if(m_pModelInfo->T_CountIterOrg%m_pModelInfo->T_RatioNC==0) {
				p_NC_Plant->NC_Drivings_Update();
				p_NC_Plant->Dyn();
				p_NC_Soil->Dyn();
			}
		}

	}
	else if(NitrogenCarbonStep==1) {
		if(m_pModelStructure->Nitrogen_Carbon>0)
			p_NC_Plant->NC_Drivings_Update();
		p_NC_Plant->SetTimeStep(m_pModelInfo->T_Step);
		p_NC_Soil->SetTimeStep(m_pModelInfo->T_Step);
		p_NC_Plant->Dyn();
		p_NC_Soil->Dyn();
		// Same as Water and Heat

	}
	else if(m_pModelInfo->T_CountIterOrg%m_pModelInfo->T_RatioNC==0) {
		if(m_pModelStructure->Nitrogen_Carbon>0)
			p_NC_Plant->NC_Drivings_Update();
		p_NC_Plant->SetTimeStep(m_pModelInfo->T_Step_NC);
		p_NC_Soil->SetTimeStep(m_pModelInfo->T_Step_NC);
		p_NC_Plant->Dyn();
		p_NC_Soil->Dyn();
	}

		//Flux();
		//Integ();

	return true;
}

void Structure::Flux()
{
	
}
void Structure::Integ()
{
}

bool Structure::End()
{
	p_Irrigation->End();
	p_NC_Plant->End();
return true;
}
Structure::~Structure(void)
{
}
