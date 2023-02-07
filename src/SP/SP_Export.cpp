// SP_Export.cpp : implementation file
//

#include "..\stdafx.h"
#include "..\CoupModel.h"
#include ".\SP_Export.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// SP_Export dialog


SP_Export::SP_Export(CWnd* pParent /*=nullptr*/)
	: CDialog(SP_Export::IDD, pParent)
{
	//{{AFX_DATA_INIT(SP_Export)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void SP_Export::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SP_Export)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SP_Export, CDialog)
	//{{AFX_MSG_MAP(SP_Export)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SP_Export message handlers

INT_PTR SP_Export::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DoModal();
}

int SP_Export::Convert()
{
   return 1;//PLOTPF_mp_EXPORT();
}
