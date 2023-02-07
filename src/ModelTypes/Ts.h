#pragma once
#include "OutSingle.h"	
class Ts :public OutSingle {

public:
	Ts(double *target, datatype data, elements elem, fysprocess fproc, bioprocess bproc, userlevel ulev);

	~Ts();

	void Def(size_t module_no, string varname, double initv, string unit, string conditions);  

protected:


};