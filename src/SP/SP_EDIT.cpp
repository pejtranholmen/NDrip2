// SP_EDIT.cpp : implementation file
//

#include "stdafx.h"
#include ".\CUG\MyCug.h"
#include "CoupModel.h"
#include "CoupModelDoc.h"
#include "CoupModelView.h"
#include "SP_EDIT.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" void   PLOTPF_mp_GETLAYER(int *, int *, int*);
extern "C" void   PLOTPF_mp_RETRIEVECOEF(int*);
extern "C" void   PLOTPF_mp_RETRIEVEPF_POINTS();
extern "C" void   PLOTPF_mp_RETRIEVETEXTURE();
extern "C" void   PLOTPF_mp_LOADALLLAYERS();
extern "C" void   PLOTPF_mp_DELETEPROFILE();
extern "C" void   PLOTPF_mp_RETRIEVESOILPROFILE();
extern "C" void   PLOTPF_mp_RESETPROFILE();
extern "C" void   PLOTPF_mp_LOADNEWPROFILE(int*);
extern "C" int   PLOTPF_mp_SCANDATABASE(int*);
extern "C" void   PLOTPF_mp_GETPROFILE(int*, int*);
extern "C" void   PLOTPF_mp_OPENBASE();
extern "C" void   PLOTPF_mp_SAVEDATABASE();
extern "C" void   PROFILE_mp_SOILST();
extern "C" void   HYDRAULIC_mp_HYDRAULIC_RECALC();

extern "C" float   HYDRAULIC_mp_ESATC(int *);
extern "C" float   HYDRAULIC_mp_ETETRE(int *);
extern "C" float   HYDRAULIC_mp_EPSIE(int *);
extern "C" float   HYDRAULIC_mp_ELAMBDA(int *);
extern "C" float   HYDRAULIC_mp_EWILT(int *);

extern "C" BOOL   PLOTPF_mp_ISINDATABASE();
extern "C" BOOL PLOTPF_mp_OPENDATABASE;
extern "C" BOOL PLOTPF_mp_FINDPROFILE;
extern "C" BOOL PLOTPF_mp_FINDLAYER;
extern "C" BOOL PLOTPF_mp_SAVED;
extern "C" BOOL PLOTPF_mp_COMSTART;
extern "C" BOOL PLOTPF_mp_COMSET;
extern "C" BOOL PLOTPF_mp_TOBEDOWNLOADED;
extern "C" BOOL PLOTPF_mp_SUBPP;

extern "C" int PLOTPF_mp_ACTUALPROFILE;
extern "C" int PLOTPF_mp_ACTUALREPPROFILE;
extern "C" int PLOTPF_mp_ACTUALLEVEL;
extern "C" int PLOTPF_mp_ACTUALNUMLEVELS;

extern "C" float PLOTPF_mp_MODPRESSURE[12];
extern "C" float PLOTPF_mp_MODTHETA[12];
extern "C" float PLOTPF_mp_MODTEXTURE[8];
extern "C" int PLOTPF_mp_NT;

extern "C" float PLOTPF_mp_PTEX[6];
extern "C" char  PLOTPF_mp_DKEY[6];
extern "C" char  PLOTPF_mp_RPLACE[16];
extern "C" char  PLOTPF_mp_COUNTY[6];
extern "C" char  PLOTPF_mp_CKOORD[16];
extern "C" int   PLOTPF_mp_SITEPROFNUM;
extern "C" short PLOTPF_mp_YEAR;
extern "C" float PLOTPF_mp_UNITCONV;
extern "C" float PLOTPF_mp_AWILT[22];
extern "C" float PLOTPF_mp_BUDEP[22];
extern "C" float PLOTPF_mp_BLDEP[22];
extern "C" float PLOTPF_mp_ALAMDA[22];
extern "C" float PLOTPF_mp_APSIE[22];
extern "C" float PLOTPF_mp_APORO[22];
extern "C" float PLOTPF_mp_ACLAY[22];
extern "C" float PLOTPF_mp_ASAND[22];
extern "C" float PLOTPF_mp_ATETRE[22];
extern "C" float PLOTPF_mp_BLB[22];
extern "C" float PLOTPF_mp_BUB[22];
extern "C" float PLOTPF_mp_Scale[22];
extern "C" float PLOTPF_mp_BSATC[22];
extern "C" float PLOTPF_mp_BBSAT[22];
extern "C" float PLOTPF_mp_BNVAR[22];
extern "C" float PLOTPF_mp_BNSR[22];
extern "C" float PLOTPF_mp_BNSE[22];
extern "C" float PLOTPF_mp_BGA[22];
extern "C" float PLOTPF_mp_BGN[22];
extern "C" float PLOTPF_mp_BGM[22];	   


