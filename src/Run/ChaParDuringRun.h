#if !defined(AFX_CHAPARDURINGRUN_H__4C0EFF03_AEF5_11D1_ADA6_00C04F95A11E__INCLUDED_)
#define AFX_CHAPARDURINGRUN_H__4C0EFF03_AEF5_11D1_ADA6_00C04F95A11E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif 

#include "..\ModelTypes\SimB.h"
class Doc;

// _MSC_VER >= 1000
// ChaParDuringRun.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChaParDuringRun dialog

class CChaParDuringRun : public CDialog
{
// Construction
public:
	CChaParDuringRun(SIMB Par, Doc* pDoc,CWnd* pParent = nullptr);   // standard P constructor

// Dialog Data
	//{{AFX_DATA(CChaParDuringRun)
	enum { IDD = IDD_DIALOG_CHAPAR };
	CString	m_year;
	CString	m_month;
	CString	m_min;
	CString	m_hour;
	CString	m_day;
	int		m_Selected;
	CString	m_SimStart;
	CString	m_SimEnd;
	CString	m_strNewValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChaParDuringRun)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_TabIndex;
	enum simtype m_Type;
	Doc* m_pDoc;
	Ps* m_pPs;
	P* m_pP;
	CDB* m_pDB;
	SIMB m_Par;
	CString m_Datum;
	CString m_strValue;
	CString m_SelectedKey;
	float m_NewValue;
	float m_Pmin,m_Pmax;
	// Generated message map functions
	//{{AFX_MSG(CChaParDuringRun)
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonAssign();
	afx_msg void OnButtonDelete();
	afx_msg void OnButtonReset();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonSelect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHAPARDURINGRUN_H__4C0EFF03_AEF5_11D1_ADA6_00C04F95A11E__INCLUDED_)
