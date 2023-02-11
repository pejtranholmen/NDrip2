#pragma once
#include "OutVector.h"	
class NE;
class X :public OutVector {

public:
	X(std::vector<double> *target, NE *pNE,enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev);

	~X();

	void Def(size_t module_no, std::string varname, std::vector<double> initv, std::string unit, std::string conditions);  

protected:


};