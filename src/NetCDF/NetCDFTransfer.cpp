
#include "../stdafx.h"
#include "../Util.h"
#include "../CoupModel.h"
#include "../MainFrm.h"
#include "NetCDFTransfer.h"
#include <vector>
#include "ncFile.h"
#include "ncDim.h"
#include "Test2.h"
#include "../PG\pg.h"
#include "../Cug\SimDoc\Mycug.h"
#include <thread>
#include <chrono>

static const int NREC =67420; // 67420
static const int N8_REC=90584;
static const int NR =248;
static const int NC_ERR = 2;
using namespace netCDF;
using namespace netCDF::exceptions;
static const double CONV_SPEC_HPA=1013.;

void Read_From_File(string var, string ncfile, int superindex, int ivar, CPG *pPG) {
  float fvar[NR];	
  vector<float> v;
  v.resize(N8_REC);

   // Open the file for read access
     std::this_thread::sleep_for(std::chrono::seconds(1));
  string finalName;
  size_t nstart=0;
		  vector<size_t> st;
		  vector<size_t> nel;
		  st.resize(3);
		  nel.resize(3);
		  st[1]=superindex%360;
		  st[2]=superindex/360;
		  st[0]=0;
		  nel[0]=248; nel[1]=1; nel[2]=1;


 size_t ncount;	
 NcFile *dataFile;
 NcVar data;
  for(int iyear=1979;iyear<2010; iyear++) {	
	  for(int imon=1; imon<13; imon++) {
		  finalName=ncfile;finalName+=FUtil::STD_ItoAscii(iyear);if(imon<10) finalName+="0"; finalName+=FUtil::STD_ItoAscii(imon);finalName+=".nc";
			dataFile =new NcFile(finalName, NcFile::read);

		    data=dataFile->getVar(var);
			int ndim;
		   ndim=data.getDimCount();
		   NcDim nndim;
		   nndim=data.getDim(0);
		   ncount=nndim.getSize();
		   nel[0]=ncount;

 

		   data.getVar(st,nel,fvar);

		   if(var=="Tair")
			 for(int i=0;i<ncount;i++) 
				   v[i+nstart]=fvar[i]-273.15;
		   else if(var=="Qair")
			 for(int i=0;i<ncount;i++) 
				   v[i+nstart]=fvar[i]*CONV_SPEC_HPA*100;
		   else if(var=="SWdown"||var=="LWdown"||var=="Rainf"||var=="Snowf")
			 for(int i=0;i<ncount;i++) 
				   v[i+nstart]=fvar[i]*86400.;
		   else
			  for(int i=0;i<ncount;i++) 
				   v[i+nstart]=fvar[i];
		   nstart+=ncount;
		   /*CString mes;
			mes=FUtil::STD_ItoAscii(iyear).c_str();
			mes+="-";
		   mes+=FUtil::STD_ItoAscii(imon).c_str();
		   MySingle::instance().GetCugPointer()->UpdateStatusBar(mes);	*/  
			delete dataFile;
	  }
  }
  pPG->AddVariableVector(&v, ivar);

  return;

}

