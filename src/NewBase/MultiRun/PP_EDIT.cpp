// PP_EDIT.cpp : implementation file
//

#include "../stdafx.h"
#include "../Util.h"
#include "../CoupModel.h"
#include "../CoupModelDoc.h"
#include "../DB\DB_Plant.h"
#include "../PG\FileDescription.h"
#include "../PG\PG.h"
#include "../MultiRun\pp_edit.h"
#include "../ModelTypes\NewModelType.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(PP_EDIT, CDialog, 0)


/////////////////////////////////////////////////////////////////////////////
// PP_EDIT dialog


PP_EDIT::PP_EDIT(CDB *pDB, Doc *pDoc, int nrow, CWnd* pParent )
	: CDialog(PP_EDIT::IDD, pParent)
{
	string Type, Group, Name;


	m_TotalReadOnly=TRUE;
	m_ReadOnly=TRUE;
	m_pCDB=pDB;
	m_ID_DB_Group=m_pCDB->GetName().c_str();
	m_pDoc=pDoc;
	m_NumToSelect=nrow;
	Name="Nitrogen and Carbon";
	Type="Switches"; Group="Model Structure";
	m_pSw_NC=(Sw*)m_pDoc->GetPtr( Type, Group, Name);

	m_SysStore=FALSE;
	m_RetrieveOne=FALSE;
	SetGroupConnections();
	CWinApp* pApp = AfxGetApp();
	CString m_PlantDB_Name;
	m_PlantDB_Name=pApp->GetProfileString("CoupModel - KTH","DataBaseDirectory",m_PlantDB_Name);
	m_PlantDB_Name+=m_ID_DB_Group;
	m_PlantDB_Name+="\\DB_"+m_ID_DB_Group+".DBM";
	
	m_InitDialog=FALSE;
 	m_SelectedIndex=-1;
	m_ArrayIndex=-1;
	m_SelectedKey=_T("");

	m_NumOriginalRecords=0;
	m_SavedDataInRecord=TRUE;

	TRY
	{
		m_SysFile.Open(m_PlantDB_Name, CFile::modeReadWrite);
		CArchive ar(&m_SysFile, CArchive::load);
		m_NumOriginalRecords=0;
		Serialize(ar);
		m_NumOriginalRecords=m_arrCountry.GetSize();
	}
	CATCH( CFileException, e )
	{
    #ifdef _DEBUG
        afxDump << "File could not be opened " << e->m_cause << "\n";
    #endif
	m_SysFile.Open(m_PlantDB_Name,CFile::modeCreate|CFile::modeReadWrite);
	CArchive ar(&m_SysFile, CArchive::store);
	Serialize(ar);
	Save();

	}

	END_CATCH

	m_PlantDB_Name=pApp->GetProfileString("CoupModel - KTH","DataBaseDirectory",m_PlantDB_Name);
	m_PlantDB_Name+=m_ID_DB_Group;
	m_PlantDB_Name+="\\DB_"+m_ID_DB_Group+".DBS";

	
	TRY
	{
		CFile file(m_PlantDB_Name, CFile::modeReadWrite);
	}
	CATCH( CFileException, e )
	{
    #ifdef _DEBUG
        afxDump << "File could not be opened " << e->m_cause << "\n";
    #endif
		CFile file(m_PlantDB_Name,CFile::modeCreate|CFile::modeReadWrite);
		CArchive ar(&file, CArchive::store);
		Serialize(ar);
		Save();
	}

	END_CATCH

	m_file.Open(m_PlantDB_Name, CFile::modeReadWrite);
	CArchive ar(&m_file, CArchive::load);
	Serialize(ar);
	//Retrieve();
	//{{AFX_DATA_INIT(PP_EDIT)
	m_PlantName = _T("Not specified");
	m_Country = _T("SE");
	m_Longitude = _T("60.58N");
	m_Latitude = _T("16.00E");
	m_NumAbove = 0;
	m_NumBelow = 0;
	m_SiteName = _T("Not specified");
	m_DriveFileExist = FALSE;
	m_DriveFile = _T("");
	m_DriveFile_Exist = FALSE;
	m_NameText = _T("");
	m_TypeText = _T("");
	m_NameText2 = _T("");
	m_TypeText2 = _T("");
	m_GenSwitch1 = _T("");
	m_GenSwitch2 = _T("");
	m_GenSwitch3 = _T("");
	m_SwTimeResolution = _T("");
	//}}AFX_DATA_INIT

	Type="Switches";
	Group=m_ID_DB_Group;
	Name="LaiInput";

	Group="Plant";
	m_pSwLAI=(Sw*)m_pDoc->GetPtr(Type, Group, Name);
	Name="RootInput";
	m_pSwRoot=(Sw*)m_pDoc->GetPtr(Type, Group, Name);
	Name="Crop data";
	Type="Files";
	m_pFile=(F*)m_pDoc->GetPtr(Type, Group, Name);
	Name="Evaporation"; Type="Switches"; Group="Model Structure";
	m_pSwGen1=(Sw*)m_pDoc->GetPtr(Type, Group, Name);
	Name="PlantType";
	m_pSwGen2=(Sw*)m_pDoc->GetPtr(Type, Group, Name);
	Name="Nitrogen and Carbon";
	m_pSw_NC=(Sw*)m_pDoc->GetPtr(Type, Group, Name);
	Name="TimeResolution";
	Group="Hidden";
	m_pSwTimeRes=(Sw*)m_pDoc->GetPtr(Type, Group, Name);

//	m_DataBaseSelected.OnUpdateDataBase(FALSE, this, pDB);
//	m_DatabasCUG.OnUpdateDataBase(TRUE, this, pDB, &m_DataBaseSelected);
	m_Data.OnUpdateDataBase(this,pDB);
	m_SysStore=FALSE;
}
PP_EDIT::PP_EDIT(CString str, Doc *pDoc, CDB *pDB, BOOL readonly, CWnd* pParent)
	: CDialog(PP_EDIT::IDD, pParent)
{
	// To retrieve or manage the dialog window. 
	string Type, Group, Name;
	m_TotalReadOnly=readonly;
	m_ID_DB_Group=str;
	m_pDoc=pDoc;

	m_ReadOnly=readonly;
	Name="Nitrogen and Carbon";
	Type="Switches"; Group="Model Structure";
	m_pSw_NC=(Sw*)m_pDoc->GetPtr(Type, Group, Name);
	m_SysStore=FALSE;

	m_RetrieveOne=TRUE;



	SetGroupConnections();
	CWinApp* pApp = AfxGetApp();
	CString m_PlantDB_Name;
	m_pCDB=pDB;
	m_PlantDB_Name=pApp->GetProfileString("CoupModel - KTH","DataBaseDirectory",m_PlantDB_Name);
	m_PlantDB_Name+=m_ID_DB_Group;
	m_PlantDB_Name+="\\DB_"+m_ID_DB_Group+".DBM";
	
	m_InitDialog=FALSE;
 	m_SelectedIndex=-1;
	m_ArrayIndex=-1;
	m_SelectedKey=_T("");
	m_NumOriginalRecords=0;
	m_SavedDataInRecord=TRUE;
	m_NumToSelect=1;

	TRY
	{
		m_SysFile.Open(m_PlantDB_Name, CFile::modeReadWrite);
		CArchive ar(&m_SysFile, CArchive::load);
		
		m_NumOriginalRecords=0;
		Serialize(ar);
		m_NumOriginalRecords=m_arrCountry.GetSize();
	}
	CATCH( CFileException, e )
	{
    #ifdef _DEBUG
        afxDump << "File could not be opened " << e->m_cause << "\n";
    #endif
	m_SysFile.Open(m_PlantDB_Name,CFile::modeCreate|CFile::modeReadWrite);
	CArchive ar(&m_SysFile, CArchive::store);
	Serialize(ar);
	Save();

	}

	END_CATCH

	m_PlantDB_Name=pApp->GetProfileString("CoupModel - KTH","DataBaseDirectory",m_PlantDB_Name);
	m_PlantDB_Name+=m_ID_DB_Group;
	m_PlantDB_Name+="\\DB_"+m_ID_DB_Group+".DBS";

	
	TRY
	{
		CFile file(m_PlantDB_Name, CFile::modeReadWrite);
	}
	CATCH( CFileException, e )
	{
    #ifdef _DEBUG
        afxDump << "File could not be opened " << e->m_cause << "\n";
    #endif
		CFile file(m_PlantDB_Name,CFile::modeCreate|CFile::modeReadWrite);
		CArchive ar(&file, CArchive::store);
		Serialize(ar);
		Save();
	}

	END_CATCH
	TRY {
		m_file.Open(m_PlantDB_Name, CFile::modeReadWrite);
		CArchive ar(&m_file, CArchive::load);
		Serialize(ar);
	}
	CATCH( CFileException, e )
	{
   	}

	END_CATCH
	//Retrieve();
	//{{AFX_DATA_INIT(PP_EDIT)
	m_PlantName = _T("Not specified");
	m_Country = _T("SE");
	m_Longitude = _T("60.58N");
	m_Latitude = _T("16.00E");
	m_NumAbove = 0;
	m_NumBelow = 0;
	m_SiteName = _T("Not specified");
	m_DriveFileExist = FALSE;
	m_DriveFile = _T("");
	m_DriveFile_Exist = FALSE;
	m_NameText = _T("");
	m_TypeText = _T("");
	m_NameText2 = _T("");
	m_TypeText2 = _T("");
	m_GenSwitch1 = _T("");
	m_GenSwitch2 = _T("");
	m_GenSwitch3 = _T("");
	m_SwTimeResolution = _T("");
	//}}AFX_DATA_INIT

	Type="Switches";
	Group=m_ID_DB_Group;
	Name="LaiInput";
	Group="Plant";
	m_pSwLAI=(Sw*)m_pDoc->GetPtr(Type, Group, Name);
	Name="RootInput";
	m_pSwRoot=(Sw*)m_pDoc->GetPtr(Type, Group, Name);
	Name="Crop data";
	Type="Files";
	m_pFile=(F*)m_pDoc->GetPtr(Type, Group, Name);
	Name="Evaporation"; Type="Switches"; Group="Model Structure";
	m_pSwGen1=(Sw*)m_pDoc->GetPtr(Type, Group, Name);
	Name="PlantType";
	m_pSwGen2=(Sw*)m_pDoc->GetPtr(Type, Group, Name);
	Name="Nitrogen and Carbon";
	m_pSw_NC=(Sw*)m_pDoc->GetPtr(Type, Group, Name);
	Name="TimeResolution";
	Group="Hidden";
	m_pSwTimeRes=(Sw*)m_pDoc->GetPtr(Type, Group, Name);

//	m_DataBaseSelected.OnUpdateDataBase(FALSE, this, NULL);
//	m_DatabasCUG.OnUpdateDataBase(TRUE, this, NULL, &m_DataBaseSelected);
	m_Data.OnUpdateDataBase(this,pDB);
	

}
PP_EDIT::PP_EDIT(CString strType, CString strKey, Doc *pDoc, CDB *pDB, CWnd* pParent)
	: CDialog(PP_EDIT::IDD, pParent)
{

// Used to retrieve a certain key of Database object, not for interaction with user

	string Type, Group, Name;
	m_pDoc=pDoc;

	m_ReadOnly=TRUE;
	m_TotalReadOnly=TRUE;
	m_SysStore=FALSE;
	Name="Nitrogen and Carbon";
	Type="Switches"; Group="Model Structure";
	m_pSw_NC=(Sw*)m_pDoc->GetPtr(Type, Group, Name);
	m_RetrieveOne=FALSE;

	CWinApp* pApp = AfxGetApp();
	CString m_PlantDB_Name;
	m_ID_DB_Group=strType;
	SetGroupConnections();
	m_InitDialog=FALSE;

 	m_SelectedIndex=-1;
	m_ArrayIndex=-1;
	m_SelectedKey=_T("");
	m_NumOriginalRecords=0;
	m_SavedDataInRecord=TRUE;
	m_NumToSelect=1;


	m_SimFileName="DB_Plant_";
	m_SimFileName=pApp->GetProfileString("CoupModel - KTH","DataBaseDirectory",m_SimFileName);
	m_SimFileName+=m_ID_DB_Group+"\\DB_"+m_ID_DB_Group+"_"+strKey;
	m_SimFileName+=".Sim";
	m_pCDB=pDB;

	TRY
	{
		CFile file(m_SimFileName, CFile::modeRead);
	}
	CATCH( CFileException, e )
	{
   // #ifdef _DEBUG
    //    afxDump << "File could not be opened " << e->m_cause << "\n";
	//	return;
    //#endif
		CFile file(m_SimFileName,CFile::modeCreate);
		return;
	}

	END_CATCH

	CFile file(m_SimFileName, CFile::modeRead);

	DB_Plant wizsheet(string(m_ID_DB_Group),m_pDoc);
	CArchive ar(&file, CArchive::load);
	file.SeekToBegin();
	if(file.GetLength()==0){
		




	}
	else {
//REMAIN		wizsheet.Serialize(ar);

		DB_Plant* pPlant;

		for(int i=0;i<m_arrGroups.GetSize();i++) {
			pPlant= new DB_Plant(string(m_arrGroups[i]),m_pDoc);
			pPlant->m_DuringRun=TRUE;
			pPlant->DB_Actual();
			delete pPlant;
		}
	}

	Type="Switches";
	Group=m_ID_DB_Group;
	Name="LaiInput";
	Group="Plant";
	m_pSwLAI=(Sw*)m_pDoc->GetPtr(Type, Group, Name);
	Name="RootInput";
	m_pSwRoot=(Sw*)m_pDoc->GetPtr(Type, Group, Name);
	Name="Crop data";
	Type="Files";
	m_pFile=(F*)m_pDoc->GetPtr(Type, Group, Name);
	Name="Evaporation"; Type="Switches"; Group="Model Structure";
	m_pSwGen1=(Sw*)m_pDoc->GetPtr(Type, Group, Name);
	Name="PlantType";
	m_pSwGen2=(Sw*)m_pDoc->GetPtr(Type, Group, Name);

	Name="Nitrogen and Carbon";
	m_pSw_NC=(Sw*)m_pDoc->GetPtr(Type, Group, Name);
	Name="TimeResolution";
	Group="Hidden";
	m_pSwTimeRes=(Sw*)m_pDoc->GetPtr(Type, Group, Name);

	
	//m_DataBaseSelected.OnUpdateDataBase(FALSE, this, NULL);
	//m_DatabasCUG.OnUpdateDataBase(TRUE, this, NULL, &m_DataBaseSelected);
	m_SysStore=FALSE;
}

