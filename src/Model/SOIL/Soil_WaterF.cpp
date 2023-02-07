#include "Soil_WaterF.h"
#include "Soil_HeatF.h"
#include "../PLANT/WaterUptake.h"
#include "../Atmosphere/Irrigation.h"
#include "../PLANT/PlantModel.h"
#include "../../Simulator/Simulator.h"

						   
Soil_WaterF::Soil_WaterF(void)
{
	ModuleName="Soil water flows";
	ModuleNo=18;
	ModuleConditions="WaterEq>0";
}

bool Soil_WaterF::Ini()	{
	Soil_Drainage::Ini();
	//m_pModelStructure = ((Simulator*)m_pModelInfo)->GetSimStructurePointer();
	
    if(!m_pModelInfo->StartStateValue) {
	 for(size_t i=0; i<Theta.size(); i++) {	
       if(InitialWater==1) 
	        Theta[i]=min(IniThetaValue, M_Saturation[i]);
	   else if(InitialWater==4) 
			Theta[i]=min(IniThetaValues[i], M_Saturation[i]);
       else if(InitialWater==3) 
			 Theta[i]=f_ThetaPsi_M(i, IniPsiValues[i]);
       else if(InitialWater==2) 
			Theta[i]=ThetaFlow(i, IniFlowRate/14400., ThicknessLayers[i]);
       else if(InitialWater==0) {
		 double PsiX;
         if(m_pModelStructure->GroundWaterFlow>0) 
            PsiX=min(IniPsiValue,-100*(DepthLayers[i]-0.5*ThicknessLayers[i]+IniGWLev));
        else 
           PsiX= IniPsiValue;
        
		Theta[i]=f_ThetaPsi_M(i, PsiX); 
		Psi[i]=PsiX;	
       }
       WaterStorage[i]=Theta[i]*ThicknessLayers[i]*10.;
	   

	}
	ThetaTotal = Theta;
	PlantModel *pPlant=(PlantModel*)m_pModelInfo->GetModelPtr("Plant Model");
	WaterUptake *p_Plant;
	p_Plant=&pPlant->m_WaterUptake;
	if(!p_Plant->Ini()) return false;

	if(m_pModelStructure->GroundWaterFlow>0) {
		   Below=false;
		   m_pModelInfo->XFMAX=100.;
		    if(m_pModelStructure->GroundWaterFlow==1) {
				for(size_t i=1; i<Theta.size(); i++) 
				 auto test=min(DrainFlow(i), m_pModelInfo->XFMAX);
			}
	}
  }
  else
        for(size_t i=0; i<Theta.size(); i++) Theta[i]=WaterStorage[i]/(ThicknessLayers[i]*10.);
   
  ThetaTotal=Theta;
//! Hysteresis 
  int nl=Theta.size();
  TposShift.resize(nl); 
  THETAO.resize(nl); 
  ThetaDer.resize(nl);
  A_PsiSh.resize(nl);
  A_Age.resize(nl); 
  A_Der.resize(nl); 
  AccDer.resize(nl);
  IcountN.resize(nl);
  Icount.resize(nl);

   if(ConvectiveGasFlow>0) {
		 MassAir.resize(nl);
		 FluxAir.resize(nl);
   }
  
	Soil_HeatF* p_Heat=(Soil_HeatF*)pHeat;   
    for(size_t i=0; i<Theta.size(); i++) {
		A_Age[i]=0.;
        TposShift[i]=m_pModelInfo->T_Current-10.;
        THETAO[i]=Theta[i];
        A_PsiSh[i]=0.;
        A_Der[i]=0.;
        AccDer[i]=0.;
        IcountN[i]=0;
        Icount[i]=0;
		if(ConvectiveGasFlow>0) {     
			MassAir[i]=MassAirFuncL(i,p_Heat->Temp[i]);
			FluxAir[i]=0.0;
		}       
	}
     FluxAirInfil=0.;
	m_pModelInfo->XFMAX=200.; //	! Initilize start conditions


   if(TransitTime) {
	    AccWaterFlow.resize(nl);
	    AccWaterStorage.resize(nl);
   }

   AccWaterFlow.assign(nl,0.);
   AccWaterStorage.assign(nl,0.);
   m_pModelInfo->CAPILMAX=0.;
   NCalc=NL;
  
   ThetaMMM(); 
  // f_PsiTheta_M = Get_D1_TableFunctor(MyFunc::PSI_THETA);

   return true;
}
bool Soil_WaterF::IniFunctors() {
	Soil_Drainage::IniFunctors();


	return true;
}
bool Soil_WaterF::Dyn()
{
	if (m_pModelStructure->WaterEq > 0) {
		Flux();
		Soil_Drainage::Dyn();
		Soil_WaterVerticalSaturated_Dyn();
		TotalRunoff = SumDrainage();
		if (m_pModelStructure->SoilVapour >= 1) WaterVapourF_Dyn();

		Integ();
	}
	return true;
}

