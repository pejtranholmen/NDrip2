
#pragma once
#include "../NewBase/ModelBase.h"
#include "FCSV.h"
#include "../Util/FUtil.hpp"

size_t NUMCHAR = 9000;


FCSV::~FCSV()
{
}
FCSV::FCSV(ifstream *ptr, enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev)
	:SimB(CSVFILE, data, elem, fproc, bproc, ulev)
{
	SetPointer(ptr);


}
void FCSV::Def(size_t ModuleNo, string name, string value, string conditions, size_t numheaders)
{
	SetModuleNo(ModuleNo);
	m_FileName=value;
	SetName(name);
	SetConditions(conditions);
	SetHide(true);
	m_headervar.clear();
	m_Exist = false;
	m_Headers = numheaders;


}

bool FCSV::CheckFileName(ModelBase *pModelBase) {
	m_fstream->clear();
	m_fstream->close();
	m_fstream->open(m_FileName);
	m_pModelBase = pModelBase;
	if (m_fstream->good()) { 
		CheckSizeOfFile();
		CheckMinMaxValues();
		m_fstream->close();
		m_fstream->clear();
		m_Exist = true;
	}
	else m_Exist = false;
	
	return m_Exist;
}


 void FCSV::SetOriginalValue()
{
	originalValue=m_FileName;
}
string FCSV::GetOriginalStrValue()
{
	return originalValue;
}

void FCSV::CheckSizeOfFile()
{
	string line;
	line.resize(NUMCHAR);
	size_t count=0;
	size_t var_count=0;
	size_t max_varcount=0;
	enum HeaderTypes {No, SimpleOneLine, TwoLineType, ThreeLine, FourLine, FiveLine, SixLine, Complete};

	enum HeaderTypes option =HeaderTypes(m_Headers);
	m_headervar.resize(0);
	m_headervar_unit.resize(0);

	while (m_fstream->good()) {
		count++;
		m_fstream->getline(&line[0],NUMCHAR);
		size_t n = m_fstream->gcount();
		var_count = 0;
		line.resize(n);
		char delim = ',';
		if (line.find(';') != string::npos)  delim = ';';

		while (line.size()>0) {
			var_count++;
			string TheValueString;
			if (line.find(delim) != string::npos)
				TheValueString = line.substr(0, line.find(delim));
			else
				TheValueString = line.substr(0, line.find('\0'));


			if (count == 1&& option==TwoLineType) m_headervar_unit.push_back(TheValueString);
			if (count== 1&&option>TwoLineType||option==SimpleOneLine) m_headervar.push_back(TheValueString);
			if (count == 3 && option > TwoLineType) m_headervar_unit.push_back(TheValueString);
			
			if (line.find(delim) != string::npos)
				line = line.substr(line.find(delim) + 1);
			else
				line.resize(0);

		}
		line.resize(NUMCHAR);
		max_varcount = max(max_varcount, var_count);
	}
	if (option == Complete) {
		if (count - 2 >= m_Headers)
			m_LineNo = count - m_Headers - 2;
		else
			m_LineNo = 0;

	}
	else
		m_LineNo = count - m_Headers;
	m_VarNo = max_varcount;


}

bool FCSV::CheckMinMaxValues()
{
	m_fstream->clear();
	m_fstream->seekg(0, ios::beg);
	string line;
	line.resize(NUMCHAR);
	size_t count = 0;
	size_t var_count = 0;
	size_t max_varcount = 0;
	m_VarMin.resize(GetNumVariables());
	m_VarMax.assign(GetNumVariables(), -1.E38);
	m_VarMin.assign(GetNumVariables(), 1.1E38);


	enum vartyp{ MinValue, MaxValue, Unknown };
	enum vartyp option;

	while (m_fstream->good()) {
		count++;
		m_fstream->getline(&line[0], NUMCHAR);
		size_t n = m_fstream->gcount();
		var_count = 0;
		line.resize(n);
		char delim = ',';
		if (line.find(';')!=string::npos)  delim = ';';
	
		option = Unknown;
		
		if (line.find("Min") == 0) option = MinValue;
		else if (line.find("Max") == 0) option = MaxValue;

		while (line.size()>0) {
			var_count++;
			string TheValueString;
			if (line.find(delim) != string::npos)
				TheValueString = line.substr(0, line.find(delim));
			else
				TheValueString = line;



			if ((count > m_Headers&&m_Headers<2)||(count-1>m_Headers&&m_Headers==7&&m_LineNo>0)) {
				m_VarMin[var_count - 1] = min(m_VarMin[var_count - 1], double(FUtil::AtoFloat(TheValueString)));
				m_VarMax[var_count - 1] = max(m_VarMax[var_count - 1], double(FUtil::AtoFloat(TheValueString)));
			}
			else if (option == MinValue&&var_count>1) {
				m_VarMin[var_count - 2] = double(FUtil::AtoFloat(TheValueString));
			}
			else if (option == MaxValue&&var_count>1) {
				m_VarMax[var_count - 2] = double(FUtil::AtoFloat(TheValueString));
			}
			if (line.find(delim) != string::npos)
				line = line.substr(line.find(delim) + 1);
			else
				line.resize(0);

		}
		line.resize(NUMCHAR);
		max_varcount = max(max_varcount, var_count);
	}

	m_Complete = false;
	if (m_Headers == 7) m_Complete = true;

	if (m_VarMin[0] < m_VarMax[0])
		return true;
	else
		return false;
}

