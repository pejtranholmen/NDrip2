// HistoryList.cpp : implementation file
//

#include "../NewBase/CommonModelInfo.h"
#include "../stdafx.h"
#include "../ModelTypes/NewModelType.h"
#include "../Std.h"
#include "../Util.h"
#include "../CoupModel.h"
#include "./HistoryList.h"


// CHistoryList dialog

IMPLEMENT_DYNAMIC(CHistoryList, CDialog)
CHistoryList::CHistoryList(SimB *pBase, int itype, Doc *pDoc, int index, CWnd* pParent /*=nullptr*/)
	: CDialog(CHistoryList::IDD, pParent)
{
	m_gridHistory.OnUpdate(pBase, pDoc, index);
	m_Init=true;

	string par, grp;
	if(pBase->GetType()==DB){
		par=pBase->GetName();	
		grp=((CDB*)pBase)->GetGroup().c_str();
		m_WindowText="History List for : ";
		m_WindowText+=par.c_str();
	}
	else if(itype==p_ModelInfo->GetTypeIndex("RunInfo")){
		par=((CRunInfo*)pBase)->GetName().c_str();
		grp=((CRunInfo*)pBase)->GetGroup().c_str();
		m_WindowText="History List for : ";
		m_WindowText+=par.c_str();
	}
	else {
		par=((SimB*)pBase)->GetName().c_str();
		grp=((SimB*)pBase)->GetGroup().c_str();
		if(index>=0) 
			par+="("+FUtil::STD_ItoAscii(index)+")";
		m_WindowText="History List for : ";
		m_WindowText+=par;
		m_WindowText+=", group : ";
		m_WindowText+=grp;
	}
	


	
}

CHistoryList::~CHistoryList()
{
}

void CHistoryList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
BOOL CHistoryList::OnInitDialog()
{

	CDialog::OnInitDialog();
	if(m_Init) {

		m_gridHistory.AttachGrid(this, IDC_GRID);
		m_Init=false;
		SetWindowText(CString(m_WindowText.c_str()));
	}


	return true;

}


BEGIN_MESSAGE_MAP(CHistoryList, CDialog)
END_MESSAGE_MAP()


// CHistoryList message handlers
