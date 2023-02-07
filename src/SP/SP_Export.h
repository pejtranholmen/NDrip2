#if !defined(AFX_SP_EXPORT_H__8ABA3453_668A_11D5_8296_E2AF02C01B53__INCLUDED_)
#define AFX_SP_EXPORT_H__8ABA3453_668A_11D5_8296_E2AF02C01B53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SP_Export.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SP_Export dialog

class SP_Export : public CDialog
{
// Construction
public:
	int Convert();
	SP_Export(CWnd* pParent = nullptr);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SP_Export)
	enum { IDD = IDD_DIALOG_SP_EXPORT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SP_Export)
	public:
	virtual INT_PTR DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SP_Export)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SP_EXPORT_H__8ABA3453_668A_11D5_8296_E2AF02C01B53__INCLUDED_)
