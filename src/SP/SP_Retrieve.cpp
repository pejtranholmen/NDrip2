// SP_Retrieve.cpp : implementation file
//

#include "../stdafx.h"
#include "../Util.h"
#include "../CUG/SimDoc/MyCug.h"
#include "../CoupModel.h"
#include "../CoupModelDoc.h"
#include "../CoupModelView.h"
#include "./sp_retrieve.h"
#include "./SP_EXCHANGE1.h"
#include "./SP_EXPORT.h"
#include "./SP_Save.h"
#include "./SP_ImportDlg.h"
#include "../SoilData/PlotPF/PFCurve.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static BOOL MultiRunChange;


/////////////////////////////////////////////////////////////////////////////
// SP_Retrieve dialog


SP_Retrieve::SP_Retrieve(Doc* pDoc, CWnd* pParent /*=nullptr*/)
	: CDialog(SP_Retrieve::IDD, pParent)
	, m_CompleteList(FALSE)
	, m_Min_NumHeads(0)
	, m_Details("")
{
	m_ReadOnly=FALSE;
	m_pDoc=pDoc;
	CWinApp* pApp = AfxGetApp();

	m_pView=m_pView=m_pSimDoc->GetModelView();
	m_pSimDoc=pDoc;
	m_LayersInModelTable=-1;
	m_NumToBeSelected=1;
	IniDataBase();
//	PLOTPF_mp_CLOSEBASE();
	//{{AFX_DATA_INIT(SP_Retrieve)
	m_Layer = 1;
	m_Prof = 1;
	m_RepNumber = 1;
	m_ProfName = "";
	m_NumLayers = "";
	m_County = "";
	m_Coordinates = "";
	m_ProfMin = 1;
	m_TopSandMin = 0;
	m_SubSandMin = 0;
	m_SubSandMax = 100;
	m_SubClayMin = 0;
	m_SubClayMax = 100;
	m_ProfMax = 999;
	m_CountyMin = "A";
	m_CountyMax = "Z";
	m_TopSandMax = 100;
	m_TopClayMin = 0;
	m_TopClayMax = 100;
	m_TopOrgMin = 0;
	m_TopOrgMax = 100;
	m_SubOrgMin = 0;
	m_SubOrgMax = 100;
	m_PFpoints = "";
	m_YearMin = 0;
	m_YearMax = 2020;
	m_charYear = "";
	m_NewHeader.KeyProf=30101;
	m_NewHeader.NumLayers=3;
	m_NewHeader.Name="Name of Place";
	p_PFCurve = &PlotpF::GetPointer();


	//}}AFX_DATA_INIT
}

