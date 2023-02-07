#include "Technical.h"

						   
Technical::Technical(void)
{
	ModuleName="Technical";
	ModuleNo=1;
	ModuleConditions="";
}

bool Technical::Ini()	{
	
	T_Step=*pT_Step;


	CandidateValues.resize(10);
	First=AcceptedRun=true;
	CalibrationMode=false;


	return true;
}
bool Technical::Dyn()
{

		//p_Soil_WaterF->Dyn();


		Flux();
		Integ();

	return true;
}

void Technical::Flux()
{
	
}
void Technical::Integ()
{
}

bool Technical::End()
{
return true;
}
Technical::~Technical(void)
{
}
