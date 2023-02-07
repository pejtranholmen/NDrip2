// SP_InitChar.cpp : implementation file
//

#include "..\stdafx.h"
#include "..\Util.h"
#include ".\SP_InitChar.h"
#include ".\SP_Save.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SP_InitChar dialog


SP_InitChar::SP_InitChar(SP_Save *pSP_Save, CWnd* pParent /*=NULL*/)
	: CDialog(SP_InitChar::IDD, pParent)
{
	m_pSP_Save=pSP_Save;
	//{{AFX_DATA_INIT(SP_InitChar)
	m_Clay = 10.0f;
	m_Sand = 20.0f;
	m_Saturation = 45.0f;
	m_UpperDepth = 0.0f;
	m_LowerDepth = 0.1f;
	//}}AFX_DATA_INIT

}


void SP_InitChar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SP_InitChar)
	DDX_Text(pDX, IDC_EDIT1, m_Clay);
	DDV_MinMaxFloat(pDX, m_Clay, 0.f, 100.f);
	DDX_Text(pDX, IDC_EDIT2, m_Sand);
	DDV_MinMaxFloat(pDX, m_Sand, 0.f, 100.f);
	DDX_Text(pDX, IDC_EDIT3, m_Saturation);
	DDV_MinMaxFloat(pDX, m_Saturation, 0.f, 100.f);
	DDX_Text(pDX, IDC_UDEP, m_UpperDepth);
	DDV_MinMaxFloat(pDX, m_UpperDepth, 0.f, 100.f);
	DDX_Text(pDX, IDC_LDEP, m_LowerDepth);
	DDV_MinMaxFloat(pDX, m_LowerDepth, 0.f, 100.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SP_InitChar, CDialog)
	//{{AFX_MSG_MAP(SP_InitChar)
	ON_BN_CLICKED(IDAPPLY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SP_InitChar message handlers

void SP_InitChar::OnApply() 
{
   	UpdateData(TRUE);
	if(m_pSP_Save->m_arrClay.GetSize()>int(m_pSP_Save->m_NumLayers)) {
		m_pSP_Save->m_arrClay.InsertAt(m_pSP_Save->m_NumLayers,&m_Clay);
		m_pSP_Save->m_arrSand.InsertAt(m_pSP_Save->m_NumLayers,&m_Sand);
		m_pSP_Save->m_arrSaturation.InsertAt(m_pSP_Save->m_NumLayers,&m_Saturation);
		m_pSP_Save->m_arrUpperDepth.InsertAt(m_pSP_Save->m_NumLayers,&m_UpperDepth);
		m_pSP_Save->m_arrLowerDepth.InsertAt(m_pSP_Save->m_NumLayers,&m_LowerDepth);
	}
	else if(m_pSP_Save->m_NumLayers>=1) {
			 float *clay=new float;
			 float *sand=new float;
			 float *saturation=new float;
			 float *udepth=new float;
			 float *ldepth=new float;
			 *clay=m_Clay;
			 *sand=m_Sand;
			 *saturation=m_Saturation;
			 *udepth=m_UpperDepth;
			 *ldepth=m_LowerDepth;
			 m_pSP_Save->m_arrUpperDepth.SetAtGrow(m_pSP_Save->m_NumLayers,udepth);
			 m_pSP_Save->m_arrLowerDepth.SetAtGrow(m_pSP_Save->m_NumLayers,ldepth);
			 m_pSP_Save->m_arrClay.SetAtGrow(m_pSP_Save->m_NumLayers,clay);
			 m_pSP_Save->m_arrSand.SetAtGrow(m_pSP_Save->m_NumLayers,sand);
			 m_pSP_Save->m_arrSaturation.SetAtGrow(m_pSP_Save->m_NumLayers,saturation);

   }
	else {
			 float *clay=new float;
			 float *sand=new float;
			 float *saturation=new float;
			 float *udepth=new float;
			 float *ldepth=new float;
			 *clay=m_Clay;
			 *sand=m_Sand;
			 *saturation=m_Saturation;
			 *udepth=m_UpperDepth;
			 *ldepth=m_LowerDepth;
			 m_pSP_Save->m_arrUpperDepth.SetAtGrow(m_pSP_Save->m_NumLayers,udepth);
			 m_pSP_Save->m_arrLowerDepth.SetAtGrow(m_pSP_Save->m_NumLayers,ldepth);
			 m_pSP_Save->m_arrClay.SetAtGrow(m_pSP_Save->m_NumLayers,clay);
			 m_pSP_Save->m_arrSand.SetAtGrow(m_pSP_Save->m_NumLayers,sand);
			 m_pSP_Save->m_arrSaturation.SetAtGrow(m_pSP_Save->m_NumLayers,saturation);
	}
	m_pSP_Save->m_NumLayers++;
	OnInitDialog();
	// TODO: Add your control notification handler code here
	
}

void SP_InitChar::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

BOOL SP_InitChar::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString string;
	string="Soil Characteristics for layer # ("+MFC_Util::ItoAscii(m_pSP_Save->m_NumLayers+1)+")";
	SetWindowText(string);
	if(m_pSP_Save->m_arrClay.GetSize()>int(m_pSP_Save->m_NumLayers)) {
   		m_UpperDepth=*(float*)m_pSP_Save->m_arrUpperDepth[m_pSP_Save->m_NumLayers];
   		m_LowerDepth=*(float*)m_pSP_Save->m_arrLowerDepth[m_pSP_Save->m_NumLayers];
		m_Clay=*(float*)m_pSP_Save->m_arrClay[m_pSP_Save->m_NumLayers];
		m_Sand=*(float*)m_pSP_Save->m_arrSand[m_pSP_Save->m_NumLayers];
		m_Saturation=*(float*)m_pSP_Save->m_arrSaturation[m_pSP_Save->m_NumLayers];
	}
	else {
		for(int i=0;i<m_pSP_Save->m_arrClay.GetSize();i++){
			m_UpperDepth=*(float*)m_pSP_Save->m_arrLowerDepth[i];
			m_LowerDepth=*(float*)m_pSP_Save->m_arrLowerDepth[i]+0.1f;
			m_Clay=*(float*)m_pSP_Save->m_arrClay[i];
			m_Sand=*(float*)m_pSP_Save->m_arrSand[i];
			m_Saturation=*(float*)m_pSP_Save->m_arrSaturation[i];
		}
	}
	CDialog::OnInitDialog();

	if(m_pSP_Save->m_arrClay.GetSize()>int(m_pSP_Save->m_NumLayers))
			GetDlgItem(IDAPPLY)->ModifyStyle(0,WS_DISABLED);
	else
			GetDlgItem(IDAPPLY)->ModifyStyle(WS_DISABLED,0);
	GetDlgItem(IDAPPLY)->RedrawWindow();




//	UpdateData(TRUE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
