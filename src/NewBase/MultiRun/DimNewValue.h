#if !defined(AFX_DIMNEWVALUE_H__1EFA96E1_B383_11D1_ADB0_00C04F95A11E__INCLUDED_)
#define AFX_DIMNEWVALUE_H__1EFA96E1_B383_11D1_ADB0_00C04F95A11E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DimNewValue.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DimNewValue dialog

class DimNewValue : public CDialog
{
// Construction
public:
	DimNewValue(int NewValue, CWnd* pParent = nullptr);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DimNewValue)
	enum { IDD = IDD_DIALOG_DIMNEW };
	int		m_NewValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DimNewValue)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(DimNewValue)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIMNEWVALUE_H__1EFA96E1_B383_11D1_ADB0_00C04F95A11E__INCLUDED_)
