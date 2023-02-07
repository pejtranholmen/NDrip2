
#include "FuncArchive.h"

FunArchive::FunArchive()
{
	FunctorClass f(FUNC_CHOICE::PM);
	auto y = f(3.23,.12);
	y = f(3.4, 5);
	y = f(FUNC_CHOICE::SOILTEMP_ANALYTICAL_YEAR,3.6 );
	y = f(10., 10., 30., 2.4);
}

FunArchive::~FunArchive()
{
}