SP_Retrieve::~SP_Retrieve() {
	//delete m_SoilPropCug;

}
SP_Retrieve::SP_Retrieve(BOOL reset,Doc* pDoc, CWnd* pParent /*=nullptr*/)
	: CDialog(SP_Retrieve::IDD, pParent)
{

	IniDataBase();
	m_NumToBeSelected=1;
	m_ReadOnly=reset;
	m_pDoc=pDoc;
	m_pSimDoc=pDoc;

 	string Name="Hydraulic conductivity, measured horizons";
	string Group="Soil Hydraulic";
	string Type="Tables";



	m_pDB=(CDB*)m_pDoc->DB_GetPointer(DB_types(1));
	m_Prof=1;
	m_RepNumber=1;



	m_pDB=nullptr;
	int nrow;
	nrow=1;
/*	m_Prof_ToSim.OnUpdate(m_pDB, FALSE, &m_Prof,&m_RepNumber, &m_SoilPropCug, this,false,"Profile",m_pSimDoc, &m_SoilPropData );
	m_SoilPropCug.OnUpdate(m_pDB, TRUE, &m_Prof,&m_RepNumber, &m_Prof_ToSim, this, TRUE,"",m_pSimDoc, &m_SoilPropData);
	m_SoilPropData.OnUpdate(m_pDB, TRUE, &m_Prof,&m_RepNumber, &m_Prof_ToSim, this, TRUE,"DataSheet", m_pSimDoc);
	m_EstimationInfo.OnUpdate(m_pDB, TRUE, &m_Prof,&m_RepNumber, &m_Prof_ToSim, this, TRUE,"DataSheet", m_pSimDoc);
	*/
	

    Tab *pTab=(Tab*)m_pDoc->GetPtr(Type,Group, Name);
	m_LayersInModelTable=pTab->GetIntValue();

	CWinApp* pApp = AfxGetApp();

	m_pView=(CCoupModelView*)pDoc->GetModelView();


	//PLOTPF_mp_TOBEDOWNLOADED=true;

	m_Layer = 1;
	m_Prof = 1;
	m_RepNumber = 1;
	m_ProfName = "";
	m_NumLayers = "";
	m_County = "";
	m_Coordinates = "";
	m_ProfMin = 1;
	m_TopSandMin = 0;
	m_SubSandMin = 0;
	m_SubSandMax = 100;
	m_SubClayMin = 0;
	m_SubClayMax = 100;
	m_ProfMax = 999;
	m_CountyMin = "A";
	m_CountyMax = "Z";
	m_TopSandMax = 100;
	m_TopClayMin = 0;
	m_TopClayMax = 100;
	m_TopOrgMin = 0;
	m_TopOrgMax = 100;
	m_SubOrgMin = 0;
	m_SubOrgMax = 100;
	m_YearMin = 1950;
	m_YearMax = 2020;
	m_InitDialog=FALSE;
	m_Min_NumHeads=0;
   m_SoilPropCug.m_ProfMin=m_ProfMin;
   m_SoilPropCug.m_ProfMax=m_ProfMax;
   m_SoilPropCug.m_TopOrgMin=m_TopOrgMin;
   m_SoilPropCug.m_TopOrgMax=m_TopOrgMax;
   m_SoilPropCug.m_TopSandMin=m_TopSandMin;
   m_SoilPropCug.m_TopSandMax=m_TopSandMax;
   m_SoilPropCug.m_TopClayMin=m_TopClayMin;
   m_SoilPropCug.m_TopClayMax=m_TopClayMax;
   m_SoilPropCug.m_SubOrgMin=m_SubOrgMin;
   m_SoilPropCug.m_SubOrgMax=m_SubOrgMax;
   m_SoilPropCug.m_SubSandMin=m_SubSandMin;
   m_SoilPropCug.m_SubSandMax=m_SubSandMax;
   m_SoilPropCug.m_SubClayMin=m_SubClayMin;
   m_SoilPropCug.m_SubClayMax=m_SubClayMax;
   m_SoilPropCug.m_YearMin=m_YearMin;
   m_SoilPropCug.m_YearMax=m_YearMax;
   m_SoilPropCug.m_Min_NumHeads=m_Min_NumHeads;
	if(m_CompleteList)
	   m_SoilPropCug.m_CompleteList=TRUE;
   else
	   m_SoilPropCug.m_CompleteList=FALSE;

	m_PFpoints = "";
	p_PFCurve = &PlotpF::GetPointer();
}
SP_Retrieve::SP_Retrieve(BOOL reset,Doc* pDoc, CDB *pDB, int nrow, CWnd* pParent /*=nullptr*/)
	: CDialog(SP_Retrieve::IDD, pParent)
{
	IniDataBase();
	m_pDoc=pDoc;
	m_pSimDoc=pDoc;/*
	if(pDB!=nullptr) {
		m_Prof_ToSim.OnUpdate(pDB, FALSE, nrow);
		BOOL Sel_One;
		if(nrow<0)
			Sel_One=TRUE;
		else
			Sel_One=FALSE;
		m_SoilPropCug.OnUpdate(pDB, TRUE, &m_Prof,&m_RepNumber, &m_Prof_ToSim, this, Sel_One,"",m_pSimDoc, &m_SoilPropData);
		m_SoilPropData.OnUpdate(pDB, TRUE, &m_Prof,&m_RepNumber, &m_Prof_ToSim, this, Sel_One,"DataSheet", m_pSimDoc);
	}*/
	m_pDB=pDB;
	m_ReadOnly=reset;



 	string Name="Hydraulic conductivity, measured horizons";
	string Group="Soil Hydraulic";
	string Type="Tables";

    Tab *pTab=(Tab*)m_pDoc->GetPtr(Type,Group, Name);
	m_LayersInModelTable=int(pTab->GetValue());

	CWinApp* pApp = AfxGetApp();
		
	m_pView=m_pSimDoc->GetModelView();

	//PLOTPF_mp_TOBEDOWNLOADED=TRUE;
	m_NumToBeSelected=nrow;


	m_Layer = 1;
	//m_Prof = 1;
	//m_RepNumber = 1;
	m_ProfName = "";
	m_NumLayers = "";
	m_County = "";
	m_Coordinates = "";
	m_ProfMin = 1;
	m_TopSandMin = 0;
	m_SubSandMin = 0;
	m_SubSandMax = 100;
	m_SubClayMin = 0;
	m_SubClayMax = 100;
	m_ProfMax = 999;
	m_CountyMin = "A";
	m_CountyMax = "Z";
	m_TopSandMax = 100;
	m_TopClayMin = 0;
	m_TopClayMax = 100;
	m_TopOrgMin = 0;
	m_TopOrgMax = 100;
	m_SubOrgMin = 0;
	m_SubOrgMax = 100;
	m_YearMin = 1950;
	m_YearMax = 2007;
	m_InitDialog=FALSE;
	m_Min_NumHeads=0;

   m_SoilPropCug.m_ProfMin=m_ProfMin;
   m_SoilPropCug.m_ProfMax=m_ProfMax;
   m_SoilPropCug.m_TopOrgMin=m_TopOrgMin;
   m_SoilPropCug.m_TopOrgMax=m_TopOrgMax;
   m_SoilPropCug.m_TopSandMin=m_TopSandMin;
   m_SoilPropCug.m_TopSandMax=m_TopSandMax;
   m_SoilPropCug.m_TopClayMin=m_TopClayMin;
   m_SoilPropCug.m_TopClayMax=m_TopClayMax;
   m_SoilPropCug.m_SubOrgMin=m_SubOrgMin;
   m_SoilPropCug.m_SubOrgMax=m_SubOrgMax;
   m_SoilPropCug.m_SubSandMin=m_SubSandMin;
   m_SoilPropCug.m_SubSandMax=m_SubSandMax;
   m_SoilPropCug.m_SubClayMin=m_SubClayMin;
   m_SoilPropCug.m_SubClayMax=m_SubClayMax;
   m_SoilPropCug.m_YearMin=m_YearMin;
   m_SoilPropCug.m_YearMax=m_YearMax;
   m_SoilPropCug.m_Min_NumHeads=m_Min_NumHeads;
   if(m_CompleteList)
	   m_SoilPropCug.m_CompleteList=TRUE;
   else
		m_SoilPropCug.m_CompleteList=FALSE;



	m_PFpoints = "";
	p_PFCurve = &PlotpF::GetPointer();
}
SP_Retrieve::SP_Retrieve(int prof, int repprof, CWnd* pParent /*=nullptr*/)
	: CDialog(SP_Retrieve::IDD, pParent)
{
	IniDataBase();

	m_LayersInModelTable=-1;
	m_Layer = 1;
	m_Prof = prof;
	m_RepNumber = repprof;
	p_PFCurve = &PlotpF::GetPointer();
}

