// DimNewValue.cpp : implementation file
//

#include "../stdafx.h"
#include "../CoupModel.h"
#include "DimNewValue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DimNewValue dialog


DimNewValue::DimNewValue(int NewValue, CWnd* pParent /*=nullptr*/)
	: CDialog(DimNewValue::IDD, pParent)
{
	m_NewValue=NewValue;
	//{{AFX_DATA_INIT(DimNewValue)
	//}}AFX_DATA_INIT
}


void DimNewValue::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DimNewValue)
	DDX_Text(pDX, IDC_EDIT1, m_NewValue);
	DDV_MinMaxInt(pDX, m_NewValue, 1, 10000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DimNewValue, CDialog)
	//{{AFX_MSG_MAP(DimNewValue)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DimNewValue message handlers

void DimNewValue::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}
