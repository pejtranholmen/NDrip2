#if !defined(AFX_DB_PLANT_F_H__F47F9A03_2EC7_11D2_AE76_00C04F95A11E__INCLUDED_)
#define AFX_DB_PLANT_F_H__F47F9A03_2EC7_11D2_AE76_00C04F95A11E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DB_Plant_F.h : header file
//
#include "Base.h"
#include "CoupModelDoc.h"

typedef CTypedPtrArray<CObArray, CEdit*>   	    CEditPtrArray;
typedef CTypedPtrArray<CObArray, CStatic*> 		CStatPtrArray;
typedef CTypedPtrArray<CObArray, CButton*> 		CButtonPtrArray;


/////////////////////////////////////////////////////////////////////////////
// DB_Plant_F dialog

class DB_Plant_F : public CPropertyPage
{
	DECLARE_DYNCREATE(DB_Plant_F)

// Construction
public:
	DB_Plant_F();
	~DB_Plant_F();

// Dialog Data
	//{{AFX_DATA(DB_Plant_F)
	enum { IDD = IDD_PLANT_FILES };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(DB_Plant_F)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CPtrArray m_pCFArray;
	CCoupModelDoc *m_pDoc;
	CString m_GroupName;
	BOOL m_ReadOnly;

protected:
	CEditPtrArray	editarr;
	CButtonPtrArray  m_checkarr;
	CStatPtrArray	statarr;
	void Cleaning();
	// Generated message map functions
	//{{AFX_MSG(DB_Plant_F)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DB_PLANT_F_H__F47F9A03_2EC7_11D2_AE76_00C04F95A11E__INCLUDED_)
