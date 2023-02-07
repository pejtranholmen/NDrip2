// MultiRunPar.cpp : implementation file
//



#include "../stdafx.h"
#include "../CUG\MyCug.h"
#include "../CoupModel.h"
#include "../CoupModelDoc.h"
#include "../CoupModelView.h"
#include "MultiRunPar.h"
#include "MultiRunTable.h"
#include "../Model\SimB.h"
//#include "./multirunpar.h"
#include "DimNewValue.h"
#include "../MainFrm.h"
#include "../SP\SP_Retrieve.h"
#include "./multirunpar.h"
#include "../BasicExcel\BasicExcel.hpp"
using namespace YExcel;

extern "C" float  BAYESIANCALIB_mp_ACTUALP;
extern "C" float  BAYESIANCALIB_mp_ACTUALPMIN;
extern "C" float  BAYESIANCALIB_mp_ACTUALPMAX;
extern "C" int  BAYESIANCALIB_mp_INDEXP;
extern "C" void BAYESIANCALIB_mp_GETBAYESIAN_PVALUES();
extern "C" void BAYESIANCALIB_mp_CALIB_DEF();
extern "C" void BAYESIANCALIB_mp_CALIB_INI();
extern "C" void BAYESIANCALIB_mp_CALIB_RESET();
extern "C" void BAYESIANCALIB_mp_CALIB_UPDATE();
extern "C" float BAYESIANCALIB_mp_CALIB_GETVALUE();
extern "C" void BAYESIANCALIB_mp_CALIB_CHECK();
extern "C" int  BAYESIANCALIB_mp_NP;
extern "C" int  BAYESIANCALIB_mp_CHAINLENGTH;
extern "C" double BAYESIANCALIB_mp_STEPSIZE;
extern "C" int	BAYESIANCALIB_mp_BESTRUNNO;
extern "C" double GENERAL_mp_LOGL1;
extern "C" double VALIDATION_mp_EXTRA_LIKELIHOOD;
extern "C" int BAYESIANCALIB_mp_INDEXP2;
extern "C" float BAYESIANCALIB_mp_GETCOVAR();
extern "C" float BAYESIANCALIB_mp_GETCOCOR();
extern "C" float BAYESIANCALIB_mp_GETMEAN();
extern "C" int	 BAYESIANCALIB_mp_INDEXRUN;
extern "C" int	 BAYESIANCALIB_mp_NUMIMPROVEDRUN;
extern "C" int	 BAYESIANCALIB_mp_NUMBURNPERIOD;
extern	"C" void BAYESIANCALIB_mp_SETCOVPROPOSAL();
extern	"C" void BAYESIANCALIB_mp_SETPCHAIN();
extern	"C" void BAYESIANCALIB_mp_COVARIANCE();

// MultiRunPar dialog

IMPLEMENT_DYNAMIC(MultiRunPar, CDialog)
MultiRunPar::MultiRunPar(CCoupModelDoc* pDoc, NewMap *p_fmap, BOOL TableType, CWnd* pParent /*=NULL*/)
	: CDialog(MultiRunPar::IDD, pParent)
	, m_TableIndexValue2(0)
	, m_ParMinValue(_T(""))
	, m_ParMaxValue(_T(""))
	, m_ParMethod(MR_METHOD(0))
	, m_DimensionSelected(0)
	, m_ParName(_T(""))
	, m_ParGroup(_T(""))
	, m_TableName(_T(""))
	, m_TotalNumberOfRuns(_T(""))
{
	m_pDoc=pDoc;
	mp_fmap=p_fmap;
	m_IsTable=TableType;
	m_TableIndexValue2=0;
	if(m_IsTable)
		m_TabIndex=-1;
	m_IsDatabase=FALSE;
	m_ShiftMode=TRUE;
	m_DepIndex=-1;
	m_pPs=NULL;
	m_StepSize=m_pDoc->m_Doc.m_BayesianStepSize;


}

MultiRunPar::~MultiRunPar()
{
}

void MultiRunPar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO2, m_TableIndex);
	int t=int(m_TableIndexValue2);
	DDX_CBIndex(pDX, IDC_COMBO2, t);
	DDX_Control(pDX, IDC_LIST1, m_ParList);
	DDX_Text(pDX, IDC_EDIT1, m_ParMinValue);
	DDX_Text(pDX, IDC_EDIT2, m_ParMaxValue);
	int mr_method=MR_METHOD(m_ParMethod);
	DDX_CBIndex(pDX, IDC_COMBO1, mr_method);
	DDX_Control(pDX, IDC_DIMENSIONS, m_Dimensions);
	DDX_LBIndex(pDX, IDC_DIMENSIONS, m_Selected_Dim);
	DDX_Text(pDX, IDC_EDIT3, m_ParName);
	DDX_Text(pDX, IDC_EDIT4, m_ParGroup);
	DDX_Text(pDX, IDC_EDIT5, m_TableName);
	DDX_Control(pDX, IDC_COMBO1, m_MethodList);
	DDX_Text(pDX, IDC_EDIT6, m_TotalNumberOfRuns);
	DDX_Control(pDX, ID_TABLE_VALUES, ctrl_TableValues);
	DDX_Control(pDX, IDC_COMBO3, m_ParDependence);
	DDX_Text(pDX, IDC_EDIT7, m_StartValue);
	DDX_Text(pDX, IDC_EDIT8, m_StepSize);
}


BEGIN_MESSAGE_MAP(MultiRunPar, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnTvnSelchangedTree1)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnCbnSelchangeCombo2)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST1, OnLvnDeleteitemList1)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_LIST1, OnLvnBeginlabeleditList1)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE, OnBnClickedButtonChangeRepetions)
	ON_BN_CLICKED(IDC_BUTTON_Counter, OnBnClickedButtonCounter)
	ON_BN_CLICKED(IDC_BUTTON_NEWDIM, OnBnClickedButtonNewdim)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_DIM, OnBnClickedButtonDeleteDim)
	ON_BN_CLICKED(IDC_BUTTO_APPLY, OnBnClickedButtoApply)
	ON_EN_CHANGE(IDC_EDIT3, OnEnChangeEdit3)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, OnLvnColumnclickList1)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnNMClickList1)
	ON_BN_CLICKED(ID_TABLE_VALUES, OnBnClickedTableValues)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeMethod)
	ON_BN_CLICKED(IDC_BUTTON_TABLE, OnBnClickedButtonTable)
	ON_BN_CLICKED(IDC_BUTTON_NOTABLE, OnBnClickedButtonNotable)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedStartValues)
	ON_CBN_SELCHANGE(IDC_COMBO3, OnCbnSelchangeParDependence)
	ON_LBN_SELCHANGE(IDC_DIMENSIONS, OnLbnSelchangeDimensions)
	ON_BN_CLICKED(IDC_APPLYBAYESIANPARAMETERS, OnBnClickedApplybayesianparameters)
	ON_BN_CLICKED(IDC_BUTTON2, OnExportToExcel)
	ON_BN_CLICKED(IDC_BUTTON3, OnImportFromExcel)
	ON_BN_CLICKED(IDC_BUTTON_NOTABLE2, OnBnClickedDataBaseMode)
	ON_BN_CLICKED(ID_TABLE_VALUES2, OnAddRecord)
END_MESSAGE_MAP()


// MultiRunPar message handlers

BOOL MultiRunPar::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//CWnd* pWnd = GetDlgItem(IDD_MULTI_RUN_PAR);
	//pWnd->
	
	if(m_IsDatabase) 
		SetWindowText(_T("MultiRun Changes - Database objects"));
	else if(m_IsTable) 
		SetWindowText(_T("MultiRun Changes - Parameter Tables"));
	else 
		SetWindowText(_T("MultiRun Changes - Parameters"));

	
	if(m_ShiftMode) {
		OnCreateParTable();
		m_ShiftMode=FALSE;
	}
	
	OnCreateList();
	OnUpdateButtons();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void MultiRunPar::OnCDBNew()
{		
	

	CString m_index, m_Col;
	CString m_Group;
    CDB* pDB;
	CString m_SelectedKey,m_strNewValue;
	int m_select=0;

   CString strID, strIDKey;
   pDB=m_pDoc->m_Doc.DB_GetPointer(string(m_ParName));
   m_ParMethod=MR_METHOD(5);
   if(!m_IsDatabase){
		m_IsDatabase=TRUE;
		m_IsTable=FALSE;
		m_ShiftMode=TRUE;
   }
   m_pPs=(Ps*)pDB;
   if(m_pDoc->m_Doc.MR_CP_Set((Ps*)pDB,-1))
   {
   }
   else {
	   int n;
	   n=pDB->MR_GetNumberOfSelections();
	   for(int i=0;i<n;i++) {
		   m_SelectedKey=((CDB*)m_pPs)->MR_GetKeySelection(i).c_str();
		}
	  	//ej lyckad

   };
    pDB->MR_Set_Method(MR_METHOD(5));

	m_Selected_Dim=pDB->MR_Get_Dim();
	if(m_Selected_Dim<=0) m_Selected_Dim=1;
	
	pDB->MR_Set_Dim(m_Selected_Dim);

	
	//OnCreateDimList();
	//OnCreateMethodList();
	int nRow;
	nRow=m_pDoc->m_Doc.MR_GetDimValue(m_Selected_Dim-1);

		
	//while(pDB->MR_GetNumberOfSelections()<m_pDoc->m_Doc.MR_GetDimValue(m_Selected_Dim)){
	if(pDB->MR_GetNumberOfSelections()<nRow) {
		if(pDB->GetName().find("Soil Properties")>-1) {
			SP_Retrieve dlg(TRUE, m_pDoc);
			int resultTab=dlg.DoModal();
			if(resultTab==IDOK) {
				m_SelectedKey=dlg.m_SelectedId;
				m_strNewValue="Soil";
				pDB->MR_AddSelection(string(m_strNewValue), string(m_SelectedKey));
			}
		}
		else {
  			PP_EDIT *pEditPlant;
			pEditPlant= new PP_EDIT(pDB,m_pDoc,nRow);
			pEditPlant->m_ReadOnly=TRUE;
			pEditPlant->m_TotalReadOnly=TRUE;
			pEditPlant->m_SelectedIndex=m_select;
 			if(pEditPlant->DoModal()==IDOK){
				m_strNewValue=pEditPlant->m_SelectedId;
				m_SelectedKey=pEditPlant->m_SelectedKey;
				m_select=pEditPlant->m_SelectedIndex+1;
			}
			//delete pEditPlant;
			pDB->MR_AddSelection(string(m_strNewValue), string(m_SelectedKey));
		}
	}
		OnUpdateStatus();
		OnUpdateButtons();

		OnCreateDimList();
		//f(m_pPs->GetType()==PAR_SINGLE) 
			m_ParMethod=((Ps*)m_pPs)->MR_Get_Method();
		/*else {
			size_t index=((P*)m_pPs)->G

		}*/
		OnCreateMethodList();
    
		CDialog::OnInitDialog();
		OnBnClickedButtoApply();
		
		}


