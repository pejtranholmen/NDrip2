// DB_Plant_SW.cpp : implementation file
//

#include "stdafx.h"
#include "CoupModel.h"
#include "DB_Plant_SW.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DB_Plant_SW property page

IMPLEMENT_DYNCREATE(DB_Plant_SW, CPropertyPage)

DB_Plant_SW::DB_Plant_SW() : CPropertyPage(DB_Plant_SW::IDD)
{
	m_ReadOnly=FALSE;
	//{{AFX_DATA_INIT(DB_Plant_SW)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

DB_Plant_SW::~DB_Plant_SW()
{
	CleaningUp();
}

void DB_Plant_SW::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DB_Plant_SW)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DB_Plant_SW, CPropertyPage)
	//{{AFX_MSG_MAP(DB_Plant_SW)
	ON_WM_VSCROLL()

	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DB_Plant_SW message handlers

BOOL DB_Plant_SW::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet* parent = (CPropertySheet*)GetParent();
	parent->SetWizardButtons(PSWIZB_NEXT);

	CRect			koord(10,0,180,20); 
	CComboBox		*pcombo;
	CStatic			*pstatic;
	CSw				*pSw;
	CEdit			*pedit;

	int				Counter;
	GetWindowText(m_GroupName);
	m_GroupName+= "- Switches";
	SetWindowText(m_GroupName);
	UpdateData(TRUE);
// Static Box	
	Counter=0;
	m_pValidSWArray.RemoveAll();
	for (int i=0;i<m_pSWArray.GetSize();i++) {
		pSw=(CSw*)m_pSWArray.GetAt(i);
		if(pSw!=NULL){
			if(m_pDoc->m_UserLevel<=pSw->GetUserLevel()){
				m_pValidSWArray.Add(pSw);
				CRect coord=koord;
				coord.left+=10;
				coord.right+=10;
				coord.top+=Counter*25;
				coord.bottom+=Counter*25;
				statarr.Add(new CStatic);
				pstatic=statarr[Counter];
				pstatic->Create(pSw->GetName(), WS_CHILD | WS_VISIBLE ,coord,this,5000);
				pstatic->SetFont(GetFont());
				Counter++;
			}
		}
	}
//  ComboBox
	Counter=0;

	CRect coord;


	for (i=0;i<m_pSWArray.GetSize();i++) {
		pSw=(CSw*)m_pSWArray.GetAt(i);
		if(pSw!=NULL){
			if(m_pDoc->m_UserLevel<=pSw->GetUserLevel()){
				coord=koord;
				coord.left+=180;
				coord.right+=280;
				coord.top+=Counter*25;
				coord.bottom+=Counter*25+100;
				if(m_ReadOnly) {
				   coord.bottom-=100;
				   editarr.Add(new CEdit);
				   pedit=editarr[Counter];
				   pedit->Create(ES_LEFT | WS_CHILD | WS_VISIBLE | WS_BORDER |
							WS_TABSTOP | ES_AUTOHSCROLL,coord,this,6000+Counter);
					pedit->ModifyStyleEx(0,WS_EX_CLIENTEDGE);
				   pedit->SetWindowText(pSw->GetOption(*(int*)pSw->GetDBValue()));
				   pedit->SetFont(GetFont());
				   pedit->SetReadOnly();
				}
				else {
					comboarr.Add(new CComboBox);
					pcombo=comboarr[Counter];
					pcombo->Create(CBS_DROPDOWNLIST|WS_CHILD | WS_VISIBLE, coord, this, 6000+i);
					for (int k=0;!pSw->GetOption(k).IsEmpty();k++)
	 					pcombo->AddString(pSw->GetOption(k));
					pcombo->SelectString(0,pSw->GetOption(*(int*)pSw->GetDBValue()));
					pcombo->SetFont(GetFont());
				}
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

LRESULT DB_Plant_SW::OnWizardNext() 
{
// Updating of values
	if(!m_ReadOnly) {
		int Counter=0;
		CComboBox *pcombo;
		for (int i=0;i<m_pValidSWArray.GetSize();i++) {
			CSw *pSw=(CSw*)m_pValidSWArray.GetAt(i);
			if(pSw!=NULL){
					pcombo=comboarr[Counter];
					int value=pcombo->GetCurSel();
					pSw->SetDBValue(value);
					Counter++;
			}
		}
		if(m_GroupName=="Meteorological Data") {
		   //RadiationInput=1
		}

	}
	CleaningUp();

	SetScrollPos(SB_VERT,0);

	return CPropertyPage::OnWizardNext();
}

void DB_Plant_SW::CleaningUp()
{
// Cleaning 
	for(int i=0;i<comboarr.GetSize();i++)
		delete comboarr.GetAt(i);
	comboarr.RemoveAll();
	for(i=0;i<statarr.GetSize();i++)
		delete statarr.GetAt(i);
	statarr.RemoveAll();
	for(i=0;i<editarr.GetSize();i++)
		delete editarr.GetAt(i);
	editarr.RemoveAll();


}

BOOL DB_Plant_SW::OnInitDialog() 
{
	m_GroupName+= "- Switches";
	SetWindowText(m_GroupName);
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void DB_Plant_SW::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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


LRESULT DB_Plant_SW::OnWizardBack() 
{
	SetScrollPos(SB_VERT,0);
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																												
	return CPropertyPage::OnWizardBack();
}

BOOL DB_Plant_SW::OnHelpInfo(HELPINFO* pHelpInfo) 
{		CSw* pCSw;
		pCSw=(CSw*)m_pSWArray[0];
		int i=((CBase*)pCSw)->GetGroupIDNumber();
	    AfxGetApp()->WinHelp(0x20000+i);
		return TRUE;

	return CPropertyPage::OnHelpInfo(pHelpInfo);
}
