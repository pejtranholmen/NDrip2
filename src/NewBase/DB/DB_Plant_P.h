#if !defined(AFX_DB_PLANT_P_H__8C75A854_2648_11D2_AE63_00C04F95A11E__INCLUDED_)
#define AFX_DB_PLANT_P_H__8C75A854_2648_11D2_AE63_00C04F95A11E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Base.h"
#include "CoupModelDoc.h"

typedef CTypedPtrArray<CObArray, CEdit*>   	CEditPtrArray;
typedef CTypedPtrArray<CObArray, CStatic*>   CStatPtrArray;
typedef CTypedPtrArray<CObArray, CButton*> 	CButtonPtrArray;


// DB_Plant_P.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DB_Plant_P dialog

class DB_Plant_P : public CPropertyPage
{
	DECLARE_DYNCREATE(DB_Plant_P)

// Construction
public:
	DB_Plant_P();
	~DB_Plant_P();

// Dialog Data
	//{{AFX_DATA(DB_Plant_P)
	enum { IDD = IDD_PLANT_P };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(DB_Plant_P)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public :
	CCoupModelDoc *m_pDoc;
	CPtrArray	m_pCPArray;
	BOOL m_ReadOnly;

protected:
	CEditPtrArray	editarr;
	CButtonPtrArray  m_checkarr;
	CStatPtrArray	statarr;
	void Cleaning();




	CEdit			*m_edit;
	CScrollBar		vscroll;
	CScrollBar		hscroll;
	CFloatArray		m_IniValues;


	// Generated message map functions
	//{{AFX_MSG(DB_Plant_P)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DB_PLANT_P_H__8C75A854_2648_11D2_AE63_00C04F95A11E__INCLUDED_)
