// SP_Estimate.cpp : implementation file
//

#include "stdafx.h"
#include "CoupModel.h"
#include "Base.h"
#include "SP_Estimate.h"
#include "FortranMap.h"
#include "RetentionPoints.h"
#include "CoupModelView.h"
#include "SP_Retrieve.h"
#include "SP_Save.h"
#include ".\sp_estimate.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern "C" float  HYDRAULIC_mp_ESATC(int *);
extern "C" float   HYDRAULIC_mp_ETETRE(int *);
extern "C" float   HYDRAULIC_mp_EPSIE(int *);
extern "C" float   HYDRAULIC_mp_ELAMBDA(int *);
extern "C" float   HYDRAULIC_mp_EWILT(int *);


extern "C" int   HYDRAULIC_mp_ESTIMATEBC(float (*)[25], float (*)[25], int*, float *);
extern "C" float HYDRAULIC_mp_AIRENTRY_EST;
extern "C" float HYDRAULIC_mp_R2_EST;
extern "C" float HYDRAULIC_mp_RESIDUAL_EST;
extern "C" float HYDRAULIC_mp_LAMBDA_EST;

extern "C" void   PLOTPF_mp_GETLAYER(int *, int *, int*);
extern "C" void   PLOTPF_mp_RETRIEVECOEF(int*);
extern "C" void   PLOTPF_mp_RETRIEVEPF_POINTS();
extern "C" void   PLOTPF_mp_LOADPF_POINTS();
extern "C" void PLOTPF_mp_LOADTEXTURE();
extern "C" void   PLOTPF_mp_RETRIEVETEXTURE();
extern "C" void   PLOTPF_mp_OPENBASE();
extern "C" void   PLOTPF_mp_SAVEDATABASE();
extern "C" BOOL PLOTPF_mp_OPENDATABASE;
extern "C" BOOL PLOTPF_mp_FINDPROFILE;
extern "C" BOOL PLOTPF_mp_FINDLAYER;
extern "C" BOOL PLOTPF_mp_SAVED;
extern "C" int PLOTPF_mp_ACTUALPROFILE;
extern "C" int PLOTPF_mp_ACTUALREPPROFILE;
extern "C" int PLOTPF_mp_ACTUALLEVEL;
extern "C" int PLOTPF_mp_NT;
extern "C" float PLOTPF_mp_MODPRESSURE[12];
extern "C" float PLOTPF_mp_MODTHETA[12];
extern "C" float PLOTPF_mp_MODTEXTURE[8];
extern "C" float PLOTPF_mp_APORO[22];
extern "C" char PLOTPF_mp_DKEY[6];
extern "C" char PLOTPF_mp_RPLACE[16];


/////////////////////////////////////////////////////////////////////////////
// SP_Estimate dialog


