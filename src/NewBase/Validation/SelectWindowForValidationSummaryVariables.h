#pragma once
#include "afxwin.h"
#include "../CoupModelDoc.h"
#include "../CUG\SumValidationCug.h"

// SelectWindowForValidationSummaryVariables dialog

class SelectWindowForValidationSummaryVariables : public CDialog
{
	DECLARE_DYNAMIC(SelectWindowForValidationSummaryVariables)

public:
	SelectWindowForValidationSummaryVariables(CCoupModelDoc* pDoc, CWnd* pParent = nullptr);   // standard constructor
	virtual ~SelectWindowForValidationSummaryVariables();
	virtual INT_PTR DoModal();
	virtual void OnOK();

	int		m_SimVarList;
	CString	m_SimHeading;
	CString	m_NumberOfSelectedVariables;
	BOOL	m_NewLists;
	bool	m_Init;
	bool	m_VariableFromFile;
	float   m_ObsValue, m_RelError, m_AbsError;

// Dialog Data
	enum { IDD = IDD_ValidationSummaryVariables };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	CCoupModelDoc* m_pDoc;
	SumValidationCug m_SelectedSumList;
	int m_NumSelected;
	size_t m_MultiSumPeriod_Start;
	size_t m_MultiSumPeriod_End;
	CString m_Str_MultiSumPeriod_Start;
	CString m_Str_MultiSumPeriod_End;
	CListCtrl m_ValidationListSel;
	CString m_SimVariableSel;
	CString m_ObsValidationSel;
	CString m_MultiSim_Start;
	void OnCreateSelList();
	void MakeSimList();
	int m_ItemSelected;
	int m_ItemSelectedShow;
	int m_TypeVal;
	int m_LinkVal;
	int m_LinkKeyInFile;
	int m_LinkKeyInMultiSimList;
	SIMSUMSOURCE m_DependentVarTypeSelected;
	size_t m_LinkFileVal;
	string m_LinkName;
	string m_LinkFileName;
	string m_LinkKeyInFileString;
	BOOL m_DeleteEnable;
	BOOL m_ChangeEnable;
	// Generated message map functions
	//{{AFX_MSG(SelectWindowForValidationSummaryVariables)
	afx_msg void OnApply();
	afx_msg void OnApplyAllId();
	
	afx_msg void OnReset();
	afx_msg void OnDisable();
		//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedDeleteItem();
	afx_msg void OnHdnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnItemclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLbnSelchangeValidationvarlist();
	afx_msg void OnLbnSelchangeSimulatedvarlist2();

	afx_msg void OnEnChangeEdit3();
	afx_msg void OnEnKillfocusEdit4();
	afx_msg void OnEnKillfocusEdit5();
	afx_msg void OnEnKillfocusCSVFile();
	afx_msg void OnEnKillfocusKeyInFile();
	afx_msg void OnEnKillfocusKeyInMultiSimList();
	afx_msg void OnEnUpdateEdit4();
	afx_msg void OnKillFocusDependentType();
	afx_msg void OnKillFocusChangeDate();
	int m_SelectedNum;


public:
	// Shows which variables that are to be used for validation
	CComboBox m_SumTypeVariable;
	CComboBox m_LinkNameVariable;
	CComboBox m_LinkFileNameBox;
	CComboBox m_LinkFileKeyId;
	CComboBox m_LinkSimMultiListKey;
	CComboBox m_DependentVarType;
	afx_msg void OnBnClickedSavetoexcel();
	afx_msg void OnBnClickedReadfromexcel();
};
