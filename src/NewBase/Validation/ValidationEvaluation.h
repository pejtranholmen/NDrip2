#pragma once

#include "../Cug\EvaluateMulti\ValidationCug.h"
class Doc;
// ValidationEvaluation dialog

class ValidationEvaluation : public CDialog
{
	DECLARE_DYNAMIC(ValidationEvaluation)

public:
	ValidationEvaluation(CCoupModelDoc* pDoc, CWnd* pParent = NULL, bool ResetAll=false);   // standard constructor
	virtual ~ValidationEvaluation();
	virtual INT_PTR DoModal();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	bool m_Changed;
	CString m_WindowText;
// Dialog Data
	enum { IDD = IDD_VALIDATIONEVALUATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	ValidationCug m_GridValidation;
	CWnd *m_pCugWnd;
	
	bool m_Init;

	Doc *m_pSimDoc;
	DECLARE_MESSAGE_MAP()
};
