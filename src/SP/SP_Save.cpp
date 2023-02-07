// SP_Save.cpp : implementation file
//

#include "..\stdafx.h"
#include "..\Util.h"
#include "..\CoupModel.h"
#include "..\ModelTypes\SimB.h"
#include "..\CoupModelDoc.h"
#include "..\SP\SP_Save.h"
//#include "..\SP\SP_InitChar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/*
extern "C" bool   PLOTPF_mp_TOBEDOWNLOADED;
extern "C" bool   PLOTPF_mp_FINDPROFILE;
extern "C" void   PLOTPF_mp_GETLAYER(int *, int *, int*);
extern "C" void   PLOTPF_mp_RETRIEVECOEF(int*);
extern "C" void   PLOTPF_mp_RETRIEVEPF_POINTS();
extern "C" void   PLOTPF_mp_RETRIEVETEXTURE();
extern "C" void   PLOTPF_mp_LOADALLLAYERS();
extern "C" void   PLOTPF_mp_RETRIEVESOILPROFILE();
extern "C" void   PLOTPF_mp_RESETPROFILE();
extern "C" void   PLOTPF_mp_LOADNEWPROFILE(int*);
extern "C" int   PLOTPF_mp_SCANDATABASE(int*);
extern "C" void   PLOTPF_mp_OPENBASE();
extern "C" void   PLOTPF_mp_SAVEDATABASE();
extern "C" void   PROFILE_mp_SOILST();
extern "C" void   HYDRAULIC_mp_HYDRAULIC_RECALC();

extern "C" float   HYDRAULIC_mp_ESATC(int *);
extern "C" float   HYDRAULIC_mp_ETETRE(int *);
extern "C" float   HYDRAULIC_mp_EPSIE(int *);
extern "C" float   HYDRAULIC_mp_ELAMBDA(int *);
extern "C" float   HYDRAULIC_mp_EWILT(int *);

extern "C" BOOL PLOTPF_mp_ISINDATABASE();
extern "C" BOOL PLOTPF_mp_COMSET;


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

*/

/////////////////////////////////////////////////////////////////////////////
// SP_Save dialog


SP_Save::SP_Save(BOOL newprofile, CCoupModelDoc *pDoc,  CWnd* pParent /*=NULL*/)
: CDialog(SP_Save::IDD, pParent)
{


	m_pDoc=pDoc;
	m_pSimDoc=&pDoc->m_Doc;
	IniDataBase();;
	SaveAllLayers=TRUE;
	m_newprofile=newprofile;
	//if(newprofile) PLOTPF_mp_TOBEDOWNLOADED=TRUE;

	//{{AFX_DATA_INIT(SP_Save)
	m_SiteName = _T("");
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
	m_Prof = 0;
	m_RepProf = 0;
	m_NumLayers = 0;
	m_Year = 1998;
	//}}AFX_DATA_INIT
	if(newprofile) {
 		//PLOTPF_mp_RESETPROFILE();
		m_SiteName = _T("My New Profile");
  		m_County = _T("C");
	  	m_Country = _T("SE");
  		m_CoordLat = _T("12");
	  	m_CoordLong = _T("14");
  		m_SubClay = 10;
	  	m_SubSand = 20;
  		m_SubOrg = 3;
	  	m_TopClay = 10;
  		m_TopSand = 20;
	  	m_TopOrg = 3;
  		m_Prof = 300;
	  	m_RepProf = 1;
  		m_NumLayers = 1;
		m_NewSoil.resize(1);
		m_NewSoil[0].clay=10.;
		m_NewSoil[0].sand=20.;
		m_NewSoil[0].saturated=45.;
		m_NewSoil[0].upperdepth=0.;
		m_NewSoil[0].lowerdepth=.1;

		m_Year = 1998;
		NEWHEADER header;
		header=m_pPFCurve->GetHeader(int(m_Prof), int(m_RepProf));

			//PLOTPF_mp_ACTUALPROFILE=m_Prof;
			//PLOTPF_mp_ACTUALREPPROFILE=m_RepProf;

	/*		while(PLOTPF_mp_ISINDATABASE()) {
				if(m_RepProf<99) m_RepProf++;
				else {
					m_Prof++;
					m_RepProf=1;
				}
				PLOTPF_mp_ACTUALPROFILE=m_Prof;
				PLOTPF_mp_ACTUALREPPROFILE=m_RepProf;
			}
*/
	}
	
}
SP_Save::SP_Save(CCoupModelDoc *pDoc,int modlayer, CWnd* pParent /*=NULL*/)
: CDialog(SP_Save::IDD, pParent)
{
	m_pDoc=pDoc;
	SaveAllLayers=FALSE;
	m_modLayer=modlayer;
	m_newprofile=FALSE;
}


