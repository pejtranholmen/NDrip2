#if !defined(AFX_MULTIRUNDB_H__69433B73_4C8D_11D2_8E7C_00C04FA92F33__INCLUDED_)
#define AFX_MULTIRUNDB_H__69433B73_4C8D_11D2_8E7C_00C04FA92F33__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MultiRunDB.h : header file
//
#include "../ModelTypes\SimB.h"
#include "../CoupModelDoc.h"
#include "./PP_Edit.h"




/////////////////////////////////////////////////////////////////////////////
// MultiRunDB dialog

class MultiRunDB : public CDialog
{
// Construction
public:
	MultiRunDB(CDB* pPar, CCoupModelDoc* pDoc, CWnd* pParent = NULL);   // standard Ps constructor

// Dialog Data
	//{{AFX_DATA(MultiRunDB)
	enum { IDD = IDD_MULTI_RUN_DB };
	CListBox	m_Dimension;
	CListBox	m_DataRecords;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MultiRunDB)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CCoupModelDoc* m_pDoc;
	CDB* m_pDB;
	Ps* m_pCP;
	CString	m_SelectedKey;
	CString	m_strNewValue;
	int m_Selected_Dim;
	int m_TabIndex;
	int m_select;


	// Generated message map functions
	//{{AFX_MSG(MultiRunDB)
	afx_msg void OnDatabaseAdd();
	afx_msg void OnDatabaseReset();
	afx_msg void OnButtoApply();
	afx_msg void OnButtonChange();
	afx_msg void OnBUTTONCounter();
	afx_msg void OnButtonDeleteDim();
	afx_msg void OnButtonNewdim();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MULTIRUNDB_H__69433B73_4C8D_11D2_8E7C_00C04FA92F33__INCLUDED_)
