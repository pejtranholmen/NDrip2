// DB_Plant_E.cpp : implementation file
//

#include "stdafx.h"
#include "CoupModel.h"
#include "DB_Plant_E.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// DB_Plant_E property page

IMPLEMENT_DYNCREATE(DB_Plant_E, CPropertyPage)

DB_Plant_E::DB_Plant_E() : CPropertyPage(DB_Plant_E::IDD)
{
	m_ReadOnly=FALSE;
	//{{AFX_DATA_INIT(DB_Plant_E)
	//}}AFX_DATA_INIT
}

DB_Plant_E::~DB_Plant_E()
{
}

void DB_Plant_E::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DB_Plant_E)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DB_Plant_E, CPropertyPage)
	//{{AFX_MSG_MAP(DB_Plant_E)
		ON_WM_VSCROLL()
	ON_WM_HELPINFO()
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DB_Plant_E message handlers
BOOL DB_Plant_E::OnSetActive() 
{
	CPropertySheet* parent = (CPropertySheet*)GetParent();
	parent->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);

	CRect			koord(10,0,260,20); 
	CEdit			*pedit;
	CStatic			*pstatic;
	CTab				*pCTab;

	int				Counter;

	for(int i=0;i<statarr.GetSize();i++) 
		delete statarr.GetAt(i);
	statarr.RemoveAll();
	for(i=0;i<editarr.GetSize();i++) 
		delete editarr.GetAt(i);
	editarr.RemoveAll();

// Static Box	
	Counter=0;
	for (i=0;i<m_pCTabArray.GetSize();i++) {
		pCTab=(CTab*)m_pCTabArray.GetAt(i);
		if(pCTab!=NULL){
			if(!pCTab->IsDB_Disabled()&&m_pDoc->m_UserLevel<=pCTab->GetUserLevel()){
				CRect coord=koord;
				coord.left+=10;
				coord.right+=60;
				coord.top+=Counter*25;
				coord.bottom+=Counter*25;
				CString kk=pCTab->GetName();
				statarr.Add(new CStatic);
				pstatic=statarr[statarr.GetUpperBound()];
				pstatic->Create(kk, WS_CHILD | WS_VISIBLE ,coord,this,((CBase*)pCTab)->GetIDCNumber());
				pstatic->SetFont(GetFont());
				Counter++;
			}
		}
	}
//  CEdit Field
	Counter=0;
	CRect coord;
	for (i=0;i<m_pCTabArray.GetSize();i++) {
		pCTab=(CTab*)m_pCTabArray.GetAt(i);
		if(pCTab!=NULL){
			if(!pCTab->IsDB_Disabled()&&m_pDoc->m_UserLevel<=pCTab->GetUserLevel()){
				coord=koord;
				coord.left+=280;
				coord.right+=50;
				coord.top+=Counter*25;
				coord.bottom+=Counter*25;
				editarr.Add(new CEdit);
				pedit=editarr[Counter];
				pedit->Create(ES_RIGHT | WS_CHILD | WS_VISIBLE | WS_BORDER |
							WS_TABSTOP | ES_AUTOHSCROLL,coord,this,((CBase*)pCTab)->GetIDCNumber());
				pedit->ModifyStyleEx(0,WS_EX_CLIENTEDGE);
				pedit->SetFont(GetFont());
				pedit->SetWindowText(ItoAscii(*(int*)pCTab->GetDBValue()));

				if(m_ReadOnly) pedit->SetReadOnly();
				coord=koord;
				coord.left+=340;
				coord.right+=130;
				coord.top+=Counter*25;
				coord.bottom+=Counter*25;
				CString kk="(";
				kk+=ItoAscii(pCTab->GetOriginalValue());kk+=")";
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
void DB_Plant_E::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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
LRESULT DB_Plant_E::OnWizardBack() 
{
	return CPropertyPage::OnWizardBack();
}

LRESULT DB_Plant_E::OnWizardNext() 
{
    int Counter=0;
	CEdit *pedit;
	CString text;
	int value;
	for (int i=0;i<m_pCTabArray.GetSize();i++) {
		CTab *pCTab=(CTab*)m_pCTabArray.GetAt(i);
		if(pCTab!=NULL){
			if(!pCTab->IsDB_Disabled()&&m_pDoc->m_UserLevel<=pCTab->GetUserLevel()){
				pedit=editarr[Counter];
				pedit->GetWindowText(text);
				value=AtoInt(text);
				pCTab->SetDBValue(value);
				Counter++;
			}
		}
	}
	Cleaning();
	return CPropertyPage::OnWizardNext();
}

void DB_Plant_E::Cleaning()
{
// Cleaning of array with pointers
	for(int i=0;i<editarr.GetSize();i++)
		delete editarr.GetAt(i);
	editarr.RemoveAll();
	for(i=0;i<statarr.GetSize();i++)
		delete statarr.GetAt(i);
	statarr.RemoveAll();

}



BOOL DB_Plant_E::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
		CTab* pCTab;
		pCTab=(CTab*)m_pCTabArray[0];
		int i=((CBase*)pCTab)->GetGroupIDNumber();
	    AfxGetApp()->WinHelp(0x20000+i);
		return TRUE;
	
//	return CPropertyPage::OnHelpInfo(pHelpInfo);
}