SP_Retrieve::SP_Retrieve(string key,CWnd* pParent /*=nullptr*/)
	: CDialog(SP_Retrieve::IDD, pParent)
{

	int layer,prof=1,repNumber=1;
	auto index=key.find(":");
	if(index!=string::npos) {
		prof=MFC_Util::AtoInt(key.substr(2,index-2));
		repNumber=MFC_Util::AtoInt(key.substr(index+1,2));
	}

	layer=1;

	IniDataBase();
	m_ActHeader=p_PFCurve->GetHeader(prof, repNumber);

   // m_NumLayers=m_ActHeader.NumLayers;//MFC_Util::ItoAscii(PLOTPF_mp_ACTUALNUMLEVELS);
	//PLOTPF_mp_TOBEDOWNLOADED=true;
	m_LayersInModelTable=-1;
	p_PFCurve = &PlotpF::GetPointer();


}

void SP_Retrieve::SetComments(string str)
{
	string txt1=p_PFCurve->GetComments();

	GetDlgItem(IDC_COMMENT1)->SetWindowText(CString(txt1.c_str()));
	GetDlgItem(IDC_COMMENT1)->RedrawWindow();
	
}

void SP_Retrieve::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SP_Retrieve)

/*	DDX_Text(pDX, IDC_EDIT4, m_ProfMin);
	DDV_MinMaxUInt(pDX, m_ProfMin, 1, 999);
	DDX_Text(pDX, IDC_EDIT8, m_TopSandMin);
	DDV_MinMaxUInt(pDX, m_TopSandMin, 0, 100);
	DDX_Text(pDX, IDC_EDIT12, m_SubSandMin);
	DDV_MinMaxUInt(pDX, m_SubSandMin, 0, 100);
	DDX_Text(pDX, IDC_EDIT13, m_SubSandMax);
	DDV_MinMaxUInt(pDX, m_SubSandMax, 0, 100);
	DDX_Text(pDX, IDC_EDIT14, m_SubClayMin);
	DDV_MinMaxUInt(pDX, m_SubClayMin, 0, 100);
	DDX_Text(pDX, IDC_EDIT15, m_SubClayMax);
	DDV_MinMaxUInt(pDX, m_SubClayMax, 0, 100);
	DDX_Text(pDX, IDC_EDIT5, m_ProfMax);
	DDV_MinMaxUInt(pDX, m_ProfMax, 1, 999);
	DDX_Text(pDX, IDC_EDIT6, m_CountyMin);
	DDV_MaxChars(pDX, m_CountyMin, 2);
	DDX_Text(pDX, IDC_EDIT7, m_CountyMax);
	DDV_MaxChars(pDX, m_CountyMax, 2);
	DDX_Text(pDX, IDC_EDIT9, m_TopSandMax);
	DDX_Text(pDX, IDC_EDIT10, m_TopClayMin);
	DDV_MinMaxUInt(pDX, m_TopClayMin, 0, 100);
	DDX_Text(pDX, IDC_EDIT11, m_TopClayMax);
	DDV_MinMaxUInt(pDX, m_TopClayMax, 0, 100);
	DDX_Text(pDX, IDC_EDIT18, m_TopOrgMin);
	DDV_MinMaxUInt(pDX, m_TopOrgMin, 0, 100);
	DDX_Text(pDX, IDC_EDIT19, m_TopOrgMax);
	DDV_MinMaxUInt(pDX, m_TopOrgMax, 0, 100);
	DDX_Text(pDX, IDC_EDIT16, m_SubOrgMin);
	DDV_MinMaxUInt(pDX, m_SubOrgMin, 0, 100);
	DDX_Text(pDX, IDC_EDIT17, m_SubOrgMax);
	DDV_MinMaxUInt(pDX, m_SubOrgMax, 0, 100);
	DDX_Text(pDX, IDC_EDIT20, m_YearMin);
	DDV_MinMaxUInt(pDX, m_YearMin, 0, 2020);
	DDX_Text(pDX, IDC_EDIT21, m_YearMax);
	DDV_MinMaxUInt(pDX, m_YearMax, 0, 2020);
	DDX_Text(pDX, IDC_EDIT22, m_Min_NumHeads);
	//DDV_MinMaxUInt(pDX, m_YearMax, 0, 2010);*/
	//}}AFX_DATA_MAP
	//DDX_Control(pDX, IDC_GRIDSIMPLE, m_SoilPropCug);
	//DDX_Control(pDX, IDC_PFCUG_TOSIM, m_Prof_ToSim);
	//DDX_Check(pDX, IDC_CHECK1, m_CompleteList);
}


