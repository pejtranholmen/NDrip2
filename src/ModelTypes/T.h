#pragma once
#include "OutVector.h"	
class T :public OutVector {

public:
	T(vector<double> *target, NE *p, datatype data, elements elem, fysprocess fproc, bioprocess bproc, userlevel ulev);

	~T();

	void Def(size_t module_no, string varname, vector<double> initv, string unit, string conditions);  

protected:


};