#pragma once
#include "./OutSingle.h"	
class Gs :public OutSingle {

public:
	Gs(double *target, enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev);

	~Gs();

	void Def(int module_no, string varname, double initv, string unit, string conditions);  

protected:


};
