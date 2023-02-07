// SP_EDIT_RETENTION.cpp : implementation file
//

#include "stdafx.h"
#include ".\CUG\MyCug.h"
#include "Base.h"
#include "CoupModel.h"
#include "CoupModelDoc.h"
#include "CoupModelView.h"
#include "SP_EDIT_RETENTION.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" float PLOTPF_mp_PPSI[12];
extern "C" float PLOTPF_mp_THETAV[20][12];
extern "C" float PLOTPF_mp_BUDEP[22];
extern "C" float PLOTPF_mp_BLDEP[22];

extern "C" BOOL PLOTPF_mp_OPENDATABASE;
extern "C" void PLOTPF_mp_SAVEDATABASE();

extern "C" int PLOTPF_mp_ACTUALPROFILE;
extern "C" int PLOTPF_mp_ACTUALREPPROFILE;
extern "C" int PLOTPF_mp_ACTUALLEVEL;
extern "C" int PLOTPF_mp_ACTUALNUMLEVELS;
extern "C" int PLOTPF_mp_NT;

static double VarValues[1000], VarXValues[1000];
static int View_YAxisStyle[2]= {2,2};
static int View_YAxisTicks[2]= {6,6};
static double View_YAxisMin[2]= {0,0};
static double View_YAxisMax[2]= {6,6};
static int View_NumberOfTimePoints;
static int View_VariableNumber;



/////////////////////////////////////////////////////////////////////////////
// SP_EDIT_RETENTION dialog


