// DB_SELECT.cpp : implementation file
//

#include "stdafx.h"
//#include "./CUG/MyCug.h"
#include "CoupModel.h"
#include "CoupModelDoc.h"
#include "CoupModelView.h"
#include "PP_Edit.h"
#include "SP_Retrieve.h"
#include "DB_SELECT.h"
#include "ChaParDuringRun.h"
#include "MultiRunDB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DB_SELECT dialog


DB_SELECT::DB_SELECT(CWnd* pParent /*=NULL*/)
	: CDialog(DB_SELECT::IDD, pParent)
{
}

DB_SELECT::DB_SELECT(DB_SelTypes index,CCoupModelDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(DB_SELECT::IDD, pParent)
{
	m_pDoc=pDoc;
	m_SelType=index;
	m_ReAllocateSoil=FALSE;
	m_ApplyPlant=FALSE;
	m_ApplySnow=FALSE;
	m_ApplySoilEvaporation=FALSE;
	m_ApplySoilFrost=FALSE;
	m_ApplyWaterFlow=FALSE;
	m_ApplyGrowth=FALSE;
	m_ApplyInorganic=FALSE;
	m_ApplyOrganic=FALSE;
	m_ApplySoilManage=FALSE;

	m_pEditPlant=NULL;
	m_pEditSnow=NULL;
	m_pEditSoilEvap=NULL;
	m_pEditSoilFrost=NULL;
	m_pEditWaterF=NULL;
	m_pEditGrowth=NULL;
	m_pEditSoilManage=NULL;
	m_pEditInorganic=NULL;
	m_pEditOrganic=NULL;
  
	m_SelPlant		=m_pDoc->DB_GetChoice(Plant);
	m_SelSoilProp	=m_pDoc->DB_GetChoice(Soil_Properties);
	m_SelSoilEvap	=m_pDoc->DB_GetChoice(Soil_Evaporation);
	m_SelSoilFrost	=m_pDoc->DB_GetChoice(Soil_Frost);
	m_SelSnow		=m_pDoc->DB_GetChoice(Snow);
	m_SelDrain		=m_pDoc->DB_GetChoice(Soil_Water_Flows);
	m_SelGrowth		=m_pDoc->DB_GetChoice(Plant_Growth);
	m_SelSoilManage	=m_pDoc->DB_GetChoice(Soil_Manage);
	m_SelInorganic	=m_pDoc->DB_GetChoice(Soil_InOrganic);
	m_SelOrganic	=m_pDoc->DB_GetChoice(Soil_Organic);

}
void DB_SELECT::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DB_SELECT)
	DDX_Text(pDX, IDC_EDIT_DRAIN, m_SelDrain);
	DDX_Text(pDX, IDC_EDIT_PLANT, m_SelPlant);
	DDX_Text(pDX, IDC_EDIT_SNOW, m_SelSnow);
	DDX_Text(pDX, IDC_EDIT_SOIL_EVAP, m_SelSoilEvap);
	DDX_Text(pDX, IDC_EDIT_SOIL_FROST, m_SelSoilFrost);
	DDX_Text(pDX, IDC_EDIT_SOIL_PROP, m_SelSoilProp);
	DDX_Text(pDX, IDC_EDIT_GROWTH, m_SelGrowth);
	DDX_Text(pDX, IDC_EDIT_DB_ORGANIC, m_SelOrganic);
	DDX_Text(pDX, IDC_EDIT_DB_INORGANIC, m_SelInorganic);
	DDX_Text(pDX, IDC_EDIT_DB_SOILMANAGE, m_SelSoilManage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DB_SELECT, CDialog)
	//{{AFX_MSG_MAP(DB_SELECT)
	ON_BN_CLICKED(IDC_DB_PLANT, OnDbPlant)
	ON_BN_CLICKED(IDC_DB_SNOW, OnDbSnow)
	ON_BN_CLICKED(IDC_DB_SOIL_PROPERTIES, OnDbSoilProperties)
	ON_BN_CLICKED(IDC_DB_SOIL_EVAPORATION, OnDbSoilEvaporation)
	ON_BN_CLICKED(IDC_DB_SOIL_FROST, OnDbSoilFrost)
	ON_BN_CLICKED(IDC_DB_WATER_FLOW, OnDbWaterFlow)
	ON_UPDATE_COMMAND_UI(IDC_DB_SOIL_PROPERTIES, OnUpdateSoilProperties)
	ON_BN_CLICKED(IDC_DB_GROWTH, OnDbGrowth)
	ON_BN_CLICKED(IDC_DB_SOILMANAGE, OnDbSoilmanage)
	ON_BN_CLICKED(IDC_DB_ORGANIC, OnDbOrganic)
	ON_BN_CLICKED(IDC_DB_INORGANIC, OnDbInorganic)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DB_SELECT message handlers

