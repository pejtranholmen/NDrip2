#if !defined(AFX_SP_EXCHANGE1_H__2A616184_1902_11D2_AE5B_00C04F95A11E__INCLUDED_)
#define AFX_SP_EXCHANGE1_H__2A616184_1902_11D2_AE5B_00C04F95A11E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SP_EXCHANGE1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SP_EXCHANGE1 dialog

class SP_EXCHANGE1 : public CDialog
{
// Construction
public:
	SP_EXCHANGE1(CCoupModelDoc *pDoc, CWnd* pParent = nullptr);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SP_EXCHANGE1)
	enum { IDD = IDD_DIALOG_SP_EXCHANGE1 };
	CListBox	m_ListMain1;
	CListBox	m_ListLocal1;
	CString	m_FileName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SP_EXCHANGE1)
	public:
	virtual INT_PTR DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_LocalFile;
	CCoupModelDoc *m_pDoc;
	// Generated message map functions
	//{{AFX_MSG(SP_EXCHANGE1)
	afx_msg void OnButtonFile();
	afx_msg void OnMoveToLocal();
	afx_msg void OnMoveToMain();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
 	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SP_EXCHANGE1_H__2A616184_1902_11D2_AE5B_00C04F95A11E__INCLUDED_)
