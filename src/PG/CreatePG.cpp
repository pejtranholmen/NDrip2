// CreatePG.cpp : implementation file
//

#include "../stdafx.h"	
#include "../std.h"
#include "../Util.h"

#ifndef PGONLY
#include "../CoupModel.h"
#endif
#include "./FileImport/PGFileImport.h"
#include "./createpg.h"
#include "../CUG\SimDoc\MyCug.h"
#include "./NoaaCoord.h"


// CCreatePG dialog

IMPLEMENT_DYNAMIC(CCreatePG, CDialog)
CCreatePG::CCreatePG(CCoupModelDoc *pDoc,SimB *Sw, CPG *pPG, CWnd* pParent /*=nullptr*/)
	: CDialog(CCreatePG::IDD, pParent)
	, m_NumVariables(1)
	, m_NumRecords(2)
{

	AfxEnableControlContainer();
	m_pCF=(F*)Sw;
	m_CleanGrid=TRUE;
	m_NormalTimeInt=1440;
	m_StartDateString="2006-01-01 12:00";
	m_OutputStartDateString="2006-01-01 12:00";
	m_IncreaseTextMessage="";
	m_Init=TRUE;
	m_pOPG=nullptr;
	m_Saved=FALSE;
	m_pGrid=nullptr;
	m_pPG_Created=nullptr;
	m_RestrictedSave=FALSE;
	m_MissingRemoveValue=FALSE;
	m_ImportFormatValue=0;
	m_Repit=1;
	if(Sw==nullptr&&pPG==nullptr) {
		CPG *pCPG;
		pCPG = new CPG();
		m_pPG_Created=pCPG;
		//pCPG->SaveAs(pCPG->GetFileName());
		pCPG->SetCompleteRead(TRUE);
		pCPG->CreateNewFile(m_NumVariables,m_NumRecords);

		//pCPG->SaveAs(pCPG->GetFileName(), TRUE);
		m_CleanGrid=FALSE;

		m_gridData.SetpDlg(this);

		m_gridData.SetpCUG(&m_gridDes);
		m_gridDes.SetpCUG(&m_gridData);


		
		//m_gridData.OnWriteGridToFile(pCPG);
		//m_gridDes.OnWriteGridToFile(pCPG);

		pCPG->SetDate(1,m_StartDateString);
		size_t t=pCPG->GetLongTime(1);
		pCPG->SetVarValue(1,1,2.);
		for(size_t i=1;i<m_NumRecords;i++) {
			t+=m_NormalTimeInt;
			pCPG->SetLongTime(i+1, t);
			pCPG->SetVarValue(1,i+1,float(i*5));
		}

		m_OutputEndDateString=pCPG->GetDate(m_NumRecords);
		m_EndDateString=pCPG->GetDate(m_NumRecords);
		pCPG->SetNormalTimeInterval(m_NormalTimeInt);
		m_gridDes.OnUpdate(6,m_NumVariables,FALSE);


		if(pCPG->GetNumBuffers()<=1)
			m_gridData.OnUpdate(m_NumVariables+1,pCPG->GetNumRecords() ,TRUE, FALSE, m_Repit);
		else
			m_gridData.OnUpdate(m_NumVariables+1,pCPG->GetCurrentRecords() ,TRUE, FALSE, m_Repit);




		m_gridData.OnUpdate(m_NumVariables+1, m_NumRecords,TRUE);

		//m_gridData.SetpPG(pCPG);
		//m_gridDes.SetpPG(pCPG);

		string UserDir;
		
		UserDir=MFC_Util::GetProfileStringStd("UserDirectory", UserDir);



		m_PG_FileName=UserDir+pCPG->GetFileName()+".bin";
		int ifile=1;
		while(FUtil::IsFileExisting(string(m_PG_FileName))) {				
			m_PG_FileName=UserDir+pCPG->GetFileName()+"_";
			m_PG_FileName+=FUtil::STD_ItoAscii(ifile);
			m_PG_FileName+=".bin";
			ifile++;
		}
		pCPG->SetNewFileName(string(m_PG_FileName));
		m_ActBlock=pCPG->GetPrevBuffer();
		m_NumBlocks=pCPG->GetNumBuffers();
		m_BlockSize=pCPG->GetNumRecordsBufSize();
		pCPG->SetActBufferSize(m_NumRecords);




		m_pOPG=pCPG;
	}
	else {

		m_gridData.SetpDlg(this);
		m_gridData.SetpCUG(&m_gridDes);
		m_gridDes.SetpCUG(&m_gridData);

		m_PG_FileName=pPG->GetFileName().c_str();
		m_pOPG=pPG;
		m_NumRecords=pPG->GetNumRecords();
		m_NumVariables=pPG->GetNumVariables();
		m_Repit=pPG->GetNumRepititions();
		if(pPG->GetCompleteRead()) {
			pPG->SetActBufferSize(m_NumRecords);
			m_ActBlock=pPG->GetPrevBuffer();
			if(m_ActBlock==0) m_ActBlock=1;
			m_BlockSize=pPG->GetNumRecordsBufSize();
			m_NumBlocks=pPG->GetNumBuffers();

		} 
		else {
			m_ActBlock=pPG->GetPrevBuffer();
			if(m_ActBlock==0) m_ActBlock=1;
			m_NumBlocks=pPG->GetNumBuffers();
			m_BlockSize=pPG->GetNumRecordsBufSize();

		}
		
		m_StartDateString=pPG->GetDate(1).c_str();
		m_StartDateString=pPG->GetDate(1).c_str();

		m_OutputStartDateString=pPG->GetDate(1).c_str();
		m_OutputEndDateString=pPG->GetDate(m_NumRecords).c_str();
		m_EndDateString=pPG->GetDate(m_NumRecords).c_str();

	}

}
void CCreatePG::ReadPG(void* pPG)
{
	CString str;
	if(m_pPG_Created!=nullptr&&m_pPG_Created!=pPG) {
		delete (CPG*)m_pPG_Created;
		m_pPG_Created=nullptr;
	}
		CPG *pCPG=(CPG*)pPG;	
		m_pOPG=pPG;
   			m_NumVariables=pCPG->GetNumVariables();
			m_Repit=pCPG->GetNumRepititions();
			if(m_Repit>1)
					pCPG->SetRepititionMode(true, m_Repit);
			else
				m_Repit=1;
			m_NumRecords=pCPG->GetNumRecords();
			m_NormalTimeInt=pCPG->GetNormalTimeInterval();
			str=MFC_Util::ItoAscii(m_NumVariables);
			GetDlgItem(IDC_EDIT1)->SetWindowText(str);
			//GetDlgItem(IDC_EDIT1)->RedrawWindow();
			m_NumRecords=pCPG->GetNumRecords();
			str=MFC_Util::ItoAscii(m_NumRecords);
			GetDlgItem(IDC_EDIT2)->SetWindowText(str);
			m_gridData.SetpPG(pCPG);
			m_gridDes.SetpPG(pCPG);
			m_gridDes.OnUpdate(6,m_NumVariables,FALSE, FALSE, m_Repit);
			m_gridDes.SetpCUG(&m_gridData);
			m_gridData.OnUpdate(m_NumVariables+1, m_NumRecords,TRUE, FALSE, m_Repit);
			m_gridData.SetpCUG(&m_gridDes);
			
			m_gridDes.OnReadFileDescriptionToGrid(pCPG);
			m_gridData.OnReadFileDataToGrid(pCPG);
			m_CleanGrid=FALSE;
			m_PG_FileName=pCPG->GetFileName().c_str();
			str=m_PG_FileName.c_str();
			
			pCPG->CheckEquiDistance();
			if(pCPG->IsEquiDistance())
				m_EquiDistanceText="Regular equal size time intervals";
			else
				m_EquiDistanceText="Non Regular time intervals";
			
			GetDlgItem(IDC_REGULAR)->SetWindowText(CString(m_EquiDistanceText.c_str()));
			GetDlgItem(IDC_REGULAR)->RedrawWindow();

			if(pCPG->CheckMissingNumbers()) {
				int in=pCPG->GetNumberofVariablesWithMissings();
				size_t im=pCPG->GetNumberofMissing();
				m_MissingText=MFC_Util::ItoAscii(im)+" missing values in "+ MFC_Util::ItoAscii(in)+" variables";
			}
			else
				m_MissingText="No missing values";

			
			GetDlgItem(IDC_MISSING)->SetWindowText(CString(m_MissingText.c_str()));
			GetDlgItem(IDC_MISSING)->RedrawWindow();

			GetDlgItem(IDC_EDIT3)->SetWindowText(str);
			GetDlgItem(IDC_EDIT3)->RedrawWindow();
			GetDlgItem(IDC_EDIT5)->SetWindowText(MFC_Util::ItoAscii(m_NormalTimeInt));
			GetDlgItem(IDC_EDIT5)->RedrawWindow();

		
			//GetDlgItem(IDC_EDIT2)->RedrawWindow();
			MFC_Util::MessageBox("PG Bin File Read");
			m_EndDateString=pCPG->GetDate(m_NumRecords).c_str();
			m_OutputEndDateString=pCPG->GetDate(m_NumRecords).c_str();
			m_StartDateString=pCPG->GetDate(1).c_str();
			m_StartDateString=pCPG->GetDate(1).c_str();			
			m_OutputStartDateString=pCPG->GetDate(1).c_str();

	OnInitDialog();
}