BEGIN_MESSAGE_MAP(SP_Retrieve, CDialog)
	//{{AFX_MSG_MAP(SP_Retrieve)
	//ON_BN_CLICKED(IDC_BUTTON_SCAN, OnButtonScan)
	ON_BN_CLICKED(IDC_BUTTON_SCAN_ALL, OnButtonScanAll)
	ON_LBN_SELCHANGE(IDC_LIST_SELECTION, OnSelchangeListSelection)
	ON_BN_CLICKED(IDC_BUTTON_VIEW, OnButtonView)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_LAYERS, OnButtonViewLayers)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_LAYERS_RET, OnButtonViewLayersRet)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_RET, OnButtonViewRet)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDIT4, OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT5, OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT6, OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT7, OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT8, OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT9, OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT10, OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT11, OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT12, OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT13, OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT14, OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT15, OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT16, OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT17, OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT18, OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT19, OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT20, OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT21, OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT22, OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_COMMENT1, OnEnChangeComment)
	ON_BN_CLICKED(IDC_BUTON_DELETE, OnBnClickedButonDelete)
	ON_BN_CLICKED(IDC_BUTON_ADD, OnBnClickedButonAdd)
	ON_BN_CLICKED(IDC_BUTON_EXCHANGE, OnBnClickedButonExchange)
	ON_BN_CLICKED(IDC_BUTON_EXPORT, OnBnClickedButonExport)
	ON_BN_CLICKED(IDC_BUTON_IMPORT, OnBnClickedButonImport)
	ON_BN_CLICKED(IDOK, &SP_Retrieve::OnBnClickedOk)
	ON_EN_SETFOCUS(IDC_COMMENT1, &SP_Retrieve::OnEnSetfocusEdit23)
	ON_EN_CHANGE(IDC_COMMENT1, &SP_Retrieve::OnEnChangeEditComments)