SP_Estimate::SP_Estimate(CTab *PointerToTable,int layer, CCoupModelDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(SP_Estimate::IDD, pParent)
{
	m_Listed_Count=0;
	m_Estimated_Count=0;
	m_Num_pF_curve=0;
	m_Layer=layer;
	m_pDoc=pDoc;
	//{{AFX_DATA_INIT(SP_Estimate)
	m_Clay = 0.0f;
	m_Sand = 0.0f;
	m_Porosity = 0.0f;
	m_EstimateMin = 1.0f;
	m_EstimateMax = 4000.0f;
	m_charProfNumber = _T("");
	m_charProfName = _T("");
	m_charProfLayer = _T("");
	m_tex2 = 0.0f;
	m_tex3 = 0.0f;
	m_tex4 = 0.0f;
	m_tex5 = 0.0f;
	m_tex6 = 0.0f;
	m_tex7 = 0.0f;
	m_org = 0.0f;
	m_charCUM1 = _T("");
	m_charCUM2 = _T("");
	m_charCUM3 = _T("");
	m_charCUM4 = _T("");
	m_charCUM5 = _T("");
	m_charCUM6 = _T("");
	m_charCUM7 = _T("");
	m_charCUM8 = _T("");
	m_charClay = _T("");
	//}}AFX_DATA_INIT
	if(PointerToTable->GetName()=="Brooks-Corey, water retention, measured horizons") {
		m_pCTab=PointerToTable;	
	  m_Retention=TRUE	;
	}
	else
	  m_Retention=FALSE;
	if(m_Retention) {
		m_Static1="\nLambda";
		m_Static2="\nAir Entry";
		m_Static3="Wilting Point";
		m_Static4="Residual Water";
	}
	else {
		m_Static1="Saturated\nConductivity";
		m_Static2="";
		m_Static3="";
		m_Static4="";

	}
	CFortranMap *p_fmap;
	p_fmap=pDoc->GetDocFortranMap();

	CBasePtrArray *arr;
	CString Name, Group, Type;

	Name="ClayContent";
	Group="Soil Hydraulic";
	Type="Table Parameters";
    pClayContent=(CPt*)p_fmap->reg.GetPtr(Name,Type,Group, arr);
	Name="SandContent";
    pSandContent=(CPt*)p_fmap->reg.GetPtr(Name,Type,Group, arr);
   	Name="ProfileNo";
    pProf=(CPt*)p_fmap->reg.GetPtr(Name,Type,Group, arr);
	Name="ProfileRepNo";
    pRepProf=(CPt*)p_fmap->reg.GetPtr(Name,Type,Group, arr);
	Name="ProfileLayer";
    pProfLayer=(CPt*)p_fmap->reg.GetPtr(Name,Type,Group, arr);

   
	
	Name="Saturation";
    pSaturation=(CPt*)p_fmap->reg.GetPtr(Name,Type,Group, arr);
	Name="Total Conductivity";
    pTotalCond=(CPt*)p_fmap->reg.GetPtr(Name,Type,Group, arr);
	Name="Matrix Conductivity";
    pMatCond=(CPt*)p_fmap->reg.GetPtr(Name,Type,Group, arr);


	Name="Lambda";
    pLambda=(CPt*)p_fmap->reg.GetPtr(Name,Type,Group, arr);
	Name="Air Entry";
    pAirEntry=(CPt*)p_fmap->reg.GetPtr(Name,Type,Group, arr);
	Name="Wilting Point";
    pWilt=(CPt*)p_fmap->reg.GetPtr(Name,Type,Group, arr);
	Name="Residual Water";
    pResidual=(CPt*)p_fmap->reg.GetPtr(Name,Type,Group, arr);

   	float v;
	v=pProf->GetValue(m_Layer-1);
	m_Prof=int(v);
	m_Sand=pProf->GetValue(m_Layer-1);
	v=pRepProf->GetValue(m_Layer-1);
	m_RepProf=int(v);
	v=pProfLayer->GetValue(m_Layer-1);
	m_ProfLayer=int(v);




	m_Sand=pSandContent->GetValue(m_Layer-1);
	m_Clay=pClayContent->GetValue(m_Layer-1);
	m_Porosity=pSaturation->GetValue(m_Layer-1);

	m_Lambda=pLambda->GetValue(m_Layer-1);
	m_AirEntry=pAirEntry->GetValue(m_Layer-1);
	m_Residual=pResidual->GetValue(m_Layer-1);
	m_Wilt=pWilt->GetValue(m_Layer-1);
	m_SatCond=pTotalCond->GetValue(m_Layer-1);

}


void SP_Estimate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SP_Estimate)
	DDX_Control(pDX, IDC_LIST_PF, m_Measured_pF);
	DDX_Control(pDX, IDC_LIST1, m_Estimated_Coef);
	DDX_Text(pDX, IDC_EDIT1, m_Clay);
	DDV_MinMaxFloat(pDX, m_Clay, 0.f, 100.f);
	DDX_Text(pDX, IDC_EDIT3, m_Sand);
	DDX_Text(pDX, IDC_EDIT4, m_Porosity);
	DDX_Text(pDX, IDC_STATIC1, m_Static1);
	DDX_Text(pDX, IDC_STATIC2, m_Static2);
	DDX_Text(pDX, IDC_STATIC3, m_Static3);
	DDX_Text(pDX, IDC_STATIC4, m_Static4);
	DDX_Text(pDX, IDC_EDIT_MIN, m_EstimateMin);
	DDV_MinMaxFloat(pDX, m_EstimateMin, 0.1f, 1000.f);
	DDX_Text(pDX, IDC_EDIT_MAX, m_EstimateMax);
	DDV_MinMaxFloat(pDX, m_EstimateMax, 100.f, 600000.f);
	DDX_Text(pDX, IDC_Prof_num, m_charProfNumber);
	DDX_Text(pDX, IDC_Prof_Name, m_charProfName);
	DDX_Text(pDX, IDC_Prof_Layer, m_charProfLayer);
	DDX_Text(pDX, IDC_EDIT5, m_tex2);
	DDV_MinMaxFloat(pDX, m_tex2, 0.f, 100.f);
	DDX_Text(pDX, IDC_EDIT6, m_tex3);
	DDX_Text(pDX, IDC_EDIT7, m_tex4);
	DDV_MinMaxFloat(pDX, m_tex4, 0.f, 100.f);
	DDX_Text(pDX, IDC_EDIT8, m_tex5);
	DDV_MinMaxFloat(pDX, m_tex5, 0.f, 100.f);
	DDX_Text(pDX, IDC_EDIT9, m_tex6);
	DDX_Text(pDX, IDC_EDIT10, m_tex7);
	DDV_MinMaxFloat(pDX, m_tex7, 0.f, 100.f);
	DDX_Text(pDX, IDC_EDIT11, m_org);
	DDV_MinMaxFloat(pDX, m_org, 0.f, 100.f);
	DDX_Text(pDX, IDC_CUM1, m_charCUM1);
	DDV_MaxChars(pDX, m_charCUM1, 5);
	DDX_Text(pDX, IDC_CUM2, m_charCUM2);
	DDV_MaxChars(pDX, m_charCUM2, 5);
	DDX_Text(pDX, IDC_CUM3, m_charCUM3);
	DDV_MaxChars(pDX, m_charCUM3, 5);
	DDX_Text(pDX, IDC_CUM4, m_charCUM4);
	DDV_MaxChars(pDX, m_charCUM4, 5);
	DDX_Text(pDX, IDC_CUM5, m_charCUM5);
	DDV_MaxChars(pDX, m_charCUM5, 5);
	DDX_Text(pDX, IDC_CUM6, m_charCUM6);
	DDV_MaxChars(pDX, m_charCUM6, 5);
	DDX_Text(pDX, IDC_CUM7, m_charCUM7);
	DDV_MaxChars(pDX, m_charCUM7, 5);
	DDX_Text(pDX, IDC_CUM8, m_charCUM8);
	DDV_MaxChars(pDX, m_charCUM8, 5);
	DDX_Text(pDX, IDC_STATIC_CLAY, m_charClay);
	DDV_MaxChars(pDX, m_charClay, 4);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SP_Estimate, CDialog)
	//{{AFX_MSG_MAP(SP_Estimate)
	ON_BN_CLICKED(ID_ESTIMATE, OnEstimate)
	ON_BN_CLICKED(IDC_ESTIMATE_PF, OnEstimatePf)
	ON_BN_CLICKED(IDC_EDIT_RETENTION, OnEditRetention)
	ON_BN_CLICKED(IDC_EDIT_RETENTION_EDIT, OnEditRetentionEdit)
	ON_BN_CLICKED(IDC_EDIT_RETENTION_RESET, OnEditRetentionReset)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_RETRIEVE, OnRetrieve)
	ON_BN_CLICKED(IDAPPLY2, OnApply2)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_ESTIMATE3, OnBnClickedEstimate3)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SP_Estimate message handlers