void MultiRunPar::OnParNew()
{		
	

	CString m_index, m_Col;
	CString m_Group;
	double fmin,fmax;
		m_Type="Parameters";
		if(m_IsTable||m_IsDatabase) {
			m_IsTable=FALSE;
			m_IsDatabase=FALSE;
			m_ShiftMode=TRUE;
		}
		m_pPs=(Ps*)m_pDoc->GetPtr(m_ParName, m_Type, m_ParGroup);
		m_Selected_Dim=m_pPs->MR_Get_Dim();
		OnCreateDimList();
		m_ParMethod=m_pPs->MR_Get_Method();
		OnCreateMethodList();
		OnCreateDependenceList();
		if(m_Selected_Dim<=0) {

			fmin=m_pPs->GetMinValue(); m_ParMinValue=FtoAscii(fmin);
			m_StartValue=m_pPs->GetValue();
			fmax=m_pPs->GetMaxValue(); m_ParMaxValue=FtoAscii(fmax);
		} 
		else	{
			if(m_ParMethod==PAR_SINGLE) {
				m_ParMinValue=FtoAscii(((Ps*)m_pPs)->MR_Get_Min());
				m_StartValue=((Ps*)m_pPs)->MR_Get_Start();
				m_ParMaxValue=FtoAscii(((Ps*)m_pPs)->MR_Get_Max());
			}
			else if(m_ParMethod==PAR_TABLE) {
				m_ParMinValue=FtoAscii(((P*)m_pPs)->MR_Get_Min(0));
				m_StartValue=((P*)m_pPs)->MR_Get_Start(0);
				m_ParMaxValue=FtoAscii(((P*)m_pPs)->MR_Get_Max(0));
			}
			else {
				m_ParMinValue="1";
				m_StartValue=1;
				m_ParMaxValue="1";

			}
		};
		OnUpdateStatus();
		OnUpdateButtons();
    
		CDialog::OnInitDialog();
}
void MultiRunPar::OnUpdateStatus()
{
	float value;
	CString text;
    // get pointer to main frame
    CMainFrame *p_mainframe;
    p_mainframe = (CMainFrame*) AfxGetMainWnd();
	if(!m_IsDatabase) { 
  	  value=m_pPs->GetMinValue();
	  text = FtoAsciiPane(value);
	  p_mainframe->UpdatePaneText(1, text);  
      value=m_pPs->GetWarnMinValue();
	  text = FtoAsciiPane(value);
	  p_mainframe->UpdatePaneText(2, text);
      value=m_pPs->GetWarnMaxValue();
	  text = FtoAsciiPane(value);
	  p_mainframe->UpdatePaneText(3, text);   
      value=m_pPs->GetMaxValue();
	  text = FtoAsciiPane(value);
	  p_mainframe->UpdatePaneText(4, text); 
	  text = m_pPs->GetUnit().c_str();
	  p_mainframe->UpdatePaneText(5, text);
	  CString kalle, lotta;
	  kalle=m_pPs->GetName().c_str();
	  int j=kalle.Find(" ");
	  while (j>-1) {
		lotta+=kalle.Left(j);
	    kalle=kalle.Mid(j+1);
		j=kalle.Find(" ");
	  }
	  lotta+=kalle;
	  lotta.Replace(",",".");
	  m_pDoc->m_Doc.m_HelpName=lotta;
	}
}
void MultiRunPar::OnUpdateButtons()
{
	if(m_Selected_Dim<=0) {
		GetDlgItem(IDC_BUTTON_CHANGE)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_BUTTON_DELETE_DIM)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_BUTTON_Counter)->ModifyStyle(0,WS_DISABLED);
	}
	else {
		GetDlgItem(IDC_BUTTON_CHANGE)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_BUTTON_DELETE_DIM)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_BUTTON_Counter)->ModifyStyle(WS_DISABLED,0);
	}

	GetDlgItem(IDC_BUTTON_CHANGE)->RedrawWindow();
	GetDlgItem(IDC_BUTTON_DELETE_DIM)->RedrawWindow();
	GetDlgItem(IDC_BUTTON_Counter)->RedrawWindow();

	if(m_ParName>" ") 
		GetDlgItem(IDC_BUTTO_APPLY)->ModifyStyle(WS_DISABLED,0);
	else
		GetDlgItem(IDC_BUTTO_APPLY)->ModifyStyle(0,WS_DISABLED);
	GetDlgItem(IDC_BUTTO_APPLY)->RedrawWindow();

	if(m_ParMethod==4){
		GetDlgItem(ID_TABLE_VALUES)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_EDIT1)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT2)->ModifyStyle(0,WS_DISABLED);
		
	}
	else {
		GetDlgItem(ID_TABLE_VALUES)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT1)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_EDIT2)->ModifyStyle(WS_DISABLED,0);
	}
	GetDlgItem(ID_TABLE_VALUES)->RedrawWindow();
	GetDlgItem(IDC_EDIT1)->RedrawWindow();
	GetDlgItem(IDC_EDIT2)->RedrawWindow();

// Dependence Selections
	if(m_ParMethod>5){
		GetDlgItem(IDC_COMBO3)->ModifyStyle(WS_DISABLED,0);
		if(m_ParMethod==6) {
			GetDlgItem(IDC_EDIT1)->ModifyStyle(0,WS_DISABLED);
			GetDlgItem(IDC_EDIT2)->ModifyStyle(0,WS_DISABLED);
			GetDlgItem(IDC_STATIC1)->ModifyStyle(0,WS_DISABLED);
			GetDlgItem(IDC_STATIC2)->ModifyStyle(0,WS_DISABLED);
		}
		else {
			GetDlgItem(IDC_EDIT1)->ModifyStyle(WS_DISABLED,0);
			GetDlgItem(IDC_EDIT2)->ModifyStyle(WS_DISABLED,0);
			GetDlgItem(IDC_STATIC1)->ModifyStyle(WS_DISABLED,0);
			GetDlgItem(IDC_STATIC2)->ModifyStyle(WS_DISABLED,0);
		}
		
	}
	else {
		GetDlgItem(IDC_COMBO3)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT1)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_EDIT2)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_STATIC1)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_STATIC2)->ModifyStyle(WS_DISABLED,0);
	}
	GetDlgItem(IDC_COMBO3)->RedrawWindow();
	GetDlgItem(IDC_STATIC1)->RedrawWindow();
	GetDlgItem(IDC_STATIC2)->RedrawWindow();

	if(m_Type=="Table Parameters")
		GetDlgItem(IDC_COMBO2)->ModifyStyle(WS_DISABLED,0);
	else
		GetDlgItem(IDC_COMBO2)->ModifyStyle(0,WS_DISABLED);
	
	GetDlgItem(IDC_COMBO2)->RedrawWindow();
	GetDlgItem(IDC_COMBO1)->ModifyStyle(WS_DISABLED,0 );
	if(m_IsTable) {
		GetDlgItem(IDC_BUTTON_TABLE)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(ID_TABLE_VALUES2)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_BUTTON_NOTABLE)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_BUTTON_NOTABLE2)->ModifyStyle(WS_DISABLED,0);
	}
	else if(m_IsDatabase) {
		GetDlgItem(IDC_BUTTON_TABLE)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(ID_TABLE_VALUES2)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_BUTTON_NOTABLE)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_BUTTON_NOTABLE2)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT1)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT2)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_COMBO1)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_COMBO3)->ModifyStyle(0,WS_DISABLED);
	}
	else {
		GetDlgItem(IDC_BUTTON_TABLE)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(ID_TABLE_VALUES2)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_BUTTON_NOTABLE)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_BUTTON_NOTABLE2)->ModifyStyle(WS_DISABLED,0);
	}
	GetDlgItem(IDC_BUTTON_TABLE)->RedrawWindow();
	GetDlgItem(ID_TABLE_VALUES2)->RedrawWindow();
	GetDlgItem(IDC_BUTTON_NOTABLE)->RedrawWindow();
	GetDlgItem(IDC_BUTTON_NOTABLE2)->RedrawWindow();
	GetDlgItem(IDC_EDIT1)->RedrawWindow();
	GetDlgItem(IDC_EDIT2)->RedrawWindow();
	GetDlgItem(IDC_COMBO1)->RedrawWindow();
	GetDlgItem(IDC_COMBO3)->RedrawWindow();

	//GetDlgItem(IDC_EDIT2)->RedrawWindow();
	/*GetDlgItem(IDC_STATIC1)->RedrawWindow();
	GetDlgItem(IDC_STATIC2)->RedrawWindow();

	GetDlgItem(ID_TABLE_VALUES)->RedrawWindow();*/
}

void MultiRunPar::OnParTableNew()
{
	
	SimB *pSw;
	CString m_index, m_Col;
	CString m_Group;
		P* pPt;
		m_pPs=NULL;
		m_Type="Table Parameters";
		if(m_IsDatabase||!m_IsTable) {
			m_IsTable=TRUE;
			m_IsDatabase=FALSE;
			m_ShiftMode=TRUE;
		}
		pSw=m_pDoc->GetPtr(m_ParName, m_Type, m_ParGroup);
		if(pSw!=NULL) {
			pPt=(P*)pSw;
			m_pPs=(Ps*)pSw;
			

//			if(pSw->IsKindOf(RUNTIME_CLASS(P))){
			   	int numrow=pPt->GetSize();// Number of rows in table
				if(m_TableIndexValue2>numrow)
					numrow=m_TableIndexValue2+1;
				m_TableIndex.ResetContent();
					for(int ncol=0; ncol<numrow;ncol++) {
					 	m_TableIndex.AddString(ItoAscii(ncol+1));
				}
				m_TableIndex.SetCurSel(m_TableIndexValue2);
//	 		}
			m_TabIndex=m_TableIndexValue2+1;
			m_Selected_Dim=m_pPs->MR_Get_Dim(m_TabIndex);
			OnCreateDimList();
			m_ParMethod=m_pPs->MR_Get_Method(m_TabIndex);
		OnCreateMethodList();
		OnCreateDependenceList();
		if(m_Selected_Dim<=0) {
			float fmin,fmax;
			fmin=m_pPs->GetMinValue(); m_ParMinValue=FtoAscii(fmin);
			fmax=m_pPs->GetMaxValue(fmax); m_ParMaxValue=FtoAscii(fmax);
			if(m_TabIndex<0) 
				m_StartValue=m_pPs->GetValue();
			else
				m_StartValue=((P*)m_pPs)->GetValue(m_TabIndex);
		} 
		else	{
			m_ParMinValue=FtoAscii(((P*)m_pPs)->MR_Get_Min(m_TabIndex));
			m_ParMaxValue=FtoAscii(((P*)m_pPs)->MR_Get_Max(m_TabIndex));
			if(m_TabIndex<0) 
				m_StartValue=((Ps*)m_pPs)->MR_Get_Start();
			else
				m_StartValue=((P*)m_pPs)->MR_Get_Start(m_TabIndex);
		};
		OnUpdateStatus();
		OnUpdateButtons();
		CDialog::OnInitDialog();
		}
		//return ;
}

