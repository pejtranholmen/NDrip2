// NewPGVariables.cpp : implementation file
//

#include "../stdafx.h"
#include "../Util.h"
#include "../CoupModel.h"
#include "./NewPGVariables.h"
#include "../ModelTypes\SimB.h"

/*
extern "C" bool GENFUNCTIONS_mp_CASUN_INIT(float *);
extern "C" bool GENFUNCTIONS_mp_SUNDAYINIT(float *);
extern "C" float GENFUNCTIONS_mp_DAYLENGTHFUNCTION(float *);
extern "C" float GENFUNCTIONS_mp_POTRISHOUR(float *);
extern "C" float GENFUNCTIONS_mp_POTRISDAILY(float *);
extern "C" float RADIATION_mp_ANGSTROM(float *);*/

// NewPGVariables dialog

IMPLEMENT_DYNAMIC(NewPGVariables, CDialog)

NewPGVariables::NewPGVariables(CPG *pX, CWnd* pParent /*=nullptr*/)
	: CDialog(NewPGVariables::IDD, pParent)
{
	m_Init=TRUE;
	m_pOPG=(CPG*)pX;
	m_pPG_Created=nullptr;
	m_NewFileName="NewFileName.Bin";
	m_AddedVar=0;
	m_initlat=60.;

	m_FuncChoiceValue=0;
	m_FuncTypeValue=0;
	m_FuncCategoryValue=0;

	m_CalcInfo.m_CalcFunc=0;
	m_CalcInfo.m_NumParCalcFunc=1;
	
	m_CalcInfo.m_CalcFuncPar[0]="Latitude";
	m_CalcInfo.m_ParValue[0]=m_initlat;


	m_CalcInfo.Init(4,m_pOPG, &m_gridDes);
	m_StoredValue=0.;
	m_gridDes.SetpPG(m_pOPG);



}

NewPGVariables::~NewPGVariables()
{
}

void NewPGVariables::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_VarChoice);
	DDX_Control(pDX, IDC_COMBO2, m_FunctionType);
	DDX_Control(pDX, IDC_COMBO3, m_FuncCategory);
}


BEGIN_MESSAGE_MAP(NewPGVariables, CDialog)
	ON_BN_CLICKED(IDOK, &NewPGVariables::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &NewPGVariables::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON3, &NewPGVariables::OnBnClickedSaveAs)
	ON_BN_CLICKED(IDC_BUTTONADD, &NewPGVariables::OnBnClickedButtonadd)
	ON_CBN_SELCHANGE(IDC_COMBO2, &NewPGVariables::OnCbnSelchangeFuncType)
	ON_CBN_SELCHANGE(IDC_COMBO1, &NewPGVariables::OnCbnSelchangeFunction)
	ON_CBN_SELCHANGE(IDC_COMBO3, &NewPGVariables::OnCbnSelchangeFuncCategory)

END_MESSAGE_MAP()


// NewPGVariables message handlers