PP_EDIT::~PP_EDIT() 
{

	int i=0;
}

void PP_EDIT::SetGroupConnections()
{
	string Type, Group, Name;


	m_arrGroups.RemoveAll();
	if(m_ID_DB_Group=="Soil Mineral N Processes")
		m_arrGroups.Add("Soil mineral N processes");
	else if(m_ID_DB_Group=="Soil Organic Processes")
		m_arrGroups.Add("Soil organic processes");
	else if(m_ID_DB_Group=="Snow")
		m_arrGroups.Add("SnowPack");
	else
		m_arrGroups.Add(m_ID_DB_Group);
	if(m_ID_DB_Group==m_pDoc->DB_GetNames(0).c_str()) { //Plant
		m_arrGroups.Add("Potential Transpiration");
		m_arrGroups.Add("Water uptake");
		m_arrGroups.Add("Interception");
	}
	else if(m_ID_DB_Group==m_pDoc->DB_GetNames(3).c_str()) {  // Snow
		Type="Switches"; Group="Model Structure";Name="SnowPack";
		Sw *pSw;
		pSw=(Sw*)m_pDoc->GetPtr(Type, Group, Name);
		pSw->SetDBIntValue(1);
	}
	else if(m_ID_DB_Group==m_pDoc->DB_GetNames(4).c_str())	 { // Soil Frost
	
	}
	else if(m_ID_DB_Group==m_pDoc->DB_GetNames(5).c_str()) {		// Soil Water Flow
		m_arrGroups.Add("Drainage and deep percolation");
		m_arrGroups.Add("Surface water");
	}
	else if(m_ID_DB_Group==m_pDoc->DB_GetNames(2).c_str()) {  // Soil Evaporation
		m_arrGroups.Add("Soil water flows");
	}
	else if(m_ID_DB_Group==m_pDoc->DB_GetNames(7).c_str()) {
		m_arrGroups.Add("External N inputs");
		m_arrGroups.Add("Meteorological Data");
		m_arrGroups.Add("Radiation properties");
		m_arrGroups.Add("Soil Management");
		m_pSw_NC->SetDBIntValue(1);
	}
	else if(m_ID_DB_Group==m_pDoc->DB_GetNames(8).c_str()) {
		m_arrGroups.Add("Common abiotic responses");
		m_pSw_NC->SetDBIntValue(1);

	}
	else if(m_ID_DB_Group==m_pDoc->DB_GetNames(9).c_str()) {
		m_arrGroups.Add("Gas processes");
		m_pSw_NC->SetDBIntValue(1);

	}
	else if(m_ID_DB_Group==m_pDoc->DB_GetNames(6).c_str()) {
		m_arrGroups.Add("Multiple Plant");
		m_arrGroups.Add("Potential Transpiration");
		m_pSw_NC->SetDBIntValue(1);
	}
	if(m_ID_DB_Group==m_pDoc->DB_GetNames(0).c_str()||m_ID_DB_Group==m_pDoc->DB_GetNames(6).c_str()) {
	 	m_PlantTypes.Add("Coniferous Forest");
		m_PlantTypes.Add("Decidous Forest");
		m_PlantTypes.Add("Mixed Forest");
		m_PlantTypes.Add("Natural Grassland");
		m_PlantTypes.Add("Crops");
		m_PlantTypes.Add("Undefined");
	//	m_arrGroups.Add("Multiple Plant");
	//	m_arrGroups.Add("Potential Transpiration");

	}
	else if(m_ID_DB_Group==m_pDoc->DB_GetNames(3).c_str()) {
		m_PlantTypes.Add("Seasonal snow");
		m_PlantTypes.Add("Permanent snow");
		m_PlantTypes.Add("Undefined");
	}
	else if(m_ID_DB_Group==m_pDoc->DB_GetNames(5).c_str()) {
		m_PlantTypes.Add("Natural drainage");
		m_PlantTypes.Add("Tile drainage");
		m_PlantTypes.Add("Ditches or canals");
		m_PlantTypes.Add("Undefined");
	}
	else {
		m_PlantTypes.Add("Clayey soil");
		m_PlantTypes.Add("Sandy soil");
		m_PlantTypes.Add("Silty soil");
		m_PlantTypes.Add("Loamy soil");
		m_PlantTypes.Add("Organic soil");
		m_PlantTypes.Add("Undefined");
	}
}


