// SP_EXCHANGE1.cpp : implementation file
//

#include "..\stdafx.h"
#include "..\CoupModel.h"
#include "..\CoupModeldoc.h"
#include ".\SP_EXCHANGE1.h"

#ifdef _DEBUG
#define new DEBUG_NEW	   
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/*
extern "C" int  PLOTPF_mp_SCANDATABASE(int*);
extern "C" BOOL PLOTPF_mp_ISINDATABASE();
extern "C" void PLOTPF_mp_OPENBASE();

extern "C" BOOL PLOTPF_mp_COMSET;

extern "C" BOOL PLOTPF_mp_SUBPP;

extern "C" int PLOTPF_mp_ACTUALPROFILE;
extern "C" int PLOTPF_mp_ACTUALREPPROFILE;

extern "C" float PLOTPF_mp_PTEX[6];
extern "C" char  PLOTPF_mp_DKEY[6];
extern "C" char  PLOTPF_mp_RPLACE[16];
extern "C" char  PLOTPF_mp_COUNTY[6];
extern "C" char  PLOTPF_mp_CKOORD[16];

extern "C" char PLOTPF_mp_LOCALFILENAME[80];
extern "C" int PLOTPF_mp_NUMLOCALFILENAME;
extern "C" void  PLOTPF_mp_OPENLOCAL();
extern "C" void   PLOTPF_mp_CLOSELOCAL();
extern "C" void   PLOTPF_mp_CREATENEWLOCAL();
extern "C" int   PLOTPF_mp_SCANLOCAL(int *);
extern "C" int   PLOTPF_mp_MOVE_TO_MAIN();
extern "C" int   PLOTPF_mp_MOVE_ALL();
extern "C" int   PLOTPF_mp_MOVE_TO_LOCAL();
extern "C" char  PLOTPF_mp_LO_DKEY[6];
extern "C" char  PLOTPF_mp_LO_RPLACE[16];
extern "C" char  PLOTPF_mp_LO_COUNTY[6];
extern "C" char  PLOTPF_mp_MOVE_DKEY[6];
*/
/////////////////////////////////////////////////////////////////////////////
// SP_EXCHANGE1 dialog


SP_EXCHANGE1::SP_EXCHANGE1(CCoupModelDoc *pDoc, CWnd* pParent /*=nullptr*/)
	: CDialog(SP_EXCHANGE1::IDD, pParent)
{
	m_pDoc=pDoc;
//	PLOTPF_mp_OPENBASE();
	m_LocalFile=FALSE;

	//{{AFX_DATA_INIT(SP_EXCHANGE1)
	m_FileName = _T("");
	//}}AFX_DATA_INIT
}


void SP_EXCHANGE1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SP_EXCHANGE1)
	DDX_Control(pDX, IDC_LIST_MAIN, m_ListMain1);
	DDX_Control(pDX, IDC_LIST_LOCAL, m_ListLocal1);
	DDX_Text(pDX, IDC_EDIT1, m_FileName);
	DDV_MaxChars(pDX, m_FileName, 60);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SP_EXCHANGE1, CDialog)
	//{{AFX_MSG_MAP(SP_EXCHANGE1)
	ON_BN_CLICKED(IDC_BUTTON_FILE, OnButtonFile)
	ON_BN_CLICKED(IDC_MOVE_TO_LOCAL, OnMoveToLocal)
	ON_BN_CLICKED(IDC_MOVE_TO_MAIN, OnMoveToMain)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SP_EXCHANGE1 message handlers
void SP_EXCHANGE1::OnButtonFile() 
{
/*	CStdioFile File;
	CString pszFileName ;
	LPCTSTR lpszDefExt;
	LPCTSTR lpszFilter;
	int fileDlgResult;
	int newfile;
	lpszDefExt="dba";
	lpszFilter ="PLOTPF database Files (*.dba)|*.dba||";

 	CFileDialog	FileDlg(TRUE,lpszDefExt,pszFileName, OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,lpszFilter);


	fileDlgResult=FileDlg.DoModal();

	//If the cancel button has been pushed then return
	if (fileDlgResult==IDCANCEL)
		return;

	//Retrieve the filename of the file that is to be opened
	pszFileName = FileDlg.GetPathName();
	newfile=0;
	//If the file doesn´t exist
	if(m_LocalFile) PLOTPF_mp_CLOSELOCAL();
	if(!File.Open(pszFileName,  CFile ::modeRead  ))
		{
			TRACE("Can't Open file %s´\n", pszFileName); 
			//Display error message
			MessageBox("The file specified couldn´t be opened. A new local data base will be created","File Open Error",MB_OK|MB_ICONEXCLAMATION);
			newfile=1;
			// Actions without file name ?
		}
	if(newfile==0) File.Close();
	//Change the filename to uppercase
	pszFileName.MakeUpper();
	PLOTPF_mp_NUMLOCALFILENAME=pszFileName.GetLength()-4;
	m_FileName=pszFileName.Mid(0,PLOTPF_mp_NUMLOCALFILENAME);
	for (int i=0;i<PLOTPF_mp_NUMLOCALFILENAME;i++) 
		PLOTPF_mp_LOCALFILENAME[i]=pszFileName[i];
	if(newfile==1) PLOTPF_mp_CREATENEWLOCAL();
	PLOTPF_mp_OPENLOCAL();
	m_LocalFile=TRUE;
	OnInitDialog();
	*/
	
}

