#if !defined(AFX_SP_EDIT_H__DFB6D403_17E7_11D2_8E72_00C04FA92F33__INCLUDED_)
#define AFX_SP_EDIT_H__DFB6D403_17E7_11D2_8E72_00C04FA92F33__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SP_EDIT.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SP_EDIT dialog

class SP_EDIT : public CDialog
{
// Construction
public:
	SP_EDIT(CCoupModelDoc *pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SP_EDIT)
	enum { IDD = IDD_DIALOG_SP_EDIT };
	CEdit	m_PFcom;
	CListBox	m_Selection;
	CString	m_SiteName;
	CString	m_County;
	CString	m_Country;
	CString	m_CoordLat;
	CString	m_CoordLong;
	float	m_SubClay;
	float	m_SubSand;
	float	m_SubOrg;
	float	m_TopClay;
	float	m_TopSand;
	float	m_TopOrg;
	UINT	m_Prof;
	UINT	m_RepProf;
	UINT    m_modLayer;
	UINT	m_NumLayers;
	UINT	m_Year;
	BOOL	m_MoveCursor;
	int		m_NumTens;
	int		m_iniProf;
	int		m_iniRepProf;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SP_EDIT)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_ChangedData;
	CCoupModelDoc *m_pDoc;
	CCoupModelView *m_pView;
	int m_CurSel;
	BOOL m_SaveNewNum;
	BOOL m_profchanged;
	// Generated message map functions
	//{{AFX_MSG(SP_EDIT)
	afx_msg void OnButtonApply();
	afx_msg void OnButtonDelete();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListSelection();
	afx_msg void OnChangeEditRep();
	afx_msg void OnChangeEditProf();
	afx_msg void OnKillfocusEditRep();
	afx_msg void OnKillfocusEditProf();
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTextureEdit();
	afx_msg void OnRetentionEdit();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnCoef();
	afx_msg void OnKillfocusEditNumLayers();
	afx_msg void OnButtonView();
	afx_msg void OnButtonViewLayers();
	afx_msg void OnButtonViewLayersRet();
	afx_msg void OnButtonViewRet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SP_EDIT_H__DFB6D403_17E7_11D2_8E72_00C04FA92F33__INCLUDED_)