void PP_EDIT::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PP_EDIT)

	DDX_Control(pDX, IDC_DETAILS, m_Details);
	
	
	//}}AFX_DATA_MAP

//	DDX_Control(pDX, IDC_PFCUG, m_DatabasCUG);
//	DDX_Control(pDX, IDC_PFCUG_TOSIM, m_DataBaseSelected);
}


BEGIN_MESSAGE_MAP(PP_EDIT, CDialog)
	//{{AFX_MSG_MAP(PP_EDIT)
	
	ON_BN_CLICKED(IDC_BUTTON_CREATE, OnButtonCreate)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, OnButtonApply)
	ON_BN_CLICKED(IDC_CHECK_DRIVE, OnBrowse)
	ON_BN_CLICKED(IDCANCEL2, OnReset)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_RESET_SELECTION, OnBnClickedResetSelection)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PP_EDIT message handlers

void PP_EDIT::OnOK() 
{
	m_file.SeekToBegin();
	CArchive ar(&m_file, CArchive::store);
	Serialize(ar);

	if(m_RetrieveOne) {
		if(m_ArrayIndex<0) {
			int result;
			CString message;
			message="No Records defined, Please click on list to select or cancel dialog";
			result=MessageBox(LPCTSTR(message),"Record Error",MB_OK);
			m_SelectedId=_T("");
			m_SelectedKey=_T("");
			return;
		}
		else {
			m_SelectedId=m_PlantTypes[m_arrPlantType[m_ArrayIndex]]; m_SelectedId+="\t";
			m_SelectedId+=m_arrPlantName[m_ArrayIndex];m_SelectedId+="\t";
			m_SelectedId+=m_arrSiteName[m_ArrayIndex];m_SelectedId+="\t";
			m_SelectedId+=m_arrLongitude[m_ArrayIndex];m_SelectedId+="/";
			m_SelectedId+=m_arrLatitude[m_ArrayIndex];m_SelectedId+="\t";
			m_SelectedId+=m_arrCountry[m_ArrayIndex];
			m_SelectedKey=m_arrIdUser[m_ArrayIndex]+MFC_Util::ItoAscii(m_arrIdNum[m_ArrayIndex]);
			if(m_pCDB!=NULL) {
				m_Reset=FALSE;
				DB_Actual();
				m_pCDB->SetValue(string(m_SelectedKey));
			}
		}

	}
	
	m_Reset=FALSE;
	CDialog::OnOK();
	//m_file.Close();
}

