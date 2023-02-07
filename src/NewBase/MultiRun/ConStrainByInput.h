#if !defined(AFX_DIMNEWVALUE_H__1EFA96E1_B383_11D1_ADB0_00C04F95A11E__INCLUDED_)
#define AFX_DIMNEWVALUE_H__1EFA96E1_B383_11D1_ADB0_00C04F95A11E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ConstrainByInput.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ConstrainByInput dialog

class ConstrainByInput : public CDialog
{
// Construction
public:
	ConstrainByInput(float MinValue, float MaxValue, string str="", CWnd* pParent = nullptr);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ConstrainByInput)
	enum { IDD = IDD_DIALOG_DIMNEW1 };
	float	m_NewMin, m_NewMax;
	string m_str;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ConstrainByInput)
	protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(ConstrainByInput)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIMNEWVALUE_H__1EFA96E1_B383_11D1_ADB0_00C04F95A11E__INCLUDED_)
