#pragma once
#include "OutVector.h"	
class T :public OutVector {

public:
	T(std::vector<double> *target, NE *p, datatype data, elements elem, fysprocess fproc, bioprocess bproc, userlevel ulev);

	~T();

	void Def(size_t module_no, std::string varname, std::vector<double> initv, std::string unit, std::string conditions);

protected:


};