void PP_EDIT::OnCancel() 
{
	// TODO: Add extra cleanup here
	m_file.Close();
	CDialog::OnCancel();
}

void PP_EDIT::OnButtonCreate() 
{	string m;
	int ans;
	m="Do you really want to make a new record from your current document:"+ m_pDoc->GetCurrentFile();
	m+="\nto be stored in your database";
	ans=MessageBox(m.c_str(),"Creating new record",MB_YESNO);
	if(ans!=IDYES) return;
	m_arrPlantType.Add(0);
	m_arrPlantName.Add("Not Specified");
	m_arrSiteName.Add("Not Specified");
	m_arrLatitude.Add("");
	m_arrLongitude.Add("");
	m_arrCountry.Add(""); 
	m_arrDriveFile.Add(""); 
	CString signature="NN";
	CWinApp* pApp = AfxGetApp();
	signature=pApp->GetProfileString("CoupModel - KTH", "Signature", signature);
	CTime tt=CTime::GetCurrentTime();
	CString str=tt.Format( "%Y-%m-%d %H:%M:%S" );
	m_arrDetails.Add("Edit this text to include method of estimation and \r\nreferences to publications/reports. \r\nAdd any information which improves the understanding of \r\nthe representativity of the parameter values.\r\nDo not forget your full name.\r\n"+signature+" "+str); 
	

	m_arrIdUser.Add(signature);  
	int max=0;
	for(int i=0;i<m_arrIdNum.GetSize();i++) {
		if(max<int(m_arrIdNum[i])&&m_arrIdUser[i]==signature) max=m_arrIdNum[i];
	}
	m_arrIdNum.Add(max+1); 
	m_arrIdKey.Add(signature+MFC_Util::ItoAscii(max+1));
	m_ArrayIndex=m_arrPlantType.GetSize()-1;
	m_SelectedKey=signature+MFC_Util::ItoAscii(max+1);
	m_SelectedKeyDisplay=m_SelectedKey;
	m_SelectedIndex=m_ArrayIndex;
// Save main archive
//	m_file.SeekToBegin();
//	CArchive ar(&m_file, CArchive::store);
//	Serialize(ar);
// Save Sim archive
	Actual_DB();
	Save();
	m_Data.OnSetup();
	OnInitDialog();

	
}
void PP_EDIT::CopyKey(int irec) 
{
	CString str;
	int isize=m_arrPlantType.GetSize();
	int istore=m_arrPlantType.GetAt(irec);
	m_arrPlantType.SetAtGrow(isize, istore);
	str=m_arrPlantName[irec];
	m_arrPlantName.SetAtGrow(isize,str);
	str=m_arrSiteName[irec];
	m_arrSiteName.SetAtGrow(isize,str);
	str=m_arrLatitude[irec];
	m_arrLatitude.SetAtGrow(isize,str);
	str=m_arrLongitude[irec];
	m_arrLongitude.SetAtGrow(isize,str);
	str=m_arrCountry[irec];
	m_arrCountry.SetAtGrow(isize,str);
	str=m_arrDriveFile[irec];
	m_arrDriveFile.SetAtGrow(isize,str);
	str=m_arrDetails[irec];


	CString signature="NN";
	CWinApp* pApp = AfxGetApp();
	signature=pApp->GetProfileString("CoupModel - KTH", "Signature", signature);
	m_arrIdUser.Add(signature); 
	
	CTime tt=CTime::GetCurrentTime();
	CString timest=tt.Format( "%Y-%m-%d %H:%M:%S" );
	str+="\r\nCopy of previous record made by "+signature+timest;
	m_arrDetails.Add(str); 

 
	int max=0;
	for(int i=0;i<m_arrIdNum.GetSize();i++) {
		if(max<int(m_arrIdNum[i])&&m_arrIdUser[i]==signature) max=m_arrIdNum[i];
	}
	m_arrIdNum.Add(max+1);
	m_arrIdKey.Add(signature+MFC_Util::ItoAscii(max+1));
	m_ArrayIndex=m_arrPlantType.GetSize()-1;


	m_file.SeekToBegin();
	CArchive ar(&m_file, CArchive::store);
	Serialize(ar);
// Save Sim archive
	Save();
	m_Data.OnSetup();
	OnInitDialog();

}
void PP_EDIT::OnButtonAdd() 
{
	UpdateData(TRUE);
	m_arrPlantType.Add(0);
	m_arrPlantName.Add(m_PlantName);
	m_arrSiteName.Add(m_SiteName);
	m_arrLatitude.Add(m_Latitude);
	m_arrLongitude.Add(m_Longitude);
	m_arrCountry.Add(m_Country); 
	m_arrDriveFile.Add(m_DriveFile); 
	CString str;
	m_Details.GetWindowText(str);
	CString signature="NN";
	CWinApp* pApp = AfxGetApp();
	signature=pApp->GetProfileString("CoupModel - KTH", "Signature", signature);
	m_arrIdUser.Add(signature); 
	
	CTime tt=CTime::GetCurrentTime();
	CString timest=tt.Format( "%Y-%m-%d %H:%M:%S" );
	str+="\r\nCopy of previous record made by "+signature+timest;
	m_arrDetails.Add(str); 

 
	int max=0;
	for(int i=0;i<m_arrIdNum.GetSize();i++) {
		if(max<int(m_arrIdNum[i])&&m_arrIdUser[i]==signature) max=m_arrIdNum[i];
	}
	m_arrIdNum.Add(max+1);
	m_arrIdKey.Add(signature+MFC_Util::ItoAscii(max+1));
	m_ArrayIndex=m_arrPlantType.GetSize()-1;
	if(m_ArrayIndex>=0) 
		m_SelectedKey=m_arrIdKey.GetAt(m_ArrayIndex);


/*	if(m_ID_DB_Group==m_pDoc->m_Doc.DB_GetNames(0)) { //Plant
 		m_pSwGen1->SetDBValue(m_GenSwitchControl1.GetCurSel());
		m_pSwGen2->SetDBValue(m_GenSwitchControl2.GetCurSel());
		m_pSwTimeRes->SetDBValue(m_TimeResolutionControl.GetCurSel());
 	}
	else {
 		m_pSwGen1->SetDBValue(m_GenSwitchControl1.GetCurSel());
		m_pSwTimeRes->SetDBValue(m_TimeResolutionControl.GetCurSel());
	}*/


// Save main archive
	m_file.SeekToBegin();
	CArchive ar(&m_file, CArchive::store);
	Serialize(ar);
// Save Sim archive
	Save();
	m_Data.OnSetup();
	OnInitDialog();

}


