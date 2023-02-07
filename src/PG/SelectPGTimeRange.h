#pragma once

#include "../CUG\CalcCug.h"


// SelectPGTimeRange dialog

class SelectPGTimeRange : public CDialog
{
	DECLARE_DYNAMIC(SelectPGTimeRange)

public:
	SelectPGTimeRange(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SelectPGTimeRange();
	void Init(void *);

// Dialog Data
	enum { IDD = IDD_DATESELECTION};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL m_Init;
	void *m_pPG;
	CalcCug m_GridDate;
private:

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedButtonReset();
};