void SP_Estimate::OnEstimate() 
{
	// TODO: Add your control notification handler code here
  
	UpdateData(TRUE);
	OnApply2();
	pSandContent->SetValue(m_Layer-1,m_Sand);
	pClayContent->SetValue(m_Layer-1,m_Clay);
	pSaturation->SetValue(m_Layer-1,m_Porosity);
	pSandContent->Apply(m_Layer);
	pClayContent->Apply(m_Layer);
	pSaturation->Apply(m_Layer);
	m_Sand=m_Sand;
	m_Clay=m_Clay;
	m_Porosity=m_Porosity/100.f;
	m_SatCond=HYDRAULIC_mp_ESATC(&m_Layer);
	m_Residual=HYDRAULIC_mp_ETETRE(&m_Layer);
	m_AirEntry=HYDRAULIC_mp_EPSIE(&m_Layer);
	m_Lambda=HYDRAULIC_mp_ELAMBDA(&m_Layer);
	m_Wilt=HYDRAULIC_mp_EWILT(&m_Layer);

	m_Porosity=m_Porosity*100.f;
	m_Estimated_Count++;
	m_Estimated_pF=FALSE;

	OnInitDialog();
	
}

void SP_Estimate::OnOK() 
{
	// TODO: Add extra validation here
	if(m_Retention) {
		pResidual->SetValue(m_Layer-1, m_Residual);
		pResidual->Apply(m_Layer);
		pAirEntry->SetValue(m_Layer-1, m_AirEntry);
		pAirEntry->Apply(m_Layer);
		pLambda->SetValue(m_Layer-1, m_Lambda);
		pLambda->Apply(m_Layer);
		pWilt->SetValue(m_Layer-1,m_Wilt);
		pWilt->Apply(m_Layer);
	}
	else {
		pTotalCond->SetValue(m_Layer-1, m_SatCond);
		pTotalCond->Apply(m_Layer);
		if(pMatCond->GetValue(m_Layer-1)>m_SatCond) {
			pMatCond->SetValue(m_Layer-1,m_SatCond);  
			pMatCond->Apply(m_Layer);  
		}
		else if(pMatCond->GetValue(m_Layer-1)<0.01*m_SatCond) {
			pMatCond->SetValue(m_Layer-1,0.01f*m_SatCond);  
			pMatCond->Apply(m_Layer);  
		}
	}
	
	CDialog::OnOK();
}