void PP_EDIT::OnButtonApply() 
{
	UpdateStrings();
	// Save Main archive
	m_file.SeekToBegin();
	CArchive ar(&m_file, CArchive::store);
	Serialize(ar);
// Save Sim-archive
	Save();
	m_SavedDataInRecord=TRUE;
	OnInitDialog();
}

void PP_EDIT::OnButtonDelete() 
{
	if(m_ArrayIndex>=0) {
		CString str, str2;
		BOOL yes;
		str2=m_arrIdKey[m_ArrayIndex];
		str=
		str="Do you really want to delete the record with key "+str2+" ?";
		yes=MessageBox(str,"File Open Error",MB_YESNO|MB_ICONQUESTION);
		if(yes=IDYES) {
			m_arrPlantType.RemoveAt(m_ArrayIndex);
			m_arrPlantName.RemoveAt(m_ArrayIndex);
			m_arrSiteName.RemoveAt(m_ArrayIndex);
			m_arrLatitude.RemoveAt(m_ArrayIndex); 
			m_arrLongitude.RemoveAt(m_ArrayIndex);
			m_arrCountry.RemoveAt(m_ArrayIndex);  
			m_arrDriveFile.RemoveAt(m_ArrayIndex);  
			m_arrDetails.RemoveAt(m_ArrayIndex);  
			DeleteFile();
			m_arrIdUser.RemoveAt(m_ArrayIndex);   
			m_arrIdNum.RemoveAt(m_ArrayIndex); 
			m_arrIdKey.RemoveAt(m_ArrayIndex);
			m_ArrayIndex--;
			m_Data.OnSetup();
		}
	}
	if(m_ArrayIndex>=0) 
		m_SelectedKey=m_arrIdKey.GetAt(m_ArrayIndex);


	OnInitDialog();
}











void PP_EDIT::Serialize(CArchive& ar) 
{
	int i, isize;
	if (ar.IsStoring())
	{  
		if(m_SysStore) {
			ar<<m_NumOriginalRecords;
			for(int i=0;i<m_NumOriginalRecords;i++) {
				ar<<m_arrPlantType[i];
				ar<<m_arrPlantName[i];
				ar<<m_arrSiteName[i];
				ar<<m_arrLatitude[i];
				ar<<m_arrLongitude[i];
				ar<<m_arrCountry[i];
				ar<<m_arrDriveFile[i];
				ar<<m_arrDetails[i];
				ar<<m_arrIdUser[i];
				ar<<m_arrIdNum[i];
			}

		}
		else {
			if(m_arrPlantType.GetSize()<=0) {
				m_arrPlantType.Add(m_PlantTypes.GetUpperBound());
				m_arrPlantName.Add("Not Specified");
				m_arrSiteName.Add("Not Specified");
				m_arrLatitude.Add("60.58N");
				m_arrLongitude.Add("15.00E");
				m_arrCountry.Add("SE"); 
				m_arrDriveFile.Add("");
				m_arrDetails.Add("Edit this text to include method of estimation and \r\nreferences to publications/reports. \r\nAdd any information which improves the understanding of \r\nthe representativity of the parameter values.\r\nDo not forget your full name."); 
				m_arrIdUser.Add("NN");  
				m_arrIdNum.Add(1);
				m_arrIdKey.Add("NN1");
			}
			ar<<m_arrPlantType.GetSize()-m_NumOriginalRecords;
			for(int i=m_NumOriginalRecords;i<m_arrPlantType.GetSize();i++) {
				ar<<m_arrPlantType[i];
				ar<<m_arrPlantName[i];
				ar<<m_arrSiteName[i];
				ar<<m_arrLatitude[i];
				ar<<m_arrLongitude[i];
				ar<<m_arrCountry[i];
				ar<<m_arrDriveFile[i];
				ar<<m_arrDetails[i];
				ar<<m_arrIdUser[i];
				ar<<m_arrIdNum[i];
			}
		}

	}
	else
	{	// loading code

		UINT ival;
		CString str;

		if(m_SysStore) {

		}
		else {


		i=m_NumOriginalRecords-1;	
		ar>>isize;
	
		TRY {
			for(int i=m_NumOriginalRecords;i<isize+m_NumOriginalRecords;i++) {
				if(!ar.IsBufferEmpty()) {
					ar>>ival;m_arrPlantType.SetAtGrow(i, ival); 
					ar>>str; m_arrPlantName.SetAtGrow(i,str); 
					ar>>str; m_arrSiteName.SetAtGrow(i,str); 
					ar>>str; m_arrLatitude.SetAtGrow(i,str);  
					ar>>str; m_arrLongitude.SetAtGrow(i,str); 
					ar>>str; m_arrCountry.SetAtGrow(i, str);   
					ar>>str; m_arrDriveFile.SetAtGrow(i, str);   
					ar>>str; m_arrDetails.SetAtGrow(i,str);   
					ar>>str; m_arrIdUser.SetAtGrow(i,str);
					ar>>ival; m_arrIdNum.SetAtGrow(i,ival);
					str+=MFC_Util::ItoAscii(ival);
					m_arrIdKey.SetAtGrow(i,str);
				}
		}
		}
		CATCH( CFileException, e )
		{
   		}

		END_CATCH
		}
	}
}

