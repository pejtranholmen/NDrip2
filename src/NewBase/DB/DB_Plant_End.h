#if !defined(AFX_DB_PLANT_END_H__681A5873_2968_11D2_AE68_00C04F95A11E__INCLUDED_)
#define AFX_DB_PLANT_END_H__681A5873_2968_11D2_AE68_00C04F95A11E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DB_Plant_End.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DB_Plant_End dialog

class DB_Plant_End : public CPropertyPage
{
	DECLARE_DYNCREATE(DB_Plant_End)

// Construction
public:
	DB_Plant_End();
	~DB_Plant_End();

// Dialog Data
	//{{AFX_DATA(DB_Plant_End)
	enum { IDD = IDD_Plant_End };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(DB_Plant_End)
	public:
	virtual BOOL OnWizardFinish();
	virtual LRESULT OnWizardBack();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(DB_Plant_End)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DB_PLANT_END_H__681A5873_2968_11D2_AE68_00C04F95A11E__INCLUDED_)