BOOL SP_Estimate::OnInitDialog() 
{
	CString String;	
	if(!PLOTPF_mp_OPENDATABASE) PLOTPF_mp_OPENBASE();
	PLOTPF_mp_GETLAYER(&m_Prof,&m_RepProf,&m_ProfLayer);
	
	if(PLOTPF_mp_FINDLAYER) {
		PLOTPF_mp_RETRIEVEPF_POINTS();
		PLOTPF_mp_RETRIEVETEXTURE();
		PLOTPF_mp_RETRIEVECOEF(&m_Layer-1);
		pSaturation->SetValue(m_Layer-1,PLOTPF_mp_APORO[PLOTPF_mp_ACTUALLEVEL-1]);
		pSaturation->Apply(m_Layer);
		m_Porosity=pSaturation->GetValue(m_Layer-1);

		m_charProfLayer="Layer: "+ItoAscii(PLOTPF_mp_ACTUALLEVEL);
		m_charProfName="Site name: ";
		for(int i=0;i<16;i++) 
			m_charProfName+=PLOTPF_mp_RPLACE[i];
		m_charProfNumber="Site no: ";
		for(i=0;i<6;i++) 
			m_charProfNumber+=PLOTPF_mp_DKEY[i];
		m_charClay=FtoAscii(PLOTPF_mp_MODTEXTURE[0]);m_charClay.Mid(0,4);
		m_charClay=m_charClay.Mid(0,4);
		for( i=0;i<6;i++) {
			if(PLOTPF_mp_MODTEXTURE[i]>100) {
				PLOTPF_mp_MODTEXTURE[i]=100;
				PLOTPF_mp_MODTEXTURE[7]=0;
			}
		}

       m_tex2=PLOTPF_mp_MODTEXTURE[1]-PLOTPF_mp_MODTEXTURE[0];
	    m_tex3=PLOTPF_mp_MODTEXTURE[2]-PLOTPF_mp_MODTEXTURE[1];
	    m_tex4=PLOTPF_mp_MODTEXTURE[3]-PLOTPF_mp_MODTEXTURE[2];
	    m_tex5=PLOTPF_mp_MODTEXTURE[4]-PLOTPF_mp_MODTEXTURE[3];
	    m_tex6=PLOTPF_mp_MODTEXTURE[5]-PLOTPF_mp_MODTEXTURE[4];
	    m_tex7=100.f-PLOTPF_mp_MODTEXTURE[7]-PLOTPF_mp_MODTEXTURE[5];
		if(m_tex7<0) {
			float diff=-1*m_tex7;
			m_tex7=m_tex7+diff;
			PLOTPF_mp_MODTEXTURE[7]=PLOTPF_mp_MODTEXTURE[7]-diff;
		}
	    m_org=PLOTPF_mp_MODTEXTURE[7];
		m_Clay=PLOTPF_mp_MODTEXTURE[0];
		m_Sand=(100.f-PLOTPF_mp_MODTEXTURE[7]-PLOTPF_mp_MODTEXTURE[3]);
		m_charCUM1=m_charClay;
		m_charCUM2=FtoAscii(PLOTPF_mp_MODTEXTURE[1]);		m_charCUM2=m_charCUM2.Mid(0,4);
		m_charCUM3=FtoAscii(PLOTPF_mp_MODTEXTURE[2]);		m_charCUM3=m_charCUM3.Mid(0,4);
		m_charCUM4=FtoAscii(PLOTPF_mp_MODTEXTURE[3]);		m_charCUM4=m_charCUM4.Mid(0,4);
		m_charCUM5=FtoAscii(PLOTPF_mp_MODTEXTURE[4]);		m_charCUM5=m_charCUM5.Mid(0,4);
		m_charCUM6=FtoAscii(PLOTPF_mp_MODTEXTURE[5]);		m_charCUM6=m_charCUM6.Mid(0,4);
		m_charCUM7=FtoAscii(100.f-PLOTPF_mp_MODTEXTURE[7]);	m_charCUM7=m_charCUM7.Mid(0,4);
		m_charCUM8=FtoAscii(100.);							m_charCUM8=m_charCUM8.Mid(0,4);

		m_arrTheta.RemoveAll();
		m_arrPressure.RemoveAll();
		for(i=0;i<PLOTPF_mp_NT;i++){
  			m_arrPressure.SetAtGrow(i,&PLOTPF_mp_MODPRESSURE[i] );
			m_arrTheta.SetAtGrow(i,&PLOTPF_mp_MODTHETA[i]);
		}



	}
	else {
		m_charProfLayer="";
		m_charProfName="";
		m_charProfNumber="";
	}

	CDialog::OnInitDialog();
	String="Estimate coefficients for layer #: ";
	String+=ItoAscii(m_Layer);
	SetWindowText(String);

     m_Estimated_Coef.SetTabStops(44);
  if(m_Estimated_Count>=m_Listed_Count) {
	  if(m_Listed_Count==0) {
		  if(m_Retention)
			String="Current values\t\t\t"+
				FtoAscii(m_Lambda)+"\t"+FtoAscii(m_AirEntry)+"\t"+
				  FtoAscii(m_Wilt)+"\t"+FtoAscii(m_Residual);
		  else
			String="Current values\t\t\t"+FtoAscii(m_SatCond);
	  }
	  else if(m_Estimated_pF) {
		  String=FtoAscii(m_Clay)+"\t"+FtoAscii(m_Sand)+"\t"+FtoAscii(m_Porosity)+"\t"+FtoAscii(HYDRAULIC_mp_R2_EST)+"("+ItoAscii(m_NumEstimated)+")"+"\t"+
				FtoAscii(m_Lambda)+"\t"+FtoAscii(m_AirEntry)+"\t"+
				  FtoAscii(m_Wilt)+"\t"+FtoAscii(m_Residual);
	  }
	  else {
		  if(m_Retention) 
			String=FtoAscii(m_Clay)+"\t"+FtoAscii(m_Sand)+"\t"+FtoAscii(m_Porosity)+"\t"+"-"+"\t"+
				FtoAscii(m_Lambda)+"\t"+FtoAscii(m_AirEntry)+"\t"+
				  FtoAscii(m_Wilt)+"\t"+FtoAscii(m_Residual);
		  else
			String=FtoAscii(m_Clay)+"\t"+FtoAscii(m_Sand)+"\t"+FtoAscii(m_Porosity)+"\t"+"-"+"\t"+
				FtoAscii(m_SatCond);
	  }

	m_Estimated_Coef.AddString(String);
	m_Listed_Count++;
  }


    m_Measured_pF.SetTabStops(38);
	m_Num_pF_curve=m_arrPressure.GetSize();
	m_Measured_pF.ResetContent();
	for (int i=0;i<m_Num_pF_curve;i++){
		String=FtoAscii(*(float*)m_arrPressure[i])+"\t"+FtoAscii(*(float*)m_arrTheta[i]);
		m_Measured_pF.AddString(String);

	}

	if(m_Num_pF_curve>0) {
		m_Measured_pF.SetCurSel(m_Num_pF_curve-1);
	}

//

	 if(m_Estimated_Count>0)
			GetDlgItem(IDOK)->ModifyStyle(WS_DISABLED,0);
		else
			GetDlgItem(IDOK)->ModifyStyle(0,WS_DISABLED);
	 GetDlgItem(IDOK)->RedrawWindow();

	 if(m_Num_pF_curve>0) {
			GetDlgItem(IDC_ESTIMATE_PF)->ModifyStyle(WS_DISABLED,0);
			GetDlgItem(IDC_EDIT_RETENTION_EDIT)->ModifyStyle(WS_DISABLED,0);
			GetDlgItem(IDC_EDIT_RETENTION_RESET)->ModifyStyle(WS_DISABLED,0);
	 }
	 else	{
			GetDlgItem(IDC_ESTIMATE_PF)->ModifyStyle(0,WS_DISABLED);
			GetDlgItem(IDC_EDIT_RETENTION_EDIT)->ModifyStyle(0,WS_DISABLED);
			GetDlgItem(IDC_EDIT_RETENTION_RESET)->ModifyStyle(0,WS_DISABLED);
	 }
	 if(!m_Retention) {
	    GetDlgItem(IDC_ESTIMATE_PF)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT_RETENTION_EDIT)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT_RETENTION_RESET)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT_RETENTION)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT_MIN)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT_MAX)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_LIST_PF)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_STATIC8)->ModifyStyle(0,WS_DISABLED);

	 }
	 GetDlgItem(IDC_ESTIMATE_PF)->RedrawWindow();
	 GetDlgItem(IDC_EDIT_RETENTION_EDIT)->RedrawWindow();
	 GetDlgItem(IDC_EDIT_RETENTION_RESET)->RedrawWindow();
	 GetDlgItem(IDC_EDIT_RETENTION)->RedrawWindow();
	 GetDlgItem(IDC_EDIT_MIN)->RedrawWindow();
	 GetDlgItem(IDC_EDIT_MAX)->RedrawWindow();
	 GetDlgItem(IDC_LIST_PF)->RedrawWindow();
	 GetDlgItem(IDC_STATIC8)->RedrawWindow();

