#pragma once
//#include "afxcmn.h"
//#include "afxwin.h"
#include "../Resource.h"


// CFileScanner dialog

class CFileScanner : public CDialog
{
	DECLARE_DYNAMIC(CFileScanner)

public:
	CFileScanner(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CFileScanner();
	void Update(size_t procent);
	void SetLocalText(CString txt);

// Dialog Data
	enum { IDD = IDD_PROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	CProgressCtrl m_prog;
	CStatic m_ProgText;
	CString m_Info;
	CStatic m_ProgText2;
};