BOOL PP_EDIT::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString str;
	if(m_ReadOnly) {
		str="Select ";
		if(m_NumToSelect>1)
			str+= MFC_Util::ItoAscii(m_NumToSelect)+ " Records with ";
		else
			str+="a single Record with ";

	}
	else
		str="View and edit ";
	str+=m_ID_DB_Group+" characteristics";
	SetWindowText(str);


	if(m_ArrayIndex>=0) 
		m_Details.SetWindowText(m_arrDetails[m_ArrayIndex]);

	if(m_ArrayIndex<m_NumOriginalRecords||m_TotalReadOnly) 
		  m_ReadOnly=TRUE;
	else 
		m_ReadOnly=FALSE;

	if(m_ReadOnly) {
		((CEdit*)GetDlgItem(IDC_DETAILS))->SetReadOnly();
	
 		if(m_TotalReadOnly) {
			GetDlgItem(IDC_BUTTON_CREATE)->	ModifyStyle(0,WS_DISABLED);
		}
		GetDlgItem(IDC_BUTTON_APPLY)->	ModifyStyle(0,WS_DISABLED);
	}
	else {
//		m_ButtonsHeading="Press button below to edit different type of data";

		((CEdit*)GetDlgItem(IDC_DETAILS))->SetReadOnly(FALSE);
		GetDlgItem(IDC_BUTTON_APPLY)->	ModifyStyle(WS_DISABLED,0);

	}
	GetDlgItem(IDC_BUTTON_APPLY)->	RedrawWindow();
	GetDlgItem(IDC_BUTTON_CREATE)->	RedrawWindow();


	CDialog::OnInitDialog();



	if(!m_InitDialog){
		m_DatabasCUG.AttachGrid(this, IDC_DATABASECUG);
		m_DataBaseSelected.AttachGrid(this, IDC_DATABASE_TOSIM);
		m_Data.AttachGrid(this,IDC_DATAFROMBASE);
		m_DatabasCUG.OnSetup();
		m_InitDialog=TRUE;
	}


    m_DataBaseSelected.OnSetup();
	m_DatabasCUG.OnSetup();
	if(m_ArrayIndex>=0)
		m_Data.OnAdjustLeft(m_ArrayIndex);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void PP_EDIT::DeleteFile()
{
	CWinApp* pApp = AfxGetApp();

	m_SimFileName="DB_Plant_";
	m_SimFileName=pApp->GetProfileString("CoupModel - KTH","DataBaseDirectory",m_SimFileName);
	m_SimFileName+=m_ID_DB_Group+"\\DB_"+m_ID_DB_Group+"_";
	m_SimFileName+=m_arrIdUser[m_ArrayIndex]+MFC_Util::ItoAscii(m_arrIdNum[m_ArrayIndex]);
	m_SimFileName+=".Sim";

	TRY
	{
		CFile file(m_SimFileName, CFile::modeWrite);
	}
	CATCH( CFileException, e )
	{
    #ifdef _DEBUG
        afxDump << "File could not be opened " << e->m_cause << "\n";
    #endif
		return;
	}

	END_CATCH

	CFile file(m_SimFileName, CFile::modeWrite);
	file.Close();
	file.Remove(m_SimFileName);

}
void PP_EDIT::Reset()
{
	CString str, Group, Type, Name;
	vector<SimB*> vvp;
	Ps *pP;
	Sw* pSw;

			for(int i=0;i<m_arrGroups.GetSize();i++) {
					Group=m_arrGroups.GetAt(i);
					Type="Switches";
					vvp=m_pDoc->GetPtrVector(string(Type), string(Group));

						for(size_t ii=0;ii<vvp.size();ii++) {
							pSw=(Sw*)vvp[ii];
							int i1=pSw->GetOriginalIntValue();
							pSw->SetDBIntValue(i1);
						}

					Type="Parameters";
					vvp=m_pDoc->GetPtrVector(string(Type), string(Group));
						for(size_t ii=0;ii<vvp.size();ii++) {
							pP=(Ps*)vvp[ii];
							float val=pP->GetOriginalValue();
							pP->SetDBValue(val);
						}


			}

}

void PP_EDIT::Save()
{
	CWinApp* pApp = AfxGetApp();

	string key=m_arrIdUser[m_ArrayIndex]+MFC_Util::ItoAscii(m_arrIdNum[m_ArrayIndex]);
	m_pDoc->SaveTo_DB_Object(string(m_ID_DB_Group), key);
/*
	m_SimFileName="DB_Plant_";
	m_SimFileName=pApp->GetProfileString("CoupModel - KTH","DataBaseDirectory",m_SimFileName);
	m_SimFileName+=m_ID_DB_Group+"\\DB_"+m_ID_DB_Group+"_";
	if(m_ArrayIndex>=0) m_SimFileName+=m_arrIdUser[m_ArrayIndex]+MFC_Util::ItoAscii(m_arrIdNum[m_ArrayIndex]);
	m_SimFileName+=".Sim";

	TRY
	{
		CFile file(m_SimFileName, CFile::modeWrite);
	}
	CATCH( CFileException, e )
	{
    #ifdef _DEBUG
        afxDump << "File could not be opened " << e->m_cause << "\n";
    #endif
		CFile file(m_SimFileName,CFile::modeCreate|CFile::modeWrite);
	}

	END_CATCH

	CFile file(m_SimFileName, CFile::modeWrite);
	CArchive ar(&file, CArchive::store);
	DB_Plant *pPlant;

	pPlant= new DB_Plant("Model Structure",&m_pDoc->m_Doc);
	// REMAINpPlant->Serialize(ar);
	delete pPlant;

	for(int i=0;i<m_arrGroups.GetSize();i++) {
		pPlant= new DB_Plant(string(m_arrGroups[i]),&m_pDoc->m_Doc );
//REMAIN		pPlant->Serialize(ar);
		delete pPlant;
	}

	CString end="End";
	ar<<end;*/
}
void PP_EDIT::MoveSystemKey(int irec, BOOL system)
{
	CString str;
	int isize=m_arrPlantType.GetSize();

	if(system) {
		m_NumOriginalRecords--;
		int istore=m_arrPlantType.GetAt(irec);
		m_arrPlantType.RemoveAt(irec);
		m_arrPlantType.SetAtGrow(isize-1, istore);
		str=m_arrPlantName[irec];
		m_arrPlantName.RemoveAt(irec);
		m_arrPlantName.SetAtGrow(isize-1,str);
		str=m_arrSiteName[irec];
		m_arrSiteName.RemoveAt(irec);
		m_arrSiteName.SetAtGrow(isize-1,str);
		str=m_arrLatitude[irec];
		m_arrLatitude.RemoveAt(irec);
		m_arrLatitude.SetAtGrow(isize-1,str);
		str=m_arrLongitude[irec];
		m_arrLongitude.RemoveAt(irec);
		m_arrLongitude.SetAtGrow(isize-1,str);
		str=m_arrCountry[irec];
		m_arrCountry.RemoveAt(irec);
		m_arrCountry.SetAtGrow(isize-1,str);
		str=m_arrDriveFile[irec];
		m_arrDriveFile.RemoveAt(irec);
		m_arrDriveFile.SetAtGrow(isize-1,str);
		str=m_arrDetails[irec];
		m_arrDetails.RemoveAt(irec);
		m_arrDetails.SetAtGrow(isize-1,str);
		str=m_arrIdUser[irec];
		m_arrIdUser.RemoveAt(irec);
		m_arrIdUser.SetAtGrow(isize-1,str);
		istore=m_arrIdNum[irec];
		m_arrIdNum.RemoveAt(irec);
		m_arrIdNum.SetAtGrow(isize-1,istore);
		str=m_arrIdKey[irec];
		m_arrIdKey.RemoveAt(irec);
		m_arrIdKey.SetAtGrow(isize-1,str);

	}
	else {
		m_NumOriginalRecords++;
		int newpos=m_NumOriginalRecords-1;
		int istore=m_arrPlantType.GetAt(irec);
		m_arrPlantType.RemoveAt(irec);
		m_arrPlantType.InsertAt(newpos, istore);
		str=m_arrPlantName[irec];
		m_arrPlantName.RemoveAt(irec);
		m_arrPlantName.InsertAt(newpos,str);
		str=m_arrSiteName[irec];
		m_arrSiteName.RemoveAt(irec);
		m_arrSiteName.InsertAt(newpos,str);
		str=m_arrLatitude[irec];
		m_arrLatitude.RemoveAt(irec);
		m_arrLatitude.InsertAt(newpos,str);
		str=m_arrLongitude[irec];
		m_arrLongitude.RemoveAt(irec);
		m_arrLongitude.InsertAt(newpos,str);
		str=m_arrCountry[irec];
		m_arrCountry.RemoveAt(irec);
		m_arrCountry.InsertAt(newpos,str);
		str=m_arrDriveFile[irec];
		m_arrDriveFile.RemoveAt(irec);
		m_arrDriveFile.InsertAt(newpos,str);
		str=m_arrDetails[irec];
		m_arrDetails.RemoveAt(irec);
		m_arrDetails.InsertAt(newpos,str);
		str=m_arrIdUser[irec];
		m_arrIdUser.RemoveAt(irec);
		m_arrIdUser.InsertAt(newpos,str);
		istore=m_arrIdNum[irec];
		m_arrIdNum.RemoveAt(irec);
		m_arrIdNum.InsertAt(newpos,istore);
		str=m_arrIdKey[irec];
		m_arrIdKey.RemoveAt(irec);
		m_arrIdKey.InsertAt(newpos,str);
	}
	m_file.SeekToBegin();
	CArchive ar(&m_file, CArchive::store);
	Serialize(ar);

	m_SysFile.SeekToBegin();
	m_SysStore=TRUE;
	CArchive ar1(&m_SysFile, CArchive::store);

	Serialize(ar1);
	m_SysStore=FALSE;
	m_Data.OnSetup();
	OnInitDialog();
}

