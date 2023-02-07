// ValidationSummaryVariables.cpp : implementation file
//

#include "../stdafx.h"
#include "../Util.h"
#include "./SelectWindowForValidationSummaryVariables.h"
#include "../CUG\SimDoc\MyCug.h"
#include "../CoupModel.h"
#include "../CoupModelDoc.h"
#include "../CoupModelView.h"
#include "../BasicExcel\BasicExcel.hpp"
using namespace YExcel;


// SelectWindowForValidationSummaryVariables dialog

IMPLEMENT_DYNAMIC(SelectWindowForValidationSummaryVariables, CDialog)
SelectWindowForValidationSummaryVariables::SelectWindowForValidationSummaryVariables(CCoupModelDoc* pDoc,CWnd* pParent /*=nullptr*/)
	: CDialog(SelectWindowForValidationSummaryVariables::IDD, pParent)
{
	m_pDoc=pDoc;
	m_NumSelected=0;
	m_ObsValue=-1.1e38f;
	m_TypeVal=0;
	m_AbsError=1.f;
	m_RelError=0.2f;
	m_NumSelected=m_pDoc->m_Doc.m_ValidationData.GetNumSumVarVariables();
	m_ItemSelectedShow=0;
	m_ChangeEnable=TRUE;
	m_Init = true;
	m_LinkVal = 0;
	m_LinkFileVal = 0;
	m_LinkKeyInMultiSimList=0;
	m_LinkKeyInFileString = "ID_PLOT";
	m_DependentVarTypeSelected = SIMSUMSOURCE::MULTIFILESTAT;
	m_Str_MultiSumPeriod_End = m_Str_MultiSumPeriod_Start = "";

}

SelectWindowForValidationSummaryVariables::~SelectWindowForValidationSummaryVariables()
{
}

void SelectWindowForValidationSummaryVariables::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SelectWindowForValidationSummaryVariables)


	DDX_Text(pDX, IDC_STATIC_Simulated, m_SimHeading);


	DDX_Text(pDX, IDC_STATIC_Number, m_NumberOfSelectedVariables);
	DDV_MaxChars(pDX, m_NumberOfSelectedVariables, 5);
//
	DDX_Text(pDX, IDC_EDIT2, m_SimVariableSel);
	DDX_Text(pDX, IDC_EDIT3, m_RelError);
	DDX_Text(pDX, IDC_EDIT4, m_AbsError);
	DDX_Text(pDX, IDC_EDIT6, m_ObsValue);

	DDX_Text(pDX, IDC_EDIT5, m_ItemSelectedShow);

	DDX_Text(pDX, IDC_EDIT_START, m_Str_MultiSumPeriod_Start);
	DDX_Text(pDX, IDC_EDIT_END, m_Str_MultiSumPeriod_End);
	//}}AFX_DATA_MAP

	DDX_Control(pDX, IDC_COMBO1, m_SumTypeVariable);
	DDX_Control(pDX, IDC_COMBO2, m_LinkNameVariable);
	DDX_Control(pDX, IDC_COMBO3, m_LinkFileNameBox);
	DDX_Control(pDX, IDC_COMBO4, m_LinkFileKeyId);
	DDX_Control(pDX, IDC_COMBO5, m_LinkSimMultiListKey);
	DDX_Control(pDX, IDC_COMBO6, m_DependentVarType);
}


BEGIN_MESSAGE_MAP(SelectWindowForValidationSummaryVariables, CDialog)

	ON_BN_CLICKED(ID_APPLY, OnApply)

	ON_BN_CLICKED(ID_APPLY2, OnApplyAllId)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_BN_DOUBLECLICKED(ID_APPLY, OnApply)
	ON_BN_DOUBLECLICKED(ID_APPLY2, OnApplyAllId)
	ON_BN_CLICKED(IDC_DISABLE, OnDisable)

	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnHdnItemclickList1)
	ON_LBN_SELCHANGE(IDC_SimulatedVarList2, OnLbnSelchangeSimulatedvarlist2)

	
	ON_EN_KILLFOCUS(IDC_EDIT4, OnEnKillfocusEdit4)
	ON_EN_KILLFOCUS(IDC_EDIT3, OnEnKillfocusEdit4)
	ON_EN_KILLFOCUS(IDC_EDIT6, OnEnKillfocusEdit4)
	ON_EN_KILLFOCUS(IDC_EDIT_START, OnKillFocusChangeDate)
	ON_EN_KILLFOCUS(IDC_EDIT_END, OnKillFocusChangeDate)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnEnKillfocusEdit4)
	ON_CBN_SELCHANGE(IDC_COMBO2, OnEnKillfocusEdit5)
	ON_CBN_SELCHANGE(IDC_COMBO3, OnEnKillfocusCSVFile)
	ON_CBN_SELCHANGE(IDC_COMBO4, OnEnKillfocusKeyInFile)
	ON_CBN_SELCHANGE(IDC_COMBO5, OnEnKillfocusKeyInMultiSimList)
	ON_CBN_SELCHANGE(IDC_COMBO6, OnKillFocusDependentType)
	ON_BN_CLICKED(IDC_SaveToExcel, OnBnClickedSavetoexcel)
	ON_BN_CLICKED(IDC_ReadFromExcel, OnBnClickedReadfromexcel)