void Read_From_File2(string var, string ncfile, int superindex, int ivar, CPG *pPG) {
  float fvar[NR];	
  vector<float> v;
  v.resize(N8_REC);

   // Open the file for read access
     std::this_thread::sleep_for(std::chrono::seconds(1));
  string finalName;
  size_t nstart=0;
		  vector<size_t> st;
		  vector<size_t> nel;
		  st.resize(3);
		  nel.resize(3);
		  st[1]=superindex%360;
		  st[2]=superindex/360;
		  st[0]=0;
		  nel[0]=248; nel[1]=1; nel[2]=1;


 size_t ncount;	
 NcFile *dataFile;
 NcVar data;
  for(int iyear=1979;iyear<2010; iyear++) {	
	  for(int imon=1; imon<13; imon++) {
		  finalName=ncfile;finalName+=FUtil::STD_ItoAscii(iyear);if(imon<10) finalName+="0"; finalName+=FUtil::STD_ItoAscii(imon);finalName+=".nc";
			dataFile =new NcFile(finalName, NcFile::read);

		    data=dataFile->getVar(var);
			int ndim;
		   ndim=data.getDimCount();
		   NcDim nndim;
		   nndim=data.getDim(0);
		   ncount=nndim.getSize();
		   nel[0]=ncount;

 

		   data.getVar(st,nel,fvar);

		   if(var=="Tair")
			 for(int i=0;i<ncount;i++) 
				   v[i+nstart]=fvar[i]-273.15;
		   else if(var=="Qair")
			 for(int i=0;i<ncount;i++) 
				   v[i+nstart]=fvar[i]*CONV_SPEC_HPA*100;
		   else if(var=="SWdown"||var=="LWdown"||var=="Rainf"||var=="Snowf")
			 for(int i=0;i<ncount;i++) 
				   v[i+nstart]=fvar[i]*86400.;
		   else
			  for(int i=0;i<ncount;i++) 
				   v[i+nstart]=fvar[i];
		   nstart+=ncount;
		   /*CString mes;
			mes=FUtil::STD_ItoAscii(iyear).c_str();
			mes+="-";
		   mes+=FUtil::STD_ItoAscii(imon).c_str();
		   MySingle::instance().GetCugPointer()->UpdateStatusBar(mes);	*/  
			delete dataFile;
	  }
  }
  pPG->AddVariableVector(&v, ivar);

  return;

}
UINT ReadFileInBackground1( LPVOID pParam )
{

    NetCDFTransfer* pObject = (NetCDFTransfer*)pParam;
		pObject->m_Ready[0]=false;
	std::this_thread::sleep_for(std::chrono::seconds(5));

	string file;
	file=pObject->m_FileBase;
	file+="\\Tair_WFDEI\\Tair_WFDEI_";
	Read_From_File(pObject->m_Var[0],file, pObject->m_SuperIndex,0,&pObject->m_PGFile);
//	pObject->Read3HFile();
	//pObject->UpdateStatistics(TRUE, nullptr);

    // do something with 'pObject'
	pObject->m_Ready[0]=true;
    return 0;   // thread completed successfully
}
UINT ReadFileInBackground2( LPVOID pParam )
{

    NetCDFTransfer* pObject = (NetCDFTransfer*)pParam;
		pObject->m_Ready[1]=false;
	std::this_thread::sleep_for(std::chrono::seconds(5));
	string file;
	file=pObject->m_FileBase;
	file+="\\Qair_WFDEI\\Qair_WFDEI_";
	Read_From_File2(pObject->m_Var[1],file, pObject->m_SuperIndex,1,&pObject->m_PGFile);
//	pObject->Read3HFile();
	//pObject->UpdateStatistics(TRUE, nullptr);

    // do something with 'pObject'
	pObject->m_Ready[1]=true;
    return 0;   // thread completed successfully
}
UINT ReadFileInBackground3( LPVOID pParam )
{

    NetCDFTransfer* pObject = (NetCDFTransfer*)pParam;
		pObject->m_Ready[2]=false;
	std::this_thread::sleep_for(std::chrono::seconds(5));
	string file;
	file=pObject->m_FileBase;
	file+="\\Wind_WFDEI\\Wind_WFDEI_";
	//Read_From_File(pObject->m_Var[2],file, pObject->m_SuperIndex,2,&pObject->m_PGFile);
//	pObject->Read3HFile();
	//pObject->UpdateStatistics(TRUE, nullptr);

    // do something with 'pObject'
	pObject->m_Ready[2]=true;
    return 0;   // thread completed successfully
}
UINT ReadFileInBackground4( LPVOID pParam )
{

    NetCDFTransfer* pObject = (NetCDFTransfer*)pParam;
		pObject->m_Ready[3]=false;
	std::this_thread::sleep_for(std::chrono::seconds(5));
	string file;
	file=pObject->m_FileBase;
	file+="\\Rainf_WFDEI_GPCC\\Rainf_WFDEI_GPCC_";
	//Read_From_File(pObject->m_Var[3],file, pObject->m_SuperIndex,3,&pObject->m_PGFile);
//	pObject->Read3HFile();
	//pObject->UpdateStatistics(TRUE, nullptr);

    // do something with 'pObject'
	pObject->m_Ready[3]=true;
    return 0;   // thread completed successfully
}
UINT ReadFileInBackground5( LPVOID pParam )
{

    NetCDFTransfer* pObject = (NetCDFTransfer*)pParam;
		pObject->m_Ready[4]=false;
	std::this_thread::sleep_for(std::chrono::seconds(5));
	string file;
	file=pObject->m_FileBase;
	file+="\\SWdown_WFDEI\\SWdown_WFDEI_";
	//Read_From_File(pObject->m_Var[4],file, pObject->m_SuperIndex,4,&pObject->m_PGFile);
//	pObject->Read3HFile();
	//pObject->UpdateStatistics(TRUE, nullptr);

    // do something with 'pObject'
	pObject->m_Ready[4]=true;
    return 0;   // thread completed successfully
}
UINT ReadFileInBackground6( LPVOID pParam )
{

    NetCDFTransfer* pObject = (NetCDFTransfer*)pParam;
		pObject->m_Ready[5]=false;
	std::this_thread::sleep_for(std::chrono::seconds(5));
	string file;
	file=pObject->m_FileBase;
	file+="\\LWdown_WFDEI\\LWdown_WFDEI_";
	//Read_From_File(pObject->m_Var[5],file, pObject->m_SuperIndex,5,&pObject->m_PGFile);
//	pObject->Read3HFile();
	//pObject->UpdateStatistics(TRUE, nullptr);

    // do something with 'pObject'
	pObject->m_Ready[5]=true;
    return 0;   // thread completed successfully
}
UINT ReadFileInBackground7( LPVOID pParam )
{

    NetCDFTransfer* pObject = (NetCDFTransfer*)pParam;
		pObject->m_Ready[6]=false;
	std::this_thread::sleep_for(std::chrono::seconds(5));
	string file;
	file=pObject->m_FileBase;
	file+="\\Snowf_WFDEI_GPCC\\Snowf_WFDEI_GPCC_";
	//Read_From_File(pObject->m_Var[6],file, pObject->m_SuperIndex,6,&pObject->m_PGFile);
//	pObject->Read3HFile();
	//pObject->UpdateStatistics(TRUE, nullptr);

    // do something with 'pObject'
	pObject->m_Ready[6]=true;
    return 0;   // thread completed successfully
}





