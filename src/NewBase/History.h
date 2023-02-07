#pragma once

#include "./ModelFiles.h"


class History :public ModelFiles {
	
public:
	History();
	bool History_Add(SimB *,int Type, int LocalIndex, time_t t, float value, string User="");
	bool History_Add(SimB *,int Type, int LocalIndex, time_t t, string str, string User="");
	bool History_Add(SimB* pSimB, int LocalIndex, time_t t, float value, string User="");
	bool History_Add(SimB* pSimB, int LocalIndex, time_t t, double value, string User = "") {return History_Add( pSimB, LocalIndex, t, float(value), User); };
	bool History_Add(SimB *, int LocalIndex, time_t t, string str, string User="");
	bool History_Add(size_t RunInfoIndex,string str);
	void History_Add(string, string,string, string, int, time_t, string, string);
	void History_NewAdd(size_t RunInfoType, string, string,string, string, int, time_t, string, string);


	size_t History_GetNumber();
	bool History_RemoveAll();
	bool History_Remove(size_t index);
    SimB* History_GetPointer(size_t index);
	time_t History_GetTime(size_t index);
	size_t History_GetRunInfoType(size_t index);
	size_t History_GetEnumType(size_t index);
	string History_GetUser(size_t index);
	string History_GetComputer(size_t index);
	string History_GetString(size_t index);
	float History_GetFloat(size_t index);
	size_t History_GetLocalIndex(size_t index);
	size_t History_GetRunNo(size_t index);
	size_t UpdateListToView();
	bool   m_ToView[7],	m_ViewAll;
	vector<size_t> m_IndexToView;

protected:

	
};