END_MESSAGE_MAP()


// SelectWindowForValidationSummaryVariables message handlers

void SelectWindowForValidationSummaryVariables::MakeSimList()
{
	string Text;
	auto koll = m_DependentVarTypeSelected;
	if (m_VariableFromFile&&m_LinkFileVal!=string::npos) {
		m_LinkNameVariable.ResetContent();

		for (auto it = m_pDoc->m_Doc.MapFlexId[m_LinkFileVal].begin(); it != m_pDoc->m_Doc.MapFlexId[m_LinkFileVal].end(); it++) {
			string id = (*it).first;
			CString idc;
			idc = id.c_str();
			m_LinkNameVariable.AddString(idc);
		}

		if (m_LinkVal > -1) {

			m_LinkNameVariable.SetCurSel(m_LinkVal);
			auto n = m_LinkNameVariable.GetLBTextLen(m_LinkVal);
			CString str1;
			m_LinkNameVariable.GetLBText(m_LinkVal, str1.GetBuffer(n));

			m_LinkName = CT2A(str1).m_psz;
			str1.ReleaseBuffer();

		}

		m_LinkFileNameBox.ResetContent();
		for (size_t i = 0; i < m_pDoc->m_Doc.m_ValsumFlexFiles.size(); i++) {
			string id = m_pDoc->m_Doc.m_ValsumFlexFiles[i].FileName;
			CString idc;
			idc = id.c_str();
			m_LinkFileNameBox.AddString(idc);
		}
		if (m_LinkFileVal != string::npos) {
			m_LinkFileNameBox.SetCurSel(m_LinkFileVal);
			auto n = m_LinkFileNameBox.GetLBTextLen(m_LinkFileVal);
			CString str1;
			m_LinkFileNameBox.GetLBText(m_LinkFileVal, str1.GetBuffer(n));
			m_LinkFileName = CT2A(str1).m_psz;
			str1.ReleaseBuffer();

		}
	}
//  Key In File
	for (size_t i = 0; i < m_pDoc->m_Doc.m_MultiRun_Array.size(); i++) {
		for (size_t ii = 0; ii < m_pDoc->m_Doc.MR_Get_NumberOfParametersWithinDim(i + 1); ii++) {
			string test = m_pDoc->m_Doc.MR_GetParName(i + 1, ii + 1);
			if (m_pDoc->m_Doc.MR_GetTabIndex(i + 1, ii + 1) != string::npos) {
				test += "(";
				test += FUtil::STD_ItoAscii(m_pDoc->m_Doc.MR_GetTabIndex(i + 1, ii + 1) + 1);
				test += ")";
			}
			CString idc;
			idc = test.c_str();
			m_LinkSimMultiListKey.AddString(idc);
		}
	}

	if (m_LinkKeyInMultiSimList > -1) {
		m_LinkSimMultiListKey.SetCurSel(m_LinkKeyInMultiSimList);
	}




//Key in File
	size_t count = 0;
	for (auto it = m_pDoc->m_Doc.MapFlexId[m_LinkFileVal].begin(); it != m_pDoc->m_Doc.MapFlexId[m_LinkFileVal].end(); it++) {
		string id = (*it).first;
		CString idc;
		if (m_pDoc->m_Doc.m_ValsumFlexFiles[m_LinkFileVal].IDVarKey == id) 
			m_LinkKeyInFile = count;
		count++;
		idc = id.c_str();
		m_LinkFileKeyId.AddString(idc);
	}
	if(m_pDoc->m_Doc.m_ValsumFlexFiles.size()>m_LinkFileVal)
		auto it = m_pDoc->m_Doc.MapFlexId[m_LinkFileVal].find(m_pDoc->m_Doc.m_ValsumFlexFiles[m_LinkFileVal].IDVarKey);
	
	if (m_LinkKeyInFile > -1) {
		m_LinkFileKeyId.SetCurSel(m_LinkKeyInFile);
	}
	m_SumTypeVariable.ResetContent();





	m_DependentVarType.ResetContent();
	m_DependentVarType.AddString(_T("Simulated Output Variables"));
	m_DependentVarType.AddString(_T("Residuals from Validation List"));

	if (m_pDoc->m_Doc.IsOutputBinFileValid()) {

		m_DependentVarType.AddString(_T("Multi Run Output Variables"));

	}

	if (m_DependentVarTypeSelected >=SIMSUMSOURCE(0))
		m_DependentVarType.SetCurSel(int(SIMSUMSOURCE(m_DependentVarTypeSelected)));

	CListBox* pList;

	// Output variables
	pList=(CListBox*)GetDlgItem(IDC_SimulatedVarList2);
	if(pList->GetCount()>0) {
		int ncur=pList->GetCurSel();
		pList->ResetContent();
		pList->SetCurSel(ncur);
	}

	CString Name;
	vector<SIMB> vv;
	vector<string> names;
	if (m_DependentVarTypeSelected == SIMSUMSOURCE::MULTIFILESTAT) {
		m_SumTypeVariable.AddString(_T("Initial"));
		m_SumTypeVariable.AddString(_T("Final"));
		m_SumTypeVariable.AddString(_T("Minimum"));
		m_SumTypeVariable.AddString(_T("Maximum"));
		m_SumTypeVariable.AddString(_T("Mean"));
		m_SumTypeVariable.AddString(_T("Accumulated"));
		if (m_TypeVal>-1)
			m_SumTypeVariable.SetCurSel(m_TypeVal);
		names = m_pDoc->m_Doc.GetAllSelectedNameList();

		for (size_t i = 0; i < names.size(); i++) {
			Name = CString(names[i].c_str());
			pList->AddString(Name);
		}
	}
	else if (m_DependentVarTypeSelected == SIMSUMSOURCE::CSVFLEXFILE) {
		CString c_str;
		for (size_t i = 0; i < m_pDoc->m_Doc.m_ValSum_Array.size(); i++) {
			c_str = m_pDoc->m_Doc.m_ValSum_Array[i].Name.c_str();
			c_str += "-";
			if (m_pDoc->m_Doc.m_ValSum_Array[i].FlexSumVar) {
				c_str+=m_pDoc->m_Doc.m_ValSum_Array[i].IDVarName.c_str();
			}
			pList->AddString(c_str);

		}




	}
	else if (m_DependentVarTypeSelected == SIMSUMSOURCE::MULTI_OUTPGFILE) {
		string pgfilename;
		pgfilename = m_pDoc->m_Doc.GetOutputFileName(0,false, true);

		if (m_pDoc->m_Doc.m_PG_MultiOutputFile.Open(pgfilename)) {
			CString c_str;
			string str;
			for (size_t i = 0; i < m_pDoc->m_Doc.m_PG_MultiOutputFile.GetNumVariables(); i++) {
				str=m_pDoc->m_Doc.m_PG_MultiOutputFile.GetVarName(i+1);
				str += m_pDoc->m_Doc.m_PG_MultiOutputFile.GetVarId(i+1);
				c_str = str.c_str();
				pList->AddString(c_str);
			}

			if (m_Str_MultiSumPeriod_Start.GetLength() <= 0) {

				m_Str_MultiSumPeriod_Start = m_pDoc->m_Doc.m_PG_MultiOutputFile.GetStartDate().c_str();

			}
			if (m_Str_MultiSumPeriod_End.GetLength() <= 0) {
				m_Str_MultiSumPeriod_End = m_pDoc->m_Doc.m_PG_MultiOutputFile.GetEndDate().c_str();


			}
			GetDlgItem(IDC_STATIC_Start)->RedrawWindow();
			GetDlgItem(IDC_STATIC_End)->RedrawWindow();

			m_pDoc->m_Doc.m_PG_MultiOutputFile.CloseFile();
		}


	}
}
BOOL SelectWindowForValidationSummaryVariables::OnInitDialog() 
{
	
	m_NumberOfSelectedVariables=MFC_Util::ItoAscii(m_NumSelected);
	CDialog::OnInitDialog();

	if (m_Init) {


		m_SelectedSumList.AttachGrid(this, IDC_EDIT1);
		m_SelectedSumList.m_VariableFromFile = m_VariableFromFile;
		m_SelectedSumList.OnUpdate(&m_pDoc->m_Doc);
		
		m_Init = FALSE;

	}


	CString Text;

	CListBox* pList;
	int i1;

	//m_SimVarList=1;

	pList=(CListBox*)GetDlgItem(IDC_SimulatedVarList2);

	i1=pList->GetCurSel();
	

	MakeSimList();

	OnCreateSelList();

	
	pList->SetCurSel(i1);


	if(m_NewLists) {
		pList->SetCurSel(0);

		m_NewLists=FALSE;
	}
	if(m_SimVariableSel.GetLength()>0) {
		GetDlgItem(ID_APPLY)->ModifyStyle(WS_DISABLED,0);
	}
	else {
		GetDlgItem(ID_APPLY)->ModifyStyle(0,WS_DISABLED);
	}
	GetDlgItem(ID_APPLY)->RedrawWindow();

	if (!m_VariableFromFile) {
		GetDlgItem(IDC_STATIC)->ModifyStyle(WS_DISABLED, 0);
		GetDlgItem(IDC_STATIC3)->ModifyStyle(0, WS_DISABLED);
		GetDlgItem(IDC_COMBO2)->ModifyStyle(0, WS_DISABLED);
		GetDlgItem(IDC_COMBO3)->ModifyStyle(0, WS_DISABLED);
		GetDlgItem(IDC_COMBO4)->ModifyStyle(0, WS_DISABLED);
		GetDlgItem(IDC_COMBO5)->ModifyStyle(0, WS_DISABLED);
	}
	else {
		GetDlgItem(IDC_STATIC)->ModifyStyle(0, WS_DISABLED);
		GetDlgItem(IDC_STATIC3)->ModifyStyle(WS_DISABLED, 0);
		GetDlgItem(IDC_COMBO2)->ModifyStyle(WS_DISABLED, 0);
		GetDlgItem(IDC_COMBO3)->ModifyStyle(WS_DISABLED, 0);
		GetDlgItem(IDC_COMBO4)->ModifyStyle(WS_DISABLED, 0);
		GetDlgItem(IDC_COMBO5)->ModifyStyle(WS_DISABLED, 0);
	}
	GetDlgItem(IDC_STATIC)->RedrawWindow();
	GetDlgItem(IDC_STATIC3)->RedrawWindow();
	GetDlgItem(IDC_COMBO2)->RedrawWindow();
	GetDlgItem(IDC_COMBO3)->RedrawWindow();
	GetDlgItem(IDC_COMBO4)->RedrawWindow();
	GetDlgItem(IDC_COMBO5)->RedrawWindow();

	if (!m_VariableFromFile) {
		
		GetDlgItem(IDC_EDIT6)->ModifyStyle(WS_DISABLED, 0);
	}
	else {

		GetDlgItem(IDC_EDIT6)->ModifyStyle(0, WS_DISABLED);
	}
	GetDlgItem(IDC_EDIT6)->RedrawWindow();
	GetDlgItem(IDC_COMBO2)->RedrawWindow();


	

	GetDlgItem(IDC_RESET)->RedrawWindow();


	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

INT_PTR SelectWindowForValidationSummaryVariables::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DoModal();
}
void SelectWindowForValidationSummaryVariables::OnCreateSelList()
{


		CRect rect;
	
		m_NumSelected=m_pDoc->m_Doc.m_ValidationData.GetNumSumVarVariables();
			// Create the columns

		m_SelectedSumList.OnSetup();

 /*
	    LV_ITEM lvi;
		char szItem[256];
		lvi.mask = LVIF_TEXT;
		int icount=0;
		for(int i=0;i<m_NumSelected;i++){
		  	CString test;
					
		// First item
				lvi.iItem = i;
				lvi.iSubItem = 0;
				printf_s(szItem,FUtil::STD_ItoAscii(i+1)) ;				
				lvi.pszText = (LPTSTR)(LPCTSTR)szItem;
				m_ValidationListSel.InsertItem(&lvi);
				for(int ifn=1; ifn<7;ifn++) {
		// SubItem 1
					lvi.iSubItem = ifn;

					if(ifn==1) 
						test=m_pDoc->m_Doc.m_ValidationData.GetValSumVarGroup(i).c_str();
					else if(ifn==2){
						test=m_pDoc->m_Doc.m_ValidationData.GetValSumVarName(i).c_str();
						int nind=m_pDoc->m_Doc.m_ValidationData.GetValSumVarLocalIndex(i);
						if(nind>=0) {
							test+=MFC_Util::AddIndexString(nind+1);
						}
					}
					else if(ifn==3) {
						int ityp=m_pDoc->m_Doc.m_ValidationData.GetValSumVarOutputType(i);
						switch( ityp) {
							case 0: test="Initial";break;
							case 1: test="Final";break;
							case 2: test="Min";break;
							case 3: test="Max";break;
							case 4: test="Mean";break;
							case 5: test="Accumulated";break;
						}
					}
					else if(ifn==4){
						float fv=m_pDoc->m_Doc.m_ValidationData.GetValSumVarMVAL(i);
						if(fv>=-1.1e37)
							test=MFC_Util::FtoAscii(fv);
						else
							test="-";
					}
					else if(ifn==5){
						float fv=m_pDoc->m_Doc.m_ValidationData.GetValSumVarP_Error(i);
						if(fv>=0)
							test=MFC_Util::FtoAscii(fv);
						else
							test="-";
					}
					else if(ifn==6){
						float fv=m_pDoc->m_Doc.m_ValidationData.GetValSumVarA_Error(i);
						if(fv>=0) 
							test=MFC_Util::FtoAscii(fv);
						else
							test="-";
					}


 					printf(szItem,test );
					lvi.pszText = szItem;
					m_ValidationListSel.SetItem(&lvi);
				}
			}*/
	}

