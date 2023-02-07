#if !defined(AFX_MULTIRUNTABLE_H__99D4EE63_46F7_11D2_8E7A_00C04FA92F33__INCLUDED_)
#define AFX_MULTIRUNTABLE_H__99D4EE63_46F7_11D2_8E7A_00C04FA92F33__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MultiRunTable.h : header file
//

#include "afxwin.h"
#include "afxcmn.h"
#include "../CUG\SimpleCug.h"
#include "../Resource.h"
class Doc;

/////////////////////////////////////////////////////////////////////////////
// MultiRunTable dialog

class MultiRunTable : public CDialog
{
// Construction
public:
	MultiRunTable(Doc *pDoc, SimB* pCP, int index, BOOL All=FALSE, CWnd* pParent = nullptr);   // standard constructor

// Dialog Data
	//{{AFX_DATA(MultiRunTable)
	enum { IDD = IDD_DIALOG_MR_TABLE };
	CEdit	m_TableValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MultiRunTable)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	Doc *m_pDoc;
	SimB *m_pCP;
	CDB *m_pDB;
	size_t m_TabIndex;
	BOOL m_IsTableSet, m_InitDialog, m_AllTables;
	SimpleCug m_SimpleGrid;
	int m_linevalid;

	string strvalue, m_SelectedKey, m_strNewValue;	
	// Generated message map functions
	//{{AFX_MSG(MultiRunTable)
	afx_msg void OnFile();
	//virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public :
	virtual BOOL OnInitDialog();
	void Update();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MULTIRUNTABLE_H__99D4EE63_46F7_11D2_8E7A_00C04FA92F33__INCLUDED_)
