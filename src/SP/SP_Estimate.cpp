// SP_Estimate.cpp : implementation file
//

#include "..\stdafx.h"
#include "..\Util.h"
#include "..\CUG\MyCug.h"
#include "..\CoupModel.h"
#include "..\ModelTypes\SimB.h"
#include "..\NewBase\NewMap.h"
#include ".\RetentionPoints.h"
#include "..\CoupModelView.h"
#include ".\SP_Retrieve.h"
#include ".\SP_Save.h"
//#include "..\Chart\ParameterPlotDlg.h"
#include ".\sp_estimate.h"
#include "..\MainFrm.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/*
extern "C" int PARAMETERS_mp_STARTSTATEVALUES;
extern "C" bool   PLOTPF_mp_FINDLAYER;
extern "C" bool   PLOTPF_mp_OPENDATABASE();
extern "C" float HYDRAULIC_mp_ESATC(int *);
extern "C" float HYDRAULIC_mp_ETETRE(int *);
extern "C" float HYDRAULIC_mp_EPSIE(int *);
extern "C" float HYDRAULIC_mp_ELAMBDA(int *);
extern "C" float HYDRAULIC_mp_EWILT(int *);


extern "C" void HYDRAULIC_mp_HYDRAULIC_RECALC();
extern "C" int  HYDRAULIC_mp_ESTIMATEBC(float (*)[25], float (*)[25], int*, float *, float *);
extern "C" float HYDRAULIC_mp_AIRENTRY_EST;
extern "C" float HYDRAULIC_mp_R2_EST;
extern "C" float HYDRAULIC_mp_RESIDUAL_EST;
extern "C" float HYDRAULIC_mp_LAMBDA_EST;

extern "C" void PLOTPF_mp_GETLAYER(int *, int *, int*);
extern "C" void PLOTPF_mp_RETRIEVECOEF(int*);
extern "C" void PLOTPF_mp_RETRIEVEPF_POINTS();
extern "C" void PLOTPF_mp_LOADPF_POINTS();
extern "C" void PLOTPF_mp_LOADTEXTURE();
extern "C" void PLOTPF_mp_RETRIEVETEXTURE();
extern "C" void PLOTPF_mp_OPENBASE();
extern "C" void PLOTPF_mp_SAVEDATABASE();




extern "C" int PLOTPF_mp_ACTUALPROFILE;
extern "C" int PLOTPF_mp_ACTUALREPPROFILE;
extern "C" int PLOTPF_mp_ACTUALLEVEL;
extern "C" int PLOTPF_mp_NT;
extern "C" float PLOTPF_mp_MODPRESSURE[12];
extern "C" float PLOTPF_mp_MODTHETA[12];
extern "C" float PLOTPF_mp_MODTEXTURE[8];
extern "C" float PLOTPF_mp_APORO[22];
extern "C" char PLOTPF_mp_DKEY[6];
extern "C" char PLOTPF_mp_RPLACE[16];*/


/////////////////////////////////////////////////////////////////////////////
// SP_Estimate dialog


