// DB_Plant_F.cpp : implementation file
//

#include "stdafx.h"
#include "CoupModel.h"
#include "DB_Plant_F.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DB_Plant_F property page

IMPLEMENT_DYNCREATE(DB_Plant_F, CPropertyPage)

DB_Plant_F::DB_Plant_F() : CPropertyPage(DB_Plant_F::IDD)
{
	//{{AFX_DATA_INIT(DB_Plant_F)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

DB_Plant_F::~DB_Plant_F()
{
	Cleaning();
}

void DB_Plant_F::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DB_Plant_F)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DB_Plant_F, CPropertyPage)
	//{{AFX_MSG_MAP(DB_Plant_F)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DB_Plant_F message handlers

BOOL DB_Plant_F::OnSetActive() 
{
	CPropertySheet* parent = (CPropertySheet*)GetParent();
	parent->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	CRect			koord(10,0,210,20); 
	CEdit			*pedit;
	CStatic			*pstatic;
	CF				*pCF;

	int				Counter;

	for(int i=0;i<statarr.GetSize();i++) 
		delete statarr.GetAt(i);
	statarr.RemoveAll();
	for(i=0;i<editarr.GetSize();i++) 
		delete editarr.GetAt(i);
	editarr.RemoveAll();

// Static Box	
	Counter=0;
	for (i=0;i<m_pCFArray.GetSize();i++) {
		pCF=(CF*)m_pCFArray.GetAt(i);
		if(pCF!=NULL){
			if(!pCF->IsDB_Disabled()&&m_pDoc->m_UserLevel<=pCF->GetUserLevel()){
				CRect coord=koord;
				coord.left+=-5;
				coord.right+=-30;
				coord.top+=Counter*25;
				coord.bottom+=Counter*25;
				CString kk=pCF->GetName();
				statarr.Add(new CStatic);
				pstatic=statarr[Counter];
				pstatic->Create(kk, WS_CHILD | WS_VISIBLE ,coord,this,5000);
				pstatic->SetFont(GetFont());
				Counter++;
			}
		}
	}
//  CEdit Field
	Counter=0;
	for (i=0;i<m_pCFArray.GetSize();i++) {
		pCF=(CF*)m_pCFArray.GetAt(i);
		if(pCF!=NULL){
			if(!pCF->IsDB_Disabled()&&m_pDoc->m_UserLevel<=pCF->GetUserLevel()){
				CRect coord=koord;
				coord.left+=120;
				coord.right+=200;
				coord.top+=Counter*25;
				coord.bottom+=Counter*25;
				editarr.Add(new CEdit);
				pedit=editarr[Counter];
				pedit->Create(ES_LEFT | WS_CHILD | WS_VISIBLE | WS_BORDER |
							WS_TABSTOP | ES_AUTOHSCROLL,coord,this,4000+Counter);

				pedit->ModifyStyleEx(0,WS_EX_CLIENTEDGE);
				pedit->SetFont(GetFont());
				pedit->SetWindowText(pCF->GetDBValue());
				Counter++;
			}
		}
	}

	if(Counter==0) {
		return CPropertyPage::OnWizardNext();
	}
	
	return CPropertyPage::OnSetActive();
}

LRESULT DB_Plant_F::OnWizardNext() 
{
	// TODO: Add your specialized code here and/or call the base class
    int Counter=0;
	CEdit *pedit;
	CString text;
	for (int i=0;i<m_pCFArray.GetSize();i++) {
		CF *pCF=(CF*)m_pCFArray.GetAt(i);
		if(pCF!=NULL){
			if(!pCF->IsDB_Disabled()&&m_pDoc->m_UserLevel<=pCF->GetUserLevel()){
				pedit=editarr[Counter];
				pedit->GetWindowText(text);
				pCF->SetDBValue(text);
				Counter++;
			}
		}
	}
	Cleaning();
	
	return CPropertyPage::OnWizardNext();
}

BOOL DB_Plant_F::OnWizardFinish() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnWizardFinish();
}

BOOL DB_Plant_F::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void DB_Plant_F::Cleaning()
{
// Cleaning of array with pointers
	for(int i=0;i<editarr.GetSize();i++)
		delete editarr.GetAt(i);
	editarr.RemoveAll();
	for(i=0;i<statarr.GetSize();i++)
		delete statarr.GetAt(i);
	statarr.RemoveAll();

}
