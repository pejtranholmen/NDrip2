// ChaParDuringRun.cpp : implementation file
//


#include "../Util/FUtil.hpp"

#include "../ModelTypes/NewModelType.h"
#include "../NewBase/Doc.h"
#include "./ChaParDuringRun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChaParDuringRun dialog

CChaParDuringRun::CChaParDuringRun(SIMB Par,Doc* pDoc,CWnd* pParent /*=nullptr*/)
	: CDialog(CChaParDuringRun::IDD, pParent)
{
	m_Par=Par;
	if(Par.pBase->Is_P()) {
		m_pPs=(Ps*)Par.pBase;
	}
	else if(Par.pBase->Is_PTable()){
		m_pP=(P*)Par.pBase;
	}
	else if(Par.pBase->Is_DB()) {
		m_pDB=(CDB*)Par.pBase;

	}
	m_pDoc=pDoc;
	m_TabIndex=-1;
	m_Type=Par.pBase->GetSim();

	m_Datum=PGUtil::StringDatum(m_pDoc->RunOpt.longMinStart).c_str();
	m_SimStart=PGUtil::StringDatum(m_pDoc->RunOpt.longMinStart).c_str();
	m_SimEnd=PGUtil::StringDatum(m_pDoc->RunOpt.longMinEnd).c_str();
	m_year	=m_Datum.Mid(0,4);
	m_month=m_Datum.Mid(5,2);
	m_day	=m_Datum.Mid(8,2);
	m_hour	=m_Datum.Mid(11,2);
	m_min=m_Datum.Mid(14,2);
	m_NewValue=float(Par.pBase->GetValue());
	m_strNewValue=MFC_Util::FtoAscii(m_NewValue);
	if(Par.pBase->Is_P()) {
		m_Pmin=float(m_pPs->GetMinValue());
		m_Pmax=float(m_pPs->GetMaxValue());
	}
	else if (Par.pBase->Is_DB()) {
		m_Pmin = 0.;

	}
	else {
		m_Pmin=float(m_pP->GetMinValue(m_Par.TabIndex));
		m_Pmax=float(m_pP->GetMaxValue(m_Par.TabIndex));
	}

	if(m_Pmax<=-1.E38) m_Pmax=fabs(m_Pmax);
	
 	//{{AFX_DATA_INIT(CChaParDuringRun)
	m_Selected = -1;
	//}}AFX_DATA_INIT
}








void CChaParDuringRun::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChaParDuringRun)
	DDX_Text(pDX, IDC_EDIT_StartYear, m_year);
	DDV_MaxChars(pDX, m_year, 4);
	DDX_Text(pDX, IDC_EDIT_StartMonth, m_month);
	DDV_MaxChars(pDX, m_month, 2);
	DDX_Text(pDX, IDC_EDIT_StartMinute, m_min);
	DDV_MaxChars(pDX, m_min, 2);
	DDX_Text(pDX, IDC_EDIT_StartHour, m_hour);
	DDV_MaxChars(pDX, m_hour, 2);
	DDX_Text(pDX, IDC_EDIT_StartDay, m_day);
	DDV_MaxChars(pDX, m_day, 2);
	DDX_LBIndex(pDX, IDC_LIST1, m_Selected);
	DDX_Text(pDX, IDC_STATIC_Start, m_SimStart);
	DDX_Text(pDX, IDC_STATIC_End, m_SimEnd);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_strNewValue);
	DDV_MaxChars(pDX, m_strNewValue, 70);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChaParDuringRun, CDialog)
	//{{AFX_MSG_MAP(CChaParDuringRun)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_RESET, OnButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_SELECT, OnButtonSelect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChaParDuringRun message handlers

void CChaParDuringRun::OnButtonAdd() 
{
	UpdateData(TRUE);
	m_Datum=m_year+"-"+m_month+"-"+m_day+	
		  " "+m_hour+":"+m_min;
	string sd;
	sd=CT2A(m_Datum).m_psz;
	if(PGUtil::MinutConv(sd)>=m_pDoc->RunOpt.longMinStart&&
	   PGUtil::MinutConv(sd)<=m_pDoc->RunOpt.longMinEnd){
		   if(m_Par.pBase->Is_DB()) {
			m_pDB->SetChaDateAndValues(PGUtil::MinutConv(sd), CT2A(m_strNewValue).m_psz, CT2A(m_SelectedKey).m_psz);
		}
		else {
			m_NewValue=MFC_Util::AtoFloat(m_strNewValue);
			if(!m_Par.pBase->Is_PTable()) 
				m_pPs->SetChaDateAndValue(PGUtil::MinutConv(sd), m_NewValue);
			else
				m_pP->SetChaDateAndValue(PGUtil::MinutConv(sd),m_Par.TabIndex, m_NewValue);
		}
	}
	OnInitDialog();
	
	// TODO: Add your control notification handler code here
	

}

