// MultiRunTable.cpp : implementation file
//

#include "../stdafx.h"
#include "../Util.h"
#include "../CoupModel.h"
#include "../CoupModelDoc.h"
#include "../SP\SP_Retrieve.h"
#include "MultiRunTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MultiRunTable dialog


MultiRunTable::MultiRunTable(Doc *pDoc, SimB *pCP, int index, BOOL All, CWnd* pParent /*=nullptr*/)
	: CDialog(MultiRunTable::IDD, pParent)
{
	m_pDoc=pDoc;
	if(index==-2) {  //New CDB identification
		m_pDB=(CDB*)pCP;
		m_pCP=pCP;
	}
	else {
		m_pCP=pCP;
		m_pDB=nullptr;
	}

	if(index<0) index=-1;
	m_TabIndex=index;
	m_InitDialog=FALSE;
	m_AllTables=All;
	//{{AFX_DATA_INIT(MultiRunTable)
	//}}AFX_DATA_INIT
}


void MultiRunTable::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MultiRunTable)
	DDX_Control(pDX, IDC_EDIT1, m_TableValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MultiRunTable, CDialog)
	//{{AFX_MSG_MAP(MultiRunTable)
	ON_BN_CLICKED(ID_FILE, OnFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MultiRunTable message handlers

void MultiRunTable::OnFile() 
{

	if(m_pDB) {

		int ndim=m_pDB->MR_Get_Dim();
		size_t nRow=size_t(m_pDoc->MR_GetNumberOfRepeationsForThisDimension(ndim-1));
		int m_select=0;
		if(m_pDB->MR_GetNumberOfSelections()<nRow) {
/*
		if(m_pDB->GetName().find("Soil Properties")>-1) {
			SP_Retrieve dlg(TRUE, m_pDoc);
			int resultTab=dlg.DoModal();
			if(resultTab==IDOK) {
				m_SelectedKey=dlg.m_SelectedId;
				m_strNewValue="Soil";
				m_pDB->MR_AddSelection(string(m_strNewValue), string(m_SelectedKey));
			}
		}
		else {
  			PP_EDIT *pEditPlant;
			pEditPlant= new PP_EDIT(m_pDB->GetName().c_str(),m_pDoc);
			pEditPlant->m_ReadOnly=TRUE;
			pEditPlant->m_TotalReadOnly=TRUE;
			pEditPlant->m_SelectedIndex=m_select;
 			if(pEditPlant->DoModal()==IDOK){
				m_strNewValue=pEditPlant->m_SelectedId;
				m_SelectedKey=pEditPlant->m_SelectedKey;
				m_select=pEditPlant->m_SelectedIndex+1;
			}
			delete pEditPlant;
			m_pDB->MR_AddSelection(string(m_strNewValue),string(m_SelectedKey));
		}*/
	}

	}
	else if(m_TabIndex==-1){
	  ((Ps*)m_pCP)->MR_Reset_Table();

	}
	else {
	  ((P*)m_pCP)->MR_Reset_Table(m_TabIndex);

	}
  OnInitDialog();
}

void MultiRunTable::Update()
{
	string title;
	int n_lines, ndim;
	if(m_TabIndex==-1) {
		ndim=m_pCP->MR_Get_Dim();
	}
	else {
		ndim=m_pCP->MR_Get_Dim(m_TabIndex);
	}
	n_lines=m_pDoc->MR_GetNumberOfRepeationsForThisDimension(ndim-1);

	if (m_TabIndex == -1) {

		title = m_pCP->GetName().c_str();
		title+= " : " ;
		title+=FUtil::STD_ItoAscii(m_linevalid) + " values of "+FUtil::STD_ItoAscii(n_lines) +" total number of repetition";
	}
	else {

		title = m_pCP->GetName();
		title+= "(" + FUtil::STD_ItoAscii(m_TabIndex + 1) + ") : " 
		+ FUtil::STD_ItoAscii(m_linevalid) + " values of "+ FUtil::STD_ItoAscii(n_lines) +" total number of repetition";
	}
	SetWindowText(CString(title.c_str()));

}
BOOL MultiRunTable::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int n_lines=0;
	if(!m_InitDialog) {
		m_SimpleGrid.AttachGrid(this, IDC_SIMPLE_GRID);
		int  ndim;
		if(m_TabIndex==-1) {
			//n_lines=m_pCP->MR_GetSizeTable();	
			ndim=m_pCP->MR_Get_Dim();
		}
		else {
			ndim=m_pCP->MR_Get_Dim(m_TabIndex);
			
			//n_lines=m_pCP->MR_GetSizeTable(m_TabIndex);
		}
		if(ndim>=1)
			n_lines=m_pDoc->MR_GetNumberOfRepeationsForThisDimension(ndim-1);


		m_SimpleGrid.SetMultiTable(n_lines,m_pCP,m_TabIndex,m_pDoc, this, &m_linevalid, m_AllTables);
		m_SimpleGrid.OnSetup();
	}

	/*	CRect rect;
		rect=CRect(0,0,m_cx+5,m_cy+35);
		MoveWindow(rect,TRUE);
		m_HScrollBar.GetWindowRect(rect);
		rect=CRect(140,m_cy-20,m_cx-20,m_cy);
		m_HScrollBar.MoveWindow(rect,TRUE);
		rect=CRect(m_cx-20,0,m_cx,m_cy-20);
		m_VScrollBar.MoveWindow(rect,TRUE);

*/



	 m_InitDialog=TRUE;












	size_t n_lines_ini, nRow;
	string title;
	if(m_pDB) {
		GetDlgItem(ID_FILE)->SetWindowText(_T("Add Record from data base"));
		int ndim=m_pDB->MR_Get_Dim();
		nRow=m_pDoc->MR_GetNumberOfRepeationsForThisDimension(ndim-1);
		title=m_pDB->GetName().c_str();
		title+=" : "+FUtil::STD_ItoAscii(m_pDB->MR_GetNumberOfSelections())+" of ";
		title+=FUtil::STD_ItoAscii(nRow)+ " items";
	}
	else if (m_pCP->GetType()==PAR_SINGLE) {
		n_lines_ini = ((Ps*)m_pCP)->MR_GetSizeTable();
		title = m_pCP->GetName();
		title+= " : " 
   		+ FUtil::STD_ItoAscii(n_lines_ini) + " values of "+FUtil::STD_ItoAscii(n_lines) +" total number of repetition";
	}
	else {
		n_lines_ini = ((P*)m_pCP)->MR_GetSizeTable(m_TabIndex);
		title = m_pCP->GetName();
		title+= "(" + FUtil::STD_ItoAscii(m_TabIndex + 1) + ") : " 
		+ FUtil::STD_ItoAscii(n_lines_ini) + " values of "+ FUtil::STD_ItoAscii(n_lines) +" total number of repetition";
	}
	SetWindowText(CString(title.c_str()));


		string str="";
		if(m_pDB) {

		int n=m_pDB->MR_GetNumberOfSelections();
		for(int i=0;i<n;i++) {
			m_SelectedKey=m_pDB->MR_GetKeySelection(i);
			str+=m_SelectedKey+"\r\n";
		}

//((CDB*)m_pPs)->MR_AddSelection(m_strNewValue, m_SelectedKey);
		}
		else if(m_TabIndex==-1) {
			for(size_t i=0;i<((Ps*)m_pCP)->MR_GetSizeTable();i++)
			str+=MFC_Util::DtoAscii(((Ps*)m_pCP)->MR_Get_TabValue(i))+"\r\n";
		}
		else {
			for(size_t i=0;i<((P*)m_pCP)->MR_GetSizeTable(m_TabIndex);i++)
			str+=MFC_Util::DtoAscii(((P*)m_pCP)->MR_Get_TabValue(m_TabIndex,i))+"\r\n";
		}
	    m_TableValue.SetWindowText(CString(str.c_str()));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void MultiRunTable::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	CString kalle;
	int nRow;
	m_TableValue.GetWindowText(kalle);
	if(m_pDB) {
		int ndim=m_pDB->MR_Get_Dim();
		nRow=m_pDoc->MR_GetNumberOfRepeationsForThisDimension(ndim-1);
		m_pDB->MR_ResetSelection();
	}
	
		int numlines;
		int messageRes;
		if(m_pDB) {
			numlines=m_pDB->MR_GetNumberOfSelections();
			if(numlines<nRow){
				string message="You have to add as many lines as number of repetitions - Continue ?";
				messageRes=MFC_Util::MessageBoxReturn(message,"Too few values",MB_YESNO|MB_ICONQUESTION|MB_TASKMODAL);

//Currentchoice=MessageBox(nullptr, LPCTSTR(message),"Change of values and default values",MB_YESNO|);
				if(messageRes==IDYES) {
					OnInitDialog();
					return;
				}
			}

		}
		else if(m_TabIndex==-1) 
			numlines=((Ps*)m_pCP)->MR_GetSizeTable();
		else
			numlines=((P*)m_pCP)->MR_GetSizeTable(m_TabIndex);

		if(numlines<2) {
			string message="Add more than one value if you should use Multiple run !";
		    MFC_Util::MessageBox(message,"Too few values",MB_OK|MB_ICONEXCLAMATION);
			OnInitDialog();
		}
	CDialog::OnOK();
}
