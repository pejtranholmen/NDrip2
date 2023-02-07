#pragma once
#include "../CoupModelDoc.h"
#include "../CUG\CreatePG\PGCug.h"
#include "../CUG\CalcCug.h"
#include "../resource.h"
#include "../ModelTypes\SimB.h"
#include "afxwin.h"
#include "PG.h"



// NewPGVariables dialog

class NewPGVariables : public CDialog
{
	DECLARE_DYNAMIC(NewPGVariables)

public:
	NewPGVariables(CPG *pX = nullptr,CWnd* pParent = nullptr);   // standard constructor
	virtual ~NewPGVariables();

// Dialog Data
	enum { IDD = IDD_ADDVARPG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	PGCug m_gridDes, m_gridDesNew;
	CalcCug m_CalcInfo;
	CPG *m_pOPG, *m_pPG_Created;
	Radiation_Functions m_RadFunc;
	virtual BOOL OnInitDialog();
	BOOL m_Init;
	int m_FuncTypeValue, m_FuncCategoryValue;
	int m_FuncChoiceValue;
	int	m_AddedVar;
	
	float m_initlat;
	float m_StoredValue;
	float VariableValueFunc(float *);
	double TimeFunc(double day, double hr);
	double Time_VarFunc(double day, double hr, float *);
public:
	CString m_NewFileName;
	CComboBox m_VarChoice;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	afx_msg void OnBnClickedSaveAs();
	afx_msg void OnBnClickedButtonadd();
	CComboBox m_FunctionType;
	afx_msg void OnCbnSelchangeFuncType();
	afx_msg void OnCbnSelchangeFunction();
	afx_msg void OnCbnSelchangeFuncCategory();
	CComboBox m_FuncCategory;
};