void SelectWindowForValidationSummaryVariables::OnApply() 
{	
	CListBox* pList;

	UpdateData(TRUE);

	m_TypeVal=m_SumTypeVariable.GetCurSel();

	m_LinkVal = m_LinkNameVariable.GetCurSel();

	pList = (CListBox*)GetDlgItem(IDC_SimulatedVarList2);
	int isim = pList->GetCurSel();

	SIMB selected;
	//m_NumSelected=m_pFile->GetNumberOfSelectedVal();
	if (m_DependentVarTypeSelected == SIMSUMSOURCE::MULTIFILESTAT) {
		vector<SIMB> vv;

		vv = m_pDoc->m_Doc.GetAllSelectedOutputsSortedByName();
		selected = FUtil::GetPointerforNumberinList(vv, isim);

	}
	else if (m_DependentVarTypeSelected == SIMSUMSOURCE::CSVFLEXFILE|| m_DependentVarTypeSelected == SIMSUMSOURCE::MULTI_OUTPGFILE) {
		selected.pBase = nullptr;
		selected.TabIndex = 0;


	}

	if (m_VariableFromFile)
		m_pDoc->m_Doc.m_ValidationData.AddValSumVarList(m_NumSelected, m_DependentVarTypeSelected, isim, m_TypeVal, selected, m_LinkName,m_LinkFileName, m_RelError, m_AbsError);

	else
		m_pDoc->m_Doc.m_ValidationData.AddValSumVarList(m_NumSelected, m_DependentVarTypeSelected, isim,  m_TypeVal , selected, m_ObsValue, m_RelError, m_AbsError);

	m_pDoc->m_Doc.m_ValSum_Array[m_NumSelected].MultiBin_StartMin = m_MultiSumPeriod_Start;
	m_pDoc->m_Doc.m_ValSum_Array[m_NumSelected].MultiBin_EndMin = m_MultiSumPeriod_End;
	
	m_NumSelected++;
	m_NumberOfSelectedVariables=MFC_Util::ItoAscii(m_NumSelected);
//	   m_pFile->SetNumberOfSelectedVal(m_NumSelected);



	m_SimVariableSel=_T("");

	m_DeleteEnable=FALSE;

	OnInitDialog();
	
}
void SelectWindowForValidationSummaryVariables::OnApplyAllId()
{
	CListBox* pList;
	UpdateData(TRUE);


	m_TypeVal = m_SumTypeVariable.GetCurSel();

	m_LinkVal = m_LinkNameVariable.GetCurSel();
	pList = (CListBox*)GetDlgItem(IDC_SimulatedVarList2);
	int isim = pList->GetCurSel();


	for (auto it = m_pDoc->m_Doc.MapFlexId[m_LinkFileVal].begin(); it != m_pDoc->m_Doc.MapFlexId[m_LinkFileVal].end(); it++) {
		string id = (*it).first;
		vector<SIMB> vv;
		vv = m_pDoc->m_Doc.GetAllSelectedOutputsSortedByName();
		SIMB selected = FUtil::GetPointerforNumberinList(vv, isim);
		if (m_VariableFromFile)
			m_pDoc->m_Doc.m_ValidationData.AddValSumVarList(m_NumSelected, m_DependentVarTypeSelected, isim, m_TypeVal, selected, id, m_LinkFileName, m_RelError, m_AbsError);

		else
			m_pDoc->m_Doc.m_ValidationData.AddValSumVarList(m_NumSelected, m_DependentVarTypeSelected, isim, m_TypeVal, selected, m_ObsValue, m_RelError, m_AbsError);

		m_pDoc->m_Doc.m_ValSum_Array[m_NumSelected].MultiBin_StartMin = m_MultiSumPeriod_Start;
		m_pDoc->m_Doc.m_ValSum_Array[m_NumSelected].MultiBin_EndMin = m_MultiSumPeriod_End;
		m_NumSelected++;
	}
	m_NumberOfSelectedVariables = MFC_Util::ItoAscii(m_NumSelected);
	//	   m_pFile->SetNumberOfSelectedVal(m_NumSelected);



	m_SimVariableSel = _T("");

	m_DeleteEnable = FALSE;

	OnInitDialog();

}
void SelectWindowForValidationSummaryVariables::OnOK() 
{
	// TODO: Add extra validation here
	CDialog::OnOK();
}

