#if !defined(AFX_SP_CHOICEDIALOG_H__7AE227A3_159C_11D2_8E72_00C04FA92F33__INCLUDED_)
#define AFX_SP_CHOICEDIALOG_H__7AE227A3_159C_11D2_8E72_00C04FA92F33__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SP_ChoiceDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SP_ChoiceDialog dialog

class SP_ChoiceDialog : public CDialog
{
// Construction
public:
	SP_ChoiceDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SP_ChoiceDialog)
	enum { IDD = IDD_DIALOG_SOIL_CHOICE };
	int		m_exist;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SP_ChoiceDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SP_ChoiceDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SP_CHOICEDIALOG_H__7AE227A3_159C_11D2_8E72_00C04FA92F33__INCLUDED_)