void DB_SELECT::OnDbPlant() 
{
	// TODO: Add your control notification handler code here
	if(m_SelType==SelChaPar) {
		CDB* pDB;
		pDB=m_pDoc->DB_GetPointer(Plant);
		CChaParDuringRun Dlg(pDB,m_pDoc);
	    Dlg.DoModal(); 
		m_ReAllocate=FALSE;
		m_pEditPlant=NULL;
	}
	else  if(m_SelType==SelMulti){
 		CDB* pDB;
		pDB=m_pDoc->DB_GetPointer(Plant);
		MultiRunDB Dlg(pDB, m_pDoc);
	    Dlg.DoModal(); 
		m_ReAllocate=FALSE;
		m_pEditPlant=NULL;
	}
	else {
		m_pEditPlant= new PP_EDIT("Plant",m_pDoc);
		m_pEditPlant->m_ReadOnly=TRUE;
		m_pEditPlant->m_TotalReadOnly=TRUE;
		if(m_pEditPlant->DoModal()==IDOK){
			m_SelPlant=m_pEditPlant->m_SelectedId;
			m_ApplyPlant=TRUE;
			m_ReAllocate=TRUE;
		}
		else {
			delete m_pEditPlant;
			m_pEditPlant=NULL;
		}
	}
	OnInitDialog();
}

void DB_SELECT::OnDbSnow() 
{
	if(m_SelType==SelChaPar) {
		CDB* pDB;
		pDB=m_pDoc->DB_GetPointer(Snow);
		CChaParDuringRun Dlg(pDB,m_pDoc);
	    Dlg.DoModal(); 
		m_ReAllocate=FALSE;
		m_pEditPlant=NULL;
	}
	else  if(m_SelType==SelMulti){
 		CDB* pDB;
		pDB=m_pDoc->DB_GetPointer(Snow);
		MultiRunDB Dlg(pDB, m_pDoc);
	    Dlg.DoModal(); 
		m_ReAllocate=FALSE;
		m_pEditPlant=NULL;
	}
	else {
		m_pEditSnow= new PP_EDIT("Snow",m_pDoc);
		m_pEditSnow->m_ReadOnly=TRUE;
		m_pEditSnow->m_TotalReadOnly=TRUE;
		if(m_pEditSnow->DoModal()==IDOK){
			m_SelSnow=m_pEditSnow->m_SelectedId;
			m_ApplySnow=TRUE;
			m_ReAllocate=TRUE;
		}
		else {
			delete m_pEditSnow;
			m_pEditSnow=NULL;
		}
	}
	OnInitDialog();
}
void DB_SELECT::OnDbSoilEvaporation() 
{
	if(m_SelType==SelChaPar) {
		CDB* pDB;
		pDB=m_pDoc->DB_GetPointer(Soil_Evaporation);
		CChaParDuringRun Dlg(pDB,m_pDoc);
	    Dlg.DoModal(); 
		m_ReAllocate=FALSE;
		m_pEditPlant=NULL;
	}
	else  if(m_SelType==SelMulti){
 		CDB* pDB;
		pDB=m_pDoc->DB_GetPointer(Soil_Evaporation);
		MultiRunDB Dlg(pDB, m_pDoc);
	    Dlg.DoModal(); 
		m_ReAllocate=FALSE;
		m_pEditPlant=NULL;
	}
	else {
		m_pEditSoilEvap= new PP_EDIT("Soil evaporation",m_pDoc);
		m_pEditSoilEvap->m_ReadOnly=TRUE;
		m_pEditSoilEvap->m_TotalReadOnly=TRUE;
		if(m_pEditSoilEvap->DoModal()==IDOK){
			m_SelSoilEvap=m_pEditSoilEvap->m_SelectedId;
			m_ApplySoilEvaporation=TRUE;
			m_ReAllocate=TRUE;
		}
		else {
			delete m_pEditSoilEvap;
			m_pEditSoilEvap=NULL;
		}
	}
	OnInitDialog();
}

