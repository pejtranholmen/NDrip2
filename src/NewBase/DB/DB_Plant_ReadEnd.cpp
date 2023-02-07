// DB_Plant_ReadEnd.cpp : implementation file
//

#include "stdafx.h"
#include "CoupModel.h"
#include "DB_Plant_ReadEnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DB_Plant_ReadEnd property page

IMPLEMENT_DYNCREATE(DB_Plant_ReadEnd, CPropertyPage)

DB_Plant_ReadEnd::DB_Plant_ReadEnd() : CPropertyPage(DB_Plant_ReadEnd::IDD)
{
	//{{AFX_DATA_INIT(DB_Plant_ReadEnd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

DB_Plant_ReadEnd::~DB_Plant_ReadEnd()
{
}

void DB_Plant_ReadEnd::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DB_Plant_ReadEnd)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DB_Plant_ReadEnd, CPropertyPage)
	//{{AFX_MSG_MAP(DB_Plant_ReadEnd)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DB_Plant_ReadEnd message handlers

BOOL DB_Plant_ReadEnd::OnWizardFinish() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnWizardFinish();
}

BOOL DB_Plant_ReadEnd::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet* parent = (CPropertySheet*)GetParent();
	parent->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
	
	return CPropertyPage::OnSetActive();
}
