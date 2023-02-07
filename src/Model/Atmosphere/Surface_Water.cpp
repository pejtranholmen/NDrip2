#include "./Surface_Water.h"
#include "../PhysConst.h"
#include "../SOIL/Soil_HeatPump.h"
#include "../../Simulator/Simulator.h"
						   
Surface_Water::Surface_Water(void)
{
	ModuleName="Surface_Water";
	ModuleNo=14;
	ModuleConditions="Surface_Water>0";
}

bool Surface_Water::Ini()	{
	//m_pModelStructure = ((Simulator*)m_pModelInfo)->GetSimStructurePointer();
	if(!SoilEvaporation::Ini()) return false;

	if(!m_pModelInfo->StartStateValue) 
		SurfacePool=SurfPoolInit;
	PumpPool=0.;

	return true;
}
bool Surface_Water::IniFunctors() {

	if (!SoilEvaporation::IniFunctors()) return false;


	f_SurfaceRunOff = Get_D1_Functor(MyFunc::SURFACE_RUNOFF);
	f_SurfaceWaterCover = Get_D1_Functor(MyFunc::SURFACE_WATER_COVER);

	return true;
}
bool Surface_Water::Dyn()
{
	SoilEvaporation::Dyn();

	Flux();
	Integ();
	return true;
}

