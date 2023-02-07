#if !defined(AFX_SP_INITCHAR_H__FC143623_162B_11D2_8E72_00C04FA92F33__INCLUDED_)
#define AFX_SP_INITCHAR_H__FC143623_162B_11D2_8E72_00C04FA92F33__INCLUDED_

#include "SP_Save.h"	// Added by ClassView
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SP_InitChar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SP_InitChar dialog

class SP_InitChar : public CDialog
{
friend class SP_Save;
// Construction
public:
	SP_InitChar(SP_Save *pSP_Save, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SP_InitChar)
	enum { IDD = IDD_DIALOG_SP_INIT };
	float	m_Clay;
	float	m_Sand;
	float	m_Saturation;
	float	m_UpperDepth;
	float	m_LowerDepth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SP_InitChar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	SP_Save *m_pSP_Save;

	// Generated message map functions
	//{{AFX_MSG(SP_InitChar)
	afx_msg void OnApply();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SP_INITCHAR_H__FC143623_162B_11D2_8E72_00C04FA92F33__INCLUDED_)