SP_Save::~SP_Save()
{	
	m_NewSoil.clear();

}

void SP_Save::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SP_Save)
	DDX_Control(pDX, IDC_LIST_SoilP3, m_SoilChar3);
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
	DDV_MinMaxUInt(pDX, m_Year, 1950, 2020);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SP_Save, CDialog)
	//{{AFX_MSG_MAP(SP_Save)
	ON_BN_CLICKED(IDC_BUTTON_SCAN, OnButtonScan)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SP_Save message handlers

 void SP_Save::OnOK() 
{
	// TODO: Add extra validation here
	CDialog::OnOK();
/*	PLOTPF_mp_OPENBASE();
    PLOTPF_mp_ACTUALPROFILE=m_Prof;
	PLOTPF_mp_ACTUALREPPROFILE=m_RepProf;
	if(m_Prof==0||m_RepProf==0) {
		
		MessageBox("Please make sure you have valid numbers of profile >0 !","Saving of Profiles to Database"); 
		return;

	}
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
	UpdateData(TRUE);
	m_PFcom.GetWindowText(kalle);
	PLOTPF_mp_SAVEDATABASE();

	m_pDoc->m_Doc.SP_SetComments(string(kalle));
			

	

	if(SaveAllLayers&&!m_newprofile) {

// Load coef
		if(PLOTPF_mp_ACTUALNUMLEVELS!=m_NumLayers&&m_NumLayers>0)
			PLOTPF_mp_ACTUALNUMLEVELS=m_NumLayers;
		PLOTPF_mp_LOADALLLAYERS();
// Write Loaded coeff
		PLOTPF_mp_SAVEDATABASE();
// Update ID 

		CString Name, Group, Type;
		P *pProf, *pRepProf,*pProfLayer;


		Name="ClayContent";
		Group="Soil Hydraulic";
		Type="Table Parameters";
	   	Name="ProfileNo";
	    pProf=(P*)m_pDoc->GetPtr(Name,Type,Group);
		Name="ProfileRepNo";
	    pRepProf=(P*)m_pDoc->GetPtr(Name,Type,Group);
		Name="ProfileLayer";
	    pProfLayer=(P*)m_pDoc->GetPtr(Name,Type,Group);
	
		for(int i=0;i<PLOTPF_mp_ACTUALNUMLEVELS;i++) {
			pProf->		SetValue(i,	   float(PLOTPF_mp_ACTUALPROFILE)	);
			pRepProf->	SetValue(i,	   float(PLOTPF_mp_ACTUALREPPROFILE) 	);
			pProfLayer->SetValue(i,	   float(i+1)	);
		}
		pProf->Apply(PLOTPF_mp_ACTUALNUMLEVELS);			
		pRepProf->Apply(PLOTPF_mp_ACTUALNUMLEVELS);	
		pProfLayer->Apply(PLOTPF_mp_ACTUALNUMLEVELS);
	}
	else if(m_newprofile) {
	

		CString Name, Group, Type;
		P *pClayContent, *pSandContent,*pSaturation;

		Name="ClayContent";
		Group="Soil Hydraulic";
		Type="Table Parameters";
		pClayContent=(P*)m_pDoc->GetPtr(Name,Type,Group);
		Name="SandContent";
	    pSandContent=(P*)m_pDoc->GetPtr(Name,Type,Group);
		Name="Saturation";
		pSaturation=(P*)m_pDoc->GetPtr(Name,Type,Group);


		PLOTPF_mp_RESETPROFILE();
		PLOTPF_mp_NT=0;
		PLOTPF_mp_ACTUALNUMLEVELS=m_NumLayers;
		for(UINT i=0;i<m_NumLayers;i++){
			pSandContent->SetValue(0,*(float*)m_arrSand[i]);
			pClayContent->SetValue(0,*(float*)m_arrClay[i]);
			pSaturation->SetValue(0,*(float*)m_arrSaturation[i]);
			pSandContent->Apply(1);
			pClayContent->Apply(1);
			pSaturation->Apply(1);
			int findex;
			findex=1;
			PLOTPF_mp_BUDEP[i]=*(float*)m_arrUpperDepth[i];
			PLOTPF_mp_BLDEP[i]=*(float*)m_arrLowerDepth[i];
			PLOTPF_mp_ASAND[i]=*(float*)m_arrSand[i];
			PLOTPF_mp_ACLAY[i]=*(float*)m_arrClay[i];
			PLOTPF_mp_ALAMDA[i]=HYDRAULIC_mp_ELAMBDA(&findex);
			PLOTPF_mp_ATETRE[i]=HYDRAULIC_mp_ETETRE(&findex);
			PLOTPF_mp_APSIE[i]=HYDRAULIC_mp_EPSIE(&findex);
			PLOTPF_mp_AWILT[i]=HYDRAULIC_mp_EWILT(&findex);
			PLOTPF_mp_BSATC[i]=HYDRAULIC_mp_ESATC(&findex);
			PLOTPF_mp_APORO[i]=*(float*)m_arrSaturation[i];
		}
		int ivalue=m_NumLayers;
		PLOTPF_mp_LOADNEWPROFILE(&ivalue);
		PLOTPF_mp_SAVEDATABASE();

	}
	else {
		PLOTPF_mp_SAVEDATABASE();

	}
	*/
}

