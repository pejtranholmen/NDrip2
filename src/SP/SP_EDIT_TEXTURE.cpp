// SP_EDIT_TEXTURE.cpp : implementation file
//

#include "stdafx.h"
#include ".\CUG\MyCug.h"
#include "Base.h"
#include "CoupModel.h"
#include "CoupModelView.h"
#include "SP_EDIT_TEXTURE.h"
#include "datatrans.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" float PLOTPF_mp_TEXF[20][8];
extern "C" float PLOTPF_mp_BUDEP[22];
extern "C" float PLOTPF_mp_BLDEP[22];

extern "C" BOOL PLOTPF_mp_OPENDATABASE;
extern "C" void PLOTPF_mp_SAVEDATABASE();

extern "C" int PLOTPF_mp_ACTUALPROFILE;
extern "C" int PLOTPF_mp_ACTUALREPPROFILE;
extern "C" int PLOTPF_mp_ACTUALLEVEL;
extern "C" int PLOTPF_mp_ACTUALNUMLEVELS;

static double VarValues[1000], VarXValues[1000];
static int View_YAxisStyle[2]= {2,2};
static int View_YAxisTicks[2]= {10,10};
static double View_YAxisMin[2]= {0,0};
static double View_YAxisMax[2]= {100,100};
static int View_NumberOfTimePoints;
static int View_VariableNumber;


/////////////////////////////////////////////////////////////////////////////
// SP_EDIT_TEXTURE dialog


SP_EDIT_TEXTURE::SP_EDIT_TEXTURE(CObject *pView, CString ProfileName,CWnd* pParent /*=NULL*/)
	: CDialog(SP_EDIT_TEXTURE::IDD, pParent)
{
	for(int i=0;i<PLOTPF_mp_ACTUALNUMLEVELS;i++) 
		for(int j=0;j<8;j++) {
			if(j==0||j==7)
				m_Texture[i][j]=PLOTPF_mp_TEXF[i][j];
			else if(j<6) 
				m_Texture[i][j]=PLOTPF_mp_TEXF[i][j]-PLOTPF_mp_TEXF[i][j-1];
			else
				m_Texture[i][j]=100-PLOTPF_mp_TEXF[i][7]-PLOTPF_mp_TEXF[i][5];
		}
	m_pView=(CCoupModelView*)pView;
	m_ProfileName=ProfileName;

	//{{AFX_DATA_INIT(SP_EDIT_TEXTURE)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
SP_EDIT_TEXTURE::~SP_EDIT_TEXTURE()
{
	for(int i=0;i<statarr.GetSize();i++)
		delete statarr.GetAt(i);
	statarr.RemoveAll();
	for(i=0;i<editarr.GetSize();i++)
		delete editarr.GetAt(i);
	editarr.RemoveAll();


}

void SP_EDIT_TEXTURE::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SP_EDIT_TEXTURE)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SP_EDIT_TEXTURE, CDialog)
	//{{AFX_MSG_MAP(SP_EDIT_TEXTURE)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_VIEW, OnButtonView)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_LAYERS, OnButtonViewLayers)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SP_EDIT_TEXTURE message handlers

BOOL SP_EDIT_TEXTURE::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString str=m_ProfileName;
	str+=" - Particle size fractions and loss on ignition (%)";
	SetWindowText(str);
	CRect			koord(54,45,104,65); 
	CRect			parent(0,0,50,50);
	CEdit			*k;
	CStatic			*s;

	CString			text;
	

	for (int i=0;i<PLOTPF_mp_ACTUALNUMLEVELS;i++) {
// Number of layers
		CRect coord=koord;
		coord.left-=0;
		coord.top+=i*20+50;
		coord.right-=0;
		coord.bottom+=i*20+50;
		text=ItoAscii(i+1);
 		statarr.Add(new CStatic);
		s=statarr[i];
		s->Create(text, WS_CHILD | WS_VISIBLE ,coord,this,5000);
		s->SetFont(GetFont());

		for (int j=0;j<8;j++) {
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
			k->SetWindowText(FtoAscii(m_Texture[i][j]));
		}
	}

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void SP_EDIT_TEXTURE::OnOK() 
{	int count=0;
	CEdit  	*k;
	CString string;
	for(int i=0;i<PLOTPF_mp_ACTUALNUMLEVELS;i++) 
		for(int j=0;j<8;j++){
				k=editarr[count];
				count++;
				k->GetWindowText(string);
				m_Texture[i][j]=AtoFloat(string);
		}
	for(i=0;i<PLOTPF_mp_ACTUALNUMLEVELS;i++) 
		for(int j=0;j<8;j++) {
			if(j==0||j==7)
				PLOTPF_mp_TEXF[i][j]=m_Texture[i][j];
			else if(j<=6) 
				PLOTPF_mp_TEXF[i][j]=PLOTPF_mp_TEXF[i][j-1]+m_Texture[i][j];
		}

	PLOTPF_mp_SAVEDATABASE();

	CDialog::OnOK();
}

void SP_EDIT_TEXTURE::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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

void SP_EDIT_TEXTURE::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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

void SP_EDIT_TEXTURE::OnButtonView() 
{
//Profile plot

		
}

void SP_EDIT_TEXTURE::OnButtonViewLayers() 
{
	// TODO: Add your control notification handler code here
	

}
