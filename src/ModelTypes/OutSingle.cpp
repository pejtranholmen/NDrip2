#include "OutSingle.h"




OutSingle::OutSingle(enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev)
	:OutBase(data,elem, fproc, bproc, ulev) 
{
	_out.ValidFlagIndex=0;
	_multi_storeflag=false;
	_AcceptMeanFlag=false;
	_storeflag=false;
		_out.Initial=0;
		_out.Final=0;
		_out.Min=0;
		_out.Max=0;
		_out.Mean=0;
		_out.Accumulated=0;
		_out.PgFileIndex=0;
		_out.PgMultiFileIndex=0;
		_out.StoreFlag=0;
		m_Annimate.ichart=0;
		m_Annimate.sequence=0;
}
OutSingle::~OutSingle(void)
{
}
bool OutSingle::AddSumVar( float initial, float final,float  min,float  max, float mean,float accumulated, int pgindex, int pgMultiIndex, int StoreFlag)
{

		_out.Initial=initial;
		_out.Final=final;
		_out.Min=min;
		_out.Max=max;
		_out.Mean=mean;
		_out.Accumulated=accumulated;
		_out.PgFileIndex=pgindex;
		_out.PgMultiFileIndex=pgMultiIndex;
		_out.StoreFlag=StoreFlag;
		return true;

}

bool OutSingle::AddOutPutVar(OUTPUT out)
{
	_out = out;
	return false;
}
