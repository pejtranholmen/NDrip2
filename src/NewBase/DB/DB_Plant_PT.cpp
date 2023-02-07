// DB_Plant_PT.cpp : implementation file
//

#include "stdafx.h"
#include "CoupModel.h"
#include "DB_Plant_PT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// DB_Plant_PT property page

IMPLEMENT_DYNCREATE(DB_Plant_PT, CPropertyPage)

DB_Plant_PT::DB_Plant_PT() : CPropertyPage(DB_Plant_PT::IDD)
{
	m_ReadOnly=FALSE;
	
	//{{AFX_DATA_INIT(DB_Plant_PT)
	m_TabName = _T("");
	//}}AFX_DATA_INIT

}

DB_Plant_PT::~DB_Plant_PT()
{
	Cleaning();
}

void DB_Plant_PT::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DB_Plant_PT)
	DDX_Text(pDX, IDC_TABNAME, m_TabName);
	DDV_MaxChars(pDX, m_TabName, 60);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DB_Plant_PT, CPropertyPage)
	//{{AFX_MSG_MAP(DB_Plant_PT)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DB_Plant_PT message handlers

BOOL DB_Plant_PT::OnSetActive() 
{
	m_NumRows=*(int*)m_pCTab->GetDBValue();
	CPropertySheet* parent = (CPropertySheet*)GetParent();
	if(m_Last)
		parent->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);
	else
		parent->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);

	CRect			koord(10,70,100,90); 
	CRect			parentRect(0,0,90,90);
	CEdit			*k;
	CStatic			*s;
	CButton			*button;

	CString			text;
	float			flyttal;
	CPt				*pCPt;
	int				Counter;
	for(int i=0; i<statarr.GetSize();i++)
		delete statarr.GetAt(i);
	statarr.RemoveAll();
	for(i=0; i<indarr.GetSize();i++)
		delete indarr.GetAt(i);
	indarr.RemoveAll();

	for(i=0; i<editarr.GetSize();i++)
		delete editarr.GetAt(i);
	editarr.RemoveAll();

	if(m_pCTab->IsDB_Disabled()) 
		return CPropertyPage::OnWizardNext();

	m_NumRows=*(int*)m_pCTab->GetDBValue();
	Counter=0;
	for (i=0;m_pCTab->GetTitle(i,&text);i++) {
		pCPt=m_pCTab->GetCPtPointer(i);
		if(pCPt!=NULL){
			if(!pCPt->IsDB_Disabled()&&m_pDoc->m_UserLevel<=pCPt->GetUserLevel()){
				CRect coord=koord;
				coord.left+=30+Counter*90;
				coord.top-=44;
				coord.right+=30+Counter*90;
				statarr.Add(new CStatic);
				s=statarr[statarr.GetUpperBound()];
				s->Create(text, WS_CHILD | WS_VISIBLE|SS_LEFT ,coord,this,5000);
				s->SetFont(GetFont());
				coord.top+=28;
				coord.bottom+=14;
				statarr.Add(new CStatic);
				text="(";
				text+=FtoAscii(pCPt->GetOriginalValue(0));text+=")  (";
				text+=pCPt->GetUnit();text+=")";
				s=statarr[statarr.GetUpperBound()];
				s->Create(text, WS_CHILD | WS_VISIBLE ,coord,this,5000);
				s->SetFont(GetFont());

				Counter++;
			}
		}
	}
	m_pCPtArray.SetSize(Counter);
	m_IniValues.RemoveAll();
	/*now it checks if there is supposed to be a index-column.
	There are two different kinds if index-columns it appears and depending
	on which one of these we creates the one that is to be*/
	Counter=0;
	for (i=0;m_pCTab->GetDBCell(0,i,&flyttal);i++)
	{
		if (m_pCTab->GetType()==0||m_pCTab->GetType()>=3) {
			CRect coord=koord;
			coord.left+=10;
			coord.right-=50;
			coord.top+=i*20;
			coord.bottom+=i*20;
			indarr.Add(new CStatic);
			if(m_pCTab->GetType()==3||m_pCTab->GetType()==4) {
				m_checkarr.Add(new CButton);
				button=m_checkarr[i];
				button->Create(ItoAscii(i+1),BS_AUTOCHECKBOX|WS_CHILD | WS_VISIBLE, coord, this, 6000+i);
			}
			else {
				coord.left+=10;
				coord.right+=20;
				s=indarr[i];
				s->Create(ItoAscii(i+1), WS_CHILD | WS_VISIBLE ,coord,this,5000);
				s->SetFont(GetFont());
			}
		}
		if (m_pCTab->GetType()==2) 
		{
			CRect coord=koord;
			coord.left+=0;
			coord.right+=0;
			coord.top+=i*20;
			coord.bottom+=i*20;
			indarr.Add(new CStatic);
			s=indarr[i];
			s->Create(FtoAscii(flyttal), WS_CHILD | WS_VISIBLE ,coord,this,3000);
			s->SetFont(GetFont());
		}

		/*Now we create all the small edit windows in which the different 
		values are to be written. This could be in one colomn or several*/
		Counter=0;
		for (int j=0;m_pCTab->GetDBCell(j,i,&flyttal);j++) {
			pCPt=m_pCTab->GetCPtPointer(j);
			if(pCPt!=NULL){
				if(!pCPt->IsDB_Disabled()&&m_pDoc->m_UserLevel<=pCPt->GetUserLevel()){
					m_pCPtArray[Counter]=pCPt;
					float *invalue=new float;
					*invalue=flyttal;
					m_IniValues.Add(invalue);
					CRect coord=koord;
					coord.left+=30+Counter*90;
					coord.top+=i*20;
					coord.right+=30+Counter*90;
					coord.bottom+=i*20;
					SetScrollRange(SB_VERT,0,coord.bottom-200);
					SetScrollRange(SB_HORZ,0,coord.right-400);
					if(m_pCTab->GetType()>3) {
						static_type4Arr.Add(new CStatic);
						s=static_type4Arr[static_type4Arr.GetSize()-1];
						s->Create(FtoAscii(flyttal), WS_CHILD | WS_VISIBLE ,coord,this,4000+Counter);
						s->SetFont(GetFont());
					}
					else {
						editarr.Add(new CEdit);
						int filos=editarr.GetSize();
						k=editarr[editarr.GetSize()-1];
						k->Create(ES_RIGHT | WS_CHILD | WS_VISIBLE | WS_BORDER |
							WS_TABSTOP | ES_AUTOHSCROLL,coord,this,4000+Counter);
						k->ModifyStyleEx(0,WS_EX_CLIENTEDGE);
						k->SetFont(GetFont());
						k->SetWindowText(FtoAscii(flyttal));
						if(m_ReadOnly) k->SetReadOnly();
					}

					Counter++;
				}
			}
		}


	}

	if(Counter==0) 
		return  CPropertyPage::OnWizardNext();
	
	return CPropertyPage::OnSetActive();
}

