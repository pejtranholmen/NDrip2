#if !defined(AFX_DB_PLANT_E_H__9F47E4E4_3B7D_11D2_AE8E_00C04F95A11E__INCLUDED_)
#define AFX_DB_PLANT_E_H__9F47E4E4_3B7D_11D2_AE8E_00C04F95A11E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Base.h"
#include "CoupModelDoc.h"

typedef CTypedPtrArray<CObArray, CEdit*>   	CEditPtrArray;
typedef CTypedPtrArray<CObArray, CStatic*>   CStatPtrArray;
typedef CTypedPtrArray<CObArray, CButton*> 	CButtonPtrArray;


// DB_Plant_E.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DB_Plant_E dialog

class DB_Plant_E : public CPropertyPage
{
	DECLARE_DYNCREATE(DB_Plant_E)

// Construction
public:
	DB_Plant_E();
	~DB_Plant_E();

// Dialog Data
	//{{AFX_DATA(DB_Plant_E)
	enum { IDD = IDD_PLANT_E };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(DB_Plant_E)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_VIRTUAL

// Implementation
public :
	CCoupModelDoc *m_pDoc;
	CPtrArray	m_pCTabArray;
	BOOL m_ReadOnly;

protected:
	CEditPtrArray	editarr;
	CButtonPtrArray  m_checkarr;
	CStatPtrArray	statarr;
	void Cleaning();

	// Generated message map functions
	//{{AFX_MSG(DB_Plant_E)
		// NOTE: the ClassWizard will add member functions here
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DB_PLANT_E_H__9F47E4E4_3B7D_11D2_AE8E_00C04F95A11E__INCLUDED_)
