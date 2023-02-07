// ValidationVariables.cpp : implementation file
//

#include "../stdafx.h"
#include "../Util.h"
#include "../CoupModel.h"
#include "../CUG\SimDoc\MyCug.h"

#include "../CoupModelDoc.h"
#include "../CoupModelView.h"
#include "./validationvariables.h"
#include "../BasicExcel\BasicExcel.hpp"
using namespace YExcel;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// ValidationVariables message handlers
/////////////////////////////////////////////////////////////////////////////
// CValidationVariables dialog


CValidationVariables::CValidationVariables(CCoupModelDoc* pDoc, int ItemSelected, CWnd* pParent /*=nullptr*/)
	: CDialog(CValidationVariables::IDD, pParent)
	, m_SelectedNum(0)
	, m_Accumulate(FALSE)
{
	m_pDoc=pDoc;
	/*m_pFile=m_pDoc->ValidationFilePointer();
	m_NumSelected=m_pFile->GetNumberOfSelectedVal();

	m_pFile->SetNumberOfSelectedVal(m_NumSelected);*/
	m_NumSelected=0;
	for(int i=0;i<MAXSIMVAL;i++) {
		m_pFile=m_pDoc->m_Doc.ValidationFilePointer(i+1);
		m_NumSelected+=m_pFile->GetNumberOfSelectedVal();
	}




	m_ItemSelected=ItemSelected;
   	//{{AFX_DATA_INIT(CValidationVariables)
//	m_ValVarList = 0;
//	m_SimVarList = 0;
	m_SimHeading = "Select Simulated Variables by click below";
	m_ValHeading = "Select Measured Variables by click below";

	m_FileName = "";


	m_NewLists=TRUE;
	m_AbsError=0.1f;
	m_RelError=0.15f;
	m_LogTrans=FALSE;

	//}}AFX_DATA_INIT
	m_Init=TRUE;
	m_gridPicker.Init(this, (CCoupModelDoc*)pDoc,TRUE, &m_gridPicker_Sel, FALSE); 
	m_gridRealPGTime.Init(this, (CCoupModelDoc*)pDoc, FALSE, &m_gridRealPGTime_Sel, FALSE); 

	m_gridPicker_Sel.Init(this, (CCoupModelDoc*)pDoc,TRUE,&m_gridPicker , TRUE); 
	m_gridRealPGTime_Sel.Init(this, (CCoupModelDoc*)pDoc, FALSE, &m_gridRealPGTime, TRUE); 

   	m_NumberOfSelectedVariables=FUtil::STD_ItoAscii(m_NumSelected);
	m_FileName=m_pFile->GetStrValue().c_str();
	
	m_ReadySim=FALSE;
	m_ReadyObs=FALSE;
  
}


void CValidationVariables::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CValidationVariables)
	DDX_Text(pDX, IDC_STATIC_Simulated,CString(m_SimHeading.c_str()));
	DDX_Text(pDX, IDC_STATIC_Validation, CString(m_ValHeading.c_str()));
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CValidationVariables, CDialog)
	//{{AFX_MSG_MAP(CValidationVariables)
	//}}AFX_MSG_MAP

	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDD_ADD, OnBnClickedAdd)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CValidationVariables message handlers



BOOL CValidationVariables::OnInitDialog() 
{
	CDialog::OnInitDialog();


	if(m_ReadySim&&m_ReadyObs){
			GetDlgItem(IDD_ADD)->ModifyStyle(WS_DISABLED,0);
			GetDlgItem(IDCANCEL)->ModifyStyle(WS_DISABLED,0);
	}
	else {
			GetDlgItem(IDD_ADD)->ModifyStyle(0,WS_DISABLED);
			GetDlgItem(IDCANCEL)->ModifyStyle(0,WS_DISABLED);
	}
	GetDlgItem(IDD_ADD)->RedrawWindow();
	GetDlgItem(IDCANCEL)->RedrawWindow();

	m_WindowText="Combination of Variables from PG-Bin files with Simulated Variables";
	m_WindowText += ", (";
	m_WindowText+= MFC_Util::ItoAscii(m_ItemSelected) + _T(") are selected");
	SetWindowText(m_WindowText);



	if(m_Init) {

	
		m_gridPicker.AttachGrid(this, IDC_GRID3);
		m_gridRealPGTime.AttachGrid(this, IDC_GRID4);
		m_gridPicker_Sel.AttachGrid(this, IDC_GRID5);
		m_gridRealPGTime_Sel.AttachGrid(this, IDC_GRID6);
		m_Init=FALSE;

	}



	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

INT_PTR CValidationVariables::DoModal() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DoModal();
}