void PP_EDIT::SaveWithKey()
{
	CWinApp* pApp = AfxGetApp();

	
	string key=m_arrIdUser[m_ArrayIndex]+MFC_Util::ItoAscii(m_arrIdNum[m_ArrayIndex]);
	m_pDoc->SaveTo_DB_Object(string(m_ID_DB_Group), key);

/*

	CFile file(m_SimFileName, CFile::modeWrite|CFile::modeCreate);
	CArchive ar(&file, CArchive::store);
	
	DB_Plant *pPlant;


	for(int i=0;i<m_arrGroups.GetSize();i++) {
		pPlant= new DB_Plant(string(m_arrGroups[i]),&m_pDoc->m_Doc);
		//REMAIN pPlant->Serialize(ar);
		delete pPlant;
	}

	CString end="End";
	ar<<end;*/
}


void PP_EDIT::Retrieve()
{
	string key=m_arrIdUser[m_ArrayIndex]+MFC_Util::ItoAscii(m_arrIdNum[m_ArrayIndex]);
	m_pDoc->ReadFrom_DB_Object(string(m_ID_DB_Group), key);
	CWinApp* pApp = AfxGetApp();
	/*
	m_SimFileName="DB_Plant_";
	m_SimFileName=pApp->GetProfileString("CoupModel - KTH","DataBaseDirectory",m_SimFileName);
	m_SimFileName+=m_ID_DB_Group+"\\DB_"+m_ID_DB_Group+"_";
	m_SimFileName+=m_arrIdUser[m_ArrayIndex]+MFC_Util::ItoAscii(m_arrIdNum[m_ArrayIndex]);
	m_SimFileName+=".Sim";

	TRY
	{
		CFile file(m_SimFileName, CFile::modeRead);
	}
	CATCH( CFileException, e )
	{
    #ifdef _DEBUG
        afxDump << "File could not be opened " << e->m_cause << "\n";
    #endif
		CFile file(m_SimFileName,CFile::modeCreate);
	}

	END_CATCH

	CFile file(m_SimFileName, CFile::modeRead);

	DB_Plant wizsheet(string(m_ID_DB_Group),&m_pDoc->m_Doc);
	CArchive ar(&file, CArchive::load);
	file.SeekToBegin();

//REMAN	else
//REMAIN		wizsheet.Serialize(ar);*/
	

}
void PP_EDIT::Actual_DB()
{

	for(int i=0;i<m_arrGroups.GetSize();i++) {
		string group=m_arrGroups[i];
		if(group=="Multiple Plant") group="Plant Growth";
		m_pDoc->Actual_DB(group);
	}
	
/*	DB_Plant *pPlant;
	pPlant= new DB_Plant("Model Structure",&m_pDoc->m_Doc);
	pPlant->Actual_DB();
	delete pPlant;

	for(int i=0;i<m_arrGroups.GetSize();i++) {
		pPlant= new DB_Plant(string(m_arrGroups[i]),&m_pDoc->m_Doc);
		pPlant->Actual_DB();
		delete pPlant;
	}*/

}
void PP_EDIT::DB_Actual()
{

	m_pDoc->Apply_DB_Struc("Evaporation");
// The section below needs to be modified to only the relevant of
// all Model Structure switches.
	/*DB_Plant *pPlant;
	pPlant= new DB_Plant("Model Structure",&m_pDoc->m_Doc, string(m_SelectedKey));*/
	if(m_ID_DB_Group==m_pDoc->DB_GetNames(0).c_str()) {
		m_pDoc->Apply_DB_Struc("Evaporation");
		m_pDoc->Apply_DB_Struc("PlantType");
		m_pDoc->Apply_DB_Struc("TimeResolution");
		m_pDoc->Apply_DB_Struc("WaterEq");
		//pPlant->DB_Struc_Actual("Evaporation");
		//pPlant->DB_Struc_Actual("PlantType");
		//pPlant->DB_Struc_Actual("TimeResolution");
		//pPlant->DB_Struc_Actual("WaterEq");
	}
	else if(m_ID_DB_Group==m_pDoc->DB_GetNames(3).c_str()) {
		m_pDoc->Apply_DB_Struc("TimeResolution");
		m_pDoc->Apply_DB_Struc("SnowPack");
		//pPlant->DB_Struc_Actual("TimeResolution");
		//pPlant->DB_Struc_Actual("SnowPack");
	}
	else if(m_ID_DB_Group==m_pDoc->DB_GetNames(4).c_str()) {
		m_pDoc->Apply_DB_Struc("WaterEq");
		m_pDoc->Apply_DB_Struc("HeatEq");
		//pPlant->DB_Struc_Actual("WaterEq");
		//pPlant->DB_Struc_Actual("HeatEq");
	}
	else if(m_ID_DB_Group==m_pDoc->DB_GetNames(2).c_str()) {
		m_pDoc->Apply_DB_Struc("Evaporation");
		m_pDoc->Apply_DB_Struc("WaterEq");
		//pPlant->DB_Struc_Actual("Evaporation");
		//pPlant->DB_Struc_Actual("WaterEq");
	}
	else if(m_ID_DB_Group==m_pDoc->DB_GetNames(5).c_str()) {
		m_pDoc->Apply_DB_Struc("WaterEq");
		m_pDoc->Apply_DB_Struc("GroundWaterFlow");
		//pPlant->DB_Struc_Actual("WaterEq");
		//pPlant->DB_Struc_Actual("GroundWaterFlow");
	}
	else if(m_ID_DB_Group==m_pDoc->DB_GetNames(7).c_str()) {
		m_pDoc->Apply_DB_Struc("Evaporation");
		m_pDoc->Apply_DB_Struc("PlantType");
		//pPlant->DB_Struc_Actual("Evaporation");
		//pPlant->DB_Struc_Actual("PlantType");
	}
//	else if(m_ID_DB_Group=="Drainage and deep percolation")


	//delete pPlant;


	for(int i=0;i<m_arrGroups.GetSize();i++) {
		m_pDoc->Apply_DB_DocFile(string(m_arrGroups[i]),string(m_SelectedKey));
		//pPlant= new DB_Plant(string(m_arrGroups[i]),&m_pDoc->m_Doc, string(m_SelectedKey) );
		//pPlant->DB_Actual();
		//delete pPlant;
	}
		string choice;
		choice=m_PlantTypes[m_arrPlantType[m_ArrayIndex]]; choice+="\t";
		choice+=m_arrPlantName[m_ArrayIndex];choice+="\t";
		choice+=m_arrSiteName[m_ArrayIndex];choice+="\t";
		choice+=m_arrLongitude[m_ArrayIndex];choice+="/";
		choice+=m_arrLatitude[m_ArrayIndex];choice+="\t";
		choice+=m_arrCountry[m_ArrayIndex];
		if(m_Reset) choice=_T("");
//		DB_Actual();*/

		if(m_ID_DB_Group==m_pDoc->DB_GetNames(0).c_str()) 
			m_pDoc->DB_SetChoice(Plant_Hydrology, choice);
		else if(m_ID_DB_Group==m_pDoc->DB_GetNames(3).c_str())
			m_pDoc->DB_SetChoice(Snow, choice);
		else if(m_ID_DB_Group==m_pDoc->DB_GetNames(4).c_str())
			m_pDoc->DB_SetChoice(Soil_Freezing, choice);
		else if(m_ID_DB_Group==m_pDoc->DB_GetNames(2).c_str())
			m_pDoc->DB_SetChoice(Soil_Evaporation, choice);
		else if(m_ID_DB_Group==m_pDoc->DB_GetNames(5).c_str())
			m_pDoc->DB_SetChoice(Soil_Water_Flows, choice);
		else if(m_ID_DB_Group==m_pDoc->DB_GetNames(6).c_str())
			m_pDoc->DB_SetChoice(Plant_Growth, choice);
		else if(m_ID_DB_Group==m_pDoc->DB_GetNames(7).c_str())
			m_pDoc->DB_SetChoice(Soil_Manage, choice);
		else if(m_ID_DB_Group==m_pDoc->DB_GetNames(9).c_str())
			m_pDoc->DB_SetChoice(Soil_InOrganic, choice);
		else if(m_ID_DB_Group==m_pDoc->DB_GetNames(8).c_str())
			m_pDoc->DB_SetChoice(Soil_Organic, choice);




}