CCreatePG::~CCreatePG()
{
	if(m_pPG_Created!=nullptr) {
		delete (CPG*)m_pPG_Created;




	}
}

void CCreatePG::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_NumVariables);
	DDX_Text(pDX, IDC_EDIT_REPIT, m_Repit);

	DDX_Text(pDX, IDC_ACTBLOCK, m_ActBlock);
	DDX_Text(pDX, IDC_NUMBLOCK, m_NumBlocks);
	//DDX_Text(pDX, IDC_BLOCKSIZE, m_BlockSize);

	//DDV_MinMaxUInt(pDX, m_NumVariables, 0, 1000);
	DDX_Text(pDX, IDC_EDIT2, m_NumRecords);
	//DDV_MinMaxLong(pDX, m_NumRecords, 0, 100000);
	//DDX_Text(pDX, IDC_EDIT4, CString(m_StartDateString.c_str()));
	DDX_Text(pDX, IDC_EDIT5, m_NormalTimeInt);
	
	DDX_Text(pDX, IDC_EDIT_NEW_START, CString(m_OutputStartDateString.c_str()));
	DDX_Text(pDX, IDC_EDIT_NEW_END, CString(m_OutputEndDateString.c_str()));
	DDX_Control(pDX, IDC_CHECK1, m_MissingRemove);

	DDX_Control(pDX, IDC_COMBODISPLAYSIZE5, m_ImportFormat);
}

BOOL CCreatePG::OnInitDialog() {
	
	CString Str;
	GetWindowText(Str);
	if(m_pCF!=nullptr) {
		Str+="; ";
		Str+=m_pCF->GetGroup().c_str();
		Str+=" : ";
		Str+=m_pCF->GetName().c_str();
		SetWindowText(Str);
	}
	if(m_pOPG!=nullptr) {
		SetWindowText(_T("Edit PG Bin File"));



	}




	CDialog::OnInitDialog();

	m_MissingRemove.SetCheck(m_MissingRemoveValue);
	m_MissingRemove.RedrawWindow();

//
	//GetDlgItem(IDC_BUTTON2)->ModifyStyle(0,WS_DISABLED);
	//GetDlgItem(IDC_BUTTON2)->RedrawWindow();
	

	if(m_pOPG!=nullptr) {
		GetDlgItem(IDC_REREADPG)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_REREADPG)->RedrawWindow();
	}
	else {

		GetDlgItem(IDC_REREADPG)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_REREADPG)->RedrawWindow();
	}

	CPG *pCPG;
	if(m_Init) {

		m_gridDes.AttachGrid(this, IDC_GRID);
		m_gridData.AttachGrid(this, IDC_GRID2);

		//OnEnChangeEdit2();
		pCPG=(CPG*)m_pOPG;

		m_gridData.SetpPG(pCPG);
		m_gridDes.SetpPG(pCPG);




		
		m_gridDes.OnUpdate(6,m_NumVariables,FALSE, FALSE, m_Repit);
		m_gridDes.OnReadFileDescriptionToGrid(pCPG);
		
		if(!pCPG->IsGood())	pCPG->ClearFile();
		if(pCPG->GetNumBuffers()>1){
			pCPG->SetActBuffer(m_ActBlock);
			GetDlgItem(IDC_BLOCKSIZE)->SetWindowText(MFC_Util::ItoAscii(m_NumRecords));
			GetDlgItem(IDC_BLOCKSIZE)->RedrawWindow();

		}
		else {
			m_ActBlock=1;
			pCPG->SetActBufferSize(m_NumRecords);
			GetDlgItem(IDC_ACTBLOCK)->ModifyStyle(0,WS_DISABLED);
			GetDlgItem(IDC_NUMBLOCK)->ModifyStyle(0,WS_DISABLED);
			GetDlgItem(IDC_ACTBLOCK)->SetWindowText(MFC_Util::ItoAscii(m_ActBlock));

			GetDlgItem(IDC_BLOCKSIZE)->SetWindowText(MFC_Util::ItoAscii(m_NumRecords));
			GetDlgItem(IDC_ACTBLOCK)->RedrawWindow();
			GetDlgItem(IDC_NUMBLOCK)->RedrawWindow();
			GetDlgItem(IDC_BLOCKSIZE)->RedrawWindow();
		}
		m_gridData.OnSetup();

		m_gridData.OnUpdate(m_NumVariables+1, m_NumRecords,TRUE, FALSE, m_Repit);
		m_gridData.OnReadFileDataToGrid(pCPG);

		m_PG_FileName=pCPG->GetFileName();
		GetDlgItem(IDC_EDIT3)->SetWindowText(CString(m_PG_FileName.c_str()));
		SetWindowText(_T("Edit PG File: ")+CString(m_PG_FileName.c_str()));

		GetDlgItem(IDC_EDIT3)->RedrawWindow();


		//m_gridData.OnWriteGridToFile(pCPG);
		//m_gridDes.OnWriteGridToFile(pCPG);
		//OnBnClickedRereadpg();
		m_Init=FALSE;


		m_ImportFormat.Clear();
		m_ImportFormat.ResetContent();
		m_ImportFormat.AddString(_T("Standard Format"));
		m_ImportFormat.AddString(_T("Year,Month,Day,Hr,Min as separate variables"));
		m_ImportFormat.AddString(_T("Year,Month,Day as separate variables"));
		m_ImportFormat.AddString(_T("date(YYYY-MM-DD) and time (HH:MM) as separate variables"));
		m_ImportFormat.AddString(_T("Year, Month, Day, Hr as separate variables"));
		m_ImportFormat.AddString(_T("Years as Columns and Lines as Repetitions"));
		m_ImportFormat.AddString(_T("Months as Columns and Lines as Years"));
		m_ImportFormat.AddString(_T("ProFound SQLite Soil"));
		m_ImportFormat.SetCurSel(m_ImportFormatValue);

	}
	else {
		pCPG=(CPG*)m_pOPG;

	}


	if(pCPG->GetCompleteRead()) {
	  GetDlgItem(IDC_MEMORY)->ModifyStyle(WS_DISABLED,0);
	  GetDlgItem(IDC_MEMREADCOMPLETE)->SetWindowText(_T("Complete Read/Write"));	
	}
	else {
	   GetDlgItem(IDC_MEMREADCOMPLETE)->SetWindowText(_T("Block Read/Write"));	
	   GetDlgItem(IDC_MEMORY)->ModifyStyle(0,WS_DISABLED);	
	}
	GetDlgItem(IDC_MEMORY)->RedrawWindow();
	GetDlgItem(IDC_MEMREADCOMPLETE)->RedrawWindow();
	if(!pCPG->IsTimeIncreasing()) {
		m_IncreaseTextMessage="Time has "+FUtil::STD_ItoAscii(pCPG->GetJumpBacksNum())+ " backwards jump";
		if(pCPG->GetJumpBacksNum()>1) m_IncreaseTextMessage+='s';
		GetDlgItem(IDC_MAKEREPSTYLE)->ModifyStyle(WS_DISABLED,0);
	}
	else {
		GetDlgItem(IDC_MAKEREPSTYLE)->ModifyStyle(0,WS_DISABLED);
		m_IncreaseTextMessage="Time is increasing";
	}
	pCPG->SetActBuffer(m_ActBlock);
	GetDlgItem(IDC_MAKEREPSTYLE)->RedrawWindow();

	GetDlgItem(IDC_INCREASE)->SetWindowText(CString(m_IncreaseTextMessage.c_str()));
	GetDlgItem(IDC_INCREASE)->RedrawWindow();


	GetDlgItem(IDC_EDIT4)->SetWindowText(CString(m_StartDateString.c_str()));
	GetDlgItem(IDC_EDIT4)->RedrawWindow();
	GetDlgItem(IDC_EDIT6)->SetWindowText(CString(m_EndDateString.c_str()));
	GetDlgItem(IDC_EDIT6)->RedrawWindow();


	return TRUE;
}

