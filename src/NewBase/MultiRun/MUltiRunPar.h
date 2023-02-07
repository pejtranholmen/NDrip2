#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "PP_Edit.h"


// MultiRunPar dialog

class MultiRunPar : public CDialog
{
	DECLARE_DYNAMIC(MultiRunPar);
	CCoupModelDoc* m_pDoc;
	SimB* m_pPs;
	float m_ErrMin;
	float m_ErrMax;
	float m_MinValue,m_MaxValue,m_StartValue,m_StepSize;
	size_t m_TableIndexValue, m_TableIndexValue2;

	size_t m_TabIndex;

	int m_Selected_Dim;
	int m_ParDependentIndex;
	BOOL  m_IsTableSet,m_IsTable, m_IsDatabase;
	NewMap* mp_fmap;
	int numrow, numrow2;
	SimB	*m_pDepPar;
	int		m_DepIndex;	
	BOOL m_ShiftMode;

public:
	MultiRunPar(CCoupModelDoc* pDoc, NewMap *p_fmap, BOOL TableType, CWnd* pParent = NULL);   // standard constructor
	virtual ~MultiRunPar();




// Dialog Data
	enum { IDD = IDD_MULTI_RUN_PAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	void OnCreateParTable();
	void OnCreateList();
	void OnCreateDimList();
	void OnCreateMethodList();
	void OnParNew();
	void OnCDBNew();
	void OnParTableNew();
	void OnUpdateStatus();
	void OnUpdateButtons();
	void OnCreateDependenceList();
	
	afx_msg void OnButtonChange();
	afx_msg void OnButtonDeleteDim();
	afx_msg void OnButtonNewdim();
	afx_msg void OnBUTTONCounter();
	afx_msg void OnButtonApply();
	afx_msg void OnDefineFile();
	afx_msg void OnTableValues();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox m_TableIndex;
	
	afx_msg void OnCbnSelchangeCombo2();
	CListCtrl m_ParList;
	afx_msg void OnLvnDeleteitemList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnBeginlabeleditList1(NMHDR *pNMHDR, LRESULT *pResult);
	CString m_ParMinValue;
	CString m_ParMaxValue;
	enum MR_METHOD m_ParMethod;
	CListBox m_Dimensions;
	int m_DimensionSelected;
	afx_msg void OnBnClickedButtonChangeRepetions();
	afx_msg void OnBnClickedButtonCounter();
	afx_msg void OnBnClickedButtonNewdim();
	afx_msg void OnBnClickedButtonDeleteDim();
	afx_msg void OnBnClickedButtoApply();
	CString m_ParName;
	CString m_ParGroup;
	CString m_Type;
	afx_msg void OnEnChangeEdit3();
	CString m_TableName;
	CComboBox m_MethodList;
	CString m_TotalNumberOfRuns;
	afx_msg void OnLvnColumnclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	CButton ctrl_TableValues;
	afx_msg void OnBnClickedTableValues();
	afx_msg void OnCbnSelchangeMethod();
	afx_msg void OnBnClickedButtonTable();
	afx_msg void OnBnClickedButtonNotable();
	afx_msg void OnBnClickedStartValues();
	CComboBox m_ParDependence;
	afx_msg void OnCbnSelchangeParDependence();
	afx_msg void OnLbnSelchangeDimensions();
	afx_msg void OnBnClickedApplybayesianparameters();
	afx_msg void OnExportToExcel();
	afx_msg void OnImportFromExcel();
	afx_msg void OnBnClickedDataBaseMode();
	afx_msg void OnAddRecord();
};
