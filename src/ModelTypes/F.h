#pragma once
#include "../std.h"
#include "./SimB.h"
#include "../PG/PG.H"

class CPG;

class F :public SimB {

public:
	F(CPG *pPG, enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev);
	F(definitions def);
	~F(void);
	void Def(size_t ModuleNo,string name, string value,string conditions);
	bool CheckFileNameAndAssignNameToPGClass() {
        bool res=m_PGPointer->Open(m_FileName, false);
		if(res) { m_PGPointer->CloseFile(), m_Exist=true;}
			else m_Exist=false; 
		return res;  
	};
	bool IsValNumber(int index);
	bool m_Exist, m_XBinExist;
	string GetExtension();

	bool SetValue(string newvalue) {m_FileName=newvalue; return true;};
	bool SetDBValue(string str) {dbstrvalue=str; return true;};

	bool GetHide() {return m_hide;};
	void SetHide(bool value) {m_hide=value;};
	string GetStrValue() const {return m_FileName;};
	string GetDBStrValue() {return dbstrvalue;};
	void SetDBStrValue(string str) {dbstrvalue=str;};
	string GetDataDescription(size_t index) {if(index<m_DataDescription.size())  return m_DataDescription[index]; return "-";};
	bool    RemoveDataDescription(size_t);
	
	int		GetValNumber(size_t index) {if(index<m_ValidationNumbers.size())  return m_ValidationNumbers[index]; else return -1;};
	bool	SetValNumber(size_t index, int value) {if(index<m_ValidationNumbers.size()) { m_ValidationNumbers[index]=value; return true;} return false;}  ;
	bool	ResetValNumbers();
	size_t		GetNumberOfSelectedVal() {size_t count=0; for(size_t i=0; i<m_ValidationNumbers.size(); i++) 
		if(m_ValidationNumbers[i]>0) count++; 
		if(m_ValidationNumbersSelected>count) return m_ValidationNumbersSelected;
		return count;};
	bool	SetNumberOfSelectedVal(int value) {m_ValidationNumbersSelected=value;  return true;};
	
	bool	CallDataDescription();

	long	GetNumberOfVariables() {return m_NumVariables;};
	long	GetNumberOfRecords() {return m_NumRecords;};

	CPG*	GetPointer() {return m_PGPointer;};
	void	SetPointer(CPG *Ptr) {m_PGPointer=Ptr;};
	virtual void	RestoreOriginalValue();		// Restores the original value
	bool	Apply() {return true;};				// set values to tmpvalues
	void	PushedApply();						//Will allway set tmpvalue to value
	void	Reset() {};				// resets tmpvalues to values
	virtual void	SetOriginalValue();		// Sets the original value to the current value
	string   GetOriginalStrValue();
	void	 SetPGResPointer(CPG *pPG) {m_pPGRes=pPG;};
	CPG*	 GetPGResPointer() {return m_pPGRes;};
protected:
	CPG *m_PGPointer;
	vector<string> m_DataDescription;
	vector<int>	 m_ValidationNumbers;
	size_t		 m_ValidationNumbersSelected;
	long		 m_StartMin,m_EndMin;
	long		 m_NumVariables, m_NumRecords;		
	string 	extension;
	string		tmpvalue, originalValue, dbstrvalue;
	void *m_Ptr;
	CPG *m_pPGRes;
	string	m_FileName;
	int 		rec, frec, view, optionals;
	bool		m_hide;
};