void CValidationVariables::OnApply() 
{	
	    CString Name;
		auto pos1=m_SelName.find("(");
		auto pos2=m_SelName.find(")");
		if(pos2>pos1) {
			string t=m_SelName.substr(pos1+1,pos2-pos1-1);
			m_SelIndex=FUtil::AtoInt(t)-1;
			if(m_SelIndex>=0)
				Name=m_SelName.substr(0,pos1).c_str();
			else {
				Name = m_SelName.c_str();
				m_SelIndex = -1;
			}

		}
		else {
			Name=m_SelName.c_str();
			m_SelIndex=-1;
		}


	    m_pDoc->m_Doc.m_ValidationData.AddNewValList(m_ItemSelected,m_FileNo, m_ValVarList,m_SelTypeInt,string(m_SelGroup), CT2A(Name).m_psz, m_SelIndex,m_RelError, m_AbsError, m_Accumulate, m_LogTrans);
	    m_ValVarList=0;
		m_SelType="";
		m_SelGroup="";
		m_SelName="";
		m_ReadySim=FALSE;
		m_ReadyObs=FALSE;
		m_gridPicker.OnSetup();
		m_gridRealPGTime.OnSetup();
		m_gridPicker_Sel.OnSetup();
		m_gridRealPGTime_Sel.OnSetup();
		
			
				  
		CString  Group, Type;
		Type="Tables";
		Group="Technical";
		Name="Scaling of Observed Variables";
		SimB *Sw;
		Sw=m_pDoc->GetPtr(Name, Type, Group);
		m_NumSelected=0;
		for(int i=0;i<MAXSIMVAL;i++) {
			m_pFile=m_pDoc->m_Doc.ValidationFilePointer(i+1);
			m_NumSelected+=m_pFile->GetNumberOfSelectedVal();
		}
	}

void CValidationVariables::OnOK() 
{
	if(m_ReadySim&&m_ReadyObs)
		OnApply();

	CDialog::OnOK();
}




void CValidationVariables::OnBnClickedReadfromexcel()
{
	BasicExcel e;
	FILEDLG out = MFC_Util::GetFileDialog("xls", "*.xls", "Excel file (*.xls)|*.xls||");
	if (out.opengood != true) return;
	e.Load(out.filename.c_str());	
	

BasicExcelWorksheet* sheet1 = e.GetWorksheet("Validation");
if (sheet1)
	{
		size_t maxRows = sheet1->GetTotalRows();
		size_t maxCols = sheet1->GetTotalCols();

		BasicExcelCell* cell = sheet1->Cell(0,0);
		string test;

	
	//	Checke Headings
		for(size_t cc=0;cc<maxCols;++cc) {
			cell=sheet1->Cell(0,cc);
			test=cell->GetString();
		}

			cell=sheet1->Cell(0,1);
			test=cell->GetString();
// test=fileName
		//OnReset();

	m_FileName=test;
	m_pFile->SetValue(test);
	m_pFile->Apply();
	m_FileExist = TRUE;
	m_pFile->SetEnable(true);




		// Variables to be defined for each line
		int Num,NFileIndex, N_Type, N_LocalIndex;
		NFileIndex = 1;
		N_Type = 0;
		string NGroup, NParName, m_Type, VALX;
		float REL_error, ABS_error;
		m_NumSelected=0;
		REL_error = 0;
		for (size_t r=2; r<maxRows; ++r)
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
						if(c==5)
							REL_error=float(cell->GetDouble());
						else if(c==6)
							ABS_error=float(cell->GetDouble());
						break;
					case BasicExcelCell::STRING:
						if(c==3)
							NGroup=cell->GetString();
						else if(c==5)
							NParName=cell->GetString();
						else if(c==6) {
							VALX=cell->GetString();
							REL_error=FUtil::AtoFloat(VALX);}
						else if(c==7) {
							VALX=cell->GetString();
							ABS_error=FUtil::AtoFloat(VALX);}
						break;
					case BasicExcelCell::WSTRING:
						printf("%10ws", cell->GetWString());
						break;
				}
			}
		// Apply the line as new input to model
		m_Type="Parameters";
	
		string xx,str1, str2;


		
	auto pos1=NParName.find("(");
	auto pos2=NParName.find(")");
	if(pos2>pos1) {
			string t=NParName.substr(pos1+1,pos2-pos1-1);
			N_LocalIndex=FUtil::AtoInt(t)-1;
			NParName=NParName.substr(0,pos1);
		}
		else {
			N_LocalIndex=-1;
		}



	   m_pDoc->m_Doc.m_ValidationData.AddValList(m_NumSelected, NFileIndex-1 ,N_Type,NGroup, NParName, N_LocalIndex,REL_error , ABS_error);

	   m_NumSelected++;
	   m_NumberOfSelectedVariables=FUtil::STD_ItoAscii(m_NumSelected);
	   m_pFile->SetNumberOfSelectedVal(m_NumSelected);

		
		}
	}
	OnInitDialog();
}



