#include "../std.h"
#include "Par.h"


Par::Par()
{
	 m_SelectedFunc=0;
	 _recalc_option=0;
}
Par::Par(definitions def):SimB(PAR, def) 
{
	 _recalc_option=0;
}
Par::Par(enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev)
	:SimB(PAR, data,elem, fproc, bproc, ulev) 
{
	 _recalc_option=0;
}

Par::~Par(void)
{
}