void MultiRunPar::OnCreateDimList()
{
	CString Text;
	m_Dimensions.ResetContent();
	if(m_pDoc->m_Doc.MR_IsEmpty()){
		m_pDoc->m_Doc.MR_AddDimValue();
		}
	m_Dimensions.AddString("No changes to be done");
	for(int i=0;i<m_pDoc->m_Doc.MR_GetNumberOfDimensions();i++) {
		int num=m_pDoc->m_Doc.MR_GetDimValue(i);
		if(num>0) 
			Text="Dimension "+ItoAscii(i+1)+" -  n = "+ItoAscii(num)+ " ";
		else Text="    ";
		m_Dimensions.AddString(Text);
	}
	if(m_Selected_Dim<0) m_Selected_Dim=0;
    m_Dimensions.SetCurSel(m_Selected_Dim); 
	m_TotalNumberOfRuns=ItoAscii(m_pDoc->m_Doc.MR_Get_TotalNumberofRuns());
}
void MultiRunPar::OnCreateMethodList()
{

	m_MethodList.ResetContent();
	m_MethodList.AddString("Linear Changes");
	m_MethodList.AddString("Logarithmic Changes");
	m_MethodList.AddString("Stochastic Linear Changes");
	m_MethodList.AddString("Stochastic Log Changes");
	m_MethodList.AddString("Table file");
	m_MethodList.AddString("Database Record");
	m_MethodList.AddString("Same As");
	m_MethodList.AddString("Relative Change As");
	m_MethodList.AddString("Opposite Relative As");
	m_MethodList.AddString("Bayesian Calibration");
	m_MethodList.AddString("Connected");
	m_ParMethod=LINEAR_CHANGE;
	m_MethodList.SetCurSel(m_ParMethod);  
}
void MultiRunPar::OnCreateDependenceList()
{

	m_ParDependence.ResetContent();
	m_ParDependentIndex=-1;
	// Loop over selected parameters, specify name
	int icount=0;
	//Vilket beroende finns för m_pPs ?
	SimB* pkoll;
	if(m_pPs->GetType()==PAR_SINGLE)
		pkoll=((Ps*)m_pPs)->Get_MR_ParDependence();
	if(m_TabIndex<0)
		pkoll=((Ps*)m_pPs)->Get_MR_ParDependence();
	else
		pkoll=((P*)m_pPs)->Get_MR_ParDependence(m_TabIndex);

	size_t pindexkoll;
	
	for(size_t i=0;i<m_pDoc->m_Doc.m_MultiRun_Array.size();i++){
	   	for(int ii=0; ii<m_pDoc->m_Doc.MR_Get_NumberParDim(i+1);ii++){
			icount++;
			CString group, name;
			group=m_pDoc->m_Doc.MR_GetParGroup(i+1,ii+1).c_str();
			name=m_pDoc->m_Doc.MR_GetParName(i+1,ii+1).c_str();
			if(m_pDoc->m_Doc.MR_GetTabIndex(i+1,ii+1)>-1) {
				name+="("+ItoAscii(m_pDoc->m_Doc.MR_GetTabIndex(i+1,ii+1))+")";
			}
			if(pkoll==m_pDoc->m_Doc.MR_Get_pPar(i+1,ii+1)){
				if(m_TabIndex<0) 
					pindexkoll=	((Ps*)m_pPs)->Get_MR_ParDependenceIndex();
				else
					pindexkoll=	((P*)m_pPs)->Get_MR_ParDependenceIndex(m_TabIndex);
				if(pindexkoll==m_pDoc->m_Doc.MR_GetTabIndex(i+1,ii+1))
					m_ParDependentIndex=icount-1;
				



			}
			m_ParDependence.AddString(name);
		}
	}

	m_ParDependence.SetCurSel(m_ParDependentIndex);  
}
void MultiRunPar::OnCreateList()

{
	ListView_SetExtendedListViewStyleEx(m_ParList.GetSafeHwnd(), LVS_EX_FULLROWSELECT , LVS_EX_FULLROWSELECT);
		CRect rect;
	
			// Create the columns
		m_ParList.DeleteAllItems();
			m_ParList.GetClientRect(&rect);
		int nInterval = rect.Width()/6;
		m_ParList.InsertColumn(0, "Dimension", LVCFMT_LEFT, int(nInterval*0.4));
		m_ParList.InsertColumn(1, "Group", LVCFMT_LEFT, int(nInterval*0.8));
		m_ParList.InsertColumn(2, "Name", LVCFMT_LEFT, int(nInterval*0.6));
		m_ParList.InsertColumn(3, "Method", LVCFMT_LEFT, int(nInterval*0.9));
		m_ParList.InsertColumn(4, "Min", LVCFMT_LEFT, int(nInterval*0.4));
		m_ParList.InsertColumn(5, "Max", LVCFMT_LEFT, int(nInterval*0.4));
		m_ParList.InsertColumn(6, "Start", LVCFMT_LEFT, int(nInterval*0.4));
		m_ParList.InsertColumn(7, "Dependence", LVCFMT_LEFT, int(nInterval*0.6));
		m_ParList.InsertColumn(8, "n", LVCFMT_LEFT, int(nInterval*0.3));

	   	int nColumnCount=m_ParList.GetHeaderCtrl()->GetItemCount();
	  
		
	
	
// Delete all of the columns.
	for (int i=0;i < nColumnCount;i++)
	{
		m_ParList.DeleteColumn(0);
	}


//		 Insert new header
		m_ParList.GetClientRect(&rect);
		nInterval = rect.Width()/6;
		m_ParList.InsertColumn(0, "Dimension", LVCFMT_LEFT, int(nInterval*0.34));
		m_ParList.InsertColumn(1, "Group", LVCFMT_LEFT, int(nInterval*1.0));
		m_ParList.InsertColumn(2, "Name", LVCFMT_LEFT, int(nInterval*1.4));
		m_ParList.InsertColumn(3, "Method", LVCFMT_LEFT, int(nInterval*0.7));
		m_ParList.InsertColumn(4, "Min", LVCFMT_LEFT, int(nInterval*0.4));
		m_ParList.InsertColumn(5, "Max", LVCFMT_LEFT, int(nInterval*0.4));
		m_ParList.InsertColumn(6, "Start", LVCFMT_LEFT, int(nInterval*0.4));
		m_ParList.InsertColumn(7, "Dependence", LVCFMT_LEFT, int(nInterval*1.0));
		m_ParList.InsertColumn(8, "n", LVCFMT_LEFT, int(nInterval*0.3));


 	   CWnd *pWnd = m_ParList.GetDlgItem(0);
		
		ASSERT(pWnd);
		CHeaderCtrl *pHeader =m_ParList.GetHeaderCtrl();
		ASSERT(pHeader);
		

		 // Make the header control track 
		pHeader->ModifyStyle(0, HDS_HOTTRACK);
 
	    LV_ITEM lvi;
		TCHAR szItem[256];
		lvi.mask = LVIF_TEXT;
		int icount=0;
		for(size_t i=0;i<m_pDoc->m_Doc.m_MultiRun_Array.size();i++){
		   	for(int ii=0; ii<m_pDoc->m_Doc.MR_Get_NumberParDim(i+1);ii++){
				icount++;
				CString test;
					
		// First item
				lvi.iItem = icount-1;
				lvi.iSubItem = 0;
				_stprintf(szItem,_T(ItoAscii(i+1)) );				
				lvi.pszText = (LPTSTR)(LPCTSTR)szItem;
				m_ParList.InsertItem(&lvi);
				for(int ifn=1; ifn<9;ifn++) {
		// SubItem 1
					lvi.iSubItem = ifn;
					if(ifn==1) test=m_pDoc->m_Doc.MR_GetParGroup(i+1,ii+1).c_str();
					else if(ifn==2) {
						test=m_pDoc->m_Doc.MR_GetParName(i+1,ii+1).c_str();
						if(m_pDoc->m_Doc.MR_GetTabIndex(i+1,ii+1)>-1) {
							test+="("+ItoAscii(m_pDoc->m_Doc.MR_GetTabIndex(i+1,ii+1))+")";
						}
					}
					else if(ifn==3) {
						switch (m_pDoc->m_Doc.MR_GetMethod(i+1,ii+1)) {
						case 0:
							test="Linear";
							break;
						case 1: test="Logarithmic";
								break;
						case 2: test="Stochastic Linear";
							break;
						case 3: test="Stochastic Log";
							break;
						case 4: test="Table";
							break;
						case 5: test="Database Record";
							break;
						case 6: test="Same As";
							break;
						case 7: test="Relative Change As";
							break;
						case 8: test="Relative Opposite Change As";
							break;
						case 9: if(m_pDoc->m_Doc.MR_UseBayesianCoVar)
									test="Apply Bayesian results";
								else
									test="Bayesian Calibration";
							break;
						case 10: test="Connected";
							break;
						}

					}
					else if(ifn==4) {
						if(m_pDoc->m_Doc.MR_GetMethod(i+1,ii+1)<9&&m_pDoc->m_Doc.MR_GetMethod(i+1,ii+1)>3)
							test="-";
						else
							test=FtoAscii(m_pDoc->m_Doc.MR_GetMin(i+1,ii+1));
					}
					else if(ifn==5) {
						if(m_pDoc->m_Doc.MR_GetMethod(i+1,ii+1)<9&&m_pDoc->m_Doc.MR_GetMethod(i+1,ii+1)>3)
							test="-";
						else
							test=FtoAscii(m_pDoc->m_Doc.MR_GetMax(i+1,ii+1));
					}
					else if(ifn==6) {
						if(m_pDoc->m_Doc.MR_GetMethod(i+1,ii+1)!=9)
							test="-";
						else
							test=FtoAscii(m_pDoc->m_Doc.MR_GetStart(i+1,ii+1));
					}
					else if(ifn==7) {
						if(m_pDoc->m_Doc.MR_GetMethod(i+1,ii+1)>=6&&m_pDoc->m_Doc.MR_GetMethod(i+1,ii+1)<9) {
							SimB *pPar, *pParDep;
							size_t index, indexdep;
							index=m_pDoc->m_Doc.MR_GetTabIndex(i+1,ii+1);
							pPar=m_pDoc->m_Doc.MR_Get_pPar(i+1,ii+1);
							if(pPar!=NULL) {
								if(pPar->GetType()==PAR_SINGLE) {
									pParDep=((Ps*)pPar)->Get_MR_ParDependence();
									indexdep=((Ps*)pPar)->Get_MR_ParDependenceIndex();
								}
								else {
									pParDep=((P*)pPar)->Get_MR_ParDependence(index);
									indexdep=((P*)pPar)->Get_MR_ParDependenceIndex(index);
								}
								if(pParDep!=NULL) {
									test=pParDep->GetName().c_str();
									if(indexdep>-1)
										test+="("+ItoAscii(indexdep)+")";
								}
								else
									test="-";
							}
	
						}
						else
							test="-";

					}
					else if(ifn==8) {			
						test=ItoAscii(m_pDoc->m_Doc.m_MultiRun_Array[i].Dim);
					}

 					_stprintf(szItem,_T(test) );
					lvi.pszText = szItem;
					m_ParList.SetItem(&lvi);
					}
			}
		}



}
void MultiRunPar::OnCreateParTable()
{
	SimB *pSw;
	CString m_index, m_Col;
	CString namn;
	CString		grp,typ, status;
	TVINSERTSTRUCT tvInsert;
	tvInsert.hParent = NULL;
	tvInsert.hInsertAfter = TVI_SORT;
	tvInsert.item.mask = TVIF_TEXT;
	CString item1, item2, item3, lastItem;
	HTREEITEM hCountry, hPA;


	// To Add code for selection of a new parameter

	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE1);
	pCtrl->DeleteAllItems();
