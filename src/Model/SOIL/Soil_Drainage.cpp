#include "Soil_Drainage.h"
#include "Soil_HeatPump.h"
#include "../Atmosphere/Irrigation.h"
#include "../../Simulator/Simulator.h"
#include "../Structure/Functors/Structure_Functors.h"
#include "./Functors/Drainage_Functor.h"
						   
Soil_Drainage::Soil_Drainage(void)
{
	ModuleName="Drainage and deep percolation";
	ModuleNo=21;
	ModuleConditions="WaterEq>0";
}

bool Soil_Drainage::Ini()	{
  Hydraulic::Ini();
  size_t NSOURCE;
 // m_pModelStructure = ((Simulator*)m_pModelInfo)->GetSimStructurePointer();
   if(WaterEq_Sw(m_pModelStructure->WaterEq)== WaterEq_Sw::off) return false;
   SetDrainageFunctors();
  
   if(DriveDrainLevel_Sw(DriveDrainLevel)>= DriveDrainLevel_Sw::Driving_File|| LBoundSaturated_Sw(LBoundSaturated)== LBoundSaturated_Sw::Dynamic_Grad_Flow) {
		string filename;
		
		filename=m_PG_Gw.GetFileName();
		m_PG_Gw.ResetPos();
        /*if(GWIndexFileName==1) {
             LL=Index(Filedriv5,'_')
            if(LL>0) {
		        Filedriv5(LL+1:)='      '
                if(GWFILEName_Index<10) {
                    WRITE(FILeDriv5(LL+1:LL+1),'(I1)') int(GWFileName_Index)
                else if(GWFILEName_INdex<100) {
                    WRITE(FILeDriv5(LL+1:LL+2),'(I2)') int(GWFileName_Index)
                else if(GWFILEName_INdex<1000) {
                    WRITE(FILeDriv5(LL+1:LL+3),'(I3)') int(GWFileName_Index)         
                else if(GWFILEName_INdex<1000) {
                    WRITE(FILeDriv5(LL+1:LL+4),'(I4)') int(GWFileName_Index)
                }
            else
                Call Utp_CR('Could not identify indexed file name'C)           
            } 
            Call Utp_CR(FileDriv5//'Could not identify indexed file name'C)   
        } */   
        m_pModelInfo->Run_Res_Id=m_PG_Gw.Open(filename);
	   m_PG_Gw.SetEndPoints_ValidValue();
	   IGW_Pos.ivar=1;
	   IGW_Pos.discrete=false;
	   IGW_Pos.pgfile=&m_PG_Gw;
	   IGW_Pos.shift=0;   

	   vDriveDrainLevel=-DepthLayers.back();



	}
    if(LBoundUnSaturated_Sw(LBoundUnSaturated)== LBoundUnSaturated_Sw::Dynamic_Psi&&GroundWaterFlow_Sw(m_pModelStructure->GroundWaterFlow)== GroundWaterFlow_Sw::off) {
		string filename;
		filename=m_PG_PressureHead.GetFileName();	
		m_pModelInfo->Run_Res_Id=m_PG_PressureHead.Open(filename);
		if (!m_pModelInfo->Run_Res_Id) return false;
	}

	if(GroundWaterFlow_Sw(m_pModelStructure->GroundWaterFlow)== GroundWaterFlow_Sw::off) {
	  LayerOfSatLevel=NL+1;
	  LayerOfDrain=NL+1;
	  return true;
	}

	if(LateralInput_Sw(m_pModelStructure->LateralInput)>LateralInput_Sw::No) NSOURCE=NL;
	else NSOURCE=0;

	LayerOfSatLevel=1;
	LayerOfSatLevelSaved=LayerOfSatLevel;  
	LayerOfSource=int(GWSoLayer);
    if(NSOURCE>LayerOfDrain) LayerOfDrain=min(NSOURCE, NL-1);

	return true;
}
bool Soil_Drainage::IniFunctors() {
	Hydraulic::IniFunctors();

	return true;
}
bool Soil_Drainage::Dyn()
{
	Flux();
	Integ();
	return true;
}

void Soil_Drainage::Flux()
{
	f_DeepPerc();
	if (GroundWaterFlow_Sw(m_pModelStructure->GroundWaterFlow) == GroundWaterFlow_Sw::off) return;


     if(EmpiricalDrainEq_Sw(EmpiricalDrainEq)== EmpiricalDrainEq_Sw::ON)
        NetEmpDrainage=f_EmpiricalDrainage(SatLev);
      

	if(PhysicalDrainEq_Sw(PhysicalDrainEq)>= PhysicalDrainEq_Sw::Linear_Model) {
			NetPhysDrainage=0.;
	//!       Loop over the number of layers that are above the drainagelevel (LayerOfDrain)
			for(size_t i=0; i<=LayerOfDrain; i++) 
				if(i<WaterDrainFlow.size())
					NetPhysDrainage+=WaterDrainFlow[i];
	}

	
	if(Pump_Sw(Pump)==Pump_Sw::ON) {
		 if(SatLev>PumpLowLevel&&SatLev<PumpHighLevel) 
			WaterDrainFlow[LayerOfSatLevel-1]+=PumpFlowRate;
	}

	if(EmpiricalDrainEq_Sw(EmpiricalDrainEq)== EmpiricalDrainEq_Sw::ON) {
	    if(LayerOfSatLevelSaved>LayerOfDrain) WaterDrainFlow[LayerOfSatLevelSaved-1]=0;
//!            Add horizontal ground water to the drainage level if
//!            saturation level is above drainage level (LayerOfDrain)
	    if(LayerOfSatLevel<=LayerOfDrain) 
			    WaterDrainFlow[LayerOfSatLevel]+=NetEmpDrainage;
		else if(LayerOfSatLevel<NL)
				WaterDrainFlow[LayerOfSatLevel]=NetEmpDrainage;
		else
			WaterDrainFlow.back()=NetEmpDrainage;

        LayerOfSatLevelSaved=LayerOfSatLevel;
	}

	TotalDrainage=NetPhysDrainage+NetEmpDrainage;
}
void Soil_Drainage::Integ()
{

	if(m_pModelStructure->LateralInput>=1||((Irrigation*)pAtm)->Dripper>0) 
		InFlow(WaterStorage, ((Irrigation*)pAtm)->Wsource);
 
	if(GWSoFlow>0.&&LayerOfSource>0)
		InFlow(WaterStorage[LayerOfSource-1], GWSoFlow);
 
	if(m_pModelStructure->GroundWaterFlow>=1) {
	  OutFlow(WaterStorage, WaterDrainFlow);
	  if(PhysicalDrainEq==0&&EmpiricalDrainEq==0) {
			WaterDrainFlow[NL-2]+=WaterDrainFlow.back();
			//Do i=1, NL-1,1
			for(size_t i=1; i<WaterDrainFlow.size()-1; i++) 	
				InFlow(WaterStorage[i], WaterDrainFlow[i-1]);
        
  
	  }
	  OutFlow(WaterStorage.back(), DeepPerc);
	}
	else
	  OutFlow(WaterStorage.back(), DeepPerc);

}

bool Soil_Drainage::End()
{
return true;
}
Soil_Drainage::~Soil_Drainage(void)
{
}
