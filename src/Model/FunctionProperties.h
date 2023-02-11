#pragma once

#include "../ModelTypes/SimB.h"
class Ps;
class P;
class FunctionProperties
{
public:
	FunctionProperties(void);
	~FunctionProperties(void);
	bool Init(std::string filename);
	PLOT_PARAM GetParam(size_t index) {if(index<m_FuncProp.size()) return m_FuncProp[index]; else {
		PLOT_PARAM out;
		return out;	
		} };
	size_t GetIndex(std::string str) {
		std::map<std::string,size_t>:: iterator it;
		it=m_FuncIndex.find(str);
		if(it!=m_FuncIndex.end())
			return it->second;
		else
			return std::string::npos;
	};
	PLOT_PARAM GetParam(std::string str) {
		size_t index=GetIndex(str);
		return GetParam(index);
	};
	size_t GetNumParDependence() { return m_FuncPar.size();};
	PLOT_PARAM_DEP GetParamDep(size_t index) { if(index<m_FuncPar.size()) return m_FuncPar[index]; else {
		PLOT_PARAM_DEP out;
		return out;
	}};
	PLOT_PARAM_DEP GetParamDep(std::string str) { size_t index=GetIndex(str); 
		return GetParamDep(index);};
	size_t GetNumFunctions() { return m_FuncIndex.size();};

	
	bool SetParamDep(std::vector<Ps*> PsVector, std::vector<P*> PVector, size_t index);
	bool SetParamDep(std::vector<Ps*> PsVector, size_t index);
	bool SetParamDep(std::vector<P*> PVector, size_t index);
	bool IsFileRead() {return m_FileRead;};
private:
	bool m_FileRead;
	std::map<std::string, size_t> m_FuncIndex;
	std::vector<PLOT_PARAM> m_FuncProp;
	std::vector<PLOT_PARAM_DEP> m_FuncPar;
};