/////////////////////////////////////////////////////////////////////////////
// SP_EDIT dialog


SP_EDIT::SP_EDIT(CCoupModelDoc *pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(SP_EDIT::IDD, pParent)
{
	m_ChangedData=TRUE;
	m_pDoc=pDoc;
	m_SaveNewNum=FALSE;
	CWinApp* pApp = AfxGetApp();
	POSITION posd=pDoc->GetFirstViewPosition();
	m_pView=(CCoupModelView*)pDoc->GetNextView(posd);


	//{{AFX_DATA_INIT(SP_EDIT)
	m_SiteName = _T("My");
	m_County = _T("C");
	m_Country = _T("SE");
	m_CoordLat = _T("12");
	m_CoordLong = _T("14");
	m_SubClay = 0;
	m_SubSand = 0;
	m_SubOrg = 0;
	m_TopClay = 0;
	m_TopSand = 0;
	m_TopOrg = 0;
	m_Prof = 1;
	m_RepProf = 1;
	m_Year = 1998;
	m_MoveCursor = TRUE;
	m_NumTens = 0;
	m_NumLayers = 1;
	//}}AFX_DATA_INIT
	if(!PLOTPF_mp_OPENDATABASE) PLOTPF_mp_OPENBASE();
	m_iniProf=PLOTPF_mp_ACTUALPROFILE;
	m_iniRepProf=PLOTPF_mp_ACTUALREPPROFILE;

	int prof=1, repNumber=1, layer=1;
	m_CurSel=0;
	PLOTPF_mp_SUBPP=TRUE;
	
	PLOTPF_mp_GETPROFILE(&prof,&repNumber);


	m_Prof=PLOTPF_mp_ACTUALPROFILE;
	m_RepProf=PLOTPF_mp_ACTUALREPPROFILE;
	m_NumTens=PLOTPF_mp_NT;
	m_NumLayers=PLOTPF_mp_ACTUALNUMLEVELS;

	while(PLOTPF_mp_ISINDATABASE()) {
		if(m_RepProf<99) m_RepProf++;
			else {
				m_Prof++;
				m_RepProf=1;
			}
		PLOTPF_mp_ACTUALPROFILE=m_Prof;
		PLOTPF_mp_ACTUALREPPROFILE=m_RepProf;
   	}


}


void SP_EDIT::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SP_EDIT)
	DDX_Control(pDX, IDC_EDIT6, m_PFcom);
	DDX_Control(pDX, IDC_LIST_SELECTION, m_Selection);
	DDX_Text(pDX, IDC_EDIT1, m_SiteName);
	DDV_MaxChars(pDX, m_SiteName, 16);
	DDX_Text(pDX, IDC_EDIT2, m_County);
	DDV_MaxChars(pDX, m_County, 2);
	DDX_Text(pDX, IDC_EDIT3, m_Country);
	DDV_MaxChars(pDX, m_Country, 2);
	DDX_Text(pDX, IDC_EDIT4, m_CoordLat);
	DDV_MaxChars(pDX, m_CoordLat, 7);
	DDX_Text(pDX, IDC_EDIT5, m_CoordLong);
	DDV_MaxChars(pDX, m_CoordLong, 7);
	DDX_Text(pDX, IDC_EDIT9, m_SubClay);
	DDV_MinMaxFloat(pDX, m_SubClay, 0, 100);
	DDX_Text(pDX, IDC_EDIT10, m_SubSand);
	DDV_MinMaxFloat(pDX, m_SubSand, 0, 100);
	DDX_Text(pDX, IDC_EDIT11, m_SubOrg);
	DDV_MinMaxFloat(pDX, m_SubOrg, 0, 100);
	DDX_Text(pDX, IDC_EDIT12, m_TopClay);
	DDV_MinMaxFloat(pDX, m_TopClay, 0, 100);
	DDX_Text(pDX, IDC_EDIT13, m_TopSand);
	DDV_MinMaxFloat(pDX, m_TopSand, 0, 100);
	DDX_Text(pDX, IDC_EDIT14, m_TopOrg);
	DDV_MinMaxFloat(pDX, m_TopOrg, 0, 100);
	DDX_Text(pDX, IDC_EDIT_PROF, m_Prof);
	DDV_MinMaxUInt(pDX, m_Prof, 1, 999);
	DDX_Text(pDX, IDC_EDIT_REP, m_RepProf);
	DDV_MinMaxUInt(pDX, m_RepProf, 1, 99);
	DDX_Text(pDX, IDC_EDIT_YEAR, m_Year);
	DDV_MinMaxUInt(pDX, m_Year, 0, 2030);
	DDX_Check(pDX, IDC_CHECK1, m_MoveCursor);
	DDX_Text(pDX, IDC_EDIT_NUM_TENS, m_NumTens);
	DDV_MinMaxInt(pDX, m_NumTens, 0, 12);
	DDX_Text(pDX, IDC_EDIT_NUM_LAYERS, m_NumLayers);
	DDV_MinMaxInt(pDX, m_NumLayers, 1, 32);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SP_EDIT, CDialog)
	//{{AFX_MSG_MAP(SP_EDIT)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_LBN_SELCHANGE(IDC_LIST_SELECTION, OnSelchangeListSelection)
	ON_EN_CHANGE(IDC_EDIT_REP, OnChangeEditRep)
	ON_EN_CHANGE(IDC_EDIT_PROF, OnChangeEditProf)
	ON_EN_KILLFOCUS(IDC_EDIT_REP, OnKillfocusEditRep)
	ON_EN_KILLFOCUS(IDC_EDIT_PROF, OnKillfocusEditProf)
	ON_BN_CLICKED(IDC_TEXTURE_EDIT, OnTextureEdit)
	ON_BN_CLICKED(IDC_RETENTION_EDIT, OnRetentionEdit)
	ON_BN_CLICKED(IDC_COEF, OnCoef)
	ON_EN_KILLFOCUS(IDC_EDIT_NUM_LAYERS, OnKillfocusEditNumLayers)
	ON_BN_CLICKED(IDC_BUTTON_VIEW, OnButtonView)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_LAYERS, OnButtonViewLayers)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_LAYERS_RET, OnButtonViewLayersRet)
	ON_BN_CLICKED(IDC_BUTTON_VIEW_RET, OnButtonViewRet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SP_EDIT message handlers
BOOL SP_EDIT::OnInitDialog() 
{

    int Record=1;
	m_Prof=10;
	m_NumTens=PLOTPF_mp_NT;
   CString string, tmpstring;	
     CDialog::OnInitDialog();
	m_PFcom.SetTabStops(41);
		CString kalle;
		kalle=m_pDoc->SP_GetComments();
		if(kalle.GetLength()>0) {
		    m_PFcom.SetWindowText(kalle);
			PLOTPF_mp_COMSET=TRUE;
		}
		else {
		    m_PFcom.SetWindowText("No details on this Profile exist");
			PLOTPF_mp_COMSET=TRUE;
		}
   UpdateData(TRUE);

//   if(!m_ChangedData) return TRUE;

   int numTab=8;
   LPINT tabstops;
   int	tabstop[8];
   tabstop[0]=100;
   tabstop[1]=170;
   tabstop[2]=220;
   tabstop[3]=244;
   tabstop[4]=270;
   tabstop[5]=296;
   tabstop[6]=320;
   tabstop[7]=344;
   tabstops=&tabstop[0];

   m_ChangedData=FALSE;
   m_Selection.SetTabStops(numTab,tabstops);
   m_Selection.ResetContent();
   while(PLOTPF_mp_SCANDATABASE(&Record)>1) {
		string="";
		for(int i=0;i<6;i++)
			string+=PLOTPF_mp_DKEY[i];
		string+=" ";
		for(int i=0;i<16;i++) {
			string+=PLOTPF_mp_RPLACE[i];
		}
		string+="\t";
		for(int i=0;i<16;i++)
			string+=PLOTPF_mp_CKOORD[i];
		string+="\t";

		for(int i=0;i<6;i++)
			string+=PLOTPF_mp_COUNTY[i];
		string+="\t";
		for(int i=0;i<6;i++) {
			tmpstring=FtoAscii(PLOTPF_mp_PTEX[i]);
			int ip=tmpstring.Find(".");
			if(ip>0) tmpstring=tmpstring.Mid(0,ip+2);
			string+=tmpstring+"\t";
		}
		if(string.Find("   :")!=0) {
			m_Selection.AddString(string);
		}
		Record++;	

   }
	if(m_CurSel>=m_Selection.GetCount()) m_CurSel=m_Selection.GetCount()-1;
	m_Selection.SetCurSel(m_CurSel);
	
	m_Selection.GetText(m_Selection.GetCurSel(),string);
	CString String2=string.Mid(0,3);
	int prof=AtoInt(String2);
	String2=string.Mid(4,2);
	int repNumber=AtoInt(String2);
	int layer=1;
	PLOTPF_mp_GETPROFILE(&prof,&repNumber);
	PLOTPF_mp_SUBPP=TRUE;


 	CString tmpplace;
	if(PLOTPF_mp_FINDPROFILE) {
	   m_Year=PLOTPF_mp_YEAR;
	   m_Prof=PLOTPF_mp_ACTUALPROFILE;
	   m_RepProf=PLOTPF_mp_ACTUALREPPROFILE;
	   m_NumLayers=PLOTPF_mp_ACTUALNUMLEVELS;
		  m_SiteName="";
	   for(int i=0;i<16;i++) 
		   m_SiteName+=PLOTPF_mp_RPLACE[i];
			tmpplace="";
	   for(int i=0;i<6;i++) 
		   tmpplace+=PLOTPF_mp_COUNTY[i];
	   tmpplace.TrimLeft();	
	   m_County=tmpplace.Mid(0,2);
	   m_Country=tmpplace.Mid(3,2);
	   if(m_Country.Compare("  ")==0) m_Country="SE";
	   m_CoordLat="";
	   for(int i=0;i<7;i++) 
		   m_CoordLat+=PLOTPF_mp_CKOORD[i];
		   m_CoordLong="";
   	   for(int i=8;i<15;i++) 
		   m_CoordLong+=PLOTPF_mp_CKOORD[i];
		m_TopClay	=PLOTPF_mp_PTEX[0];
		m_TopSand	=PLOTPF_mp_PTEX[1];
		m_TopOrg	=PLOTPF_mp_PTEX[2];
		m_SubClay	=PLOTPF_mp_PTEX[3];
		m_SubSand	=PLOTPF_mp_PTEX[4];
		m_SubOrg	=PLOTPF_mp_PTEX[5];
    }

	if(m_NumTens>0) {
		GetDlgItem(IDC_BUTTON_VIEW_RET)->ModifyStyle(WS_DISABLED,0);
		GetDlgItem(IDC_BUTTON_VIEW_LAYERS_RET)->ModifyStyle(WS_DISABLED,0);
	}
	else {
		GetDlgItem(IDC_BUTTON_VIEW_RET)->ModifyStyle(0,WS_DISABLED);
		GetDlgItem(IDC_BUTTON_VIEW_LAYERS_RET)->ModifyStyle(0,WS_DISABLED);
	}
	GetDlgItem(IDC_BUTTON_VIEW_RET)->RedrawWindow();
	GetDlgItem(IDC_BUTTON_VIEW_LAYERS_RET)->RedrawWindow();
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

}


void SP_EDIT::OnButtonApply() 
{
	UpdateData(TRUE);
	PLOTPF_mp_ACTUALPROFILE=m_Prof;
	PLOTPF_mp_ACTUALREPPROFILE=m_RepProf;
	PLOTPF_mp_ACTUALNUMLEVELS=m_NumLayers;

	PLOTPF_mp_YEAR=short(m_Year);
	while(m_SiteName.GetLength()<16)
		m_SiteName+=" ";;
	CString lower=m_SiteName;
	lower.MakeLower();
	m_SiteName=m_SiteName.Mid(0,1)+lower.Mid(1,15);
	for (int i=0;i<16;i++) {
	   PLOTPF_mp_RPLACE[i]=m_SiteName[i];
	}
	while (m_County.GetLength()<2)
		m_County+=" ";
	for(int i=0;i<2;i++) 
		PLOTPF_mp_COUNTY[i]=m_County[i];
	PLOTPF_mp_COUNTY[2]=char(32);
	while (m_Country.GetLength()<2)
		m_Country+=" ";
	for(int i=0;i<2;i++) 
		PLOTPF_mp_COUNTY[i+3]=m_Country[i];
	PLOTPF_mp_COUNTY[5]=char(32);
	while(m_CoordLat.GetLength()<7)
		m_CoordLat+=" ";
	while(m_CoordLong.GetLength()<7)
		m_CoordLong+=" ";
	PLOTPF_mp_CKOORD[7]=char(47);
	PLOTPF_mp_CKOORD[15]=char(32);
	for(int i=0;i<7;i++) {
		PLOTPF_mp_CKOORD[i]=m_CoordLat[i];
		PLOTPF_mp_CKOORD[i+8]=m_CoordLong[i];
	}
	PLOTPF_mp_PTEX[0]=m_TopClay	;
	PLOTPF_mp_PTEX[1]=m_TopSand	;
	PLOTPF_mp_PTEX[2]=m_TopOrg	;
	PLOTPF_mp_PTEX[3]=m_SubClay	;
	PLOTPF_mp_PTEX[4]=m_SubSand	;
	PLOTPF_mp_PTEX[5]=m_SubOrg	;
// Write Comments
	CString kalle;
	m_PFcom.GetWindowText(kalle);
	// Reset
	m_pDoc->SP_SetComments(kalle);
// Load coef
//	PLOTPF_mp_LOADALLLAYERS();
// Write Loaded coeff
	PLOTPF_mp_SAVEDATABASE();
	m_ChangedData=TRUE;
   	if(m_MoveCursor&&!m_SaveNewNum) {
		if(m_CurSel<m_Selection.GetCount()-1) 
			m_CurSel++;
		m_Selection.SetCurSel(m_CurSel);
	}
	m_SaveNewNum=FALSE;
	OnInitDialog();	
}

void SP_EDIT::OnButtonDelete() 
{
	CString String;
	PLOTPF_mp_DELETEPROFILE();
	if(m_CurSel==m_Selection.GetCount()-1) 
		m_Selection.GetText(m_CurSel-1,String);
	else
		m_Selection.GetText(m_CurSel+1,String);
	CString String2=String.Mid(0,3);
	int prof=AtoInt(String2);
	String2=String.Mid(4,2);
	int repNumber=AtoInt(String2);
	int layer=1;
	PLOTPF_mp_GETPROFILE(&prof,&repNumber);
	UpdateData(TRUE);
	m_ChangedData=TRUE;
	OnInitDialog();
}
void SP_EDIT::OnSelchangeListSelection() 
{
	CString String;
	m_CurSel=m_Selection.GetCurSel();
	m_Selection.GetText(m_Selection.GetCurSel(),String);
	CString String2=String.Mid(0,3);
	int prof=AtoInt(String2);
	String2=String.Mid(4,2);
	int repNumber=AtoInt(String2);
	int layer=1;
//	m_pDoc->SP_ResetComments();
	PLOTPF_mp_GETPROFILE(&prof,&repNumber);
	UpdateData(TRUE);
	PLOTPF_mp_SUBPP=TRUE;
	OnInitDialog();
}
void SP_EDIT::OnChangeEditRep() 
{
	m_profchanged=TRUE;
}

void SP_EDIT::OnChangeEditProf() 
{
	m_profchanged=TRUE;
}

void SP_EDIT::OnKillfocusEditRep() 
{
	CString message;
	int choice;
	// TODO: Add your control notification handler code here
	if(m_profchanged) {
		UpdateData(TRUE);
		int prof=m_Prof;
		int repNumber=m_RepProf;
		int layer=1;
		PLOTPF_mp_ACTUALPROFILE=prof;
		PLOTPF_mp_ACTUALREPPROFILE=repNumber;
		if(PLOTPF_mp_ISINDATABASE()) {
			message="Another profile "+ItoAscii(prof)+":"+ItoAscii(repNumber)+ "Exist \n" ;
			message+="Do you really want to overwrite the other profile ?" ;
			choice=MessageBox(LPCTSTR(message),"Possible loss of data in database ",MB_YESNO|MB_ICONQUESTION|MB_TASKMODAL);
			if (choice==IDYES)
				PLOTPF_mp_SUBPP=TRUE;
			else {
				PLOTPF_mp_SUBPP=FALSE;
				OnInitDialog();
			}

		}
		m_profchanged=FALSE;
		PLOTPF_mp_FINDPROFILE=FALSE;
	}
}
void SP_EDIT::OnKillfocusEditProf() 
{
	OnKillfocusEditRep(); 
}


void SP_EDIT::OnTextureEdit() 
{
	// TODO: Add your control notification handler code here

	
}

void SP_EDIT::OnRetentionEdit() 
{

}

void SP_EDIT::OnOK() 
{
	// TODO: Add extra validation here
 	CDialog::OnOK();
}

void SP_EDIT::OnCancel() 
{
	PLOTPF_mp_GETPROFILE(&m_iniProf,&m_iniRepProf);
	CDialog::OnCancel();
}

void SP_EDIT::OnCoef() 
{

}


void SP_EDIT::OnKillfocusEditNumLayers() 
{
  int orignumlayers=m_NumLayers;
  CString message;
  int choice;
  UpdateData(TRUE);
  if(orignumlayers<int(m_NumLayers)) {
	message="Do you really want to increase the number of layers for the profile from "+ ItoAscii(orignumlayers)+" to "+ItoAscii(m_NumLayers)+ " layers ?";
	message+="\n\nNote that the new layers will by default have a thickness of 0.1 m and they will have the same coefficients as the the previous bottom layer of the soil profile. \n\nYou have to edit all coeffients as well as the texture and the retention points sepately !";
	choice=MessageBox(LPCTSTR(message),"Crucial change",MB_YESNO|MB_ICONQUESTION|MB_TASKMODAL);
	if(choice!=IDYES) {
		m_NumLayers=orignumlayers;
		UpdateData(FALSE);
	}
	else {
		for(int i=orignumlayers;i<int(m_NumLayers);i++) {
			PLOTPF_mp_BUDEP[i]=PLOTPF_mp_BLDEP[i-1];
			PLOTPF_mp_BLDEP[i]=PLOTPF_mp_BLDEP[i-1]+0.1f;
			PLOTPF_mp_APORO[i]=PLOTPF_mp_APORO[i-1];
			PLOTPF_mp_ASAND[i] =PLOTPF_mp_ASAND[i-1] ;
			PLOTPF_mp_ACLAY[i] =PLOTPF_mp_ACLAY[i-1] ;
			PLOTPF_mp_ALAMDA[i]=PLOTPF_mp_ALAMDA[i-1];
			PLOTPF_mp_ATETRE[i]=PLOTPF_mp_ATETRE[i-1];
			PLOTPF_mp_APSIE[i] =PLOTPF_mp_APSIE[i-1] ;
			PLOTPF_mp_AWILT[i] =PLOTPF_mp_AWILT[i-1] ;
			PLOTPF_mp_BSATC[i] =PLOTPF_mp_BSATC[i-1] ;
			PLOTPF_mp_BBSAT[i] =PLOTPF_mp_BBSAT[i-1] ;
			PLOTPF_mp_BNVAR[i] =PLOTPF_mp_BNVAR[i-1] ;
			PLOTPF_mp_BNSR[i] =PLOTPF_mp_BNSR[i-1] ;
			PLOTPF_mp_BNSE[i] =PLOTPF_mp_BNSE[i-1] ;
			PLOTPF_mp_BLB[i] =PLOTPF_mp_BLB[i-1] ;
			PLOTPF_mp_BUB[i] =PLOTPF_mp_BUB[i-1] ;
			PLOTPF_mp_BGA[i] =PLOTPF_mp_BGA[i-1] ;
			PLOTPF_mp_BGN[i] =PLOTPF_mp_BGN[i-1] ;
			PLOTPF_mp_BGM[i] =PLOTPF_mp_BGM[i-1] ;
		}
		m_SaveNewNum=TRUE;
		OnButtonApply();
	}

  }
  else if(orignumlayers>int(m_NumLayers)) {
		message="Do you really want to reduce the number of layers for the profile from "+ ItoAscii(orignumlayers)+" to "+ItoAscii(m_NumLayers)+ " layers ?";
		choice=MessageBox(LPCTSTR(message),"Crucial change that may cause loss of data",MB_YESNO|MB_ICONQUESTION|MB_TASKMODAL);
		if(choice!=IDYES) {
			m_NumLayers=orignumlayers;
			UpdateData(FALSE);
		}
		else
			m_SaveNewNum=TRUE;
			OnButtonApply();


  }
	
}

void SP_EDIT::OnButtonView() 
{
// Texture Profile

}

void SP_EDIT::OnButtonViewLayers() 
{
// Texture Layers

}

void SP_EDIT::OnButtonViewLayersRet() 
{
// Retention Layers

	
}

void SP_EDIT::OnButtonViewRet() 
{
// Retention Profile

}