SP_Estimate::SP_Estimate(Tab *PointerToTable,int layer, CCoupModelDoc* pDoc, CWnd* pParent /*=NULL*/, BOOL all)
	: CDialog(SP_Estimate::IDD, pParent)
{
	m_Listed_Count=0;
	m_Estimated_Count=0;
	m_Num_pF_curve=0;
	m_Layer=layer;
	m_pDoc=pDoc;
	m_pPFCurve=m_pDoc->m_Doc.m_pPFCurve;
	m_AllLayers=all;
	POSITION posd=pDoc->GetFirstViewPosition();
	m_pView=pDoc->GetNextView(posd);
	m_pCTab=PointerToTable;
	m_NumberOfLayers=m_pCTab->GetNumOfRow();
	m_PreviousViewFunction=0;
	m_ViewCounter=1;
	
	//{{AFX_DATA_INIT(SP_Estimate)
	m_Clay = 0.0f;
	m_Sand = 0.0f;
	m_Porosity = 0.0f;
	m_EstimateMin = 10.0f;
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
	m_Residual = 0.0f;
	//}}AFX_DATA_INIT
	if(PointerToTable->GetName()=="Brooks-Corey, water retention, measured horizons")
	  m_Retention=TRUE	;
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
		m_Static2=_T("");
		m_Static3=_T("");
		m_Static4=_T("");

	}


	string Name, Group, Type;

	Name="ClayContent";
	Group="Soil Hydraulic";
	Type="Table Parameters";
    pClayContent=(P*)m_pDoc->m_Doc.GetPtr(Type,Group,Name);
	Name="SandContent";
    pSandContent=(P*)m_pDoc->m_Doc.GetPtr(Type,Group,Name);
   	Name="ProfileNo";
    pProf=(P*)m_pDoc->m_Doc.GetPtr(Type,Group,Name);
	Name="ProfileRepNo";
    pRepProf=(P*)m_pDoc->m_Doc.GetPtr(Type,Group,Name);
	Name="ProfileLayer";
    pProfLayer=(P*)m_pDoc->m_Doc.GetPtr(Type,Group,Name);
	m_PlotCurve=TRUE;

   
	
	Name="Saturation";
    pSaturation=(P*)m_pDoc->m_Doc.GetPtr(Type,Group,Name);
	Name="Total Conductivity";
    pTotalCond=(P*)m_pDoc->m_Doc.GetPtr(Type,Group,Name);
	Name="Matrix Conductivity";
    pMatCond=(P*)m_pDoc->m_Doc.GetPtr(Type,Group,Name);


	Name="Lambda";
    pLambda=(P*)m_pDoc->m_Doc.GetPtr(Type,Group,Name);
	Name="Air Entry";
    pAirEntry=(P*)m_pDoc->m_Doc.GetPtr(Type,Group,Name);
	Name="Wilting Point";
    pWilt=(P*)m_pDoc->m_Doc.GetPtr(Type,Group,Name);
	Name="Residual Water";
    pResidual=(P*)m_pDoc->m_Doc.GetPtr(Type,Group,Name);
	Name="Macro Pore";
    pMacro=(P*)m_pDoc->m_Doc.GetPtr(Type,Group,Name);

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

	m_InitTexture=FALSE;

	m_Estimated_pF=FALSE;
	if(!m_pPFCurve->IsProfile()) {

		m_pPFCurve->OpenEditMainDataBase();
		m_pPFCurve->ReadHeaderFile();
		m_pPFCurve->SaveNewFormat();
		m_pPFCurve->CloseMainDataBase();
		 string directory;
		directory=MFC_Util::GetProfileStringStd("DataBaseDirectory", directory);
		m_pPFCurve->OpenMainFile(directory+"PfProf");




	}

	NEWHEADER pfcurve=m_pPFCurve->GetHeader(m_Prof,m_RepProf); 
	//PLOTPF_mp_OPENBASE();
	//PLOTPF_mp_GETLAYER(&m_Prof,&m_RepProf,&m_ProfLayer);
	m_DetailedSandEstimate=FALSE;
	m_NewEstimate=FALSE;




		CMainFrame *pMainFrame;
			pMainFrame = (CMainFrame*) AfxGetMainWnd();


			pMainFrame->m_wndProperties.m_ChartCug.Init(m_pDoc);
			pMainFrame->m_wndProperties.m_ChartCug.ParameterPlotMode(TRUE);


			pMainFrame->m_wndProperties.m_ChartCug.m_ParameterPlot.InitData(m_pDoc,  pLambda, m_Layer-1);		
			pMainFrame->m_wndProperties.m_ChartCug.m_ParameterPlot.OnInit();
			pMainFrame->m_wndProperties.m_ChartCug.OnSetup();

//	m_DetailedSand=FALSE;
					
	/*if(m_pParChart->InitData(m_pDoc,(Ps*)m_pCTab, pLambda, m_Layer-1)) {
		m_pParChart->Create(IDD_PARAMETERPLOT_DIALOG, NULL);
		m_pParChart->ShowWindow(SW_SHOW);
	}*/

}


SP_Estimate::~SP_Estimate()
{
	
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
	DDX_Text(pDX, IDC_EDIT12, m_Residual);
	DDV_MinMaxFloat(pDX, m_Residual, 0.f, 50.f);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SPIN2, m_LayersChange);
	DDX_Control(pDX, IDC_DETAILEDSAND, m_DetailedSand);
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
	ON_BN_CLICKED(ID_ESTIMATE_PF2, OnEstimate2)
	ON_BN_CLICKED(ID_ESTIMATE3, OnPlotCurve)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_ALL_LAYERS, OnBnClickedButtonAllLayers)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN2, OnDeltaposSpin2)
	ON_BN_CLICKED(IDRETRIEVE, OnBnClickedRetrieve)

	ON_BN_CLICKED(IDC_DETAILEDSAND, OnBnClickedRadio1Sand)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SP_Estimate message handlers


void SP_Estimate::OnEstimate() 
{
	// TODO: Add your control notification handler code here
  
	UpdateData(TRUE);
	m_NumEstimated=0;
	if(m_AllLayers) {
		int oldlayer=m_Layer;
		m_NewEstimate=TRUE;
		for(int i=0; i<m_NumberOfLayers;i++) {
			m_Layer=i+1;
			PedoTexture();

			OnInitDialog();
		}
		m_Layer=oldlayer;
	}
	else {
			m_NewEstimate=TRUE;
			PedoTexture();
			OnInitDialog();
	}

	
}

