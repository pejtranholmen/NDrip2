// MultiRun.cpp : implementation file
//

#include "../stdafx.h"
#include "../Util.h"
#include "../CoupModel.h"
#include "../ModelTypes\NewModelType.h"
#include "MultiRun.h"
#include "MultiRunTable.h"
#include "DimNewValue.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MultiRun dialog
MultiRun::MultiRun(SIMB PAR, Doc* pDoc, CWnd* pParent /*=nullptr*/)
	: CDialog(MultiRun::IDD, pParent)
{
	m_IsTableSet=FALSE;
	m_IsLanFile=FALSE;
	m_Par=PAR;
	m_TabIndex=-1;
	if(!PAR.pBase->Is_PTable()) {
 		m_pPs=(Ps*)PAR.pBase;
		m_Selected_Dim=m_pPs->MR_Get_Dim();
	}
	else if (PAR.pBase->Is_PTable()) {
		m_pP=(P*)PAR.pBase;
		m_TabIndex=PAR.TabIndex;
		m_Selected_Dim=m_pP->MR_Get_Dim(m_Par.TabIndex);
	}
	else {
		m_pPs=nullptr;
		m_pP=nullptr;
		
	}


	m_pDoc=pDoc;


	if(m_Selected_Dim<=0) {	 // Values come from Ps or P objects.
		if(!m_Par.pBase->Is_PTable()&&m_pPs!=nullptr) {
			m_StartValue=m_pPs->GetValue();			
			m_MinValue=m_pPs->GetWarnMinValue();
			if(m_MinValue<-1.e38)
				m_MinValue=m_pPs->GetMinValue();

			m_MaxValue=m_pPs->GetWarnMaxValue();
			if(m_MaxValue<-1.e38)
				m_MaxValue=m_pPs->GetMaxValue();
			if(m_MinValue==m_MaxValue) {
 		 		m_MinValue=m_pPs->MR_Get_Min();
				m_MaxValue=m_pPs->MR_Get_Max();
			}
			m_pPs->MR_Set_Min(m_MinValue);
			m_pPs->MR_Set_Max(m_MaxValue);
			m_pPs->MR_Set_Start(m_StartValue);
			m_pPs->MR_Set_Method(MR_METHOD(2));
			m_Selected_Dim=1;
			m_pPs->MR_Set_Dim(m_Selected_Dim);
		}
		else {
			m_StartValue=m_pP->GetValue(m_Par.TabIndex);
		
			m_MinValue=m_pP->GetWarnMinValue(m_Par.TabIndex);
			if(m_MinValue<-1.e38)
				m_MinValue=m_pP->GetMinValue(m_Par.TabIndex);

			m_MaxValue=m_pP->GetWarnMaxValue(m_Par.TabIndex);
			if(m_MaxValue<-1.e38)
				m_MaxValue=m_pP->GetMaxValue(m_Par.TabIndex);
			if(m_MinValue==m_MaxValue) {
 		 		m_MinValue=m_pP->MR_Get_Min(m_Par.TabIndex);
				m_MaxValue=m_pP->MR_Get_Max(m_Par.TabIndex);
			}
			m_pP->MR_Set_Min(m_Par.TabIndex,m_MinValue);
			m_pP->MR_Set_Max(m_Par.TabIndex,m_MaxValue);
			m_pP->MR_Set_Start(m_Par.TabIndex,m_StartValue);
			m_pP->MR_Set_Method(m_Par.TabIndex, MR_METHOD(2));
			m_Selected_Dim=1;
			m_pP->MR_Set_Dim(m_Par.TabIndex, m_Selected_Dim);



		}





	}
	else{  // Values later come from edit boxes.
		if(m_Par.pBase->MR_Get_Method()<3||m_Par.pBase->MR_Get_Method()==9) {
			if(!m_Par.pBase->Is_PTable()) {
		 		m_MinValue=m_pPs->MR_Get_Min();
				m_MaxValue=m_pPs->MR_Get_Max();
				m_StartValue=m_pPs->MR_Get_Start();
				m_ErrMin=m_pPs->GetMinValue();
				m_ErrMax=m_pPs->GetMaxValue();
			}
			else {
		 		m_MinValue=m_pP->MR_Get_Min(m_Par.TabIndex);
				m_MaxValue=m_pP->MR_Get_Max(m_Par.TabIndex);
				m_StartValue=m_pP->MR_Get_Start(m_Par.TabIndex);

				m_ErrMin=m_pP->GetMinValue(m_Par.TabIndex);
				m_ErrMax=m_pP->GetMaxValue(m_Par.TabIndex);


			}
		}
	}



	//{{AFX_DATA_INIT(MultiRun)
	//}}AFX_DATA_INIT
}