void DB_SELECT::OnDbSoilFrost() 
{
	if(m_SelType==SelChaPar) {
		CDB* pDB;
		pDB=m_pDoc->DB_GetPointer(Soil_Frost);
		CChaParDuringRun Dlg(pDB,m_pDoc);
	    Dlg.DoModal(); 
		m_ReAllocate=FALSE;
		m_pEditPlant=NULL;
	}
	else  if(m_SelType==SelMulti){
 		CDB* pDB;
		pDB=m_pDoc->DB_GetPointer(Soil_Frost);
		MultiRunDB Dlg(pDB, m_pDoc);
	    Dlg.DoModal(); 
		m_ReAllocate=FALSE;
		m_pEditPlant=NULL;
	}
	else {
		m_pEditSoilFrost= new PP_EDIT("Soil frost",m_pDoc);
		m_pEditSoilFrost->m_ReadOnly=TRUE;
		m_pEditSoilFrost->m_TotalReadOnly=TRUE;
		if(m_pEditSoilFrost->DoModal()==IDOK){
			m_SelSoilFrost=m_pEditSoilFrost->m_SelectedId;
			m_ApplySoilFrost=TRUE;
			m_ReAllocate=TRUE;
		}
		else {
			delete m_pEditSoilFrost;
			m_pEditSoilFrost=NULL;
		}
	}
	OnInitDialog();
}

void DB_SELECT::OnDbWaterFlow() 
{
	if(m_SelType==SelChaPar) {
		CDB* pDB;
		pDB=m_pDoc->DB_GetPointer(Soil_Water_Flows);
		CChaParDuringRun Dlg(pDB,m_pDoc);
	    Dlg.DoModal(); 
		m_ReAllocate=FALSE;
		m_pEditPlant=NULL;
	}
	else  if(m_SelType==SelMulti){
 		CDB* pDB;
		pDB=m_pDoc->DB_GetPointer(Soil_Water_Flows);
		MultiRunDB Dlg(pDB, m_pDoc);
	    Dlg.DoModal(); 
		m_ReAllocate=FALSE;
		m_pEditPlant=NULL;
	}
	else {
		m_pEditWaterF= new PP_EDIT("Soil water flows",m_pDoc);
		m_pEditWaterF->m_ReadOnly=TRUE;
		m_pEditWaterF->m_TotalReadOnly=TRUE;
		if(m_pEditWaterF->DoModal()==IDOK){
			m_SelDrain=m_pEditWaterF->m_SelectedId;
			m_ApplyWaterFlow=TRUE;
			m_ReAllocate=TRUE;
		}
		else {
			delete m_pEditWaterF;
			m_pEditWaterF=NULL;
		}
	}
	OnInitDialog();
}

