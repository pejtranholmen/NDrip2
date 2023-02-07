#pragma once
#include "OutVector.h"	
class NE;
class X :public OutVector {

public:
	X(vector<double> *target, NE *pNE,enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev);

	~X();

	void Def(size_t module_no, string varname, vector<double> initv, string unit, string conditions);  

protected:


};