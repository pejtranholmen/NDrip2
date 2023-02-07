#pragma once

#include "../CUG\HistoryCug.h"
// CHistoryList dialog
class SimB;
class Doc;

class CHistoryList : public CDialog
{
	DECLARE_DYNAMIC(CHistoryList)

public:
	CHistoryList(SimB *pBase, int itype, Doc *pDoc,int index=-1, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CHistoryList();

// Dialog Data
	enum { IDD = IDD_DIALOG_HISTORYVIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	HistoryCug m_gridHistory;
	bool m_Init;
	string m_WindowText;
	DECLARE_MESSAGE_MAP()
};
