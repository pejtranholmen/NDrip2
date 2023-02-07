// CalculateVariable.cpp : implementation file
//

//Hej will show changes made   ddnnnn
#include "../stdafx.h"
#include "../CoupModel.h"
#include "./calculatevariable.h"


// CCalculateVariable dialog

IMPLEMENT_DYNAMIC(CCalculateVariable, CDialog)
CCalculateVariable::CCalculateVariable(CWnd* pParent /*=nullptr*/)
	: CDialog(CCalculateVariable::IDD, pParent)
{
	m_Init=TRUE;
}
void CCalculateVariable::Init(void *pPG)
{
	m_GridCalc.Init(0, pPG, &m_GridDes);
	m_GridDes.Init(1, pPG, &m_GridCalc);
	m_pPG=pPG;
}
CCalculateVariable::~CCalculateVariable()
{
}

void CCalculateVariable::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCalculateVariable, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_RESET, OnBnClickedButtonReset)
END_MESSAGE_MAP()


BOOL CCalculateVariable::OnInitDialog() {

	CDialog::OnInitDialog();
	if(m_Init) {
		m_GridCalc.AttachGrid(this, IDC_GRID);
		m_Init=FALSE;
		m_GridDes.AttachGrid(this, IDC_GRID2);
	}

	return TRUE;

}

// CCalculateVariable message handlers

void CCalculateVariable::OnBnClickedButtonReset()
{
	// TODO: Add your control notification handler code here
	((CPG*)m_pPG)->ResetSelections();
	m_GridCalc.OnSetup();
	m_GridDes.OnSetup();
}