BEGIN_MESSAGE_MAP(CCreatePG, CDialog)
	ON_BN_CLICKED(IDC_BUTTONREGEN, OnBnClickedMakeRegularTimeIncrease)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedReadTextFile)
	//ON_EN_CHANGE(IDC_EDIT2, OnEnChangeEdit2)
//	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedSave)
	ON_EN_CHANGE(IDC_EDIT3, OnEnChangeEdit3)
	ON_EN_CHANGE(IDC_EDIT_REPIT, OnEnChangeEdit3)
	//ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedCleanGrid)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_REREADPG, OnBnClickedRereadpg)
	ON_EN_KILLFOCUS(IDC_EDIT2, OnEnKillfocusEdit2)

	ON_EN_KILLFOCUS(IDC_EDIT_NEW_START, OnEnChangePeriodOut)
	ON_EN_KILLFOCUS(IDC_EDIT_NEW_END, OnEnChangePeriodOut)

	ON_EN_CHANGE(IDC_EDIT4, OnEnChangeStartDate)
	ON_EN_CHANGE(IDC_EDIT5, OnEnChangeNormalTimeInt)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CCreatePG::OnBnClickedButtonAdd)
	ON_CBN_SELCHANGE(IDC_COMBODISPLAYSIZE5, &CCreatePG::OnCbnSelchangeCombodisplaysize5)
	ON_BN_CLICKED(IDC_MAKEREPSTYLE, &CCreatePG::OnBnClickedMakerepstyle)
	ON_BN_CLICKED(IDCANCEL, &CCreatePG::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_ACTBLOCK, &CCreatePG::OnEnChangeActblock)
	//ON_EN_CHANGE(IDC_BLOCKSIZE, &CCreatePG::OnEnChangeBlocksize)
	//ON_EN_KILLFOCUS(IDC_BLOCKSIZE, &CCreatePG::OnEnKillfocusBlocksize)
	ON_EN_HSCROLL(IDC_ACTBLOCK, &CCreatePG::OnEnHscrollActblock)
	ON_BN_CLICKED(IDC_INCREASEBLOCK, &CCreatePG::OnBnClickedIncreaseblock)
	ON_BN_CLICKED(IDC_DECREASEBLOCK, &CCreatePG::OnBnClickedDecreaseblock)
	ON_BN_CLICKED(IDC_UPDATEGRID, &CCreatePG::OnBnClickedUpdategrid)
	ON_BN_CLICKED(IDC_MAKEREPSTYLE, &CCreatePG::OnBnClickedMakerepstyle2)
	ON_BN_CLICKED(IDC_MEMORY, &CCreatePG::OnBnClickedMemory)
END_MESSAGE_MAP()
void CCreatePG::OnEnChangeStartDate()
{  //Make start date
	UpdateData(TRUE);
	CString text;
	GetDlgItem(IDC_EDIT4)->GetWindowText(text);
	m_StartDateString = CT2A(text).m_psz;
	if (m_StartDateString.size() > 0) {

	};
	//CPG *pCPG = new CPG();
	CPG *pCPG = (CPG*)m_pOPG;
}

// CCreatePG message handlers

void CCreatePG::OnBnClickedMakeRegularTimeIncrease()
{ // Make Regular time interval

	UpdateData(TRUE);
	//CPG *pCPG = new CPG();
	CPG *pCPG=(CPG*)m_pOPG;	




//	pCPG->SetCompleteRead(TRUE);
//	pCPG->CreateNewFile(m_NumVariables,m_NumRecords);

	if(pCPG->GetNumBuffers()>1)
		pCPG->SetActBuffer(1);

	pCPG->SetDate(1,string(m_StartDateString));


	
	size_t t=pCPG->GetLongTime(1);
	for(size_t i=1;i<m_NumRecords;i++) {
		t+=m_NormalTimeInt;
		pCPG->SetLongTime(i+1, t);

	}
	pCPG->SetDate(1,string(m_StartDateString));


	if(pCPG->GetNumBuffers()>1)
	    pCPG->SetActBuffer(1);
	m_gridData.OnReadTime(pCPG);


	m_EndDateString=pCPG->GetDate(m_NumRecords).c_str();
	pCPG->SetNormalTimeInterval(m_NormalTimeInt);
	GetDlgItem(IDC_EDIT6)->SetWindowText(CString(m_EndDateString.c_str()));
	GetDlgItem(IDC_EDIT6)->RedrawWindow();
	GetDlgItem(IDC_EDIT_NEW_END)->SetWindowText(CString(m_EndDateString.c_str()));
	GetDlgItem(IDC_EDIT_NEW_END)->RedrawWindow();

}
static TCHAR buffer[900000];
void CCreatePG::OnBnClickedReadTextFile()
{ // Ascii file Read function

	CPG *pCPG_Base=(CPG*)m_pOPG;
	pCPG_Base->SetCompleteRead(FALSE);

		CString strNewFileName;

		CString pszFileName ;
		LPCTSTR lpszDefExt=_T("txt|csv"), lpszFilter;
		lpszFilter = _T("Text File (*.txt)|*.txt|Comma delimited Text File (*.csv) |*.csv|");
		pszFileName=CString(m_PG_FileName.c_str());
 		CFileDialog	NewFileDlg(TRUE,lpszDefExt,pszFileName, OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR|OFN_ALLOWMULTISELECT,lpszFilter);

 	//CFileDialog	DriveFileDlg(TRUE,lpszDefExt,pszFileName, OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR|OFN_ALLOWMULTISELECT,lpszFilter);
	NewFileDlg.m_ofn.lpstrFile=&buffer[0];
	NewFileDlg.m_ofn.nMaxFile=900000;
	//DriveFileDlg.m_ofn.lpstrInitialDir=UserDirectory;




		if(NewFileDlg.DoModal()==IDOK) {

			CString str=NewFileDlg.GetPathName();
			pCPG_Base->CloseFile();
			pCPG_Base->SetNewFileName("Dump.bin");
			 
			pCPG_Base->ReOpen();

			POSITION pos=NewFileDlg.GetStartPosition( );
			size_t count_files=0;
			while(pos!=nullptr) {
				NewFileDlg.GetNextPathName(pos);
				count_files++;
			}

			pos=NewFileDlg.GetStartPosition( );
			string filename;
			PGFileImport File;
			while(pos!=nullptr) {
				str=NewFileDlg.GetNextPathName(pos);
				if(File.ImportFromFile(CT2A(str).m_psz, false, m_ImportFormatValue)) {
					filename=File.GetFileName();
					File.CloseFile();
				}
				pCPG_Base->CloseFile();
				pCPG_Base->Reset();

			}



			
			
			

		
			//pCPG_Base->SetCompleteRead(TRUE);
			
			if(pCPG_Base->Open(filename)) {
				m_NumVariables=pCPG_Base->GetNumVariables();
				pCPG_Base->UpdateViewOrder();
				str=MFC_Util::ItoAscii(m_NumVariables);
				GetDlgItem(IDC_EDIT1)->SetWindowText(str);
				GetDlgItem(IDC_EDIT1)->RedrawWindow();
				m_Repit=pCPG_Base->GetNumRepititions();
				GetDlgItem(IDC_EDIT_REPIT)->SetWindowText(MFC_Util::ItoAscii(m_Repit));
				GetDlgItem(IDC_EDIT_REPIT)->RedrawWindow();
				if(m_Repit>1) {
					pCPG_Base->SetRepititionMode(true, m_Repit);
					m_BlockSize=pCPG_Base->GetNumRecordsBufSize();
					GetDlgItem(IDC_BLOCKSIZE)->SetWindowText(MFC_Util::ItoAscii(m_BlockSize));
					GetDlgItem(IDC_ACTBLOCK)->ModifyStyle(0,WS_DISABLED);
					GetDlgItem(IDC_NUMBLOCK)->ModifyStyle(0,WS_DISABLED);
					GetDlgItem(IDC_NUMBLOCK)->RedrawWindow();
					GetDlgItem(IDC_BLOCKSIZE)->RedrawWindow();
				}
				m_NumRecords=pCPG_Base->GetNumRecords();
				
				m_NormalTimeInt=pCPG_Base->GetNormalTimeInterval();
				m_gridDes.OnUpdate(6,m_NumVariables,FALSE,FALSE, m_Repit);
				m_gridDes.SetpCUG(&m_gridData);
				m_gridData.OnUpdate(m_NumVariables+1, m_NumRecords,TRUE,FALSE, m_Repit);
				
				m_gridDes.OnReadFileDescriptionToGrid(pCPG_Base);
				m_gridData.OnReadFileDataToGrid(pCPG_Base);
				m_CleanGrid=FALSE;
				m_PG_FileName=pCPG_Base->GetFileName().c_str();
				GetDlgItem(IDC_EDIT3)->SetWindowText(CString(m_PG_FileName.c_str()));
				GetDlgItem(IDC_EDIT3)->RedrawWindow();
				SetWindowText(_T("Edit PG File: ")+CString(m_PG_FileName.c_str()));


				GetDlgItem(IDC_EDIT_REPIT)->SetWindowText(MFC_Util::ItoAscii(m_Repit));
				GetDlgItem(IDC_EDIT_REPIT)->RedrawWindow();
				str=MFC_Util::ItoAscii(m_NumVariables);
				GetDlgItem(IDC_EDIT1)->SetWindowText(str);
				//GetDlgItem(IDC_EDIT1)->RedrawWindow();
				m_NumRecords=pCPG_Base->GetNumRecords();
				str=MFC_Util::ItoAscii(m_NumRecords);
				GetDlgItem(IDC_EDIT2)->SetWindowText(str);
				GetDlgItem(IDC_EDIT2)->RedrawWindow();

				GetDlgItem(IDC_EDIT5)->SetWindowText(MFC_Util::ItoAscii(m_NormalTimeInt));
				GetDlgItem(IDC_EDIT5)->RedrawWindow();


				m_OutputStartDateString=pCPG_Base->GetDate(1).c_str();
				m_OutputEndDateString=pCPG_Base->GetDate(m_NumRecords).c_str();
				m_gridDes.m_Changed=false;
				m_gridData.m_Changed=false;


			}


		}


}

