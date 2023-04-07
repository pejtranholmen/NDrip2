#pragma once
#include "./OutBase.h"
#include "../Util/FUtil.hpp"
class OutSingle :
	public OutBase
{
	public:
		ANN_INFO m_Annimate;
		bool tmpflag;

		~OutSingle(void);
		OutSingle(datatype data, elements elem, fysprocess fproc, bioprocess bproc, userlevel ulev);
		OutSingle(definitions def);
		//string GetUnit() {return _unit;};
		//void SetUnit(string str) {_unit=str;};
		void SetStoreFlag(int value) {_out.StoreFlag=value;};
		void SetMultiStoreFlag(bool value) {_multi_storeflag=value;};
		void SetAcceptMeanFlag(bool value) {_AcceptMeanFlag=value;};
		void SetValue(double value) {*_ptarget=value;};
		
		bool IsFlagSet() const {return _storeflag;};
		double GetValue() { return *_ptarget;};
		size_t GetNumberOfSelectedFlags(bool multi=false) {if(_out.StoreFlag>0) return 1; return 0;};
		OUTPUT GetSumList() {return _out;};

		bool    AddSumVar(float, float, float, float, float, float, int, int, int StoreFlag=0);
		bool	AddOutPutVar(OUTPUT out);

		bool    SetOptInitialValue( float value) {_out.OptInitial=value; return value;};
		double  GetOptInitialValue() {return double(_out.OptInitial);};
		size_t	GetPgFileIndex() const {if(_out.StoreFlag==2) return _out.PgMultiFileIndex;
											else return _out.PgFileIndex;};
		size_t	GetPgMultiFileIndex() const {return _out.PgMultiFileIndex;};
		size_t	GetPgSingleFileIndex() const {return _out.PgFileIndex;};

		size_t	GetSumVarIndex(size_t col) const { return _out.PgFileIndex * 6 + col - FUtil::GetPositionSumInitialFromOutSheet(); };
		bool	SetPgFileIndex(size_t value) {_out.PgFileIndex=value; return true;};
		void	SetMultiPGFileIndex(size_t value) {_out.PgMultiFileIndex=value;};
		void	SetValidFlagIndex(size_t value) { _out.ValidFlagIndex=value;};
		bool    ApplyInitialValue() {SetValue(_out.OptInitial); return true;};

		float   GetInitial() const {return _out.Initial;};
		float   GetFinal() const {return _out.Final;};
		float   GetMin() const{return _out.Min;};
		float   GetMax() const{return _out.Max;};
		void   SetInitial(double value) { _out.Initial = value; };
		void   SetFinal(double value) { _out.Final = value; };

		float   GetMean() const{return _out.Mean;};
		float   GetAccumulated() const{return _out.Accumulated;};
		size_t	GetValidFlagIndex() const{ return _out.ValidFlagIndex;};
		size_t	GetStoreFlag() const{ return _out.StoreFlag;};
		bool    GetMultiStoreFlag() {return _multi_storeflag;};
		bool    GetAcceptMeanFlag() {return _AcceptMeanFlag;};

		bool	SetAnnimateChart(int value, int seq) { m_Annimate.ichart=value; m_Annimate.sequence=seq;return true;};
		int		GetAnnimateChart() {return m_Annimate.ichart;};
		int		GetAnnimateSequence() {return m_Annimate.sequence;};

	protected:
		double *_ptarget;
		OUTPUT _out;
		
	private:

		//string _unit;
		bool _storeflag, _multi_storeflag, _AcceptMeanFlag;
};