//	ON_BN_CLICKED(IDOKBUTTON, &SP_Retrieve::OnBnClickedOkbutton)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SP_Retrieve message handlers

void SP_Retrieve::OnButtonScan() 
{  
   UpdateData(TRUE);
   m_SoilPropCug.m_ProfMin=m_ProfMin;
   m_SoilPropCug.m_ProfMax=m_ProfMax;
   m_SoilPropCug.m_TopOrgMin=m_TopOrgMin;
   m_SoilPropCug.m_TopOrgMax=m_TopOrgMax;
   m_SoilPropCug.m_TopSandMin=m_TopSandMin;
   m_SoilPropCug.m_TopSandMax=m_TopSandMax;
   m_SoilPropCug.m_TopClayMin=m_TopClayMin;
   m_SoilPropCug.m_TopClayMax=m_TopClayMax;
   m_SoilPropCug.m_SubOrgMin=m_SubOrgMin;
   m_SoilPropCug.m_SubOrgMax=m_SubOrgMax;
   m_SoilPropCug.m_SubSandMin=m_SubSandMin;
   m_SoilPropCug.m_SubSandMax=m_SubSandMax;
   m_SoilPropCug.m_SubClayMin=m_SubClayMin;
   m_SoilPropCug.m_SubClayMax=m_SubClayMax;
   m_SoilPropCug.m_YearMin=m_YearMin;
   m_SoilPropCug.m_YearMax=m_YearMax;
   m_SoilPropCug.m_Min_NumHeads=m_Min_NumHeads;
   if(m_CompleteList)
	   m_SoilPropCug.m_CompleteList=TRUE;
   else
		m_SoilPropCug.m_CompleteList=FALSE;
 	OnInitDialog();




}

void SP_Retrieve::OnButtonScanAll() 
{

}

void SP_Retrieve::OnButtonSelect() 
{
	// TODO: Add your control notification handler code here
	
}

void SP_Retrieve::OnSelchangeListSelection() 
{
	// TODO: Add your control notification handler code here
	CString String;
	UpdateData(TRUE);

	m_Selection.GetText(m_Selection.GetCurSel(),String);
	CString String2=String.Mid(0,3);
	int prof=MFC_Util::AtoInt(String2);
	String2=String.Mid(4,2);
	int repNumber=MFC_Util::AtoInt(String2);
	m_Prof=prof;
	m_RepNumber=repNumber;
	int layer;
	layer=m_Layer;
//	m_pDoc->SP_ResetComments();
	p_PFCurve->GetHeader(prof, repNumber);
	//PLOTPF_mp_GETLAYER(&prof,&repNumber,&layer);
	UpdateData(TRUE);
	OnInitDialog();
}

