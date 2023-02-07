#pragma once
#include "OutVector.h"	
class G :public OutVector {

public:
	G(vector<double> *target, NE *, enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev);

	~G();

	void Def(int module_no, string varname, vector<double> initv, string unit, string conditions);  

protected:


};