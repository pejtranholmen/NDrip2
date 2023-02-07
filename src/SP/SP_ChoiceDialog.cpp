// SP_ChoiceDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CoupModel.h"
#include "SP_ChoiceDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SP_ChoiceDialog dialog


SP_ChoiceDialog::SP_ChoiceDialog(CWnd* pParent /*=NULL*/)
	: CDialog(SP_ChoiceDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(SP_ChoiceDialog)
	m_exist = 0;
	//}}AFX_DATA_INIT
}


void SP_ChoiceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SP_ChoiceDialog)
	DDX_Radio(pDX, IDC_RADIO1, m_exist);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SP_ChoiceDialog, CDialog)
	//{{AFX_MSG_MAP(SP_ChoiceDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SP_ChoiceDialog message handlers
