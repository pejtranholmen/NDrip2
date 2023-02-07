// MultiRunDB.cpp : implementation file
//

#include "../stdafx.h"
#include "../CUG\MyCug.h"
#include "../CoupModel.h"
#include "MultiRunDB.h"
#include "DimNewValue.h"
#include "../CoupModelView.h"
#include "../SP\SP_Retrieve.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MultiRunDB dialog


MultiRunDB::MultiRunDB(CDB* pPar, CCoupModelDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(MultiRunDB::IDD, pParent)
{
	m_TabIndex=-1;
	m_pDB=pPar;
	m_pCP=(Ps*)pPar;
	m_pDoc=pDoc;
	m_Selected_Dim=pPar->MR_Get_Dim();
    pPar->MR_Set_Method(MR_METHOD(5));

	//{{AFX_DATA_INIT(MultiRunDB)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void MultiRunDB::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MultiRunDB)
	DDX_Control(pDX, IDC_DIMENSIONS, m_Dimension);
	DDX_Control(pDX, IDC_Records, m_DataRecords);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MultiRunDB, CDialog)
	//{{AFX_MSG_MAP(MultiRunDB)
	ON_BN_CLICKED(ID_DATABASE_ADD, OnDatabaseAdd)
	ON_BN_CLICKED(ID_DATABASE_RESET, OnDatabaseReset)
	ON_BN_CLICKED(IDC_BUTTO_APPLY, OnButtoApply)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE, OnButtonChange)
	ON_BN_CLICKED(IDC_BUTTON_Counter, OnBUTTONCounter)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_DIM, OnButtonDeleteDim)
	ON_BN_CLICKED(IDC_BUTTON_NEWDIM, OnButtonNewdim)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MultiRunDB message handlers

void MultiRunDB::OnDatabaseAdd() 
{
	// TODO: Add your control notification handler code here
	if(m_pDB->GetName().find("Soil Properties")>-1) {
		SP_Retrieve dlg(TRUE, m_pDoc);
		int resultTab=dlg.DoModal();
		if(resultTab==IDOK) {
			m_SelectedKey=dlg.m_SelectedId;
			m_strNewValue=((CDB*)m_pDB)->GetStringValue().c_str();
			((CDB*)m_pDB)->MR_AddSelection(string(m_strNewValue), string(m_SelectedKey));
		}
	}
	else {
  		PP_EDIT *pEditPlant;
		pEditPlant= new PP_EDIT(((CDB*)m_pDB)->GetName().c_str(),m_pDoc);
		pEditPlant->m_ReadOnly=TRUE;
		pEditPlant->m_TotalReadOnly=TRUE;
		pEditPlant->m_SelectedIndex=m_select;
 		if(pEditPlant->DoModal()==IDOK){
			m_strNewValue=pEditPlant->m_SelectedId;
			m_SelectedKey=pEditPlant->m_SelectedKey;
			m_select=pEditPlant->m_SelectedIndex+1;
		}
		delete pEditPlant;
		((CDB*)m_pDB)->MR_AddSelection(string(m_strNewValue), string(m_SelectedKey));
	}
	OnInitDialog();
	
}


void MultiRunDB::OnDatabaseReset() 
{
	// TODO: Add your control notification handler code here
   m_pDB->MR_ResetSelection();	
   OnInitDialog();
}

void MultiRunDB::OnButtoApply() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_Selected_Dim=m_Dimension.GetCurSel();
	((CDB*)m_pDB)->MR_Set_Dim(m_Selected_Dim);

	if(m_Selected_Dim>0) m_pDoc->m_Doc.MR_CP_Set(m_pCP, m_TabIndex);
	else {
		m_pDoc->m_Doc.MR_CP_Reset(m_pCP);
		return;
	}
	for(int i=m_pDoc->m_Doc.MR_GetNumberOfDimensions();i>0;i--) {
		if(m_pDoc->m_Doc.MR_Get_NumberParDim(i)<1)
			m_pDoc->m_Doc.MR_DeleteDimValue(i-1);
		else break;
	}

    int expected_n_observations = m_pDoc->m_Doc.MR_GetDimValue(((CDB*)m_pDB)->MR_Get_Dim()-1);

	OnInitDialog();
}
	

void MultiRunDB::OnButtonChange() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
		((CDB*)m_pDB)->MR_Set_Dim(m_Selected_Dim);
	DimNewValue dlg(m_pDoc->m_Doc.MR_GetDimValue(m_Selected_Dim-1));
	if(dlg.DoModal()==IDOK) {
		m_pDoc->m_Doc.MR_SetDimValue(m_Selected_Dim-1, dlg.m_NewValue);
	}
	OnInitDialog();
	
}

