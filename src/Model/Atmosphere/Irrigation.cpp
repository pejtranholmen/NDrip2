#include "../../std.h"
#include "./Irrigation.h"
#include "../PhysConst.h"
#include "../PLANT_NC/PlantOrgModel.h"
#include "../PLANT_NC/NC_Plant.h"

void Irrigation::setIrrigation(ModelInfo* pInfo, ModelMap* pMap, Structure* pStruc)
{
	ModuleName = "Irrigation";
	ModuleNo = p_ModelInfo->GetGroupIndex(ModuleName);
	ModuleConditions = "Irrigation>0";
	m_pModelInfo = pInfo; m_pModelMap = pMap; m_pModelStructure = pStruc;
	pT_Step = &m_pModelInfo->T_Step;
	T_Step = *pT_Step;
}
Irrigation::Irrigation(void)
{
	ModuleName="Irrigation";
//	ModuleNo=p_ModelInfo->GetGroupIndex(ModuleName);
	ModuleConditions="Irrigation>0";
}

bool Irrigation::Ini()	{
	if(!Surface_Water::Ini()) return false;


	DripContainer=0.0;

	if(Dripper>0) {
		double SumFrac=0.;
		SumFrac=sum(DrippInfilDistF);
		
		if(SumFrac!=1.&&SumFrac>0) 
           for(size_t i=0; i<DrippInfilDistF.size(); i++)
			   DrippInfilDistF[i]=DrippInfilDistF[i]/SumFrac;
		DripInUse = true;
	}
    IrriON=false;
    OpenTank=false;
    IrriSum=0.;
    IrrigationRate=0.;

	return true;
}
bool Irrigation::IniFunctors() {
	if (!Surface_Water::IniFunctors()) return false;
	return true;

}
bool Irrigation::Dyn()
{
	Radiation::Dyn();
	Flux();
	Integ();
	return true;
}

void Irrigation::Flux()
{
	if(IrrigationInput>=1){
			WStore=0.;
			for(size_t i=0; i<size_t(IStoreLayer); i++)
				WStore+=p_Soil_WaterF->WaterStorage[i];


			if(WStore<=IStoreMin||IrriON) {
				PlantOrgModel* pNCPlant=(PlantOrgModel*)m_pModelInfo->GetModelPtr("Plant Org Model");
				NC_Plant *pNC_Plant=&pNCPlant->m_NC_Plant;
				if(IrrigationInput>1&&(pNC_Plant->GrowthStageIndex[0]<1||pNC_Plant->GrowthStageIndex[0]>2.99))
					IrrigationRate=0.;
				else 
					IrrigationRate=min(IrrigRate, (IrrigAmount-IrriSum)/T_Step);
				if(TAir<5.) IrrigationRate=0.;			// ! Added August 27 by PEJ
				if(!IrriON) {
					IrriON=true;
					IrriSum=0.;
				}
				else if(IrrigationRate<=0) {
					IrriSum=0.;
					IrriON=false;
				}

			}
//! ********** Check of irrigation amount *****************************
			if(IrriON) {
				if(IrriSum>IrrigAmount) {
					IrrigationRate=0.;
					IrriSum=0.;
					IrriON=false;
				}
				IrriSum+=IrrigationRate*T_Step;
			}
		}
		else if( IrrigationInput==0) {
			if(IrrigIpos.ivar==0||!DrivPG1)
		   
		        IrrigationRate=0.;
		   else 
	            IrrigationRate=GetEcoDat(&m_PGPOS_Meteor[IrrigIpos.ivar], IrrigIpos);

		if(IrrigationRate>50) {

			int here=0;
		}

		   if(UnitIrrig>0) // ! Converts from mm to mm/day when using input
				IrrigationRate=IrrigationRate/T_Step;
		   
		}

		if(Dripper>0) { //! Dripp Irrigation
			PlantOrgModel* pNCPlant=(PlantOrgModel*)m_pModelInfo->GetModelPtr("Plant Org Model");
			NC_Plant *pNC_Plant=&pNCPlant->m_NC_Plant;

			DripInUse=false;
			for(size_t i=0; i<p_Plant->NumPlants; i++) {
				if((pNC_Plant->Drip[i]>0&&pNC_Plant->GrowthStageIndex[i]>=0)|| pNC_Plant->Growth == 0) {
					 DripInUse=true;
					 PlantIndexDrip=i;
				}

			}

			if(DripInUse) {

 				if( IrrigationRate>0.&&OpenTank) {
					OpenTank=false,
					DripFill=IrrigationRate;
					DripContainer=DripFill*T_Step+DripContainer;
				}
				else {
					DripFill=0.0;
					if(DripContainer<1.) OpenTank=true;	// ! Threshold for Open Container
				}
			
				IrrigationRate=0.;//  ! Irrigationrate is going into container
 
				DripOutlet=min(DripIrrigRate, max(0., DripContainer/T_Step));
				if (IrrigationInput > 0) IrrigationRate = DripOutlet;

				DripContainer-=DripOutlet*T_Step;
				for(size_t i=0; i<Wsource.size(); i++) Wsource[i]=DrippInfilDistF[i]*DripOutlet;	  //! WSource is input to soil from container
				for(size_t i=0; i<Wsource.size(); i++) p_Plant->SaltSourceFlow[i]=Wsource[i]*p_Plant->SaltIrrigationConc;
			}
			else {
				 Wsource.assign(Wsource.size(),0.);
				 p_Plant->SaltSourceFlow.assign(p_Plant->SaltSourceFlow.size(),0.);
			}
 	  
	}
}
void Irrigation::Integ()
{
}

bool Irrigation::End()
{
	Surface_Water::End();
return true;
}
Irrigation::~Irrigation(void)
{
}