void Soil_WaterF::Flux()
{
/*	Use Timeinfo
Use Drainage, Only	: DrainFlow, Below, Zero,  LayerOfDrain, &
						PhysicalDrainEq, Gw_LowestLayer, DeepPerc
Use CommonStruc, Only	: WUptakeRate,LayerOfSatLevel
Use Numerical, Only	: CapilMax*/
Soil_HeatF* p_Heat=(Soil_HeatF*)pHeat; 
WaterUptake* p_Plant=(WaterUptake*)pPlant;
Irrigation* p_Atm=(Irrigation*)pAtm;
double PsiGra, MaxThet, WcondM, WcondG, OvFlow, Diff, MatFlow, Sorp;

//!Vertical SOIL WATER FLOW ***************************************



  WaterDrainFlow.assign(NL,0.);
  WBypassFlow.assign(NL,0.);
  WaterFlow.assign(NL,0.);
	   
Below=false;

//Loop_over_layers:      DO i=1,NCALC
//! 
for(size_t i=0; i<NCalc; i++) {
      if(!Frost||FrostInteract==0) {
        ThetaTotal[i]=Theta[i];
       // Psi[i]=PsiFunc(i,Theta[i]);
		Psi[i] = f_PsiTheta_M(i, Theta[i]);
      }

      if(Hysteresis>=1) 
		 HysEffect[i]=Hys_Eff_Func(i);
	  

      if(Hysteresis==1||Hysteresis==3) 
           Psi[i]=Psi[i]*pow(10.,HysEffect[i]*HysMaxEffRet[i]);
      

      if(i==0) continue ;//cycle loop_over_layers

//! 
//!C*************** WATER POTENTIAL BENEATH GROUND WATER TABLE ************
//! 
      if(m_pModelStructure->GroundWaterFlow==1) {
         WaterDrainFlow[i-1]=min(DrainFlow(i), m_pModelInfo->XFMAX);

		 if(Below) {
			if(Zero&&p_Heat->ThQual[i]<.05) { 
			    MaxThet=max(Theta[i], ThetaTotal[i]);
				WaterFlow[i-1]=(M_Saturation[i]-MaxThet)*ThicknessLayers[i]*10/T_Step;
				WaterFlow[i-1]=max(-10.,min(m_pModelInfo->XFMAX, WaterFlow[i-1]));
				if(WaterFlow[i-1]<-5.) {
					size_t ij=0;

				}
			}
			else {
		        WaterFlow[i-1]=0.;
                if(i==1) {
                    WaterDrainFlow[i-1]=max(WaterDrainFlow[i-1], 0.);
					WaterDrainFlow[i-1]=max(WaterDrainFlow[i-1],(ThetaTotal[i-1]-M_Saturation[i-1])*ThicknessLayers[i-1]*10);
                }
				if(p_Heat->ThQual[i-1]>0.05||p_Heat->ThQual[i]>0.05) {
					//Disable drainage when frozen conditions
					WaterDrainFlow[i-1]=0.;
				}
	        }

			continue; //Cycle Loop_Over_layers
		 }
      }

 
//!C*************** SOIL WATER FLOW ***************************************
//! 
    PsiGra=(Psi[i]-Psi[i-1])/DistanceMidpLayers[i]/100.;
/*!
!C941219 Changed condition for oversaturation to avoid problems
!C       due to load potential that means that saturation and 
!C       and Psi=0 did not corresponded exactly during frozen conditions    
!C931217 Changed prevention for oversaturation
!C9505   Included prevention of oversaturation in partly frozen soil*/
      MaxThet=max(Theta[i], ThetaTotal[i]);
      if(MaxThet>M_Saturation[i]+0.01&&i>LayerOfSatLevel) {
          WaterFlow[i-1]=(M_Saturation[i]-MaxThet)*ThicknessLayers[i]*10/T_Step;
		  WaterFlow[i-1]=max(-5., WaterFlow[i-1]);
	  }
//!c931030 changed threshold from 0 to 10 cm of water for gravity flow
//!C950125 Deleted condition for gravity flow when Psi<5 cm water
      else if(Frost&&PsiGra>0.) {
        //  WcondM=WCondB(i-1, Theta[i-1], p_Heat->Temp[i-1]);
		  WcondM = f_CondTheta(i - 1, Theta[i - 1], p_Heat->Temp[i - 1]);
          WaterFlow[i-1]=min(m_pModelInfo->XFMAX, WcondM);
	  }
      else {
		if(FrostKUpward==1&&p_Heat->ThQual[i-1]>0.&&PsiGra<-1.) 
			ThetaB[i-1]=min(Theta[i-1],ThetaB[i-1]);
		
//!CPEJ 910711 Added PsiGra<0 from suggestion by HJ
        if(p_Heat->ThQual[i-1]>0.001&&PsiGra<0) {
		//	WcondM=WCondBF(i-1, ThetaB[i-1], p_Heat->Temp[i-1], p_Heat->ThQual[i-1]);
			WcondM = f_CondThetaFrozen(i - 1, ThetaB[i - 1], p_Heat->Temp[i - 1], p_Heat->ThQual[i - 1]);
			if (m_pModelInfo->NumMethod == 1) {
			//	WcondG = WCondBF(i - 1, Theta[i - 1], p_Heat->Temp[i - 1], p_Heat->ThQual[i - 1]);
				WcondG = f_CondThetaFrozen(i - 1, Theta[i - 1], p_Heat->Temp[i - 1], p_Heat->ThQual[i - 1]);
			}
		}
        else {
			//WcondM=WCondMat(i-1, ThetaB[i-1],p_Heat->Temp[i-1]);
		  WcondM=f_CondThetaMatric(true, i - 1, ThetaB[i - 1], p_Heat->Temp[i - 1]);
		  if (m_pModelInfo->NumMethod == 1) {
		//	  WcondG = WCondB(i - 1, Theta[i - 1], p_Heat->Temp[i - 1]);
			  WcondG=  f_CondTheta(i - 1, Theta[i - 1], p_Heat->Temp[i - 1]);
		  }
          
	    }
 
        if(Psi[i]<0.) PsiGra=min(-1.,PsiGra);
        if(Hysteresis>=2) {
          WcondM*=pow(10.,HysEffect[i-1]*HysMaxEffCond[i-1]);
          WcondG*=pow(10.,HysEffect[i-1]*HysMaxEffCond[i-1]);
        }
        if(m_pModelInfo->NumMethod==1&&i<=LayerOfSatLevel) 
          WaterFlow[i-1]=WcondM*PsiGra+WcondG;
        else
          WaterFlow[i-1]=WcondM*(1+PsiGra);
        

        if(!Frost) m_pModelInfo->CAPILMAX+=p_Plant->WUptakeRate[i];

        WaterFlow[i-1]=max(-m_pModelInfo->CAPILMAX,min(m_pModelInfo->XFMAX,WaterFlow[i-1]));
        if(WaterFlow[i-1]>-1.E4&&WaterFlow[i-1]<1.E4) 
            Below=false ;
        else
            Below=false;  
        
        
      }
      if(Crack&&p_Atm->SoilInfil>1.) {
        if(i==1) 
          OvFlow=p_Atm->SoilInfil;
        else
          OvFlow=WaterFlow[i-2];
        
        Diff=OvFlow-WaterFlow[i-1];
        MatFlow=WaterFlow[i-1];
//!cPEJ900506 Corrected argument to ALOG10 to avoid negative numbers
        Sorp=M_SatMatCond[i-1]*ThicknessLayers[i-1]*AScaleSorp*log10( max(10.,Psi[i-1]) );
        if(Diff>Sorp&&Psi[i]>0) 
          WaterFlow[i-1]=OvFlow-Sorp;
        else if(Psi[i]<0) 
          WaterFlow[i-1]=min(WaterFlow[i-1], 0.);
        
		 WBypassFlow[i-1]=max(0., WaterFlow[i-1])-MatFlow;
	  }
      else
        WBypassFlow[i-1]=0.;
      
//!C     Check for oversaturation	if groundwater is considered
	  if(ThetaTotal[i-1]>M_Saturation[i-1]&&m_pModelStructure->GroundWaterFlow==1) {
         WaterDrainFlow[i-1]=max(WaterDrainFlow[i-1], 
	    (ThetaTotal[i-1]-M_Saturation[i-1])*ThicknessLayers[i-1]*10);
		  WaterDrainFlow[i-1]=max(WaterDrainFlow[i-1], 0.);
      } 
}
//Enddo Loop_over_Layers

if(!Below&&m_pModelStructure->GroundWaterFlow==1) //{ ! Adjust groundwaterlevel from last layer
  	 GW_LowestLayer();




 

}
void Soil_WaterF::Integ()
{

	IntFlow(WaterStorage, WaterFlow);
}

bool Soil_WaterF::End()
{
return true;
}
Soil_WaterF::~Soil_WaterF(void)
{
}