void CCreatePG::OnEnChangeEdit2()
{
	//OnBnClickedButtonUpdategrid();
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	/*UpdateData(TRUE);
	CPG *pCPG=(CPG*)m_pOPG;
	if(pCPG!=nullptr) {
		m_gridDes.OnWriteGridToFile(pCPG);
	}


	if(m_CleanGrid) {
		m_gridDes.OnUpdate(6,m_NumVariables,FALSE, TRUE, m_Repit);
		m_gridData.OnUpdate(m_NumVariables+1, m_NumRecords,TRUE, TRUE, m_Repit);

		
		m_CleanGrid=FALSE;
	}
	else {
		m_gridDes.UpdateGridSize(6,m_NumVariables,FALSE);
		m_gridData.UpdateGridSize(m_NumVariables+1,m_NumRecords, TRUE);
	}
	*/}

void CCreatePG::OnBnClickedSave()
{
	// Save to File
	CPG *pCPG;
	bool newgrid;
	newgrid=FALSE;
	if(m_pOPG==nullptr) {
		pCPG = new CPG();
		pCPG->SetCompleteRead(TRUE);
		pCPG->CreateNewFile(m_NumVariables,m_NumRecords);
		m_gridData.OnWriteGridDataToFile(pCPG);
		m_gridDes.OnWriteGridDescriptionToFile(pCPG);
	}
	else {
		pCPG = (CPG*)m_pOPG;

		if (pCPG->GetCompleteRead()) {

			for (size_t i = 0; i < pCPG->GetNumVariables(); i++) {
				if (!pCPG->GetToBeSaved(i)) {
					pCPG->RemoveVariable(i);
					m_gridData.DeleteCol(i + 1);
					m_gridDes.DeleteRow(i);
					i--;
				}
			}

			if (!m_CleanGrid || m_gridDes.m_Changed) {
				m_gridData.OnWriteGridDataToFile(pCPG);
				m_gridDes.OnWriteGridDescriptionToFile(pCPG);
				newgrid = TRUE;
				m_CleanGrid = TRUE;
			}

		}
		else {
			if (m_MissingRemoveValue) {
				bool koll = RemoveMissingSave();


			}
			else {
				// Save info from grid if modifications made

				if (m_gridData.m_Changed) {
					pCPG->CloseFile();
					pCPG->ReOpen(true);
					m_gridData.OnWriteGridDataToFile(pCPG);
					m_gridDes.OnWriteGridDescriptionToFile(pCPG);
					m_gridData.m_Changed = false;
					pCPG->CloseFile();
				}
				if (m_gridDes.m_Changed) {
					pCPG->CloseFile();
					pCPG->ReOpen(true);
					m_gridDes.OnWriteGridDescriptionToFile(pCPG);
					m_gridDes.m_Changed = false;
					pCPG->CloseFile();
				}




				// Writing a non completly read file
				CString oldfile = CString(pCPG->GetFileName().c_str());

				CString pszFileName;
				LPCTSTR lpszDefExt = _T("bin"), lpszFilter;
				lpszFilter = _T("PG Bin File (*.bin)|*.bin||");
				pszFileName = oldfile;
				CFileDialog	NewFileDlg(FALSE, lpszDefExt, pszFileName, OFN_NOCHANGEDIR, lpszFilter);
				CString newfilename;

				if (NewFileDlg.DoModal() == IDOK) {
					newfilename = NewFileDlg.GetPathName();
				}
				else
					newfilename = oldfile;

				string nn, nvar, str;

				if (oldfile == newfilename) {
					pCPG->CloseFile();
					pCPG->ReOpen(true);
					pCPG->ReCalculateMinMax();
					pCPG->WritePGFileHeader(pCPG->GetNumRecords(), pCPG->GetNumRepititions());
					MFC_Util::MessageBox("PG Bin File: " + str + " \nSaved with \n" + nn + " records\n" + nvar + " Variables");
					pCPG->CloseFile();
				}
				else {
					m_PG_FileName = CT2A(newfilename).m_psz;
					pCPG->CloseFile();
					pCPG->SetNewFileName(CT2A(newfilename).m_psz);
					pCPG->ReOpen(true);
					pCPG->WritePGFile();
					pCPG->ReCalculateMinMax();
					pCPG->WritePGFileEnd(pCPG->GetNumRecords());
					//pCPG->WritePGFileHeader(pCPG->GetNumRecords(), pCPG->GetNumRepititions());
					pCPG->CloseFile();

					str = pCPG->GetFileName();
					nvar = FUtil::STD_ItoAscii(pCPG->GetNumVariables());
					nn = FUtil::STD_ItoAscii(pCPG->GetNumRecords());
					MFC_Util::MessageBox("PG Bin File: " + str + " \nSaved with \n" + nn + " records\n" + nvar + " Variables");


					/*if(!pCPG->WriteNewPGFileAs(string(newfilename), true)){
						  pCPG->CloseFile();
						  pCPG->ReOpen();
						  pCPG->WriteNewPGFileAs(string(newfilename), true);
					};*/


					/* if(pCPG->Open(string(newfilename))) {
						   m_NumVariables=pCPG->GetNumVariables();
						   m_NumRecords=pCPG->GetNumRecords();
						   m_gridDes.OnUpdate(6,m_NumVariables,FALSE);
						   m_gridDes.SetpCUG(&m_gridData);
						   m_gridData.OnUpdate(m_NumVariables+1, pCPG->GetCurrentRecords(),TRUE);
						   m_gridDes.OnReadFileToGrid(pCPG);
						   m_gridData.OnReadFileToGrid(pCPG);
					 };*/

				}
				str = pCPG->GetFileName().c_str();
				nvar = FUtil::STD_ItoAscii(pCPG->GetNumVariables());
				nn = FUtil::STD_ItoAscii(pCPG->GetNumRecords());
				MFC_Util::MessageBox("PG Bin File: " + str + " \nSaved with \n" + nn + " records\n" + nvar + " Variables");

				//return;
			}
		}

	}

//	if(m_pCF!=nullptr) {
		CString strNewFileName;
		//strNewFileName=UserDirectory;
		//CStdioFile NewFileDlg;
		CString pszFileName ;
		LPCTSTR lpszDefExt=_T("bin"), lpszFilter;
		lpszFilter = _T("PG Bin File (*.bin)|*.bin||");
		pszFileName=CString(m_PG_FileName.c_str());
 		CFileDialog	NewFileDlg(FALSE,lpszDefExt,pszFileName, OFN_NOCHANGEDIR,lpszFilter);
		
		size_t irecmiss;

		if(NewFileDlg.DoModal()==IDOK) {
			string str=CT2A(NewFileDlg.GetPathName()).m_psz;
			if(m_MissingRemoveValue) 
				irecmiss=pCPG->RemoveMissingRecords();
			
			if(pCPG->SaveAs(str,newgrid)){

				//OnInitDialog();


				if(m_MissingRemoveValue) {
					string nn;
					nn=FUtil::STD_ItoAscii(irecmiss);
					MFC_Util::MessageBox(nn+" Records are removed from file: "+str);
				}
				if(!m_gridData.isMeanCalculated()) {
					
					if(m_RestrictedSave) {
						string nn;
						nn=FUtil::STD_ItoAscii(pCPG->GetOutFileRecords());
						MFC_Util::MessageBox("PG Bin File: "+str+" Created with "+nn+" records");
					}
					else {
						string nn,nvar, nrep;
						nn=FUtil::STD_ItoAscii(pCPG->GetOutFileRecords());
						nvar=FUtil::STD_ItoAscii(pCPG->GetNumVariables());
						
						if(pCPG->GetNumRepititions()>1) {
							nrep=FUtil::STD_ItoAscii(pCPG->GetNumRepititions());
							MFC_Util::MessageBox("PG Bin File: "+str+" \nSaved with \n"+nn+" records\n"+nvar+ " Variables \n"+ nrep+ " Repetitions");
						}
						else
							MFC_Util::MessageBox("PG Bin File: "+str+" \nSaved with \n"+nn+" records\n"+nvar+ " Variables");

						m_PG_FileName=str;
						m_Saved=TRUE;
						GetDlgItem(IDC_EDIT3)->SetWindowText(CString(str.c_str()));
						GetDlgItem(IDC_EDIT3)->RedrawWindow();
					}
				}
				
				
					
			}
		}
//	}
//	else {
//		pCPG->SaveAs(m_PG_FileName);
//
//	}
	m_gridDes.m_Changed=false;
	m_gridData.m_Changed=false;

	if(m_pOPG==nullptr)
		delete pCPG;
	//Create PG Bin File
	// TODO: Add your control notification handler code here
}


