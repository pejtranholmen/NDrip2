#include "Numerical.h"
#include "../../Simulator/Simulator.h"

						   
Numerical::Numerical(void)
{
	ModuleName="Numerical";
	ModuleNo=2;
	ModuleConditions="";
}

bool Numerical::Ini()	{
	//m_pModelStructure = ((Simulator*)m_pModelInfo)->GetSimStructurePointer();
	Technical::Ini();


	//	Use TimeInfo, Only	: Itt, T_StepOrg

	if(TimeStepOption==1) {
		ThickMinimum.resize(p_Soil_WaterF->NL-1);
		OldTemperature.resize(p_Soil_WaterF->NL);
	   	for(size_t i=0;i<ThickMinimum.size(); i++) 
			ThickMinimum[i]=min(p_Soil_WaterF->ThicknessLayers[i],p_Soil_WaterF->ThicknessLayers[i+1]);
		
   	}

/*	if(FindTimeStep==1) {
		Allocate(WaterFlowOutputs(NL,Num_Output_Records))
		Allocate(TimeStepOutputs(Num_Output_Records))
		Allocate(Diff_At(Num_Output_Records))

		if (RESTART) {
			T_StepOrg=2*T_StepOrg/(2*ID_Loop_Count)
		else
			T_StepOrg=NEWDELTAT
		}
	}*/

	ITT=0;
	Next=true;
    m_pModelInfo->CAPILMAX=0.;


	return true;
}
bool Numerical::Dyn()
{

		//p_Soil_WaterF->Dyn();

		Technical::Dyn();
		Flux();
		Integ();

	return true;
}

void Numerical::Flux()
{
	Timestep=GetNewStep();
}
void Numerical::Integ()
{
}

bool Numerical::End()
{
return true;
}
Numerical::~Numerical(void)
{
}
