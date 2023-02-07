// DB_Plant_End.cpp : implementation file
//

#include "stdafx.h"
#include "CoupModel.h"
#include "DB_Plant_End.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DB_Plant_End property page

IMPLEMENT_DYNCREATE(DB_Plant_End, CPropertyPage)

DB_Plant_End::DB_Plant_End() : CPropertyPage(DB_Plant_End::IDD)
{
	//{{AFX_DATA_INIT(DB_Plant_End)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

DB_Plant_End::~DB_Plant_End()
{
}

void DB_Plant_End::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DB_Plant_End)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DB_Plant_End, CPropertyPage)
	//{{AFX_MSG_MAP(DB_Plant_End)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DB_Plant_End message handlers

BOOL DB_Plant_End::OnWizardFinish() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnWizardFinish();
}

LRESULT DB_Plant_End::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnWizardBack();
}

BOOL DB_Plant_End::OnSetActive() 
{
	CPropertySheet* parent = (CPropertySheet*)GetParent();
	parent->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
	
	return CPropertyPage::OnSetActive();
}
