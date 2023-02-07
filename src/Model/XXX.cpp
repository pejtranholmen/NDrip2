#include "XXX.h"
#include "PhysConst.h"
						   
XXX::XXX(void)
{
	ModuleName="XXX";
	ModuleNo=34;
	ModuleConditions="XXX SOIL Module >0";
}

bool XXX::Ini()	{

	return true;
}
bool XXX::Dyn()
{
	Flux();
	Integ();
	return true;
}

void XXX::Flux()
{
	
}
void XXX::Integ()
{
}

bool XXX::End()
{
return true;
}
XXX::~XXX(void)
{
}
