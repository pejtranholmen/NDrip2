// SelectPGTimeRange.cpp : implementation file
//

//Hej will show changes made   ddnnnn
#include "../stdafx.h"
#include "../CoupModel.h"
#include "./SelectPGTimeRange.h"


// SelectPGTimeRange dialog

IMPLEMENT_DYNAMIC(SelectPGTimeRange, CDialog)
SelectPGTimeRange::SelectPGTimeRange(CWnd* pParent /*=nullptr*/)
	: CDialog(SelectPGTimeRange::IDD, pParent)
{
	m_Init=TRUE;
}
void SelectPGTimeRange::Init(void *pPG)
{
	m_GridDate.Init(2, pPG, &m_GridDate);
	m_pPG=pPG;
}
SelectPGTimeRange::~SelectPGTimeRange()
{
}

void SelectPGTimeRange::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SelectPGTimeRange, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_RESET, OnBnClickedButtonReset)
END_MESSAGE_MAP()


BOOL SelectPGTimeRange::OnInitDialog() {

	CDialog::OnInitDialog();
	if(m_Init) {
		m_Init=FALSE;
		m_GridDate.AttachGrid(this, IDC_DATE_IN_GRID);
		SetWindowText(_T("Select Date Range (red lines valid)"));
	}

	return TRUE;

}

// SelectPGTimeRange message handlers

void SelectPGTimeRange::OnBnClickedButtonReset()
{
	// TODO: Add your control notification handler code here
	((CPG*)m_pPG)->ResetSelections();

}
