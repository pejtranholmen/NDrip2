// FileScanner.cpp : implementation file
//

#include "../stdafx.h"
#include "FileScanner.h"


// CFileScanner dialog

IMPLEMENT_DYNAMIC(CFileScanner, CDialog)
CFileScanner::CFileScanner(CWnd* pParent /*=nullptr*/)
	: CDialog(CFileScanner::IDD, pParent)
	
{
	m_Info="";	 
}

CFileScanner::~CFileScanner()
{

}

void CFileScanner::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_prog);
	DDX_Control(pDX, IDC_STATIC_TEXT, m_ProgText);
	DDX_Control(pDX, IDC_STATIC_TEXT2, m_ProgText2);
}
BOOL CFileScanner::OnInitDialog()
{
	if(m_Info.GetLength()>0)
		SetWindowText(m_Info);
	CDialog::OnInitDialog();
	
	return true;
}	
void CFileScanner::Update(size_t procent)
{
	if(procent>0&&procent<101)
		m_prog.SetPos(procent);
	CString txt;
	txt.Format(_T("%u"),procent);
	m_ProgText.SetWindowText(txt);
	m_ProgText.RedrawWindow();
	m_prog.RedrawWindow();

}
void CFileScanner::SetLocalText(CString txt)
{

	m_ProgText2.SetWindowText(txt);
	m_ProgText2.RedrawWindow();

}



BEGIN_MESSAGE_MAP(CFileScanner, CDialog)





END_MESSAGE_MAP()

// CFileScanner message handlers