void DB_SELECT::OnOK() 
{
	// TODO: Add extra validation here
	if(m_ReAllocateSoil) {
		CTab* pCTab;
		CBasePtrArray *arr;
		CString Name="Hydraulic conductivity, measured horizons";
		CString Type="Tables";
		CString Group="Soil Hydraulic";
		CFortranMap *p_fmap;
		p_fmap=m_pDoc->GetDocFortranMap();
		pCTab=(CTab*)p_fmap->reg.GetPtr(Name, Type, Group, arr);
		pCTab->SetValue(m_NumSoilLayers);
		pCTab->Apply_CE_only();
		m_ReAllocate=TRUE;
	}
	if(m_pEditPlant!=NULL) {
		CString string=m_SelPlant;
		string+="\n Use \"Edit menus : Options, Parameters and Parameter Tables \"  \nto view and/or modify the selected properties"; 
		MessageBox(string,"Plant characteristics downloaded from database");
		m_pEditPlant->DB_Actual();
		delete m_pEditPlant;
	}
	if(m_pEditSnow!=NULL) {
		CString string=m_SelSnow;
		string+="\n Use \"Edit menus : Options, Parameters and Parameter Tables \"  \nto view and/or modify the selected properties"; 
		MessageBox(string,"Snow characteristics downloaded from database");
		m_pEditSnow->DB_Actual();
		delete m_pEditSnow;
	}
	if(m_pEditSoilEvap!=NULL) {
		CString string=m_SelSoilEvap;
		string+="\n Use \"Edit menus : Options, Parameters and Parameter Tables \"  \nto view and/or modify the selected properties"; 
		MessageBox(string,"Soil evaporation characteristics downloaded from database");
		m_pEditSoilEvap->DB_Actual();
		delete m_pEditSoilEvap;
	}
	if(m_pEditSoilFrost!=NULL) {
		CString string=m_SelSoilFrost;
		string+="\n Use \"Edit menus : Options, Parameters and Parameter Tables \"  \nto view and/or modify the selected properties"; 
		MessageBox(string,"Soil frost characteristics downloaded from database");
		m_pEditSoilFrost->DB_Actual();
		delete m_pEditSoilFrost;
	}
	if(m_pEditWaterF!=NULL) {
		CString string=m_SelDrain;
		string+="\n Use \"Edit menus : Options, Parameters and Parameter Tables \"  \nto view and/or modify the selected properties"; 
		MessageBox(string,"Soil water flow and drainage characteristics downloaded from database");
		m_pEditWaterF->DB_Actual();
		delete m_pEditWaterF;
	}
	if(m_pEditGrowth!=NULL) {
		CString string=m_SelGrowth;
		string+="\n Use \"Edit menus : Options, Parameters and Parameter Tables \"  \nto view and/or modify the selected properties"; 
		MessageBox(string,"Plant Growth characteristic downloaded from database");
		m_pEditGrowth->DB_Actual();
		delete m_pEditGrowth;
	}
	if(m_pEditSoilManage!=NULL) {
		CString string=m_SelSoilManage;
		string+="\n Use \"Edit menus : Options, Parameters and Parameter Tables \"  \nto view and/or modify the selected properties"; 
		MessageBox(string,"Site Management characteristics downloaded from database");
		m_pEditSoilManage->DB_Actual();
		delete m_pEditSoilManage;
	}
	if(m_pEditOrganic!=NULL) {
		CString string=m_SelOrganic;
		string+="\n Use \"Edit menus : Options, Parameters and Parameter Tables \"  \nto view and/or modify the selected properties"; 
		MessageBox(string,"Soil oganic characteristics downloaded from database");
		m_pEditOrganic->DB_Actual();
		delete m_pEditOrganic;
	}
	if(m_pEditInorganic!=NULL) {
		CString string=m_SelInorganic;
		string+="\n Use \"Edit menus : Options, Parameters and Parameter Tables \"  \nto view and/or modify the selected properties"; 
		MessageBox(string,"Soil inorganic characteristics downloaded from database");
		m_pEditInorganic->DB_Actual();
		delete m_pEditInorganic;
	}

	if(m_ReAllocate) {
		m_pDoc->ReallocateTheWholeMap();
	}

	CDialog::OnOK();
}