void SelectWindowForValidationSummaryVariables::OnReset() 
{
	size_t ncount;
	
	SimB* pSw;

	vector<SIMB> vv;
	for(size_t j=0; j<4;j++) {
			switch (j) {
			case 0:
				vv=m_pDoc->m_Doc.m_X_Array;
				break;
			case 1:
				vv=m_pDoc->m_Doc.m_T_Array;
				break;
			case 2:
				vv=m_pDoc->m_Doc.m_G_Array;
				break;
			case 3:
				vv=m_pDoc->m_Doc.m_D_Array;
				break;
			}
		ncount=vv.size();
		for(size_t i=0; i<ncount; i++) {
		  pSw = (SimB*)vv[i].pBase;	
		  if(pSw->Is_Vector())
			for(size_t ii=0;ii<((OutVector*)pSw)->GetNumberOfSelectedFlags();ii++) 
				((OutVector*)pSw)->SetValidFlagIndex(ii, -1);

	   }
	}
   	m_pDoc->m_Doc.m_ValidationData.ClearSumVarList();

   	m_NumberOfSelectedVariables=MFC_Util::ItoAscii(0);
	OnInitDialog();
}

void SelectWindowForValidationSummaryVariables::OnDisable() 
{
	OnReset();
	OnInitDialog();


	// TODO: Add your control notification handler code here
	
}



