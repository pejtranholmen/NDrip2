#include "../std.h"
#include "P.h"
#include "NE.h"


P::P(std::vector<double> *target, definitions def): Par(def)
{
	_ptarget=target;
	;
}
P::P(std::vector<double> *target,NE *pE, enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev)
	:Par(data, elem, fproc, bproc, ulev)
{
	_ptarget=target;
	pNE=pE;
	SetSimType(PAR_TABLE);

	;
}
void P::Def(size_t ModuleNo, string name, vector<double> init, string unit, string conditions, 
			double errormin, double errormax, double warningmin, double warningmax,
			int recalc, int view)
{
SetModuleNo(ModuleNo);
SetConditions(conditions);
SetViewFunction(view);
SetReCalcOption(recalc);
SetUnit(unit);
SetValue(init);
SetName(name);
ReSize(init.size());
SetMinValue(errormin); SetMaxValue(errormax); SetWarnMinValue(warningmin); SetWarnMaxValue(warningmax); 
SetOriginalValue(init);
ResetOldValue(init);
for(size_t i=0; i<init.size(); i++)	{	
	SetNotOriginalIndicator(i, false);
	_mr[i].Min=_mr[i].Max=MISSING;
}
SetSelectedFunc(0);

}

P::~P(void)
{
}
