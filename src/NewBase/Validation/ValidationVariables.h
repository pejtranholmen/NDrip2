// ValidationVariables.h : header file
//
#include "../ModelTypes\SimB.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "../CUG\CValidTimeCug.h"
/////////////////////////////////////////////////////////////////////////////
// CValidationVariables dialog

class CCoupModelDoc;

class CValidationVariables : public CDialog
{
// Construction
public:
	CValidationVariables(CCoupModelDoc* pDoc,int ItemSelected, CWnd* pParent = nullptr);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CValidationVariables)
	enum { IDD = IDD_ValidationTimeSerieVariables };

	int		m_SimVarList;
	string	m_SimHeading;
	string	m_ValHeading;
	BOOL	m_FileExist;
	string	m_FileName;
	string	m_NumberOfSelectedVariables;
	BOOL	m_NewLists;


	BOOL    m_makeApply;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CValidationVariables)
	public:
	virtual INT_PTR DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CCoupModelDoc* m_pDoc;
	F* m_pFile;
	int m_NumSelected;


	CValidTimeCug m_gridRealPGTime, m_gridPicker;
	CValidTimeCug m_gridRealPGTime_Sel, m_gridPicker_Sel;
	CString m_WindowText;

	BOOL m_Init;






	
	// Generated message map functions
	//{{AFX_MSG(CValidationVariables)

	afx_msg void OnApply();

	virtual void OnOK();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	float m_RelError, m_AbsError;
	bool m_Accumulate,m_LogTrans;
	string  m_SelGroup, m_SelName, m_ValDesc, m_SelType;
	int m_ItemSelected, m_ValVarList, m_SelIndex, m_FileNo, m_SelTypeInt;
	BOOL m_ReadySim, m_ReadyObs;




	int m_SelectedNum;
	afx_msg void OnBnClickedReadfromexcel();
	afx_msg void OnBnClickedSavetoexcel();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedAdd();
	
};
