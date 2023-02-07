#if !defined(AFX_PP_EDIT_H__02A873F3_23BC_11D2_AE60_00C04F95A11E__INCLUDED_)
#define AFX_PP_EDIT_H__02A873F3_23BC_11D2_AE60_00C04F95A11E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include  "../modelTypes\SimB.h"
//#include "afxwin.h"
#include "../CUG\PFCUG\pfCug.h"
#include "../CUG\DBCug.h"
#include "../Resource.h"
class Doc;
// PP_EDIT.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// PP_EDIT dialog

class PP_EDIT : public CDialog
{
// Construction
friend class DB_SELECT;
friend class CChaParDuringRun;
friend class MultiRunDB;
friend class MultiRunPar;
friend class MultiRunTable;
friend class MyCug;
friend class PFCug;
friend class DBCug;
friend class PFCommands;
friend class PFSheets;
friend class SoilData;
public:
	DECLARE_SERIAL(PP_EDIT)
	PP_EDIT() : m_SelectedKeyDisplay(_T(""))
	{
	
	};
	PP_EDIT(CString str,Doc *pDoc,CDB *pDB=NULL, BOOL readonly=FALSE, CWnd* pParent = NULL);   // standard constructor
	PP_EDIT(CDB *pDB,Doc *pDoc, int nrow, CWnd* pParent = NULL);  
	PP_EDIT(CString str,CString key, Doc *pDoc, CDB *pDB=NULL, CWnd* pParent = NULL);   // standard constructor
	~PP_EDIT();
//	PP_EDIT(BOOL no_ini,CCoupModelDoc *pDoc, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(PP_EDIT)
	enum { IDD = IDD_DIALOG_PP_EDIT };
	CComboBox	m_TimeResolutionControl;
	CComboBox	m_GenSwitchControl3;
	CComboBox	m_GenSwitchControl2;
	CComboBox	m_GenSwitchControl1;
	CEdit	m_Details;
	CComboBox	m_PlantType;
	CListBox	m_PlantList;
 
	CString	m_PlantName;
	CString	m_Country;
	CString	m_Longitude;
	CString	m_Latitude;
	CString m_strDetails;
	int		m_intPlantType;
	int		m_NumOriginalRecords;
	int		m_NumAbove;
	int		m_NumBelow;

	CString	m_SiteName;
	BOOL	m_DriveFileExist;
	CString	m_DriveFile;
	BOOL	m_DriveFile_Exist;
	CString	m_NameText;
	CString	m_TypeText;
	CString	m_NameText2;
	CString	m_TypeText2;
	CString	m_GenSwitch1;
	CString	m_GenSwitch2;
	CString	m_GenSwitch3;
	CString	m_SwTimeResolution;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PP_EDIT)
	public:
	virtual void Serialize(CArchive& ar);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void DeleteKeyFile();
	void SaveWithKey();
	void MoveSystemKey(int irec, BOOL system);
	void CopyKey(int irec);
	void DeleteKey(int irec);
	int m_SelectedIndex;

	Doc *m_pDoc;


protected:
	void UpdateStrings();
	void SetGroupConnections();
	void Save();
	void Actual_DB();
	void DB_Actual();
	void DeleteFile();
	void Retrieve();
	void Reset();

	CDB *m_pCDB;

	BOOL m_InitDialog;
	BOOL FileExist();
	BOOL m_ReadOnly;
	BOOL m_TotalReadOnly;
	BOOL m_Reset;
	BOOL m_SavedDataInRecord;
	BOOL m_SysStore;
	BOOL m_RetrieveOne;
	CString m_ID_DB_Group;
	CString m_SelectedId;
	CString m_SelectedKey;
	CStringArray m_arrGroups;

	Sw *m_pSwLAI, *m_pSwRoot, *m_pSwGen1, *m_pSwGen2, *m_pSwTimeRes;
	Sw *m_pSw_NC;
	F *m_pFile;



	CUIntArray m_arrPlantType;
	CStringArray m_PlantTypes;
	CStringArray m_arrPlantName, m_arrSiteName, m_arrDetails;
	CStringArray m_arrLatitude,m_arrLongitude,m_arrCountry;
	CStringArray m_arrIdUser, m_arrDriveFile,m_arrIdKey;
	CUIntArray	 m_arrIdNum;
	int m_ArrayIndex;
	CString SelectedKey;
	CFile m_file, m_SysFile;
	CString m_SimFileName;
	int m_NumToSelect;

	// Generated message map functions
	//{{AFX_MSG(PP_EDIT)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnButtonApply();
	afx_msg void OnButtonDelete();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonCreate();
	afx_msg void OnBrowse();
	afx_msg void OnFileView();
	afx_msg void OnReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CString m_SelectedKeyDisplay;
public:
	PFCug m_DatabasCUG;
	PFCug m_DataBaseSelected;
	DBCug m_Data;
	afx_msg void OnBnClickedResetSelection();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PP_EDIT_H__02A873F3_23BC_11D2_AE60_00C04F95A11E__INCLUDED_)