//	for (int il=0; il<2; il++) {
	if(m_IsDatabase) {
		CDB* pDB;

		item2=" ";
		/*		ar<<DB_GetChoice(Plant);
		ar<<DB_GetChoice(Soil_Properties);
		ar<<DB_GetChoice(Soil_Evaporation);
		ar<<DB_GetChoice(Snow);
		ar<<DB_GetChoice(Soil_Frost);
		ar<<DB_GetChoice(Soil_Water_Flows);
		ar<<DB_GetChoice(Plant_Growth);
		ar<<DB_GetChoice(Soil_Manage);
		ar<<DB_GetChoice(Soil_Organic);
		ar<<DB_GetChoice(Soil_InOrganic);*/
		for(int i=0;i<10;i++) {
			pDB=m_pDoc->m_Doc.DB_GetPointer(DB_types(i));
			item2=pDB->GetName().c_str();
			tvInsert.item.pszText = (LPTSTR)(LPCTSTR)item2;
			hCountry = pCtrl->InsertItem(&tvInsert);
			//hPA = pCtrl->InsertItem(TVIF_TEXT,item2, 0, 0, 0, 0, 0, hCountry, TVI_SORT);
		}
	}
	else {

			typ="Parameters";
			if(m_IsTable) typ="Tables" ;

			vector<SimB*> vvp;
			vvp=m_pDoc->m_Doc.GetPtrVector(string(typ), "");

			for(size_t i=0; i<vvp.size();i++) {
								pSw=vvp[i];
								if(pSw->IsEnabled()){
									namn=pSw->GetGroup().c_str();
									item2=pSw->GetName().c_str();
									tvInsert.item.pszText = (LPTSTR)(LPCTSTR)namn;
									if(namn==lastItem) {
										hPA = pCtrl->InsertItem(TVIF_TEXT,item2, 0, 0, 0, 0, 0, hCountry, TVI_SORT);
									}
									else {
										hCountry = pCtrl->InsertItem(&tvInsert);
										hPA = pCtrl->InsertItem(TVIF_TEXT,item2, 0, 0, 0, 0, 0, hCountry, TVI_SORT);
										lastItem=namn;
									}
									if(m_IsTable) {
 										int numrow=((Tab*)pSw)->GetValue();// Number of rows in table
			 						   	//for(int nRow=0; ((Tab*)pSw)->GetCell(0, nRow,&Value); nRow++){
										//   	}
										//hTab = pCtrl->InsertItem(TVIF_TEXT,item3, 0, 0, 0, 0, 0, hPA, TVI_SORT);
										for(size_t nCol=0; nCol<((Tab*)pSw)->GetNumOfCol();nCol++) {
											CString Item3=((Tab*)pSw)->GetTitle(nCol).c_str();
											if(numrow>1) Item3+="("+ItoAscii(numrow)+")";
											pCtrl->InsertItem(Item3, hPA, NULL);

										}	
									}



								}

			}
	}
}
void MultiRunPar::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	CString m_index, m_Col;
	CString m_Group;

	CTreeCtrl* pCtrl = (CTreeCtrl*) GetDlgItem(IDC_TREE1);
	*pResult = 0;
	HTREEITEM hItem = pCtrl->GetSelectedItem();
	CString m_Name=pCtrl->GetItemText(hItem);
	if(m_ShiftMode) return;
	// Database case
	if(m_IsDatabase) {
		m_Group="Database";
		m_ParName=m_Name;
		OnCDBNew();
	}

	HTREEITEM hItemgroup= pCtrl->GetParentItem(hItem);
	if(hItemgroup==NULL) return;


// Parameter case
	if(m_IsTable==FALSE){
		m_ParName=m_Name;
		m_Type="Parameters";
		m_ParGroup=pCtrl->GetItemText(hItemgroup);
		OnParNew();
	}

	HTREEITEM hItemTable= pCtrl->GetParentItem(hItemgroup);
	if(hItemTable==NULL) return;
//  Table case
	m_Type="Table Parameters";
	m_Group=pCtrl->GetItemText(hItemTable);
	m_Name=pCtrl->GetItemText(hItemgroup);
   	m_ParName=pCtrl->GetItemText(hItem);
	m_ParName=m_ParName.Mid(0,m_ParName.Find("("));
	m_ParGroup=m_Group;
	m_TableName=pCtrl->GetItemText(hItemgroup);
	if(m_IsTable)
		OnParTableNew();
	


		// TODO: Add your control notification handler code here
	//*pResult = 0;
//	m_StatusLine="Items from simulation :"+ RunNo+" are ready  - Select variables from the list";
	//	CDialog::OnInitDialog();
		//*pResult = 0;

}
void MultiRunPar::OnCbnSelchangeCombo2()
{
	// TODO: Add your control notification handler code here
/*	m_TableIndexValue2=m_TableIndex.GetCurSel();
	Text="MultipleRun Changes for Parameter : "+m_pPs->GetName()+" ("+m_pPs->GetUnit()+")";
	if(m_TabIndex>-1) Text+=", index="+ItoAscii(m_TabIndex);
	SetWindowText(Text);
	int method;
	if(m_TabIndex<=-1) {
		m_Selected_Dim=m_pPs->MR_Get_Dim();  // ERROR
		method=m_pPs->MR_Get_Method();
	}
	else { 
		m_Selected_Dim=m_pPs->MR_Get_Dim(m_TabIndex); // ERROR
		method=m_pPs->MR_Get_Method(m_TabIndex);
	}
	if(method<3){
		if(m_TabIndex<=-1) {
			m_MinValue=m_pPs->MR_Get_Min();
			m_MaxValue=m_pPs->MR_Get_Max();
		}
		else {
			m_MinValue=m_pPs->MR_Get_Min(m_TabIndex);
			m_MaxValue=m_pPs->MR_Get_Max(m_TabIndex);
		}
		m_ctrlMin.SetWindowText(FtoAscii(m_MinValue));
		m_ctrlMax.SetWindowText(FtoAscii(m_MaxValue));
	}
*/

}

void MultiRunPar::OnLvnDeleteitemList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void MultiRunPar::OnLvnBeginlabeleditList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void MultiRunPar::OnBnClickedButtonChangeRepetions()
{
//REMAIN
/*	if(m_pPs->IsKindOf(RUNTIME_CLASS(CDB))) {
			CDB* pDB;
			pDB=(CDB*)m_pPs;
			pDB->MR_Set_Dim(m_Selected_Dim);
	}
	else
		m_pPs->MR_Set_Dim(m_Selected_Dim);*/
	DimNewValue dlg(m_pDoc->m_Doc.MR_GetDimValue(m_Selected_Dim-1));
	if(dlg.DoModal()==IDOK) {
		m_pDoc->m_Doc.MR_SetDimValue(m_Selected_Dim-1, dlg.m_NewValue);
	}
	OnCreateDimList();
	OnInitDialog();
	// TODO: Add your control notification handler code here
}

void MultiRunPar::OnBnClickedButtonCounter()
{
	Ps* pPar;
	int ndim,npar;
	ndim=m_pDoc->m_Doc.MR_GetNumberOfDimensions();
	for(int i=0;i<ndim;i++) {
		npar=m_pDoc->m_Doc.MR_Get_NumberParDim(i+1);
		for(int l1=npar;l1>0;l1--) {
			int nTabIndex=m_pDoc->m_Doc.MR_GetTabIndex(i+1,l1);
			pPar=(Ps*)m_pDoc->m_Doc.MR_Get_pPar(i+1,l1);
			if(nTabIndex>0)
				m_pDoc->m_Doc.MR_CP_Reset(pPar, nTabIndex);
			else
				m_pDoc->m_Doc.MR_CP_Reset(pPar);
		}
		m_pDoc->m_Doc.MR_DeleteDimValue(i);
	}
	OnCreateDimList();
	OnInitDialog();
}

void MultiRunPar::OnBnClickedButtonNewdim()
{
	m_pDoc->m_Doc.MR_AddDimValue();
	OnCreateDimList();
	// TODO: Add your control notification handler code here
}

void MultiRunPar::OnBnClickedButtonDeleteDim()

{
	SimB* pPar;
	SIMB pPAR;
	UpdateData(TRUE);
	int delcount=0, maxdim=0;
	maxdim=m_pDoc->m_Doc.m_MultiRun_Array.size();
	for(int i=0;i<maxdim;i++){
		if(m_pDoc->m_Doc.MR_Get_NumberParDim(i+1)<=0){
			m_pDoc->m_Doc.MR_DeleteDimValue(i);
			delcount++;
		}
		else if( delcount>0) {
			for(int l1=0;l1<m_pDoc->m_Doc.MR_Get_NumberParDim(i+1);l1++) {
				pPAR=m_pDoc->m_Doc.MR_Get_PAR(i+1,l1+1);
				//int ntabindex=m_pDoc->m_Doc.MR_GetTabIndex(i+1,l1+1);
				//pPar=(Ps*)m_pDoc->m_Doc.MR_Get_pPar(i+1,l1+1);
				if(pPAR.pBase->Is_P()) {
					int dim=((Ps*)pPAR.pBase)->MR_Get_Dim();
					((Ps*)pPAR.pBase)->MR_Set_Dim(dim-delcount);
				}
				else {
					int dim=((P*)pPAR.pBase)->MR_Get_Dim(pPAR.TabIndex);
					((P*)pPAR.pBase)->MR_Set_Dim(pPAR.TabIndex, dim-delcount);
				}

			}
			

		}
	}
	OnCreateDimList();
	// TODO: Add your control notification handler code here
}

