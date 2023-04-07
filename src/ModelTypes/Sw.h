#pragma once
#include "SimB.h" 
using namespace std;
class Sw : public SimB {
 public:
	Sw(size_t *target, enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev);

	~Sw();
	void Def(size_t module_no, string varname, int initv, string options, string conditions, int recalc);  
public:
	virtual size_t GetIntValue() const {return *_ptarget;};
	size_t GetOriginalIntValue() const {return org_value;};
	size_t SetIntValue(size_t ivalue) {old_value=*_ptarget;_IsOldValue=true;  *_ptarget=ivalue; SendNotice();if(*_ptarget!=org_value) SimB::SetNotOriginalValue();return m_recalc;};
	void SetNotOldValue() {_IsOldValue=false;};
	std::string GetOption(size_t index) {
		if(index<_options.size()) 
			return _options[index]; 
		else 
			return ""; 
	};
	size_t GetNumOptions();
	bool IsOldValue() {return _IsOldValue;};
	size_t SetStrValue(string str);
	size_t SetDBStrValue(string str);
	std::string GetName() {return SimB::GetName();};
	void SetDBIntValue(size_t value) {_DBIntValue=value; };
	size_t GetDBIntValue() {return _DBIntValue;};
	size_t GetOldIntValue() {return old_value;};
protected:
	size_t *_ptarget;
	size_t org_value;
	size_t old_value;
	bool _IsOldValue;
	size_t m_recalc;
private:
	vector <std::string> _options;
};