#if !defined(AFX_DB_PLANT_PT_H__D5A65B93_265B_11D2_AE63_00C04F95A11E__INCLUDED_)
#define AFX_DB_PLANT_PT_H__D5A65B93_265B_11D2_AE63_00C04F95A11E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DB_Plant_PT.h : header file
//

#include "Base.h"
#include "CoupModelDoc.h"

typedef CTypedPtrArray<CObArray, CEdit*>   	CEditPtrArray;
typedef CTypedPtrArray<CObArray, CStatic*> 	CStatPtrArray;
typedef CTypedPtrArray<CObArray, CButton*> 	CButtonPtrArray;


/////////////////////////////////////////////////////////////////////////////
// DB_Plant_PT dialog

class DB_Plant_PT : public CPropertyPage
{
	DECLARE_DYNCREATE(DB_Plant_PT)

// Construction
public:
	DB_Plant_PT();
	~DB_Plant_PT();

// Dialog Data
	//{{AFX_DATA(DB_Plant_PT)
	enum { IDD = IDD_PLANT_PT };
	int		m_NumRows;
	CString	m_TabName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(DB_Plant_PT)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	virtual LRESULT OnWizardBack();
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CCoupModelDoc *m_pDoc;
	CTab		*m_pCTab;
	BOOL		m_Last;
	BOOL m_ReadOnly;
protected:
	void Cleaning();
	CEditPtrArray	editarr;
	CStatPtrArray	statarr;
	CStatPtrArray	indarr;
	CButtonPtrArray m_checkarr;
	CStatPtrArray	static_type4Arr;
	CPtrArray		m_pCPtArray;
	CFloatArray		m_IniValues;




	CEdit *m_edit;
	CScrollBar	vscroll;
	CScrollBar	hscroll;
	CWnd*	ParentWindow;


	// Generated message map functions
	//{{AFX_MSG(DB_Plant_PT)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKillfocusNumRows();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL SetValue();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DB_PLANT_PT_H__D5A65B93_265B_11D2_AE63_00C04F95A11E__INCLUDED_)