void CValidationVariables::OnBnClickedSavetoexcel()
{
	BasicExcel e;
	CString test;
	int icount=0;
	FILEDLG File_Res = MFC_Util::GetFileDialog("xls", "*.xls", "Excel file (*.xls)|*.xls||");
	if(File_Res.opengood!=true)
			return;
// Create a new workbook with 1 sheet
	e.New(1);
	e.RenameWorksheet("Sheet1", "Validation");
	BasicExcelWorksheet* sheet = e.GetWorksheet("Validation");

	if (sheet)
	{
		
		sheet->Cell(0,0)->SetString("Validation File Name:");	
		sheet->Cell(0,1)->SetString(m_FileName.c_str());	
		sheet->Cell(1,0)->SetString("Num");	
		sheet->Cell(1,1)->SetString("Validation Variable #");	
		sheet->Cell(1,2)->SetString("Validation Variable Name");	
		sheet->Cell(1,3)->SetString("Sim Group");
		sheet->Cell(1,4)->SetString("Sim Type");
		sheet->Cell(1,5)->SetString("Sim Name");	
		sheet->Cell(1,6)->SetString("Relative Error");	
		sheet->Cell(1,7)->SetString("Absolute Error");	

		for(int i=0;i<m_NumSelected;i++){
			string sval;
			sheet->Cell(i+2,0)->SetInteger(i+1);
			int ind=m_pDoc->m_Doc.m_ValidationData.GetValFileIndex(i);
			sheet->Cell(i+2,1)->SetInteger(ind+1);
			sval=m_pFile->GetDataDescription(size_t(ind+1));
			sheet->Cell(i+2,2)->SetString(sval.c_str());
			sval=m_pDoc->m_Doc.m_ValidationData.GetValGroup(i).c_str();
			sheet->Cell(i+2,3)->SetString(sval.c_str());
			int itype;
			itype=m_pDoc->m_Doc.m_ValidationData.GetValNameType(i);
			sheet->Cell(i+2,4)->SetInteger(itype);
			sval=m_pDoc->m_Doc.m_ValidationData.GetValName(i).c_str();
			int nind=m_pDoc->m_Doc.m_ValidationData.GetValLocalIndex(i);
			if(nind>=0) {
				sval+="("+FUtil::STD_ItoAscii(nind+1)+")";
			}
			sheet->Cell(i+2,5)->SetString(sval.c_str());
			float fv=m_pDoc->m_Doc.m_ValidationData.GetValP_Error(i);
			if(fv>=0)
				sval=FUtil::STD_FtoAscii(fv);
			else
				sval="-";
			sheet->Cell(i+2,6)->SetString(sval.c_str());
			fv=m_pDoc->m_Doc.m_ValidationData.GetValA_Error(i);
			if(fv>=0) 
				sval=FUtil::STD_FtoAscii(fv);
			else
				sval="-";
			sheet->Cell(i+2,7)->SetString(sval.c_str());
	}
	
	MFC_Util::MessageBox("The Data are now written to the excel file.","File Information",0);
		
	e.SaveAs(File_Res.filename.c_str());

}
}


void CValidationVariables::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	//OnCancel();
	m_ValVarList=0;
	m_ReadySim=FALSE;
	m_ReadyObs=FALSE;
	m_gridPicker.OnSetup();
	m_gridRealPGTime.OnSetup();
	m_gridPicker_Sel.OnSetup();
	m_gridRealPGTime_Sel.OnSetup();
	OnInitDialog();
}

void CValidationVariables::OnBnClickedAdd()

{

	OnApply();
	

	m_makeApply=FALSE;
	m_ItemSelected++;
	OnInitDialog();
	// TODO: Add your control notification handler code here
}


