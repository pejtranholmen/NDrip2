
#include "NE.h"
#include "P.h"
#include "OutVector.h"
NE::~NE()
{

}
NE::NE(size_t* pInt,size_t def, size_t min, size_t max, size_t wmin, size_t wmax): SimB()
{
	_Elem=pInt;
	SetNEValue(def);
	//_orginalvalue = def;
	
	SetOriginalNEValue(def);	
	errormin=min; errormax=max; warnmin=wmin, warnmax=wmax;
}
void NE::LinkTo(P* ptr)
{	
	  m_LinksToP.push_back(ptr);
}
void NE::Def(string name)
{
	SetName(name);
}
void NE::LinkTo(X* ptr)
{	
	   m_LinksToX.push_back(ptr);
}
void NE::LinkTo(T* ptr)
{	
	  m_LinksToT.push_back(ptr);
}
void NE::LinkTo(G* ptr)
{	
	 m_LinksToG.push_back(ptr);
}
void NE::LinkTo(D* ptr)
{	
	m_LinksToD.push_back(ptr);
}
void NE::ReSizeValue(size_t value) {
	SetNEValue(value);
	size_t vtest=GetNEValue();
	for(size_t i=0; i<m_LinksToP.size(); i++) 
		((P*)m_LinksToP[i])->ReSize(value);
	for(size_t i=0; i<m_LinksToG.size(); i++) 
		((OutVector*)m_LinksToG[i])->ReSize(value);
	for(size_t i=0; i<m_LinksToX.size(); i++) 
		((OutVector*)m_LinksToX[i])->ReSize(value);
	for(size_t i=0; i<m_LinksToT.size(); i++) 
		((OutVector*)m_LinksToT[i])->ReSize(value);
	for(size_t i=0; i<m_LinksToD.size(); i++) 
		((OutVector*)m_LinksToD[i])->ReSize(value);
}