void SelectWindowForValidationSummaryVariables::OnBnClickedDeleteItem()
{
	CListBox* pList;//, *pListVal;
	int  vlocalindex;
	CString vgroup,vname;
	
	pList=(CListBox*)GetDlgItem(IDC_SimulatedVarList2);
	
	int SelectedIndex=pList->GetCurSel();


	vlocalindex=m_pDoc->m_Doc.m_ValidationData.GetValSumVarLocalIndex(m_ItemSelected);
	vgroup=m_pDoc->m_Doc.m_ValidationData.GetValSumVarGroup(m_ItemSelected).c_str();
	vname=m_pDoc->m_Doc.m_ValidationData.GetValSumVarName(m_ItemSelected).c_str();
	

	m_pDoc->m_Doc.m_ValidationData.RemoveValSumVarNode(m_ItemSelected);

//	m_pFile->RemoveDataDescription(m_ItemSelected);
	m_NumSelected--;
	m_NumberOfSelectedVariables=MFC_Util::ItoAscii(m_NumSelected);
	// Clean Flag from file
//	m_pFile->SetNumberOfSelectedVal(m_NumSelected);
	
	// Clean Flag from Sim Outputs
	//REMAINS
	/*
	 SimB* pSw;
	 int m_Iflag;
	 m_Iflag=m_ItemSelected;
	 pSw = (SimB*)m_pDoc->m_XTGD_Array.GetAt(m_Iflag);	
	 int itype=m_pDoc->m_XTGD_Type_Array.GetAt(m_Iflag);
	  if (pSw->IsKindOf(RUNTIME_CLASS(G))){
		  for(int ii=0;ii<((G*)pSw)->GetNumberOfSelectedFlags();ii++) 
				((G*)pSw)->SetValidFlagIndex(ii, -1);
	  }
	  else {
 		for(int ii=0;ii<((X*)pSw)->GetNumberOfSelectedFlags();ii++)
				((X*)pSw)->SetValidFlagIndex(ii, -1);
	  }
	  */
	 m_SimVariableSel=_T("");
	m_DeleteEnable=FALSE;
	OnInitDialog();
	pList->SetCurSel(SelectedIndex);

}
void SelectWindowForValidationSummaryVariables::OnHdnItemclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	m_ItemSelected=m_ValidationListSel.GetSelectionMark();
	m_ItemSelectedShow=m_ItemSelected+1;
	

	m_SimVariableSel=m_ValidationListSel.GetItemText(m_ItemSelected,2);// TODO: Add your control notification handler code here
	GetDlgItem(IDC_BUTTON1)->ModifyStyle(WS_DISABLED,0);
	GetDlgItem(IDC_BUTTON1)->RedrawWindow();
	m_DeleteEnable=TRUE;
	CListBox  *pList;

	pList=(CListBox*)GetDlgItem(IDC_SimulatedVarList2);
	int isim=pList->GetCurSel();

	int iOutputs=m_pDoc->m_Doc.m_ValidationData.GetValSumVarOutputType(m_ItemSelected);
	m_SumTypeVariable.SetCurSel(iOutputs);
	/*m_SimVariableSel=m_pDoc->GetValSumVarName(m_ItemSelected);
	int localindex=m_pDoc->GetValSumVarLocalIndex(m_ItemSelected);
	if(localindex>0) 
		m_SimVariableSel+="("+MFC_Util::ItoAscii(localindex)+")";*/
	m_RelError=m_pDoc->m_Doc.m_ValidationData.GetValSumVarP_Error(m_ItemSelected);
	m_AbsError=m_pDoc->m_Doc.m_ValidationData.GetValSumVarA_Error(m_ItemSelected);
	m_ObsValue=m_pDoc->m_Doc.m_ValidationData.GetValSumVarMVAL(m_ItemSelected);
	CDialog::OnInitDialog();

	pList->SetCurSel(isim);
	m_ChangeEnable=TRUE;
}

