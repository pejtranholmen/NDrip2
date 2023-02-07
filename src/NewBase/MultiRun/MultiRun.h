#pragma once



class Doc;




// _MSC_VER >= 1000
// MultiRun.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// MultiRun dialog

class MultiRun : public CDialog
{
// Construction
public:
 	// standard P constructor
	MultiRun(SIMB value, Doc* pDoc, CWnd* pParent = nullptr);

// Dialog Data
	//{{AFX_DATA(MultiRun)
	enum { IDD = IDD_MULTI_RUN };
	CEdit	m_ctrlMax;
	CEdit	m_ctrlMin;
	CComboBox	m_Num_Method;
	int		m_Selected_Dim;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MultiRun)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	Doc* m_pDoc;
	Ps* m_pPs;

	CDB *m_pDB;
	P*  m_pP;
	SIMB m_Par;

	double m_ErrMin;
	double m_ErrMax;
	double m_MinValue,m_MaxValue, m_StartValue;
	int m_TabIndex;
	BOOL m_IsLanFile, m_IsTableSet;

	// Generated message map functions
	//{{AFX_MSG(MultiRun)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnButtonChange();
	afx_msg void OnButtonDeleteDim();
	afx_msg void OnButtonNewdim();
	afx_msg void OnBUTTONCounter();
	afx_msg void OnButtonApply();
	afx_msg void OnDefineFile();
	afx_msg void OnTableValues();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ctrlStart;
};


