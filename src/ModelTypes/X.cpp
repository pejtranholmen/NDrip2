 #include "X.h"
#include "NE.h"

X::~X()
{
}
X::X(vector<double> *target,NE *pNNE,enum datatype data, enum elements elem, enum fysprocess fproc, enum bioprocess bproc, enum userlevel ulev)
	:OutVector(data, elem, fproc, bproc, ulev)
{
	_ptarget=target;
	SetSimType(STATE);
	pNE=pNNE;
	;
}
void X::Def(size_t ModuleNo, string name, vector<double> init, string unit, string conditions)
{

	SetValue(init);
	SetModuleNo(ModuleNo);
	SetUnit(unit);
	SetName(name);
	SetConditions(conditions);



	
//	for (size_t i=0;i<numFlags;i++)
//	{
		//tmpflag[i]=*(flag+i);
		//m_AnnimateVector[i].ichart=-1;
		//m_AnnimateVector[i].sequence=0;

		//pNode.OptInitial=float(MISSING);
	SumList.resize(pNE->GetNEValue());
	_storeflag.resize(pNE->GetNEValue());
	_multi_storeflag.resize(pNE->GetNEValue());
	_AcceptMeanFlag.resize(pNE->GetNEValue());
	for(size_t i=0; i<SumList.size();i++) {
		SumList[i].OptInitial=float(MISSING);
		SumList[i].Initial=float(MISSING);
		SumList[i].Final=float(MISSING);
		SumList[i].Min=float(MISSING);
		SumList[i].Max=float(MISSING);
		SumList[i].Mean=float(MISSING);
		SumList[i].Accumulated=float(MISSING);
		SumList[i].ValidFlagIndex=-1;
		SumList[i].PgFileIndex=-1;
		SumList[i].StoreFlag=0;
//		pNode.ValidFlagIndex=-1;
//		pNode.PgFileIndex=-1;
//		SumList.push_back(pNode);
	}
//	}


}
