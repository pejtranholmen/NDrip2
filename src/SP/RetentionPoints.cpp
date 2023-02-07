// RetentionPoints.cpp : implementation file
//

#include "..\stdafx.h"
#include "..\Util.h"
#include "..\CoupModel.h"
#include ".\RetentionPoints.h"
#include ".\SP_Estimate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*extern "C" int PLOTPF_mp_NT;
extern "C" float PLOTPF_mp_MODPRESSURE[12];
extern "C" float PLOTPF_mp_MODTHETA[12];*/

/////////////////////////////////////////////////////////////////////////////
// RetentionPoints dialog


RetentionPoints::RetentionPoints(SP_Estimate* pSP_est, CWnd* pParent /*=NULL*/)
	: CDialog(RetentionPoints::IDD, pParent)
{
	m_pSP_Est=pSP_est;
	//{{AFX_DATA_INIT(RetentionPoints)
	m_Pressure = 5.0f;
	m_Theta = 60.0f;
	//}}AFX_DATA_INIT
}


void RetentionPoints::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RetentionPoints)
	DDX_Text(pDX, IDC_EDIT1, m_Pressure);
	DDV_MinMaxFloat(pDX, m_Pressure, 0.f, 15000.f);
	DDX_Text(pDX, IDC_EDIT2, m_Theta);
	DDV_MinMaxFloat(pDX, m_Theta, 0.1f, 96.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RetentionPoints, CDialog)
	//{{AFX_MSG_MAP(RetentionPoints)
	ON_BN_CLICKED(IDAPPLY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RetentionPoints message handlers

BOOL RetentionPoints::OnInitDialog() 
{
	CString string;
	string="Water Retention points # ("+MFC_Util::ItoAscii(m_pSP_Est->m_Num_pF_curve+1)+")";
	SetWindowText(string);
	if(m_pSP_Est->m_arrPressure.size()>m_pSP_Est->m_Num_pF_curve) {
		m_Pressure=m_pSP_Est->m_arrPressure[m_pSP_Est->m_Num_pF_curve];
		m_Theta=m_pSP_Est->m_arrTheta[m_pSP_Est->m_Num_pF_curve];
	}
	else {
		for(int i=0;i<m_pSP_Est->m_arrPressure.size();i++){
			if(m_Pressure<m_pSP_Est->m_arrPressure[i])
				m_Pressure=m_pSP_Est->m_arrPressure[i];
			if(m_Theta>m_pSP_Est->m_arrTheta[i])
				m_Theta=m_pSP_Est->m_arrTheta[i];
		}
	}
	CDialog::OnInitDialog();

	if(m_pSP_Est->m_arrPressure.size()>m_pSP_Est->m_Num_pF_curve)
			GetDlgItem(IDAPPLY)->ModifyStyle(0,WS_DISABLED);
	else
			GetDlgItem(IDAPPLY)->ModifyStyle(WS_DISABLED,0);
	GetDlgItem(IDAPPLY)->RedrawWindow();




//	UpdateData(TRUE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void RetentionPoints::OnOK() 
{
	// TODO: Add extra validation here
	m_pSP_Est->m_Num_pF_curve++;
	CDialog::OnOK();
}

void RetentionPoints::OnApply() 
{
	UpdateData(TRUE);
	if(m_pSP_Est->m_arrPressure.size()>m_pSP_Est->m_Num_pF_curve) {
		m_pSP_Est->m_arrPressure[m_pSP_Est->m_Num_pF_curve]=m_Pressure;
		m_pSP_Est->m_arrTheta[m_pSP_Est->m_Num_pF_curve]=m_Theta;
	}
	else if(m_pSP_Est->m_Num_pF_curve>=1) {
		   if(m_pSP_Est->m_arrPressure[m_pSP_Est->m_Num_pF_curve-1]!=m_Pressure||
			   m_pSP_Est->m_arrTheta[m_pSP_Est->m_Num_pF_curve-1]!=m_Theta) {
				 m_pSP_Est->m_arrPressure[m_pSP_Est->m_Num_pF_curve]=m_Pressure;
				 m_pSP_Est->m_arrTheta[m_pSP_Est->m_Num_pF_curve]=m_Theta;
		   }
		   else
			 m_pSP_Est->m_Num_pF_curve--;
	}
	else {
		if(m_pSP_Est->m_Num_pF_curve>m_pSP_Est->m_arrPressure.size()) {
			m_pSP_Est->m_arrPressure.resize(m_pSP_Est->m_Num_pF_curve);
			m_pSP_Est->m_arrTheta.resize(m_pSP_Est->m_Num_pF_curve);
		}
		m_pSP_Est->m_arrPressure[m_pSP_Est->m_Num_pF_curve]=m_Pressure;
		m_pSP_Est->m_arrTheta[m_pSP_Est->m_Num_pF_curve]=m_Theta;		
		
	}
	m_pSP_Est->m_Num_pF_curve++;
	//PLOTPF_mp_NT=m_pSP_Est->m_Num_pF_curve;
	OnInitDialog();
}