void MultiRunPar::OnBnClickedButtoApply()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_IsTable) {
		m_TabIndex=m_TableIndexValue2+1;
		if(m_TabIndex<=0) m_TabIndex=1;
	}
		else
			m_TabIndex=-1;
	if(m_IsDatabase) {
		m_ParMethod=MR_METHOD(5);
	}
	// Check for invalid conditions.
	else if(m_MethodList.GetCurSel()<4) {
		/*CString str;
		m_ctrlMin.GetWindowText(str);
		m_MinValue=AtoFloat(str);
		m_ctrlMax.GetWindowText(str);
		m_MaxValue=AtoFloat(str);*/
		m_MinValue=AtoFloat(m_ParMinValue);
		m_MaxValue=AtoFloat(m_ParMaxValue);
		if((m_ParMethod==1||m_ParMethod==3||m_ParMethod==10||m_ParMethod==7||m_ParMethod==8)&&m_MinValue<=0) {
           MessageBox("Zero or negative values not allowed for logaritic distribution \n Changed min 1.E-5","Critical information",MB_OK|MB_ICONINFORMATION);
    	   m_MinValue=1.e-5f;
		   	// Set dimension, method, and min.
			if(m_TabIndex<=-1) {
				((Ps*)m_pPs)->MR_Set_Dim(m_Selected_Dim);
				((Ps*)m_pPs)->MR_Set_Method(m_ParMethod);
				if(m_ParMethod<4)	((Ps*)m_pPs)->MR_Set_Min(m_MinValue);
			}
			else {
				((P*)m_pPs)->MR_Set_Dim(m_TabIndex, m_Selected_Dim);
				((P*)m_pPs)->MR_Set_Method(m_TabIndex, m_ParMethod);
				if(m_ParMethod)	((P*)m_pPs)->MR_Set_Min(m_TabIndex, m_MinValue);
			}
		    OnInitDialog();
    	}
       	if((m_ParMethod==1||m_ParMethod==3||m_ParMethod==10||m_ParMethod==7||m_ParMethod==8)&&m_MaxValue<=0) {
    		MessageBox("Zero or negative values not allowed for logaritic distribution \n Changed max-value to 1.0","Critical information",MB_OK|MB_ICONINFORMATION);
    		m_MaxValue=1.;
			// Set dimension, method, and max.
			if(m_TabIndex<=-1) {
				((Ps*)m_pPs)->MR_Set_Dim(m_Selected_Dim);
				((Ps*)m_pPs)->MR_Set_Method(m_ParMethod);
				if(m_ParMethod<4)	((Ps*)m_pPs)->MR_Set_Max(m_MaxValue);
			}
			else {
				((P*)m_pPs)->MR_Set_Dim(m_TabIndex, m_Selected_Dim);
				((P*)m_pPs)->MR_Set_Method(m_TabIndex, m_ParMethod);
				if(m_ParMethod<4)	((P*)m_pPs)->MR_Set_Max(m_TabIndex, m_MaxValue);
			}
		   	OnCreateList();
    	}
	}
	else if(m_MethodList.GetCurSel()>4) {
		if(m_MethodList.GetCurSel()>6) {
			m_MinValue=AtoFloat(m_ParMinValue);
			m_MaxValue=AtoFloat(m_ParMaxValue);
		}
			if(m_pDepPar==NULL) OnCbnSelchangeParDependence();
			if(m_TabIndex<=-1) {
				((Ps*)m_pPs)->MR_Set_Dim(m_Selected_Dim);
				((Ps*)m_pPs)->MR_Set_Method(m_ParMethod);
					if(m_DepIndex>=0){
						((Ps*)m_pPs)->Set_MR_ParDependence(m_pDepPar);
						((Ps*)m_pPs)->Set_MR_ParDependenceIndex(m_DepIndex);
					}
					else
						((Ps*)m_pPs)->Set_MR_ParDependence(m_pDepPar);

			}
			else {
				((P*)m_pPs)->MR_Set_Dim(m_TabIndex, m_Selected_Dim);
				((P*)m_pPs)->MR_Set_Method(m_TabIndex, m_ParMethod);
				if(m_DepIndex>=0) {
					((P*)m_pPs)->Set_MR_ParDependence(m_TabIndex,m_pDepPar);
					((P*)m_pPs)->Set_MR_ParDependenceIndex(m_TabIndex, m_DepIndex);
				}
				else
					((P*)m_pPs)->Set_MR_ParDependence(m_TabIndex, m_pDepPar);
			}
		    OnInitDialog();
	}


	// Set dimension, method, min, and max.
	if(m_TabIndex<=-1) {
		//if(m_pPs->IsKindOf(RUNTIME_CLASS(CDB))) {
			CDB* pDB;
			pDB=(CDB*)m_pPs;
			pDB->MR_Set_Dim(m_Selected_Dim);
			pDB->MR_Set_Method(m_ParMethod);
		//}
/*		else {
//REMAIN			m_pPs->MR_Set_Dim(m_Selected_Dim);
			m_pPs->MR_Set_Method(m_ParMethod);
			if(m_ParMethod<4||m_ParMethod>6) {
				m_pPs->MR_Set_Min(m_MinValue);
				m_pPs->MR_Set_Max(m_MaxValue);
				m_pPs->MR_Set_Start(m_StartValue);
			}
		}*/
	}
	else {
		((P*)m_pPs)->MR_Set_Dim(m_TabIndex, m_Selected_Dim);
		((P*)m_pPs)->MR_Set_Method(m_TabIndex, m_ParMethod);
		if(m_ParMethod<4||m_ParMethod>6) {
			((P*)m_pPs)->MR_Set_Min(m_TabIndex, m_MinValue);
			((P*)m_pPs)->MR_Set_Max(m_TabIndex, m_MaxValue);
			((P*)m_pPs)->MR_Set_Start(m_TabIndex, m_StartValue);
		}

	}

	// Further processing on dimensions.
	
	if(m_Selected_Dim>0) {
		if(!m_IsDatabase)
			m_pDoc->m_Doc.MR_CP_Set(m_pPs, m_TabIndex);
		else
			if(m_pDoc->m_Doc.MR_CP_Set(m_pPs, m_TabIndex))
			{
			int kk=0;	
				
			}
			else {
				int ll=0;
				
			};
	}
	else {
		CString temp = "No multiple runs will be applied for the selected parameter:\n";
		temp+=m_ParName;
		if(m_IsTable) {
			temp+="(";
			temp+=ItoAscii(m_TabIndex);
			temp+=")";
		}
	    MessageBox(temp);
		if(m_TabIndex>0)
			m_pDoc->m_Doc.MR_CP_Reset(m_pPs, m_TabIndex);
		else
			m_pDoc->m_Doc.MR_CP_Reset(m_pPs);
		//return;
	}
	for(int i=m_pDoc->m_Doc.MR_GetNumberOfDimensions();i>0;i--) {
		if(m_pDoc->m_Doc.MR_Get_NumberParDim(i)<1)
			m_pDoc->m_Doc.MR_DeleteDimValue(i-1);
		else break;
	}

	m_pDoc->m_Doc.m_BayesianStepSize=m_StepSize;
	if(!m_pDoc->m_Doc.MR_UseBayesianCoVar)
		m_pDoc->m_Doc.MR_Init_BayesCovar(m_pDoc->m_Doc.MR_Get_NumBayesPar());

	OnCreateDimList();
	
	OnInitDialog();
}

void MultiRunPar::OnEnChangeEdit3()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void MultiRunPar::OnLvnColumnclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void MultiRunPar::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	CString Text, Group, Name;
	int i, icol;
	i=1;icol=3;
	//m_ParList.GetItem(pItem);
	//i=(int)pItem->iItem;
	int imark=m_ParList.GetSelectionMark();
	m_ParGroup=m_ParList.GetItemText(imark, 1);
	Name=m_ParList.GetItemText(imark, 2);
	int ipar_st, ipar_end;
	ipar_st=Name.Find("(");
	ipar_end=Name.Find(")");

	if(ipar_st>0) {
		CString Index;
		Index=Name.Mid(ipar_st+1,ipar_end-ipar_st-1);
		m_TableIndexValue2=AtoInt(Index)-1;
		m_ParName=Name.Mid(0,ipar_st);
		OnParTableNew();
		return;

	}
	else if (m_ParGroup=="DataBase") {
		
		m_ParName=Name;
		OnCDBNew();
		return;		
	}
	else {
		m_ParName=Name;

		OnParNew();
		return;
	}

}
void MultiRunPar::OnBnClickedTableValues()
{
	MultiRunTable dlg(m_pDoc, m_pPs, m_TabIndex);
	if(dlg.DoModal()==IDOK)
		m_IsTableSet=TRUE;
	else
		m_IsTableSet=FALSE;
	OnInitDialog();
}	// TODO: Add your control notification handler code here

void MultiRunPar::OnCbnSelchangeMethod()
{
	OnUpdateButtons();
	OnBnClickedButtoApply();
	// TODO: Add your control notification handler code here
}

void MultiRunPar::OnBnClickedButtonTable()
{
	m_IsTable=TRUE;
	m_IsDatabase=FALSE;
	m_ShiftMode=TRUE;
	OnInitDialog();
	// TODO: Add your control notification handler code here
}
void MultiRunPar::OnBnClickedButtonNotable()
{
	m_IsTable=FALSE;
	m_IsDatabase=FALSE;
	m_ShiftMode=TRUE;
	OnInitDialog();
	// TODO: Add your control notification handler code here
}
void MultiRunPar::OnBnClickedStartValues()
{
	SimB *pPar;
	int nTabIndex;
	float v;
	int icount;
		icount=-1;
		
		for(size_t i=0;i<m_pDoc->m_Doc.m_MultiRun_Array.size();i++){
		   	for(int ii=0; ii<m_pDoc->m_Doc.MR_Get_NumberParDim(i+1);ii++){
				if(m_pDoc->m_Doc.MR_GetMethod(i+1,ii+1)==9) {
					icount++;
					pPar=(Ps*)m_pDoc->m_Doc.MR_Get_pPar(i+1,ii+1);
					nTabIndex=m_pDoc->m_Doc.MR_GetTabIndex(i+1,ii+1);
					if(m_pDoc->m_Doc.MR_UseBayesianCoVar) {
							v=m_pDoc->m_Doc.MR_Get_BayesMean(icount); 
					}
					if(nTabIndex<0) {	
						if(!m_pDoc->m_Doc.MR_UseBayesianCoVar) {
							v=pPar->GetValue();
						}
						((Ps*)pPar)->MR_Set_Start(v);
						if(v<m_pDoc->m_Doc.MR_GetMin(i+1,ii+1)) ((Ps*)pPar)->MR_Set_Min(v-0.1f*v);
						if(v>m_pDoc->m_Doc.MR_GetMax(i+1,ii+1)) ((Ps*)pPar)->MR_Set_Max(v+0.1f*v);
					}
					else {
						if(!m_pDoc->m_Doc.MR_UseBayesianCoVar) {
							v=((P*)pPar)->GetValue(nTabIndex-1);
						}
						((P*)pPar)->MR_Set_Start(nTabIndex, v);
						if(v<m_pDoc->m_Doc.MR_GetMin(i+1,ii+1)) ((P*)pPar)->MR_Set_Min(nTabIndex, v-0.1f*v);
						if(v>m_pDoc->m_Doc.MR_GetMax(i+1,ii+1)) ((P*)pPar)->MR_Set_Max(nTabIndex, v+0.1f*v);


					}
				}
					
			}
		}
	OnCreateList();
}

void MultiRunPar::OnCbnSelchangeParDependence()
{
	// TODO: Add your control notification handler code here
	m_ParDependentIndex=m_ParDependence.GetCurSel();
	int icount=0;
	for(size_t i=0;i<m_pDoc->m_Doc.m_MultiRun_Array.size();i++){
	   	for(int ii=0; ii<m_pDoc->m_Doc.MR_Get_NumberParDim(i+1);ii++){
			if(icount==m_ParDependentIndex) {
				if(m_pDoc->m_Doc.MR_Get_pPar(i+1,ii+1)!=NULL){
					m_pDepPar=m_pDoc->m_Doc.MR_Get_pPar(i+1,ii+1);
					m_DepIndex=m_pDoc->m_Doc.MR_GetTabIndex(i+1,ii+1);
				}
				else
					m_pDepPar=NULL;
				m_DepIndex=m_pDoc->m_Doc.MR_GetTabIndex(i+1,ii+1); 
				break;
			}
			icount++;
		}
	}
}
void MultiRunPar::OnOK()
{
	UpdateData(TRUE);
	if(m_pPs!=NULL) 
		OnBnClickedButtoApply();
	m_pDoc->m_Doc.m_BayesianStepSize=m_StepSize;
	CDialog::OnOK();
}

void MultiRunPar::OnLbnSelchangeDimensions()
{
	OnBnClickedButtoApply();
	// TODO: Add your control notification handler code here
}