/*void DB_Plant_PT::OnView() 
{
	// TODO: Add your control notification handler code here
	
} */


void DB_Plant_PT::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	int ds,low,high,cur;
	switch(nSBCode) {
		case SB_LINERIGHT : ds=-10; break;
		case SB_LINELEFT : ds=10; break;
		case SB_PAGERIGHT : ds=-80; break;
		case SB_PAGELEFT : ds=80; break;
		default: ds=0; }
	GetScrollRange(SB_HORZ,&low,&high);
	cur=GetScrollPos(SB_HORZ);
	if (cur-ds >= high) ds=cur-high; 
	if (cur-ds <= low) ds=cur-low; 
	ScrollWindow(ds,0);
	SetScrollPos(SB_HORZ,cur-ds);
	
	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void DB_Plant_PT::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
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

LRESULT DB_Plant_PT::OnWizardNext() 
{
	SetScrollPos(SB_VERT,0);
	if(SetValue())	
		return CPropertyPage::OnWizardNext();
	else
		return CPropertyPage::OnWizardNext();
}

BOOL DB_Plant_PT::OnWizardFinish() 
{
	if(SetValue())
	SetScrollPos(SB_VERT,0);
	return CPropertyPage::OnWizardFinish();
}

BOOL DB_Plant_PT::SetValue()
{
	CEdit	*k;
	float	flyttal;
	CString tmpfloat;
	CPt				*pCPt;
	CButton			*button;


	 int j=0, index=0;
	 m_pCTab->ResetPlotIndex();
	 for(j=0; j<m_checkarr.GetSize();j++){
		button=m_checkarr[j];
		if(button->GetCheck()) 
			m_pCTab->SetPlotIndex(j);
	 }

	 //  No Action if type 4
	if(m_pCTab->GetType()==4) return FALSE;

	 j=0;
	//we traverse the table list by calling the allocated spaces one by one
	for (int i=0;m_pCTab->GetDBCell(j,i,&flyttal);i++)
	{
		for (j;m_pCTab->GetDBCell(j,i,&flyttal);j++)
		{
		pCPt=m_pCTab->GetCPtPointer(j);
		if(pCPt!=NULL){
			if(!pCPt->IsDB_Disabled()&&m_pDoc->m_UserLevel<=pCPt->GetUserLevel()){

			//the editarr is an array that contains the data, let k point to the data 
				k=editarr[index];
			//get the text, most often a number, and put in tmpfloat
				k->GetWindowText((tmpfloat));
			//put the new value (or the old) back in the table list, but first we have to
			//make the ascii to a float.
				m_pCTab->SetDBCell(j,i,AtoFloat(tmpfloat));
				m_pCTab->GetDBCell(j,i,&flyttal);
				index++;
			}
		}
		}
		j=0;
	}

	Cleaning();
return TRUE;

}
void DB_Plant_PT::Cleaning()
{
// Cleaning up
	for(int i=0;i<editarr.GetSize();i++)
		delete editarr.GetAt(i);
	editarr.RemoveAll();
	for(i=0;i<statarr.GetSize();i++)
		delete statarr.GetAt(i);
	statarr.RemoveAll();
	for(i=0;i<m_checkarr.GetSize();i++)
		delete m_checkarr.GetAt(i);
	m_checkarr.RemoveAll();
	for(i=0;i<static_type4Arr.GetSize();i++)
		delete static_type4Arr.GetAt(i);
	static_type4Arr.RemoveAll();
/*	for(i=0;i<m_IniValues.GetSize();i++)
		delete m_IniValues.GetAt(i);*/
	m_IniValues.RemoveAll();
	for(i=0;i<indarr.GetSize();i++)
		delete indarr.GetAt(i);
	indarr.RemoveAll();
	for(i=0;i<m_checkarr.GetSize();i++)
		delete m_checkarr.GetAt(i);
	m_checkarr.RemoveAll();
}


LRESULT DB_Plant_PT::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class
	SetScrollPos(SB_HORZ,0);
	SetScrollPos(SB_VERT,0);
	return CPropertyPage::OnWizardBack();
}

void DB_Plant_PT::WinHelp(DWORD dwData, UINT nCmd) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::WinHelp(dwData, nCmd);
}

BOOL DB_Plant_PT::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
		int i=((CBase*)m_pCTab)->GetGroupIDNumber();
	    AfxGetApp()->WinHelp(0x20000+i);
		return TRUE;
	
//	return CPropertyPage::OnHelpInfo(pHelpInfo);
}