BOOL SP_Retrieve::OnInitDialog() 
{
	//pSP_Retrieve=this;
	
	string InfoText;
	if(m_ReadOnly) {
		if(m_NumToBeSelected<0)
			InfoText="Select one profile from database with "+FUtil::STD_ItoAscii(p_PFCurve->GetNumProfiles())+ " soil profiles"; 
		else
			InfoText="Select "+FUtil::STD_ItoAscii(m_NumToBeSelected)+" records from database with "+FUtil::STD_ItoAscii(p_PFCurve->GetNumProfiles())+ " soil profiles"; 
	}
	else {
		InfoText="Edit your soil physical database (PFProf)";
	}


	SetWindowText(CString(InfoText.c_str()));


	CDialog::OnInitDialog();
	if(!m_InitDialog){
		m_SoilPropCug.AttachGrid(this, IDC_PFCUG);
		m_Prof_ToSim.AttachGrid(this, IDC_PFCUG_TOSIM);
		m_SoilPropData.AttachGrid(this, IDC_PFCUG_DATA);
		//m_EstimationInfo.AttachGrid(this, IDC_PF_GRID_EST);
		m_InitDialog=TRUE;
	}

	m_Prof_ToSim.OnSetup();
    m_SoilPropCug.OnSetup();
	m_SoilPropData.OnSetup();
	m_EstimationInfo.OnSetup();
	
	SetComments("");

	string str;
	str=MFC_Util::ItoAscii(m_SoilPropCug.m_row)+" profiles selected from database";
	GetDlgItem(IDC_STATIC_SELFROMBASE)->SetWindowText(CString(str.c_str()));
	str="";
	GetDlgItem(IDC_STATIC_SELTOSIM)->SetWindowText(CString(str.c_str()));
	

	
	GetDlgItem(IDC_STATIC_SELFROMBASE)->RedrawWindow();		
	GetDlgItem(IDC_STATIC_SELTOSIM)->RedrawWindow();
	if(m_ReadOnly) {
		GetDlgItem(IDC_EDIT23)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT23)->RedrawWindow();
	}
	if(p_PFCurve->IsProfile()) 
		GetDlgItem(IDC_BUTON_DELETE)->ModifyStyle(WS_DISABLED,0);
	else
		GetDlgItem(IDC_BUTON_DELETE)->ModifyStyle(0,WS_DISABLED);
	GetDlgItem(IDC_BUTON_DELETE)->RedrawWindow();		

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SP_Retrieve::OnChangeProf()
{
	int prof=m_Prof;
	int repNumber=m_RepNumber;
	int layer=m_Layer;

//	m_pDoc->SP_ResetComments();
	p_PFCurve->GetHeader(prof, repNumber);
	//PLOTPF_mp_GETLAYER(&prof,&repNumber,&layer);
	UpdateData(TRUE);
	OnInitDialog();

}
string SP_Retrieve::GetCurrentName()
{
	string str;
	if(p_PFCurve->IsProfile()) {
		str=p_PFCurve->m_ActProfile.OldKey;
		str+="  ";
		str+=p_PFCurve->m_ActProfile.Name;
	}
	else
		str="";

	return str;
}

void SP_Retrieve::OnCancel() 
{
	// TODO: Add extra cleanup here
	//PLOTPF_mp_TOBEDOWNLOADED=false;
	if(p_PFCurve->IsProfile()) {
		int prof=m_IniProf;
		int repNumber=m_IniRepProf;
		int layer=1;
		if(m_IniProf>0)
			p_PFCurve->GetHeader(prof,repNumber);
	}

	CDialog::OnCancel();
}

void SP_Retrieve::OnButtonView() 
{
// Texture Profile

}

void SP_Retrieve::OnButtonViewLayers() 
{
// Texture Layers

}

void SP_Retrieve::OnButtonViewLayersRet() 
{

	
}

void SP_Retrieve::OnButtonViewRet() 
{
// Retention Profile

}

