/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*=-=-=-=-=-                    CLASS: CSW                     -=-=-=-=-=*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "Tab.h"

//Destructor
Tab::~Tab()
{

}
Tab::Tab(vector<P*> *target,datatype data, elements elem, fysprocess fproc, bioprocess bproc, userlevel ulev)
	:SimB(TABLE, data, elem, fproc, bproc, ulev)
{
	_ptarget=target; 
	;
	_Hidden=true;
}

void Tab::Def(size_t ModuleNo,  string name, size_t minp_toenable)
{
	SetModuleNo(ModuleNo);
	SetName(name);
	_minp_toenable=minp_toenable;
	bool a=CheckEnabled();
	a=CheckOriginalValue();
	db_rows=0;
}
bool Tab::CheckEnabled()
{
	size_t count=0;
	for(size_t i=0; i<_ptarget->size(); i++) 
		if(_ptarget->at(i)->IsEnabled()) count++;
	if(count>=_minp_toenable) return true;
	else return false;
	//return SimB::IsEnabled();
}
bool Tab::CheckOriginalValue()
{
	bool Original=true;
	for(size_t i=0; i<_ptarget->size(); i++) {
		if(_ptarget->at(i)->IsEnabled()) {
			for(size_t ii=0; ii<_ptarget->at(i)->GetSize(); ii++) 
				if(_ptarget->at(i)->IsNotOriginalValue(ii)) Original=false;
			if(_ptarget->at(i)->GetSize()!=_ptarget->at(i)->GetOriginalSize()) Original=false;
		}
	}
	
	if(!Original) SetNotOriginalValue();
	else SetIsOriginalValue();

	return IsNotOriginalValue();
}