void Surface_Water::Flux()
{
Soil_HeatPump *pHeat=(Soil_HeatPump*)p_Soil_HeatF;
double ThetaLiquid, WCond, WCondFrozen, WCondLiquid, ThetaFrozen, SurfaceCoverFlux;//		! Local Variables
double Gradient, ThetaUpperLayer, PsiUpper;

double	PotInfil, ExtraInfil;

   SpoolCover=f_SurfaceWaterCover(SurfacePool);

   p_Soil_WaterF->ThetaTotal.front()=p_Soil_WaterF->Theta.front()+p_Soil_WaterF->Theta.front()*pHeat->ThQual.front()*0.1+
									 p_Soil_WaterF->WaterHFD.front()/p_Soil_WaterF->ThicknessLayers.front()/10.;

  
	if(p_Plant->ThroughFall>0||SnowOutFlow>0.||IrrigationRate>0||SurfacePool>0.) {
      if(p_Soil_WaterF->FrostInfil==1) 
        ThetaLiquid=p_Soil_WaterF->M_Saturation.front()-p_Soil_WaterF->Theta.front()*pHeat->ThQual.front();
      else
        ThetaLiquid=p_Soil_WaterF->M_Saturation.front();
      
	  WCond=p_Soil_WaterF->f_CondTheta(1, ThetaLiquid, pHeat->Temp.front());  
      if(p_Soil_WaterF->FrostInfil==2&&pHeat->ThQual.front()>.001) {
        ThetaFrozen=p_Soil_WaterF->Theta.front()*pHeat->ThQual.front()*1.1;
        ThetaLiquid=p_Soil_WaterF->Theta.front()*(1-pHeat->ThQual.front());
        WCondFrozen=p_Soil_WaterF->f_CondTheta(1, ThetaFrozen+ThetaLiquid, pHeat->Temp.front());
        WCondLiquid=p_Soil_WaterF->f_CondTheta(1, ThetaLiquid, pHeat->Temp.front());
        WCond=max(0.1E-12, WCond-WCondFrozen+WCondLiquid);
        FracInfiltrationHighFlow=min(1., max(0.,1-WCondLiquid/WCond));
	  }
      else
        FracInfiltrationHighFlow=0.;
      

	  ThetaUpperLayer=p_Soil_WaterF->WaterStorage.front()/p_Soil_WaterF->ThicknessLayers.front()/10.;
	  PsiUpper=p_Soil_WaterF->f_PsiTheta_M(1,ThetaUpperLayer);


	  Gradient=max(1., PsiUpper/(p_Soil_WaterF->ThicknessLayers.front()*50.));

      SoilInfil=(1-SpoolCover)*Gradient*min(WCond, 
	    max(0.,(p_Soil_WaterF->M_Saturation.front()-p_Soil_WaterF->ThetaTotal.front()/1.08)*p_Soil_WaterF->ThicknessLayers.front()*10./(2*T_Step)));
	 
	  if(WTotSnow>1.E-8||SnowOutFlow>0) 
		  PotInfil=SnowOutFlow;
	  else
		  PotInfil=p_Plant->ThroughFall;
	  
      if(Furrow<=0) {
			SoilInfil=min(SoilInfil,PotInfil+SoilIrrigF*IrrigationRate);
			Spoolinflow=PotInfil+SoilIrrigF*IrrigationRate-SoilInfil;
	  }
	  else {
			SoilInfil=min(SoilInfil,PotInfil);
			Spoolinflow=PotInfil+SoilIrrigF*IrrigationRate-SoilInfil;
	  }
	}
   else {
      Spoolinflow=0.;
      SoilInfil=0.;
	  ThetaUpperLayer=p_Soil_WaterF->WaterStorage.front()/p_Soil_WaterF->ThicknessLayers.front()/10.;
	  PsiUpper=p_Soil_WaterF->f_PsiTheta_M(0,ThetaUpperLayer);
	  Gradient=max(1., PsiUpper/(p_Soil_WaterF->ThicknessLayers.front()*50.));
   }

   if(m_pModelStructure->LateralInput>=1) 
   	   Spoolinflow+=SpoolRunOn;
	

//! Deleted bug August 98 by moving Gradient from without AMIN1 function to inside.
//!Caused problems with high gradient and small negative SurfacePool values

    SpoolSoilInfil=0.;
    if(SurfacePool>0.001) {
       SpoolSoilInfil=SpoolCover*min(WCond*Gradient, SurfacePool/(4*T_Step));
	   SpoolSoilInfil=min(SpoolSoilInfil,
		 max(0.,(p_Soil_WaterF->M_Saturation.front()-p_Soil_WaterF->ThetaTotal.front()/1.08)*p_Soil_WaterF->ThicknessLayers.front()*10./(4*T_Step)));
       SpoolSoilInfil=max(SpoolSoilInfil, 0.0);
	}
    else
	     SpoolSoilInfil=max(0.,SurfacePool/T_Step);
    
//!c940920
    if(Furrow>0) {
      FurrowInfil=SpoolSoilInfil*InfFurrow;
      SpoolSoilInfil=SpoolSoilInfil-FurrowInfil;
      if(Furrow==2) {
        FurrowPrec=SoilInfil*InfFurrow;
        FurrowInfil=FurrowInfil+FurrowPrec;
        SoilInfil=SoilInfil*(1-InfFurrow);
      }
    }

//!
  SoilInfil=SoilInfil+SpoolSoilInfil;
  SpoolRunoff=f_SurfaceRunOff(SurfacePool);

  if(SoilEvaporationFlux>SoilInfil&&SurfacePool/T_Step>SoilEvaporationFlux) {
		ExtraInfil=min(SoilEvaporationFlux, SurfacePool/T_Step);
		SoilInfil+=ExtraInfil;
		SpoolSoilInfil+=ExtraInfil;
  }


//!c***** Accounting for a cover of the soil

    if((SoilCover>=0.&&SoilCover<=1.)||MulchInUse) {
		if(MulchInUse) 
			SurfaceCoverFlux=SoilCoverRunOff*max(0., SoilInfil-SoilIrrigF*IrrigationRate);
		else
			SurfaceCoverFlux=SoilCover*max(0., SoilInfil-SoilIrrigF*IrrigationRate);
		
		SoilInfil=SoilInfil-SurfaceCoverFlux;
		SpoolRunoff+=SurfaceCoverFlux;
		Spoolinflow+=SurfaceCoverFlux;
	} 
	
	
	if(PumpStation>0) {
	    PumpInFlow=PumpPartitioning*SpoolRunoff;
	    
	    if(PumpPool>PumpLevelStart)
	        PumpOutFlow=PumpRate;
	    else if( PumpPool<=PumpLevelStop) 
	        PumpOutFlow=0.;
	    
	                
	    SpoolDelayRunoff=(1-PumpPartitioning)*SpoolRunoff+PumpOutFlow;
		
	}

}
void Surface_Water::Integ()
{




	InFlow(p_Soil_WaterF->WaterStorage.front(), SoilInfil);
//! Corrected error on January 12 SOOLInfil was replaced by SPOOLSoilInfil

if(Furrow>0) {
   InFlow(p_Soil_WaterF->WaterStorage[1], FurrowInfil);
   OutFlow(SurfacePool, FurrowInfil-FurrowPrec);
}

   InFlow(SurfacePool, Spoolinflow);

   InFlow(SurfacePool, p_Soil_WaterF->SurfaceOutFlow);
   OutFlow(p_Soil_WaterF->WaterStorage.front(), p_Soil_WaterF->SurfaceOutFlow);

   OutFlow(SurfacePool, SpoolSoilInfil);
   OutFlow(SurfacePool, SpoolRunoff);

	if(PumpStation>0) {
	
	   InFlow(PumpPool, PumpInFlow);
	   OutFlow(PumpPool, PumpOutFlow);    
	   	
	}

}

bool Surface_Water::End()
{
	SoilEvaporation::End();
return true;
}
Surface_Water::~Surface_Water(void)
{
}
