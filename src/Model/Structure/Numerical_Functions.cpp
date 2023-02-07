#include "Numerical_Functions.h"
#include "../SOIL/SoilModel.h"
#include "../Atmosphere/AtmModel.h"
#include "../PLANT/PlantModel.h"
#include "../HBV/HBVModel.h"
#include "../PLANT_NC/PlantOrgModel.h"
#include "../SOIL_NC/SoilOrgModel.h"
#include "../Structure/Structure.h"

Numerical_Functions::Numerical_Functions(void)
{


}
Numerical_Functions::~Numerical_Functions(void)
{
}


bool Numerical_Functions::Def()
{


	Technical::Def();
	ModuleNo=2;
	ModuleConditions="";
 //	SoilModel* pSoil=(SoilModel*)pMod->GetModelPtr("Soil Model");
//	p_Soil_WaterF=&pSoil->m_SoilFrost;
Sw* pSw;
pSw= new Sw(&m_pModelInfo->NumMethod,SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
 
Define(pSw, "NumMethod", 1, "Central Difference|Forward Difference","OnlyNC <1");
pSw= new Sw(&TimeStepOption,SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
Define(pSw, "TimeStepOption",0,"Empirical|Mathematical|Constant","OnlyNC <1");
pSw= new Sw(&FindTimeStep,SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
Define(pSw, "FindTimeStep",0,"No|Yes","OnlyNC < 1");
pSw= new Sw(&NitrogenCarbonStep,SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
Define(pSw, "NitrogenCarbonStep",0,"TimeResolution determined|As Water and Heat|Independent","Nitrogen and Carbon >0");

Ps* pPs;
pPs= new Ps(&Xloop,   SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
Define(pPs, "Xloop", 1.,"#","TimeStepOption=0|OnlyNC < 1",1.,10.);
pPs= new Ps(&m_pModelInfo->XNLev,   SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
Define(pPs, "XNLev", 10.,"#","TimeStepOption=0|OnlyNC < 1",1., 100.);
pPs= new Ps(&XADiv,   SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
Define(pPs, "XADiv", 4.,"#","TimeStepOption=0|OnlyNC < 1",1., 16.,1., 8.);
pPs= new Ps(&SaftyPerCent,   SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
Define(pPs,  "SaftyPerCent", 80., "%", "TimeStepOption=1|OnlyNC < 1", 10.,30., 90., 1000.);
pPs= new Ps(&Precision,   SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
Define(pPs,  "Precision", 2., "%", "FindTimeStep=1|OnlyNC < 1", 0.01,5., 3., 5.);
pPs= new Ps(&NC_Iterations,   SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
Define(pPs,  "NC Iteration", 8., "#", "Nitrogen and Carbon >0|NitrogenCarbonStep =2", 1.,2048.);
Gs *pGs= new Gs(&Timestep,   SINGLE,  GENERAL, NOFPROC, NOBPROC, NORMAL);
Define(pGs, "Timestep", 0.,"log(day)","OnlyNC < 1");


//! Initialize Logical variables for adaptive solution of time step

	RESTART=true;
	APEAK=false;
	ID_Loop_Count=1;



return true;
}
double Numerical_Functions::GetNewStep() {

/*Use Timeinfo
Use Hydraulic, Only:	M_Saturation
Use commonstruc, Only: Frost, FrostInteract, &
					 SoilEvaporation, Theta, SatLev, SnowOutFlow
Use Drivings, Only	: IrrigationRate, WSource, SpoolRunOn*/

double TestLimit;

      m_pModelInfo->T_Step=m_pModelInfo->T_StepOrg;
	  m_pModelInfo->T_RatioOrgStep=1;
	  m_pModelInfo->T_CountAdd=0;
	  TestLimit=p_Irrigation->SnowOutFlow+max(p_Plant->ThroughFall+p_Irrigation->IrrigationRate, p_Irrigation->PrecCorr)+p_Irrigation->Wsource.front()+p_Irrigation->SpoolRunOn;
      TestLimit=min(500., TestLimit);  
//      if(T_Current>183.) {
 //           TESTLIMIt=TestLimit      
  //    }
      if(Next&&TimeResolution>=1) {
            m_pModelInfo->XFMAX=max(TestLimit, m_pModelInfo->XFMAX);
            m_pModelInfo->T_Step=m_pModelInfo->T_StepOrg/int(XADiv);
			m_pModelInfo->T_RatioOrgStep=int(XADiv);
			m_pModelInfo->FIRST=false;
			m_pModelInfo->T_CountAdd=0;
	  }
      else if(TimeResolution>=1) {
            m_pModelInfo->XFMAX=max(TestLimit, 50.);
			m_pModelInfo->XFMAX=max(m_pModelInfo->XFMAX, 0.75*m_pModelInfo->XFMAX);
	  }
      else {           
		    m_pModelInfo->XFMAX=max(TestLimit, 500.);
			m_pModelInfo->XFMAX=max( m_pModelInfo->XFMAX,0.75*m_pModelInfo->XFMAX);
	  }

      if(p_Soil_WaterF->Frost&&p_Soil_WaterF->FrostInteract>=1) 
        m_pModelInfo->CAPILMAX=5;
      else
        m_pModelInfo->CAPILMAX=min(100.,max(p_Irrigation->SoilEvaporationFlux, 5.));
      

      Next=false;

	  if(TimeStepOption==0) {
	      if(p_Soil_WaterF->Frost) {
	        m_pModelInfo->T_Step=m_pModelInfo->T_StepOrg/int(XADiv);
			m_pModelInfo->T_RatioOrgStep=int(XADiv);
			m_pModelInfo->T_CountAdd=0;
		  }
	      else if(TestLimit>10.) {
	        m_pModelInfo->T_Step=m_pModelInfo->T_StepOrg/int(XADiv);
			m_pModelInfo->T_RatioOrgStep=int(XADiv);
			m_pModelInfo->FIRST=false;
			m_pModelInfo->T_CountAdd=0;
	        Next=true;
		  }
	      else if(m_pModelStructure->GroundWaterFlow>0&&p_Soil_WaterF->SatLev>-0.80)  {
	        m_pModelInfo->T_Step=m_pModelInfo->T_StepOrg/int(XADiv);
			m_pModelInfo->T_RatioOrgStep=int(XADiv);
			m_pModelInfo->T_CountAdd=0;
		   }
	      else if(p_Soil_WaterF->M_Saturation.front()>85&&p_Soil_WaterF->Theta.front()<5.) {
	        m_pModelInfo->T_Step=m_pModelInfo->T_StepOrg/int(XADiv);
			m_pModelInfo->T_RatioOrgStep=int(XADiv);
			m_pModelInfo->T_CountAdd=0;
		  }

	  }
	  else if(TimeStepOption==1) 
		   Timestep=GetNewStep();
	  else {
		   m_pModelInfo->T_Step=m_pModelInfo->T_StepOrg;
		   m_pModelInfo->T_RatioOrgStep=1;
		   m_pModelInfo->T_CountAdd=0;
	  }
	  

      return log10(m_pModelInfo->T_Step);

}