void SelectWindowForValidationSummaryVariables::OnLbnSelchangeValidationvarlist()
{
	// TODO: Add your control notification handler code here
	CListBox* pListVal, *pList;
	pListVal=(CListBox*)GetDlgItem(IDC_ValidationVarList);
	pList=(CListBox*)GetDlgItem(IDC_SimulatedVarList2);
	int isim=pList->GetCurSel();
	int SelectedIndex=pListVal->GetCurSel();
//	m_ObsValidationSel=m_pFile->GetDataDescription(SelectedIndex+1);

	OnInitDialog();
	pListVal->SetCurSel(SelectedIndex);
	pList->SetCurSel(isim);
}

void SelectWindowForValidationSummaryVariables::OnLbnSelchangeSimulatedvarlist2()
{
	
	CListBox* pList;
	CString Name;


	pList=(CListBox*)GetDlgItem(IDC_SimulatedVarList2);
	
	int SelectedIndex=pList->GetCurSel();
	pList->GetText(SelectedIndex, Name);
	Name=Name.TrimLeft();
	int i2=Name.Find(_T("*"));
	m_SimVariableSel=Name.Mid(i2);
	OnInitDialog();
//	OnEnKillfocusEdit4();
	pList->SetCurSel(SelectedIndex);


    m_ChangeEnable=TRUE;
			
	// TODO: Add your control notification handler code here
}

void SelectWindowForValidationSummaryVariables::OnKillFocusDependentType()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_DependentVarTypeSelected = SIMSUMSOURCE(m_DependentVarType.GetCurSel());
	if (m_ChangeEnable) {
		
		OnInitDialog();
	}
	if(m_DependentVarTypeSelected== SIMSUMSOURCE::MULTI_OUTPGFILE){
		GetDlgItem(IDC_EDIT_START)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_EDIT_END)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_STATIC_Start)->ModifyStyle(WS_DISABLED, 0);
		GetDlgItem(IDC_STATIC_End)->ModifyStyle(WS_DISABLED, 0);

	}
	GetDlgItem(IDC_EDIT_START)->RedrawWindow();
	GetDlgItem(IDC_EDIT_END)->RedrawWindow();
	GetDlgItem(IDC_STATIC_Start)->RedrawWindow();
	GetDlgItem(IDC_STATIC_Start)->RedrawWindow();
	GetDlgItem(IDC_STATIC_End)->RedrawWindow();

}

