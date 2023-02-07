#pragma once
#include "../std.h"
#include "SimB.h"	
class P;
class X;
class T;
class G;
class D;
/*#include "X.h"
#include "T.h"
#include "G.h"
#include "D.h"*/
class NE :public SimB
	{					

public:
	~NE();
	NE(size_t *pInt,  size_t def, size_t min, size_t max, size_t wmin, size_t wmax); 
	void Def(string varname);
	void LinkTo(P*);
	void LinkTo(X*);
	void LinkTo(T*);
 	void LinkTo(G*);
	void LinkTo(D*);

	

	void SetNEValue(size_t value) { *_Elem=value;};
	void ReSizeValue(size_t value);
	size_t GetNE_MinValue() { return errormin; };
	size_t GetNE_MaxValue() { return errormax; };
	size_t* GetNE_Elem() { return _Elem; };
	size_t  GetNEValue() { return *_Elem;};
	void SetOriginalNEValue(size_t value) {_orginalvalue=value;};
	size_t  GetOriginalNEValue() {return _orginalvalue;};
	size_t GetNumLinks_P() { return  m_LinksToP.size(); };
	size_t GetNumLinks_X() { return  m_LinksToX.size(); };
	size_t GetNumLinks_T() { return  m_LinksToT.size(); };
	size_t GetNumLinks_G() { return  m_LinksToG.size(); };
	size_t GetNumLinks_D() { return  m_LinksToD.size(); };
	P* Get_PLink(size_t index) { return m_LinksToP[index]; };
	X* Get_XLink(size_t index) { return m_LinksToX[index]; };
	T* Get_TLink(size_t index) { return m_LinksToT[index]; };
	G* Get_GLink(size_t index) { return m_LinksToG[index]; };
	D* Get_DLink(size_t index) { return m_LinksToD[index]; };
protected:
	size_t* _Elem;
	size_t errormin, errormax, warnmin, warnmax;
	size_t _orginalvalue;
	vector<P*> m_LinksToP;
	vector<X*> m_LinksToX;
	vector<T*> m_LinksToT;
	vector<G*> m_LinksToG;
	vector<D*> m_LinksToD;


};