void PP_EDIT::OnFileView() 
{

	string name;
	name="DB_Plant_";
	name=MFC_Util::GetProfileStringStd("DataBaseDirectory",name);
	name+=m_ID_DB_Group+"\\"+m_DriveFile+".Bin";
	CPG	pgfile(name);
	
	
		
}
BOOL PP_EDIT::FileExist()
{
	CWinApp* pApp = AfxGetApp();
	string name;
	name="DB_Plant_";
	name=MFC_Util::GetProfileStringStd("DataBaseDirectory",name);
	name+=m_ID_DB_Group+"\\"+m_DriveFile+".Bin";
	CPG	pgfile(name);
	if(pgfile.FileExist())
		return TRUE;
	else
		return FALSE;
}

void PP_EDIT::OnBrowse() 
{
	CStdioFile driveFile;
	CString pszFileName ;
	LPCTSTR lpszDefExt;
	LPCTSTR lpszFilter;
	int fileDlgResult;

	CString extension;

	extension="bin";

//Create a read dialog
	lpszDefExt="bin";
	lpszFilter ="Driving Files (*.Bin)|*.Bin||";

 	CFileDialog	DriveFileDlg(TRUE,lpszDefExt,pszFileName, OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,lpszFilter);

	//Show the dialog
	fileDlgResult=DriveFileDlg.DoModal();

	//If the cancel button has been pushed then return
	if (fileDlgResult==IDCANCEL)
		return;
	//Retrieve the filename of the file that is to be opened
	pszFileName = DriveFileDlg.GetPathName();
	CPG drive;
	drive.Open(string(pszFileName));
	
	CString dbFileName=pszFileName.Mid(pszFileName.ReverseFind('\\')+1);
	CWinApp* pApp = AfxGetApp();

	CString name;
	name="DB_Plant_";
	name=pApp->GetProfileString("CoupModel - KTH","DataBaseDirectory",name);
	name+=m_ID_DB_Group+"\\"+dbFileName;

//	CFile dbFile(name,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
//	CArchive arw(&dbFile, CArchive::store);
	if(drive.SaveAs(string(name)))
		m_DriveFile_Exist=TRUE;
	else
		m_DriveFile_Exist=FALSE;

	int i=dbFileName.Find('.');
	m_DriveFile=dbFileName.Left(i);
	m_arrDriveFile.SetAt(m_ArrayIndex, m_DriveFile); 
	m_pFile->SetDBValue(string(m_DriveFile));
	OnInitDialog();
	
}

void PP_EDIT::UpdateStrings()
{
	UpdateData(TRUE);
	//m_arrPlantType.SetAt(m_ArrayIndex, m_PlantType.GetCurSel());

	CString str;
	m_Details.GetWindowText(str);
	m_arrDetails.SetAt(m_ArrayIndex, str); 
	if(!m_ReadOnly) m_SavedDataInRecord=FALSE;

/*	if(m_ID_DB_Group==m_pDoc->DB_GetNames(0)) {
 		m_pSwGen1->SetDBValue(m_GenSwitchControl1.GetCurSel());
		m_pSwGen2->SetDBValue(m_GenSwitchControl2.GetCurSel());
		m_pSwTimeRes->SetDBValue(m_TimeResolutionControl.GetCurSel());
 	}
	else {
 		m_pSwGen1->SetDBValue(m_GenSwitchControl1.GetCurSel());
		m_pSwTimeRes->SetDBValue(m_TimeResolutionControl.GetCurSel());
	}*/

}

void PP_EDIT::DeleteKeyFile()
{

	CFile file(m_SimFileName, CFile::modeReadWrite);
	file.Close();
	file.Remove(m_SimFileName);

}

void PP_EDIT::OnReset() 
{
	// TODO: Add your control notification handler code here
	if(!m_ReadOnly) {
		m_file.SeekToBegin();
		CArchive ar(&m_file, CArchive::store);
		Serialize(ar);
// Save Sim-archive
		Save();
	}
	if(m_ReadOnly) {
		m_SelectedId=_T("");
		m_ArrayIndex=-1;
		m_Reset=TRUE;
		m_SelectedKey=_T("");

	}
	CDialog::OnOK();
	m_file.Close();

	
}

void PP_EDIT::OnBnClickedResetSelection()
{
	if(m_pCDB!=NULL) {
		m_pCDB->MR_ResetSelection();
		OnInitDialog();

	}
	// TODO: Add your control notification handler code here
}