void SP_Estimate::OnOK() 
{
	
	if(!m_AllLayers) {
	// TODO: Add extra validation here
		if(m_Retention) {
			// Confirm
		}
		else {

		}
	}
	
	Func* pFunc;
	SimB *pSimB1;
	pSimB1=m_pDoc->m_Doc.GetPtr(FUNCTION,"ReCalc Soil Function");
	pFunc=dynamic_cast<Func*>(pSimB1);
	if(pFunc!=NULL)
			pFunc->MakeOperation();


	CDialog::OnOK();
}


BOOL SP_Estimate::OnInitDialog() 
{
	CString String;	

	if(!m_NewEstimate) {
	NEWHEADER pfcurve=m_pPFCurve->GetHeader(m_Prof,m_RepProf); 
	
	if(m_pPFCurve->IsProfile()) {

		//PLOTPF_mp_RETRIEVEPF_POINTS();

		m_charProfLayer="Layer: "+MFC_Util::ItoAscii(m_ProfLayer);
		m_charProfName="Site name: ";

		m_charProfName+=pfcurve.Name.c_str();
		m_charProfNumber="Site no: ";
		m_charProfNumber+=pfcurve.OldKey.c_str();
		
		m_arrPressure=m_pPFCurve->GetPF_Pressure(pfcurve.RecLayers);
		m_arrTheta=m_pPFCurve->GetPF_Theta(m_ProfLayer);


		Retrieve();
	}
	else {
		m_charProfLayer=_T("");
		m_charProfName=_T("");
		m_charProfNumber=_T("");
	}
	}
	CDialog::OnInitDialog();
	if(m_AllLayers) 
		String="Estimate coefficients for all layers";
	else {
		String="Estimate coefficients for layer #: ";
		String+=MFC_Util::ItoAscii(m_Layer);
	}
	SetWindowText(String);

     m_Estimated_Coef.SetTabStops(44);
	 m_PlotCurve=FALSE;
  if(m_Estimated_Count>=m_Listed_Count) {
	  if(m_Listed_Count==0) {
		  if(m_Retention)
			String=MFC_Util::ItoAscii(m_Layer)+"\t"+"Current values\t\t\t"+
				MFC_Util::FtoAscii(m_Lambda)+"\t"+MFC_Util::FtoAscii(m_AirEntry)+"\t"+
				  MFC_Util::FtoAscii(m_Wilt)+"\t"+MFC_Util::FtoAscii(m_Residual);
		  else
			String=MFC_Util::ItoAscii(m_Layer)+"\t"+"Current values\t\t\t"+MFC_Util::FtoAscii(m_SatCond);
	  }
	  else if(m_Estimated_pF) {
		  String=MFC_Util::ItoAscii(m_Layer)+"\t"+MFC_Util::FtoAscii(m_Clay)+"\t"+MFC_Util::FtoAscii(m_Sand)+"\t"+MFC_Util::FtoAscii(m_Porosity)+"\t"+MFC_Util::FtoAscii(m_Est_R2)+"("+MFC_Util::ItoAscii(m_NumEstimated)+")"+"\t"+
				MFC_Util::FtoAscii(m_Lambda)+"\t"+MFC_Util::FtoAscii(m_AirEntry)+"\t"+
				  MFC_Util::FtoAscii(m_Wilt)+"\t"+MFC_Util::FtoAscii(m_Residual);
			m_PlotCurve=TRUE;
	  }
	  else {
		  if(m_Retention)  {
			String=MFC_Util::ItoAscii(m_Layer)+"\t"+MFC_Util::FtoAscii(m_Clay)+"\t"+MFC_Util::FtoAscii(m_Sand)+"\t"+MFC_Util::FtoAscii(m_Porosity)+"\t"+"-"+"\t"+
				MFC_Util::FtoAscii(m_Lambda)+"\t"+MFC_Util::FtoAscii(m_AirEntry)+"\t"+
				  MFC_Util::FtoAscii(m_Wilt)+"\t"+MFC_Util::FtoAscii(m_Residual);
				m_PlotCurve=TRUE;
		  }
		  else {
			String=MFC_Util::ItoAscii(m_Layer)+"\t"+MFC_Util::FtoAscii(m_Clay)+"\t"+MFC_Util::FtoAscii(m_Sand)+"\t"+MFC_Util::FtoAscii(m_Porosity)+"\t"+"-"+"\t"+
				MFC_Util::FtoAscii(m_SatCond);
				m_PlotCurve=FALSE;
		  }
	  }

	m_Estimated_Coef.AddString(String);
	m_Listed_Count++;
	m_Estimated_Coef.SetCurSel(m_Listed_Count);

  }

	if(m_PlotCurve) OnPlotCurve();
    m_Measured_pF.SetTabStops(38);
	m_Num_pF_curve=m_arrPressure.size();
	m_Measured_pF.ResetContent();
	for (int i=0;i<m_Num_pF_curve;i++){

		String=MFC_Util::FtoAscii(m_arrPressure[i])+"\t"+MFC_Util::FtoAscii(m_arrTheta[i]);
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
		GetDlgItem(ID_ESTIMATE_PF2)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT_RETENTION_EDIT)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT_RETENTION_RESET)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT_RETENTION)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT_MIN)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT_MAX)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_LIST_PF)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_STATIC8)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT12)->ModifyStyle(0,WS_DISABLED);

	 }

	 GetDlgItem(IDC_ESTIMATE_PF)->RedrawWindow();
	 GetDlgItem(ID_ESTIMATE_PF2)->RedrawWindow();
	 GetDlgItem(IDC_EDIT_RETENTION_EDIT)->RedrawWindow();
	 GetDlgItem(IDC_EDIT_RETENTION_RESET)->RedrawWindow();
	 GetDlgItem(IDC_EDIT_RETENTION)->RedrawWindow();
	 GetDlgItem(IDC_EDIT_MIN)->RedrawWindow();
	 GetDlgItem(IDC_EDIT_MAX)->RedrawWindow();
	 GetDlgItem(IDC_LIST_PF)->RedrawWindow();
	 GetDlgItem(IDC_STATIC8)->RedrawWindow();
	 GetDlgItem(IDC_EDIT12)->RedrawWindow();