bool CCreatePG::RemoveMissingSave() {
	CString strNewFileName;
	//strNewFileName=UserDirectory;
	//CStdioFile NewFileDlg;
	CString pszFileName;
	LPCTSTR lpszDefExt = _T("bin"), lpszFilter;
	lpszFilter = _T("PG Bin File (*.bin)|*.bin||");
	pszFileName = CString(m_PG_FileName.c_str());
	CFileDialog	NewFileDlg(FALSE, lpszDefExt, pszFileName, OFN_NOCHANGEDIR, lpszFilter);

	size_t irecmiss;
	CPG *pCPG; 
	pCPG = (CPG*)m_pOPG;

	if (NewFileDlg.DoModal() == IDOK) {
		string str = CT2A(NewFileDlg.GetPathName()).m_psz;
		if (m_MissingRemoveValue)
			irecmiss = pCPG->RemoveMissingRecords();

		if (pCPG->SaveAs(str, true)) {

			//OnInitDialog();


			if (m_MissingRemoveValue) {
				string nn;
				nn = FUtil::STD_ItoAscii(irecmiss);
				MFC_Util::MessageBox(nn + " Records are removed from file: " + str);
			}
			if (!m_gridData.isMeanCalculated()) {

				if (m_RestrictedSave) {
					string nn;
					nn = FUtil::STD_ItoAscii(pCPG->GetOutFileRecords());
					MFC_Util::MessageBox("PG Bin File: " + str + " Created with " + nn + " records");
				}
				else {
					string nn, nvar, nrep;
					nn = FUtil::STD_ItoAscii(pCPG->GetOutFileRecords());
					nvar = FUtil::STD_ItoAscii(pCPG->GetNumVariables());

					if (pCPG->GetNumRepititions()>1) {
						nrep = FUtil::STD_ItoAscii(pCPG->GetNumRepititions());
						MFC_Util::MessageBox("PG Bin File: " + str + " \nSaved with \n" + nn + " records\n" + nvar + " Variables \n" + nrep + " Repetitions");
					}
					else
						MFC_Util::MessageBox("PG Bin File: " + str + " \nSaved with \n" + nn + " records\n" + nvar + " Variables");

					m_PG_FileName = str;
					m_Saved = TRUE;
					GetDlgItem(IDC_EDIT3)->SetWindowText(CString(str.c_str()));
					GetDlgItem(IDC_EDIT3)->RedrawWindow();
				}
			}



		}
	}
	//	}
	//	else {
	//		pCPG->SaveAs(m_PG_FileName);
	//
	//	}
	m_gridDes.m_Changed = false;
	m_gridData.m_Changed = false;

	if (m_pOPG == nullptr)
		delete pCPG;


	return  true;
}
void CCreatePG::OnEnChangeEdit3()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CCreatePG::OnBnClickedButton4()
{  //Import from PG Bin File

	CPG *pCPG = new CPG();
	
	CString pszFileName ;
	LPCTSTR lpszDefExt=_T("bin"), lpszFilter;
	lpszFilter = _T("PG Bin File (*.bin)|*.bin||");
	CFileDialog	OpenFileDlg(TRUE,lpszDefExt,pszFileName, OFN_NOCHANGEDIR,lpszFilter);
	
	if(OpenFileDlg.DoModal()==IDOK) {
		CString str=OpenFileDlg.GetPathName();
		if(pCPG->Open(CT2A(str).m_psz)) {
			m_NumVariables=pCPG->GetNumVariables();
			m_NumRecords=pCPG->GetNumRecords();
			m_NormalTimeInt=pCPG->GetNormalTimeInterval();
			m_gridDes.OnUpdate(6,m_NumVariables,FALSE);
			m_gridDes.SetpCUG(&m_gridData);
			m_gridData.OnUpdate(m_NumVariables+1, m_NumRecords,TRUE);
			
			m_gridDes.OnReadFileDescriptionToGrid(pCPG);
			m_gridData.OnReadFileDataToGrid(pCPG);
			m_CleanGrid=FALSE;
			m_PG_FileName= CT2A(str).m_psz;
			GetDlgItem(IDC_EDIT3)->SetWindowText(str);
			GetDlgItem(IDC_EDIT3)->RedrawWindow();
			str=MFC_Util::ItoAscii(m_NumVariables);
			GetDlgItem(IDC_EDIT1)->SetWindowText(str);
			//GetDlgItem(IDC_EDIT1)->RedrawWindow();
			m_NumRecords=pCPG->GetNumRecords();
			str=MFC_Util::ItoAscii(m_NumRecords);
			GetDlgItem(IDC_EDIT2)->SetWindowText(str);


			GetDlgItem(IDC_EDIT5)->SetWindowText(MFC_Util::ItoAscii(m_NormalTimeInt));
			GetDlgItem(IDC_EDIT5)->RedrawWindow();


			m_OutputStartDateString=pCPG->GetDate(1).c_str();
			m_OutputEndDateString=pCPG->GetDate(m_NumRecords).c_str();


			}
	}


	delete pCPG;


	// TODO: Add your control notification handler code here
}

void CCreatePG::OnBnClickedCleanGrid()

{
	m_CleanGrid=TRUE;
	OnEnChangeEdit2();
	// TODO: Add your control notification handler code here
}

void CCreatePG::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if(	m_gridDes.m_Changed||m_gridData.m_Changed){
		OnBnClickedSave();	
		if(m_pPG_Created!=nullptr) {
			delete m_pPG_Created;
			m_pPG_Created=nullptr;
		}
	}
	if(m_pPG_Created!=nullptr) {
		delete m_pPG_Created;
		m_pPG_Created=nullptr;
	}
	OnOK();

}

