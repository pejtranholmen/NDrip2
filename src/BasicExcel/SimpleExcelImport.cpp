// SimpleExcelImport.cpp : implementation file
//

// SimpleExcelImport.cpp : implementation file
//

#include "..\stdafx.h"
#include "..\CUG\MyCug.h"
#include "..\CoupModel.h"

#include "..\CoupModelDoc.h"
#include "..\CoupModelView.h"

#include "..\MultiRun\MultiRunTable.h"
#include "..\Model\SimB.h"

#include ".\simpleexcelimport.h"


// SimpleExcelImport dialog

IMPLEMENT_DYNAMIC(SimpleExcelImport, CDialog)
SimpleExcelImport::SimpleExcelImport(CCoupModelDoc* pDoc, NewMap *pfmap,CWnd* pParent /*=NULL*/)
	: CDialog(SimpleExcelImport::IDD, pParent)
{
	m_pDoc=pDoc;
	m_pfmap=pfmap;
	m_FileRead=FALSE;
	m_ReRead=FALSE;
	m_ApplyAll=FALSE;
	OnBnReadFile();
}

SimpleExcelImport::~SimpleExcelImport()
{
}

void SimpleExcelImport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO3, m_WorkSheet);
	DDX_Control(pDX, IDC_LIST1, m_ParList);
	DDX_Control(pDX, IDC_EDIT1, m_pSelectedFile);
	DDX_Control(pDX, IDC_EDIT2, m_pSwitches);
	DDX_Control(pDX, IDC_EDIT3, m_pParameters);
	DDX_Control(pDX, IDC_EDIT4, m_pParTables);
}


BEGIN_MESSAGE_MAP(SimpleExcelImport, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDOK4, OnApplySelected)
	ON_BN_CLICKED(IDOK2, OnBnReadFile)

	ON_CBN_SELCHANGE(IDC_COMBO3, OnCbnSelchangeWorkSheet)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, OnLvnColumnclickList1)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_LIST1, OnLvnItemActivateList1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnLvnItemchangedList1)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST1, OnLvnBegindragList1)
	ON_BN_CLICKED(IDOK5, OnBnApplyAllItems)
END_MESSAGE_MAP()


// SimpleExcelImport message handlers

void SimpleExcelImport::OnBnClickedOk()
{
	OnOK();
}

void SimpleExcelImport::OnApplySelected()
{
	// TODO: Add your control notification handler code here
	CString Group, Name,SwValue;
	int Index;
	float value;

	nParApplied=0;
	nParTableApplied=0;
	nSwitches=0;

	int nindex=m_WorkSheet.GetCurSel();

	BasicExcelWorksheet* sheet2=e.GetWorksheet(nindex);
	int maxRows = sheet2->GetTotalRows();
	BasicExcelCell* cell;
	POSITION pos;
	if(m_ApplyAll)
		pos=0;
	else
		pos=m_ParList.GetFirstSelectedItemPosition();
	int ipos=0;
	while(ipos>=0&&ipos<maxRows-2){
		if(m_ApplyAll){
			if(pos==0)
				pos=POSITION(1);
			else
				ipos++;
		}
		else
			ipos=m_ParList.GetNextSelectedItem(pos);
		
		CString test;
		cell=sheet2->Cell(0,0);
		test=cell->GetString();
		if(test.Find("Gr")==0) {
			cell=sheet2->Cell(0,1);
			test=cell->GetString();
			if(test.Find("TabName")==0) {
//		Parameter Tables
				cell=sheet2->Cell(ipos+1,0);
				Group=cell->GetString();
				cell=sheet2->Cell(ipos+1,2);
				Name=cell->GetString();
				cell=sheet2->Cell(ipos+1,3);
				value=float(cell->GetDouble());
				Name+="("+FtoAscii(value)+")";
				cell=sheet2->Cell(ipos+1,4);
				value=float(cell->GetDouble());
			}
			else {
//      Parameter values
				cell=sheet2->Cell(ipos+1,0);
				Group=cell->GetString();
				cell=sheet2->Cell(ipos+1,1);
				Name=cell->GetString();
				cell=sheet2->Cell(ipos+1,2);		
				if(cell->Type()==BasicExcelCell::STRING)
					SwValue=cell->GetString();
				else {
					test="Parameters";
					value=float(cell->GetDouble());
				}
			}
		}
		else {
			cell=sheet2->Cell(ipos+1,0);
			Group=cell->GetString();
			cell=sheet2->Cell(ipos+1,1);
			Name=cell->GetString();
			cell=sheet2->Cell(ipos+1,2);
			value=float(cell->GetDouble());
		}

		P* pParT;
		Ps* pPar;	
		SimB* pParB;
		CString m_Type="Parameters";
		int pos1, pos2;
		
		pos1 =Name.Find("(");
		pos2 =Name.Find(")");
		if(pos2>pos1) { //Table
			CString t=Name.Mid(pos1+1,pos2-pos1-1);
			Index=AtoInt(t);
			Name=Name.Mid(0,pos1);
			m_Type="Table Parameters";
			pParB=m_pDoc->GetPtr(Name, m_Type, Group);
			pParT=(P*)pParB;
			if(pParT!=NULL) {
				pParT->SetValue(Index-1,value);
//				pParT->Apply(Index-1);
				nParTableApplied++;
			}
				}
		else { 
			if(test=="Parameters") {
			// Single Par
				pParB=m_pDoc->GetPtr(Name, m_Type, Group);
				pPar=(Ps*)pParB;
				if(pPar!=NULL) {
					pPar->SetValue(value);
					//pPar->Apply();
					nParApplied++;
				}
			}
			else {
			// Switches
				m_Type="Switches";
				Sw* pSw;
				pParB=m_pDoc->GetPtr(Name, m_Type, Group);
				pSw=(Sw*)pParB;
				if(pSw!=NULL) {
					pSw->SetStrValue(string(SwValue));
						//pSw->Apply();
						nSwitches++;

				}
			}
	}
	}
	//m_ParList.
	m_pParameters.SetWindowText(ItoAscii(nParApplied));
	m_pParTables.SetWindowText(ItoAscii(nParTableApplied));
	m_pSwitches.SetWindowText(ItoAscii(nSwitches));
	GetDlgItem(IDOK4)->ModifyStyle(0,WS_DISABLED);
	GetDlgItem(IDOK4)->RedrawWindow();


	UpdateData(TRUE);
	//OnInitDialog();
}