// IDC_SAVE used
//	 GetDlgItem(IDC_SAVE)->ModifyStyle(0,WS_DISABLED);
//	 GetDlgItem(IDC_SAVE)->RedrawWindow();
// IDC_RETRIEVE  used
//	 GetDlgItem(IDC_RETRIEVE)->ModifyStyle(0,WS_DISABLED);
//	 GetDlgItem(IDC_RETRIEVE)->RedrawWindow();


	UpdateData(TRUE);
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SP_Estimate::OnEstimatePf() 
{	float Theta[25], Psi[25];
	UpdateData(TRUE);
	m_NumEstimated=0;
	m_Num_pF_curve=m_arrPressure.GetSize();
	for(int i=0;i<m_Num_pF_curve;i++) {
		if(*(float*)m_arrPressure[i]>=m_EstimateMin&&
			*(float*)m_arrPressure[i]<=m_EstimateMax&&
			*(float*)m_arrTheta[i]>0.001) {
				Theta[m_NumEstimated]=*(float*)m_arrTheta[i];
				Psi[m_NumEstimated]=*(float*)m_arrPressure[i];
				m_NumEstimated++;
			}
	}
	if(m_NumEstimated>1) {
		int i=HYDRAULIC_mp_ESTIMATEBC(&Theta, &Psi, &m_NumEstimated, &m_Porosity);
		m_Lambda=HYDRAULIC_mp_LAMBDA_EST;
		m_AirEntry=HYDRAULIC_mp_AIRENTRY_EST;
		m_Residual=HYDRAULIC_mp_RESIDUAL_EST;
		pWilt->SetValue(m_Layer-1,m_Wilt);
		pWilt->Apply(m_Layer);

		m_Estimated_pF=TRUE;
		m_Estimated_Count++;
		OnInitDialog();
	}
}

