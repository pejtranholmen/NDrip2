#pragma once
#include "OutVector.h"	
class D :public OutVector {

public:
	D(vector<double> *target,NE *ne, enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev);

	~D();

	void Def(int module_no, string varname, vector<double> initv, string unit, string conditions);  

protected:


};