#pragma once

#include "../CUG\CalcCug.h"


// CCalculateVariable dialog

class CCalculateVariable : public CDialog
{
	DECLARE_DYNAMIC(CCalculateVariable)

public:
	CCalculateVariable(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CCalculateVariable();
	void Init(void *);

// Dialog Data
	enum { IDD = IDD_DIALOG_CALCULATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL m_Init;
	void *m_pPG;
	CalcCug m_GridCalc;
	CalcCug m_GridDes;

private:

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedButtonReset();
};
