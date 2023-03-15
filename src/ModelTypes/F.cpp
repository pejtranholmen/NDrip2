
#include "F.h"
#include "../Util/FUtil.hpp"
#include "../PG/PG_Stream.h"


#define BASE   1721424L  /* Jan 1, AD 1 */

F::~F()
{
}
F::F(CPG *ptr, enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev)
	:SimB(PGFILE, data, elem, fproc, bproc, ulev)
{
	SetPointer(ptr);
	m_pPGRes=nullptr;
	m_NumVariables=0;

}
void F::Def(size_t ModuleNo, string name, string value, string conditions)
{
	SetModuleNo(ModuleNo);
	m_FileName=value;
	SetName(name);
	SetConditions(conditions);
	SetHide(true);
	m_ValidationNumbersSelected=0;
	m_Exist=false;
	m_XBinExist=false;
}

 void F::SetOriginalValue()
{
	originalValue=m_FileName;
}
string F::GetOriginalStrValue()
{
	return originalValue;
}

void F::RestoreOriginalValue() 
{
	SetValue(originalValue);
	//SetBackOriginalValue();
}
bool F::ResetValNumbers()
{
	m_ValidationNumbers.resize(m_PGPointer->GetNumVariables()); for (size_t i = 0; i<m_ValidationNumbers.size(); i++) m_ValidationNumbers[i] = 0; m_ValidationNumbersSelected = 0; return true; 
	return false;
}
bool F::CallDataDescription() {
	if(m_DataDescription.size()>0) return true;
	if (m_PGPointer->AreAllValuesAssigned()) {
		m_DataDescription.clear();
		string str1, str2;
		for (size_t i = 0; i < m_PGPointer->GetNumVariables(); i++) {
			str1 = m_PGPointer->GetVarName(i + 1); FUtil::trim(str1);
			str2 = m_PGPointer->GetVarId(i + 1); FUtil::trim(str2);
			m_DataDescription.push_back(str1 + ' ' + str2);
		}
		return true;
	}
	else if(CheckFileNameAndAssignNameToPGClass() ) {
		m_DataDescription.clear();
		m_PGPointer->ReOpen();
		string str1,str2;
		for(size_t i=0; i<m_PGPointer->GetNumVariables();i++) {
			str1=m_PGPointer->GetVarName(i+1); FUtil::trim(str1);
			str2=m_PGPointer->GetVarId(i+1); FUtil::trim(str2);

			m_DataDescription.push_back(str1+' '+str2);
		}
		m_PGPointer->CloseFile();
		return true;
	}
	return false;
}

