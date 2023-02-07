#pragma once
#include "OutSingle.h"	
class Ds :public OutSingle {

public:
	Ds(double *target, datatype data, elements elem,  fysprocess fproc, bioprocess bproc, userlevel ulev);

	~Ds();

	void Def(int module_no, string varname, double initv, string unit, string conditions);  

protected:


};