void MultiRun::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MultiRun)
	DDX_Control(pDX, IDC_EDIT2, m_ctrlMax);
	DDX_Control(pDX, IDC_EDIT1, m_ctrlMin);
	DDX_Control(pDX, IDC_COMBO1, m_Num_Method);
	DDX_LBIndex(pDX, IDC_DIMENSIONS, m_Selected_Dim);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EDIT3, m_ctrlStart);
}


BEGIN_MESSAGE_MAP(MultiRun, CDialog)
	//{{AFX_MSG_MAP(MultiRun)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE, OnButtonChange)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_DIM, OnButtonDeleteDim)
	ON_BN_CLICKED(IDC_BUTTON_NEWDIM, OnButtonNewdim)
	ON_BN_CLICKED(IDC_BUTTON_Counter, OnBUTTONCounter)
	ON_BN_CLICKED(IDC_BUTTO_APPLY, OnButtonApply)
	ON_BN_CLICKED(ID_DEFINE_FILE, OnDefineFile)
	ON_BN_CLICKED(ID_TABLE_VALUES, OnTableValues)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MultiRun message handlers

BOOL MultiRun::OnInitDialog() // Get values. Set text.
{
   	CDialog::OnInitDialog();
	UpdateData(TRUE);

   	string Text;
 	Text="MultipleRun Changes for Parameter : ";
	if(!m_Par.pBase->Is_PTable()) {
		Text+=m_pPs->GetName();
		Text+=" (";
		Text+=m_pPs->GetUnit();
		Text+=")";

	}
	else {
		Text += m_pP->GetName();
		Text += " (";
		Text += m_pP->GetUnit();
		Text += ")";
		if(m_Par.TabIndex!=string::npos) Text+=", index="+FUtil::STD_ItoAscii(m_Par.TabIndex+1);

	}


	SetWindowText(CString(Text.c_str()));
	int method;
	if(!m_Par.pBase->Is_PTable()) {
		m_Selected_Dim=m_pPs->MR_Get_Dim();  // ERROR
		method=m_pPs->MR_Get_Method();
	}
	else { 
		m_Selected_Dim=m_pP->MR_Get_Dim(m_TabIndex); // ERROR
		method=m_pP->MR_Get_Method(m_Par.TabIndex);
	}
	m_ctrlMin.SetWindowText(MFC_Util::DtoAscii(m_MinValue));
	m_ctrlMax.SetWindowText(MFC_Util::DtoAscii(m_MaxValue));
	m_ctrlStart.SetWindowText(MFC_Util::DtoAscii(m_StartValue));
	if(method<=3||method==9){
		if(!m_Par.pBase->Is_PTable()) {
			m_MinValue=m_pPs->MR_Get_Min();
			m_MaxValue=m_pPs->MR_Get_Max();
			m_StartValue=m_pPs->MR_Get_Start();
		}
		else {
			m_MinValue=m_pP->MR_Get_Min(m_Par.TabIndex);
			m_MaxValue=m_pP->MR_Get_Max(m_Par.TabIndex);
			m_StartValue=m_pP->MR_Get_Start(m_Par.TabIndex);
		}
		m_ctrlMin.SetWindowText(MFC_Util::DtoAscii(m_MinValue));
		m_ctrlMax.SetWindowText(MFC_Util::DtoAscii(m_MaxValue));
		m_ctrlStart.SetWindowText(MFC_Util::DtoAscii(m_StartValue));
	}


	if(m_Selected_Dim>0&&method<=3) {
  		GetDlgItem(IDC_EDIT1)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_EDIT2)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_EDIT3)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_STATIC1)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_STATIC2)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_STATIC3)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(ID_DEFINE_FILE)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(ID_TABLE_VALUES)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDOK)->ModifyStyle(WS_DISABLED,0);
	}
	else if( m_Selected_Dim>0&&method==4){
  		GetDlgItem(IDC_EDIT1)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT2)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_STATIC1)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_STATIC2)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT3)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_STATIC3)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(ID_DEFINE_FILE)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(ID_TABLE_VALUES)->ModifyStyle(WS_DISABLED,0);
		if(m_IsTableSet) 
			GetDlgItem(IDOK)->ModifyStyle(WS_DISABLED,0);
		else
			GetDlgItem(IDOK)->ModifyStyle(0,WS_DISABLED);
   	}
		else if(m_Selected_Dim>0&&method==9) {
  		GetDlgItem(IDC_EDIT1)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_EDIT2)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_EDIT3)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_STATIC1)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_STATIC2)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_STATIC3)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(ID_DEFINE_FILE)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(ID_TABLE_VALUES)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDOK)->ModifyStyle(WS_DISABLED,0);
	}
	else {
		GetDlgItem(IDC_EDIT1)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT2)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_STATIC1)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_STATIC2)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_EDIT3)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_STATIC3)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(ID_DEFINE_FILE)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(ID_TABLE_VALUES)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDOK)->ModifyStyle(WS_DISABLED,0);
     }

	CListBox* pList;
	
	pList=(CListBox*)GetDlgItem(IDC_DIMENSIONS);
	if(pList->GetCount()>0) pList->ResetContent();
	if(m_pDoc->MR_IsEmpty()){
		m_pDoc->MR_AddNewDimensiomWith11Repeatitions();
		}
	pList->AddString(_T("No changes to be done"));
	for(size_t i=0;i<m_pDoc->MR_GetNumberOfDimensions();i++) {
		int num=m_pDoc->MR_GetNumberOfRepeationsForThisDimension(i);
		if(num>0) 
			Text="Dimension "+FUtil::STD_ItoAscii(i+1)+" -  n = "+FUtil::STD_ItoAscii(num)+ " ";
		else Text="    ";
		pList->AddString(CString(Text.c_str()));
	}
    pList->SetCurSel(m_Selected_Dim);  // Usual way to set a control.
	//if (pList->SetCurSel(m_Selected_Dim) == LB_ERR) TRACE("Error with setting cursor selection.");
	
	m_Num_Method.ResetContent();
	m_Num_Method.AddString(_T("Linear Changes"));
	m_Num_Method.AddString(_T("Logarithmic Changes"));
	m_Num_Method.AddString(_T("Stochastic Linear Changes"));
	m_Num_Method.AddString(_T("Stochastic Log Changes"));
	m_Num_Method.AddString(_T("Table file"));
	m_Num_Method.AddString(_T("Database Record"));
	m_Num_Method.AddString(_T("Same As"));
	m_Num_Method.AddString(_T("Relative Change As"));
	m_Num_Method.AddString(_T("Opposite Relative As"));
	m_Num_Method.AddString(_T("Bayesian Calibration"));
	m_Num_Method.AddString(_T("Connected"));
	m_Num_Method.AddString(_T("Bayesian Calibration using log distribution"));
	m_Num_Method.AddString(_T("Nelder Mead optimization"));

	m_Num_Method.SetCurSel(method);  // Alternate way to set a control when it will not contain information.

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
	GetDlgItem(IDC_EDIT1)->RedrawWindow();
	GetDlgItem(IDC_EDIT2)->RedrawWindow();
	GetDlgItem(IDC_EDIT3)->RedrawWindow();
	GetDlgItem(IDC_STATIC1)->RedrawWindow();
	GetDlgItem(IDC_STATIC2)->RedrawWindow();
	GetDlgItem(IDC_STATIC3)->RedrawWindow();
	GetDlgItem(ID_DEFINE_FILE)->RedrawWindow();
	GetDlgItem(ID_TABLE_VALUES)->RedrawWindow();

	return TRUE;  
}

