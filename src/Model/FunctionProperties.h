#pragma once
#include "../std.h"
#include "../ModelTypes/SimB.h"
class Ps;
class P;
class FunctionProperties
{
public:
	FunctionProperties(void);
	~FunctionProperties(void);
	bool Init(string filename);
	PLOT_PARAM GetParam(size_t index) {if(index<m_FuncProp.size()) return m_FuncProp[index]; else {
		PLOT_PARAM out;
		return out;	
		} };
	size_t GetIndex(string str) {
		map<string,size_t>:: iterator it;
		it=m_FuncIndex.find(str);
		if(it!=m_FuncIndex.end())
			return it->second;
		else
			return string::npos;
	};
	PLOT_PARAM GetParam(string str) {
		size_t index=GetIndex(str);
		return GetParam(index);
	};
	size_t GetNumParDependence() { return m_FuncPar.size();};
	PLOT_PARAM_DEP GetParamDep(size_t index) { if(index<m_FuncPar.size()) return m_FuncPar[index]; else {
		PLOT_PARAM_DEP out;
		return out;
	}};
	PLOT_PARAM_DEP GetParamDep(string str) { size_t index=GetIndex(str); 
		return GetParamDep(index);};
	size_t GetNumFunctions() { return m_FuncIndex.size();};

	
	bool SetParamDep(vector<Ps*> PsVector, vector<P*> PVector, size_t index);
	bool SetParamDep(vector<Ps*> PsVector, size_t index);
	bool SetParamDep(vector<P*> PVector, size_t index);
	bool IsFileRead() {return m_FileRead;};
private:
	bool m_FileRead;
	map<string, size_t> m_FuncIndex;
	vector<PLOT_PARAM> m_FuncProp;
	vector<PLOT_PARAM_DEP> m_FuncPar;
};