void CCreatePG::OnBnClickedRereadpg()
{
	// TODO: Add your control notification handler code here
	if(m_pOPG==nullptr) return;
	CPG *pCPG = (CPG*)m_pOPG;
	CString str;
	if(!pCPG->GetCompleteRead()) 
		pCPG->ReOpen();


	m_NumVariables=pCPG->GetNumVariables();
	m_NumRecords=pCPG->GetNumRecords();
	m_Repit=pCPG->GetNumRepititions();

	bool vlog=pCPG->GetCompleteRead();
	pCPG->CheckEquiDistance();
	vlog=pCPG->GetCompleteRead();
	pCPG->CheckMissingNumbers();
	pCPG->ReCalculateMinMax();
	pCPG->WritePGFileEnd(m_NumRecords, m_Repit);
	vlog=pCPG->GetCompleteRead();
			
	m_gridDes.OnUpdate(6,m_NumVariables,FALSE,FALSE, m_Repit);
	m_gridDes.OnReadFileDescriptionToGrid(pCPG);
	if(!pCPG->GetCompleteRead()) {
		if(!pCPG->IsGood())	pCPG->ClearFile();
		pCPG->SetActBuffer(m_ActBlock);
	}
	if(pCPG->GetNumBuffers()<=1)
		m_gridData.OnUpdate(m_NumVariables+1,pCPG->GetNumRecords() ,TRUE, FALSE, m_Repit);
	else
		m_gridData.OnUpdate(m_NumVariables+1,pCPG->GetCurrentRecords() ,TRUE, FALSE, m_Repit);


	//m_gridData.OnUpdate(m_NumVariables+1, m_NumRecords,TRUE, FALSE, m_Repit);
	m_gridData.SetpDlg(this);
	m_gridData.OnReadFileDataToGrid(pCPG);
	m_CleanGrid=FALSE;
	str=pCPG->GetFileName().c_str();
	m_NormalTimeInt=pCPG->GetNormalTimeInterval();

	m_PG_FileName=CT2A(str).m_psz;
	GetDlgItem(IDC_EDIT3)->SetWindowText(str);
	GetDlgItem(IDC_EDIT3)->RedrawWindow();
	str=MFC_Util::ItoAscii(m_NumVariables);
	GetDlgItem(IDC_EDIT1)->SetWindowText(str);
	//GetDlgItem(IDC_EDIT1)->RedrawWindow();
	m_NumRecords=pCPG->GetNumRecords();
	m_OutputStartDateString=pCPG->GetDate(1).c_str();
	m_OutputEndDateString=pCPG->GetDate(m_NumRecords).c_str();

			

	if(pCPG->IsEquiDistance())
		m_EquiDistanceText="Regular equal size time intervals";
	else
		m_EquiDistanceText="Non Regular time intervals";
			
	GetDlgItem(IDC_REGULAR)->SetWindowText(CString(m_EquiDistanceText.c_str()));
	GetDlgItem(IDC_REGULAR)->RedrawWindow();

	if(pCPG->CheckMissingNumbers()) {
		int in=pCPG->GetNumberofVariablesWithMissings();
		size_t im=pCPG->GetNumberofMissing();
		m_MissingText=MFC_Util::ItoAscii(im)+" missing values in "+ MFC_Util::ItoAscii(in)+" variables";
	}
	else
		m_MissingText="No missing values";
			
	GetDlgItem(IDC_MISSING)->SetWindowText(CString(m_MissingText.c_str()));
	GetDlgItem(IDC_MISSING)->RedrawWindow();

	if(!pCPG->IsTimeIncreasing()) {
		m_IncreaseTextMessage="Time has "+FUtil::STD_ItoAscii(pCPG->GetJumpBacksNum())+ " backwards jump";
		if(pCPG->GetJumpBacksNum()>1) m_IncreaseTextMessage+='s';
		GetDlgItem(IDC_MAKEREPSTYLE)->ModifyStyle(WS_DISABLED,0);
	}
	else {
		GetDlgItem(IDC_MAKEREPSTYLE)->ModifyStyle(0,WS_DISABLED);
		m_IncreaseTextMessage="Time is increasing";
	}
	GetDlgItem(IDC_MAKEREPSTYLE)->RedrawWindow();

	GetDlgItem(IDC_INCREASE)->SetWindowText(CString(m_IncreaseTextMessage.c_str()));
	GetDlgItem(IDC_INCREASE)->RedrawWindow();

	GetDlgItem(IDC_EDIT_NEW_START)->SetWindowText(CString(m_OutputStartDateString.c_str()));
	GetDlgItem(IDC_EDIT_NEW_START)->RedrawWindow();
	GetDlgItem(IDC_EDIT_NEW_END)->SetWindowText(CString(m_OutputEndDateString.c_str()));
	GetDlgItem(IDC_EDIT_NEW_END)->RedrawWindow();
	GetDlgItem(IDC_EDIT6)->SetWindowText(CString(m_OutputEndDateString.c_str()));
	GetDlgItem(IDC_EDIT6)->RedrawWindow();
	GetDlgItem(IDC_EDIT4)->SetWindowText(CString(m_OutputStartDateString.c_str()));
	GetDlgItem(IDC_EDIT4)->RedrawWindow();

	str=MFC_Util::ItoAscii(m_NumRecords);
	GetDlgItem(IDC_EDIT2)->SetWindowText(str);
	GetDlgItem(IDC_EDIT5)->SetWindowText(MFC_Util::ItoAscii(m_NormalTimeInt));
	GetDlgItem(IDC_EDIT5)->RedrawWindow();
	//GetDlgItem(IDC_EDIT2)->RedrawWindow();
	pCPG->SetActBuffer(m_ActBlock);
	MFC_Util::MessageBox("PG Bin File: "+m_PG_FileName+" Re Read");
}
void CCreatePG::SetpGrid(void *pGrid)
{
	m_pGrid=pGrid;
	m_gridData.SetpDlg(this);
	m_gridDes.SetpDlg(this);
	if(m_pPG_Created!=nullptr)
		OnBnClickedRereadpg();
}

void CCreatePG::OnEnKillfocusEdit2()
{
	OnEnChangeEdit2();
	// TODO: Add your control notification handler code here
}

void CCreatePG::OnBnClickedButtonNewvar()
{ // Add new variables
	// TODO: Add your control notification handler code here
}

void CCreatePG::OnBnClickedButtonAppend()
{// Add new period
	// TODO: Add your control notification handler code here
}

void CCreatePG::OnBnClickedButtonNewdata()
{ //Import new data
	// TODO: Add your control notification handler code here
}
void CCreatePG::OnEnChangePeriodOut()
{ //Import new data
	UpdateData(TRUE);
	CPG *pCPG=(CPG*)m_pOPG;	
	m_RestrictedSave=pCPG->SetCommonPeriod(string(m_OutputStartDateString),string(m_OutputEndDateString));

	// TODO: Add your control notification handler code here
}

void CCreatePG::OnEnChangeNormalTimeInt()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code	UpdateData(TRUE);
	CPG *pCPG=(CPG*)m_pOPG;	pCPG->SetNormalTimeInterval(m_NormalTimeInt);

}

void CCreatePG::OnBnClickedCheck1()
{
	m_MissingRemoveValue=m_MissingRemove.GetCheck();
	// TODO: Add your control notification handler code here
}

void CCreatePG::OnBnClickedButtonAdd()
{
	// TODO: Add your control notification handler code here
	//Import from excel
	CPG *pCPG_Base=(CPG*)m_pOPG;

		CString strNewFileName;
		//strNewFileName=UserDirectory;
		//CStdioFile NewFileDlg;
		CString pszFileName ;
		LPCTSTR lpszDefExt=_T("txt"), lpszFilter;
		lpszFilter = _T("Ascii File (*.txt)|*.txt||");
		pszFileName=CString(m_PG_FileName.c_str());
 		CFileDialog	NewFileDlg(TRUE,lpszDefExt,pszFileName, OFN_NOCHANGEDIR,lpszFilter);
		
		if(NewFileDlg.DoModal()==IDOK) {
			CString str=NewFileDlg.GetPathName();

			pCPG_Base->CloseFile();
			pCPG_Base->SetNewFileName("Dump.bin");		 
			pCPG_Base->ReOpen();

			PGFileImport File;
			if(File.ImportFromFile(CT2A(str).m_psz, true, m_ImportFormatValue)) {
				string filename=File.GetFileName();
				File.CloseFile();

			/*if(pCPG_Base->ImportFromFile(string(str), TRUE,m_ImportFormatValue )) {
				pCPG_Base->SetCompleteRead(TRUE);
	*/

			if(pCPG_Base->Open(filename)) {
				m_NumVariables=pCPG_Base->GetNumVariables();
				m_NumRecords=pCPG_Base->GetNumRecords();
				m_NormalTimeInt=pCPG_Base->GetNormalTimeInterval();
				m_gridDes.OnUpdate(6,m_NumVariables,FALSE);
				m_gridDes.SetpCUG(&m_gridData);
				m_gridData.OnUpdate(m_NumVariables+1, m_NumRecords,TRUE);
				
				m_gridDes.OnReadFileDescriptionToGrid(pCPG_Base);
				m_gridData.OnReadFileDataToGrid(pCPG_Base);
				m_CleanGrid=FALSE;
				m_PG_FileName=pCPG_Base->GetFileName().c_str();
				GetDlgItem(IDC_EDIT3)->SetWindowText(str);
				GetDlgItem(IDC_EDIT3)->RedrawWindow();
				str=MFC_Util::ItoAscii(m_NumVariables);
				GetDlgItem(IDC_EDIT1)->SetWindowText(str);
				//GetDlgItem(IDC_EDIT1)->RedrawWindow();
				m_NumRecords=pCPG_Base->GetNumRecords();
				str=MFC_Util::ItoAscii(m_NumRecords);
				GetDlgItem(IDC_EDIT2)->SetWindowText(str);

				m_OutputStartDateString=pCPG_Base->GetDate(1).c_str();
				m_OutputEndDateString=pCPG_Base->GetDate(m_NumRecords).c_str();
				m_gridDes.m_Changed=false;
				m_gridData.m_Changed=false;
			}
		}
	}
}

