#pragma once
#include "SimB.h" 
#include "P.h"
class Tab : public SimB {
 public:
	Tab(vector<P*> *target, datatype data, elements elem, fysprocess fproc, bioprocess bproc, userlevel ulev);

	~Tab();
	void Def(size_t module_no, string varname, string pname, string cond) {SetConditions(cond); Def(module_no, varname, 1);};
	void Def(size_t module_no, string varname, size_t minP_toenable);  
	size_t GetViewFunction() {return _viewvalue;};
	void SetViewFunction(size_t value) {_viewvalue=value;};
	bool GetHide() {return _Hidden;};
	void SetHide(bool value) {_Hidden=value;};
	size_t GetNumOfCol() {return _ptarget->size();};
	void SetNumOfRow(size_t Nrow) {P *pP; pP=_ptarget->at(0);pP->SetSize(Nrow); };
	size_t GetNumOfRow() { P *pP; if(_ptarget->size()>0) { pP=_ptarget->at(0); return pP->GetSize();} return 0;};
	size_t GetNumOfOriginalRow() { P *pP; if(_ptarget->size()>0) { pP=_ptarget->at(0); return pP->GetOriginalSize();} return 0;};

	bool CheckEnabled();
	bool CheckOriginalValue();
	P* GetP_Pointer(size_t index) {if(index<_ptarget->size()) { return _ptarget->at(index);};return nullptr;};
	string GetViewTitle() {return _title;};
	void SetViewTitle(string str) {_title=str;};
	void SetPlotIndex(size_t n) { m_IndexToPlot.push_back(n);};
	void ResetPlotIndex() {	m_IndexToPlot.clear();};
	size_t GetNumbersToPlot() {	return m_IndexToPlot.size();};
	size_t GetPlotIndex(size_t n) { return m_IndexToPlot[n];};
	size_t GetDB_Row() {return db_rows;};
	void SetDB_Row(size_t rows) {db_rows=rows;};
	void SetMainIndex(size_t index) { _mainindex=index; };
	size_t GetMainIndex() { return _mainindex;};
	string GetTitle(size_t index) {if(index<_ptarget->size()) {P* pP=_ptarget->at(index); return pP->GetName();} return "";};


	string GetXLabel() {return _xlabel;};
	string GetYLabel() {return _ylabel;};
	double GetViewValue(double x) {return _viewYvalue;};
	string GetLegend() { return _legend;};
	bool GetChartLog() {return _chartlog;};



protected:
	vector <P*> *_ptarget;
	bool _Hidden;
	size_t _viewvalue;
	string _title;
	vector<size_t> m_IndexToPlot;
	size_t db_rows;
	size_t _mainindex;
	bool _chartlog;

	string _xlabel;
	string _ylabel;
	string _legend;
	double _viewYvalue;
private:
	size_t _minp_toenable;

};