void SP_Retrieve::OnBnClickedCheck1()
{
	UpdateData(TRUE);
	OnButtonScan();
	// TODO: Add your control notification handler code here
}

void SP_Retrieve::OnBnClickedButton1()
{
	if(m_pDB!=nullptr)
		m_pDB->MR_ResetSelection();
	

	OnInitDialog();
	// TODO: Add your control notification handler code here
}

void SP_Retrieve::OnEnChangeEdit4()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	OnButtonScan();
}

void SP_Retrieve::OnEnChangeComment()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString cstr;
	GetDlgItem(IDC_COMMENT1)->GetWindowText(cstr);
	m_Details = CT2A(cstr).m_psz;
	p_PFCurve->SetComments(m_Details);
}
void SP_Retrieve::OnAddComments(string str)
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	CString cstr;
	GetDlgItem(IDC_COMMENT1)->GetWindowText(cstr);
	m_Details = CT2A(cstr).m_psz;
	m_Details+="\r\n";
	m_Details+=str;
	p_PFCurve->SetComments(m_Details);
	GetDlgItem(IDC_COMMENT1)->SetWindowText(CString(m_Details.c_str()));
	GetDlgItem(IDC_COMMENT1)->RedrawWindow();

	
}


void SP_Retrieve::OnBnClickedButonDelete()
{
	// TODO: Add your control notification handler code here

	CString String;
	m_SoilPropCug.SaveTopRow();
	size_t rec_head=p_PFCurve->GetActHeaderRecord();
	if(p_PFCurve->DeleteActHeader()) {
		MFC_Util::MessageBox("Profile deleted !","File Information",MB_OK);
		if(rec_head>0)
			p_PFCurve->SetHeader(rec_head-1);			
	}
	else {
		MFC_Util::MessageBox("Profile could not be deleted !","File Information",MB_OK);

		
	};
	OnInitDialog();
	m_SoilPropCug.SetTopRowExternt();
}

void SP_Retrieve::OnBnClickedButonAdd()

{

	//SP_Save dlg(TRUE, m_pDoc);
	p_PFCurve->CloseMainDataBase();
	p_PFCurve->OpenMainFile(p_PFCurve->GetMainDataBaseName(false),true);
	if(p_PFCurve->Set_Header(p_PFCurve->GetNewHeader())) {
		p_PFCurve->UpdateHeaderFile();
		p_PFCurve->OpenEditMainDataBase();
		p_PFCurve->SaveNewFormat();
		m_Prof_ToSim.OnSheetSetup(2);
		m_SoilPropCug.OnSheetSetup(2);
		MFC_Util::MessageBox("New Profile was created (MySite) - Edit description field\n Make sure that all data are added !\n","File Information",MB_OK);
	}
	else {
		MFC_Util::MessageBox("New Profile could not be created !","File Information",MB_OK);

	}

//	if(dlg.DoModal()==IDOK) {
		OnInitDialog();


//	}
	// TODO: Add your control notification handler code here
}

void SP_Retrieve::OnBnClickedButonExchange()
{

//	SP_EXCHANGE1 dlg(m_pDoc);
//	dlg.DoModal();
	// TODO: Add your control notification handler code here
}

void SP_Retrieve::OnBnClickedButonExport()
{
	SP_Export dlg;
	if(dlg.DoModal()) {
		dlg.Convert();
		MFC_Util::MessageBox("File Database.Txt created - Header fist line ","File Information",MB_OK);

	}


	// TODO: Add your control notification handler code here
}

void SP_Retrieve::OnBnClickedButonImport()
{
	SP_ImportDlg dlg;
	if(dlg.DoModal()) {


	}
	// TODO: Add your control notification handler code here
}


