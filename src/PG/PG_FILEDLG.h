#pragma once
#include "../CUG\PGFileCug.h"

// CPG_FILEDLG dialog

class CPG_FILEDLG : public CDialog
{
	DECLARE_DYNAMIC(CPG_FILEDLG)

public:
	CPG_FILEDLG(void *pPG, enum pgfilemethod, BOOL interpolate=FALSE, BOOL textsource=FALSE, BOOL createdirect=FALSE,CWnd* pParent = nullptr);   // standard constructor
	virtual ~CPG_FILEDLG();
	virtual INT_PTR DoModal();
	virtual void OnOK();
	BOOL m_Created;
// Dialog Data
	enum { IDD = IDD_DIALOG_PG_FILES };

	void *m_pPG_out;
	CString m_FileName;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
	void *m_pPG_in;
	BOOL m_Mix, m_Init ;
	CString m_WindowText;
	PGFileCug m_GridPGFiles;
	BOOL m_Interpolate;
	BOOL m_MergeSimilar;
	BOOL m_Textsource;
	BOOL m_CreateDirect;
	CString m_MixFileName;
	enum pgfilemethod m_PGMethod;
public:
	afx_msg void OnBnClickedAdd();

	afx_msg void OnBnClickedCreate();
};
