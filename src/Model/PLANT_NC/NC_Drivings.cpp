#include "NC_Drivings.h"
#include "../../Simulator/Simulator.h"

						   
NC_Drivings::NC_Drivings(void)
{
	ModuleName="Abiotic driving variables";
	ModuleNo=27;
	ModuleConditions="";
	NumAccNDriv = 0;
}

bool NC_Drivings::Ini()	{
	//USE GENERAL,Only:	ICREATBIN, IOPENBIN

	//m_pModelStructure = ((Simulator*)m_pModelInfo)->GetSimStructurePointer();
   DrivNPG1=false;
   Extracted =true;
   NumAccNDriv=0;

   if(m_pModelStructure->Nitrogen_Carbon>=1) {
		if(SoilWaterInput==0)
			NDrivTheta.assign(pSoil->NL, SoilWaterContent);
		else 
			for(size_t i=0; i<NDrivTheta.size(); i++) NDrivTheta[i]=pSoil->Theta[i];
		NDrivWaterStress.assign(pPlant->NumPlants,1.);
		NDrivSaltStress.assign(pPlant->NumPlants,1.);
		if(WaterStressInput==1) DrivNPG1=true;
		if(SoilInfilInput==1) DrivNPG1=true;
		if(SoilDrainageInput==2) 
			DrivNPG1=true;
		else if(SoilDrainageInput==3) 
			AccNDrivDrainage.resize(pSoil->NL);
		
		if(DeepPercolationInput==2) DrivNPG1=true;
		if(SoilWaterInput==1) 
			DrivNPG1=true;
		else if(SoilWaterInput==2) 
			AccNDrivTheta.resize(pSoil->NL);
		
		if(SoilTempInput==1) 
			 DrivNPG1=true;
		else if(SoilTempInput==2) 
			AccNDrivTemperature.resize(pSoil->NL);

		if(SoilWaterFlowInput==1) 
			DrivNPG1=true;
		else  if(SoilWaterFlowInput==2) 
			AccNDrivWaterFlow.resize(pSoil->NL);
		
		AccNDrivTranspiration.resize(pPlant->NumPlants,1.);
		AccNDrivWaterStress.resize(pPlant->NumPlants,1.);
		AccNDrivSaltStress.resize(pPlant->NumPlants,1.);
		NDrivTranspiration.resize(pPlant->NumPlants,1.);
   }

   if(DrivNPG1) {
	   string filename;
	   filename=m_PG_Abiotic.GetFileName();
	   m_PG_Abiotic.Open(filename);
		//Run_res_id=INECO(FileDriv9, NUnitDriv9, DAT2)
		if(!m_PG_Abiotic.Open(filename)) {
		  Message("ERROR! - Abiotic Driving variable file not found");
		  //Call Utp(Screen(1:llcar(screen))//': '//FileDriv9//' 'C)
		  return false;
		}
		m_pModelInfo->Run_Res_Id=CheckEcoDriv3();
		if(!m_pModelInfo->Run_Res_Id) return false;
																						  
	/*	if(NumValidInput3<=0) {
			WRITE(Screen,*) "ERROR! - No valid driving variable found"
			Call Utp(Screen(1:llcar(screen))//': '//FileDriv9//' 'C)
			Run_res_id=-1
		else
			CALL INIECO(1, 1, NumValidInput3, NUnitDriv9, ICONDRIV9)
		}*/
    }


   if (GreenIndexForcing == 1) {
	   string filename;
	   filename = m_PG_GreenIndex.GetFileName();
	   if(!m_PG_GreenIndex.Open(filename)){
		   Message("ERROR! - Abiotic Driving variable file not found");
		   m_pModelInfo->Run_Res_Id = false;
		   return false;
	   }
	   GreenIndexIpos.ivar = 1;
	   GreenIndexIpos.discrete = false;
	   GreenIndexIpos.pgfile = &m_PG_GreenIndex;
	   m_PG_GreenIndex.ResetPos();
   }
	return true;
}
bool NC_Drivings::IniFunctors() {
	//USE GENERAL,Only:	ICREATBIN, IOPENBIN

	return true;
}
bool NC_Drivings::Dyn()
{
	Flux();
	Integ();
	return true;
}

