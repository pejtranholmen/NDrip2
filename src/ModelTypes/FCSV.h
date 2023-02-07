#pragma once
#include "../std.h"
#include "./SimB.h"
#include <fstream>
#include <iostream>


class ModelBase;
class FCSV :public SimB {

public:
	FCSV(ifstream *, enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev);
	FCSV(definitions def);
	~FCSV(void);
	void Def(size_t ModuleNo, string name, string value, string conditions, size_t numheaders);

	bool CheckFileName(ModelBase *pModelBase=nullptr);
	bool CheckIfExistAndOpen() { if (m_Exist&&m_headervar.size() > 0) return true; else return false; };

	bool SetValue(string newvalue) {m_FileName=newvalue; return true;};
	bool SetDBValue(string str) {dbstrvalue=str; return true;};

	bool GetHide() {return m_hide;};
	void SetHide(bool value) {m_hide=value;};
	string GetStrValue() const {return m_FileName;};
	string GetDBStrValue() {return dbstrvalue;};
	void SetDBStrValue(string str) {dbstrvalue=str;};
			// resets tmpvalues to values
	virtual void	SetOriginalValue();		// Sets the original value to the current value
	string   GetOriginalStrValue();
	ifstream*	GetPointer() { return m_fstream; };
	void	SetPointer(ifstream *Ptr) { m_fstream = Ptr; };

	void CheckSizeOfFile();
	bool CheckMinMaxValues();
	size_t AssignSIMB_PointersFromHeader();
	string GetVarHeader(size_t index) { if (m_headervar.size() > index) return m_headervar[index]; else return ""; };
	string GetVarHeader_Unit(size_t index) { if (m_headervar_unit.size() > index) return m_headervar_unit[index]; else return ""; };


	double GetVarMin(size_t index) { if (m_VarMin.size() > index) return m_VarMin[index]; else return double(MISSING); };
	double GetVarMax(size_t index) { if (m_VarMax.size() > index) return m_VarMax[index]; else return double(MISSING); };
	size_t GetNumLines() { return m_LineNo; };
	size_t GetNumVariables() { return m_VarNo; };
	size_t GetNumHeadings() { return m_Headers; };
	void   SetNumHeadings(size_t numheadings) { m_Headers = numheadings; };
	size_t AssignKey_WithValues();
	vector<double> GetParValue(size_t keyindex);
	vector<string> GetCDB_Value(size_t keyindex);
	vector<double> GetMinVector() { return m_VarMin; };
	vector<double> GetMaxVector() { return m_VarMax; };
	vector<SIMB> GetSIMB_vector();
	vector<size_t> GetKeyVector();
	bool WithTableValues() { if (m_LineNo > 0 && m_Complete) return true;
		else return false;
	};

	bool m_Complete;
protected:
	ifstream *m_fstream;
	
	
	string 	extension;
	string		tmpvalue, originalValue, dbstrvalue;
	void *m_Ptr;
	string	m_FileName;
	int 		rec, frec, view, optionals;
	bool		m_hide;

private :
	ModelBase *m_pModelBase;
	bool m_Exist;
	size_t m_LineNo, m_VarNo;
	vector<string> m_headervar, m_headervar_unit;
	vector<double> m_VarMin;
	vector<double> m_VarMax;
	vector<SIMB> m_ParameterPointers;
	size_t m_Headers;
	map <size_t,size_t>KeyIndex;
	vector< vector<double>> m_Values;
	vector<  vector<string>> m_KeyValues;
};