void MultiRunPar::OnBnClickedApplybayesianparameters()
{
	m_pDoc->m_Doc.MR_UseBayesianCoVar=TRUE;
	OnBnClickedStartValues();
	// TODO: Add your control notification handler code here
}

void MultiRunPar::OnExportToExcel()
{
	BasicExcel e;
	CString test;
	int icount;

	icount=0;
int fileDlgResult;
	CString pszFileName ;
	LPCTSTR lpszDefExt="xls";
	LPCTSTR lpszFilter;
	//The next line is only neccessary as long as the PGP does not handle long filenames!
    //PEJ Excluded info box
    //	MessageBox("Since PGP can not handle long filenames, no subpart of the path must\nbe longer than 8 characters plus a 3 letter extension!","Critical information",MB_OK|MB_ICONINFORMATION);
	//Create a read dialog
		static char BASED_CODE szFilter[] = "Excel file (*.xls)|*.xls||";
		CFileDialog	ParFileDlg(FALSE,lpszDefExt="xls",lpszFilter = "*.xls",OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,szFilter);
	//Show the dialog
		fileDlgResult=ParFileDlg.DoModal();
	//If the cancel button has been pushed then return
		if (fileDlgResult==IDCANCEL)
			return;
	//Retrieve the filename of the file that is to be opened
		pszFileName = ParFileDlg.GetPathName();









// Create a new workbook with 2 worksheets and write some contents.
	e.New(1);
	e.RenameWorksheet("Sheet1", "MultiRun");
	BasicExcelWorksheet* sheet = e.GetWorksheet("MultiRun");

	if (sheet)
	{
		
		sheet->Cell(0,0)->SetString("Dimension");	
		sheet->Cell(0,1)->SetString("Group");	
		sheet->Cell(0,2)->SetString("Name");	
		sheet->Cell(0,3)->SetString("Method");	
		sheet->Cell(0,4)->SetString("Min");	
		sheet->Cell(0,5)->SetString("Max");	
		sheet->Cell(0,6)->SetString("Start");	
		sheet->Cell(0,7)->SetString("Dependence");	
		sheet->Cell(0,8)->SetString("n");	
		for(size_t i=0;i<m_pDoc->m_Doc.m_MultiRun_Array.size();i++){
		   	for(int ii=0; ii<m_pDoc->m_Doc.MR_Get_NumberParDim(i+1);ii++){
				icount++;
				sheet->Cell(icount,0)->Set((int)i+1);
				sheet->Cell(icount,1)->SetString(m_pDoc->m_Doc.MR_GetParGroup(i+1,ii+1).c_str());
				test=m_pDoc->m_Doc.MR_GetParName(i+1,ii+1).c_str();
				if(m_pDoc->m_Doc.MR_GetTabIndex(i+1,ii+1)>-1) {
							test+="("+ItoAscii(m_pDoc->m_Doc.MR_GetTabIndex(i+1,ii+1))+")";
						}
				sheet->Cell(icount,2)->SetString(test);

				CString xx;
				
				BasicExcelWorksheet* sheet_tab; 
				int nRow=m_pDoc->m_Doc.m_MultiRun_Array[i].nCount;
				SIMB pPAR=m_pDoc->m_Doc.MR_Get_PAR(i+1,ii+1);
				CDB* pCPB;
				int it;
				float v;


				switch (m_pDoc->m_Doc.MR_GetMethod(i+1,ii+1)) {
						case 0:
							test="Linear";
							break;
						case 1: test="Logarithmic";
								break;
						case 2: test="Stochastic Linear";
							break;
						case 3: test="Stochastic Log";
							break;
						case 4: test="Table";
								xx="Table_";
								xx+=m_pDoc->m_Doc.MR_GetParName(i+1,ii+1).c_str();
								sheet_tab = e.AddWorksheet(xx);
								sheet_tab = e.GetWorksheet(xx);
								sheet_tab->Cell(0,0)->SetString("Index");
								sheet_tab->Cell(0,1)->SetString("Value");
								if(sheet_tab) {
									for (it=0;it<nRow;it++) {
										if(pPAR.pBase->Is_P()) 
											v=((Ps*)pPAR.pBase)->MR_Get_TabValue(it);
										else 
											v=((P*)pPAR.pBase)->MR_Get_TabValue(pPAR.TabIndex,it);
										sheet_tab->Cell(it+1,0)->SetInteger(it+1);
										sheet_tab->Cell(it+1,1)->SetDouble(v);
									}
								}
								sheet = e.GetWorksheet("MultiRun");
							break;
						case 5: test="Database Record";
								xx="DataBase_";
								xx+=m_pDoc->m_Doc.MR_GetParName(i+1,ii+1).c_str();
								sheet_tab = e.AddWorksheet(xx);
								sheet_tab = e.GetWorksheet(xx);
								if(sheet_tab) {
									sheet_tab->Cell(0,0)->SetString("Index");
									sheet_tab->Cell(0,1)->SetString("Value");
									pCPB=(CDB*)pPAR.pBase;
									for(it=0;it<nRow;it++){
										sheet_tab->Cell(it+1,0)->SetString(pCPB->MR_GetSelection(it).c_str());
										sheet_tab->Cell(it+1,1)->SetString(pCPB->MR_GetKeySelection(it).c_str());
									}
								}
								sheet = e.GetWorksheet("MultiRun");
							break;
						case 6: test="Same As";
							break;
						case 7: test="Relative Change As";
							break;
						case 8: test="Relative Opposite Change As";
							break;
						case 9: if(m_pDoc->m_Doc.MR_UseBayesianCoVar)
									test="Apply Bayesian results";
								else
									test="Bayesian Calibration";
							break;
						}


					sheet->Cell(icount,3)->SetString(test);
				if(m_pDoc->m_Doc.MR_GetMethod(i+1,ii+1)==6) {
					sheet->Cell(icount,4)->SetString("-");
					sheet->Cell(icount,5)->SetString("-");}
				else {
						sheet->Cell(icount,4)->Set((float)m_pDoc->m_Doc.MR_GetMin(i+1,ii+1));					
						sheet->Cell(icount,5)->Set((float)m_pDoc->m_Doc.MR_GetMax(i+1,ii+1));
				}
				if(m_pDoc->m_Doc.MR_GetMethod(i+1,ii+1)!=9) 
					sheet->Cell(icount,6)->SetString("-");
				else
					sheet->Cell(icount,6)->Set((float)m_pDoc->m_Doc.MR_GetStart(i+1,ii+1));

				if(m_pDoc->m_Doc.MR_GetMethod(i+1,ii+1)>=6&&m_pDoc->m_Doc.MR_GetMethod(i+1,ii+1)<9) {
							Ps *pPar, *pParDep;
							int index, indexdep;
							SIMB pPP, pPP_Dep;
							//index=m_pDoc->m_Doc.MR_GetTabIndex(i+1,ii+1);
							pPP=m_pDoc->m_Doc.MR_Get_PAR(i+1,ii+1);
							if(pPP.pBase!=NULL) {
								if(pPP.pBase->Is_P()) {
									pPP_Dep=((Ps*)pPP.pBase)->Get_MR_PAR_Dependence();

								}
								else {
									pPP_Dep=((P*)pPP.pBase)->Get_MR_PAR_Dependence(pPP.TabIndex);

								}
								if(pPP_Dep.pBase!=NULL) {
									test=pPP_Dep.pBase->GetName().c_str();
									if(pPP_Dep.pBase->Is_PTable())
										test+="("+ItoAscii(pPP_Dep.TabIndex)+")";
									}
									else
										test="-";
								}
	
						}
						else
							test="-";		

				sheet->Cell(icount,7)->SetString(test);
				sheet->Cell(icount,8)->Set((int)m_pDoc->m_Doc.m_MultiRun_Array[i].Dim);
				}

			}
	}


	sheet = e.AddWorksheet("CoVariance", 1);
	sheet = e.GetWorksheet(1); 
	if (sheet)
	{
		int icount=0;
		for(size_t i=0;i<m_pDoc->m_Doc.m_MultiRun_Array.size();i++){
		   	for(int ii=0; ii<m_pDoc->m_Doc.MR_Get_NumberParDim(i+1);ii++){
				icount++;
				CString test;
			}	
		}
		int ncount=m_pDoc->m_Doc.MR_Get_NumBayesPar();
		
		for(int i=0;i<ncount;i++) {
			sheet->Cell(1,i+2)->SetString(m_pDoc->m_Doc.MR_Get_BayesPar(i).c_str());
			sheet->Cell(i+2,1)->SetString(m_pDoc->m_Doc.MR_Get_BayesPar(i).c_str());
			sheet->Cell(0,i+2)->SetString(m_pDoc->m_Doc.MR_Get_BayesGroup(i).c_str());
			sheet->Cell(i+2,0)->SetString(m_pDoc->m_Doc.MR_Get_BayesGroup(i).c_str());
				for(int ii=0;ii<ncount;ii++) {
					sheet->Cell(i+2,ii+2)->Set(m_pDoc->m_Doc.MR_Get_BayesCoVar(i,ii));
				}
			sheet->Cell(ncount+3,0)->SetString("Mean Value");
			sheet->Cell(ncount+4,0)->SetString("CV Value");
			sheet->Cell(ncount+3,i+2)->Set(m_pDoc->m_Doc.MR_Get_BayesMean(i));
			sheet->Cell(ncount+4,i+2)->Set(m_pDoc->m_Doc.MR_Get_BayesCoCor(i,i)/m_pDoc->m_Doc.MR_Get_BayesMean(i));
			sheet->Cell(0,0)->SetString("Group");
			sheet->Cell(1,1)->SetString("Name");
			sheet->Cell(0,ncount+3)->SetString("Mean Value");
			sheet->Cell(0,ncount+4)->SetString("CV Value");
			sheet->Cell(i+2,ncount+3)->Set(m_pDoc->m_Doc.MR_Get_BayesMean(i));
			sheet->Cell(i+2,ncount+4)->Set(m_pDoc->m_Doc.MR_Get_BayesCoCor(i,i)/m_pDoc->m_Doc.MR_Get_BayesMean(i));

		}
	}
	sheet = e.AddWorksheet("CoCorrelation", 1);
	sheet = e.GetWorksheet(1); 
	if (sheet)
	{
		int icount=0;
		for(size_t i=0;i<m_pDoc->m_Doc.m_MultiRun_Array.size();i++){
		   	for(int ii=0; ii<m_pDoc->m_Doc.MR_Get_NumberParDim(i+1);ii++){
				icount++;
				CString test;
			}	
		}
		int ncount=m_pDoc->m_Doc.MR_Get_NumBayesPar();
		
		for(int i=0;i<ncount;i++) {
			sheet->Cell(1,i+2)->SetString(m_pDoc->m_Doc.MR_Get_BayesPar(i).c_str());
			sheet->Cell(i+2,1)->SetString(m_pDoc->m_Doc.MR_Get_BayesPar(i).c_str());
			sheet->Cell(0,i+2)->SetString(m_pDoc->m_Doc.MR_Get_BayesGroup(i).c_str());
			sheet->Cell(i+2,0)->SetString(m_pDoc->m_Doc.MR_Get_BayesGroup(i).c_str());


				for(int ii=0;ii<ncount;ii++) {
					sheet->Cell(i+2,ii+2)->Set(m_pDoc->m_Doc.MR_Get_BayesCoCor(i,ii));
				}
		}
// Write Mean Values and CV
		sheet->Cell(0,0)->SetString("Group");
		sheet->Cell(1,1)->SetString("Name");
		sheet->Cell(ncount+3,0)->SetString("Mean Value");
		sheet->Cell(ncount+4,0)->SetString("CV Value");
		sheet->Cell(0,ncount+3)->SetString("Mean Value");
		sheet->Cell(0,ncount+4)->SetString("CV Value");

		for(int i=0;i<ncount;i++) {
			sheet->Cell(ncount+3,i+2)->Set(m_pDoc->m_Doc.MR_Get_BayesMean(i));
			sheet->Cell(ncount+4,i+2)->Set(m_pDoc->m_Doc.MR_Get_BayesCoCor(i,i)/m_pDoc->m_Doc.MR_Get_BayesMean(i));
			sheet->Cell(i+2,ncount+3)->Set(m_pDoc->m_Doc.MR_Get_BayesMean(i));
			sheet->Cell(i+2,ncount+4)->Set(m_pDoc->m_Doc.MR_Get_BayesCoCor(i,i)/m_pDoc->m_Doc.MR_Get_BayesMean(i));

		}



	}
	sheet = e.AddWorksheet("ParMeanValues", 1);
	sheet = e.GetWorksheet(1); 
	if (sheet)
	{
		int ncount=m_pDoc->m_Doc.MR_Get_NumBayesPar();

		sheet->Cell(0,0)->SetString("Group");
		sheet->Cell(0,1)->SetString("Name");
		sheet->Cell(0,2)->SetString("Value");

		for(int i=0;i<ncount;i++) {
			sheet->Cell(i+1,1)->SetString(m_pDoc->m_Doc.MR_Get_BayesPar(i).c_str());
			sheet->Cell(i+1,0)->SetString(m_pDoc->m_Doc.MR_Get_BayesGroup(i).c_str());
			sheet->Cell(i+1,2)->Set(m_pDoc->m_Doc.MR_Get_BayesMean(i));
		}
	}

	e.SaveAs(pszFileName);

}

