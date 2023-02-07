// SP_Texture.cpp : implementation file
//

#include "..\stdafx.h"
#include "..\CoupModel.h"
#include "SP_Texture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SP_Texture

IMPLEMENT_DYNAMIC(SP_Texture, CDaoRecordset)

SP_Texture::SP_Texture(CDaoDatabase* pdb)
	: CDaoRecordset(pdb)
{
	//{{AFX_FIELD_INIT(SP_Texture)
	m_SiteNumber = 0;
	m_ReplicateNumber = 0;
	m_Layer = 0;
	m_Clay = 0.0f;
	m_Sand = 0.0f;
	m_Porosity = 0.0f;
	m_nFields = 6;
	//}}AFX_FIELD_INIT
	m_nDefaultType = dbOpenTable;
}


CString SP_Texture::GetDefaultDBName()
{
	return _T("C:\\CoupModel\\Exe\\SoilProp.mdb");
}

CString SP_Texture::GetDefaultSQL()
{
	return _T("[Texture]");
}

void SP_Texture::DoFieldExchange(CDaoFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(SP_Texture)
	pFX->SetFieldType(CDaoFieldExchange::outputColumn);
	DFX_Short(pFX, _T("[SiteNumber]"), m_SiteNumber);
	DFX_Short(pFX, _T("[ReplicateNumber]"), m_ReplicateNumber);
	DFX_Short(pFX, _T("[Layer]"), m_Layer);
	DFX_Single(pFX, _T("[Clay]"), m_Clay);
	DFX_Single(pFX, _T("[Sand]"), m_Sand);
	DFX_Single(pFX, _T("[Porosity]"), m_Porosity);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// SP_Texture diagnostics

#ifdef _DEBUG
void SP_Texture::AssertValid() const
{
	CDaoRecordset::AssertValid();
}

void SP_Texture::Dump(CDumpContext& dc) const
{
	CDaoRecordset::Dump(dc);
}
#endif //_DEBUG
