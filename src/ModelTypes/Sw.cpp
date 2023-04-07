/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*=-=-=-=-=-                    CLASS: CSW                     -=-=-=-=-=*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "Sw.h"
using namespace std;
//Destructor
Sw::~Sw()
{

}
Sw::Sw(size_t *target, datatype data, elements elem, fysprocess fproc, bioprocess bproc, userlevel ulev)
	:SimB(SWITCH,data, elem, fproc, bproc, ulev)
{
	_ptarget=target; 
	;
}

void Sw::Def(size_t ModuleNo,  string name, int def, string opt, string conditions, int recalc)
{
	org_value=def;
	_orgIntValue = def;
	*_ptarget=def;
	SetName(name);
	SetModuleNo(ModuleNo);
	SetConditions(conditions);
	_IsOldValue=false;
	SetIsOriginalValue();

	string		option;
	if (opt.size()<=0) return;
	
	while(opt.size()>0) {
		int npos=opt.find('|');
		if(npos>0){
			option=opt.substr(0,npos);
			opt=opt.substr(npos+1);
		}
		else {
			option=opt;
			opt="";
		}
		_options.push_back(option);
	}
	m_recalc=recalc;

}
size_t Sw::SetStrValue(string str) {

	for(size_t i=0; i<_options.size(); i++) {
		if(str==_options[i]) {
			return SetIntValue(i);
		}
	}
	return 0;
}

size_t Sw::SetDBStrValue(string str)
{
	for (size_t i = 0; i<_options.size(); i++) {
		if (str == _options[i]) {
			 SetDBIntValue(i);
			 return _DBIntValue;
		}
	}
	return 0;

}
size_t Sw::GetNumOptions() {
	return _options.size();
}