void SelectWindowForValidationSummaryVariables::OnEnKillfocusEdit4()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_TypeVal = m_SumTypeVariable.GetCurSel();
	if(m_ChangeEnable) {
	
		m_pDoc->m_Doc.m_ValidationData.SetValSumVarErrors(m_ItemSelected,m_TypeVal, m_ObsValue, m_RelError, m_AbsError);
		OnInitDialog();
	}

}
void SelectWindowForValidationSummaryVariables::OnEnKillfocusEdit5()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_LinkVal = m_LinkNameVariable.GetCurSel();
		//m_pDoc->m_Doc.m_ValidationData.SetValSumVarErrors(m_ItemSelected, m_TypeVal, m_ObsValue, m_RelError, m_AbsError);
	OnInitDialog();


}
void SelectWindowForValidationSummaryVariables::OnEnKillfocusCSVFile()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_LinkFileVal = m_LinkFileNameBox.GetCurSel();
	//m_pDoc->m_Doc.m_ValidationData.SetValSumVarErrors(m_ItemSelected, m_TypeVal, m_ObsValue, m_RelError, m_AbsError);
	OnInitDialog();


}
void SelectWindowForValidationSummaryVariables::OnEnKillfocusKeyInFile()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_LinkKeyInFile = m_LinkFileKeyId.GetCurSel();

	auto n = m_LinkFileKeyId.GetLBTextLen(m_LinkKeyInFile);
	CString str1;
	m_LinkFileKeyId.GetLBText(m_LinkKeyInFile, str1.GetBuffer(n));
	m_LinkKeyInFileString = CT2A(str1).m_psz;
	m_pDoc->m_Doc.m_ValsumFlexFiles[m_LinkFileVal].IDVarKey = m_LinkKeyInFileString;

	//m_pDoc->m_Doc.m_ValidationData.SetValSumVarErrors(m_ItemSelected, m_TypeVal, m_ObsValue, m_RelError, m_AbsError);
	OnInitDialog();


}
void SelectWindowForValidationSummaryVariables::OnEnKillfocusKeyInMultiSimList()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	m_LinkKeyInMultiSimList = m_LinkSimMultiListKey.GetCurSel();
	//m_pDoc->m_Doc.m_ValidationData.SetValSumVarErrors(m_ItemSelected, m_TypeVal, m_ObsValue, m_RelError, m_AbsError);
	OnInitDialog();


}


void SelectWindowForValidationSummaryVariables::OnBnClickedSavetoexcel()
{
	BasicExcel e;
	CString test;
	int icount;

	icount=0;
	int fileDlgResult;
	CString pszFileName ;
	LPCTSTR lpszDefExt=_T("xls");
	LPCTSTR lpszFilter =_T("*.xls");
	//The next line is only neccessary as long as the PGP does not handle long filenames!
    //PEJ Excluded info box
    //	MessageBox("Since PGP can not handle long filenames, no subpart of the path must\nbe longer than 8 characters plus a 3 letter extension!","Critical information",MB_OK|MB_ICONINFORMATION);
	//Create a read dialog
		CString szFilter = _T("Excel file (*.xls)|*.xls||");
		CFileDialog	ParFileDlg(FALSE,lpszDefExt,lpszFilter ,OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,szFilter);
	//Show the dialog
		fileDlgResult=ParFileDlg.DoModal();
	//If the cancel button has been pushed then return
		if (fileDlgResult==IDCANCEL)
			return;
	//Retrieve the filename of the file that is to be opened
		pszFileName = ParFileDlg.GetPathName();

// Create a new workbook with 1 sheet
	e.New(1);
	e.RenameWorksheet("Sheet1", "ValidationSumVariables");
	BasicExcelWorksheet* sheet = e.GetWorksheet("ValidationSumVariables");

	if (sheet)
	{
		sheet->Cell(0,0)->SetString("Sim Group");
		sheet->Cell(0,1)->SetString("Sim Type");
		sheet->Cell(0,2)->SetString("Sim Name");	
		sheet->Cell(0,3)->SetString("Sim Stage");
		sheet->Cell(0,4)->SetString("Observed Value");
		sheet->Cell(0,5)->SetString("Relative Error");	
		sheet->Cell(0,6)->SetString("Absolute Error");	
	
		

		for(int i=0;i<m_NumSelected;i++){
			string sval;
			sval=m_pDoc->m_Doc.m_ValidationData.GetValSumVarGroup(i).c_str();
			sheet->Cell(i+1,0)->SetString(sval.c_str());
		
			int itype=m_pDoc->m_Doc.m_ValidationData.GetValSumVarNameType(i);
			switch (itype){
				case (1):
					sval="State";
					break;
				case (2):
					sval="Flow";
					break;
				case (3):
					sval="Auxiliary";
					break;
				case (4):
					sval="Drivings";
					break;
			}
			
			sheet->Cell(i+1,1)->SetString(sval.c_str());
			
			sval=m_pDoc->m_Doc.m_ValidationData.GetValSumVarName(i).c_str();
			int nind=m_pDoc->m_Doc.m_ValidationData.GetValSumVarLocalIndex(i);
			if(nind>=0) {
				sval += "(";
				sval+=FUtil::STD_ItoAscii(nind + 1) + ")";
			}
			sheet->Cell(i+1,2)->SetString(sval.c_str());

			itype=m_pDoc->m_Doc.m_ValidationData.GetValSumVarOutputType(i);
			switch (itype){
				case (0):
					sval="Initial";
					break;
				case (1):
					sval="Final";
					break;
				case (2):
					sval="Min";
					break;
				case (3):
					sval="Max";
					break;
				case (4):
					sval="Mean";
					break;
				case (5):
					sval="Accumulated";
					break;
			}
			sheet->Cell(i+1,3)->SetString(sval.c_str());

			float v=m_pDoc->m_Doc.m_ValidationData.GetValSumVarMVAL(i);
			sheet->Cell(i+1,4)->SetDouble(v);

			v=m_pDoc->m_Doc.m_ValidationData.GetValSumVarP_Error(i);
			sheet->Cell(i+1,5)->SetDouble(v);

			v=m_pDoc->m_Doc.m_ValidationData.GetValSumVarA_Error(i);
			sheet->Cell(i+1,6)->SetDouble(v);

	}
	MFC_Util::MessageBox("The Data are now written to the excel file.","File Information",0);
	

	}
	e.SaveAs(CT2A(pszFileName).m_psz);

	// TODO: Add your control notification handler code here
}

