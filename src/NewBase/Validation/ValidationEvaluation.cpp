// ValidationEvaluation.cpp : implementation file
//

#include "../stdafx.h"
#include "../CoupModel.h"

#include "../CUG\SimDoc\MyCug.h"

#include "../CoupModelDoc.h"



#include "ValidationEvaluation.h"


// ValidationEvaluation dialog

IMPLEMENT_DYNAMIC(ValidationEvaluation, CDialog)
ValidationEvaluation::ValidationEvaluation(CCoupModelDoc* pDoc,CWnd* pParent, bool ResetAll /*=NULL*/)
	: CDialog(ValidationEvaluation::IDD, pParent)
{
	m_GridValidation.SetpDoc(pDoc);
	m_GridValidation.SetpDlg(this);
	m_pSimDoc=&pDoc->m_Doc;
	if(ResetAll) {
			m_pSimDoc->m_MStorage.UpdateAcceptedRuns(true, true);

	}
	m_pCugWnd=pParent;
	m_Init=true;
	m_Changed=false;
}

ValidationEvaluation::~ValidationEvaluation()
{
}

void ValidationEvaluation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
BOOL ValidationEvaluation::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if(m_Init) {
		m_Init=false;
		m_GridValidation.AttachGrid(this, IDC_GRID);
		
		m_WindowText="Definition of Accepted simulations";
		SetWindowText(m_WindowText);
		
	}
	
	return TRUE;
}
void ValidationEvaluation::OnOK() 
{
	if(!m_pSimDoc->m_MStorage.m_Lock&&m_Changed) {
		m_pSimDoc->m_MStorage.UpdateAcceptedRuns(false, true);
		m_pSimDoc->MR_Reset_MeanResiduals();
	}
	CDialog::OnOK();
}

INT_PTR ValidationEvaluation::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DoModal();
}
BEGIN_MESSAGE_MAP(ValidationEvaluation, CDialog)
END_MESSAGE_MAP()


// ValidationEvaluation message handlers
