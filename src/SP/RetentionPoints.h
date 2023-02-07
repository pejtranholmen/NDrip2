#if !defined(AFX_RETENTIONPOINTS_H__2E4849B3_080F_11D2_AE30_00C04F95A11E__INCLUDED_)
#define AFX_RETENTIONPOINTS_H__2E4849B3_080F_11D2_AE30_00C04F95A11E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "..\ModelTypes\SimB.h"
#include ".\RetentionPoints.h "
#include ".\SP_Estimate.h"

// header file
//

/////////////////////////////////////////////////////////////////////////////
// RetentionPoints dialog

class RetentionPoints : public CDialog
{
// Construction
public:
	RetentionPoints(SP_Estimate *pSP_Est, CWnd* pParent = NULL);   // standard constructor

	SP_Estimate *m_pSP_Est;
// Dialog Data
	//{{AFX_DATA(RetentionPoints)
	enum { IDD = IDD_DIALOG_RETENTION };
	float	m_Pressure;
	float	m_Theta;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(RetentionPoints)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(RetentionPoints)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnApply();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RETENTIONPOINTS_H__2E4849B3_080F_11D2_AE30_00C04F95A11E__INCLUDED_)
