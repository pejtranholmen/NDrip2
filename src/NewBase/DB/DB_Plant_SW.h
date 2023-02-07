#if !defined(AFX_DB_PLANT_SW_H__8C75A853_2648_11D2_AE63_00C04F95A11E__INCLUDED_)
#define AFX_DB_PLANT_SW_H__8C75A853_2648_11D2_AE63_00C04F95A11E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DB_Plant_SW.h : header file
//
#include "Base.h"
#include "CoupModelDoc.h"
#include "CoupModel.h"

typedef CTypedPtrArray<CObArray, CComboBox*>   	CComboBoxPtrArray;
typedef CTypedPtrArray<CObArray, CStatic*> 		CStatPtrArray;
typedef CTypedPtrArray<CObArray, CEdit*> 		CEditPtrArray;
typedef CTypedPtrArray<CObArray, CButton*> 		CButtonPtrArray;


/////////////////////////////////////////////////////////////////////////////
// DB_Plant_SW dialog

class DB_Plant_SW : public CPropertyPage
{
	DECLARE_DYNCREATE(DB_Plant_SW)

// Construction
public:
	DB_Plant_SW();
	~DB_Plant_SW();

// Dialog Data
	//{{AFX_DATA(DB_Plant_SW)
	enum { IDD = IDD_Plant_SW };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(DB_Plant_SW)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CPtrArray m_pSWArray;
	CCoupModelDoc *m_pDoc;
	CString m_GroupName;
	BOOL m_ReadOnly;
protected:
	CPtrArray m_pValidSWArray;
	CComboBoxPtrArray	comboarr;
	CStatPtrArray		statarr;
	CEditPtrArray		editarr;
	CButtonPtrArray		m_checkarr;


	CComboBox	*m_combo;
	CEdit		*m_pEdit;
	CScrollBar	vscroll;
	CScrollBar	hscroll;
	CWnd*		ParentWindow;
	CUIntArray  m_IniValues;


protected:
	void CleaningUp();
	// Generated message map functions
	//{{AFX_MSG(DB_Plant_SW)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DB_PLANT_SW_H__8C75A853_2648_11D2_AE63_00C04F95A11E__INCLUDED_)
