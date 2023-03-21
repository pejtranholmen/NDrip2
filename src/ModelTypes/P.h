#pragma once
#include "Par.h"
class P;
#include "NE.h"
class Func;

class P : public Par
{
	public:
		P(std::vector<double> *target, definitions def);
		//P(std::vector<double> *target, size_t* num, enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev);	
		P(std::vector<double> *target, NE* num, enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev);	
		
		~P(void);
		P(void) {};
		void SetValue(vector<double> vec) {*_ptarget=vec;};
		double GetValue(size_t index) { if (_ptarget->size() > index) return _ptarget->at(index); else return MISSING; };
		std::vector<double> GetVector() { return *_ptarget;}
		void SetSize(size_t NRow) {if(pNE!=nullptr) ((NE*)pNE)->ReSizeValue(NRow);};
		size_t  GetSize() noexcept {return _ptarget->size();};
		void Def(size_t ModuleNo, string name, vector<double> init, string unit, string conditions, 
			double errormin, double errormax, double warningmin, double warningmax,
			int recalc=0,  int view=0);
		size_t SetValue(size_t index, double v) {if(index<_ptarget->size()) {_oldValueVector[index]=_ptarget->at(index);_IsOldValueVector[index]=true;_ptarget->at(index)=v;if(_ptarget->at(index)!=GetOriginalValue(index)) SetNotOriginalIndicator(index, true);}return _recalc_option;};

		void SetDBValue(size_t index, double v) {if(index<_DB_ValueVector.size()) _DB_ValueVector[index]=v; };
		double GetDBValue(size_t index) const {if(index<_DB_ValueVector.size()) return _DB_ValueVector[index];return MISSING;};

		NE* GetNEPointer() { return pNE; };
		void SetOldValue(vector<double> value) {_oldValueVector.resize(value.size()); 
		_IsOldValueVector.resize(value.size()); for(size_t i=0; i<value.size();i++) {_oldValueVector[i]=value[i];_IsOldValueVector[i]=true;};};
		void ResetOldValue(vector<double> value) {_oldValueVector.resize(value.size()); 
		_IsOldValueVector.resize(value.size()); for(size_t i=0; i<value.size();i++) {_oldValueVector[i]=value[i];_IsOldValueVector[i]=false;};};

		bool IsOldValue(size_t index) {if(index<_IsOldValueVector.size() ) return _IsOldValueVector[index];return false;};
		double GetOldValue(size_t index) {if(index<_oldValueVector.size()) return _oldValueVector[index];return -1;};
		void SetNotOldValue(size_t index) {if(index<_IsOldValueVector.size()) _IsOldValueVector[index]=false;};
		
		
		bool IsNotOriginalValue(size_t index) const {if(index<m_IsNotOriginalv.size()) return m_IsNotOriginalv[index]; else return false;};	
		void SetNotOriginalIndicator(size_t index, bool value)  {if(index<m_IsNotOriginalv.size()) m_IsNotOriginalv[index]=value;};	
	

		void SetMinValue(size_t index, double v) {_min[index]=v;};
		void SetMinValue(double v) { for(size_t i=0; i<_min.size(); i++) _min[i]=v; };
		double GetMinValue(size_t index) { if (_min.size() > index) return _min[index]; else return MISSING; };
		void SetMaxValue(size_t index, double v) {_max[index]=v;};
		void SetMaxValue(double v) { for(size_t i=0; i<_max.size(); i++) _max[i]=v; };
		double GetMaxValue(size_t index) { if (_max.size() > index) return _max[index]; else return MISSING; };
		void SetWarnMinValue(size_t index, double v) {_wmin[index]=v;};
		void SetWarnMinValue(double v) { for(size_t i=0; i<_wmin.size(); i++) _wmin[i]=v; };
		double GetWarnMinValue(size_t index) {return _wmin[index];};
		void SetWarnMaxValue(size_t index, double v) {_wmax[index]=v;};
		void SetWarnMaxValue(double v) { for(size_t i=0; i<_wmax.size(); i++) _wmax[i]=v; };
		double GetWarnMaxValue(size_t index) {return _wmax[index];};

