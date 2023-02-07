// SP_EDIT_COEF.cpp : implementation file
//

#include "stdafx.h"
#include "Base.h"
#include "CoupModel.h"
#include "SP_EDIT_COEF.h"
#include "datatrans.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" float PLOTPF_mp_ALAMDA[22];
extern "C" float PLOTPF_mp_APSIE[22];
extern "C" float PLOTPF_mp_APORO[22];
extern "C" float PLOTPF_mp_AWILT[22];
extern "C" float PLOTPF_mp_ATETRE[22];
extern "C" float PLOTPF_mp_BLB[22];
extern "C" float PLOTPF_mp_BUB[22];
extern "C" float PLOTPF_mp_BBSAT[22];
extern "C" float PLOTPF_mp_BSATC[22];
extern "C" float PLOTPF_mp_BNVAR[22];
extern "C" float PLOTPF_mp_BNSR[22];
extern "C" float PLOTPF_mp_BNSE[22];
extern "C" float PLOTPF_mp_BUDEP[22];
extern "C" float PLOTPF_mp_BLDEP[22];
extern "C" float PLOTPF_mp_BBGN[22];
extern "C" float PLOTPF_mp_BBGM[22];
extern "C" float PLOTPF_mp_BBGA[22];
extern "C" float PLOTPF_mp_BGN[22];
extern "C" float PLOTPF_mp_BGM[22];
extern "C" float PLOTPF_mp_BGA[22];


extern "C" BOOL PLOTPF_mp_OPENDATABASE;
extern "C" void PLOTPF_mp_SAVEDATABASE();

extern "C" int PLOTPF_mp_ACTUALPROFILE;
extern "C" int PLOTPF_mp_ACTUALREPPROFILE;
extern "C" int PLOTPF_mp_ACTUALLEVEL;
extern "C" int PLOTPF_mp_ACTUALNUMLEVELS;


/////////////////////////////////////////////////////////////////////////////
// SP_EDIT_COEF dialog


SP_EDIT_COEF::SP_EDIT_COEF(CWnd* pParent /*=NULL*/)
	: CDialog(SP_EDIT_COEF::IDD, pParent)
{
	for(int i=0;i<PLOTPF_mp_ACTUALNUMLEVELS;i++) {
		m_Coef[i][0] =	PLOTPF_mp_BUDEP[i]  ;
		m_Coef[i][1] =	PLOTPF_mp_BLDEP[i]  ;
		m_Coef[i][2] =	PLOTPF_mp_ALAMDA[i];
		m_Coef[i][3] =	PLOTPF_mp_APSIE[i]	;
		m_Coef[i][4] =	PLOTPF_mp_APORO[i]	;
		m_Coef[i][5] =	PLOTPF_mp_AWILT[i]	;
		m_Coef[i][6] =	PLOTPF_mp_ATETRE[i]	;
		m_Coef[i][7] =	PLOTPF_mp_BLB[i]	;
		m_Coef[i][8] =	PLOTPF_mp_BUB[i]	;
		m_Coef[i][9]=	PLOTPF_mp_BBSAT[i]	;
		m_Coef[i][10]=	PLOTPF_mp_BSATC[i]	;
		m_Coef[i][11]=	PLOTPF_mp_BNVAR[i]	;
		m_Coef[i][14]=	PLOTPF_mp_BGM[i]	;
		m_Coef[i][15]=	PLOTPF_mp_BGN[i]	;
		m_Coef[i][16]=	PLOTPF_mp_BGA[i]	;
		m_Coef[i][12]=	PLOTPF_mp_BNSR[i]	;
		m_Coef[i][13]=	PLOTPF_mp_BNSE[i]	;
	}			 
	
	//{{AFX_DATA_INIT(SP_EDIT_COEF)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}
SP_EDIT_COEF::~SP_EDIT_COEF()
{
	for(int i=0;i<statarr.GetSize();i++)
		delete statarr.GetAt(i);
	statarr.RemoveAll();
	for(i=0;i<editarr.GetSize();i++)
		delete editarr.GetAt(i);
	editarr.RemoveAll();


}



void SP_EDIT_COEF::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SP_EDIT_COEF)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SP_EDIT_COEF, CDialog)
	//{{AFX_MSG_MAP(SP_EDIT_COEF)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SP_EDIT_COEF message handlers

BOOL SP_EDIT_COEF::OnInitDialog() 
{
	CDialog::OnInitDialog();
	SetWindowText("Coefficients for the water retention and the hydraulic conductivity functions");
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

		for (int j=0;j<17;j++) {
// Floating point values
			CRect coord=koord;
			coord.left+=20+j*50;
			coord.top+=i*20+50;
			coord.right+=20+j*50;
			coord.bottom+=i*20+50;
			SetScrollRange(SB_VERT,0,coord.bottom-250);
			SetScrollRange(SB_HORZ,0,coord.right-750);
		   	editarr.Add(new CEdit);
	
			int filos=editarr.GetSize();
			k=editarr[editarr.GetSize()-1];
			k->Create(ES_RIGHT | WS_CHILD | WS_VISIBLE | WS_BORDER |
					WS_TABSTOP | ES_AUTOHSCROLL,coord,this,4000);
			k->ModifyStyleEx(0,WS_EX_CLIENTEDGE);
			k->SetFont(GetFont());
			k->SetWindowText(FtoAscii(m_Coef[i][j]));
		}
	}

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
	
void SP_EDIT_COEF::OnOK() 
{	int count=0;
	CEdit  	*k;
	CString string;
	for(int i=0;i<PLOTPF_mp_ACTUALNUMLEVELS;i++) {
		for(int j=0;j<17;j++){
			k=editarr[count];
			count++;
			k->GetWindowText(string);
			m_Coef[i][j]=AtoFloat(string);
		}

		PLOTPF_mp_BUDEP[i]   =m_Coef[i][0] 	;
		PLOTPF_mp_BLDEP[i]	 =m_Coef[i][1] 	;
		PLOTPF_mp_ALAMDA[i]	 =m_Coef[i][2] 	;
		PLOTPF_mp_APSIE[i]	 =m_Coef[i][3] 	;
		PLOTPF_mp_APORO[i]	 =m_Coef[i][4] 	;
		PLOTPF_mp_AWILT[i]	 =m_Coef[i][5] 	;
		PLOTPF_mp_ATETRE[i]	 =m_Coef[i][6] 	;
		PLOTPF_mp_BLB[i]	 =m_Coef[i][7] 	;
		PLOTPF_mp_BUB[i]	 =m_Coef[i][8] 	;
		PLOTPF_mp_BBSAT[i]	 =m_Coef[i][9]	;
		PLOTPF_mp_BSATC[i]	 =m_Coef[i][10]	;
		PLOTPF_mp_BNVAR[i]	 =m_Coef[i][11]	;
		PLOTPF_mp_BBGN[i]	 =m_Coef[i][15]	;
		PLOTPF_mp_BBGM[i]	 =m_Coef[i][14]	;
		PLOTPF_mp_BBGA[i]	 =m_Coef[i][16]	;
		PLOTPF_mp_BGN[i]	 =m_Coef[i][15]	;
		PLOTPF_mp_BGM[i]	 =m_Coef[i][14]	;
		PLOTPF_mp_BGA[i]	 =m_Coef[i][16]	;
		PLOTPF_mp_BNSR[i]	 =m_Coef[i][12]	;
		PLOTPF_mp_BNSE[i]	 =m_Coef[i][13]	;
		}

	PLOTPF_mp_SAVEDATABASE();

	CDialog::OnOK();
}


void SP_EDIT_COEF::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
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

void SP_EDIT_COEF::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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
