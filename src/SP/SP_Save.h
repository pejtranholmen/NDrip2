
#pragma once
//#include "SP_Retrieve.h"
#include "..\SoilData\PlotPF\PFCurve.h"
// Added by ClassView
// SP_Save.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SP_Save dialog
//typedef CTypedPtrArray<CPtrArray,float*> CFloatArray;


struct NEWSOIL {
	float clay;
	float sand;
	float saturated;
	float upperdepth;
	float lowerdepth;
};
class CCoupModelDoc;
class Doc;

class SP_Save : public CDialog
{
	friend class SP_InitChar;
// Construction
public:
	SP_Save( BOOL newprofile,CCoupModelDoc *pDoc, CWnd* pParent = NULL);   // standard constructor
	SP_Save(CCoupModelDoc *pDoc, int modlayer, CWnd* pParent = NULL);   // standard constructor
//Destructor
	~SP_Save();

// Dialog Data
	//{{AFX_DATA(SP_Save)
	enum { IDD = IDD_DIALOG_Save };
	CListBox	m_SoilChar3;
	CEdit	m_PFcom;
	CListBox	m_Selection;
	CListBox	m_Details;
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
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SP_Save)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_IniProf, m_IniRepProf;
	//CFloatArray m_arrClay, m_arrSand, m_arrSaturation;
	//CFloatArray m_arrUpperDepth,m_arrLowerDepth;
	vector<NEWSOIL> m_NewSoil;
	BOOL m_newprofile;
	BOOL SaveAllLayers;
	CCoupModelDoc *m_pDoc;
	PFCurve *m_pPFCurve;
	NEWHEADER m_ActHeader;
	Doc *m_pSimDoc;

	void Import();
	void IniDataBase();
	// Generated message map functions
	//{{AFX_MSG(SP_Save)
	virtual void OnOK();
	afx_msg void OnButtonScan();
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnEdit();
	afx_msg void OnReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
