// SP_ImportDlg.cpp : implementation file
//

#include "..\stdafx.h"
#include "..\Util.h"
#include "..\CoupModel.h"
#include ".\sp_importdlg.h"


// SP_ImportDlg dialog

IMPLEMENT_DYNAMIC(SP_ImportDlg, CDialog)
SP_ImportDlg::SP_ImportDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(SP_ImportDlg::IDD, pParent)
	, m_Rows(0)
{
	m_InitDialog=FALSE;
}

SP_ImportDlg::~SP_ImportDlg()
{
}
BOOL SP_ImportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if(!m_InitDialog){
		m_grid.AttachGrid(this, IDC_GRID2);
		m_InitDialog=TRUE;
		m_Rows=m_grid.GetNumberOfRows();
		CDialog::OnInitDialog();
	}
	return TRUE;
}

void SP_ImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT4, m_Rows);
}
void SP_ImportDlg::OnOK() 
{
	CDialog::OnOK();
}

BEGIN_MESSAGE_MAP(SP_ImportDlg, CDialog)
	ON_EN_CHANGE(IDC_EDIT4, OnEnChangeImportNumber)
	ON_BN_CLICKED(ID_IMPORT, OnBnClickedImport)
END_MESSAGE_MAP()


// SP_ImportDlg message handlers

void SP_ImportDlg::OnEnChangeImportNumber()
{
	UpdateData(TRUE);


	m_grid.NewRowNumber(m_Rows);
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void SP_ImportDlg::OnBnClickedImport()
{
	int numberInput;
	numberInput=m_grid.Import();
	string str;
	str=FUtil::STD_ItoAscii(numberInput);
	str+=" profiles were succesfully imported";
	MessageBox(LPCTSTR(str.c_str()),_T("Data imported to Soil Database"),MB_OK);
			
	// TODO: Add your control notification handler code here
}