//IDC_BUTTON_ALL_LAYERS,
 	GetDlgItem(IDC_EDIT5)->ModifyStyle(0,WS_DISABLED);
	GetDlgItem(IDC_EDIT6)->ModifyStyle(0,WS_DISABLED);
	GetDlgItem(IDC_EDIT7)->ModifyStyle(0,WS_DISABLED);
	GetDlgItem(IDC_EDIT11)->ModifyStyle(0,WS_DISABLED);
	 if(!m_DetailedSandEstimate) {
		GetDlgItem(IDC_EDIT8)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT9)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT10)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDRETRIEVE)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDAPPLY2)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT3)->ModifyStyle(WS_DISABLED,0);
	 }
	 else {
		GetDlgItem(IDC_EDIT8)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_EDIT9)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_EDIT10)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDRETRIEVE)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDAPPLY2)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_EDIT3)->ModifyStyle(0,WS_DISABLED);
	 }

	GetDlgItem(IDC_EDIT3)->RedrawWindow();
	GetDlgItem(IDC_EDIT5)->RedrawWindow();
	GetDlgItem(IDC_EDIT6)->RedrawWindow();
	GetDlgItem(IDC_EDIT7)->RedrawWindow();
	GetDlgItem(IDC_EDIT8)->RedrawWindow();
	GetDlgItem(IDC_EDIT9)->RedrawWindow();
	GetDlgItem(IDC_EDIT10)->RedrawWindow();
	GetDlgItem(IDC_EDIT11)->RedrawWindow();
	GetDlgItem(IDRETRIEVE)->RedrawWindow();
	GetDlgItem(IDAPPLY2)->RedrawWindow();



	 CString strtext;
	 if(m_AllLayers)
		 strtext="Change to Single Layer";
	 else
		 strtext="Change to All Layers";

	GetDlgItem(IDC_BUTTON_ALL_LAYERS)->SetWindowText(strtext);

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
{	
	UpdateData(TRUE);
	m_NumEstimated=0;
	NEWHEADER pfcurve=m_pPFCurve->GetHeader(m_Prof,m_RepProf); 
	if(m_AllLayers) {
		int oldlayer=m_Layer;
		m_pDoc->m_Doc.m_Simulator.m_SoilModel.m_SoilFrost.ReCalc();
	//	HYDRAULIC_mp_HYDRAULIC_RECALC();
		m_NewEstimate=TRUE;
		m_arrPressure.clear();
		m_arrPressure=m_pPFCurve->GetPF_Pressure(pfcurve.RecLayers);
		for(int ii=0; ii<m_NumberOfLayers;ii++) {
			m_Layer=ii+1;
			//PLOTPF_mp_ACTUALLEVEL=m_Layer;
			//PLOTPF_mp_RETRIEVEPF_POINTS();
			m_arrTheta.clear();
			
			m_arrTheta=m_pPFCurve->GetPF_Theta(ii);
		/*	for(int i=0;i<PLOTPF_mp_NT;i++){
  				m_arrPressure.SetAtGrow(i,&PLOTPF_mp_MODPRESSURE[i] );
				m_arrTheta.SetAtGrow(i,&PLOTPF_mp_MODTHETA[i]);
			}*/

			PedoRetension();	
			
			OnInitDialog();

		}
		m_Layer=oldlayer;
		m_pDoc->m_Doc.m_Simulator.m_SoilModel.m_SoilFrost.ReCalc();
		//HYDRAULIC_mp_HYDRAULIC_RECALC();
	}
	else {
			m_NewEstimate=TRUE;
			PedoRetension();
			OnInitDialog();
			m_pDoc->m_Doc.m_Simulator.m_SoilModel.m_SoilFrost.ReCalc();
			//HYDRAULIC_mp_HYDRAULIC_RECALC();
	}

}