void SimpleExcelImport::OnBnReadFile()
{
	int fileDlgResult;
	CString pszFileName ;
	LPCTSTR lpszDefExt="xls";
	LPCTSTR lpszFilter;
	CString Group, Name, TabName;
		static char BASED_CODE szFilter[] = "Excel file (*.xls)|*.xls||";
		CFileDialog	ParFileDlg(TRUE,lpszDefExt="xls",lpszFilter = "*.xls",OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,szFilter);
	//Show the dialog
		fileDlgResult=ParFileDlg.DoModal();
	//If the cancel button has been pushed then return
		if (fileDlgResult==IDCANCEL)
			return;
	//Retrieve the filename of the file that is to be opened
		pszFileName = ParFileDlg.GetPathName();


		  CString message;
		  message = "The file will be read and investigated (Big files may require long time to read).";
		  MessageBox(message);

	// Load a workbook with one sheet, display its contents and save into another file.
	//cout << "Start" << endl;
	e.Load(pszFileName);
//	
	m_FileName=pszFileName;
	//m_FileName=pszFileName;
	size_t maxSheets = e.GetTotalWorkSheets();
	
	NumSheets=maxSheets;
	if(maxSheets>0) 
		m_FileRead=TRUE;
	else
		m_FileRead=FALSE;

	if(m_ReRead)
		OnInitDialog();
	else
		m_ReRead=TRUE;

	// TODO: Add your control notification handler code here
}