void SP_Retrieve::OnBnClickedOk()
{
	UpdateData(TRUE);
	int layer,prof,repNumber;
	layer=m_Layer;
	repNumber=m_RepNumber;
	prof=m_Prof;
//	m_pDoc->SP_ResetComments();
	//PLOTPF_mp_ACTUALPROFILE=0;
	//PLOTPF_mp_GETLAYER(&prof,&repNumber,&layer);
	p_PFCurve->SetHeader(prof, repNumber);

	string str=GetCurrentName();

	if(m_pDB!=nullptr&& p_PFCurve->IsProfile()){

		m_pSimDoc->GetProfileFromPlotPF(false,false);
		m_NumLayers=FUtil::STD_ItoAscii(int(p_PFCurve->m_ActProfile.NumLayers));
		m_pDoc->DB_SetChoice(Soil_Properties, str);
		m_SelectedId=str;

		if(m_NumToBeSelected<=m_pDB->MR_GetNumberOfSelections()) {
			m_Prof_ToSim.SaveToSim();
			string test;
			BOOL Testkey;
			Testkey=TRUE;
			int layer,prof,repNumber,index;
			m_pDB->SetName("Soil Properties");
			m_pDB->SetValue(string(m_SelectedId));

			for(size_t i=0;i<m_NumToBeSelected;i++) {
				test=m_pDB->MR_GetKeySelection(i).c_str();
		
				index=test.find(":");
				if(index>0) {
					prof=MFC_Util::AtoInt(test.substr(2,index-2));
					repNumber=MFC_Util::AtoInt(test.substr(index+1,2));
					p_PFCurve->GetHeader(prof, repNumber);
				}
				else {
					Testkey = FALSE;
				}
				layer=1;
				
				if(p_PFCurve->m_ActProfile.NumLayers<=0)
					Testkey=FALSE;
			}
		
			if(!Testkey) {
				string string;
				string="";
				string+="Use right key on mouse to copy Profile no from data base list\n";
				string+="and make paste on the selection list of profiles\n";
				string+="All cells right of (Simulation No:) must have a key with xxx:yy\n";
				string+="xxx is profile no and yy is replicate profile number\n";
				MFC_Util::MessageBox(string,"Soil Profile Keys are not correct", 0);
				return;
				// Problems


			}
		
		}
		else {
			return;
			//Problems

		}

	}

	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}


void SP_Retrieve::OnEnSetfocusEdit23()
{
	// TODO: Add your control notification handler code here
}


void SP_Retrieve::OnEnChangeEditComments()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString cstr;
	GetDlgItem(IDC_COMMENT1)->GetWindowText(cstr);
	m_Details = CT2A(cstr).m_psz;
	p_PFCurve->SetComments(string(m_Details));
	//m_pDoc->m_Doc.SP_SetComments(string(m_Details));
}


void SP_Retrieve::OnBnClickedOkbutton()
{
	p_PFCurve->SaveNewFormat();
	p_PFCurve->CloseMainDataBase();
	// TODO: Add your control notification handler code here
}
void SP_Retrieve::IniDataBase() {
	if(!p_PFCurve->IsOpen()) {
	  string directory;
	  directory=MFC_Util::GetProfileStringStd("DataBaseDirectory", directory);
	 if(!p_PFCurve->OpenMainFile(directory+"PfProf")) {
		 return;
	 }
	 
	}
	if(p_PFCurve->NewFormatFile()) {
		p_PFCurve->ReadHeaderFile();
		m_ActHeader=p_PFCurve->GetHeader(0);
		p_PFCurve->SetHeader(0);
		//m_ActHeader=p_PFCurve->GetHeader(p_PFCurve->GetInsertRecord(0));
	}
	else if(!p_PFCurve->NewFormatFile()) {
		p_PFCurve->OpenEditMainDataBase();
		p_PFCurve->ReadHeaderFile();
		p_PFCurve->SaveNewFormat();
		p_PFCurve->CloseMainDataBase();
		 string directory;
		directory=MFC_Util::GetProfileStringStd("DataBaseDirectory", directory);
		p_PFCurve->OpenMainFile(directory+"PfProf");
		m_ActHeader=p_PFCurve->GetHeader(p_PFCurve->GetInsertRecord(0));
	}
		if(m_ActHeader.RecHeader>0) {
	   m_IniProf=m_ActHeader.KeyProf/100;
	   m_IniRepProf=m_ActHeader.KeyProf%100;
	}
	else
		m_IniProf=-10;

}