void SP_Estimate::OnEditRetention() 

{   BOOL change=TRUE;
	RetentionPoints dlg( this);
	if(dlg.DoModal()==IDOK) {	
		if(m_Num_pF_curve>1) {
		   if(m_arrPressure[m_Num_pF_curve-2]==dlg.m_Pressure&&
			   m_arrTheta[m_Num_pF_curve-2]==dlg.m_Theta) change=FALSE;
		}
		if(change) {							
			if(m_Num_pF_curve==0) m_Num_pF_curve++;
			if(m_Num_pF_curve>m_arrPressure.size()) {
				m_arrPressure.resize(m_Num_pF_curve);
				m_arrTheta.resize(m_Num_pF_curve);
			}
			m_arrPressure[m_Num_pF_curve-1]=dlg.m_Pressure;
			m_arrTheta[m_Num_pF_curve-1]=dlg.m_Theta;
		/*	PLOTPF_mp_MODPRESSURE[m_Num_pF_curve-1]=dlg.m_Pressure;
			PLOTPF_mp_MODTHETA[m_Num_pF_curve-1]=dlg.m_Theta;
			PLOTPF_mp_NT=m_Num_pF_curve;
			PLOTPF_mp_LOADPF_POINTS();*/
		}
		else
			m_Num_pF_curve--;

	}
	OnInitDialog();
}

void SP_Estimate::OnEditRetentionEdit() 
{	//float *pressure=new float;
	//float *theta=new float;
	m_Num_pF_curve=m_Measured_pF.GetCurSel();
	RetentionPoints dlg(this);
	if(dlg.DoModal()==IDOK){
		if(m_Num_pF_curve>m_arrPressure.size()) {
				m_arrPressure.resize(m_Num_pF_curve);
				m_arrTheta.resize(m_Num_pF_curve);
		}
		m_arrPressure[m_Num_pF_curve-1]=dlg.m_Pressure;
		m_arrTheta[m_Num_pF_curve-1]=dlg.m_Theta;
	/*	PLOTPF_mp_MODPRESSURE[m_Num_pF_curve-1]=dlg.m_Pressure;
		PLOTPF_mp_MODTHETA[m_Num_pF_curve-1]=dlg.m_Theta;
		PLOTPF_mp_LOADPF_POINTS();*/

	}
	OnInitDialog();
}

void SP_Estimate::OnEditRetentionReset() 
{
	m_arrPressure.clear();
	m_arrTheta.clear();	
	OnInitDialog();
	//PLOTPF_mp_NT=0;

}

void SP_Estimate::OnSave() 
{
	// TODO: Add your control notification handler code here
//	if(!PLOTPF_mp_OPENDATABASE()) PLOTPF_mp_OPENBASE();
//	PLOTPF_mp_GETLAYER(&m_Prof,&m_RepProf,&m_ProfLayer);

	SP_Save dlg(m_pDoc, m_ProfLayer);
	dlg.DoModal();

	
}

void SP_Estimate::OnRetrieve() 
{
	// TODO: Add your control notification handler code here

	NEWHEADER pfcurve=m_pPFCurve->GetHeader(m_Prof,m_RepProf); 
	
	SP_Retrieve dlg(m_pDoc);
	if(dlg.DoModal()==IDOK){
		//PLOTPF_mp_RETRIEVECOEF(&m_Layer-1);
		//m_Prof=PLOTPF_mp_ACTUALPROFILE;
		pProf->SetValue(m_Layer-1,double(m_Prof));
		pProf->Apply(m_Layer);
	//	m_RepProf=PLOTPF_mp_ACTUALREPPROFILE;
		pRepProf->SetValue(m_Layer-1,double(m_RepProf));
		pRepProf->Apply(m_Layer);
	//	m_ProfLayer=PLOTPF_mp_ACTUALLEVEL;
		pProfLayer->SetValue(m_Layer-1,double(m_ProfLayer));
		pProfLayer->Apply(m_Layer);
		OnInitDialog();
	}
}