void MultiRun::OnOK() // Read text. Set values.
{
	UpdateData(TRUE);

	if(m_Num_Method.GetCurSel()<4) {
 		CString str;
		m_ctrlMin.GetWindowText(str);
		m_MinValue=MFC_Util::AtoFloat(str);
		m_ctrlMax.GetWindowText(str);
		m_MaxValue=MFC_Util::AtoFloat(str);
	    if(!m_Par.pBase->Is_PTable()) 
		  	m_pPs->MR_Set_Min(m_MinValue);
		else
	  		m_pP->MR_Set_Min(m_Par.TabIndex, m_MinValue);
	}
	if(m_Num_Method.GetCurSel()==9||m_Num_Method.GetCurSel()==11) {
		CString str;
		m_ctrlMin.GetWindowText(str);
		m_MinValue=MFC_Util::AtoFloat(str);
		m_ctrlMax.GetWindowText(str);
		m_MaxValue=MFC_Util::AtoFloat(str);

		m_ctrlStart.GetWindowText(str);
		m_StartValue=MFC_Util::AtoFloat(str);

		if(!m_Par.pBase->Is_PTable()) {
		  	m_pPs->MR_Set_Min(m_MinValue);
			m_pPs->MR_Set_Max(m_MaxValue);
			m_pPs->MR_Set_Start(m_StartValue);
		}
		else {
	  		m_pP->MR_Set_Min(m_Par.TabIndex, m_MinValue);
			m_pP->MR_Set_Max(m_Par.TabIndex, m_MaxValue);
			m_pP->MR_Set_Start(m_Par.TabIndex, m_StartValue);

		}


	}
	/*else if(m_Num_Method.GetCurSel() == 3) { // LAN case
		  CString message;
		  message = "The number of replications (n) in the selected dimension will be the number of bins in the histogram.";
		  MessageBox(message);
		  //m_pPs->MR_Set_Bins(m_pDoc->MR_GetNumberOfRepeationsForThisDimension(m_Selected_Dim-1));
		 // m_pPs->CreateHistogram(m_pDoc->MR_GetNumberOfRepeationsForThisDimension(m_Selected_Dim-1),m_Par.TabIndex);
	}*/

	if(!m_Par.pBase->Is_PTable()) 
		m_pPs->MR_Set_Dim(m_Selected_Dim);
	else
		m_pP->MR_Set_Dim(m_Par.TabIndex, m_Selected_Dim);
	// What is  m_Selected_Dim? The value from the dimension edit box.
	if(m_Selected_Dim>0) {
		if(!m_Par.pBase->Is_PTable())
			m_pDoc->MR_CP_Set(m_pPs, m_Par.TabIndex);
		else
			m_pDoc->MR_CP_Set(m_pP, m_Par.TabIndex);
	}
	else {
		m_pDoc->MR_CP_Reset(m_pPs);
		CDialog::OnOK();
		return;
	}
	for(int i=m_pDoc->MR_GetNumberOfDimensions();i>0;i--) {
		if(m_pDoc->MR_Get_NumberOfParametersWithinDim(i)<1)
			m_pDoc->MR_DeleteDimValue(i-1);
		else break;
	}


	if(!m_Par.pBase->Is_PTable()) {
	  	if(m_Num_Method.GetCurSel() < 3) m_pPs->MR_Set_Max(m_MaxValue);

		m_pPs->MR_Set_Method(MR_METHOD(m_Num_Method.GetCurSel()));
	}
	else{
	  	if(m_Num_Method.GetCurSel() < 3) m_pP->MR_Set_Max(m_Par.TabIndex, m_MaxValue);
	 	m_pP->MR_Set_Method(m_Par.TabIndex, MR_METHOD(m_Num_Method.GetCurSel()));
	}
    // Special processing for the Table case.
	if(m_Num_Method.GetCurSel() == 4) { // Table case
		int actual_n_observations;
		if(!m_Par.pBase->Is_PTable()) { // Ps case 
			actual_n_observations = m_pPs->MR_GetSizeTable();
		    int expected_n_observations = m_pDoc->MR_GetNumberOfRepeationsForThisDimension(m_Selected_Dim-1);
  		    if (actual_n_observations > expected_n_observations) {
			    CString temp = _T("Only the first ");
				temp.Append(MFC_Util::ItoAscii(expected_n_observations));
				temp.Append(_T(" observations will be used."));
				MessageBox(temp, _T("Too many !"));
			}
			else if (actual_n_observations < expected_n_observations) {
  			    CString temp = _T("Only ") + 
					MFC_Util::ItoAscii(actual_n_observations)
					+ _T(" observations in the table");
				MessageBox(temp,_T("Too few !"));
				OnInitDialog();
				return;
			}
		}
		else { //P case
			actual_n_observations = m_pP->MR_GetSizeTable(m_Par.TabIndex);
	  		int expected_n_observations = m_pDoc->MR_GetNumberOfRepeationsForThisDimension(m_Selected_Dim-1);
			if (actual_n_observations > expected_n_observations) {
  				CString temp = _T("Only the first ") + 
					MFC_Util::ItoAscii(expected_n_observations)
					+ _T(" observations will be retained.");
				MessageBox(temp, _T("Too many !"));
			}
			else if (actual_n_observations < expected_n_observations) {
  			    CString temp = _T("Only ") + 
					MFC_Util::ItoAscii(actual_n_observations)
					+ _T(" observations in the table");
				MessageBox(temp,_T("Too few !"));
				OnInitDialog();
				return;
			}

		}
	}

	CDialog::OnOK();
}