NetCDFTransfer::NetCDFTransfer()
{
	 
	for(size_t i=0; i<7; i++) m_Ready[i]=false;

	CStdioFile driveFile;
	CString pszFileName ;
	LPCTSTR lpszDefExt;
	int fileDlgResult;

	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	size_t nproc=siSysInfo.dwNumberOfProcessors;
	vector<thread> mythreads;
	mythreads.resize(nproc-2);

/*
	lpszDefExt="txt";
	CString UserDirectory;


 	CFileDialog	DriveFileDlg(TRUE,lpszDefExt,pszFileName, OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,lpszFilter);

	if (DriveFileDlg.DoModal()==IDOK) {
		pszFileName = DriveFileDlg.GetPathName();
		int iv=pszFileName.ReverseFind('\\');
		m_FileBase=pszFileName.Mid(0,iv);
	}

	CFileDialog	DriveFileDlg(TRUE, lpszDefExt, pszFileName, OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, lpszFilter);*/
	FILEDLG FileDlg = MFC_Util::GetFileDialog("*.txt", "", "List of File to be converted (*.*)|*.*||");

	if (FileDlg.opengood != IDOK) {
		return;

	}
	auto iv = FileDlg.filename.rfind('\\');
	m_FileBase = FileDlg.filename.substr(0, iv);

	ifstream file;
	file.open(CString((FileDlg.filename).c_str()),ios::in);
	m_PGFile.SetCompleteRead(true);

	m_PGFile.CreateNewFile(6, N8_REC,1);
	m_PGFile.SetDate(1,"1979-01-01 00:00");
	long m_st=m_PGFile.GetLongTime(1);

	int file_count=0;
	
	if(file.is_open()) {
		
		string line, s_long, s_lat, s_land, code_unit;
		float f_long, f_lat;
		int superindex;
		file.seekg(0,ios::end);
		unsigned long long totsize=file.tellg();
		file.seekg(0,ios::beg);
		while(file.good()) {
			getline(file,line);
			int icom=line.find(',');
			if(icom<0) icom=line.find(';');

			if(icom>0) {
				s_long=line.substr(0,icom);
				f_long=FUtil::AtoFloat(s_long);
				line=line.substr(icom+1);
				icom=line.find(',');
				if(icom<0) icom=line.find(';');
				if(icom>0) {
					s_lat=line.substr(0,icom);
					f_lat=FUtil::AtoFloat(s_lat);
				}
				m_SuperIndex=int((f_lat+89.75)/0.5)+360*int((f_long+179.75)/0.5);
				line=line.substr(icom+1);
				s_land=line;

				string ncfile;
				long min;
				for(int i=0;i<N8_REC;i++) {
					min=m_st+i*180;
					m_PGFile.SetLongTime(i+1,min);
				}
				for (int i=0; i<6;i++) {
					string code_dir, code_var, code_var2;
					ncfile=m_FileBase;
					switch (i) {
					case 0:
						code_dir="\\Tair\\";
						code_var2="Air Temperature";
						code_var="Tair";
						code_unit="C";
						break;
					case 1:
						code_dir="\\Qair\\";
						code_var="Qair";
						code_var2="Vapour Pressure";
						code_unit="Pa";
						break;
					case 2:
						code_dir="\\Wind\\";
						code_var="Wind";
						code_var2="Wind Speed";
						code_unit="m/s";
						break;
					case 3:
						code_dir="\\Precip\\";
						code_var="Precip";
						code_var2="Precipitation";
						code_unit="mm/day";
						break;
					case 4:
						code_dir="\\SWdown\\";
						code_var="SWdown";
						code_var2="Global Radiation";
						code_unit="J/m2/day";
						break;
					case 5:
						code_dir="\\LWdown\\";
						code_var="LWdown";
						code_var2="Long Wave Radiaton";
						code_unit="J/m2/day";
						break;


					}
					ncfile+=code_dir;
					ncfile+=code_var;
					ncfile+="_WFDEI_";
					vector<float> v;
					v.resize(N8_REC);
					if(Read3HFile(ncfile,superindex, code_var, &v)) {
						int good=1;
						m_PGFile.AddVariableVector(&v,i);


					}
				 //  mythreads[i]=thread(Read_From_File,ncfile, superindex, code_var, &m_PGFile, i);


					m_PGFile.SetVarName(i+1,code_var2);
					m_PGFile.SetVarUnit(i+1,code_unit);
					m_PGFile.SetVarId(i+1,s_land);
					double latv=FUtil::AtoFloat(s_lat);
					m_PGFile.SetVarLat(i+1, latv);
					double longv=FUtil::AtoFloat(s_long);
					m_PGFile.SetVarLong(i+1, longv);
				}
				for(size_t i=0; i<6;i++)
					mythreads[i].join();

				string filut;
				filut=m_FileBase;
				filut+="\\WatchB_";
				file_count++;
				filut+=FUtil::STD_ItoAscii(superindex);
				
				//filut+=ItoNumAscii(file_count);
				//filut+=s_land;
				filut+=".bin";
				m_PGFile.CheckMissingNumbers();
				m_PGFile.ReCalculateMinMax();
				m_PGFile.SaveAs(filut, true);
				m_PGFile.CloseFile();
			}

		}
	}
}
NetCDFTransfer::NetCDFTransfer(size_t iopt)
{
	 m_Var[0]="Tair";
	 m_Var[1]="Qair";
	 m_Var[2]="Wind";
	 m_Var[4]="SWdown";
	 m_Var[3]="Rainf";
	 m_Var[5]=="LWdown";
	 m_Var[6]="Snowf";
	

	CStdioFile driveFile;
	CString pszFileName ;
	LPCTSTR lpszDefExt;
	int fileDlgResult;

	CString UserDirectory;


	if(iopt>1) {
		WFDEI();
		return;
	}

	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	size_t nproc=siSysInfo.dwNumberOfProcessors;
	vector<thread> mythreads;
	mythreads.resize(nproc-2);


 /*	CFileDialog	DriveFileDlg(TRUE,lpszDefExt,pszFileName, OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,lpszFilter);*/
	FILEDLG FileDlg = MFC_Util::GetFileDialog("*.txt", "", "List of File to be converted (*.*)|*.*||");

	if (FileDlg.opengood!=IDOK) {
		return;

	}

	ifstream file;
	file.open(CString((FileDlg.filename).c_str()),ios::in);
	m_PGFile.SetCompleteRead(true);

	m_PGFile.CreateNewFile(6, NREC,1);
	m_PGFile.SetDate(1,"1979-01-01 12:00");
	long m_st=m_PGFile.GetLongTime(1);

	int file_count=0;
	
	if(file.is_open()) {
		
		string line, s_long, s_lat, s_land, code_unit;
		file.seekg(0,ios::end);
		unsigned long long totsize=file.tellg();
		file.seekg(0,ios::beg);
		while(file.good()) {
			getline(file,line);
			int icom=line.find(',');
			if(icom<0) icom=line.find(';');

			if(icom>0) {
				s_land=line;
				s_long=line.substr(0,icom);
				float f_long=FUtil::AtoFloat(s_long);
				line=line.substr(icom+1);
				icom=line.find(',');
				if(icom<0) icom=line.find(';');
				float f_lat;
				if(icom>0) {
					s_lat=line.substr(0,icom);
					f_lat=FUtil::AtoFloat(s_lat);
				}
				m_SuperIndex=int((f_lat+89.75)/0.5)+360*int((f_long+179.75)/0.5);


				string ncfile;
				long min;
				for(int i=0;i<NREC;i++) {
					min=m_st+i*1440;
					m_PGFile.SetLongTime(i+1,min);
				}
				for (int i=0; i<6;i++) {
					string code_dir, code_var, code_var2;
					ncfile=m_FileBase;
					switch (i) {
					case 0:
						code_dir="\\Tair\\";
						code_var2="Soil Carbon";
						code_var="Tair";
						code_unit="Kgm2";
						break;
					case 1:
						code_dir="\\Qair\\";
						code_var="longitude";
						code_var2="Long";
						code_unit="";
						break;
					case 2:
						code_dir="\\Wind\\";
						code_var="latitude";
						code_var2="Latitude";
						code_unit="m/s";
						break;
					case 3:
						code_dir="\\Precip\\";
						code_var="vwilt";
						code_var2="wilting point";
						code_unit="mm/day";
						break;
					case 4:
						code_dir="\\SWdown\\";
						code_var="vsat";
						code_var2="sat conductivity";
						code_unit="J/m2/day";
						break;
					case 5:
						code_dir="\\LWdown\\";
						code_var="vcrit";
						code_var2="Coef";
						code_unit="J/m2/day";
						break;


					}
					ncfile= FileDlg.filename;
					vector<float> v;
					v.resize(NREC);


					if(ReadFile(ncfile,code_var, &v)) {
						int good=1;
						m_PGFile.AddVariableVector(&v,i);


					}
					//mythreads[i]=thread(Read_From_File,ncfile, superindex, code_var, &v);
					m_PGFile.SetVarName(i+1,code_var2);
					m_PGFile.SetVarUnit(i+1,code_unit);
					m_PGFile.SetVarId(i+1,s_land);
					double latv=FUtil::AtoFloat(s_lat);
					m_PGFile.SetVarLat(i+1, latv);
					double longv=FUtil::AtoFloat(s_long);
					m_PGFile.SetVarLong(i+1, longv);
				}

				string filut;
				filut=m_FileBase;
				filut+="Watch_";
				file_count++;
				filut+=FUtil::STD_ItoAscii(file_count);
				
				//filut+=ItoNumAscii(file_count);
				//filut+=s_land;
				filut+=".bin";
				m_PGFile.CheckMissingNumbers();
				m_PGFile.ReCalculateMinMax();
				m_PGFile.SaveAs(filut, true);
				m_PGFile.CloseFile();
			}

		}
	}
}
void NetCDFTransfer::WFDEI()
{
	 

	CStdioFile driveFile;
	CString pszFileName ;
	LPCTSTR lpszDefExt;
	int fileDlgResult;

	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	size_t nproc=siSysInfo.dwNumberOfProcessors;
	vector<thread> mythreads;
	mythreads.resize(7);
	std::thread k[7];


	CString UserDirectory;
	vector<CWinThread*> pWinThread;
	pWinThread.resize(7);


 	//CFileDialog	DriveFileDlg(TRUE,lpszDefExt,pszFileName, OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,lpszFilter);
	FILEDLG FileDlg = MFC_Util::GetFileDialog("*.txt", "", "List of File to be converted (*.*)|*.*||");

	if (FileDlg.opengood != IDOK) {
		return;

	}
	auto iv = FileDlg.filename.rfind('\\');
	m_FileBase = FileDlg.filename.substr(0, iv);


	ifstream file;
	file.open(FileDlg.filename,ios::in);
	m_PGFile.SetCompleteRead(true);

	m_PGFile.CreateNewFile(7, N8_REC,1);
	m_PGFile.SetDate(1,"1979-01-01 00:00");
	long m_st=m_PGFile.GetLongTime(1);

	int file_count=0;
	
	if(file.is_open()) {
		
		string line, s_long, s_lat, s_land, code_unit;
		float f_long, f_lat;
		int superindex;
		file.seekg(0,ios::end);
		unsigned long long totsize=file.tellg();
		file.seekg(0,ios::beg);
		while(file.good()) {
			getline(file,line);
			int icom=line.find(',');
			if(icom<0) icom=line.find(';');

			if(icom>0) {
				s_long=line.substr(0,icom);
				f_long=FUtil::AtoFloat(s_long)+0.25;
				line=line.substr(icom+1);
				icom=line.find(',');
				if(icom<0) icom=line.find(';');
				if(icom>0) {
					s_lat=line.substr(0,icom);
					f_lat=FUtil::AtoFloat(s_lat)+0.25;
				}
				m_SuperIndex=int((f_lat+89.75)/0.5+0.01)+360*int((f_long+179.75)/0.5+0.01);
				line=line.substr(icom+1);
				s_land=line;

				string ncfile;
				long min;
				for(int i=0;i<N8_REC;i++) {
					min=m_st+i*180;
					m_PGFile.SetLongTime(i+1,min);
				}
				for (int i=0; i<7;i++) {
					string code_dir, code_var, code_var2;
					ncfile=m_FileBase;
					switch (i) {
					case 0:
						code_dir="\\Tair_WFDEI\\";
						code_var2="Air Temperature";
						code_var="Tair";
						code_unit="C";
						break;
					case 1:
						code_dir="\\Qair_WFDEI\\";
						code_var="Qair";
						code_var2="Vapour Pressure";
						code_unit="Pa";
						break;
					case 2:
						code_dir="\\Wind_WFDEI\\";
						code_var="Wind";
						code_var2="Wind Speed";
						code_unit="m/s";
						break;
					case 3:
						code_dir="\\Rainf_WFDEI_GPCC\\";
						code_var="Rainf";
						code_var2="Precipitation";
						code_unit="mm/day";
						break;
					case 4:
						code_dir="\\SWdown_WFDEI\\";
						code_var="SWdown";
						code_var2="Global Radiation";
						code_unit="J/m2/day";
						break;
					case 5:
						code_dir="\\LWdown_WFDEI\\";
						code_var="LWdown";
						code_var2="Long Wave Radiaton";
						code_unit="J/m2/day";
						break;
					case 6:
						code_dir="\\Snowf_WFDEI_GPCC\\";
						code_var="Snowf";
						code_var2="Snowfall";
						code_unit="mm/day";
						break;
					case 7:
						code_dir="\\PSurf_WFDEI\\";
						code_var="PSurf";
						code_var2="Long Wave Radiaton";
						code_unit="J/m2/day";
						break;


					}
					ncfile+=code_dir;
					ncfile+=code_var;
					ncfile+="_WFDEI_";
					if(i==6||i==3) ncfile+="GPCC_";
					vector<float> v;
					v.resize(N8_REC);
					if(Read3HFile(ncfile,m_SuperIndex, code_var, &v)) {
						int good=1;
						m_PGFile.AddVariableVector(&v,i);


					}
					/*switch( i) {
						case 0: pWinThread[i]=AfxBeginThread(ReadFileInBackground1, this); break;
						case 1: pWinThread[i]=AfxBeginThread(ReadFileInBackground2, this); break;
						case 2: pWinThread[i]=AfxBeginThread(ReadFileInBackground3, this); break;
						case 3: pWinThread[i]=AfxBeginThread(ReadFileInBackground4, this); break;
						case 4: pWinThread[i]=AfxBeginThread(ReadFileInBackground5, this); break;
						case 5: pWinThread[i]=AfxBeginThread(ReadFileInBackground6, this); break;
						case 6: pWinThread[i]=AfxBeginThread(ReadFileInBackground7, this); break;

					}
					//mythreads[i]=thread(Read_From_File,ncfile, superindex, code_var, &m_PGFile, i);*/
					m_PGFile.SetVarName(i+1,code_var2);
					m_PGFile.SetVarUnit(i+1,code_unit);
					m_PGFile.SetVarId(i+1,s_land);
					double latv=FUtil::AtoFloat(s_lat);
					m_PGFile.SetVarLat(i+1, latv);
					double longv=FUtil::AtoFloat(s_long);
					m_PGFile.SetVarLong(i+1, longv);
					//mythreads[i].join();
				}

				while (m_Ready[0]==false||m_Ready[1]==false||m_Ready[2]==false||m_Ready[3]==false||m_Ready[4]==false||m_Ready[5]==false||m_Ready[6]==false) {
				}
				string filut;
				filut=m_FileBase;
				filut+="\\WatchB_";

				file_count++;
				//filut+=MFC_Util::MFC_Util::MFC_Util::FUtil::STD_ItoAscii(superindex);
				int ilat=int((f_lat+89.75)/0.5+0.01)+1;
				int ilong=int((f_long+179.75)/0.5+0.01)+1;
				string num;
				num=FUtil::ItoNumAscii(ilat);
				
				filut+=num.substr(2,3);filut+="_";
				num=FUtil::ItoNumAscii(ilong);
				
				filut+=num.substr(2,3);

				
				//filut+=ItoNumAscii(file_count);
				//filut+=s_land;
				filut+=".bin";
				m_PGFile.CheckMissingNumbers();
				m_PGFile.ReCalculateMinMax();
				m_PGFile.SaveAs(filut, true);
				m_PGFile.CloseFile();
			}

		}
	}



}
NetCDFTransfer::~NetCDFTransfer()
{
}
bool NetCDFTransfer::ReadFile(string file, string var, vector<float> *v)
{

   float fvar[NR];	
   try {
   // Open the file for read access
   NcFile dataFile(file, NcFile::read);

   // Retrieve the variable named "data"




  vector<size_t> st;
  vector<size_t> nel;
  st.resize(3);
  nel.resize(3);
  st[0]=0, st[1]=0; st[2]=0;
  nel[0]=248; nel[1]=1; nel[2]=1;


   NcVar data=dataFile.getVar(var);
   if(data.isNull()) { 

	   return false;
   }
   int ndim;
   ndim=data.getDimCount();
   NcDim nndim;
   nndim=data.getDim(0);
   size_t ncount=nndim.getSize();
   nndim=data.getDim(1);
   size_t ncount2=nndim.getSize();
   nndim=data.getDim(2);
   size_t ncount3=nndim.getSize();
 

   data.getVar(st,nel,&fvar);

   if(var=="Tair")
	 for(int i=0;i<NREC;i++) 
		   v->at(i)=fvar[i]-273.15;
   else if(var=="Qair")
	 for(int i=0;i<NREC;i++) 
		   v->at(i)=fvar[i]*CONV_SPEC_HPA*100;
   else if(var=="SWdown"||var=="LWdown"||var=="Precip")
	 for(int i=0;i<NREC;i++) 
		   v->at(i)=fvar[i]*86400.;
   else
	  for(int i=0;i<NREC;i++) 
		   v->at(i)=fvar[i];
  


	return true;

   }catch(NcException& e)
   {
	   return false;
  }


	
}
bool NetCDFTransfer::Read3HFile(string file,int superindex, string var, vector<float> *v)
{

   float fvar[NR];	

   // Open the file for read access

  string finalName;
  size_t nstart=0;
		  vector<size_t> st;
		  vector<size_t> nel;
		  st.resize(3);
		  nel.resize(3);
		  st[1]=superindex%360;
		  st[2]=superindex/360;
		  st[0]=0;
		  nel[0]=248; nel[1]=1; nel[2]=1;


 size_t ncount;	
 NcFile *dataFile;
 NcVar data;

  for(int iyear=1979;iyear<2010; iyear++) {	
	  for(int imon=1; imon<13; imon++) {
			
		//   try {
		   finalName=file;finalName+=FUtil::STD_ItoAscii(iyear);if(imon<10) finalName+="0"; finalName+=FUtil::STD_ItoAscii(imon);finalName+=".nc";
		  
		   dataFile =new NcFile(finalName, NcFile::read);
		   data=dataFile->getVar(var);
		   if(data.isNull()) { 

			   return false;
		   }
		   int ndim;
		 //  ndim=data.getDimCount();
		   NcDim nndim;
		   nndim=data.getDim(0);
		   ncount=nndim.getSize();
		   nel[0]=ncount;

 

		   data.getVar(st,nel,fvar);

		   if(var=="Tair")
			 for(int i=0;i<ncount;i++) 
				   v->at(i+nstart)=fvar[i]-273.15;
		   else if(var=="Qair")
			 for(int i=0;i<ncount;i++) 
				   v->at(i+nstart)=fvar[i]*CONV_SPEC_HPA*100;
		   else if(var=="SWdown"||var=="LWdown"||var=="Rainf"||var=="Snowf")
			 for(int i=0;i<ncount;i++) 
				   v->at(i+nstart)=fvar[i]*86400.;
		   else
			  for(int i=0;i<ncount;i++) 
				   v->at(i+nstart)=fvar[i];
		   nstart+=ncount;
		   CString mes;
		   mes=FUtil::STD_ItoAscii(iyear).c_str();
		   mes+="-";
		   mes+=FUtil::STD_ItoAscii(imon).c_str();
			MySingle::instance().GetCugPointer()->UpdateStatusBar(mes);	  
			delete dataFile;

	//	}catch(NcException& e)
	//	{
	//		 return false;
	//	}

	
	  }
  
 }

return true;

}
float NetCDFTransfer::GetValue(string file, string var, size_t index)
{

   float fvar[NREC];	
   try {
   // Open the file for read access
   NcFile dataFile(file, NcFile::read);

   // Retrieve the variable named "data"
   NcVar data=dataFile.getVar(var);
   if(data.isNull()) { 

	   return false;
   }
   data.getVar(fvar);

   return fvar[index];
   
   }catch(NcException& e)
   {
	   return MISSING;
  }


	
}