void CCreatePG::OnCbnSelchangeCombodisplaysize5()
{
	// TODO: Add your control notification handler code here
	m_ImportFormatValue=m_ImportFormat.GetCurSel();
}



void CCreatePG::OnBnClickedButtonUpdategrid()
{

	UpdateData(TRUE);
	CPG *pCPG=(CPG*)m_pOPG;
	if(pCPG!=nullptr) {
		m_gridDes.OnWriteGridDescriptionToFile(pCPG);
	}
	if(m_CleanGrid) {
		m_gridDes.OnUpdate(6,m_NumVariables,FALSE, TRUE, m_Repit);


		if(pCPG->GetNumBuffers()<=1)
			m_gridData.OnUpdate(m_NumVariables+1,pCPG->GetNumRecords() ,TRUE, FALSE, m_Repit);
		else
			m_gridData.OnUpdate(m_NumVariables+1,pCPG->GetCurrentRecords() ,TRUE, FALSE, m_Repit);



		//m_gridData.OnUpdate(m_NumVariables+1, m_NumRecords,TRUE, TRUE, m_Repit);
		m_CleanGrid=FALSE;
	}
	else {
		m_gridDes.UpdateGridSize(6,m_NumVariables,FALSE, m_Repit);
		m_gridData.UpdateGridSize(m_NumVariables+1,m_NumRecords, TRUE, m_Repit);
	}
	if(pCPG->GetCompleteRead()) {
		m_BlockSize=m_NumRecords;
		pCPG->SetActBufferSize(m_NumRecords);

	}

	// TODO: Add your control notification handler code here
}


void CCreatePG::OnBnClickedMakerepstyle()
{
	CPG *pCPG=(CPG*)m_pOPG;
	 m_NumRecords=pCPG->GetNumRecords()/(pCPG->GetJumpBacksNum()+1);
	 m_NumVariables=pCPG->GetNumVariables();
	 m_Repit= pCPG->GetJumpBacksNum()+1;
	 pCPG->SetRepititionMode(true,m_Repit);
	 if(!pCPG->IsAddInfoSet())
		 pCPG->SetInfoMode();


	//pCPG->AdjustSize(m_NumRecords, m_NumVariables,m_Repit);
	pCPG->WriteNewPGFileAs(pCPG->GetFileName());
	m_NumVariables=pCPG->GetNumVariables();
	GetDlgItem(IDC_EDIT1)->SetWindowText(MFC_Util::ItoAscii(m_NumRecords));
	GetDlgItem(IDC_EDIT_REPIT)->SetWindowText(MFC_Util::ItoAscii(m_Repit));
	pCPG->Open(pCPG->GetFileName());

	for(size_t irep=0; irep<m_Repit; irep++) {
	   pCPG->SetRepititionSequence(irep+1);
	   pCPG->ReCalculateMinMax(irep+1);
	}

	OnBnClickedRereadpg();

	//OnBnClickedButtonUpdategrid();
	// TODO: Add your control notification handler code here
}


void CCreatePG::OnBnClickedCancel()
	{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
	}


void CCreatePG::OnEnChangeActblock()
{
	UpdateData(TRUE);
	if(m_ActBlock==0) return;
	if(m_pOPG==nullptr) return;
	CPG *pCPG = (CPG*)m_pOPG;
	m_gridData.OnTabSelected(0);
	if(m_gridData.m_Changed) {
		if(!m_gridData.OnWriteGridDataToFile(pCPG)) return;
		m_gridData.m_Changed=false;

	}
	pCPG->SetActBuffer(m_ActBlock);
	if(pCPG->GetNumBuffers()<=1)
		m_gridData.OnUpdate(m_NumVariables+1,pCPG->GetNumRecords() ,TRUE, FALSE, m_Repit);
	else
		m_gridData.OnUpdate(m_NumVariables+1,pCPG->GetCurrentRecords() ,TRUE, FALSE, m_Repit);

//	pCPG->SetActBufferReset();
	m_gridData.OnReadFileDataToGrid(pCPG);
	m_gridData.m_Changed=false;
	m_CleanGrid=FALSE;
	//OnBnClickedRereadpg();
}


void CCreatePG::OnEnChangeBlocksize()
{
	//UpdateData(TRUE);
	CPG *pCPG = (CPG*)m_pOPG;
	if(m_BlockSize<=0) return;
	if(m_ActBlock<=0) m_ActBlock=1;

	size_t cpos=(m_ActBlock-1)*pCPG->GetNumRecordsBufSize();
	pCPG->SetActBufferSize(m_BlockSize);
	m_NumBlocks=pCPG->GetNumBuffers();
	m_ActBlock=cpos/m_BlockSize+1;
	pCPG->SetActBuffer(m_ActBlock);
	if(pCPG->GetNumBuffers()<=1)
		m_gridData.OnUpdate(m_NumVariables+1,pCPG->GetNumRecords() ,TRUE, FALSE, m_Repit);
	else
		m_gridData.OnUpdate(m_NumVariables+1,pCPG->GetCurrentRecords() ,TRUE, FALSE, m_Repit);

	m_gridData.OnReadFileDataToGrid(pCPG);
	m_CleanGrid=FALSE;
	GetDlgItem(IDC_ACTBLOCK)->SetWindowText(MFC_Util::ItoAscii(m_ActBlock));
	GetDlgItem(IDC_NUMBLOCK)->SetWindowText(MFC_Util::ItoAscii(pCPG->GetNumBuffers()));
	GetDlgItem(IDC_ACTBLOCK)->RedrawWindow();
	GetDlgItem(IDC_NUMBLOCK)->RedrawWindow();


}


void CCreatePG::OnEnKillfocusBlocksize()
{
	UpdateData(TRUE);
	CPG *pCPG = (CPG*)m_pOPG;
	if(m_BlockSize<=0) return;
	size_t cpos=(m_ActBlock-1)*pCPG->GetNumRecordsBufSize();
	pCPG->SetActBufferSize(m_BlockSize);
	m_NumBlocks=pCPG->GetNumBuffers();
	m_ActBlock=cpos/m_BlockSize+1;
	pCPG->SetActBuffer(m_ActBlock);
	if(pCPG->GetNumBuffers()<=1)
		m_gridData.OnUpdate(m_NumVariables+1,pCPG->GetNumRecords() ,TRUE, FALSE, m_Repit);
	else
		m_gridData.OnUpdate(m_NumVariables+1,pCPG->GetCurrentRecords() ,TRUE, FALSE, m_Repit);

	m_gridData.OnReadFileDataToGrid(pCPG);
	m_CleanGrid=FALSE;
	GetDlgItem(IDC_ACTBLOCK)->SetWindowText(MFC_Util::ItoAscii(m_ActBlock));
	GetDlgItem(IDC_NUMBLOCK)->SetWindowText(MFC_Util::ItoAscii(pCPG->GetNumBuffers()));
	GetDlgItem(IDC_ACTBLOCK)->RedrawWindow();
	GetDlgItem(IDC_NUMBLOCK)->RedrawWindow();
	// TODO: Add your control notification handler code here
}


void CCreatePG::OnEnHscrollActblock()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_SCROLL flag ORed into the lParam mask.

	// TODO:  Add your control notification handler code here
}


