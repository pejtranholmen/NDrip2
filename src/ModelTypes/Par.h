#pragma once

#include "SimB.h"
#include "Func.h"
class Par :
	public SimB
	{
	public:
		Par(void);
		Par(enum datatype, enum elements, enum userlevel);
		Par(enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev);
		Par(definitions def);
		~Par(void);
		string GetUnit() {return _unit;};
		void SetUnit(string str) {_unit=str;};
		void SetViewFunction(size_t value) {_view_func=value;};
		size_t GetViewFunction() {return _view_func;};
		void SetReCalcOption(size_t value) {_recalc_option=value;};
		size_t GetNumOfTitles() { return m_FuncVector.size();};
		string GetViewTitle(size_t index=0) {if(m_FuncVector.size()>index) 
			return m_FuncVector[index]->GetName(); 
		return "";};
		double GetViewValue(double x) { if(m_FuncVector.size()>m_SelectedFunc) return m_FuncVector[m_SelectedFunc]->GetValue(x) ;else return 0.;};
		double GetChartMin() {  if(m_FuncVector.size()>m_SelectedFunc) return m_FuncVector[m_SelectedFunc]->GetXMin();else return MISSING;};
		double GetChartMax() {   if(m_FuncVector.size()>m_SelectedFunc) return m_FuncVector[m_SelectedFunc]->GetXMax();else return MISSING;};
		bool GetChartLog() { if(m_FuncVector.size()>m_SelectedFunc) return m_FuncVector[m_SelectedFunc]->GetLogScale();	else return false;};
		int GetChartPoints() { if(m_FuncVector.size()>m_SelectedFunc) {if(m_FuncVector[m_SelectedFunc]->GetLogScale())	return 400; else return 10;} return 0;};
		void AddFuncPointer(Func *pFunc) {m_FuncVector.push_back(pFunc);};
        PLOT_PARAM GetPlotParam() {PLOT_PARAM temp; if(m_FuncVector.size()>m_SelectedFunc) {return m_FuncVector[m_SelectedFunc]->GetPlotParam();} else {return temp;} };
		Func* GetFuncPointer() {
			if(m_FuncVector.size()>m_SelectedFunc) return m_FuncVector[m_SelectedFunc];
			else return nullptr;};
		void SetSelectedFunc(size_t value) {m_SelectedFunc=value;};
		size_t GetSelectedFunc() {return m_SelectedFunc;};
/*string GetViewTitle()
{
	if(view>0) {
		SOIL_MAIN_mp_SOIL_VIEW_INIT(&view);
		return GTitle;
	}
	return _T("");
}*/

	string GetYLabel() { if(m_FuncVector.size()>m_SelectedFunc) return m_FuncVector[m_SelectedFunc]->GetYLabel(); return "";};
	string GetXLabel() { if(m_FuncVector.size()>m_SelectedFunc) return m_FuncVector[m_SelectedFunc]->GetXLabel();	return "";};
	
	
	protected:
		size_t _recalc_option;
		
	private:
		size_t m_SelectedFunc;
		vector<Func*> m_FuncVector;
		//string _unit;
		size_t _view_func;
		
};