SP_EDIT_RETENTION::SP_EDIT_RETENTION(CObject *pView, CString ProfileName, CWnd* pParent /*=NULL*/)
	: CDialog(SP_EDIT_RETENTION::IDD, pParent)
{	
	for(int j=0;j<PLOTPF_mp_NT;j++)
		m_PFlevels[j]=PLOTPF_mp_PPSI[j]*100;
	for(int i=0;i<PLOTPF_mp_ACTUALNUMLEVELS;i++) {
		for(j=0;j<PLOTPF_mp_NT;j++)
			m_WaterContent[i][j]=PLOTPF_mp_THETAV[i][j];
	}
	m_pView=(CCoupModelView*)pView;
	m_ProfileName=ProfileName;

	
	//{{AFX_DATA_INIT(SP_EDIT_RETENTION)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void SP_EDIT_RETENTION::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SP_EDIT_RETENTION)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SP_EDIT_RETENTION, CDialog)
	//{{AFX_MSG_MAP(SP_EDIT_RETENTION)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_VIEW, OnButtonView)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_LAYERS, OnButtonViewLayers)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SP_EDIT_RETENTION message handlers

BOOL SP_EDIT_RETENTION::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetWindowText("Water content (vol %) at different pressure heads");
	CRect			koord(54,45,104,65); 
	CRect			parent(0,0,50,50);
	CEdit			*k;
	CStatic			*s;

	CString			text;
	int				Counter=0;
	
// Numbers to points on top
	for (int j=0;j<PLOTPF_mp_NT;j++) {
 		CRect coord=koord;
 		coord.left+=40+j*50;
 		coord.top-=10;
 		coord.right+=40+j*50;
 		statarr.Add(new CStatic);
 		s=statarr[j];
		text=ItoAscii(j+1);
 		s->Create(text, WS_CHILD | WS_VISIBLE ,coord,this,5000);
 		s->SetFont(GetFont());
 		Counter++;
 	}
// Tension levels on top
	for (j=0;j<PLOTPF_mp_NT;j++) {
	   	CRect coord=koord;
	   	coord.left+=20+j*50;
		coord.top+=10;
		coord.bottom+=10;
	   	coord.right+=20+j*50;
	   	SetScrollRange(SB_VERT,0,coord.bottom+20);
	   	SetScrollRange(SB_HORZ,0,coord.right+20);
	     editarr.Add(new CEdit);
	
	   	k=editarr[editarr.GetSize()-1];
	   	k->Create(ES_RIGHT | WS_CHILD | WS_VISIBLE | WS_BORDER |
	   			WS_TABSTOP | ES_AUTOHSCROLL,coord,this,4000);
	   	k->ModifyStyleEx(0,WS_EX_CLIENTEDGE);
	   	k->SetFont(GetFont());
	   	k->SetWindowText(FtoAscii(m_PFlevels[j]));
	}



	for (int i=0;i<PLOTPF_mp_ACTUALNUMLEVELS;i++) {
// Number of layers
		CRect coord=koord;
		coord.left-=0;
		coord.top+=i*20+50;
		coord.right-=0;
		coord.bottom+=i*20+50;
		text=ItoAscii(i+1);
 		statarr.Add(new CStatic);
		s=statarr[i+PLOTPF_mp_NT];
		s->Create(text, WS_CHILD | WS_VISIBLE ,coord,this,5000);
		s->SetFont(GetFont());


		for (int j=0;j<PLOTPF_mp_NT;j++) {
// Floating point values
			CRect coord=koord;
			coord.left+=20+j*50;
			coord.top+=i*20+50;
			coord.right+=20+j*50;
			coord.bottom+=i*20+50;
			SetScrollRange(SB_VERT,0,coord.bottom+20);
			SetScrollRange(SB_HORZ,0,coord.right+20);
		   	editarr.Add(new CEdit);
	
			int filos=editarr.GetSize();
			k=editarr[editarr.GetSize()-1];
			k->Create(ES_RIGHT | WS_CHILD | WS_VISIBLE | WS_BORDER |
					WS_TABSTOP | ES_AUTOHSCROLL,coord,this,4000);
			k->ModifyStyleEx(0,WS_EX_CLIENTEDGE);
			k->SetFont(GetFont());
			k->SetWindowText(FtoAscii(m_WaterContent[i][j]));
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SP_EDIT_RETENTION::OnOK() 
{	int count=0;
	CEdit  	*k;
	CString string;
 	for(int j=0;j<PLOTPF_mp_NT;j++){
		k=editarr[count];
		count++;
		k->GetWindowText(string);
		PLOTPF_mp_PPSI[j]=AtoFloat(string)/100.f;
	}
	for(int i=0;i<PLOTPF_mp_ACTUALNUMLEVELS;i++) 
		for(j=0;j<PLOTPF_mp_NT;j++){
				k=editarr[count];
				count++;
				k->GetWindowText(string);
				PLOTPF_mp_THETAV[i][j]=AtoFloat(string);
		}
	PLOTPF_mp_SAVEDATABASE();
	CDialog::OnOK();
}

void SP_EDIT_RETENTION::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int ds,low,high,cur;
	switch(nSBCode) {		
		case SB_LINEDOWN : ds=-10; break;
		case SB_LINEUP : ds=10; break;
		case SB_PAGEDOWN : ds=-30; break;
		case SB_PAGEUP : ds=30; break;
		default: ds=0; }
	GetScrollRange(SB_VERT,&low,&high);
	cur=GetScrollPos(SB_VERT);
	if (cur-ds >= high) ds=cur-high; 
	if (cur-ds <= low) ds=cur-low; 
	ScrollWindow(0,ds);
	SetScrollPos(SB_VERT,cur-ds);
	
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void SP_EDIT_RETENTION::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	int ds,low,high,cur;
	switch(nSBCode) {
		case SB_LINERIGHT : ds=-10; break;
		case SB_LINELEFT : ds=10; break;
		case SB_PAGERIGHT : ds=-30; break;
		case SB_PAGELEFT : ds=30; break;
		default: ds=0; }
	GetScrollRange(SB_HORZ,&low,&high);
	cur=GetScrollPos(SB_HORZ);
	if (cur-ds >= high) ds=cur-high; 
	if (cur-ds <= low) ds=cur-low; 
	ScrollWindow(ds,0);
	SetScrollPos(SB_HORZ,cur-ds);
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

SP_EDIT_RETENTION::~SP_EDIT_RETENTION()
{
	for(int i=0;i<statarr.GetSize();i++)
		delete statarr.GetAt(i);
	statarr.RemoveAll();
	for(i=0;i<editarr.GetSize();i++)
		delete editarr.GetAt(i);
	editarr.RemoveAll();
}

void SP_EDIT_RETENTION::OnButtonView() 
{
	// TODO: Add your control notification handler code here
//Profile plot
	
}

void SP_EDIT_RETENTION::OnButtonViewLayers() 
{
	
	
}