void DB_SELECT::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void DB_SELECT::OnDbSoilProperties() 
{
	// TODO: Add your control notification handler code here
	if(m_SelType==SelChaPar) {
		/*CDB* pDB;
		pDB=m_pDoc->DB_GetPointer(Soil_Properties);
		CChaParDuringRun Dlg(pDB,m_pDoc);
	    Dlg.DoModal(); 
		m_ReAllocate=FALSE;
		m_pEditPlant=NULL;*/
	}
	else  if(m_SelType==SelMulti){
 		CDB* pDB;
		pDB=m_pDoc->DB_GetPointer(Soil_Properties);
		MultiRunDB Dlg(pDB, m_pDoc);
	    Dlg.DoModal(); 
		m_ReAllocate=FALSE;
		m_pEditPlant=NULL;
	}
	else {	
		SP_Retrieve dlg(TRUE, m_pDoc);
		int resultTab=dlg.DoModal();
		if(resultTab==IDOK) {
			m_ReAllocateSoil=TRUE;
			m_SelSoilProp=dlg.m_SelectedId;
			m_NumSoilLayers=AtoInt(dlg.m_NumLayers);
		}
	}
	OnInitDialog();
}

BOOL DB_SELECT::OnInitDialog() 
{
   CDialog::OnInitDialog();
   int numTab=5;
   LPINT tabstops;
   int	tabstop[5];
	CFortranMap *p_fmap;							  
	p_fmap=m_pDoc->GetDocFortranMap();
	CBasePtrArray *arr;
	CString Name, Group, Type;

	Name="WaterEq";
	Group="Model Structure";
	Type="Switches";
    pWaterEq=(CSw*)p_fmap->reg.GetPtr(Name,Type,Group, arr);
	Name="Nitrogen and Carbon";
    pNitrogen=(CSw*)p_fmap->reg.GetPtr(Name,Type,Group, arr);



   tabstop[0]=80;
   tabstop[1]=140;
   tabstop[2]=195;
   tabstop[3]=240;
   tabstop[4]=260;
   tabstops=&tabstop[0];
   ((CEdit*)GetDlgItem(IDC_EDIT_PLANT))->SetTabStops(numTab, tabstops);
   ((CEdit*)GetDlgItem(IDC_EDIT_SOIL_EVAP))->SetTabStops(numTab, tabstops);
   ((CEdit*)GetDlgItem(IDC_EDIT_SOIL_FROST))->SetTabStops(numTab, tabstops);
   ((CEdit*)GetDlgItem(IDC_EDIT_SOIL_PROP))->SetTabStops(numTab, tabstops);
   ((CEdit*)GetDlgItem(IDC_EDIT_SNOW))->SetTabStops(numTab, tabstops);
   ((CEdit*)GetDlgItem(IDC_EDIT_DRAIN))->SetTabStops(numTab, tabstops);
   ((CEdit*)GetDlgItem(IDC_EDIT_GROWTH))->SetTabStops(numTab, tabstops);
   ((CEdit*)GetDlgItem(IDC_EDIT_DB_SOILMANAGE))->SetTabStops(numTab, tabstops);
   ((CEdit*)GetDlgItem(IDC_EDIT_DB_ORGANIC))->SetTabStops(numTab, tabstops);
   ((CEdit*)GetDlgItem(IDC_EDIT_DB_INORGANIC))->SetTabStops(numTab, tabstops);
   GetDlgItem(IDC_EDIT_PLANT)->Invalidate(); 
	
	CDialog::OnInitDialog();
	CString str;
	if(m_SelType==SelCurrent) {
		str="Select a setup based on predefined values from data base";
	    GetDlgItem(IDC_DB_SOIL_PROPERTIES)->ModifyStyle(WS_DISABLED,0);
	}
	else if(m_SelType==SelChaPar)  {
		str="Select a record of predefined values from data base for changes within run";
	    GetDlgItem(IDC_DB_SOIL_PROPERTIES)->ModifyStyle(0,WS_DISABLED);
	}
	else {
	    GetDlgItem(IDC_DB_SOIL_PROPERTIES)->ModifyStyle(WS_DISABLED,0);
		str="Select a record of predefined values from data base for changes between runs";
	}
	SetWindowText(str);
	if(*(int*)((CSw*)pNitrogen)->GetValue()>0) {
	 	GetDlgItem(IDC_DB_GROWTH)->	   	ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_DB_SOILMANAGE)->	ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_DB_ORGANIC)->   	ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_DB_INORGANIC)-> 	ModifyStyle(WS_DISABLED,0);
	}
	else {
	 	GetDlgItem(IDC_DB_GROWTH)->	   	ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_DB_SOILMANAGE)->	ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_DB_ORGANIC)->   	ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_DB_INORGANIC)-> 	ModifyStyle(0,WS_DISABLED);
	}
 	if(*(int*)((CSw*)pWaterEq)->GetValue()>0) {
		GetDlgItem(IDC_DB_PLANT)->				ModifyStyle(WS_DISABLED,0);					
		GetDlgItem(IDC_DB_SOIL_PROPERTIES)->	ModifyStyle(WS_DISABLED,0);	
		GetDlgItem(IDC_DB_SOIL_EVAPORATION)->	ModifyStyle(WS_DISABLED,0);	
		GetDlgItem(IDC_DB_SOIL_FROST)->			ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_DB_WATER_FLOW)->			ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_DB_SNOW)->  				ModifyStyle(WS_DISABLED,0);
	}
		else {	
		GetDlgItem(IDC_DB_PLANT)->				ModifyStyle(0,WS_DISABLED);					
		GetDlgItem(IDC_DB_SOIL_PROPERTIES)->	ModifyStyle(0,WS_DISABLED);	
		GetDlgItem(IDC_DB_SOIL_EVAPORATION)->	ModifyStyle(0,WS_DISABLED);	
		GetDlgItem(IDC_DB_SOIL_FROST)->			ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_DB_WATER_FLOW)->			ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_DB_SNOW)->  				ModifyStyle(0,WS_DISABLED);
	}




	GetDlgItem(IDC_DB_PLANT)->			RedrawWindow();
	GetDlgItem(IDC_DB_SOIL_PROPERTIES)->RedrawWindow();
	GetDlgItem(IDC_DB_SOIL_EVAPORATION)->RedrawWindow();
	GetDlgItem(IDC_DB_SOIL_FROST)->		RedrawWindow();
	GetDlgItem(IDC_DB_WATER_FLOW)->		RedrawWindow();
	GetDlgItem(IDC_DB_SNOW)->			RedrawWindow();
	GetDlgItem(IDC_DB_GROWTH)->RedrawWindow();
	GetDlgItem(IDC_DB_SOILMANAGE)->RedrawWindow();
	GetDlgItem(IDC_DB_ORGANIC)->RedrawWindow();
	GetDlgItem(IDC_DB_INORGANIC)->RedrawWindow();




	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void DB_SELECT::OnUpdateSoilProperties(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_SelType!=SelCurrent);
}