void MultiRun::OnButtonChange() 
{
	if (!m_Par.pBase->Is_PTable())
		m_pPs->MR_Set_Dim(m_Selected_Dim);
	else
		m_pP->MR_Set_Dim(m_Par.TabIndex, m_Selected_Dim);
//	UpdateData(TRUE);
//	m_pPs->MR_Set_Dim(m_Selected_Dim);
	DimNewValue dlg(m_pDoc->MR_GetNumberOfRepeationsForThisDimension(m_Selected_Dim-1));
	if(dlg.DoModal()==IDOK) {
		m_pDoc->MR_SetRepeatitionsOfDimension(m_Selected_Dim-1, dlg.m_NewValue);
	}
	OnInitDialog();
}

void MultiRun::OnButtonDeleteDim() 
{
	UpdateData(TRUE);
	m_pDoc->MR_DeleteDimValue(m_Selected_Dim-1);
	OnInitDialog();
}

void MultiRun::OnButtonNewdim() 
{
	m_pDoc->MR_AddNewDimensiomWith11Repeatitions();
	OnInitDialog();
}

void MultiRun::OnBUTTONCounter() 
{
	int v=0;
	for(size_t i=0;i<m_pDoc->MR_GetNumberOfDimensions();i++) 
	m_pDoc->MR_SetDimCounter(i,1);
	OnInitDialog();
}