		int MR_Get_Dim(size_t index) {return _mr[index].Dimension;};
		double MR_Get_Min(size_t index) {return _mr[index].Min;};
		double MR_Get_Max(size_t index) {return _mr[index].Max;};
		double MR_Get_Start(size_t index) {return _mr[index].Start;};
		double MR_Get_RelChange(size_t index) {return _mr[index].RelChange;};
		enum MR_METHOD MR_Get_Method(size_t index) {return _mr[index].Method;};
		SimB *Get_MR_ParDependence(size_t index) {return _mr[index].MR_Dependent.pBase;};
		size_t Get_MR_ParDependenceIndex(size_t index) {return _mr[index].MR_Dependent.TabIndex;};
		SIMB Get_MR_PAR_Dependence(size_t index) {return _mr[index].MR_Dependent;};

		void Apply(size_t) {;};
		void EditApply(size_t index) {;};
		void MR_Set_Dim(size_t index, int value) {_mr[index].Dimension=value;};
		void MR_Set_Min(size_t index, double value) {_mr[index].Min=value;};
		void MR_Set_Max(size_t index, double value) {_mr[index].Max=value;};
		void MR_Set_Start(size_t index, double value) {_mr[index].Start=value;};
		void MR_Set_Method(size_t index, enum MR_METHOD value) {if(index<_mr.size())_mr[index].Method=value; else if(_mr.size()>0) _mr.back().Method=value; };
		void MR_Set_RelChange(size_t index, double value) { _mr[index].RelChange=value;};
		void Set_MR_ParDependence(size_t index, SimB *pSimB, size_t depIndex=-1) {_mr[index].MR_Dependent.pBase=pSimB;_mr[index].MR_Dependent.TabIndex=depIndex;};
		void Set_MR_ParDependenceIndex(size_t index, size_t dindex) {_mr[index].MR_Dependent.TabIndex=dindex;};
		void Set_MR_PAR_Dependence(size_t index, SIMB value) { if(index<_mr.size()) _mr.resize(index+1); _mr[index].MR_Dependent=value;};

		size_t MR_GetSizeTable(size_t index) {return _mr_Tab_Value[index].size();};
		void MR_Reset_Table(size_t index) {_mr_Tab_Value[index].clear();};
		void MR_Append_TableValue(size_t index, double value) {_mr_Tab_Value[index].push_back(value);};
		double MR_Get_TabValue(size_t index, size_t row) {return _mr_Tab_Value[index][row];}
		void   MR_Set_TabValue(size_t index, size_t row, double value) {_mr_Tab_Value[index][row]=value;};