void SP_Estimate::OnEstimatePf2() 
{
   	//double Theta[25], Psi[25];
	vector<double> Theta, Psi;
	UpdateData(TRUE);
	m_NumEstimated=0;
	m_Num_pF_curve=m_arrPressure.size();
	m_Wilt=pWilt->GetValue(m_Layer-1);
	for(int i=0;i<m_Num_pF_curve;i++) {
		if(m_arrPressure[i]>=m_EstimateMin&&
			m_arrPressure[i]<=m_EstimateMax&&
			m_arrTheta[i]>0.001) {
				Theta[m_NumEstimated]=m_arrTheta[i];
				Psi[m_NumEstimated]=m_arrPressure[i];
				if(Psi[m_NumEstimated]>14900.&&Psi[m_NumEstimated]<15100)
					m_Wilt=Theta[m_NumEstimated];
				m_NumEstimated++;
			}
	}
	m_Estimated_pF=FALSE;
	if(m_NumEstimated>1) {
		BC_ESTIMATE bc=m_pPFCurve->Estimate_BC(&Theta, &Psi,m_Porosity, m_Residual);
		//int i=HYDRAULIC_mp_ESTIMATEBC(&Theta, &Psi, &m_NumEstimated, &m_Porosity, &m_Residual);
		m_Lambda=bc.Lambda;//HYDRAULIC_mp_LAMBDA_EST;
		m_AirEntry=bc.AirEntry;//HYDRAULIC_mp_AIRENTRY_EST;
		m_Residual=bc.Residual;//HYDRAULIC_mp_RESIDUAL_EST;
		m_Est_R2=bc.R2;
		pWilt->SetValue(m_Layer-1,m_Wilt);
		pWilt->Apply(m_Layer);
		pResidual->SetValue(m_Layer-1, m_Residual);
		pResidual->EditApply(m_Layer-1);
		pAirEntry->SetValue(m_Layer-1, m_AirEntry);
		pAirEntry->EditApply(m_Layer-1);
		pLambda->SetValue(m_Layer-1, m_Lambda);
		pLambda->EditApply(m_Layer-1);

		m_Estimated_pF=TRUE;
		m_Estimated_Count++;
		m_pDoc->m_Doc.m_Simulator.m_SoilModel.m_SoilFrost.ReCalc();
		//HYDRAULIC_mp_HYDRAULIC_RECALC();
		OnInitDialog();
	}
	
}

void SP_Estimate::OnEstimate2() 
{
	// TODO: Add your control notification handler code here
	OnEstimatePf2() ;

}

void SP_Estimate::OnPlotCurve() 
{
	// TODO: Add your control notification handler code here
	//SetValues();
	//OnSave();
	//PLOTPF_mp_OPENBASE();
	//PLOTPF_mp_GETLAYER(&m_Prof,&m_RepProf,&m_ProfLayer);


		pWilt->SetValue(m_Layer-1,m_Wilt);
		pWilt->Apply(m_Layer);

//		m_pCTab->Apply();


		CMainFrame *pMainFrame;
			pMainFrame = (CMainFrame*) AfxGetMainWnd();


			pMainFrame->m_wndProperties.m_ChartCug.Init(m_pDoc);
			pMainFrame->m_wndProperties.m_ChartCug.ParameterPlotMode(TRUE);


			pMainFrame->m_wndProperties.m_ChartCug.m_ParameterPlot.InitData(m_pDoc,  pLambda, m_Layer-1);		
			pMainFrame->m_wndProperties.m_ChartCug.m_ParameterPlot.OnInit();
			pMainFrame->m_wndProperties.m_ChartCug.OnSetup();




}

void SP_Estimate::OnBnClickedButtonAllLayers()
{
	UpdateData(TRUE);
	if(!m_AllLayers)
		m_AllLayers=TRUE;
	else
		m_AllLayers=FALSE;
	OnInitDialog();
	// TODO: Add your control notification handler code here
}