BOOL SimpleExcelImport::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString koll2;
	int index=0;
	m_WorkSheet.ResetContent();
	BasicExcelWorksheet* sheet=e.GetWorksheet(index);
	if(sheet) {
	while(index<NumSheets) {
		sheet=e.GetWorksheet(index);
		koll2=sheet->GetAnsiSheetName();
		m_WorkSheet.AddString(koll2);
		//koll2=koll;
		index++;
	}

	m_WorkSheet.SetCurSel(0);
	}
	if(NumSheets>0)
		OnReadSelectedSheet();

	m_pSelectedFile.SetWindowText(m_FileName);

	m_pParameters.SetWindowText(ItoAscii(nParApplied));
	m_pParTables.SetWindowText(ItoAscii(nParTableApplied));
	m_pSwitches.SetWindowText(ItoAscii(nSwitches));
	
	UpdateData(TRUE);
		//AddString(m_FileName);
	//	SetDlgItemText(IDC_EDIT1,m_FileName);

	
	GetDlgItem(IDOK4)->ModifyStyle(WS_DISABLED,0);

	GetDlgItem(IDOK4)->RedrawWindow();

	return TRUE;



}
void SimpleExcelImport::OnReadSelectedSheet()
{
	// TODO: Add your control notification handler code here
	CString Group, Name, TabName, SwValue;
	int		Index;
	float value;
	CString sheetname;


	nSwitches=0;
	nParApplied=0;
	nParTableApplied=0;

	
	int nindex=m_WorkSheet.GetCurSel();
	BasicExcelWorksheet* sheet2=e.GetWorksheet(nindex);
	ListView_SetExtendedListViewStyleEx(m_ParList.GetSafeHwnd(), LVS_EX_FULLROWSELECT , LVS_EX_FULLROWSELECT);
	CRect rect;

	if(sheet2) { // Reading Parameters
		BasicExcelCell* cell;
		float v;
		int maxRows = sheet2->GetTotalRows();
		int maxCols = sheet2->GetTotalCols();
		m_ParList.DeleteAllItems();
		m_ParList.GetClientRect(&rect);
		int nInterval = rect.Width()/maxCols;
		for(int i=0;i<maxCols;i++) {
			cell=sheet2->Cell(0,i);
			m_ParList.InsertColumn(0, cell->GetString(), LVCFMT_LEFT, int(nInterval));
		}	
	   	int nColumnCount=m_ParList.GetHeaderCtrl()->GetItemCount();
	  
// Delete all of the columns.
	for (int i=0;i < nColumnCount;i++)
	{
		m_ParList.DeleteColumn(0);
	}

//		 Insert new header
		m_ParList.GetClientRect(&rect);

		for(int i=0;i<maxCols;i++) {
			cell=sheet2->Cell(0,i);
			CString test;
					switch (cell->Type())
					{
					case BasicExcelCell::UNDEFINED:
						break;
					case BasicExcelCell::INT:
						test=ItoAscii(cell->GetInteger());
						break;
					case BasicExcelCell::DOUBLE:
						v=float(cell->GetDouble());
						test=FtoAscii(v);
						break;
					case BasicExcelCell::STRING:
						test=cell->GetString();
						break;
					case BasicExcelCell::WSTRING:
						wprintf(L"%10s", cell->GetWString());
						break;
					}
					//_stprintf(szItem,_T(test) );
				m_ParList.InsertColumn(i, test, LVCFMT_LEFT, int(nInterval));
		}
		
		
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
		for(int i=0;i<maxRows-1;i++){
				lvi.iItem = i;
				lvi.iSubItem = 0;
				cell=sheet2->Cell(i+1,0);

					CString test;
					switch (cell->Type())
					{
					case BasicExcelCell::UNDEFINED:
						break;
					case BasicExcelCell::INT:
						test=ItoAscii(cell->GetInteger());
						break;
					case BasicExcelCell::DOUBLE:
						v=float(cell->GetDouble());
						test=FtoAscii(v);
						break;
					case BasicExcelCell::STRING:
						test=cell->GetString();
						break;
					case BasicExcelCell::WSTRING:
						wprintf(L"%10s", cell->GetWString());
						break;
					}
				_stprintf(szItem,_T(test));
				lvi.pszText = (LPTSTR)(LPCTSTR)szItem;
				m_ParList.InsertItem(&lvi);
				for(int ifn=1; ifn<maxCols;ifn++) {
		// SubItem 1
					cell=sheet2->Cell(i+1,ifn);
					lvi.iSubItem = ifn;
					CString test;
					switch (cell->Type())
					{
					case BasicExcelCell::UNDEFINED:
						break;
					case BasicExcelCell::INT:
						test=ItoAscii(cell->GetInteger());
						break;
					case BasicExcelCell::DOUBLE:
						v=float(cell->GetDouble());
						test=FtoAscii(v);
						break;
					case BasicExcelCell::STRING:
						test=cell->GetString();
						break;
					case BasicExcelCell::WSTRING:
						wprintf(L"%10s", cell->GetWString());
						break;
					}
					_stprintf(szItem,_T(test) );
					lvi.pszText = szItem;
					m_ParList.SetItem(&lvi);
				}
			}


			
		for(size_t rr=1;rr<maxRows;++rr){ 

			cell=sheet2->Cell(rr,0);
			Group=cell->GetString();
			cell=sheet2->Cell(rr,1);
			Name=cell->GetString();
			cell=sheet2->Cell(rr,2);
			value=float(cell->GetDouble());
		
		P* pParT;
		Ps* pPar;	
		SimB* pParB;
		CString m_Type="Parameters";
		int pos1, pos2;
		
		pos1 =Name.Find("(");
		pos2 =Name.Find(")");
		if(pos2>pos1) { //Table
			CString t=Name.Mid(pos1+1,pos2-pos1-1);
			Index=AtoInt(t);
			Name=Name.Mid(0,pos1);
			m_Type="Table Parameters";
			pParB=m_pDoc->GetPtr(Name, m_Type, Group);
			pParT=(P*)pParB;
		}
		else { // Single Par
			pParB=m_pDoc->GetPtr(Name, m_Type, Group);
			pPar=(Ps*)pParB;
			if(pPar!=NULL) {
				
			}

		}

	}

	
	}

}

void SimpleExcelImport::OnCbnSelchangeWorkSheet()
{
//	UpdateData(TRUE);
	OnReadSelectedSheet();
	// TODO: Add your control notification handler code here
}

void SimpleExcelImport::OnLvnColumnclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	//if(m_FileRead) 
	//	GetDlgItem(IDOK4)->ModifyStyle(WS_DISABLED,0);
	//else
	
}

void SimpleExcelImport::OnLvnItemActivateList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	


}

void SimpleExcelImport::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

GetDlgItem(IDOK4)->ModifyStyle(WS_DISABLED,0);
	GetDlgItem(IDOK4)->RedrawWindow();
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void SimpleExcelImport::OnLvnBegindragList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void SimpleExcelImport::OnBnApplyAllItems()
{
	m_ApplyAll=TRUE;
	OnApplySelected();
	m_ApplyAll=FALSE;

	// TODO: Add your control notification handler code here
}
