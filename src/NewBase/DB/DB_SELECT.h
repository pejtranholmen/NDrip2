#if !defined(AFX_DB_SELECT_H__C45613A4_3074_11D2_AE7B_00C04F95A11E__INCLUDED_)
#define AFX_DB_SELECT_H__C45613A4_3074_11D2_AE7B_00C04F95A11E__INCLUDED_

#include "CoupModelDoc.h"	// Added by ClassView
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DB_SELECT.h : header file
//
enum DB_SelTypes{
	SelCurrent,
	SelChaPar,
	SelMulti
};

/////////////////////////////////////////////////////////////////////////////
// DB_SELECT dialog

class DB_SELECT : public CDialog
{
// Construction
public:
	DB_SELECT(CWnd* pParent = NULL);   // standard constructor
	DB_SELECT(DB_SelTypes, CCoupModelDoc *pDoc,CWnd* pParent = NULL);   // standard constructor

	DB_SelTypes m_SelType;
// Dialog Data
	//{{AFX_DATA(DB_SELECT)
	enum { IDD = IDD_DIALOG_DB_SELECT };
	CString	m_SelDrain;
	CString	m_SelPlant;
	CString	m_SelSnow;
	CString	m_SelSoilEvap;
	CString	m_SelSoilFrost;
	CString	m_SelSoilProp;
	CString m_SelGrowth;
	CString m_SelSoilManage;
	CString m_SelOrganic;
	CString m_SelInorganic;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DB_SELECT)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CCoupModelDoc *m_pDoc;
	CSw *pNitrogen, *pWaterEq;
	BOOL m_ReAllocateSoil;
	BOOL m_ApplyPlant, m_ApplySnow,m_ApplySoilFrost;
	BOOL m_ApplySoilEvaporation, m_ApplyWaterFlow;
	BOOL m_ApplyGrowth,m_ApplySoilManage,m_ApplyOrganic, m_ApplyInorganic;
	BOOL m_ReAllocate;
	PP_EDIT *m_pEditPlant,*m_pEditSoilEvap,*m_pEditWaterF;
	PP_EDIT *m_pEditSnow,*m_pEditSoilFrost;
	PP_EDIT *m_pEditGrowth,*m_pEditSoilManage,*m_pEditOrganic,*m_pEditInorganic;


	int m_NumSoilLayers;
	// Generated message map functions
	//{{AFX_MSG(DB_SELECT)
	afx_msg void OnDbPlant();
	afx_msg void OnDbSnow();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDbSoilProperties();
	virtual BOOL OnInitDialog();
	afx_msg void OnDbSoilEvaporation();
	afx_msg void OnDbSoilFrost();
	afx_msg void OnDbWaterFlow();
	afx_msg void OnUpdateSoilProperties(CCmdUI* pCmdUI);
	afx_msg void OnDbGrowth();
	afx_msg void OnDbSoilmanage();
	afx_msg void OnDbOrganic();
	afx_msg void OnDbInorganic();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DB_SELECT_H__C45613A4_3074_11D2_AE7B_00C04F95A11E__INCLUDED_)
