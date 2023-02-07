
#pragma once
#include ".\BasicExcel.hpp"
#include "afxwin.h"
#include "afxcmn.h"
using namespace YExcel;
// SimpleExcelImport dialog

class SimpleExcelImport : public CDialog
{
	DECLARE_DYNAMIC(SimpleExcelImport)

public:
	SimpleExcelImport(CCoupModelDoc* pDoc, NewMap *pfmap, CWnd* pParent = NULL);   // standard constructor
	virtual ~SimpleExcelImport();

// Dialog Data
	enum { IDD = IDD_DIALOG12 };

protected:
	CCoupModelDoc *m_pDoc;
	NewMap *m_pfmap;
	BasicExcel e;
	int NumSheets;
	bool m_ReRead, m_ApplyAll;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnApplySelected();
	afx_msg void OnBnReadFile();
	CComboBox m_WorkSheet;
	CListCtrl m_ParList;
	afx_msg void OnReadSelectedSheet();
	CString m_FileName;
	int nSwitches;
	int nParApplied;
	int nParTableApplied;
	bool m_FileRead,m_SheetRead;
	CEdit m_pSelectedFile;
	afx_msg void OnCbnSelchangeWorkSheet();
	CEdit m_pSwitches;
	CEdit m_pParameters;
	CEdit m_pParTables;
	afx_msg void OnLvnColumnclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemActivateList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnBegindragList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnApplyAllItems();
};
