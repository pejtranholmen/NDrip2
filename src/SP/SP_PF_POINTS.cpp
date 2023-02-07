// SP_PF_POINTS.cpp : implementation file
//

#include "stdafx.h"
#include "CoupModel.h"
#include "SP_PF_POINTS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SP_PF_POINTS

IMPLEMENT_DYNAMIC(SP_PF_POINTS, CDaoRecordset)

SP_PF_POINTS::SP_PF_POINTS(CDaoDatabase* pdb)
	: CDaoRecordset(pdb)
{
	//{{AFX_FIELD_INIT(SP_PF_POINTS)
	m_SiteNumber = 0;
	m_ReplicateNumber = 0;
	m_Layer = 0;
	m_Psi1 = 0.0f;
	m_Theta1 = 0.0f;
	m_Psi2 = 0.0f;
	m_Theta2 = 0.0f;
	m_Psi3 = 0.0f;
	m_Theta3 = 0.0f;
	m_Psi4 = 0.0f;
	m_Theta4 = 0.0f;
	m_Psi5 = 0.0f;
	m_Theta5 = 0.0f;
	m_Psi6 = 0.0f;
	m_Theta6 = 0.0f;
	m_Psi7 = 0.0f;
	m_Theta7 = 0.0f;
	m_Psi8 = 0.0f;
	m_Theta8 = 0.0f;
	m_Psi9 = 0.0f;
	m_Theta9 = 0.0f;
	m_Psi10 = 0.0f;
	m_Theta10 = 0.0f;
	m_Psi11 = 0.0f;
	m_Theta11 = 0.0f;
	m_Psi12 = 0.0f;
	m_Theta12 = 0.0f;
	m_Psi13 = 0.0f;
	m_Theta13 = 0.0f;
	m_Psi14 = 0.0f;
	m_Theta14 = 0.0f;
	m_Psi15 = 0.0f;
	m_Theta15 = 0.0f;
	m_nFields = 33;
	//}}AFX_FIELD_INIT
	m_nDefaultType = dbOpenTable;
}


CString SP_PF_POINTS::GetDefaultDBName()
{
	return _T("C:\\CoupModel\\Exe\\SoilProp.mdb");
}

CString SP_PF_POINTS::GetDefaultSQL()
{
	return _T("[pF-Curve]");
}

void SP_PF_POINTS::DoFieldExchange(CDaoFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(SP_PF_POINTS)
	pFX->SetFieldType(CDaoFieldExchange::outputColumn);
	DFX_Short(pFX, _T("[SiteNumber]"), m_SiteNumber);
	DFX_Short(pFX, _T("[ReplicateNumber]"), m_ReplicateNumber);
	DFX_Short(pFX, _T("[Layer]"), m_Layer);
	DFX_Single(pFX, _T("[Psi1]"), m_Psi1);
	DFX_Single(pFX, _T("[Theta1]"), m_Theta1);
	DFX_Single(pFX, _T("[Psi2]"), m_Psi2);
	DFX_Single(pFX, _T("[Theta2]"), m_Theta2);
	DFX_Single(pFX, _T("[Psi3]"), m_Psi3);
	DFX_Single(pFX, _T("[Theta3]"), m_Theta3);
	DFX_Single(pFX, _T("[Psi4]"), m_Psi4);
	DFX_Single(pFX, _T("[Theta4]"), m_Theta4);
	DFX_Single(pFX, _T("[Psi5]"), m_Psi5);
	DFX_Single(pFX, _T("[Theta5]"), m_Theta5);
	DFX_Single(pFX, _T("[Psi6]"), m_Psi6);
	DFX_Single(pFX, _T("[Theta6]"), m_Theta6);
	DFX_Single(pFX, _T("[Psi7]"), m_Psi7);
	DFX_Single(pFX, _T("[Theta7]"), m_Theta7);
	DFX_Single(pFX, _T("[Psi8]"), m_Psi8);
	DFX_Single(pFX, _T("[Theta8]"), m_Theta8);
	DFX_Single(pFX, _T("[Psi9]"), m_Psi9);
	DFX_Single(pFX, _T("[Theta9]"), m_Theta9);
	DFX_Single(pFX, _T("[Psi10]"), m_Psi10);
	DFX_Single(pFX, _T("[Theta10]"), m_Theta10);
	DFX_Single(pFX, _T("[Psi11]"), m_Psi11);
	DFX_Single(pFX, _T("[Theta11]"), m_Theta11);
	DFX_Single(pFX, _T("[Psi12]"), m_Psi12);
	DFX_Single(pFX, _T("[Theta12]"), m_Theta12);
	DFX_Single(pFX, _T("[Psi13]"), m_Psi13);
	DFX_Single(pFX, _T("[Theta13]"), m_Theta13);
	DFX_Single(pFX, _T("[Psi14]"), m_Psi14);
	DFX_Single(pFX, _T("[Theta14]"), m_Theta14);
	DFX_Single(pFX, _T("[Psi15]"), m_Psi15);
	DFX_Single(pFX, _T("[Theta15]"), m_Theta15);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// SP_PF_POINTS diagnostics

#ifdef _DEBUG
void SP_PF_POINTS::AssertValid() const
{
	CDaoRecordset::AssertValid();
}

void SP_PF_POINTS::Dump(CDumpContext& dc) const
{
	CDaoRecordset::Dump(dc);
}
#endif //_DEBUG