void SP_Estimate::OnEditRetention() 

{   float *pressure= new float;
	float *theta= new float;
	BOOL change=TRUE;
	RetentionPoints dlg( this);
	if(dlg.DoModal()==IDOK) {	
		if(m_Num_pF_curve>1) {
		   if(*(float*)m_arrPressure[m_Num_pF_curve-2]==dlg.m_Pressure&&
			   *(float*)m_arrTheta[m_Num_pF_curve-2]==dlg.m_Theta) change=FALSE;
		}
		if(change) {							
			*pressure=dlg.m_Pressure;
			*theta=dlg.m_Theta;
			if(m_Num_pF_curve==0) m_Num_pF_curve++;
			m_arrPressure.SetAtGrow(m_Num_pF_curve-1, pressure);
			m_arrTheta.SetAtGrow(m_Num_pF_curve-1,theta);
			PLOTPF_mp_MODPRESSURE[m_Num_pF_curve-1]=dlg.m_Pressure;
			PLOTPF_mp_MODTHETA[m_Num_pF_curve-1]=dlg.m_Theta;
			PLOTPF_mp_NT=m_Num_pF_curve;
			PLOTPF_mp_LOADPF_POINTS();
		}
		else
			m_Num_pF_curve--;
	}
	OnInitDialog();
}

