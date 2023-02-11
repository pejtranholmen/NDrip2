#pragma once
#include "OutVector.h"	
class G :public OutVector {

public:
	G(std::vector<double> *target, NE *, enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev);

	~G();

	void Def(int module_no, std::string varname, std::vector<double> initv, std::string unit, std::string conditions);

protected:


};