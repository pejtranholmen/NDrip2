// DB_Plant_P.cpp : implementation file
//

#include "stdafx.h"
#include "CoupModel.h"
#include "CoupModel_Hidc.h"
#include "DB_Plant_P.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DB_Plant_P property page

IMPLEMENT_DYNCREATE(DB_Plant_P, CPropertyPage)

DB_Plant_P::DB_Plant_P() : CPropertyPage(DB_Plant_P::IDD)
{
	m_ReadOnly=FALSE;
	//{{AFX_DATA_INIT(DB_Plant_P)
	//}}AFX_DATA_INIT
}

DB_Plant_P::~DB_Plant_P()
{
	Cleaning();
}

void DB_Plant_P::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DB_Plant_P)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DB_Plant_P, CPropertyPage)
	//{{AFX_MSG_MAP(DB_Plant_P)
	ON_WM_VSCROLL()
	ON_WM_HELPINFO()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DB_Plant_P message handlers

BOOL DB_Plant_P::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet* parent = (CPropertySheet*)GetParent();
	parent->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	CRect			koord(10,0,120,20); 
	CEdit			*pedit;
	CStatic			*pstatic;
	CP				*pCP;

	int				Counter;

	for(int i=0;i<statarr.GetSize();i++) 
		delete statarr.GetAt(i);
	statarr.RemoveAll();
	for(i=0;i<editarr.GetSize();i++) 
		delete editarr.GetAt(i);
	editarr.RemoveAll();

// Static Box	
	Counter=0;
	for (i=0;i<m_pCPArray.GetSize();i++) {
		pCP=(CP*)m_pCPArray.GetAt(i);
		if(pCP!=NULL){
			if(!pCP->IsDB_Disabled()&&m_pDoc->m_UserLevel<=pCP->GetUserLevel()){
				CRect coord=koord;
				coord.left+=10;  // Name
				coord.right+=50;
				coord.top+=Counter*25;
				coord.bottom+=Counter*25;
				CString kk=pCP->GetName();
				statarr.Add(new CStatic);
				pstatic=statarr[statarr.GetUpperBound()];
				pstatic->Create(kk, WS_CHILD | WS_VISIBLE ,coord,this,((CBase*)pCP)->GetIDCNumber());
				pstatic->SetFont(GetFont());
				Counter++;
				coord.left+=400;	  // Unit
				coord.right+=410;
				kk="(";
				kk+=pCP->GetUnit();kk+=")";
				statarr.Add(new CStatic);
				pstatic=statarr[statarr.GetUpperBound()];
				pstatic->Create(kk, WS_CHILD | WS_VISIBLE ,coord,this,5000);
				pstatic->SetFont(GetFont());
			}
		}
	}
//  CEdit Field
	Counter=0;
	CRect coord;
	for (i=0;i<m_pCPArray.GetSize();i++) {
		pCP=(CP*)m_pCPArray.GetAt(i);
		if(pCP!=NULL){
			if(!pCP->IsDB_Disabled()&&m_pDoc->m_UserLevel<=pCP->GetUserLevel()){
				coord=koord;
				coord.left+=180;
				coord.right+=140;
				coord.top+=Counter*25;
				coord.bottom+=Counter*25;
				editarr.Add(new CEdit);
				pedit=editarr[Counter];
				pedit->Create(ES_RIGHT | WS_CHILD | WS_VISIBLE | WS_BORDER |
							WS_TABSTOP | ES_AUTOHSCROLL,coord,this,((CBase*)pCP)->GetIDCNumber());
				pedit->ModifyStyleEx(0,WS_EX_CLIENTEDGE);
				pedit->SetFont(GetFont());
				pedit->SetWindowText(FtoAscii(*(float*)pCP->GetDBValue()));

				if(m_ReadOnly) pedit->SetReadOnly();
				coord.left+=160;
				coord.right+=150;
				CString kk="(";
				kk+=FtoAscii(*(float*)pCP->GetOriginalValue());kk+=")";
				statarr.Add(new CStatic);
				pstatic=statarr[statarr.GetUpperBound()];
				pstatic->Create(kk, WS_CHILD | WS_VISIBLE ,coord,this,5000);
				pstatic->SetFont(GetFont());
				Counter++;
			}
		}
	}

	if(Counter==0) {
		return CPropertyPage::OnWizardNext();
	}

	SetScrollRange(SB_VERT,0,coord.bottom-200);

	return CPropertyPage::OnSetActive();
}

/*void DB_Plant_P::OnView() 
{
	// TODO: Add your control notification handler code here
	
} */

LRESULT DB_Plant_P::OnWizardNext() 
{
// Updating of values
      int Counter=0;
	CEdit *pedit;
	CString text;
	float value;
	for (int i=0;i<m_pCPArray.GetSize();i++) {
		CP *pCP=(CP*)m_pCPArray.GetAt(i);
		if(pCP!=NULL){
			if(!pCP->IsDB_Disabled()&&m_pDoc->m_UserLevel<=pCP->GetUserLevel()){
				pedit=editarr[Counter];
				pedit->GetWindowText(text);
				value=AtoFloat(text);
				pCP->SetDBValue(value);
				Counter++;
			}
		}
	}

	Cleaning();

	SetScrollPos(SB_VERT,0);

	return CPropertyPage::OnWizardNext();
}
void DB_Plant_P::Cleaning()
{
// Cleaning of array with pointers
	for(int i=0;i<editarr.GetSize();i++)
		delete editarr.GetAt(i);
	editarr.RemoveAll();
	for(i=0;i<statarr.GetSize();i++)
		delete statarr.GetAt(i);
	statarr.RemoveAll();

}

void DB_Plant_P::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int ds,low,high,cur;
	switch(nSBCode) {		
		case SB_LINEDOWN : ds=-10; break;
		case SB_LINEUP : ds=10; break;
		case SB_PAGEDOWN : ds=-80; break;
		case SB_PAGEUP : ds=80; break;
		default: ds=0; }
	GetScrollRange(SB_VERT,&low,&high);
	cur=GetScrollPos(SB_VERT);
	if (cur-ds >= high) ds=cur-high; 
	if (cur-ds <= low) ds=cur-low; 
	ScrollWindow(0,ds);
	SetScrollPos(SB_VERT,cur-ds);
	
	
	CPropertyPage::OnVScroll(nSBCode, nPos, pScrollBar);
}

LRESULT DB_Plant_P::OnWizardBack() 
{
	SetScrollPos(SB_VERT,0);
	return CPropertyPage::OnWizardBack();
}

BOOL DB_Plant_P::OnHelpInfo(HELPINFO* pHelpInfo) 
{		CP* pCP;
		pCP=(CP*)m_pCPArray[0];
		int i=((CBase*)pCP)->GetGroupIDNumber();
	    AfxGetApp()->WinHelp(0x20000+i);
		return TRUE;
//		return CPropertyPage::OnHelpInfo(pHelpInfo);
}

void DB_Plant_P::OnContextMenu(CWnd* pWnd, CPoint point) 
{
		CP* pCP;
		pCP=(CP*)m_pCPArray[0];
		int i=((CBase*)pCP)->GetGroupIDNumber();
	    AfxGetApp()->WinHelp(0x20000+i);
}