void SP_EXCHANGE1::OnMoveToLocal() 
{
 /*  int index[50];
   CString string;
   int max=50;
   int nsel;
   nsel=m_ListMain1.GetSelItems(max, &index[0]);
   for(int i=0;i<nsel;i++) {
		m_ListMain1.GetText(index[i],string);
		for(int j=0;j<6;j++) 
			PLOTPF_mp_MOVE_DKEY[j]=string[j];
		if(PLOTPF_mp_MOVE_TO_LOCAL()) {
			MessageBox("Profile : "+string.Mid(0,6)+"\n Moved to Local database","Moved Profile");
		    m_ListMain1.SetSel(index[i],FALSE);
		}

   }*/
   OnInitDialog();
	
}

void SP_EXCHANGE1::OnMoveToMain() 
{
 /*  int index[50];
   CString string;
   int max=50;
   int nsel;
   
//   PLOTPF_mp_MOVE_ALL();
   nsel=m_ListLocal1.GetSelItems(max, &index[0]);
   for(int i=0;i<nsel;i++) {
		m_ListLocal1.GetText(index[i],string);
		for(int j=0;j<6;j++) 
			PLOTPF_mp_MOVE_DKEY[j]=string[j];
		if(PLOTPF_mp_MOVE_TO_MAIN()) {
			MessageBox("Profile : "+string.Mid(0,6)+"\n Moved to Main database","Moved Profile");
		    m_ListLocal1.SetSel(index[i],FALSE);
		}
   }

   OnInitDialog();
   */
}

BOOL SP_EXCHANGE1::OnInitDialog() 
{
	CDialog::OnInitDialog();
   CString string;
	 int numTab=2, Record=1;
   LPINT tabstops;
   int	tabstop[2];
   tabstop[0]=90;
   tabstop[1]=100;
   tabstops=&tabstop[0];
/*
   m_ListMain1.SetTabStops(numTab,tabstops);
   m_ListMain1.ResetContent();
   while(PLOTPF_mp_SCANDATABASE(&Record)>1) {
		string=_T("");
		for(int i=0;i<6;i++)
			string+=PLOTPF_mp_DKEY[i];
		string+=" ";
		for(int i=0;i<16;i++) {
			string+=PLOTPF_mp_RPLACE[i];
		}
		string+="\t";
	  
		for(int i=0;i<6;i++)
			string+=PLOTPF_mp_COUNTY[i];
	  		if(string.Find("   :")!=0) {
			m_ListMain1.AddString(string);
		}
		Record++;	
   }

   if(m_LocalFile) {
		Record=1;
		m_ListLocal1.SetTabStops(numTab,tabstops);
		m_ListLocal1.ResetContent();
		while(PLOTPF_mp_SCANLOCAL(&Record)>1) {
			string=_T("");
			for(int i=0;i<6;i++)
				string+=PLOTPF_mp_LO_DKEY[i];
			string+=" ";
			for(int i=0;i<16;i++) {
				string+=PLOTPF_mp_LO_RPLACE[i];
			}
			string+="\t";
		  
			for(int i=0;i<6;i++)
				string+=PLOTPF_mp_LO_COUNTY[i];
		  	if(string.Find("   :")!=0) {
				m_ListLocal1.AddString(string);
			}
			Record++;	
		}
	}
	
	// TODO: Add extra initialization here
	*/
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SP_EXCHANGE1::OnOK() 
{
//	if(m_LocalFile) PLOTPF_mp_CLOSELOCAL();

	CDialog::OnOK();
}
void SP_EXCHANGE1::OnCancel() 
{
//	if(m_LocalFile) PLOTPF_mp_CLOSELOCAL();

	CDialog::OnCancel();
}


INT_PTR SP_EXCHANGE1::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DoModal();
}
