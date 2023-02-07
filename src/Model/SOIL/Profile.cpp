#include "Profile.h"
#include "./Functors/Profile_Functors.h"

						   
Profile::Profile(void)
{
	ModuleName="Soil Profile";
	ModuleNo=4;
	ModuleConditions="";
}

bool Profile::Ini()	{
	auto ReCalcProfile = ModelProfileLayers(NL);

	ReCalcProfile(OriginalThickness, ThicknessLayers, AThicknessLayers, DistanceMidpLayers,
		DepthLayers, ModelUpperDepth, DepthMidpLayer, ModelLowerDepth);
	return true;
}
bool Profile::IniFunctors() {
	return true;
}
bool Profile::Dyn()
{
	Flux();
	Integ();
	return true;
}

void Profile::Flux()
{
	
}
void Profile::Integ()
{
}

bool Profile::End()
{
return true;
}
Profile::~Profile(void)
{
}
