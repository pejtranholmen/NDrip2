// SoilProp_Texture.cpp : implementation file
//

#include "..\stdafx.h"
#include "..\CoupModel.h"
#include "SoilProp_Texture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SoilProp_Texture

IMPLEMENT_DYNAMIC(SoilProp_Texture, CRecordset)

SoilProp_Texture::SoilProp_Texture(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(SoilProp_Texture)
	//}}AFX_FIELD_INIT
	m_nDefaultType = dbOpenTable;
}


CString SoilProp_Texture::GetDefaultConnect()
{
	return _T("ODBC;DSN=C:\CoupModel\Exe\SoilProp.mdb");
}

CString SoilProp_Texture::GetDefaultSQL()
{
	return _T("");
}

void SoilProp_Texture::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(SoilProp_Texture)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	//}}AFX_FIELD_MAP
}

/////////////////////////////////////////////////////////////////////////////
// SoilProp_Texture diagnostics

#ifdef _DEBUG
void SoilProp_Texture::AssertValid() const
{
	CRecordset::AssertValid();
}

void SoilProp_Texture::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