void MultiRunDB::OnBUTTONCounter() 
{
	int v=0;
	for(int i=0;i<m_pDoc->m_Doc.MR_GetNumberOfDimensions();i++) 
	m_pDoc->m_Doc.MR_SetDimCounter(i,1);
	OnInitDialog();
	
}

void MultiRunDB::OnButtonDeleteDim() 
{
	UpdateData(TRUE);
	m_pDoc->m_Doc.MR_DeleteDimValue(m_Selected_Dim-1);
	OnInitDialog();
	
}

void MultiRunDB::OnButtonNewdim() 
{
	m_pDoc->m_Doc.MR_AddDimValue();
	OnInitDialog();
	
}

BOOL MultiRunDB::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Selected_Dim=((CDB*)m_pDB)->MR_Get_Dim();

	
	if(m_Dimension.GetCount()>0) m_Dimension.ResetContent();
	if(m_pDoc->m_Doc.MR_IsEmpty()){
		m_pDoc->m_Doc.MR_AddDimValue();
		}
	m_Dimension.AddString("No changes to be done");
	CString Text;
	for(int i=0;i<m_pDoc->m_Doc.MR_GetNumberOfDimensions();i++) {
		int num=m_pDoc->m_Doc.MR_GetDimValue(i);
		if(num>0) 
			Text="Dimension "+ItoAscii(i+1)+" -  n = "+ItoAscii(num)+ " ";
		else Text="    ";
		m_Dimension.AddString(Text);
	}
	m_Dimension.SetCurSel(m_Selected_Dim);

// Update listbox of database records
	m_DataRecords.ResetContent();
	CString str;
	for(int i=0;i<m_pDB->MR_GetNumberOfSelections();i++) {
		str=ItoAscii(i+1)+":";
		str+=m_pDB->MR_GetSelection(i).c_str();
		str=str.Mid(0,26);
		str+="\t";
		str+=m_pDB->MR_GetKeySelection(i).c_str();
		m_DataRecords.AddString(str);
	}


	UpdateData(TRUE);

	if(m_Selected_Dim<=0) {
		GetDlgItem(IDC_BUTTON_CHANGE)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_BUTTON_DELETE_DIM)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_BUTTON_Counter)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(ID_DATABASE_ADD)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(ID_DATABASE_RESET)->ModifyStyle(0,WS_DISABLED);
	}
	else {
		GetDlgItem(IDC_BUTTON_CHANGE)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_BUTTON_DELETE_DIM)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_BUTTON_Counter)->ModifyStyle(WS_DISABLED,0);
		if(m_pDB->MR_GetNumberOfSelections()==m_pDoc->m_Doc.MR_GetDimValue(m_Selected_Dim-1)) 
			 GetDlgItem(ID_DATABASE_ADD)->ModifyStyle(0,WS_DISABLED);
		else
			 GetDlgItem(ID_DATABASE_ADD)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(ID_DATABASE_RESET)->ModifyStyle(WS_DISABLED,0);

	}

	GetDlgItem(ID_DATABASE_RESET)->RedrawWindow();
	GetDlgItem(ID_DATABASE_ADD)->RedrawWindow();
	GetDlgItem(IDC_BUTTON_CHANGE)->RedrawWindow();
	GetDlgItem(IDC_BUTTON_DELETE_DIM)->RedrawWindow();
	GetDlgItem(IDC_BUTTON_Counter)->RedrawWindow();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void MultiRunDB::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	m_Selected_Dim=m_Dimension.GetCurSel();
	((CDB*)m_pDB)->MR_Set_Dim(m_Selected_Dim);
	m_TabIndex=-1;
	if(m_Selected_Dim>0) m_pDoc->m_Doc.MR_CP_Set(m_pCP, m_TabIndex);
	else {
		m_pDoc->m_Doc.MR_CP_Reset(m_pCP);
		CDialog::OnOK();
		return;
	}
	for(int i=m_pDoc->m_Doc.MR_GetNumberOfDimensions();i>0;i--) {
		if(m_pDoc->m_Doc.MR_Get_NumberParDim(i)<1)
			m_pDoc->m_Doc.MR_DeleteDimValue(i-1);
		else break;
	}
	((CDB*)m_pDB)->MR_Set_Method(MR_METHOD(5));

	
	CDialog::OnOK();

}

void MultiRunDB::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}