BOOL NewPGVariables::OnInitDialog() {
	


	if(m_pOPG!=nullptr&&m_Init) {
		CString Str;
		GetWindowText(Str);
		Str="Add New Variables to current file based on various functions: ";
		Str+=m_pOPG->GetFileName().c_str();
		SetWindowText(Str);



	}




	CDialog::OnInitDialog();






	if(m_Init) {

		m_gridDes.AttachGrid(this, IDC_GRID3);
		m_CalcInfo.AttachGrid(this, IDC_GRID2);
		m_gridDesNew.AttachGrid(this, IDC_GRID);
		m_gridDes.SetpPG(m_pOPG);

		m_gridDes.OnUpdate(6,m_pOPG->GetNumVariables(),FALSE, TRUE);
		m_gridDes.OnReadFileDescriptionToGrid(m_pOPG);
		m_gridDes.SetpCUG(&m_CalcInfo);



		m_CalcInfo.Init(4, m_pOPG, &m_gridDes);


		m_Init=FALSE;
		m_FunctionType.Clear();
		m_FunctionType.ResetContent();
		m_FunctionType.AddString(_T("Time only"));
		m_FunctionType.AddString(_T("Time and Variable value Dependence"));
		m_FunctionType.AddString(_T("Variable value dependence"));
		m_FunctionType.SetCurSel(m_FuncTypeValue);

		m_FuncCategory.Clear();
		m_FuncCategory.ResetContent();
		m_FuncCategory.AddString(_T("Sunshine Radiation"));
		m_FuncCategory.SetCurSel(m_FuncCategoryValue);

		m_VarChoice.Clear();
		m_VarChoice.ResetContent();
		m_VarChoice.AddString(_T("Global Radiation"));
		m_VarChoice.SetCurSel(m_FuncChoiceValue);
		m_FuncChoiceValue=m_CalcInfo.m_CalcFunc=m_VarChoice.GetCurSel();
		m_VarChoice.GetLBText(m_CalcInfo.m_CalcFunc,m_CalcInfo.m_CalcFuncName);


	}

	if(m_pPG_Created!=nullptr) {
			m_gridDesNew.OnUpdate(6,m_pPG_Created->GetNumVariables(),FALSE, TRUE);
			m_gridDesNew.OnReadFileDescriptionToGrid(m_pPG_Created);
	}

	if(m_FuncTypeValue==1) {
	// Time dependence

		m_CalcInfo.m_VarSelectCurrent=0;
		m_FuncCategory.Clear();
		m_FuncCategory.ResetContent();
		m_FuncCategory.AddString(_T("Sunshine Radiation"));
		m_FuncCategory.SetCurSel(m_FuncCategoryValue);

		switch (m_FuncCategoryValue) {
			case 0:
			m_VarChoice.Clear();
			m_VarChoice.ResetContent();
			m_VarChoice.AddString(_T("Global Radiation"));
			m_VarChoice.AddString(_T("Daily sum of Global Radiation"));
			m_VarChoice.AddString(_T("Day Length"));
			m_VarChoice.SetCurSel(m_FuncChoiceValue);
			m_FuncChoiceValue=m_CalcInfo.m_CalcFunc=m_VarChoice.GetCurSel();
			m_VarChoice.GetLBText(m_CalcInfo.m_CalcFunc,m_CalcInfo.m_CalcFuncName);
			switch(m_FuncChoiceValue) {
				case 0:	 // Daily Glob Rad
					m_CalcInfo.m_CalcFuncUnit="J/m2/day";
					m_CalcInfo.m_NumParCalcFunc=1;
					m_CalcInfo.m_CalcFuncPar[0]="Latitude (N decimal format): ";
					m_CalcInfo.m_ParValue[0]=m_initlat;
					break;
				case 1: // Day Length
					m_CalcInfo.m_CalcFuncUnit="minutes";
					m_CalcInfo.m_NumParCalcFunc=1;
					m_CalcInfo.m_CalcFuncPar[0]="Latitude (N decimal format): ";
					m_CalcInfo.m_ParValue[0]=m_initlat;
					break;
				case 2:	  // Glob Rad
					m_CalcInfo.m_CalcFuncUnit="J/m2/day";
					m_CalcInfo.m_NumParCalcFunc=1;
					m_CalcInfo.m_CalcFuncPar[0]="Latitude (N decimal format): ";
					m_CalcInfo.m_ParValue[0]=m_initlat;
					break;
			}
			break;
	


		}



	}
	else if(m_FuncTypeValue==0) {
	// Time dependence and variable value dependence

		m_CalcInfo.m_VarSelectCurrent=1;
		m_FuncCategory.Clear();
		m_FuncCategory.ResetContent();
		m_FuncCategory.AddString(_T("Sunshine Radiation"));
		m_FuncCategory.SetCurSel(m_FuncCategoryValue);

		switch (m_FuncCategoryValue) {
			case 0:
			m_VarChoice.Clear();
			m_VarChoice.ResetContent();
			m_VarChoice.AddString(_T("Global Radiation"));
			m_VarChoice.AddString(_T("Net Global Radiation"));
			m_VarChoice.AddString(_T("Cloudiness"));
			m_VarChoice.AddString(_T("Daily sum of Global Radiation"));
			m_VarChoice.SetCurSel(m_FuncChoiceValue);
			m_FuncChoiceValue=m_CalcInfo.m_CalcFunc=m_VarChoice.GetCurSel();
			m_VarChoice.GetLBText(m_CalcInfo.m_CalcFunc,m_CalcInfo.m_CalcFuncName);
			switch(m_FuncChoiceValue) {
				case 2:	 // Glob Rad as function of cloudiness
					m_CalcInfo.m_CalcFuncUnit="J/m2/day";
					m_CalcInfo.m_NumParCalcFunc=3;
					m_CalcInfo.m_CalcFuncPar[0]="Latitude (N decimal format): ";
					m_CalcInfo.m_ParValue[0]=m_initlat;
					m_CalcInfo.m_VarValueChoice[0]=1;
					m_CalcInfo.m_CalcFuncPar[1]="Cloudiness (0-1):";
					m_CalcInfo.m_ParValue[1]=0.;
					m_CalcInfo.m_VarValueChoice[1]=0;
					m_CalcInfo.m_VarSelect[1]=1;
					m_CalcInfo.m_CalcFuncPar[2]="Solar Time Adjust (minutes):";
					m_CalcInfo.m_ParValue[2]=0.;
					m_CalcInfo.m_VarValueChoice[2]=1;
					m_CalcInfo.m_VarSelect[2]=1;
					break;
				case 3: // Net Glob Rad as function of cloudiness and albedo
					m_CalcInfo.m_CalcFuncUnit="J/m2/day";
					m_CalcInfo.m_NumParCalcFunc=4;
					m_CalcInfo.m_CalcFuncPar[0]="Latitude (N decimal format): ";
					m_CalcInfo.m_VarValueChoice[0]=1;
					m_CalcInfo.m_ParValue[0]=m_initlat;
					m_CalcInfo.m_CalcFuncPar[1]="Cloudiness (0-1):";
					m_CalcInfo.m_ParValue[1]=0.;
					m_CalcInfo.m_CalcFuncPar[2]="Albedo (0-1):";
					m_CalcInfo.m_ParValue[2]=0.;
					m_CalcInfo.m_VarValueChoice[2]=1;
					m_CalcInfo.m_VarSelect[2]=1;
					m_CalcInfo.m_CalcFuncPar[3]="Solar Time Adjust (minutes):";
					m_CalcInfo.m_ParValue[3]=0.;
					m_CalcInfo.m_VarValueChoice[3]=1;
					m_CalcInfo.m_VarSelect[3]=1;
					break;
				case 0:	 // Cloudiness as function of global radiation
					m_CalcInfo.m_CalcFuncUnit="-";
					m_CalcInfo.m_NumParCalcFunc=3;
					m_CalcInfo.m_CalcFuncPar[0]="Latitude (N decimal format): ";
					m_CalcInfo.m_ParValue[0]=m_initlat;
					m_CalcInfo.m_VarValueChoice[0]=1;
					m_CalcInfo.m_CalcFuncPar[1]="Global Radiation (J/m2/day):";
					m_CalcInfo.m_ParValue[1]=0.;
					m_CalcInfo.m_VarValueChoice[1]=0;
					m_CalcInfo.m_VarSelect[1]=1;
					m_CalcInfo.m_CalcFuncPar[2]="Solar Time Adjust (minutes):";
					m_CalcInfo.m_ParValue[2]=0.;
					m_CalcInfo.m_VarValueChoice[2]=1;
					m_CalcInfo.m_VarSelect[2]=1;
					break;
				case 1:	 // Daily sum of Glob Rad as function of cloudiness
					m_CalcInfo.m_CalcFuncUnit="J/m2/day";
					m_CalcInfo.m_NumParCalcFunc=2;
					m_CalcInfo.m_CalcFuncPar[0]="Latitude (N decimal format): ";
					m_CalcInfo.m_ParValue[0]=m_initlat;
					m_CalcInfo.m_VarValueChoice[0]=1;
					m_CalcInfo.m_CalcFuncPar[1]="Cloudiness (0-1):";
					m_CalcInfo.m_ParValue[1]=0.;
					m_CalcInfo.m_VarValueChoice[1]=0;
					m_CalcInfo.m_VarSelect[1]=1;
					break;
			}
			break;
	


		}



	}
	else {
		// Variable dependence onlye


		m_FuncCategory.Clear();
		m_FuncCategory.ResetContent();
		m_FuncCategory.AddString(_T("Meteorological"));
		m_FuncCategory.AddString(_T("Longwave Radiation"));
		m_FuncCategory.SetCurSel(m_FuncCategoryValue);
		m_CalcInfo.m_VarSelectCurrent=1;


	 //Variable dependence
		switch (m_FuncCategoryValue) {

			case 0:
			m_VarChoice.Clear();
			m_VarChoice.ResetContent();
			m_VarChoice.AddString(_T("In Long Radiation"));
			m_VarChoice.AddString(_T("Net Long Radiation"));
			m_VarChoice.AddString(_T("Out Long Radiation"));
			m_VarChoice.SetCurSel(m_FuncChoiceValue);
			m_FuncChoiceValue=m_CalcInfo.m_CalcFunc=m_VarChoice.GetCurSel();
			m_VarChoice.GetLBText(m_CalcInfo.m_CalcFunc,m_CalcInfo.m_CalcFuncName);


			switch(m_FuncChoiceValue) {
				case 0:
				case 1:
					m_CalcInfo.m_CalcFuncUnit="J/m2/day";
					m_CalcInfo.m_NumParCalcFunc=3;	
					m_CalcInfo.m_CalcFuncPar[0]="Temperature (";
					m_CalcInfo.m_CalcFuncPar[0].AppendChar(CHAR(176));
					m_CalcInfo.m_CalcFuncPar[0].Append(_T("C)"));
					m_CalcInfo.m_VarSelect[0]=1;
					m_CalcInfo.m_ParValue[0]=1;
					m_CalcInfo.m_VarValueChoice[0]=0;
					m_CalcInfo.m_CalcFuncPar[1]="Vapour Pressure (Pa)";
					m_CalcInfo.m_VarSelect[1]=1;
					m_CalcInfo.m_ParValue[1]=1;
					m_CalcInfo.m_VarValueChoice[1]=0;
					m_CalcInfo.m_CalcFuncPar[2]="Cloudiness (0-1)";
					m_CalcInfo.m_VarSelect[2]=1;
					m_CalcInfo.m_ParValue[2]=1;
					m_CalcInfo.m_VarValueChoice[2]=0;
					break;
			

				case 2:
					m_CalcInfo.m_CalcFuncUnit="J/m2/day";
					m_CalcInfo.m_NumParCalcFunc=1;	
					m_CalcInfo.m_CalcFuncPar[0]="Temperature (";
					m_CalcInfo.m_CalcFuncPar[0].AppendChar(CHAR(176));
					m_CalcInfo.m_CalcFuncPar[0].Append(_T("C)"));
					m_CalcInfo.m_VarSelect[0]=1;
					m_CalcInfo.m_ParValue[0]=1;
					m_CalcInfo.m_VarValueChoice[0]=0;
					break;
			}
			break;
			case 1:
			m_VarChoice.Clear();
			m_VarChoice.ResetContent();
			m_VarChoice.AddString(_T("Sat vap pressure"));
			m_VarChoice.AddString(_T("Slope Sat vap pressure"));
			m_VarChoice.AddString(_T("Vapour pressure"));
			m_VarChoice.AddString(_T("Relative humidity"));
			m_VarChoice.SetCurSel(m_FuncChoiceValue);
			m_FuncChoiceValue=m_CalcInfo.m_CalcFunc=m_VarChoice.GetCurSel();
			m_VarChoice.GetLBText(m_CalcInfo.m_CalcFunc,m_CalcInfo.m_CalcFuncName);


			switch(m_FuncChoiceValue) {
				case 0:
					m_CalcInfo.m_CalcFuncUnit="%";
					m_CalcInfo.m_NumParCalcFunc=2;	
					m_CalcInfo.m_CalcFuncPar[0]="Temperature (";
					m_CalcInfo.m_CalcFuncPar[0].AppendChar(CHAR(176));
					m_CalcInfo.m_CalcFuncPar[0].Append(_T("C)"));
					m_CalcInfo.m_VarSelect[0]=1;
					m_CalcInfo.m_CalcFuncPar[1]="Vapour pressure (Pa)";
					m_CalcInfo.m_VarSelect[1]=1;
					break;
				case 1:
					m_CalcInfo.m_CalcFuncUnit="Pa";
					m_CalcInfo.m_NumParCalcFunc=1;	
					m_CalcInfo.m_CalcFuncPar[0]="Temperature (";
					m_CalcInfo.m_CalcFuncPar[0].AppendChar(CHAR(176));
					m_CalcInfo.m_CalcFuncPar[0].Append(_T("C)"));
					m_CalcInfo.m_VarSelect[0]=1;
					break;
				case 2:
					m_CalcInfo.m_CalcFuncUnit="Pa/";
					m_CalcInfo.m_CalcFuncUnit.AppendChar(CHAR(176));
					m_CalcInfo.m_CalcFuncUnit.Append(_T("C"));
					m_CalcInfo.m_NumParCalcFunc=1;	
					m_CalcInfo.m_CalcFuncPar[0]="Variable No:";
					m_CalcInfo.m_VarSelect[0]=1;	
					break;
				case 3:
					m_CalcInfo.m_CalcFuncUnit="Pa";
					m_CalcInfo.m_NumParCalcFunc=2;	
					m_CalcInfo.m_CalcFuncPar[0]="Temperature (";
					m_CalcInfo.m_CalcFuncPar[0].AppendChar(CHAR(176));
					m_CalcInfo.m_CalcFuncPar[0].Append(_T("C)"));
					m_CalcInfo.m_VarSelect[0]=1;
					m_CalcInfo.m_CalcFuncPar[1]="Relative humidity (%)";
					m_CalcInfo.m_VarSelect[1]=1;
					break;
			}
			break;
			
		}



	}
	m_CalcInfo.OnSetup();

	/*if(m_AddedVar>0){
		GetDlgItem(IDC_BUTTONADD)->ModifyStyle(WS_DISABLED,0);
		}
	else {
		GetDlgItem(IDC_BUTTONADD)->ModifyStyle(0, WS_DISABLED);
	}*/
	

	if(m_AddedVar>0){
		GetDlgItem(IDC_BUTTON3)->ModifyStyle(WS_DISABLED,0);
		}
	else {
		GetDlgItem(IDC_BUTTON3)->ModifyStyle(0, WS_DISABLED);
	}


	GetDlgItem(IDC_BUTTONADD)->RedrawWindow();
	GetDlgItem(IDC_BUTTON3)->RedrawWindow();




	return TRUE;
}

