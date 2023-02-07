#pragma once
#include "../CoupModelDoc.h"
#include "../CUG\CreatePG\PGCug.h"
#include "../resource.h"
#include "../ModelTypes\SimB.h"
#include "afxwin.h"
// CCreatePG dialog

class CCreatePG : public CDialog
{
	DECLARE_DYNAMIC(CCreatePG)

public:

	CCreatePG(CCoupModelDoc *pDoc, SimB *pF = nullptr, CPG *pPG = nullptr, CWnd* pParent = nullptr) ;   // standard constructor

	virtual ~CCreatePG();


// Dialog Data
	enum { IDD = IDD_CREATEPG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	PGCug m_gridDes, m_gridData;
	int m_ImportFormatValue;
	F *m_pCF;
	void *m_pOPG, *m_pPG_Created;

	void *m_pGrid;
	BOOL m_CleanGrid;
	BOOL m_MissingRemoveValue;
	string m_EquiDistanceText, m_MissingText, m_IncreaseTextMessage;
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
public:
	void ReadPG(void* pPG);   // extra constructor
	void SetpGrid(void *);
	afx_msg void OnEnChangeStartDate();
	afx_msg void OnBnClickedMakeRegularTimeIncrease();
	afx_msg void OnBnClickedReadTextFile();
	size_t m_NumVariables;
	size_t m_NumRecords;
	size_t m_NormalTimeInt;
	size_t m_Repit;
	size_t m_ActBlock, m_NumBlocks, m_BlockSize;
	string m_StartDateString;
	string m_PG_FileName;
	string m_EndDateString, m_OutputStartDateString;
	string m_OutputEndDateString;
	BOOL m_Init;
	BOOL m_Saved;
	BOOL m_RestrictedSave;

	bool RemoveMissingSave();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnBnClickedSave();
	afx_msg void OnEnChangeEdit3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedCleanGrid();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedRereadpg();
	afx_msg void OnEnKillfocusEdit2();
	afx_msg void OnBnClickedButtonNewvar();
	afx_msg void OnBnClickedButtonAppend();
	afx_msg void OnBnClickedButtonNewdata();
	afx_msg void OnEnChangePeriodOut();
	afx_msg void OnEnChangeNormalTimeInt();
	afx_msg void OnBnClickedCheck1();
	CButton m_MissingRemove;
	afx_msg void OnBnClickedButtonAdd();
	CComboBox m_ImportFormat;
	afx_msg void OnCbnSelchangeCombodisplaysize5();
	afx_msg void OnBnClickedButtonUpdategrid();
	afx_msg void OnBnClickedMakerepstyle();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnChangeActblock();
	afx_msg void OnEnChangeBlocksize();
	afx_msg void OnEnKillfocusBlocksize();
	afx_msg void OnEnHscrollActblock();
	afx_msg void OnBnClickedIncreaseblock();
	afx_msg void OnBnClickedDecreaseblock();
	afx_msg void OnBnClickedUpdategrid();
	afx_msg void OnBnClickedMakerepstyle2();
	afx_msg void OnBnClickedMemoryreadbut();
	afx_msg void OnBnClickedMemory();
};
