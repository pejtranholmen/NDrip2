#if !defined(AFX_SP_EDIT_RETENTION_H__A2733283_197F_11D2_AE5C_00C04F95A11E__INCLUDED_)
#define AFX_SP_EDIT_RETENTION_H__A2733283_197F_11D2_AE5C_00C04F95A11E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SP_EDIT_RETENTION.h : header file
//
typedef CTypedPtrArray<CObArray, CEdit*>   	CEditPtrArray;
typedef CTypedPtrArray<CObArray, CStatic*> 	CStatPtrArray;

/////////////////////////////////////////////////////////////////////////////
// SP_EDIT_RETENTION dialog

class SP_EDIT_RETENTION : public CDialog
{
// Construction
public:
	 ~SP_EDIT_RETENTION();
	float m_PFlevels[12];
	float m_WaterContent[20][12] ;
	SP_EDIT_RETENTION(CObject *pView, CString ProfileName,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SP_EDIT_RETENTION)
	enum { IDD = IDD_SP_RETENTION };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SP_EDIT_RETENTION)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CEditPtrArray	editarr;
	CStatPtrArray	statarr;
	CStatPtrArray	indarr;
	CCoupModelView *m_pView;
	CString m_ProfileName;


	CEdit *m_edit;
	CScrollBar	vscroll;
	CScrollBar	hscroll;
	CWnd*	ParentWindow;
public:

	// Generated message map functions
	//{{AFX_MSG(SP_EDIT_RETENTION)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnButtonView();
	afx_msg void OnButtonViewLayers();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SP_EDIT_RETENTION_H__A2733283_197F_11D2_AE5C_00C04F95A11E__INCLUDED_)
