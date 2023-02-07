#if !defined(AFX_SP_EDIT_COEF_H__B35C8833_37A9_11D2_AE89_00C04F95A11E__INCLUDED_)
#define AFX_SP_EDIT_COEF_H__B35C8833_37A9_11D2_AE89_00C04F95A11E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SP_EDIT_COEF.h : header file
//
typedef CTypedPtrArray<CObArray, CEdit*>   	CEditPtrArray;
typedef CTypedPtrArray<CObArray, CStatic*> 	CStatPtrArray;


/////////////////////////////////////////////////////////////////////////////
// SP_EDIT_COEF dialog

class SP_EDIT_COEF : public CDialog
{
// Construction
public:
	SP_EDIT_COEF(CWnd* pParent = NULL);   // standard constructor
	~SP_EDIT_COEF();   // standard destructor
	float m_Coef[22][17];
// Dialog Data
	//{{AFX_DATA(SP_EDIT_COEF)
	enum { IDD = IDD_SP_COEF };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SP_EDIT_COEF)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CEditPtrArray	editarr;
	CStatPtrArray	statarr;
	CStatPtrArray	indarr;


	CEdit *m_edit;
	CScrollBar	vscroll;
	CScrollBar	hscroll;
	CWnd*	ParentWindow;


	// Generated message map functions
	//{{AFX_MSG(SP_EDIT_COEF)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SP_EDIT_COEF_H__B35C8833_37A9_11D2_AE89_00C04F95A11E__INCLUDED_)
