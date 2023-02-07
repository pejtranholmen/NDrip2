// ConstrainByInput.cpp : implementation file
//

#include "../stdafx.h"
#include "../CoupModel.h"
#include "ConstrainByInput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ConstrainByInput dialog


ConstrainByInput::ConstrainByInput(float MinValue, float MaxValue, string parname, CWnd* pParent /*=nullptr*/)
	: CDialog(ConstrainByInput::IDD, pParent)
{
	m_NewMin=MinValue;
	m_NewMax = MaxValue;
	m_str = parname;
	//{{AFX_DATA_INIT(ConstrainByInput)
	//}}AFX_DATA_INIT
}

BOOL ConstrainByInput::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog

	// TODO: Add extra initialization here
	SetWindowText(CString(m_str.c_str()));


	return TRUE;  // return TRUE  unless you set the focus to a control
}
void ConstrainByInput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ConstrainByInput)
	DDX_Text(pDX, IDC_EDIT1, m_NewMin);
	DDX_Text(pDX, IDC_EDIT2, m_NewMax);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ConstrainByInput, CDialog)
	//{{AFX_MSG_MAP(ConstrainByInput)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ConstrainByInput message handlers

void ConstrainByInput::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}
