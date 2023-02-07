#include "Soil_Frost.h"
#include "../Atmosphere/Irrigation.h"

void Soil_Frost::setSoil_Frost(ModelInfo* pInfo, ModelMap* pMap, Structure* pStruc)
{
	ModuleName = "Soil frost";
	ModuleNo = 15;
	ModuleConditions = "WaterEq>0|HeatEq>0";
	m_pModelInfo = pInfo; m_pModelMap = pMap; m_pModelStructure = pStruc;
	pT_Step = &m_pModelInfo->T_Step;
	T_Step = *pT_Step;
}
Soil_Frost::Soil_Frost(void)
{
	ModuleName="Soil frost";
	ModuleNo=15;
	ModuleConditions="WaterEq>0|HeatEq>0";
}

bool Soil_Frost::Ini()	{
	T_Step=*pT_Step;

	Soil_WaterF::Ini();
    if(!m_pModelInfo->StartStateValue) 
            WaterHFD.assign(NL,0.);
	return true;
}
bool Soil_Frost::IniFunctors() {


	Soil_WaterF::IniFunctors();

	return true;
}
bool Soil_Frost::Dyn()
{
	
	Flux();		
	
	Soil_WaterF::Dyn();

	Integ();
	return true;
}

void Soil_Frost::Flux()
{
	Irrigation* p_Atm=(Irrigation*)pAtm;
	WaterFlowFrost();
	InfilHighDomain=p_Atm->SoilInfil*p_Atm->FracInfiltrationHighFlow;
}
void Soil_Frost::Integ()
{

//!  Infiltration into high-flow domain 
    InFlow(WaterHFD.front(), InfilHighDomain);
    OutFlow(WaterStorage.front(), InfilHighDomain);


//!  Percolation of infiltrated water            
	if(FrFlowDomain>=1) {
		IntFlow (WaterHFD, WaterflowHFD);
//!  Freezing of percolating water 
		TransFlow (WaterHFD, WaterStorage, Infreeze);
//!  Flow from unfrozen top layer to frozen layer below 
		TrIntFlow (WaterStorage, WaterHFD, WaterflowLD_HD);
//!  Flow from frozen layer to unfrozen below 
		TrIntFlow (WaterHFD, WaterStorage, WaterflowHD_LD);
	}



	return;
}

bool Soil_Frost::End()
{
return true;
}
Soil_Frost::~Soil_Frost(void)
{
}