void MultiRunPar::OnImportFromExcel()
{

	BasicExcel e;
	int fileDlgResult;
	CString pszFileName ;
	LPCTSTR lpszDefExt="xls";
	LPCTSTR lpszFilter;
	//The next line is only neccessary as long as the PGP does not handle long filenames!
    //PEJ Excluded info box
    //	MessageBox("Since PGP can not handle long filenames, no subpart of the path must\nbe longer than 8 characters plus a 3 letter extension!","Critical information",MB_OK|MB_ICONINFORMATION);
	//Create a read dialog
		static char BASED_CODE szFilter[] = "Excel file (*.xls)|*.xls||";
		CFileDialog	ParFileDlg(TRUE,lpszDefExt="xls",lpszFilter = "*.xls",OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,szFilter);
	//Show the dialog
		fileDlgResult=ParFileDlg.DoModal();
	//If the cancel button has been pushed then return
		if (fileDlgResult==IDCANCEL)
			return;
	//Retrieve the filename of the file that is to be opened
		pszFileName = ParFileDlg.GetPathName();

	// Load a workbook with one sheet, display its contents and save into another file.
	//cout << "Start" << endl;
e.Load(pszFileName);	
	//cout << "Loaded" << endl;

	//m_MultiRun_Array.RemoveAll();
	//m_MR_Index_Count.RemoveAll();
	//m_P_MR_Array.RemoveAll();
	//m_MR_TabIndex.RemoveAll();
	m_pDoc->m_Doc.MR_ResetAll();

BasicExcelWorksheet* sheet1 = e.GetWorksheet("MultiRun");
if (sheet1)
	{

		size_t maxRows = sheet1->GetTotalRows();
		size_t maxCols = sheet1->GetTotalCols();

		BasicExcelCell* cell = sheet1->Cell(0,0);
		CString test;

		BAYESIANCALIB_mp_NP=maxCols;
		
		BAYESIANCALIB_mp_CALIB_RESET();
		BAYESIANCALIB_mp_CALIB_DEF();


	//	Checke Headings
		for(size_t cc=0;cc<maxCols;++cc) {
			cell=sheet1->Cell(0,cc);
			test=cell->GetString();
		}

		// Variables to be defined for each line
		int Ndimension,Nindex,NDepIndex,NRepetition;
		CString NGroup, NParName, NMethod, Ndep;
		float NMin, NMax, NStart;
		for (size_t r=1; r<maxRows; ++r)
		{
			for (size_t c=0; c<maxCols; ++c) {
				cell=sheet1->Cell(r,c);
				switch (cell->Type())
				{
					case BasicExcelCell::UNDEFINED:
						break;
					case BasicExcelCell::INT:
						if(c==0) 
							Ndimension=cell->GetInteger();
						else if(c==8)
							NRepetition=cell->GetInteger();
						break;
					case BasicExcelCell::DOUBLE:
						if(c==4)
							NMin=float(cell->GetDouble());
						else if(c==5)
							NMax=float(cell->GetDouble());
						else if(c==6)
							NStart=float(cell->GetDouble());
						break;
					case BasicExcelCell::STRING:
						if(c==1)
							NGroup=cell->GetString();
						else if(c==2)
							NParName=cell->GetString();
						else if(c==3)
							NMethod=cell->GetString();
						else if(c==7)
							Ndep=cell->GetString();
						break;
					case BasicExcelCell::WSTRING:
						wprintf(L"%10s", cell->GetWString());
						break;
				}
			}
		// Apply the line as new input to model
		int pos1, pos2;
		P* pParT;
		Ps* pPar;	
		SimB* pParB,*pdParB;
		m_Type="Parameters";
		int imethod, it, iv;
		CString xx,str1, str2;
		BasicExcelWorksheet* sheet_tab;
		float v;

		if(NMethod=="linear") 
			imethod=0;
		else if(NMethod=="Logarithmic")
			imethod=1;
		else if(NMethod=="Stochastic Linear")
			imethod=2;
		else if(NMethod=="Stochastic Log")
			imethod=3;
		else if(NMethod=="Table")
			imethod=4;
		else if(NMethod=="Database Record")
            imethod=5;
		else if(NMethod=="Same As")
			imethod=6;
		else if(NMethod=="Relative Change As")
			imethod=7;
		else if(NMethod=="Relative Opposite Change As")
			imethod=8;
		else if(NMethod=="Bayesian Calibration") {
			imethod=9;
			m_pDoc->m_Doc.MR_UseBayesianCoVar=FALSE;
		}
		else if(NMethod=="Apply Bayesian results") {
			imethod=9;
			m_pDoc->m_Doc.MR_UseBayesianCoVar=TRUE;
		}
		else if(NMethod=="Connected")
			imethod=10;
		else
			imethod=-1;

	// Dependence parameter decoding
		pos1=Ndep.Find("(");
		pos2=Ndep.Find(")");
		if(pos2>pos1) {
			CString t=Ndep.Mid(pos1+1,pos2-pos1-1);
			NDepIndex=AtoInt(t);
			Ndep=Ndep.Mid(0,pos1);
			m_Type="Table Parameters";
			pdParB=m_pDoc->GetPtr(Ndep, m_Type, NGroup);
		}
		else {
			NDepIndex=-1;
			m_Type="Parameters";
			pdParB=m_pDoc->GetPtr(Ndep, m_Type, NGroup);
		}
		pos1 =NParName.Find("(");
		pos2 =NParName.Find(")");
		if(imethod==5) {
				CDB *pDB;
				pDB=m_pDoc->m_Doc.DB_GetPointer(string(NParName));
				pDB->MR_ResetSelection();
				m_pDoc->m_Doc.MR_CP_Set((Ps*)pDB,-1);

				pDB->MR_Set_Method(MR_METHOD(5));
  				//m_pDoc->m_Doc.m_MR_TabIndex.Add(-1);

				m_Selected_Dim=pDB->MR_Get_Dim();
				if(m_Selected_Dim<=0) m_Selected_Dim=1;
				pDB->MR_Set_Dim(m_Selected_Dim);
				xx="DataBase_"+NParName;
				sheet_tab=e.GetWorksheet(xx);
				if(sheet_tab) {
					for(it=0;it<NRepetition;it++) {
						cell=sheet_tab->Cell(it+1,0);
						str1=cell->GetString();
						cell=sheet_tab->Cell(it+1,1);
						str2=cell->GetString();
						pDB->MR_AddSelection(string(str1),string(str2));
					}
				}
				sheet1 = e.GetWorksheet("MultiRun");	
		}
		else if(pos2>pos1) { //Table
			CString t=NParName.Mid(pos1+1,pos2-pos1-1);
			Nindex=AtoInt(t);
			NParName=NParName.Mid(0,pos1);
			m_Type="Table Parameters";
			pParB=m_pDoc->GetPtr(NParName, m_Type, NGroup);
			pParT=(P*)pParB;
			if(pParT!=NULL) {
				if(Ndimension>m_pDoc->m_Doc.MR_GetNumberOfDimensions())
				m_pDoc->m_Doc.MR_AddDimValue();
				m_pDoc->m_Doc.MR_SetDimValue(Ndimension-1,NRepetition);
				pParT->MR_Set_Dim(Nindex,Ndimension);
				pParT->MR_Set_Method(Nindex, MR_METHOD(imethod));
				pParT->MR_Set_Min(Nindex, NMin);
				pParT->MR_Set_Max(Nindex, NMax);
				pParT->MR_Set_Start(Nindex, NStart);
				if(pdParB!=NULL) {
					if(NDepIndex>=0) 
						pParT->Set_MR_ParDependence(Nindex, (Ps*)pdParB, NDepIndex);
					else
						pParT->Set_MR_ParDependence(Nindex, (Ps*)pdParB);
				}
				m_pDoc->m_Doc.MR_CP_Set(pParT,Nindex);	
			}
			if(imethod==4) {
				pParT->MR_Reset_Table(Nindex);
				xx="Table_"+NParName;
				sheet_tab=e.GetWorksheet(xx);
				if(sheet_tab) {
					for(it=0;it<NRepetition;it++) {
						cell=sheet_tab->Cell(it+1,0);
						iv=cell->GetInteger();
						cell=sheet_tab->Cell(it+1,1);
						v=float(cell->GetDouble());
						pParT->MR_Append_TableValue(Nindex,v);
					}
				}
				sheet1 = e.GetWorksheet("MultiRun");
			}
			

		}
		else { // Single Par
			pParB=m_pDoc->GetPtr(NParName, m_Type, NGroup);
			pPar=(Ps*)pParB;
			if(pPar!=NULL) {
				pPar->MR_Reset_Table();
				if(Ndimension>m_pDoc->m_Doc.MR_GetNumberOfDimensions())
					m_pDoc->m_Doc.MR_AddDimValue();
					m_pDoc->m_Doc.MR_SetDimValue(Ndimension-1,NRepetition);
					pPar->MR_Set_Dim(Ndimension);
					pPar->MR_Set_Method(MR_METHOD(imethod));
					pPar->MR_Set_Min(NMin);
					pPar->MR_Set_Max(NMax);
					pPar->MR_Set_Start(NStart);
					Nindex=-1;
					m_pDoc->m_Doc.MR_CP_Set(pPar,Nindex);
					if(imethod==4) {
						xx="Table_"+NParName;
						sheet_tab=e.GetWorksheet(xx);
						if(sheet_tab) {
							for(it=0;it<NRepetition;it++) {
								cell=sheet_tab->Cell(it+1,0);
								iv=cell->GetInteger();
								cell=sheet_tab->Cell(it+1,1);
								v=float(cell->GetDouble());
								pPar->MR_Append_TableValue(v);
							}
						}
						sheet1 = e.GetWorksheet("MultiRun");
					}
					if(pdParB!=NULL) {
						if(NDepIndex>=0)
							pPar->Set_MR_ParDependence((Ps*)pdParB, NDepIndex);
						else
							pPar->Set_MR_ParDependence((Ps*)pdParB);
					}	
			}
		}
	}
}
	sheet1 = e.GetWorksheet("Sheet1");
		size_t maxRows;
		size_t maxCols;

if (sheet1) // bELOW ONLY FROM sHEET WITH sHEET1 AS NAME - Reading Data
	{
		maxRows = sheet1->GetTotalRows();
		maxCols = sheet1->GetTotalCols();
	//	cout << "Dimension of " << sheet1->GetAnsiSheetName() << " (" << maxRows << ", " << maxCols << ")" << endl;

		printf("          ");
		for (size_t c=0; c<maxCols; ++c) printf("%10d", c+1);
	//	cout << endl;

		
		
		BasicExcelCell* cell = sheet1->Cell(0,0);
		CString test;

		for(size_t cc=0;cc<maxCols;++cc) {
			cell=sheet1->Cell(0,cc);
			test=cell->GetString();
		}
		BAYESIANCALIB_mp_NP=maxCols;
		BAYESIANCALIB_mp_CHAINLENGTH=8000;
		BAYESIANCALIB_mp_NUMBURNPERIOD=1;
		
		BAYESIANCALIB_mp_CALIB_RESET();
		BAYESIANCALIB_mp_CALIB_DEF();

		int count=0;
		int RunNo1, RunNo2;

		for(size_t rr=0;rr<maxRows;++rr){
			BAYESIANCALIB_mp_INDEXP=rr+1;	
			cell=sheet1->Cell(rr+1,0);
			RunNo1=int(cell->GetDouble());
			cell=sheet1->Cell(rr+2,0);
			RunNo2=int(cell->GetDouble());
			while (count<RunNo2) {
				count++;
				for(size_t cc=0;cc<maxCols;++cc) {
					BAYESIANCALIB_mp_INDEXP2=count;
					cell=sheet1->Cell(rr+1,cc);
					BAYESIANCALIB_mp_ACTUALP=float(cell->GetDouble());
					BAYESIANCALIB_mp_SETPCHAIN();
				}
			}
		}

		BAYESIANCALIB_mp_INDEXRUN=count;
		BAYESIANCALIB_mp_COVARIANCE();
	}
	BasicExcelWorksheet* sheet2=e.GetWorksheet("Sheet2");
	if(sheet1) { //Writing Covariance matrix to Sheet2
		float v;
		CString test;
		BasicExcelCell* cell = sheet2->Cell(0,0);
		BasicExcelCell* cell1 = sheet1->Cell(0,0);

		for(size_t rr=0;rr<maxCols;++rr){
			BAYESIANCALIB_mp_INDEXP=rr+1;
			test=sheet1->Cell(0,rr)->GetString();
			sheet2->Cell(0,rr+1)->SetString(test);
			sheet2->Cell(rr+1,0)->SetString(test);
			sheet2->Cell(maxCols+2,rr+1)->SetDouble(BAYESIANCALIB_mp_GETMEAN());
			BAYESIANCALIB_mp_INDEXP2=rr+1;
			v=BAYESIANCALIB_mp_GETCOCOR();
			sheet2->Cell(maxCols+3,rr+1)->SetDouble(v);
		}

		for(size_t rr=0;rr<maxCols;++rr){
			BAYESIANCALIB_mp_INDEXP=rr+1;
			for(size_t cc=0;cc<maxCols;++cc) {
				cell= sheet2->Cell(rr+1,cc+1);
				BAYESIANCALIB_mp_INDEXP2=cc+1;
				v=BAYESIANCALIB_mp_GETCOCOR();
				cell->Set((float)v);
			}
		}
	}

	BasicExcelWorksheet* sheet3=e.GetWorksheet("Sheet3");
	if(sheet3) { //Writing Covariance matrix to Sheet3
		float v;
		CString test;
		BasicExcelCell* cell = sheet3->Cell(0,0);
		BasicExcelCell* cell1 = sheet1->Cell(0,0);

		for(size_t rr=0;rr<maxCols;++rr){
			BAYESIANCALIB_mp_INDEXP=rr+1;
			test=sheet1->Cell(0,rr)->GetString();
			sheet3->Cell(0,rr+1)->SetString(test);
			sheet3->Cell(rr+1,0)->SetString(test);
			sheet3->Cell(maxCols+2,rr+1)->SetDouble(BAYESIANCALIB_mp_GETMEAN());
			BAYESIANCALIB_mp_INDEXP2=rr+1;
			v=BAYESIANCALIB_mp_GETCOVAR();
			sheet3->Cell(maxCols+3,rr+1)->SetDouble(v);
		}

		for(size_t rr=0;rr<maxCols;++rr){
			BAYESIANCALIB_mp_INDEXP=rr+1;
			for(size_t cc=0;cc<maxCols;++cc) {
				cell= sheet3->Cell(rr+1,cc+1);
				BAYESIANCALIB_mp_INDEXP2=cc+1;
				v=BAYESIANCALIB_mp_GETCOVAR();
				cell->Set((float)v);
			}
		}
	}



	sheet2=e.GetWorksheet("CoVariance");
	if(sheet2) { // Reading Covariance
		BasicExcelCell* cell;
		float v;
		maxRows = sheet2->GetTotalRows();
		maxCols = sheet2->GetTotalCols();
		if(maxRows==maxCols&&maxRows>3) {
			BAYESIANCALIB_mp_NP=m_pDoc->m_Doc.MR_Get_NumBayesPar();	
			BAYESIANCALIB_mp_CALIB_RESET();
			BAYESIANCALIB_mp_CALIB_DEF();

			for(size_t rr=2;rr<maxCols-3;++rr){ 
				BAYESIANCALIB_mp_INDEXP=rr-1;
				for(size_t cc=2;cc<maxCols-3;++cc) {
					cell= sheet2->Cell(rr,cc);
					BAYESIANCALIB_mp_INDEXP2=cc-1;
					BAYESIANCALIB_mp_ACTUALP=float(cell->GetDouble());
					m_pDoc->m_Doc.MR_Set_BayesCoCor(rr-2,cc-2,BAYESIANCALIB_mp_ACTUALP);
					BAYESIANCALIB_mp_SETCOVPROPOSAL();
				}
				cell=sheet2->Cell(maxCols-1, rr+2);
				v=float(cell->GetDouble());
	// Make use of mean value
				m_pDoc->m_Doc.MR_Set_BayesMean(rr-2,v);
				cell=sheet2->Cell(maxCols, rr+2);
				v=float(cell->GetDouble());
			}
		}
		else {
	//	Make message


		}

	}
	
	
	
	sheet3=e.GetWorksheet("CoCorrelation");
//	cout << endl;
	e.SaveAs(pszFileName);
	OnInitDialog();

	// Create a new workbook with 2 worksheets and write some contents.

//	m_pDoc->m_Doc.ImportMultiRunOptions();
	// TODO: Add your control notification handler code here


}