void NC_Drivings::Flux()
{
/*	Use Timeinfo
Use Fileinfo
Use GenFunctions, Only	: SVP, Sintemp
Use CommonStruc, Only	: DampDep, SoilEvap
Use Drivings,	only	: RadInShort
*/

//double Ych, PreccFactor	;//! Local variable

 //  if(DrivNPG1) {
	//	NDrivEcoDat=GetEcoDat(9, NumValidInput3, NDrivECODAT
	//}
	
	Extracted=true;
	if(NumAccNDriv==0) 
		 NumAccNDriv=1;
	

	if (GreenIndexForcing == 1) {
		GreenIndexDriver = GetEcoDat(&m_PGPos_GreenIndex, GreenIndexIpos);
	}

	if(SoilTempInput==2) 
		 for(size_t i=0; i<NDrivTemperature.size();i++) NDrivTemperature[i]=AccNDrivTemperature[i]/NumAccNDriv;
	else if(SoilTempInput==1) //				! Soil Temperature
		NDrivTemperature=GetEcoDat(&m_PGPos_Abiotic,pSoil->NL,TempIpos);
	else
		NDrivTemperature.assign(pSoil->NL,SoilTemperature);
	

	if(SoilWaterInput==2) {
		for(size_t i=0; i<NDrivTheta.size();i++) { 
			NDrivTheta[i]=AccNDrivTheta[i]/NumAccNDriv;
			if(NDrivTheta[i]<=0) 
				NDrivTheta[i]=0.001;
		
		}
	}
	else if(SoilWaterInput==1)  //				! Soil water content
		NDrivTheta=GetEcoDat(&m_PGPos_Abiotic,pSoil->NL,ThetaIpos);
	else
		NDrivTheta.assign(pSoil->NL,SoilWaterContent);
	

	if(SoilWaterFlowInput==2) //{
		for(size_t i=0; i<NDrivWaterFlow.size();i++) NDrivWaterFlow[i]=AccNDrivWaterFlow[i]/NumAccNDriv;
 	else if(SoilWaterFlowInput==1) 				//! Soil water flow
		NDrivWaterFlow=GetEcoDat(&m_PGPos_Abiotic, pSoil->NL, WaterFIpos);
	else
		NDrivWaterFlow.assign(pSoil->NL,SoilWaterFlow);
	

	if(SoilDrainageInput==3) 
		for(size_t i=0; i<NDrivDrainage.size();i++) NDrivDrainage[i]=AccNDrivDrainage[i]/NumAccNDriv;
 	else if(SoilDrainageInput==2) 			//	! Soil water flow
		NDrivDrainage=GetEcoDat(&m_PGPos_Abiotic, pSoil->NL,DrainageIpos);
	else if(SoilDrainageInput==1) 
		NDrivDrainage.assign(pSoil->NL,SoilDrainage);
	

	if(SoilInfilInput==2) 
		NDrivInfiltration=AccNDrivInfiltration/NumAccNDriv;
  	else if(SoilInfilInput==1)	//				! Soil water flow
		NDrivInfiltration=GetEcoDat(&m_PGPos_Abiotic,InfiltrationIpos);
	else 
		NDrivInfiltration=SoilDrainage;
	
	if( WaterStressInput==2) 
		for(size_t i=0; i<NDrivWaterStress.size();i++) NDrivWaterStress[i]=AccNDrivWaterStress[i]/NumAccNDriv;
   	else if(WaterStressInput==1) //
		NDrivWaterStress=GetEcoDat(&m_PGPos_Abiotic,pPlant->NumPlants, StressIpos);
	else 
		NDrivWaterStress.assign(pPlant->NumPlants,PlantWaterStress);
	
	if(DeepPercolationInput==3) 
		NDrivDeepPercolation=AccNDrivDeepPercolation/NumAccNDriv;
   	else if(DeepPercolationInput==2) 				//! Soil water flow
		NDrivDeepPercolation=GetEcoDat(&m_PGPos_Abiotic,DeepPercIpos);
	else if(DeepPercolationInput==1) 
		NDrivDeepPercolation=DeepPercolation;
	
//!	Preliminary implementation of Transpiration

	for(size_t i=0; i<NDrivTranspiration.size();i++) NDrivTranspiration[i]=AccNDrivTranspiration[i]/NumAccNDriv;
	NDrivRadInShort=AccNDrivRadInShort/NumAccNDriv;
	NDrivTAir	=	AccNDrivTair/NumAccNDriv;
	if(m_pModelStructure->Nitrogen_Carbon>=1) 
		for(size_t i=0; i<NDrivSaltStress.size();i++) NDrivSaltStress[i]=AccNDrivSaltStress[i]/NumAccNDriv;
	

}
void NC_Drivings::Integ()
{
}

bool NC_Drivings::End()
{
return true;
}
NC_Drivings::~NC_Drivings(void)
{
}