void SP_Estimate::OnEditRetentionEdit() 
{	float *pressure=new float;
	float *theta=new float;
	m_Num_pF_curve=m_Measured_pF.GetCurSel();
	RetentionPoints dlg(this);
	if(dlg.DoModal()==IDOK){
		*pressure=dlg.m_Pressure;
		*theta=dlg.m_Theta;
		m_arrPressure.SetAtGrow(m_Num_pF_curve, pressure);
		m_arrTheta.SetAtGrow(m_Num_pF_curve,theta);
		PLOTPF_mp_MODPRESSURE[m_Num_pF_curve-1]=dlg.m_Pressure;
		PLOTPF_mp_MODTHETA[m_Num_pF_curve-1]=dlg.m_Theta;
		PLOTPF_mp_LOADPF_POINTS();

	}
	OnInitDialog();
}

void SP_Estimate::OnEditRetentionReset() 
{
	m_arrPressure.RemoveAll();
	m_arrTheta.RemoveAll();	
	OnInitDialog();
	PLOTPF_mp_NT=0;

}

void SP_Estimate::OnSave() 
{
	// TODO: Add your control notification handler code here
	if(!PLOTPF_mp_OPENDATABASE) PLOTPF_mp_OPENBASE();
	PLOTPF_mp_GETLAYER(&m_Prof,&m_RepProf,&m_ProfLayer);

	SP_Save dlg(m_pDoc, m_ProfLayer);
	dlg.DoModal();

	
}