void MultiRunPar::OnBnClickedDataBaseMode()
{
	m_IsDatabase=TRUE;
	m_IsTable=FALSE;
	m_ShiftMode=TRUE;
	OnInitDialog();
	// TODO: Add your control notification handler code here
}

void MultiRunPar::OnAddRecord()
{




	CString m_index, m_Col;
	CString m_Group;
    CDB* pDB;
	CString m_SelectedKey,m_strNewValue;
	int m_select=0;

   CString strID, strIDKey;
   pDB=m_pDoc->m_Doc.DB_GetPointer(string(m_ParName));
   m_ParMethod=MR_METHOD(5);
   m_IsDatabase=TRUE;
   m_pPs=(Ps*)pDB;
   if(m_pDoc->m_Doc.MR_CP_Set((Ps*)pDB,-1))
   {
   }
   else {
	   int n;
	   n=pDB->MR_GetNumberOfSelections();
	   for(int i=0;i<n;i++) {
		   m_SelectedKey=((CDB*)m_pPs)->MR_GetKeySelection(i).c_str();
		}
	  	//ej lyckad

   };
   pDB->MR_Set_Method(MR_METHOD(5));
  	//m_pDoc->m_Doc.m_MR_TabIndex.Add(-1);

	m_Selected_Dim=pDB->MR_Get_Dim();
	if(m_Selected_Dim<=0) m_Selected_Dim=1;
	
	pDB->MR_Set_Dim(m_Selected_Dim);

	
	//OnCreateDimList();
	//OnCreateMethodList();
	int nRow;
	nRow=m_pDoc->m_Doc.MR_GetDimValue(m_Selected_Dim-1);




	MultiRunTable dlg(m_pDoc, (Ps*)pDB, m_TabIndex);
	if(dlg.DoModal()==IDOK)
		m_IsTableSet=TRUE;
	else
		m_IsTableSet=FALSE;
	/*










	//while(pDB->MR_GetNumberOfSelections()<m_pDoc->m_Doc.MR_GetDimValue(m_Selected_Dim)){
	if(pDB->MR_GetNumberOfSelections()<m_pDoc->m_Doc.MR_GetDimValue(m_Selected_Dim-1)) {
		if(pDB->GetName().Find("Soil Properties")>-1) {
			SP_Retrieve dlg(TRUE, m_pDoc);
			int resultTab=dlg.DoModal();
			if(resultTab==IDOK) {
				m_SelectedKey=dlg.m_SelectedId;
				m_strNewValue="Soil";
				((CDB*)m_pPs)->MR_AddSelection(m_strNewValue, m_SelectedKey);
			}
		}
		else {
  			PP_EDIT *pEditPlant;
			pEditPlant= new PP_EDIT(((CDB*)pDB)->GetName(),m_pDoc);
			pEditPlant->m_ReadOnly=TRUE;
			pEditPlant->m_TotalReadOnly=TRUE;
			pEditPlant->m_SelectedIndex=m_select;
 			if(pEditPlant->DoModal()==IDOK){
				m_strNewValue=pEditPlant->m_SelectedId;
				m_SelectedKey=pEditPlant->m_SelectedKey;
				m_select=pEditPlant->m_SelectedIndex+1;
			}
			delete pEditPlant;
			((CDB*)m_pPs)->MR_AddSelection(m_strNewValue, m_SelectedKey);
		}
	}
		OnUpdateStatus();
		OnUpdateButtons();

		OnCreateDimList();
		m_ParMethod=m_pPs->MR_Get_Method();
		OnCreateMethodList();
    
		CDialog::OnInitDialog();
		OnBnClickedButtoApply();
*/		
	
	// TODO: Add your control notification handler code here
}
