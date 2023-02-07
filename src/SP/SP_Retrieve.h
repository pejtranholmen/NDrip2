#if !defined(AFX_SP_RETRIEVE_H__2C1D1101_1179_11D2_AE4A_00C04F95A11E__INCLUDED_)
#define AFX_SP_RETRIEVE_H__2C1D1101_1179_11D2_AE4A_00C04F95A11E__INCLUDED_

#include "..\CoupModelDoc.h"	// Added by ClassView
#include "afxwin.h"
#include "..\CUG\PFCUG\pfCug.h"
#include "..\SoilData\PlotPF\PFCurve.h"
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SP_Retrieve.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SP_Retrieve dialog
class PFCurve;
class Doc;
class SP_Retrieve : public CDialog
{
// Construction
public:
	string GetCurrentName();
	SP_Retrieve(Doc * pDoc, CWnd* pParent = nullptr);   // standard constructor
	SP_Retrieve(BOOL Reset,Doc * pDoc, CWnd* pParent = nullptr); 
	SP_Retrieve(BOOL Reset,Doc * pDoc, CDB*, int, CWnd* pParent = nullptr); 
	SP_Retrieve(int prof, int repprof, CWnd* pParent = nullptr); 
	SP_Retrieve(string key, CWnd* pParent = nullptr); 
	~SP_Retrieve();
	void SetComments(string);
	string m_SelectedId;


// Dialog Data
	//{{AFX_DATA(SP_Retrieve)
	enum { IDD = IDD_DIALOG_Retrieve };
		CEdit	m_PFcom;
	CListBox	m_Selection;
	UINT	m_Layer;
	UINT	m_Prof;
	UINT	m_RepNumber;
	string	m_ProfName;
	string	m_NumLayers;
	string	m_County;
	string	m_Country;
	string	m_Year;
	string	m_Coordinates;
	UINT	m_ProfMin;
	UINT	m_TopSandMin;
	UINT	m_SubSandMin;
	UINT	m_SubSandMax;
	UINT	m_SubClayMin;
	UINT	m_SubClayMax;
	UINT	m_ProfMax;
	string	m_CountyMin;
	string	m_CountyMax;
	UINT	m_TopSandMax;
	UINT	m_TopClayMin;
	UINT	m_TopClayMax;
	UINT	m_TopOrgMin;
	UINT	m_TopOrgMax;
	UINT	m_SubOrgMin;
	UINT	m_SubOrgMax;
	string	m_PFpoints;
	UINT	m_YearMin;
	UINT	m_YearMax;
	string	m_charYear;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SP_Retrieve)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	Doc *m_pDoc;
	CObject *m_pView;
	Doc *m_pSimDoc;
	void OnChangeProf();
	void IniDataBase();
	BOOL m_ReadOnly;
	vector<string> m_arrComments;
	int m_IniProf, m_IniRepProf;
	int m_LayersInModelTable;
	size_t m_NumToBeSelected;
	BOOL m_InitDialog;
	CDB *m_pDB;
	PFCurve *p_PFCurve;
	NEWHEADER m_ActHeader;
	NEWHEADER m_NewHeader;

	// Generated message map functions
	//{{AFX_MSG(SP_Retrieve)
	afx_msg void OnButtonScan();
	afx_msg void OnButtonScanAll();
	afx_msg void OnButtonSelect();
//	virtual void OnOK();
	afx_msg void OnSelchangeListSelection();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnButtonView();
	afx_msg void OnButtonViewLayers();
	afx_msg void OnButtonViewLayersRet();
	afx_msg void OnButtonViewRet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	PFCug m_SoilPropCug;
	PFCug m_SoilPropData;
	PFCug m_Prof_ToSim;
	PFCug m_EstimationInfo;
	BOOL m_CompleteList;
	void OnAddComments(string str);
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnEnChangeEdit4();
	unsigned int m_Min_NumHeads;
	// This includes details
	string m_Details;
	afx_msg void OnEnChangeComment();
	afx_msg void OnBnClickedButonDelete();
	afx_msg void OnBnClickedButonAdd();
	afx_msg void OnBnClickedButonExchange();
	afx_msg void OnBnClickedButonExport();
	afx_msg void OnBnClickedButonImport();
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnSetfocusEdit23();
	afx_msg void OnEnChangeEditComments();
	afx_msg void OnBnClickedOkbutton();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SP_RETRIEVE_H__2C1D1101_1179_11D2_AE4A_00C04F95A11E__INCLUDED_)