void SP_Estimate::OnRetrieve() 
{
	// TODO: Add your control notification handler code here
	if(!PLOTPF_mp_OPENDATABASE) PLOTPF_mp_OPENBASE();
	PLOTPF_mp_GETLAYER(&m_Prof,&m_RepProf,&m_ProfLayer);

	SP_Retrieve dlg(m_pDoc);
	if(dlg.DoModal()==IDOK){
		PLOTPF_mp_RETRIEVECOEF(&m_Layer-1);
		m_Prof=PLOTPF_mp_ACTUALPROFILE;
		pProf->SetValue(m_Layer-1,float(m_Prof));
		pProf->Apply(m_Layer);
		m_RepProf=PLOTPF_mp_ACTUALREPPROFILE;
		pRepProf->SetValue(m_Layer-1,float(m_RepProf));
		pRepProf->Apply(m_Layer);
		m_ProfLayer=PLOTPF_mp_ACTUALLEVEL;
		pProfLayer->SetValue(m_Layer-1,float(m_ProfLayer));
		pProfLayer->Apply(m_Layer);
		OnInitDialog();
	}
}

void SP_Estimate::OnApply2() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	pSaturation->SetValue(m_Layer-1,m_Porosity);
	pSaturation->Apply(m_Layer);
	PLOTPF_mp_APORO[PLOTPF_mp_ACTUALLEVEL-1]=m_Porosity;

	PLOTPF_mp_MODTEXTURE[0]=m_Clay;
	PLOTPF_mp_MODTEXTURE[1]=PLOTPF_mp_MODTEXTURE[0]+m_tex2;
	PLOTPF_mp_MODTEXTURE[2]=PLOTPF_mp_MODTEXTURE[1]+m_tex3;
	PLOTPF_mp_MODTEXTURE[3]=PLOTPF_mp_MODTEXTURE[2]+m_tex4;
	PLOTPF_mp_MODTEXTURE[4]=PLOTPF_mp_MODTEXTURE[3]+m_tex5;
	PLOTPF_mp_MODTEXTURE[5]=PLOTPF_mp_MODTEXTURE[4]+m_tex6;
	PLOTPF_mp_MODTEXTURE[6]=PLOTPF_mp_MODTEXTURE[5]+m_tex7;
	PLOTPF_mp_MODTEXTURE[7]=m_org;
	if(PLOTPF_mp_MODTEXTURE[7]+PLOTPF_mp_MODTEXTURE[6]>100) 
	for(int i=0;i<7;i++) {
		if(PLOTPF_mp_MODTEXTURE[i]>100) {
			PLOTPF_mp_MODTEXTURE[i]=100;
			PLOTPF_mp_MODTEXTURE[7]=0;
		}
	}
	PLOTPF_mp_LOADTEXTURE();
	PLOTPF_mp_SAVEDATABASE();
	OnInitDialog();
}

void SP_Estimate::OnBnClickedEstimate3()
{
	POSITION posd=m_pDoc->GetFirstViewPosition();
	CObject* m_pView;

	m_pView=m_pDoc->GetNextView(posd);
	int m_PreviousViewFunction;
	int m_ViewCounter;
	int function_no=401;
	if(function_no==m_PreviousViewFunction)
		m_ViewCounter++;
	else {
		((CCoupModelView*)m_pView)->m_OverLayPF=FALSE;
		((CCoupModelView*)m_pView)->Graph_End();
		m_ViewCounter=1;
		m_pCTab->SetPlotIndex(m_Layer);
		((CCoupModelView*)m_pView)->Graph_Ini();
	}
	m_PreviousViewFunction=function_no;
	((CCoupModelView*)m_pView)->Graph_DataLoad(m_pCTab, m_ViewCounter);
	m_pCTab->SetViewFunction(function_no);
	m_pCTab->ResetPlotIndex();// TODO: Add your control notification handler code here
}