size_t FCSV::AssignSIMB_PointersFromHeader()
{

	m_ParameterPointers.resize(0);
	auto conv = [&](string str) {
		SIMB simb;
		if (str.find_last_of(')') > str.find_last_of('(')) {
			string  index_s = str.substr(str.find_last_of('(')+1, str.find_last_of(')') - str.find_last_of('(')-1 );
			simb.TabIndex = FUtil::AtoInt(index_s)-1;
			simb.pBase = m_pModelBase->GetP_Pointer(str.substr(0, str.find_last_of('(')));
			if (simb.pBase == nullptr) {
				simb.pBase = m_pModelBase->DB_GetPointer(str.substr(0, str.find_last_of('(')));


			}
			if (simb.pBase != nullptr) m_ParameterPointers.push_back(simb);
		}
		else {
			simb.TabIndex = string::npos;
			simb.pBase = m_pModelBase->GetPsPointer(str);
			if (simb.pBase != nullptr) m_ParameterPointers.push_back(simb);
		}
	};




	for (auto it = m_headervar.begin(); it != m_headervar.end(); conv(*it), it++) {



	}






	return m_ParameterPointers.size();
}

size_t FCSV::AssignKey_WithValues() {
	m_fstream->clear();
	m_fstream->close();
	m_fstream->open(m_FileName);
	m_fstream->seekg(0, ios::beg);
	string line;
	line.resize(NUMCHAR);
	size_t line_count = 0;
	size_t var_count = 0;
	size_t max_varcount = 0;
	bool TableMode = false;
	

	while (m_fstream->good()) {
		line_count++;
		m_fstream->getline(&line[0], NUMCHAR);
		size_t n = m_fstream->gcount();
		vector<double> Values;
		vector<string> KeyValues;
		Values.resize(m_ParameterPointers.size()+1);
		var_count = 0;
		line.resize(n);
		char delim = ',';
		if (line.find(';') != string::npos)  delim = ';';
		if (line.find("keyindex") == 0 || line.find("KeyIndex") == 0) TableMode = true;
		bool fineline = false;
		bool DBString;
		DBString = false;
		if (m_ParameterPointers[var_count].pBase->Is_DB()) {
			DBString = true;
			KeyValues.resize(m_ParameterPointers.size() + 1);
		}


		while (line.size()>0&&m_ParameterPointers.size()+1>var_count)  {

			var_count++;
			string TheValueString;
			if (line.find(delim) != string::npos)
				TheValueString = line.substr(0, line.find(delim));
			else
				TheValueString = line;

			if (DBString) {
				KeyValues[var_count - 1] = TheValueString;

			}

			else if (line_count > m_Headers+1) {
				if(TableMode)
					Values[var_count - 1] = double(FUtil::AtoFloat(TheValueString));
				else
					Values[var_count - 1] = double(FUtil::AtoFloat(TheValueString));
			}
			if (line.find(delim) != string::npos)
				line = line.substr(line.find(delim) + 1);
			else
				line.resize(0);
			if (var_count == Values.size()&& (line_count > m_Headers + 1||(DBString&&line_count>1))) {
				fineline = true;
			}
		}
		if (fineline)  {
			if (DBString) {
				m_KeyValues.push_back(KeyValues);
				size_t key = size_t(FUtil::AtoInt(KeyValues[0]));
				KeyIndex.insert(pair<size_t, size_t>(key, m_KeyValues.size() - 1));

			}
			else {
				m_Values.push_back(Values);
				size_t key = size_t(Values[0]);
				KeyIndex.insert(pair<size_t, size_t>(key, m_Values.size() - 1));
			}
		}


		line.resize(NUMCHAR);
		max_varcount = max(max_varcount, var_count);
	}

	return KeyIndex.size();

}
vector<string> FCSV::GetCDB_Value(size_t keyindex) {
	auto it = KeyIndex.find(keyindex);
	if (it != KeyIndex.end()) {
		return m_KeyValues[it->second];
	}
	else {
		vector<string> v;
		return v;
	}


}

vector<double> FCSV::GetParValue(size_t keyindex)
{



	auto it = KeyIndex.find(keyindex);
	if (it != KeyIndex.end()) {
		return m_Values[it->second];
	}
	else {
		vector<double> v;
		return v;
	}
}

vector<SIMB> FCSV::GetSIMB_vector()
{

	return m_ParameterPointers;
}

vector<size_t> FCSV::GetKeyVector()
{
	vector<size_t> out;
	for (auto it = KeyIndex.begin(); it != KeyIndex.end(); it++) {
		out.push_back(it->first);
	}
	return out;
}