void NewPGVariables::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//m_pOPG->ReOpen();	
	m_pOPG->CloseFile();
	m_pOPG->SetCompleteRead(FALSE);
	if(m_AddedVar==0) 
		OnCancel();
	else
		OnOK();
}

void NewPGVariables::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	if(m_AddedVar>0) {
		m_pPG_Created->RemoveFile();
	}
	OnCancel();
}

void NewPGVariables::OnBnClickedSaveAs()
{
	// TODO: Add your control notification handler code here
		int fileDlgResult;
		CString pszFileName ;
		LPCTSTR lpszDefExt=_T("bin");
		CString szFilter= _T("PG Bin file (*.BIN)|*.BIN||");
		CFileDialog	BinFileDlg(FALSE,lpszDefExt,_T("NewFileName.BIN"),OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,szFilter);
		fileDlgResult=BinFileDlg.DoModal();
		if(fileDlgResult==IDOK) {
			pszFileName = BinFileDlg.GetPathName();
			m_pPG_Created->SetNewFileName(CT2A(pszFileName).m_psz);
			m_NewFileName = pszFileName;
		}

}

void NewPGVariables::OnBnClickedButtonadd()
{
	// Here calculations should be made
	// TODO: Add your control notification handler code here
	if(m_pPG_Created==nullptr) {

		int fileDlgResult;
		CString pszFileName ;
		LPCTSTR lpszDefExt=_T("bin");
		CString szFilter = _T("PG Bin file (*.BIN)|*.BIN||");
		CFileDialog	BinFileDlg(FALSE,lpszDefExt=_T("bin"),_T("NewFileName.BIN"),OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,szFilter);
		fileDlgResult=BinFileDlg.DoModal();
		if(fileDlgResult==IDOK)
			m_NewFileName = BinFileDlg.GetPathName();


		// Create a new structure
		m_pPG_Created= new CPG(CT2A(m_NewFileName).m_psz);
		m_AddedVar=1;
		m_pPG_Created->SetCompleteRead(false);
		m_pPG_Created->CreateNewFile(m_AddedVar,m_pOPG->GetNumRecords());
		if(!m_pPG_Created->SaveAs(CT2A(m_NewFileName).m_psz, TRUE))
			return;
		
		if (!m_pPG_Created->ReOpen(true)) {
			m_pPG_Created->CloseFile();
			m_pPG_Created->ReOpen(true);
		}

		if(!m_pOPG->ReOpen()){
			m_pOPG->CloseFile();	
			m_pOPG->ReOpen();					
		};
		m_gridDesNew.SetpPG(m_pPG_Created);


	}
	else {
		m_AddedVar++;
		m_pOPG->ReOpen();
	}



	float value, vmin, vmax;
	double day=1, hr;
	float v[5];
	vmax=-1.E38f;
	vmin=1.E38f;
	CPG *pPGN=nullptr;
	if(m_AddedVar>1) {
		pPGN = new CPG;
		pPGN->SetCompleteRead(FALSE);
		pPGN->Open(CT2A(m_NewFileName).m_psz, TRUE);
		m_pPG_Created->CreateNewFile(m_AddedVar,m_pOPG->GetNumRecords());
		m_pPG_Created->SaveAs("Temp.Bin", TRUE);
	}

	//	m_PhysFunc.CaSun_Init(m_initlat);
	//	m_PhysFunc.SunDayInit(day);
		value = MISSING;
		for (size_t i=1; i<=m_pOPG->GetNumRecords();i++) {
			if(m_FuncTypeValue==1) {
				day=float(m_pOPG->GetDayNumber(i));
				hr=float(m_pOPG->GetMinuteNumber(i))/60;
				value=float(TimeFunc(day, hr));
			}
			else if(m_FuncTypeValue==0) {
				day=float(m_pOPG->GetDayNumber(i));
				hr=float(m_pOPG->GetMinuteNumber(i))/60;
				for (int ip=0; ip<m_CalcInfo.m_NumParCalcFunc;ip++) {
					if(m_CalcInfo.m_VarValueChoice[ip]==0)
						v[ip]=m_pOPG->GetVarValue(m_CalcInfo.m_VarSelect[ip] , i);
					else
						v[ip]=m_CalcInfo.m_ParValue[ip];
				}
				value=float(Time_VarFunc(day, hr, &v[0]));

			}
			else if(m_FuncTypeValue==2) {
				for (int ip=0; ip<m_CalcInfo.m_NumParCalcFunc;ip++) {
					if(m_CalcInfo.m_VarValueChoice[ip]==0)
						v[ip]=m_pOPG->GetVarValue(m_CalcInfo.m_VarSelect[ip] , i);
					else
						v[ip]=m_CalcInfo.m_ParValue[ip];
				}
				value=VariableValueFunc(&v[0]);

			}
			float *pvar;
			pvar=(float*)m_pPG_Created->GetRecordPointer(i);
			pvar=(float*)m_pOPG->GetRecordPointer(i);
			*(pvar+1)=value;
			if(value<vmin)
				vmin=value;
			if(value>vmax)
				vmax=value;
			if(m_AddedVar>1) {
				float *pvarprev=(float*)pPGN->GetRecordPointer(i);
				m_pPG_Created->SetRecordPointer(pvarprev);
				m_pPG_Created->SetRecordPointer(pvar, m_AddedVar-1, 1);
			}
			else 
				m_pPG_Created->SetRecordPointer(pvar);	

			//m_pPG_Created->WriteNewPGFileRecord();
			if (!m_pPG_Created->WritePGFileRecord(i)) {
				m_pPG_Created->ReOpen(true);
				m_pPG_Created->WritePGFileRecord(i);
			};
			pvar=(float*)m_pPG_Created->GetRecordPointer(i);

		}







		m_pPG_Created->SetVarMin(m_AddedVar, vmin);
		m_pPG_Created->SetVarMax(m_AddedVar, vmax);
		m_pPG_Created->SetVarName(m_AddedVar,CT2A(m_CalcInfo.m_CalcFuncName).m_psz);
		m_pPG_Created->SetVarUnit(m_AddedVar,CT2A(m_CalcInfo.m_CalcFuncUnit).m_psz);
		
		if(m_CalcInfo.m_VarValueChoice[0]==1)
			m_pPG_Created->SetVarId(m_AddedVar,CT2A(m_CalcInfo.m_CalcFuncPar[0]+": "+MFC_Util::FtoAscii( m_CalcInfo.m_ParValue[0])).m_psz);
		else
			m_pPG_Created->SetVarId(m_AddedVar,CT2A(m_CalcInfo.m_CalcFuncPar[0]).m_psz);

		if(m_AddedVar>1) {
			for (int i=0;i<m_AddedVar-1;i++) {
				m_pPG_Created->SetVarMin(i+1, pPGN->GetVarMin(i+1));
				m_pPG_Created->SetVarMax(i+1, pPGN->GetVarMax(i+1));
				m_pPG_Created->SetVarName(i+1,pPGN->GetVarName(i+1));
				m_pPG_Created->SetVarUnit(i+1,pPGN->GetVarUnit(i+1));
				m_pPG_Created->SetVarId(i+1,pPGN->GetVarId(i+1));
			}
		}

		if(m_AddedVar>1) {
				pPGN->CloseFile();
				pPGN->RemoveFile();
				pPGN->Reset();
				delete pPGN;
		}
		m_pPG_Created->WritePGFileHeader(m_pOPG->GetNumRecords()) ;
		m_pPG_Created->CloseFile();
		if(m_AddedVar>1)
			m_pPG_Created->SetNewFileName(CT2A(m_NewFileName).m_psz);
		
		m_pOPG->CloseFile();

		m_gridDesNew.OnUpdate(6,m_pPG_Created->GetNumVariables(),FALSE, TRUE);
		m_gridDesNew.OnReadFileDescriptionToGrid(m_pPG_Created);



	if(m_AddedVar>0){
		GetDlgItem(IDC_BUTTON3)->ModifyStyle(WS_DISABLED,0);
		}
	else {
		GetDlgItem(IDC_BUTTON3)->ModifyStyle(0, WS_DISABLED);
	}
	GetDlgItem(IDC_BUTTON3)->RedrawWindow();

}

