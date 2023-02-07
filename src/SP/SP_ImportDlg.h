#pragma once
#include "..\CUG\PF_Import_CUG.h"
#include "afxwin.h"

// SP_ImportDlg dialog

class SP_ImportDlg : public CDialog
{
	DECLARE_DYNAMIC(SP_ImportDlg)

public:
	SP_ImportDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SP_ImportDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_PF_IMPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();	
	virtual BOOL OnInitDialog();
	PF_Import_Cug m_grid;
	BOOL m_InitDialog;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeImportNumber();

	// The number of lines
	unsigned int m_Rows;
	afx_msg void OnBnClickedImport();
};