void SP_Save::OnButtonScan() 
{
 /*  int Record=1;
   CString string, tmpstring;	
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

   UpdateData(TRUE);
   m_Selection.SetTabStops(numTab,tabstops);
   m_Selection.ResetContent();
   while(PLOTPF_mp_SCANDATABASE(&Record)>1) {
		string=_T("");
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
			tmpstring=MFC_Util::FtoAscii(PLOTPF_mp_PTEX[i]);
			int ip=tmpstring.Find(".");
			if(ip>0) tmpstring=tmpstring.Mid(0,ip+2);
			string+=tmpstring+"\t";
		}
		m_Selection.AddString(string);
		Record++;	
	}
   
	if(m_Selection.GetCount()>0) 
		m_Selection.SetCurSel(0);
	else
		MessageBox("No Profiles Fits Your Constrains - Make a new Trial !","Selection of Profiles from Database"); 

	// TODO: Add your control notification handler code here
	*/
}

BOOL SP_Save::OnInitDialog() 
{
	CString tmpplace;
	m_ActHeader=m_pPFCurve->GetHeader(m_Prof, m_RepProf);

	if(m_pPFCurve->IsProfile()) {
	 //  m_Prof=PLOTPF_mp_ACTUALPROFILE;


	   m_SiteName=m_ActHeader.Name.c_str();

	   m_County=m_ActHeader.County.c_str();
	   m_Country=m_ActHeader.Country.c_str();



		m_TopClay	=m_ActHeader.Upper_Clay;
		m_TopSand	=m_ActHeader.Upper_Sand;
		m_TopOrg	=m_ActHeader.Upper_Org;
		m_SubClay	=m_ActHeader.Lower_Clay;
		m_SubSand	=m_ActHeader.Lower_Sand;
		m_SubOrg	=m_ActHeader.Lower_Org;
    }


	CDialog::OnInitDialog();
	if(m_newprofile) {
		m_SoilChar3.ResetContent();
		m_SoilChar3.SetTabStops(24);
		CString string;
		int m_NumLayers=m_NewSoil.size();
		for (int i=0;i<m_NumLayers;i++){
			NEWSOIL soil;
			soil=m_NewSoil[i];
			string=MFC_Util::ItoAscii(i+1);
			string+="\t";
			string+=MFC_Util::FtoAscii(soil.upperdepth);
			string+="\t";
			string+=MFC_Util::FtoAscii(soil.lowerdepth);
			string+="\t";
			string+=MFC_Util::FtoAscii(soil.clay);
			string+="\t";
			string+=MFC_Util::FtoAscii(soil.sand);
			string+="\t";
			string+=MFC_Util::FtoAscii(soil.saturated);
			m_SoilChar3.AddString(string);
		}

		m_SoilChar3.SetCurSel(m_NumLayers-1);

	}
	else {
// Hide controls
//		WS_VISIBLE
		GetDlgItem(IDC_LIST_SoilP3)->ModifyStyle(0,WS_VISIBLE);
	    GetDlgItem(IDC_LIST_SoilP3)->RedrawWindow();
		GetDlgItem(IDC_ADD)->ModifyStyle(0,WS_DISABLED);
	    GetDlgItem(IDC_ADD)->RedrawWindow();
		GetDlgItem(IDC_EDIT)->ModifyStyle(0,WS_DISABLED);
	    GetDlgItem(IDC_EDIT)->RedrawWindow();
		GetDlgItem(IDC_RESET)->ModifyStyle(0,WS_DISABLED);
	    GetDlgItem(IDC_RESET)->RedrawWindow();

		GetDlgItem(IDC_NEW)->ModifyStyle(0,WS_DISABLED);
	    GetDlgItem(IDC_NEW)->RedrawWindow();
		GetDlgItem(IDC_NEW1)->ModifyStyle(0,WS_DISABLED);
	    GetDlgItem(IDC_NEW1)->RedrawWindow();
		GetDlgItem(IDC_NEW2)->ModifyStyle(0,WS_DISABLED);
	    GetDlgItem(IDC_NEW2)->RedrawWindow();
		GetDlgItem(IDC_NEW3)->ModifyStyle(0,WS_DISABLED);
	    GetDlgItem(IDC_NEW3)->RedrawWindow();
	   	GetDlgItem(IDC_NEW4)->ModifyStyle(0,WS_DISABLED);
	    GetDlgItem(IDC_NEW4)->RedrawWindow();
	   	GetDlgItem(IDC_STATIC_DEP)->ModifyStyle(0,WS_DISABLED);
	    GetDlgItem(IDC_STATIC_DEP)->RedrawWindow();


	}

		m_PFcom.SetTabStops(41);
		//PLOTPF_mp_COMSET=TRUE;
		CString kalle;
		if(m_newprofile) {
			CTime time=CTime::GetCurrentTime();
  			kalle = time.Format( "%A, %d %B , %Y" );
			kalle+="\r\n please edit this text with relevant notes on";
			kalle+="\r\n projects and publications ....";
			kalle+="\r\n and do not forget your signature";
		}
		else
			kalle=m_pPFCurve->GetComments().c_str();
		if(kalle.GetLength()>0) {
		    m_PFcom.SetWindowText(kalle);
		}
		else {
		    m_PFcom.SetWindowText("No details on this Profile exist");
		}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SP_Save::OnAdd() 
{
	UpdateData(TRUE);
/*
	SP_InitChar dlg( this);
	if(dlg.DoModal()==IDOK) {
			NEWSOIL soil;
			soil.clay=dlg.m_Clay;
			soil.sand=dlg.m_Sand;
			soil.saturated=dlg.m_Saturation;
			soil.upperdepth=dlg.m_UpperDepth;
			soil.lowerdepth=dlg.m_LowerDepth;
			if(m_NumLayers<m_NewSoil.size()) 
				m_NewSoil[m_NumLayers]=soil;
			else {
				m_NewSoil.resize(m_NumLayers+1);
				m_NewSoil[m_NumLayers]=soil;
			}

	}*/
	m_NumLayers++;
	OnInitDialog();
	
}

void SP_Save::OnEdit() 
{
	UpdateData(TRUE);

	int layer=m_SoilChar3.GetCurSel();
	int laysave=m_NumLayers;
	m_NumLayers=layer;
/*	SP_InitChar dlg(this);
	if(dlg.DoModal()==IDOK){
			NEWSOIL soil;
			soil.clay=dlg.m_Clay;
			soil.sand=dlg.m_Sand;
			soil.saturated=dlg.m_Saturation;
			soil.upperdepth=dlg.m_UpperDepth;
			soil.lowerdepth=dlg.m_LowerDepth;
			if(m_NumLayers<m_NewSoil.size()) 
				m_NewSoil[m_NumLayers]=soil;
			else {
				m_NewSoil.resize(m_NumLayers+1);
				m_NewSoil[m_NumLayers]=soil;
			}
	}*/
	m_NumLayers=laysave;
	OnInitDialog();
   }

void SP_Save::OnReset() 
{
	int layer=m_SoilChar3.GetCurSel();
	
	m_NumLayers--;
	m_NewSoil.resize(m_NumLayers);
	OnInitDialog();
	// TODO: Add your control notification handler code here
	
}
void SP_Save::IniDataBase() {
	m_pPFCurve=MySingle::instance().GetPlotPFPointer();
	if(!m_pPFCurve->IsOpen()) {
	  string directory;
	  directory=MFC_Util::GetProfileStringStd("DataBaseDirectory", directory);
	  m_pPFCurve->OpenMainFile(directory+"PfProf");

	}
	if(!m_pPFCurve->NewFormatFile()) {
		m_pPFCurve->OpenEditMainDataBase();
		m_pPFCurve->ReadHeaderFile();
		m_pPFCurve->SaveNewFormat();
		m_pPFCurve->CloseMainDataBase();
		 string directory;
		directory=MFC_Util::GetProfileStringStd("DataBaseDirectory", directory);
		m_pPFCurve->OpenMainFile(directory+"PfProf");
	}
	m_ActHeader=m_pPFCurve->GetHeader(m_pPFCurve->GetInsertRecord(0));
	m_ActHeader=m_pPFCurve->GetHeader(m_pPFCurve->GetInsertRecord(1));
	if(m_ActHeader.RecHeader>0) {
	   m_IniProf=m_ActHeader.KeyProf/100;
	   m_IniRepProf=m_ActHeader.KeyProf%100;
	}
	else
		m_IniProf=-10;

}