void NewPGVariables::OnCbnSelchangeFuncType()
{
	// TODO: Add your control notification handler code here

	m_FuncTypeValue=m_FunctionType.GetCurSel();
	m_FuncChoiceValue=0;
	m_FuncCategoryValue=0;
	OnInitDialog();
}

void NewPGVariables::OnCbnSelchangeFunction()
{
	m_FuncChoiceValue=m_CalcInfo.m_CalcFunc=m_VarChoice.GetCurSel();
	m_VarChoice.GetLBText(m_CalcInfo.m_CalcFunc,m_CalcInfo.m_CalcFuncName);

	OnInitDialog();
	// TODO: Add your control notification handler code here
}
void NewPGVariables::OnCbnSelchangeFuncCategory()
{
	// TODO: Add your control notification handler code here

	m_FuncCategoryValue=m_FuncCategory.GetCurSel();
	m_FuncChoiceValue=0;
	OnInitDialog();
}
double NewPGVariables::TimeFunc(double day, double hr)
{
	float value=MISSING;
/*	// TODO: Add your control notification handler code here
			if(m_CalcInfo.m_CalcFunc==2) {
				m_PhysFunc.SunDayInit(day);
				
				value=float(m_PhysFunc.PotRisHour(hr));
			}
			else if( m_CalcInfo.m_CalcFunc==0) {
				value=float(m_PhysFunc.PotRisDaily(day));			
			}
			else if( m_CalcInfo.m_CalcFunc==1) {				
				  value=float(m_PhysFunc.DayLengthFunction(day));
			}
			else if(m_CalcInfo.m_CalcFunc==3) {

			}*/
	return value;
}
double NewPGVariables::Time_VarFunc(double day, double hr, float *v)
{
	double value;
	double relsun;
	value=*v;
	
/*
	// TODO: Add your control notification handler code here
			if(m_CalcInfo.m_CalcFunc==0) {
			   //Cloudiness as function of global radiation
				m_PhysFunc.CaSun_Init(*v);
				m_PhysFunc.SunDayInit(day);

				if(m_CalcInfo.m_CalcFunc==2)
					hr=hr+*(v+2)/60.;
				value=m_PhysFunc.PotRisHour(hr);
				float radmax, radmin;
				float f;
				f=0.;
				radmax=float(m_RadFunc.f_Angstrom(f)*value);
				f=1.;
				radmin=float(m_RadFunc.f_Angstrom(f)*value);
				if(radmax>.5E6) {
					value=1.-*(v+1)/(radmax-radmin);
					if(value>1)
						value=1.;
					else if(value<0.)
						value=0.;
					m_StoredValue=float(value);
				}
				else
					value=m_StoredValue;


			}		
			else if(m_CalcInfo.m_CalcFunc<=3) {
				//Radiation as function of cloudiness
				m_PhysFunc.CaSun_Init(*v);
				m_PhysFunc.SunDayInit(day);
				//GENFUNCTIONS_mp_CASUN_INIT(v);
				//GENFUNCTIONS_mp_SUNDAYINIT(&day);
				
				if(m_CalcInfo.m_CalcFunc==3)
					hr=hr+*(v+3)/60.;
				else if(m_CalcInfo.m_CalcFunc==2)
					hr=hr+*(v+2)/60.;		
				else if(m_CalcInfo.m_CalcFunc==1)
					hr=0;
				if(m_CalcInfo.m_CalcFunc==1) {
					value=m_PhysFunc.PotRisDaily(day);

				}
				else {

					value=m_PhysFunc.PotRisHour(hr);
	
				}
				relsun=1-*(v+1);
				value=m_RadFunc.f_Angstrom(relsun)*value;
				
				if(m_CalcInfo.m_CalcFunc==3) {
					value=(1-*(v+2))*value;

				}

			}
*/
	return value;
}
/*extern "C" float GENFUNCTIONS_mp_SVP(float *);
extern "C" float GENFUNCTIONS_mp_DSVP(float *);
extern "C" float RADIATION_mp_RADLONGIN(float *,  float *,float *);
extern "C" float RADIATION_mp_RADLONGOUT(float *);
extern "C" float RADIATION_mp_RADNETLONG(float *,float *, float *);*/
float NewPGVariables::VariableValueFunc(float *v)
{
	float value=MISSING;
/*	switch (m_FuncCategoryValue) {
		case 0:  //Radiation Functions
			switch (m_FuncChoiceValue) {
				case 0:
					value=float(m_RadFunc.f_RadLongIn_Konz(*v, *(v+2),*(v+1)));
					break;
				case 1:
					value=float(m_RadFunc.f_RadNetLong_Air(*v,*(v+1),*(v+2)));
					break;
				case 2:
					value=float(m_PhysFunc.RadLongOut(*v));
					break;
			}
			break;
		case 1:   // Meteorological Function
			switch (m_FuncChoiceValue) {
				case 0: // Relative humidity
					value=float(*(v+1)/m_PhysFunc.SVP(*v)*100.);
					break;
				case 1: //Saturated Vapour pressure
					value=float(m_PhysFunc.SVP(*v));
					break;
				case 2: // Slope of Saturated Vapour pressure
					value=float(m_PhysFunc.DSVP(*v));
					break;
				case 3: // Vapour pressure
					value=float(m_PhysFunc.SVP(*v)**(v+1)/100.);
					break;

			}
			break;


	}
	*/

	return value;
}