		long GetChaDate(size_t indp, size_t index) {if(index<ChaDateLong[indp].size()) return ChaDateLong[indp][index]; return -1;};
		double GetChaParValue(size_t indp, size_t index) {if(index<ChaParValue[indp].size()) return ChaParValue[indp][index]; return MISSING;};
		size_t GetNumChaDates(size_t indp) {if(indp>=ChaDateLong.size()) return 0; 
			else return ChaDateLong[indp].size();
		};
		bool SetChaDateAndValue(long minnew,  size_t index, double newvalue) {
				long test;
				if(index>=ChaDateLong.size()) {
					ChaDateLong.resize(index+1);
					ChaParIndex.resize(index+1);
					ChaParValue.resize(index+1);
				}

				for(size_t i=0;i<ChaDateLong[index].size();i++){
					test=ChaDateLong[index][i];
					if(test>minnew) {
						ChaDateLong[index][i]=minnew;
						ChaParValue[index][i]=newvalue;
						ChaParIndex[index][i]=index;
						return true;
					}
					else if(test==minnew&&ChaParIndex[index][i]==index) {
						ChaDateLong[index][i]=minnew;
						ChaParValue[index][i]=newvalue;
						ChaParIndex[index][i]=index;
						return true;
					}

				}
				ChaDateLong[index].push_back(minnew);
				ChaParValue[index].push_back(newvalue);
				ChaParIndex[index].push_back(index);
				return false;
	
		};
		void ReSize(size_t value) {
			if(_ptarget->size()>0) {
				while(value>_ptarget->size())
					_ptarget->push_back(_ptarget->back());
			}
			else
				_ptarget->resize(value);
			if(value<_ptarget->size()) _ptarget->resize(value);
			
			if (_orgFVector.size() > 0) {
				while (value > _orgFVector.size())
					_orgFVector.push_back(_orgFVector.back());
			}
			else
				_orgFVector.resize(value);
			if (value < _orgFVector.size()) _orgFVector.resize(value);




			size_t expand=0;
			size_t priorend = 0;
			if(_min.size()>0) priorend= _min.size() - 1;
			if(_mr.size()<value) expand=value-_mr.size();
			_mr.resize(value);_mr_RelChange_Value.resize(value);_min.resize(value);
			_max.resize(value); _wmin.resize(value);
			_oldValueVector.resize(value); _IsOldValueVector.resize(value);_DB_ValueVector.resize(value);
			_wmax.resize(value);m_IsNotOriginalv.resize(value);
			_mr_Tab_Value.resize(value);
			if(expand>0) {
				auto lastvalue=_min[priorend];
				for(size_t i=priorend+1; i<_mr.size(); i++) {
					_mr[i]=_mr[priorend];
					_min[i]=_min[priorend];
					_max[i]=_max[priorend];
					_wmin[i]=_wmin[priorend];
					_wmax[i]=_wmax[priorend];
				}
			}

			if (GetNumOfTitles() > 0) {
				for (size_t i = 0; i < GetNumOfTitles(); i++) {
					SetSelectedFunc(i);
					auto Func = GetFuncPointer();
					if (Func != nullptr) {
						Func->ResizeDoubleParVector(_ptarget,value);
					}
				}
			}
		};
		bool EditChaDateAndValue(size_t indp, long min, double value, size_t index) { if(indp<ChaDateLong.size()) {ChaDateLong[indp][index]=min; ChaParValue[indp][index]=value;return true;};return false;};

		bool DeleteChaDate(size_t indp, size_t index) {ChaDateLong[indp].erase(ChaDateLong[indp].begin()+index);ChaParValue[indp].erase(ChaParValue[indp].begin()+index);ChaParIndex[indp].erase(ChaParIndex[indp].begin()+index);return true;};
		bool ResetChaDates(size_t indp) { ChaDateLong[indp].clear(); ChaParValue[indp].clear();ChaParIndex[indp].clear();return true;};
		void MR_Set_Monitoring(size_t indp, size_t value) { 
			if(indp>=_mrMonitoring.size()) _mrMonitoring.resize(indp+1); _mrMonitoring[indp]=value;};
		size_t MR_Get_Monitoring(size_t indp) {if(indp<_mrMonitoring.size()) return _mrMonitoring[indp]; return -1;};
		

		

	private:

	vector <bool> m_IsNotOriginalv;
	vector<vector<long> > ChaDateLong;
	vector<vector<double> > ChaParValue;
	vector<vector<size_t> > ChaParIndex;
	std::vector<double> *_ptarget;

	std::vector<double> _min,_max,_wmin,_wmax;
	std::vector<double> _oldValueVector, _DB_ValueVector;
	vector<bool> _IsOldValueVector;
	std::vector<vector<double> > _mr_Tab_Value;
	std::vector<vector<double> > _mr_RelChange_Value;
	NE *pNE;
	vector<MR_PAR> _mr;
	vector<size_t> _mrMonitoring;
	
};


