 #include "G.h"
#include "NE.h"

G::~G()
{
}
G::G(std::vector<double> *target,NE *ne, datatype data, elements elem, fysprocess fproc, bioprocess bproc,  userlevel ulev)
	:OutVector(data, elem, fproc, bproc, ulev)
{
	_ptarget=target;
	SetSimType(AUX);
	pNE=ne;
	;
}
void G::Def(int ModuleNo, std::string name, std::vector<double> init, std::string unit, std::string conditions)
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
	}
//	}


}
