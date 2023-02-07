#pragma once
class Ps;
#include "Par.h"

class Ps :	public Par
{
	public:
		Ps(void){};
		Ps(double *target, definitions def);
		Ps(double *target, enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev);
		~Ps(void);
		void Def(size_t ModuleNo, string name, double init, string unit, string conditions, 
			double errormin, double errormax, double warningmin, double warningmax,
			int recalc=0,  int view=0);
		size_t SetValue(double v) {_oldValue=*_ptarget;_IsOldValue=true;
		*_ptarget=v; 
		if(*_ptarget!=GetOriginalValue()) SetNotOriginalValue();
		return _recalc_option;
			};
		double GetValue() const {return *_ptarget;};
		double GetOldValue() const {return _oldValue;};
		void SetNotOldValue() {_IsOldValue=false;};
		void SetDBValue(double v) {_DBvalue=v;};
		double GetDBValue() const {return _DBvalue;};
		void SetMinValue(double v) {_min=v;};
		double GetMinValue() {return _min;};
		void SetMaxValue(double v) {_max=v;};
		double GetMaxValue() {return _max;};
		void SetWarnMinValue(double v) {_wmin=v;};
		double GetWarnMinValue() {return _wmin;};
		void SetWarnMaxValue(double v) {_wmax=v;};
		double GetWarnMaxValue() {return _wmax;};
		void Apply() {;};
		void EditApply() {;};
		virtual double GetOriginalValue() {return _orgFValue;};
		void SetOriginalValue(double value) {_orgFValue=value;};
		//double GetOriginalValue() {return _orgFValue;};
		
		int MR_Get_Dim() {return _mr.Dimension;};
		double MR_Get_Min() {return _mr.Min;};
		double MR_Get_Max() {return _mr.Max;};
		double MR_Get_Start() {return _mr.Start;};
		double MR_Get_RelChange() {return _mr.RelChange;};
		enum MR_METHOD MR_Get_Method() {return _mr.Method;};
		SIMB Get_MR_PAR_Dependence() {return _mr.MR_Dependent;};
		SimB *Get_MR_ParDependence() {return _mr.MR_Dependent.pBase;};
		size_t Get_MR_ParDependenceIndex() {return _mr.MR_Dependent.TabIndex;};
		void MR_Set_Dim(int value) {_mr.Dimension=value;};
		void MR_Set_Min(double value) {_mr.Min=value;};
		void MR_Set_Max(double value) {_mr.Max=value;};
		void MR_Set_Start(double value) {_mr.Start=value;};
		void MR_Set_Method(enum MR_METHOD value) {_mr.Method=value;};
		void MR_Set_RelChange(double value) { _mr.RelChange=value;};
		void Set_MR_ParDependence(SimB *pSimB, size_t depIndex=-1) {_mr.MR_Dependent.pBase=pSimB;_mr.MR_Dependent.TabIndex=depIndex;};
		void Set_MR_ParDependenceIndex(size_t index) {_mr.MR_Dependent.TabIndex=index;};
		void Set_MR_PAR_Dependence(SIMB value) {  _mr.MR_Dependent=value;};

		
		size_t MR_GetSizeTable() {return _mr_Tab_Value.size();};
		void MR_Reset_Table() {_mr_Tab_Value.clear();};
		void MR_Append_TableValue(double value) {_mr_Tab_Value.push_back(value);};
		double MR_Get_TabValue(size_t index) {return _mr_Tab_Value[index];};
		void   MR_Set_TabValue(size_t index, double value) {_mr_Tab_Value[index]=value;};


		void def(int ModuleNo, string name, double init, string unit, string conditions, 
			double errormin, double errormax, double warningmin, double warningmax,
			int recalc=0,  int view=0);
		bool IsOldValue() {return _IsOldValue;};

		string GetProcess() {return _Process;};

		void MR_Set_Bins(size_t nbins) {  m_n_bins = nbins;};
		bool IsViewValid() {if(view>0) return true;	return false;};



		long GetChaDate(size_t index) {if(index<ChaDateLong.size()) return ChaDateLong[index]; return -1;};
		double GetChaParValue(size_t index) {if(index<ChaParValue.size()) return ChaParValue[index]; return MISSING;};
		size_t GetNumChaDates() {return ChaDateLong.size();};
		bool SetChaDateAndValue(long minnew, double newvalue) { 
				long test;
				for(size_t i=0;i<ChaDateLong.size();i++){
					test=ChaDateLong[i];
					if(test>minnew) {
						ChaDateLong[i]=minnew;
						ChaParValue[i]=newvalue;
						ChaParIndex[i]=string::npos;
						return true;
					}
					else if(test==minnew) {
						ChaDateLong[i]=minnew;
						ChaParValue[i]=newvalue;
						return true;
					}

				}
				ChaDateLong.push_back(minnew);
				ChaParValue.push_back(newvalue);
				ChaParIndex.push_back(string::npos);
				return false;			

		};
		
		bool DeleteChaDate(size_t index) {ChaDateLong.erase(ChaDateLong.begin()+index);ChaParValue.erase(ChaParValue.begin()+index);return true;};
		bool ResetChaDates() { ChaDateLong.clear(); ChaParValue.clear();return true;};
		void MR_Set_Monitoring(size_t value) {_mrMonitoring=value;};
		size_t MR_Get_Monitoring() {return _mrMonitoring;};

	



	private:

	double _orgFValue;
	double *_ptarget; 
	double _min,_max, _wmin, _wmax;
	double _DBvalue;
	double _oldValue;
	std::vector<double> _mr_Tab_Value;
	vector<long> ChaDateLong;
	vector<double> ChaParValue;
	vector<size_t> ChaParIndex;
	MR_PAR _mr;
	string _ViewTitle;
	string _Process;
	size_t _mrMonitoring;
	size_t m_n_bins;
	size_t view;
	string GLegend, GLeftTitle, GXTitle;
	double GXmin, GXmax;
	size_t GPointType, GraphType;

	bool _IsOldValue;
};

