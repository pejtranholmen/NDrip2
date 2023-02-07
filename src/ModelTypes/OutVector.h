#pragma once
#include "./OutBase.h"
class NE;
class OutVector :
	public OutBase
{
	public:
		vector<ANN_INFO> m_AnnimateVector;
		vector<bool> tmpflag;
		~OutVector(void);
		OutVector(enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev);
		
		OutVector(definitions def);

		//string GetUnit() {return _unit;};
		//void SetUnit(string str) {_unit=str;};
	//	void SetStoreFlag(int value) { for(auto i=_storeflag.begin(); i!=_storeflag.end(); *i=value, i++);};
		void SetMultiStoreFlag(bool value) { for(auto i=_multi_storeflag.begin(); i!=_multi_storeflag.end(); *i=value, i++);};
		int GetStoreFlag(size_t i) const;// {if(i<SumList.size.size()) return SumList[i].StoreFlag;else return 0; };
		OUTPUT GetSumList(size_t index) const {if(index<SumList.size()) return SumList[index]; return miss;};
		bool SetFlags(string flagString, int value);

		void SetValue(double value) { for(auto i=_ptarget->begin(); i!=_ptarget->end();*i=value, i++);};
		void SetValue(vector<double> value) { *_ptarget=value;};
		void SetStoreFlag(size_t i, int value) { if(SumList.size()>i) SumList[i].StoreFlag=value;};
		void SetAllStoreFlags(int value) {for(size_t i=0; i<SumList.size();i++) SumList[i].StoreFlag=value;}; 
		void SetMultiStoreFlag(size_t i, bool value) { if(_multi_storeflag.size()>i) _multi_storeflag[i]=value;};
		void SetMeanAcceptFlag(size_t i, bool value) { if(_AcceptMeanFlag.size()>i) _AcceptMeanFlag[i]=value;};
		
		
		bool AreAllFlagsSet();
		size_t GetNumberOfSelectedFlags(bool multi=false) ;
		size_t GetNumberOfFlags() const;
		bool IsFlagsSet(bool multi=false);

		void SetValue(size_t i, double value) {_ptarget->at(i)=value;};
		double GetValue(size_t index) { if(index<_ptarget->size()) return _ptarget->at(index); else if(_ptarget->size()==1) return _ptarget->at(0);else return MISSING; };
		size_t GetNumVector() {return _ptarget->size();};

		bool    AddSumVar(size_t, float, float, float, float, float, float, int, int, int StoreFlag=0);
		bool    AddSumVar(size_t, double, double, double, double, double, double, int,  int, int StoreFlag=0);
		bool    AddOutPutVar(size_t index, OUTPUT out);

		bool    SetOptInitialValue(size_t, float);
		double  GetOptInitialValue(size_t);
		size_t		GetPgFileIndex(size_t) const;
		size_t		GetSumVarIndex(size_t IndexSum, size_t col) const;
		size_t		GetPgSingleFileIndex(size_t) const;
		size_t		GetPgMultiFileIndex(size_t) const;
		bool	SetPgFileIndex(size_t, size_t);
		void	SetMultiPGFileIndex(size_t index, size_t value) {if(SumList.size()>index) SumList[index].PgMultiFileIndex=value;};
		bool    ApplyInitialValue(size_t);
   		void    CleanSumVarList();
		size_t		GetLocalIndex(size_t) const;
		size_t    GetSelectedLocalIndex(size_t, bool multi=false) const;
		vector<size_t>    GetAllSelectedLocalIndex();
		size_t     GetValidFlagIndex(size_t) const;
		bool    SetValidFlagIndex(size_t, size_t);


		float   GetInitial(size_t) const;
		float   GetFinal(size_t) const;
		float   GetMin(size_t) const;
		float   GetMax(size_t) const;
		float   GetMean(size_t) const;
		float   GetAccumulated(size_t) const;
		NE*		GetNEPointer() { return pNE; };

		bool	SetAnnimateChart(size_t index, int value, int seq) {if(index>=m_AnnimateVector.size()) return false;m_AnnimateVector[index].ichart=value; m_AnnimateVector[index].sequence=seq;return true;};
		int		GetAnnimateChart(size_t index) {if(index>=m_AnnimateVector.size()) return -1;return m_AnnimateVector[index].ichart;};
		int		GetAnnimateSequence(size_t index) {if(index>=m_AnnimateVector.size()) return -1;return m_AnnimateVector[index].sequence;};
		bool	GetAcceptMeanFlag(size_t index) {return _AcceptMeanFlag[index];};
		void    ReSize(size_t value) {_ptarget->resize(value); 
		SumList.resize(value);_storeflag.resize(value); 
		_multi_storeflag.resize(value);
		_AcceptMeanFlag.resize(value);};




	protected:
		vector<double> *_ptarget;
		vector<OUTPUT> SumList;
		OUTPUT miss;
		vector<int> _storeflag;
		vector<bool> _multi_storeflag;
		vector<bool> _AcceptMeanFlag;
		NE *pNE;
		
	private:

		//string _unit;
		
	
};

