#pragma once
#include "OutSingle.h"	
class Xs :public OutSingle {

public:
	Xs(double *target, datatype data, elements elem, fysprocess fproc, bioprocess bproc, userlevel ulev);

	~Xs();

	void Def(size_t module_no, string varname, double initv, string unit, string conditions);  

protected:


};