void CChaParDuringRun::OnButtonDelete() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_Par.pBase->Is_DB())
		m_pDB->DeleteChaDate(size_t(m_Selected));
	else if(!m_Par.pBase->Is_PTable())
		m_pPs->DeleteChaDate(size_t(m_Selected));
	else
		m_pP->DeleteChaDate(m_Par.TabIndex, m_Selected);

	OnInitDialog();
	
}

void CChaParDuringRun::OnButtonReset() 
{
	// TODO: Add your control notification handler code here
	
	if(m_Par.pBase->Is_DB())
		m_pDB->ResetChaDates();
	else if(!m_Par.pBase->Is_PTable())
		m_pPs->ResetChaDates();
	else 
		m_pP->ResetChaDates(m_Par.TabIndex);
	OnInitDialog();
}

BOOL CChaParDuringRun::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString Text;
	CListBox* pList;
	int NumSelected;

// List of Dates
	NumSelected = 0;
	switch (m_Type) {
		case PAR_SINGLE:
	 		Text="ChangeParameterValues : ";
			Text+=m_Par.pBase->GetName().c_str();
			Text+=+" (";
			Text+=m_Par.pBase->GetUnit().c_str();
			Text+=")";
			NumSelected=m_pPs->GetNumChaDates();
			break;
		case PAR_TABLE:
	 		Text="ChangeParameterValues : ";
			Text+=m_Par.pBase->GetName().c_str();
			Text+=+" (";
			Text+=m_Par.pBase->GetUnit().c_str();
			Text+=")";
			Text+=_T(", index=")+MFC_Util::ItoAscii(m_Par.TabIndex+1);
			NumSelected=m_pP->GetNumChaDates(m_Par.TabIndex);
			break;
		case DB:
	 		Text="ChangeParameterValues : ";
			Text+=m_pDB->GetName().c_str();
			NumSelected=m_pDB->GetNumChaDates();
	}
	SetWindowText(Text);

	pList=(CListBox*)GetDlgItem(IDC_LIST1);
	if(pList->GetCount()>0) pList->ResetContent();
	int count=0;
	for(int i=0;i<NumSelected;i++) {
		long num;
		if(m_Type==DB)
			num=m_pDB->GetChaDate(i);
		else if(m_Type==PAR_SINGLE) 
			num=m_pPs->GetChaDate(i);
		else
			num=m_pP->GetChaDate(m_Par.TabIndex, i);

		if(num>0) {
			Text=PGUtil::StringDatum(num).c_str();
			count++;
			if(m_Par.pBase->Is_PTable()) {
				int indvalue=m_Par.TabIndex;
				Text += MFC_Util::AddIndexString(indvalue + 1);
			}
		}
		else Text="    ";
		Text+="  - ";
		if(m_Type==DB) {
			Text+=m_pDB->GetChaParValue(i).c_str();
		}
		else if(m_Type==PAR_SINGLE)
			Text+=MFC_Util::DtoAscii(m_pPs->GetChaParValue(i));
		else {
			Text+=MFC_Util::DtoAscii(m_pP->GetChaParValue(m_Par.TabIndex, i));
		}
		pList->AddString(Text);
	}
	if(m_Type==DB) {
		GetDlgItem(IDC_BUTTON_SELECT)->ModifyStyle(WS_DISABLED,0);
		if(m_strNewValue.GetLength()<=0)
			GetDlgItem(IDC_BUTTON_ADD)->ModifyStyle(0,WS_DISABLED);
		else
			GetDlgItem(IDC_BUTTON_ADD)->ModifyStyle(WS_DISABLED,0);
	}
	else {
		GetDlgItem(IDC_BUTTON_SELECT)->ModifyStyle(0,WS_DISABLED);
	}

	GetDlgItem(IDC_BUTTON_SELECT)->RedrawWindow();
	GetDlgItem(IDC_BUTTON_ADD)->RedrawWindow();
   	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChaParDuringRun::OnOK() 
{
	if(m_Type==DB) {
		if(m_pDB->GetNumChaDates()>0) 
			m_pDoc->Set_P_CH(m_pDB, -1);
		else
			m_pDoc->Reset_P_CH(m_Par.pBase);
	}
	else if(m_Type==PAR_SINGLE) {
		if(((Ps*)m_Par.pBase)->GetNumChaDates()>0)
			m_pDoc->Set_P_CH(m_Par.pBase,0);
		else
			m_pDoc->Reset_P_CH((Ps*)m_Par.pBase);

	}
	else {
		if(((P*)m_Par.pBase)->GetNumChaDates(m_Par.TabIndex)>0)
			m_pDoc->Set_P_CH(m_Par.pBase,1, m_Par.TabIndex);
		else
			m_pDoc->Reset_P_CH(m_Par.pBase);
	}
	CDialog::OnOK();
}

void CChaParDuringRun::OnButtonSelect() 
{

	
}
