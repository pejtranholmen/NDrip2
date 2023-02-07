// PG_FILEDLG.cpp : implementation file
//

#include "../stdafx.h"
#include "../CoupModel.h"
#include "./pg_filedlg.h"
#include "./FileImport\PGFileImport.h"



// CPG_FILEDLG dialog

IMPLEMENT_DYNAMIC(CPG_FILEDLG, CDialog)
CPG_FILEDLG::CPG_FILEDLG(void *pPG, enum pgfilemethod method, BOOL interpolate,BOOL textsource,BOOL createdirect, CWnd* pParent /*=nullptr*/)
	: CDialog(CPG_FILEDLG::IDD, pParent)
{
	m_PGMethod=method;


	if(method==PGMIX)
		m_Mix=TRUE;
	else if(method==PGADD)
		m_Mix=FALSE;
	else if(method==PGMERGE)
		m_Mix=FALSE;
	m_Interpolate=interpolate;
	m_Textsource=textsource;
	m_CreateDirect=createdirect;
	m_MixFileName=_T("");
	if(m_Mix)
		if(m_Interpolate)
			m_WindowText="Specify PG files that will be mixed and merge by interpolation to time points of the first file (new variables)";
		else
			m_WindowText="Specify PG files that will be mixed to the first file (new variables)";
	else if(method==PGADD)
		m_WindowText="Specify PG files that will be added to the first file (new time period)";
	else
		m_WindowText="Specify Similar PG Files that will be merge to a single aggregated file";

	m_pPG_in=pPG;
	m_pPG_out=nullptr;
	m_Init=TRUE;
	if(pPG==nullptr) {
		OnBnClickedAdd();
		if(m_CreateDirect) {

			auto FileName=MFC_Util::GetBinFileName();

			CPG *pPG= new CPG(string(FileName));
			m_pPG_out=pPG;
			m_FileName=pPG->GetFileName().c_str();
			pPG->SetCompleteRead(FALSE);
			if(m_Mix) {
				// Create a file with common time
				m_GridPGFiles.m_Interpolate=m_Interpolate;
				m_GridPGFiles.MixFiles(pPG);
			}
			else if(method==PGADD) {
				// Create a file with common variables
				m_GridPGFiles.AddFiles(pPG);
			}
			else if(method==PGMERGE) {
				m_GridPGFiles.MergeFiles(pPG);

			}






			//OnOK();
		}

	}
	else {
		m_GridPGFiles.OnUpdate((CPG*)pPG,1);
	}

}

CPG_FILEDLG::~CPG_FILEDLG()
{
}

void CPG_FILEDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CPG_FILEDLG::OnInitDialog()
{
	CDialog::OnInitDialog();
	if(m_Init) {

		m_GridPGFiles.AttachGrid(this, IDC_GRID);
		m_Init=FALSE;
		SetWindowText(m_WindowText);
		//OnBnClickedAdd();
		GetDlgItem(IDOK)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDOK)->RedrawWindow();
	}

	return TRUE;
}
INT_PTR CPG_FILEDLG::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DoModal();
}
void CPG_FILEDLG::OnOK() 
{
	// TODO: Add extra validation here
	//m_GridPGFiles.DeleteFiles();
	((CPG*)m_pPG_out)->CloseFile();
	CDialog::OnOK();
}
BEGIN_MESSAGE_MAP(CPG_FILEDLG, CDialog)
	ON_BN_CLICKED(ID_ADD, OnBnClickedAdd)
	ON_BN_CLICKED(ID_CREATE, OnBnClickedCreate)
END_MESSAGE_MAP()


// CPG_FILEDLG message handlers

void CPG_FILEDLG::OnBnClickedAdd()
{

	if(m_Textsource) {
		vector<string> txt_files;
		txt_files=MFC_Util::GetTxtFileNames();
		for(size_t i=0; i<txt_files.size(); i++) {
				PGFileImport *pPGn= new PGFileImport;
				pPGn->ImportFromFile(txt_files[i]);
				pPGn->SetCompleteRead(FALSE);		
				pPGn->Open(pPGn->GetFileName());
				pPGn->CloseFile();
				string filename=pPGn->GetFileName();
				delete pPGn;
				CPG *pPG= new CPG;
				pPG->Open(filename);
				if(pPG->IsOpen()) {
					m_GridPGFiles.OnUpdate(pPG);
				}
		}
	}
	else {
		vector<string> binnames;
		binnames=MFC_Util::GetBinFileNames();
		for(size_t i=0; i<binnames.size(); i++) {
			CPG *pPG= new CPG;
			pPG->Open(binnames[i]);
			if(pPG->IsOpen()) {
				m_GridPGFiles.OnUpdate(pPG);
			}
		}
	}



}

void CPG_FILEDLG::OnBnClickedCreate()
{
	m_Created=TRUE;
	m_GridPGFiles.m_Created=TRUE;
	GetDlgItem(IDOK)->ModifyStyle(WS_DISABLED,0);
	GetDlgItem(IDOK)->RedrawWindow();
	GetDlgItem(ID_ADD)->ModifyStyle(0,WS_DISABLED);
	GetDlgItem(ID_ADD)->RedrawWindow();
	
	GetDlgItem(ID_CREATE)->ModifyStyle(0,WS_DISABLED);
	GetDlgItem(ID_CREATE)->RedrawWindow();
	auto FileName = MFC_Util::GetBinFileName();
	CPG *pPG= new CPG(FileName);
	m_pPG_out=pPG;
	m_FileName=pPG->GetFileName().c_str();
	pPG->SetCompleteRead(FALSE);
	if(m_PGMethod==PGMIX) {
		// Create a file with common time
		m_GridPGFiles.m_Interpolate=m_Interpolate;
		m_GridPGFiles.MixFiles(pPG);
	}
	else if(m_PGMethod==PGADD) {
		// Create a file with common variables
		m_GridPGFiles.AddFiles(pPG);
	}
	else if(m_PGMethod==PGMERGE) {
		// Create a file with common variables
		m_GridPGFiles.MergeFiles(pPG);
	}

	// TODO: Add your control notification handler code here
}