void CCreatePG::OnBnClickedIncreaseblock()
{
	CPG *pCPG = (CPG*)m_pOPG;
	if(m_ActBlock<pCPG->GetNumBuffers()) {
		pCPG->SetActBuffer(m_ActBlock);
		m_ActBlock++;
		GetDlgItem(IDC_ACTBLOCK)->SetWindowText(MFC_Util::ItoAscii(m_ActBlock));
		GetDlgItem(IDC_ACTBLOCK)->RedrawWindow();
		//OnEnChangeActblock();
	}
}


void CCreatePG::OnBnClickedDecreaseblock()
{
	CPG *pCPG = (CPG*)m_pOPG;
	if(m_ActBlock>1) {
		pCPG->SetActBuffer(m_ActBlock);
		m_ActBlock--;
		GetDlgItem(IDC_ACTBLOCK)->SetWindowText(MFC_Util::ItoAscii(m_ActBlock));
		GetDlgItem(IDC_ACTBLOCK)->RedrawWindow();
		//OnEnChangeActblock();
	}
}


void CCreatePG::OnBnClickedUpdategrid()
{

	UpdateData(TRUE);
	CPG *pCPG=(CPG*)m_pOPG;
	if(pCPG!=nullptr) {
		if(m_gridDes.m_Changed)
			m_gridDes.OnWriteGridDescriptionToFile(pCPG);
		if(m_gridData.m_Changed)
			m_gridData.OnWriteGridDataToFile(pCPG);
	}
	if(m_CleanGrid) {
		m_gridDes.OnUpdate(6,m_NumVariables,FALSE, TRUE, m_Repit);


		if(pCPG->GetNumBuffers()<=1)
			m_gridData.OnUpdate(m_NumVariables+1,pCPG->GetNumRecords() ,TRUE, TRUE, m_Repit);
		else
			m_gridData.OnUpdate(m_NumVariables+1,pCPG->GetCurrentRecords() ,TRUE, TRUE, m_Repit);

		m_CleanGrid=FALSE;
	}
	else {
		m_gridDes.UpdateGridSize(6,m_NumVariables,FALSE, m_Repit);
		CString str;
		GetDlgItem(IDC_BLOCKSIZE)->GetWindowText(str);
		if(pCPG->GetNumBuffers()>1) {
			m_BlockSize=MFC_Util::AtoInt(str);
			pCPG->AdjustSize(m_NumRecords,pCPG->GetNumVariables());
			pCPG->SetActBufferSize(m_BlockSize);
			pCPG->SetActBuffer(1);
			m_NumBlocks=pCPG->GetNumBuffers();
			GetDlgItem(IDC_NUMBLOCK)->SetWindowText(MFC_Util::ItoAscii(m_NumBlocks));
			GetDlgItem(IDC_NUMBLOCK)->RedrawWindow();
			m_gridData.OnUpdate(m_NumVariables+1,pCPG->GetCurrentRecords() ,TRUE, FALSE, m_Repit);
			m_gridData.OnReadFileDataToGrid(pCPG);
		//	m_gridData.UpdateGridSize(m_NumVariables+1,pCPG->GetCurrentRecords(), TRUE, m_Repit);
		}
		else
			m_gridData.UpdateGridSize(m_NumVariables+1,m_NumRecords, TRUE, m_Repit);
	}
	if(pCPG->GetCompleteRead()) {
		m_BlockSize=m_NumRecords;
		pCPG->SetActBufferSize(m_NumRecords);
		GetDlgItem(IDC_BLOCKSIZE)->SetWindowText(MFC_Util::ItoAscii(m_NumRecords));
		GetDlgItem(IDC_BLOCKSIZE)->RedrawWindow();
		m_NumBlocks=1;
		GetDlgItem(IDC_NUMBLOCK)->SetWindowText(MFC_Util::ItoAscii(m_NumBlocks));
		GetDlgItem(IDC_NUMBLOCK)->RedrawWindow();
	}
	else {
		if(m_BlockSize>m_NumRecords) m_BlockSize=m_NumRecords;
		pCPG->SetActBufferSize(m_BlockSize);
	}
	m_gridData.m_Changed=true;

	// TODO: Add your control notification handler code here
}


void CCreatePG::OnBnClickedMakerepstyle2()
	{
	// TODO: Add your control notification handler code here
	}


void CCreatePG::OnBnClickedMemoryreadbut()
{
	CPG *pCPG=(CPG*)m_pOPG;
	pCPG->CloseFile();
	pCPG->SetCompleteRead(false);
	pCPG->Open(pCPG->GetFileName());


	if(pCPG->GetCompleteRead()) {
	  GetDlgItem(IDC_MEMORY)->ModifyStyle(WS_DISABLED,0);
	  GetDlgItem(IDC_MEMREADCOMPLETE)->SetWindowText(_T("Complete Read/Write"));	
	}
	else {
	   GetDlgItem(IDC_MEMREADCOMPLETE)->SetWindowText(_T("Block Read/Write"));	
	   GetDlgItem(IDC_MEMORY)->ModifyStyle(0,WS_DISABLED);	
	}
	GetDlgItem(IDC_MEMORY)->RedrawWindow();
	GetDlgItem(IDC_MEMREADCOMPLETE)->RedrawWindow();


	// TODO: Add your control notification handler code here
}


void CCreatePG::OnBnClickedMemory()
{
	CPG *pCPG=(CPG*)m_pOPG;

	if(m_gridData.m_Changed){
			m_gridData.OnTabSelected(0);
			
			m_gridData.OnWriteGridDataToFile(pCPG);

			if(pCPG->GetCompleteRead()) {
				pCPG->CloseFile();
				pCPG->ReOpen(true);
				pCPG->WritePGFile();
			}
	}


	if(pCPG->GetCompleteRead()) {

		pCPG->CloseFile();
		pCPG->SetCompleteRead(false);

		m_BlockSize=min(10000, m_NumRecords);
		m_ActBlock=1;
		pCPG->SetActBufferSize(m_BlockSize);
		m_NumBlocks=pCPG->GetNumBuffers();
		pCPG->Open(pCPG->GetFileName());
		pCPG->CloseFile();
		pCPG->ReOpen(true);
		m_NumBlocks=pCPG->GetNumBuffers();
		GetDlgItem(IDC_ACTBLOCK)->SetWindowText(MFC_Util::ItoAscii(m_ActBlock));
		GetDlgItem(IDC_ACTBLOCK)->RedrawWindow();
		GetDlgItem(IDC_BLOCKSIZE)->SetWindowText(MFC_Util::ItoAscii(m_BlockSize));
		GetDlgItem(IDC_BLOCKSIZE)->RedrawWindow();
		//m_gridData.OnReadFileToGrid(pCPG);

	}
	else {

		pCPG->CloseFile();
		pCPG->SetCompleteRead(true);
		pCPG->Open(pCPG->GetFileName());
		pCPG->SetActBufferSize(m_NumRecords);
		m_ActBlock=1;
		m_NumBlocks=pCPG->GetNumBuffers();
		m_gridData.OnReadFileDataToGrid(pCPG);
		GetDlgItem(IDC_ACTBLOCK)->SetWindowText(MFC_Util::ItoAscii(m_ActBlock));
		GetDlgItem(IDC_ACTBLOCK)->RedrawWindow();
		GetDlgItem(IDC_BLOCKSIZE)->SetWindowText(MFC_Util::ItoAscii(m_NumRecords));
		GetDlgItem(IDC_BLOCKSIZE)->RedrawWindow();


	}
	m_NumBlocks=pCPG->GetNumBuffers();
	GetDlgItem(IDC_NUMBLOCK)->SetWindowText(MFC_Util::ItoAscii(m_NumBlocks));
	GetDlgItem(IDC_NUMBLOCK)->RedrawWindow();

	if(pCPG->GetCompleteRead()) {
	  GetDlgItem(IDC_MEMORY)->SetWindowText(_T("Stepwise Read"));
	  GetDlgItem(IDC_MEMREADCOMPLETE)->SetWindowText(_T("Complete Read/Write"));	
	}
	else {
	   GetDlgItem(IDC_MEMREADCOMPLETE)->SetWindowText(_T("Block Read/Write"));	
	   GetDlgItem(IDC_MEMORY)->SetWindowText(_T("Complete Read"));	
	}
	GetDlgItem(IDC_MEMORY)->RedrawWindow();
	GetDlgItem(IDC_MEMREADCOMPLETE)->RedrawWindow();



	int i=0;
	// TODO: Add your control notification handler code here
}