void SelectWindowForValidationSummaryVariables::OnBnClickedReadfromexcel()
{
/*	BasicExcel e;
	int fileDlgResult, Selectedtyp;
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
	

BasicExcelWorksheet* sheet1 = e.GetWorksheet("ValidationSumVariables");
if (sheet1)
	{
		size_t maxRows = sheet1->GetTotalRows();
		size_t maxCols = sheet1->GetTotalCols();

		BasicExcelCell* cell = sheet1->Cell(0,0);
		CString test;

	
	//	Checke Headings
		for(size_t cc=0;cc<maxCols;++cc) {
			cell=sheet1->Cell(0,cc);
			test=cell->GetString();
		}

			cell=sheet1->Cell(0,1);
			test=cell->GetString();
// test=fileName
		OnReset();

		// Variables to be defined for each line
		int Num,NFileIndex, N_Type, N_LocalIndex;
		CString NGroup, NParName, m_Type, VALX, N_SType;
		float REL_error, ABS_error, OBS_v;
		m_NumSelected=0;
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
							Num=cell->GetInteger();
						else if(c==1)
							NFileIndex=cell->GetInteger();
						else if(c==4)
							N_Type=cell->GetInteger();
						break;
					case BasicExcelCell::DOUBLE:
						if(c==4)
							OBS_v=float(cell->GetDouble());
						else if(c==5)
							REL_error=float(cell->GetDouble());
						else if(c==6)
							ABS_error=float(cell->GetDouble());
						break;
					case BasicExcelCell::STRING:
						if(c==0)
							NGroup=cell->GetString();
						else if(c==1){
							N_SType=cell->GetString();
							if(N_SType=="State")
								m_TypeVal=1;
							else if(N_SType=="Flow")
								m_TypeVal=2;
							else if(N_SType=="Auxiliary")
								m_TypeVal=3;
							else if(N_SType=="Drivings")
								m_TypeVal=4;
						}
						else if(c==2)
							NParName=cell->GetString();
						else if(c==3)
							m_Type=cell->GetString();
							if(m_Type=="Initial")
								Selectedtyp=0;
							else if(m_Type=="Final")
								Selectedtyp=1;
							else if(m_Type=="Min")
								Selectedtyp=2;
							else if(m_Type=="Max")
								Selectedtyp=3;
							else if(m_Type=="Mean")
								Selectedtyp=4;
							else if(m_Type=="Accumulated")
								Selectedtyp=5;
						else if(c==4) {
							VALX=cell->GetString();
							REL_error=MFC_Util::AtoFloat(VALX);}
						else if(c==5) {
							VALX=cell->GetString();
							ABS_error=MFC_Util::AtoFloat(VALX);}
						break;
					case BasicExcelCell::WSTRING:
						printf("%10s", cell->GetWString());
						break;
				}
			}
		// Apply the line as new input to model
		int pos1, pos2;

		m_Type="Parameters";
		CString xx,str1, str2;


		
		pos1=NParName.Find("(");
		pos2=NParName.Find(")");
		if(pos2>pos1) {
			CString t=NParName.Mid(pos1+1,pos2-pos1-1);
			N_LocalIndex=MFC_Util::AtoInt(t)-1;
			NParName=NParName.Mid(0,pos1);
		}
		else {
			N_LocalIndex=-1;
		}

       m_pDoc->m_Doc.m_ValidationData.AddValSumVarList(m_NumSelected,Selectedtyp, m_TypeVal ,  string(NGroup), string(NParName), N_LocalIndex, OBS_v, REL_error,ABS_error);

	   m_NumSelected++;
	   m_NumberOfSelectedVariables=MFC_Util::ItoAscii(m_NumSelected);

/*
	   m_pDoc->AddValList(m_NumSelected, NFileIndex-1 ,N_Type,NGroup, NParName, N_LocalIndex,REL_error , ABS_error);

	   m_NumSelected++;
	   m_NumberOfSelectedVariables=MFC_Util::ItoAscii(m_NumSelected);
*/
		
	//	}
	//}
	OnInitDialog();
	// TODO: Add your control notification handler code here
}
void SelectWindowForValidationSummaryVariables::OnKillFocusChangeDate()
{
	UpdateData();
	string str = CT2A(m_Str_MultiSumPeriod_Start).m_psz;
	if(str.size()>0) m_MultiSumPeriod_Start = PGUtil::MinutConv(str);
	str = CT2A(m_Str_MultiSumPeriod_End).m_psz;
	if(str.size()>0) m_MultiSumPeriod_End = PGUtil::MinutConv(str);

}