void DB_SELECT::OnDbGrowth() 
{
	if(m_SelType==SelChaPar) {
		CDB* pDB;
		pDB=m_pDoc->DB_GetPointer(Plant_Growth);
		pDB->SetName("Plant Growth");
		CChaParDuringRun Dlg(pDB,m_pDoc);
	    Dlg.DoModal(); 
		m_ReAllocate=FALSE;
		m_pEditPlant=NULL;
	}
	else  if(m_SelType==SelMulti){
 		CDB* pDB;
		pDB=m_pDoc->DB_GetPointer(Plant_Growth);
		pDB->SetName("Plant Growth");
		MultiRunDB Dlg(pDB, m_pDoc);
	    Dlg.DoModal(); 
		m_ReAllocate=FALSE;
		m_pEditPlant=NULL;
	}
	else {
		m_pEditGrowth= new PP_EDIT("Plant Growth",m_pDoc);
		m_pEditGrowth->m_ReadOnly=TRUE;
		m_pEditGrowth->m_TotalReadOnly=TRUE;
		if(m_pEditGrowth->DoModal()==IDOK){
			m_SelGrowth=m_pEditGrowth->m_SelectedId;
			m_ApplyGrowth=TRUE;
			m_ReAllocate=TRUE;
		}
		else {
			delete m_pEditGrowth;
			m_pEditGrowth=NULL;
		}
	}
	OnInitDialog();
	
}