void SP_Estimate::OnDeltaposSpin2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	pNMUpDown->iDelta;
	if(pNMUpDown->iDelta==-1) 
		m_Layer++;
	else
		m_Layer--;

	if(m_Layer<1)
		m_Layer=1;
	else if(m_Layer>m_NumberOfLayers)
		m_Layer=m_NumberOfLayers;
	m_ProfLayer=m_Layer;
	*pResult = 0;
	m_Estimated_Count++;
	//HYDRAULIC_mp_R2_EST=0.;
	m_Sand=pSandContent->GetValue(m_Layer-1);
	m_Clay=pClayContent->GetValue(m_Layer-1);
	m_Porosity=pSaturation->GetValue(m_Layer-1);

	m_Lambda=pLambda->GetValue(m_Layer-1);
	m_AirEntry=pAirEntry->GetValue(m_Layer-1);
	m_Residual=pResidual->GetValue(m_Layer-1);
	m_Wilt=pWilt->GetValue(m_Layer-1);
	m_SatCond=pTotalCond->GetValue(m_Layer-1);
	OnInitDialog();
}
void SP_Estimate::PedoTexture()
{			
		if(m_AllLayers) {
			m_ProfLayer=m_Layer;
			Retrieve();
			m_Porosity=pSaturation->GetValue(m_Layer-1);
		}

			pSandContent->SetValue(m_Layer-1, m_Sand);
			pClayContent->SetValue(m_Layer-1, m_Clay);
			pSaturation->SetValue(m_Layer-1, m_Porosity);
			pSandContent->EditApply(m_Layer-1);
			pClayContent->EditApply(m_Layer-1);
			pSaturation->EditApply(m_Layer-1);

			m_SatCond=m_pDoc->m_Doc.m_Simulator.m_SoilModel.m_SoilFrost.ESatCFunc(m_Layer);
			m_Residual=m_pDoc->m_Doc.m_Simulator.m_SoilModel.m_SoilFrost.ETetResFunc(m_Layer);
			m_AirEntry=m_pDoc->m_Doc.m_Simulator.m_SoilModel.m_SoilFrost.EPsiFunc(m_Layer);
			m_Lambda=m_pDoc->m_Doc.m_Simulator.m_SoilModel.m_SoilFrost.ELambdaFunc(m_Layer);
			m_Wilt=m_pDoc->m_Doc.m_Simulator.m_SoilModel.m_SoilFrost.EWiltFunc(m_Layer);
			if(m_Retention) {
				pResidual->SetValue(m_Layer-1, m_Residual);
				pResidual->EditApply(m_Layer-1);
				pAirEntry->SetValue(m_Layer-1, m_AirEntry);
				pAirEntry->EditApply(m_Layer-1);
				pLambda->SetValue(m_Layer-1, m_Lambda);
				pLambda->EditApply(m_Layer-1);
				pWilt->SetValue(m_Layer-1,m_Wilt);
				pWilt->EditApply(m_Layer-1);
				pMacro->EditApply(m_Layer-1);
			}
			else {
					
				pTotalCond->SetValue(m_Layer-1, m_SatCond);
				pTotalCond->EditApply(m_Layer-1);
				if(pMatCond->GetValue(m_Layer-1)>m_SatCond) {
					pMatCond->SetValue(m_Layer-1,m_SatCond);  
					pMatCond->EditApply(m_Layer-1);  
				}
				else if(pMatCond->GetValue(m_Layer-1)<0.01*m_SatCond) {
					pMatCond->SetValue(m_Layer-1,0.01f*m_SatCond);  
					pMatCond->EditApply(m_Layer-1);  
				}



			}

			m_Estimated_pF=FALSE;
			m_Estimated_Count++;

}
void SP_Estimate::PedoRetension()
{
//double Theta[25], Psi[25];
	vector<double> Theta, Psi;
	Theta.resize(25); Psi.resize(25);
	UpdateData(TRUE);
	m_NumEstimated=0;
	m_Num_pF_curve=m_arrPressure.size();
	m_Wilt=pWilt->GetValue(m_Layer-1);
	for(int i=0;i<m_Num_pF_curve;i++) {
		if(m_arrPressure[i]>=m_EstimateMin&&
			m_arrPressure[i]<=m_EstimateMax&&
			m_arrTheta[i]>0.001) {
				Theta[m_NumEstimated]=m_arrTheta[i];
				Psi[m_NumEstimated]=m_arrPressure[i];
				m_NumEstimated++;
				if(Psi[m_NumEstimated]>14900.&&Psi[m_NumEstimated]<15100)
					m_Wilt=Theta[m_NumEstimated];
			}
	}
	




	m_Residual=0.;
	m_Porosity=pSaturation->GetValue(m_Layer-1);
	if(m_NumEstimated>1) {
		m_Residual=0.;
		BC_ESTIMATE bc=m_pPFCurve->Estimate_BC(&Theta, &Psi,m_Porosity, m_Residual);
		//int i=HYDRAULIC_mp_ESTIMATEBC(&Theta, &Psi, &m_NumEstimated, &m_Porosity, &m_Residual);
		m_Lambda=bc.Lambda;
		m_AirEntry=bc.AirEntry;
		m_Residual=bc.Residual;
		m_Est_R2=bc.R2;
		pResidual->SetValue(m_Layer-1, m_Residual);
		pResidual->EditApply(m_Layer-1);
		pAirEntry->SetValue(m_Layer-1, m_AirEntry);
		pAirEntry->EditApply(m_Layer-1);
		pLambda->SetValue(m_Layer-1, m_Lambda);
		pLambda->EditApply(m_Layer-1);
		pWilt->SetValue(m_Layer-1,m_Wilt);
		pWilt->EditApply(m_Layer-1);
		pMacro->EditApply(m_Layer-1);
		m_Estimated_Count++;
		m_Estimated_pF=TRUE;

	}
}
void SP_Estimate::OnApply2() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

//	PLOTPF_mp_ACTUALLEVEL=m_Layer;
	//m_ProfLayer=PLOTPF_mp_ACTUALLEVEL;
