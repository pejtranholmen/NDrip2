#pragma once
#include "OutBase.h"




OutBase::OutBase(enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev)
	:SimB(PAR, data,elem, fproc, bproc, ulev) 
{


}
OutBase::~OutBase(void)
{
}