void DB_SELECT::OnDbSoilmanage() 
{
	// TODO: Add your control notification handler code here
	if(m_SelType==SelChaPar) {
		CDB* pDB;
		pDB=m_pDoc->DB_GetPointer(Soil_Manage);
		CChaParDuringRun Dlg(pDB,m_pDoc);
	    Dlg.DoModal(); 
		m_ReAllocate=FALSE;
		m_pEditPlant=NULL;
	}
	else  if(m_SelType==SelMulti){
 		CDB* pDB;
		pDB=m_pDoc->DB_GetPointer(Soil_Manage);
		MultiRunDB Dlg(pDB, m_pDoc);
	    Dlg.DoModal(); 
		m_ReAllocate=FALSE;
		m_pEditPlant=NULL;
	}
	else {
		m_pEditSoilManage= new PP_EDIT("Site Management",m_pDoc);
		m_pEditSoilManage->m_ReadOnly=TRUE;
		m_pEditSoilManage->m_TotalReadOnly=TRUE;
		if(m_pEditSoilManage->DoModal()==IDOK){
			m_SelSoilManage=m_pEditSoilManage->m_SelectedId;
			m_ApplySoilManage=TRUE;
			m_ReAllocate=TRUE;
		}
		else {
			delete m_pEditSoilManage;
			m_pEditSoilManage=NULL;
		}
	}
	OnInitDialog();
	
}

void DB_SELECT::OnDbOrganic() 
{
	// TODO: Add your control notification handler code here
	if(m_SelType==SelChaPar) {
		CDB* pDB;
		pDB=m_pDoc->DB_GetPointer(Soil_Organic);
		CChaParDuringRun Dlg(pDB,m_pDoc);
	    Dlg.DoModal(); 
		m_ReAllocate=FALSE;
		m_pEditPlant=NULL;
	}
	else  if(m_SelType==SelMulti){
 		CDB* pDB;
		pDB=m_pDoc->DB_GetPointer(Soil_Organic);
		MultiRunDB Dlg(pDB, m_pDoc);
	    Dlg.DoModal(); 
		m_ReAllocate=FALSE;
		m_pEditPlant=NULL;
	}
	else {
		m_pEditOrganic= new PP_EDIT("Soil organic processes",m_pDoc);
		m_pEditOrganic->m_ReadOnly=TRUE;
		m_pEditOrganic->m_TotalReadOnly=TRUE;
		if(m_pEditOrganic->DoModal()==IDOK){
			m_SelOrganic=m_pEditOrganic->m_SelectedId;
			m_ApplyOrganic=TRUE;
			m_ReAllocate=TRUE;
		}
		else {
			delete m_pEditOrganic;
			m_pEditOrganic=NULL;
		}
	}
	OnInitDialog();
	
}

void DB_SELECT::OnDbInorganic() 
{
	// TODO: Add your control notification handler code here
	if(m_SelType==SelChaPar) {
		CDB* pDB;
		pDB=m_pDoc->DB_GetPointer(Soil_InOrganic);
		CChaParDuringRun Dlg(pDB,m_pDoc);
	    Dlg.DoModal(); 
		m_ReAllocate=FALSE;
		m_pEditPlant=NULL;
	}
	else  if(m_SelType==SelMulti){
 		CDB* pDB;
		pDB=m_pDoc->DB_GetPointer(Soil_InOrganic);
		MultiRunDB Dlg(pDB, m_pDoc);
	    Dlg.DoModal(); 
		m_ReAllocate=FALSE;
		m_pEditPlant=NULL;
	}
	else {
		m_pEditInorganic= new PP_EDIT("Soil mineral N processes",m_pDoc);
		m_pEditInorganic->m_ReadOnly=TRUE;
		m_pEditInorganic->m_TotalReadOnly=TRUE;
		if(m_pEditInorganic->DoModal()==IDOK){
			m_SelInorganic=m_pEditInorganic->m_SelectedId;
			m_ApplyInorganic=TRUE;
			m_ReAllocate=TRUE;
		}
		else {
			delete m_pEditInorganic;
			m_pEditInorganic=NULL;
		}
	}
	OnInitDialog();
	
}