void MultiRun::OnButtonApply() // Read text. Set values.
{
	UpdateData(TRUE);

	// Check for invalid conditions.
	if(m_Num_Method.GetCurSel()<4) {
		CString str;
		m_ctrlMin.GetWindowText(str);
		m_MinValue=MFC_Util::AtoFloat(str);
		m_ctrlMax.GetWindowText(str);
		m_MaxValue=MFC_Util::AtoFloat(str);
		if((m_Num_Method.GetCurSel()==1||m_Num_Method.GetCurSel()==3)&&m_MinValue<=0) {
           MessageBox(_T("Zero or negative values not allowed for logaritmic distribution \n Changed min 1.E-5"),_T("Critical information"),MB_OK|MB_ICONINFORMATION);
    	   m_MinValue=1.e-5f;
		   	// Set dimension, method, and min.
			if(!m_Par.pBase->Is_PTable()) {
				m_pPs->MR_Set_Dim(m_Selected_Dim);
				m_pPs->MR_Set_Method(MR_METHOD(m_Num_Method.GetCurSel()));
				if(m_Num_Method.GetCurSel()<4)	m_pPs->MR_Set_Min(m_MinValue);
			}
			else {
				m_pP->MR_Set_Dim(m_Par.TabIndex, m_Selected_Dim);
				m_pP->MR_Set_Method(m_Par.TabIndex, MR_METHOD(m_Num_Method.GetCurSel()));
				if(m_Num_Method.GetCurSel()<4)	m_pP->MR_Set_Min(m_Par.TabIndex, m_MinValue);
			}
		    OnInitDialog();
    	}
       	if((m_Num_Method.GetCurSel()==1||m_Num_Method.GetCurSel()==3)&&m_MaxValue<=0) {
    		MessageBox(_T("Zero or negative values not allowed for logaritmic distribution \n Changed max-value to 1.0"),_T("Critical information"),MB_OK|MB_ICONINFORMATION);
    		m_MaxValue=1.;
			// Set dimension, method, and max.
			if(!m_Par.pBase->Is_PTable()) {
				m_pPs->MR_Set_Dim(m_Selected_Dim);
				m_pPs->MR_Set_Method(MR_METHOD(m_Num_Method.GetCurSel()));
				if(m_Num_Method.GetCurSel()<4)	m_pPs->MR_Set_Max(m_MaxValue);
			}
			else {
				m_pP->MR_Set_Dim(m_Par.TabIndex, m_Selected_Dim);
				m_pP->MR_Set_Method(m_Par.TabIndex, MR_METHOD(m_Num_Method.GetCurSel()));
				if(m_Num_Method.GetCurSel()<4)	m_pP->MR_Set_Max(m_Par.TabIndex, m_MaxValue);
			}
		   	OnInitDialog();
    	}
	}

	// Set dimension, method, min, and max.
	if(!m_Par.pBase->Is_PTable()) {
		m_pPs->MR_Set_Dim(m_Selected_Dim);
		m_pPs->MR_Set_Method(MR_METHOD(m_Num_Method.GetCurSel()));
		if(m_Num_Method.GetCurSel()<4)	m_pPs->MR_Set_Min(m_MinValue);
		if(m_Num_Method.GetCurSel()<4)	m_pPs->MR_Set_Max(m_MaxValue);
		if(m_Num_Method.GetCurSel()==9||m_Num_Method.GetCurSel()==11) {
			m_pPs->MR_Set_Min(m_MinValue);
			m_pPs->MR_Set_Max(m_MaxValue);
			m_pPs->MR_Set_Start(m_StartValue);
		}
		if(m_Selected_Dim>0) m_pDoc->MR_CP_Set(m_pPs, -1);
		else {
			CString temp = _T("No multiple runs will be applied for your parameter.");
			MessageBox(temp);
			m_pDoc->MR_CP_Reset(m_pPs);
			return;
		}


	}
	else {
		m_pP->MR_Set_Dim(m_TabIndex, m_Selected_Dim);
		m_pP->MR_Set_Method(m_TabIndex, MR_METHOD(m_Num_Method.GetCurSel()));
		if(m_Num_Method.GetCurSel()<4)	m_pP->MR_Set_Min(m_TabIndex, m_MinValue);
		if(m_Num_Method.GetCurSel()<4)	m_pP->MR_Set_Max(m_TabIndex, m_MaxValue);

		if(m_Num_Method.GetCurSel()==9||m_Num_Method.GetCurSel()==11) {
			m_pP->MR_Set_Min(m_TabIndex, m_MinValue);
			m_pP->MR_Set_Max(m_TabIndex, m_MaxValue);
			m_pP->MR_Set_Start(m_TabIndex, m_StartValue);
		}
		if(m_Selected_Dim>0) m_pDoc->MR_CP_Set(m_pP, m_TabIndex);
		else {
			CString temp = _T("No multiple runs will be applied for your parameter.");
			MessageBox(temp);
			m_pDoc->MR_CP_Reset(m_pPs);
			return;
		}



	}
	// Further processing on dimensions.

	for(int i=m_pDoc->MR_GetNumberOfDimensions();i>0;i--) {
		if(m_pDoc->MR_Get_NumberOfParametersWithinDim(i)<1)
			m_pDoc->MR_DeleteDimValue(i-1);
		else break;
	}
	
	OnInitDialog();
}

void MultiRun::OnDefineFile() 
{

	
}

void MultiRun::OnTableValues() 
{
	if (!m_Par.pBase->Is_PTable()) {
		MultiRunTable dlg(m_pDoc, m_pPs, -1);
		if (dlg.DoModal() == IDOK)
			m_IsTableSet = TRUE;
		else
			m_IsTableSet = FALSE;
	}
	else {
		MultiRunTable dlg(m_pDoc, m_pP, m_TabIndex);
		if (dlg.DoModal() == IDOK)
			m_IsTableSet = TRUE;
		else
			m_IsTableSet = FALSE;

	}


	OnInitDialog();
}