//	PLOTPF_mp_APORO[PLOTPF_mp_ACTUALLEVEL-1]=m_Porosity;
	vector<float> texture;
	texture.resize(8);
	texture[0]=m_Clay;
	texture[1]=texture[0]+m_tex2;
	texture[2]=texture[1]+m_tex3;
	texture[3]=texture[2]+m_tex4;
	texture[4]=texture[3]+m_tex5;
	texture[5]=texture[4]+m_tex6;
	texture[6]=texture[5]+m_tex7;
	texture[7]=m_org;
	if(texture[7]+texture[6]>100) 
	for(int i=0;i<7;i++) {
		if(texture[i]>100) {
			texture[i]=100;
			texture[7]=0;
		}
	}
	m_pPFCurve->SetPF_Texture(m_Layer-1, texture);
//	PLOTPF_mp_LOADTEXTURE();
//	PLOTPF_mp_SAVEDATABASE();
	OnInitDialog();
}
void SP_Estimate::Retrieve()
{
	//PLOTPF_mp_GETLAYER(&m_Prof,&m_RepProf,&m_ProfLayer);
	NEWHEADER pfcurve=m_pPFCurve->GetHeader(m_Prof,m_RepProf); 
	if(m_pPFCurve->IsProfile()) {

		//PLOTPF_mp_RETRIEVEPF_POINTS();
		//PLOTPF_mp_RETRIEVETEXTURE();
		//PLOTPF_mp_RETRIEVECOEF(&m_Layer);

		m_charProfLayer="Layer: "+MFC_Util::ItoAscii(pfcurve.NumLayers);
		m_charProfName="Site name: ";

		m_charProfName+=pfcurve.Name.c_str();
		m_charProfNumber="Site no: ";
		m_charProfNumber+=pfcurve.OldKey.c_str();
		vector<float> texture;
		texture=m_pPFCurve->GetTexture(m_Layer-1);

		m_charClay=MFC_Util::FtoAscii(texture[0]);m_charClay.Mid(0,4);
		m_charClay=m_charClay.Mid(0,4);
		for(int i=0;i<6;i++) {
			if(texture[i]>100) {
				texture[i]=100;
				texture[7]=0;
			}
		}

		m_tex2=texture[1]-texture[0];
	    m_tex3=texture[2]-texture[1];
	    m_tex4=texture[3]-texture[2];
	    m_tex5=texture[4]-texture[3];
	    m_tex6=texture[5]-texture[4];
	    m_tex7=100.f-texture[7]-texture[5];
		if(m_tex7<0) {
			float diff=-1*m_tex7;
			m_tex7=m_tex7+diff;
			texture[7]=texture[7]-diff;
		}


		if(m_DetailedSandEstimate) {
			m_Clay=texture[0];
			m_Sand=(100.f-texture[7]-texture[3]);
		}
		m_org=texture[7];
		m_charCUM1=m_charClay;
		m_charCUM2=MFC_Util::FtoAscii(texture[1]);		m_charCUM2=m_charCUM2.Mid(0,4);
		m_charCUM3=MFC_Util::FtoAscii(texture[2]);		m_charCUM3=m_charCUM3.Mid(0,4);
		m_charCUM4=MFC_Util::FtoAscii(texture[3]);		m_charCUM4=m_charCUM4.Mid(0,4);
		m_charCUM5=MFC_Util::FtoAscii(texture[4]);		m_charCUM5=m_charCUM5.Mid(0,4);
		m_charCUM6=MFC_Util::FtoAscii(texture[5]);		m_charCUM6=m_charCUM6.Mid(0,4);
		m_charCUM7=MFC_Util::FtoAscii(100.f-texture[7]);	m_charCUM7=m_charCUM7.Mid(0,4);
		m_charCUM8=MFC_Util::FtoAscii(100.);							m_charCUM8=m_charCUM8.Mid(0,4);

		m_arrTheta.clear();
		m_arrPressure.clear();
		m_arrPressure=m_pPFCurve->GetPF_Pressure(pfcurve.RecLayers);
		m_arrTheta=m_pPFCurve->GetPF_Theta(m_Layer-1);
		//for(int i=0;i<PLOTPF_mp_NT;i++){
  		//	m_arrPressure.SetAtGrow(i,&PLOTPF_mp_MODPRESSURE[i] );
		//	m_arrTheta.SetAtGrow(i,&PLOTPF_mp_MODTHETA[i]);
		//}


	}

}
void SP_Estimate::OnBnClickedRetrieve()
{
	Retrieve();
	OnInitDialog();
	
}




void SP_Estimate::OnBnClickedRadio1Sand()
{
	UpdateData(TRUE);
	BOOL IV=m_DetailedSand.GetState();
	if(IV==9)
		m_DetailedSandEstimate=TRUE;
	else
		m_DetailedSandEstimate=FALSE;
	OnInitDialog();
	// TODO: Add your control notification handler code here
}
