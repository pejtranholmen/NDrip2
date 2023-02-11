#pragma once
#include "OutVector.h"	
class D :public OutVector {

public:
	D(std::vector<double> *target,NE *ne, enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev);

	~D();

	void Def(int module_no, std::string varname, std::vector<double> initv, std::string unit, std::string conditions);

protected:


};