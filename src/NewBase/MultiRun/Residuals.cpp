// Residuals.cpp : implementation file
//
#ifndef COUPSTD
#include "../stdafx.h"
#endif
#include "../../Util/FUtil.hpp"
#include "../../NewBase/Doc.h"
#include "Residuals.h"
#ifndef COUPSTD
#include "../NewBase/FileScanner.h"
#include "../MainFrm.h"
#endif

/*
Residual Files are named XBin.

The Files have binary format with a header of 3 NonSigned integer of 4 bytes

See Method CreateFile
or OpenFile To create a new file or to Open an existing file


To Get the value of a specific Variable at a certain time point and a certain MultiRun See

Method GetResiduals
or GetResidualVector

The Residuals are stored as float with 4 bytes each.
A vector of Residuals consist of all records for each variables in the the files.

The position in the files are in a sequence


Records - Number of time points for the Variable, corresponding to the number of time points in the Validation File
Variable
Run 

To get the position for a give residual in the file:
		std::streamsize byte=4;
		std::streamsize newpos=irun*(m_RecL*m_NumVar)+iv*m_RecL+m_RecL;
		if(m_RecL<3) newpos=irun*(m_RecL*m_NumVar)+iv*m_RecL+3;
		newpos=newpos*byte;

To Read the Residual vector
			m_ResidualFile.read((char*)&m_Residuals[0], m_RecL * 4);

To Get the position of the specific record use the index of the vector read.

*/





// CResiduals

CResiduals::CResiduals()
{
	m_IsOpen=false;
	m_Allocated=false;
	m_PrevRun=-1;
	m_PrevVar=-1;
	m_IsUpdated=false;
	m_IsAllUpdated=false;
	m_pNewMap=nullptr;
	p95=p50=p5=0.;
	m_LogLiNew=nullptr;
	m_NewLogLiFileExist=false;
	m_NewLogLiFileOpen=false;
	m_SortedResiduals=nullptr;
	m_SortedFileExist=false;
	m_SortedFileOpen=false;
	m_Calculating=false;
	m_MeanDefined=false;
	m_RecL = 0;
	

}

CResiduals::~CResiduals()
{
	if(m_IsOpen)
		CloseFile();

	if(m_LogLiNew!=nullptr)
		delete [] m_LogLiNew;
	if(m_SortedResiduals!=nullptr)
		delete [] m_SortedResiduals;
}
void CResiduals::InitDoc(NewMap *pMap, int file)
{
	m_pNewMap=pMap;
	m_FileNo=file;

}
void CResiduals::ExitSortedResidualFile()
{
	if(!m_SortedFileOpen) return;

	m_SortedResidualFile.close();
}
void CResiduals::InitSortedResidualFile()
{
	if(m_SortedFileOpen) return;

	
	bool k=FUtil::IsFileExisting(m_SortedFileName);
	if(k) {
		m_SortedResidualFile.open(m_SortedFileName,ios::binary|ios::out|ios::in);

	}
	else {
		//m_SortedResidualFile.open(m_SortedFileName,CFile::modeReadWrite|CFile::typeBinary|CFile::shareDenyNone|CFile::modeCreate);
		m_SortedResidualFile.open(m_SortedFileName,ios::binary|ios::out|ios::in);

	}
	m_SortedFileExist=true;
	m_SortedFileOpen=true;
	if(m_SortedResiduals==nullptr)
		m_SortedResiduals= new float [m_RecL+7];
	m_ExtraRecL=4*(m_RecL+7);
}
void CResiduals::ResetSortedResidualFile()
{
	if(!m_SortedFileOpen) return;
	ExitSortedResidualFile();
	delete [] m_SortedResiduals;
	m_SortedResiduals=nullptr;
	m_SortedFileOpen=false;
	
}
int CResiduals::AddSortedResiduals(float *m_Sort, size_t nrec)
{
	DistStat Stat;
	size_t ncheck;
	MeanStat(m_Sort,nrec, &ncheck,&Stat);
	
	auto pos=m_SortedResidualFile.tellg();
	m_SortedResidualFile.write((char*)&*m_Sort,nrec*4); 
	m_SortedResidualFile.write((char*)&Stat.Mean,4);
	m_SortedResidualFile.write((char*)&Stat.Std,4);
	m_SortedResidualFile.write((char*)&Stat.Min,4);
	m_SortedResidualFile.write((char*)&Stat.P5,4);
	m_SortedResidualFile.write((char*)&Stat.P50,4);
	m_SortedResidualFile.write((char*)&Stat.P95,4);
	m_SortedResidualFile.write((char*)&Stat.Max,4);

	return m_ExtraRecL;
}
void CResiduals::ExitAddLogLiFile()
{
	if(!m_NewLogLiFileOpen) return;

	m_ExtraFile.close();
}
void CResiduals::InitExtraLogLiFile()
{
	if(m_NewLogLiFileOpen) return;

	
	bool k=FUtil::IsFileExisting(m_ExtraFileName);
	if(k) {
		m_ExtraFile.open(m_ExtraFileName,ios::binary|ios::out|ios::in);

	}
	else {
		m_ExtraFile.open(m_ExtraFileName,ios::binary|ios::out|ios::in);


	}
	m_NewLogLiFileExist=true;
	m_NewLogLiFileOpen=true;
	if(m_LogLiNew==nullptr)
		m_LogLiNew= new float [m_RecL+7];
	m_ExtraRecL=4*(m_RecL+7);
}
void CResiduals::ResetExtraLogLiFile()
{
	if(!m_NewLogLiFileOpen) return;
	ExitAddLogLiFile();
	delete [] m_LogLiNew;
	m_LogLiNew=nullptr;
	m_NewLogLiFileOpen=false;
	
}
int CResiduals::AddLogLi(float *Log, size_t nrec)
{
	DistStat Stat;
	size_t ncheck;
	StatUtil::SortArray(Log,nrec);
	MeanStat(Log,nrec, &ncheck, &Stat);
	
	auto pos=m_ExtraFile.tellg();
	m_ExtraFile.write((char*)&*Log,nrec*4); 
	m_ExtraFile.write((char*)&Stat.Mean,4);
	m_ExtraFile.write((char*)&Stat.Std,4);
	m_ExtraFile.write((char*)&Stat.Min,4);
	m_ExtraFile.write((char*)&Stat.P5,4);
	m_ExtraFile.write((char*)&Stat.P50,4);
	m_ExtraFile.write((char*)&Stat.P95,4);
	m_ExtraFile.write((char*)&Stat.Max,4);

	return true;
}
void CResiduals::CloseFile()
{
	if(m_IsOpen) {
		m_ResidualFile.close();
		m_IsOpen=false;
	}
	if(m_NewLogLiFileOpen) {

		m_ExtraFile.close();
		m_NewLogLiFileOpen=false;
	}
}
bool CResiduals::CreateFile(string filename, size_t NumRec, size_t RecL, size_t NumVar) {
	m_FileName=filename;

	if(m_IsOpen) CloseFile();


	if(!FUtil::IsFileExisting(filename)) {
		m_ResidualFile.open(m_FileName, ios::binary|ios::out);
		m_ResidualFile.close();
	}
	

	m_ResidualFile.open(m_FileName, ios::binary|ios::out|ios::in);

	m_NumRec=NumRec; m_RecL=RecL; m_NumVar=NumVar;
	m_ResidualFile.write((char*)&m_NumRec,4);
	m_ResidualFile.write((char*)&m_RecL,4);
	m_ResidualFile.write((char*)&m_NumVar,4);
	if(m_RecL>3)
	m_ResidualFile.seekp(m_RecL*4, ios::beg);
	if(m_ResidualFile.good()) m_IsOpen=true;
	return m_ResidualFile.good();
}
bool CResiduals::AddResiduals(float *pFloat) {

	m_ResidualFile.write((char*)pFloat, m_RecL*4);
	return m_ResidualFile.good();

}
bool CResiduals::OpenFile(string filename, bool SetFileName)
{
	if(SetFileName)
		m_FileName=filename;

	
	m_ExtraFileName = m_FileName;
	int npos = m_FileName.find(".XBin");
	m_ExtraFileName = m_FileName.substr(0, npos) + ".YBin";
	
	m_SortedFileName = m_FileName;
	m_SortedFileName = m_FileName.substr(0, npos) + ".ZBin";

	

	if(m_IsOpen)
		return true;

	if(m_ResidualFile.is_open())
		m_ResidualFile.close();
	bool k=FUtil::IsFileExisting(filename);
	if(!k)
		m_ResidualFile.open(filename,ios::binary|ios::out);
	else
		m_ResidualFile.open(filename,ios::binary|ios::out|ios::in );
	if(!m_ResidualFile.is_open())
	{
		m_ResidualFile.open(m_FileName, ios::binary|ios::out|ios::in);
		if(!m_ResidualFile.is_open()) {
			string UserDirectory="";
			UserDirectory=FUtil::GetProfileStringStd("UserDirectory", UserDirectory);
			string testname;
			int ipos=filename.find("\\");
			while(ipos>=0) {
				filename=filename.substr(ipos+1);
				ipos=filename.find("\\");
			}
			testname=UserDirectory+filename;
			m_ResidualFile.open(testname,ios::binary|ios::out|ios::in );

			if(!m_ResidualFile.is_open()) {

				//Display error message
				m_pNewMap->m_ErrorCount++;
#ifndef COUPSTD
				TRACE("Can't Open file %s´\n", m_FileName); 
				if(m_pNewMap->m_ErrorCount<2) {
					string str;
					str="For some reason the file ("; str+=testname;str+=") specified couldn´t be opened.";
					MFC_Util::MessageBox( str.c_str(),"File reading error" ,MB_OK);
			
				}
#endif
				return false;
				// Actions without file name ?
			}
			filename=testname;
		}
	}
	if(!k) {
		//MessageBox("Could not find ValidationFile","File open error");
		return false;
	}
	if (!ReadHeader()) {
		m_ResidualFile.close();
		return false;
	}
	m_ResidualFile.close();
	if (!OpenResidualFile(true)) return false;
	else {
		m_IsOpen = true;
		return true;
	}

	return true;
}
bool CResiduals::IsExtraLogLiFile()
{

	return m_NewLogLiFileOpen;
}


float CResiduals::GetLogLi(size_t irun, size_t iv, size_t irec, bool res)
{
	if(m_PrevRun!=irun||m_PrevVar!=iv) {
		if(irun==110000) {
			irun=0;
			m_PrevRun=110000;
		}
		else
			m_PrevRun=irun;
		std::streamsize newpos=irun*(m_ExtraRecL*m_NumVar)+iv*(m_ExtraRecL);
		m_ExtraFile.seekg(newpos,ios::beg);
		m_ExtraFile.read((char*)&*m_LogLiNew,m_ExtraRecL);
		if(m_SortedFileOpen) {
			m_SortedResidualFile.seekg(newpos,ios::beg);
			m_SortedResidualFile.read((char*)&*m_SortedResiduals, m_ExtraRecL);	
		}
		m_PrevVar=iv;
	}
	if(res&&m_SortedFileOpen)
		return *(m_SortedResiduals+irec);	
	else if (res)
		return MISSING;
	else
		return *(m_LogLiNew+irec);
}
float CResiduals::GetLogLiMean(size_t irun, size_t iv, bool res)
{
	if(m_PrevRun!=irun||m_PrevVar!=iv) {
		if(irun==110000) {
			irun=0;
			m_PrevRun=110000;
		}
		else
			m_PrevRun=irun;
		std::streamsize newpos=irun*(m_ExtraRecL*m_NumVar)+iv*(m_ExtraRecL);
		std::streamsize actpos;
		m_ExtraFile.seekg(newpos,ios::beg);
		actpos=m_ExtraFile.tellg();
		if(m_SortedFileOpen) {
			m_SortedResidualFile.seekg(newpos,ios::beg);
			actpos=m_SortedResidualFile.tellg();
			m_SortedResidualFile.read((char*)&*m_SortedResiduals, m_ExtraRecL);	
		}
		m_ExtraFile.read((char*)&*m_LogLiNew,m_ExtraRecL);		
		m_PrevVar=iv;
	}
	if(res&&m_SortedFileOpen)
		return *(m_SortedResiduals+m_RecL);	
	else if(res)
		return MISSING;
	else
    	return *(m_LogLiNew+m_RecL);
}
float CResiduals::GetLogLiCV(size_t irun, size_t iv, bool res)
{
	if(m_PrevRun!=irun||m_PrevVar!=iv) {
		if(irun==110000) {
			irun=0;
			m_PrevRun=110000;
		}
		else
			m_PrevRun=irun;
		std::streamsize newpos=irun*(m_ExtraRecL*m_NumVar)+iv*(m_ExtraRecL);
		m_ExtraFile.seekg(newpos,ios::beg);
		m_ExtraFile.read((char*)&*m_LogLiNew,m_ExtraRecL);	
		if(m_SortedFileOpen) {
			m_SortedResidualFile.seekg(newpos,ios::beg);
			m_SortedResidualFile.read((char*)&*m_SortedResiduals, m_ExtraRecL);	
		}	
		m_PrevVar=iv;

	}
	if(res&&m_SortedFileOpen)
		return *(m_SortedResiduals+m_RecL+1);
	else if(res)
		return MISSING;
	else
		return *(m_LogLiNew+m_RecL+1);
}
float CResiduals::GetLogLiPMIN(size_t irun, size_t iv, bool res)
{
	if(m_PrevRun!=irun||m_PrevVar!=iv) {
		if(irun==110000) {
			irun=0;
			m_PrevRun=110000;
		}
		else
			m_PrevRun=irun;
		std::streamsize newpos=irun*(m_ExtraRecL*m_NumVar)+iv*(m_ExtraRecL);
		m_ExtraFile.seekg(newpos,ios::beg);
		m_ExtraFile.read((char*)&*m_LogLiNew,m_ExtraRecL);	
		if(m_SortedFileOpen) {
			m_SortedResidualFile.seekg(newpos,ios::beg);
			m_SortedResidualFile.read((char*)&*m_SortedResiduals, m_ExtraRecL);	
		}		
		m_PrevVar=iv;

	}
	if(res&&m_SortedFileOpen)
		return *(m_SortedResiduals+m_RecL+2);	
	else if(res)
		return MISSING;
	else
		return *(m_LogLiNew+m_RecL+2);
}
float CResiduals::GetLogLiP5(size_t irun, size_t iv, bool res)
{
	if(m_PrevRun!=irun||m_PrevVar!=iv) {
		if(irun==110000) {
			irun=0;
			m_PrevRun=110000;
		}
		else
			m_PrevRun=irun;
		std::streamsize newpos=irun*(m_ExtraRecL*m_NumVar)+iv*(m_ExtraRecL);
		m_ExtraFile.seekg(newpos,ios::beg);
		m_ExtraFile.read((char*)&*m_LogLiNew,m_ExtraRecL);	
		if(m_SortedFileOpen) {
			m_SortedResidualFile.seekg(newpos,ios::beg);
			m_SortedResidualFile.read((char*)&*m_SortedResiduals, m_ExtraRecL);	
		}		
		m_PrevVar=iv;
	}
	if(res&&m_SortedFileOpen)
		return *(m_SortedResiduals+m_RecL+3) ;	
	else if(res)
		return MISSING;
	else
		return *(m_LogLiNew+m_RecL+3);
}
float CResiduals::GetLogLiP50(size_t irun, size_t iv, bool res)
{
	if(m_PrevRun!=irun||m_PrevVar!=iv) {
		if(irun==110000) {
			irun=0;
			m_PrevRun=110000;
		}
		else
			m_PrevRun=irun;
		std::streamsize newpos=irun*(m_ExtraRecL*m_NumVar)+iv*(m_ExtraRecL);
		m_ExtraFile.seekg(newpos,ios::beg);
		m_ExtraFile.read((char*)&*m_LogLiNew,m_ExtraRecL);	

		if(m_SortedFileOpen) {
			m_SortedResidualFile.seekg(newpos,ios::beg);
			m_SortedResidualFile.read((char*)&*m_SortedResiduals, m_ExtraRecL);	
		}		
		m_PrevVar=iv;
	}
	if(res&&m_SortedFileOpen)
		return *(m_SortedResiduals+m_RecL+4);
	else if(res)
		return MISSING;
	else
		return *(m_LogLiNew+m_RecL+4);
}
float CResiduals::GetZ_ScoreLogLiP95(size_t irun, size_t iv, bool res)
{
	if(m_PrevRun!=irun||m_PrevVar!=iv) {
		if(irun==110000) {
			irun=0;
			m_PrevRun=110000;
		}
		else
			m_PrevRun=irun;
		std::streamsize newpos=irun*(m_ExtraRecL*m_NumVar)+iv*(m_ExtraRecL);
		m_ExtraFile.seekg(newpos,ios::beg);
		m_ExtraFile.read((char*)&*m_LogLiNew,m_ExtraRecL);	
		if(m_SortedFileOpen) {
			m_SortedResidualFile.seekg(newpos,ios::beg);
			m_SortedResidualFile.read((char*)&*m_SortedResiduals, m_ExtraRecL);	
		}		
		m_PrevVar=iv;
	}
	if(res&&m_SortedFileOpen)
		return StatUtil::Normal_Inv(double(.95),double(*(m_SortedResiduals+m_RecL)), double(*(m_SortedResiduals+m_RecL+1)));	
	else if(res)
		return MISSING;
	else
		return StatUtil::Normal_Inv(double(.95),double(*(m_LogLiNew+m_RecL)), double(*(m_LogLiNew+m_RecL+1)));
}
float CResiduals::GetZ_ScoreLogLiP5(size_t irun, size_t iv, bool res)
{
	if(m_PrevRun!=irun||m_PrevVar!=iv) {
		if(irun==110000) {
			irun=0;
			m_PrevRun=110000;
		}
		else
			m_PrevRun=irun;
		std::streamsize newpos=irun*(m_ExtraRecL*m_NumVar)+iv*(m_ExtraRecL);
		m_ExtraFile.seekg(newpos,ios::beg);
		m_ExtraFile.read((char*)&*m_LogLiNew,m_ExtraRecL);	
		if(m_SortedFileOpen) {
			m_SortedResidualFile.seekg(newpos,ios::beg);
			m_SortedResidualFile.read((char*)&*m_SortedResiduals, m_ExtraRecL);	
		}
		
		m_PrevVar=iv;

	}
	if(res&&m_SortedFileOpen)
		return StatUtil::Normal_Inv(double(.05),double(*(m_SortedResiduals+m_RecL)), double(*(m_SortedResiduals+m_RecL+1)));	
	else if(res)
		return MISSING;
	else
		return StatUtil::Normal_Inv(double(.05),double(*(m_LogLiNew+m_RecL)), double(*(m_LogLiNew+m_RecL+1)));
}
float CResiduals::GetLogLiP95(size_t irun, size_t iv, bool res)

{
	if(m_PrevRun!=irun||m_PrevVar!=iv) {
		if(irun==110000) {
			irun=0;
			m_PrevRun=110000;
		}
		else
			m_PrevRun=irun;
		std::streamsize newpos=irun*(m_ExtraRecL*m_NumVar)+iv*(m_ExtraRecL);
		m_ExtraFile.seekg(newpos,ios::beg);
		m_ExtraFile.read((char*)&*m_LogLiNew,m_ExtraRecL);	
		if(m_SortedFileOpen) {
			m_SortedResidualFile.seekg(newpos,ios::beg);
			m_SortedResidualFile.read((char*)&*m_SortedResiduals, m_ExtraRecL);	
		}
		
		m_PrevVar=iv;

	}
	if(res&&m_SortedFileOpen)
		return *(m_SortedResiduals+m_RecL+5);	
	else if(res)
		return MISSING;
	else
		return *(m_LogLiNew+m_RecL+5);
}
float CResiduals::GetLogLiPMAX(size_t irun, size_t iv, bool res)

{
	if(m_PrevRun!=irun||m_PrevVar!=iv) {
		if(irun==110000) {
			irun=0;
			m_PrevRun=110000;
		}
		else
			m_PrevRun=irun;
		std::streamsize newpos=irun*(m_ExtraRecL*m_NumVar)+iv*(m_ExtraRecL);
		m_ExtraFile.seekg(newpos,ios::beg);
		m_ExtraFile.read((char*)&*m_LogLiNew,m_ExtraRecL);	
		if(m_SortedFileOpen) {
			m_SortedResidualFile.seekg(newpos,ios::beg);
			m_SortedResidualFile.read((char*)&*m_SortedResiduals, m_ExtraRecL);	
		}		
		m_PrevVar=iv;

	}
	if(res&&m_SortedFileOpen)
		return *(m_SortedResiduals+m_RecL+6);	
	else if(res)
		return MISSING;
	else
		return *(m_LogLiNew+m_RecL+6);
}
bool CResiduals::ReadHeader() {
	auto koll=m_ResidualFile.tellg();
	unsigned int RecL, NumRec, Numv;

	if (m_FileSize == -1) {
		m_ResidualFile.read((char*)&NumRec, 4);
	}
	bool check=true;

	m_ResidualFile.seekg(0, ios::end);

	m_FileSize=m_ResidualFile.tellg();//koll.seekpos;
	if (m_FileSize == 0) return false;

	m_ResidualFile.seekg(0, ios::beg);

	m_ResidualFile.read((char*)&NumRec,4);
	m_NumRec=NumRec;
	if(!m_ResidualFile.good()) check=false; 

	m_ResidualFile.read((char*)&RecL,4);
	m_RecL=RecL;
	if(!m_ResidualFile.good()) check=false; 
	m_ResidualFile.read((char*)&Numv,4);
	m_NumVar=Numv;
	m_MaxNumRec=size_t(m_FileSize/(m_RecL*m_NumVar*4));
	if (m_pNewMap != nullptr) {

		if (m_pNewMap->m_MultiRun_Array[0].NumberOfRepeatitionsWithinDimension < m_NumRec) {
			if (m_pNewMap->m_MultiRun_Array.size() == 1)  m_NumRec = m_pNewMap->m_MultiRun_Array[0].NumberOfRepeatitionsWithinDimension * m_NumVar;
			else m_NumRec = 0;
			for (size_t i = 1; i < m_pNewMap->m_MultiRun_Array.size(); i++) {
				m_NumRec += m_pNewMap->m_MultiRun_Array[i].NumberOfRepeatitionsWithinDimension*m_NumVar;

			}
		}

	}

	if(m_MaxNumRec<m_NumRec/m_NumVar) {
#ifndef COUPSTD
			if(m_pNewMap!=nullptr) {
				m_pNewMap->m_ErrorCount++;
				if(m_pNewMap->m_ErrorCount<10) {
					string str;
					str="For some reason the file \n("; str+=m_FileName;str+=")\n contains only :"+FUtil::STD_ItoAscii(m_MaxNumRec)+" Runs";
					str+="\nAdjust the Number of Multi Runs to correspond with that number";
					str+="\nExpected Number was: ";
					str+=FUtil::STD_ItoAscii(m_NumRec/m_NumVar);
					MFC_Util::MessageBox( str.c_str(),"Lacking data in XBin" ,MB_OK);
				}
		}
#endif		

	} 

	return check;

}

vector<float> CResiduals::GetResidualVector(size_t irun, size_t iv)
{

	std::vector <float> v;
	v.resize(m_RecL);
		std::streamsize byte=4;
		std::streamsize newpos;

		if (irun < 102) {
			newpos = std::streamsize(irun) * (std::streamsize(m_RecL) * std::streamsize(m_NumVar)) + iv * m_RecL + m_RecL;
		}
		else {
			newpos = std::streamsize(irun) * (std::streamsize(m_RecL) * std::streamsize(m_NumVar)) + iv * m_RecL + m_RecL;

		}
		if(m_RecL<3) newpos=irun*(m_RecL*m_NumVar)+iv*m_RecL+3;
		newpos=newpos*4;

		/*fpos_t byte=4;
		fpos_t newpos=size_t(irun)*(m_RecL*m_NumVar)+iv*m_RecL+m_RecL;
		if(m_RecL<3) newpos=size_t(irun)*(m_RecL*m_NumVar)+iv*m_RecL+3;
		newpos=newpos*byte;*/
		m_ResidualFile.seekg(newpos,ios::beg);
		
		if(!m_ResidualFile.good()) {
			ReOpenFile(true);
			m_ResidualFile.seekg(newpos,ios::beg);

		}
		if(newpos<=m_FileSize) {
			m_ResidualFile.seekg(newpos,ios::beg);
			m_ResidualFile.read((char*)&v[0],m_RecL*4);

		}
		else {
			ReOpenFile(true);
			v.resize(0);
			return v;
		}

		if(!m_ResidualFile.good()) {
			ReOpenFile(true);
			m_ResidualFile.seekg(newpos,ios::beg);
			v.resize(0);
		};
		
		m_PrevVar=iv;

	//}


	return v;
}
float CResiduals::GetResiduals(size_t irun, size_t iv, size_t irec)

{
	if (m_PrevRun != irun || m_PrevVar != iv) {
		if (irun == 110000) {
			irun = 0;
			m_PrevRun = 110000;
		}
		else
			m_PrevRun = irun;
        
        std::streamsize byte=4;
        std::streamsize newpos=irun*(m_RecL*m_NumVar)+iv*m_RecL+m_RecL;
        if(m_RecL<3) newpos=irun*(m_RecL*m_NumVar)+iv*m_RecL+3;
        newpos=newpos*byte;


		m_ResidualFile.seekg(newpos, ios::beg);
		if (!m_ResidualFile.good()) {
			ReOpenFile();
			m_ResidualFile.seekg(newpos, ios::beg);
		}
		if (newpos <= m_FileSize) {
			m_ResidualFile.read((char*)&m_Residuals[0], m_RecL * 4);
		}

		m_PrevVar = iv;

	}

	return m_Residuals[irec];
}

bool CResiduals::SaveMeanValues()
{
	auto actpos = m_ResidualFile.tellp();
	auto actpos2 = actpos;
	actpos2 = size_t(m_TotNoRuns)*(m_RecL*m_NumVar) + m_RecL;
	if (m_RecL < 3) actpos2 = size_t(m_TotNoRuns)*(m_RecL*m_NumVar) + 3;

	actpos2= actpos2*4;
	m_ResidualFile.seekp(actpos2, ios::beg);
	std::streamsize newpos = m_ResidualFile.tellp();
	if (actpos2 != newpos) {
		ReOpenFile();
		m_ResidualFile.seekp(newpos, ios::beg);
		actpos = m_ResidualFile.tellp();
		if (actpos2 != newpos)  return false;
	}

	m_ResidualFile.write((char*)&m_MeanPost.mean[0][0], m_RecL * 4 * m_NumVar);
	m_ResidualFile.write((char*)&m_MeanPost.min[0][0], m_RecL * 4 * m_NumVar);
	m_ResidualFile.write((char*)&m_MeanPost.max[0][0], m_RecL * 4 * m_NumVar);
	m_ResidualFile.write((char*)&m_MeanPost.std[0][0], m_RecL * 4 * m_NumVar);
	m_ResidualFile.write((char*)&m_MeanPrior.mean[0][0], m_RecL * 4 * m_NumVar);
	m_ResidualFile.write((char*)&m_MeanPrior.min[0][0], m_RecL * 4 * m_NumVar);
	m_ResidualFile.write((char*)&m_MeanPrior.max[0][0], m_RecL * 4 * m_NumVar);
	m_ResidualFile.write((char*)&m_MeanPrior.std[0][0], m_RecL * 4 * m_NumVar);
	m_ResidualFile.write((char*)&m_MeanPost.p10[0][0], m_RecL * 4 * m_NumVar);
	m_ResidualFile.write((char*)&m_MeanPost.p50[0][0], m_RecL * 4 * m_NumVar);
	m_ResidualFile.write((char*)&m_MeanPost.p90[0][0], m_RecL * 4 * m_NumVar);
	if (m_ResidualFile.good()) {
		actpos = m_ResidualFile.tellp();
		m_IsUpdated = true;
		m_MeanDefined = true;
		m_ResidualFile.seekg(0, ios::end);
		auto koll = m_ResidualFile.tellg();
		m_FileSize = m_ResidualFile.tellg();//koll.seekpos;
		ReOpenFile();


		m_ResidualFile.seekg(0, ios::end);
		m_FileSize = m_ResidualFile.tellg();
		m_ResidualFile.seekg(0, ios::beg);
	}
	else {
		return false;

	}
	return true;
}

bool CResiduals::ReOpenFile(bool ReadOnly) {

	m_ResidualFile.close();
	m_ResidualFile.clear();
	if(ReadOnly)
		m_ResidualFile.open(m_FileName, ios::binary | ios::in);
	else
		m_ResidualFile.open(m_FileName, ios::binary | ios::in|ios::out);

	size_t ikoll = 0;
	if (m_FileName.find("V2_COUP_001570.XBin") != string::npos) {
		ikoll = 1;

	}
	ReadHeader();
	return m_ResidualFile.is_open();
}
bool CResiduals::ReadMeanValues()
{
	std::streamsize newpos = m_TotNoRuns*m_RecL*m_NumVar + m_RecL;
	if (m_RecL < 3) newpos = m_TotNoRuns*m_RecL*m_NumVar + 3;
	newpos *= 4;
    
	m_ResidualFile.seekg(newpos, ios::beg);
	if (m_pNewMap != nullptr) {
		if (m_pNewMap->m_FileVersionNumberRead > 65) {

			if (!EnsembleReadWrite(true, "Name")) return false;

		}
		else {
			m_ResidualFile.read((char*)&m_MeanPost.mean[0][0], m_RecL * 4 * m_NumVar);
			m_ResidualFile.read((char*)&m_MeanPost.min[0][0], m_RecL * 4 * m_NumVar);
			m_ResidualFile.read((char*)&m_MeanPost.max[0][0], m_RecL * 4 * m_NumVar);
			m_ResidualFile.read((char*)&m_MeanPost.std[0][0], m_RecL * 4 * m_NumVar);
			m_ResidualFile.read((char*)&m_MeanPrior.mean[0][0], m_RecL * 4 * m_NumVar);
			m_ResidualFile.read((char*)&m_MeanPrior.min[0][0], m_RecL * 4 * m_NumVar);
			m_ResidualFile.read((char*)&m_MeanPrior.max[0][0], m_RecL * 4 * m_NumVar);
			m_ResidualFile.read((char*)&m_MeanPrior.std[0][0], m_RecL * 4 * m_NumVar);
			m_ResidualFile.read((char*)&m_MeanPost.p10[0][0], m_RecL * 4 * m_NumVar);
			m_ResidualFile.read((char*)&m_MeanPost.p50[0][0], m_RecL * 4 * m_NumVar);
			m_ResidualFile.read((char*)&m_MeanPost.p90[0][0], m_RecL * 4 * m_NumVar);
		}
	}
	if(m_ResidualFile.good()){	
		m_IsAllUpdated=true;
		m_IsUpdated=true;
		m_MeanDefined=true;
		if(*m_MinOfAll>=*m_MaxOfAll)
			m_IsAllUpdated=false;
		return true;
	}
	else {
		m_ResidualFile.clear();
		return true;
	}

}

bool CResiduals::SmartUpdateStatistics(bool AcceptedOnly, void *pScanner)
{
 	if (m_Calculating) return false;
#ifndef COUPSTD	
	m_Calculating = true;
#endif

	bool BayesMeanMethod;
	BayesMeanMethod = false;
	size_t BURN_PERIOD = 100;
	if (m_pNewMap->m_MStorage.GetMeanCriteria(1) == 10)
		BayesMeanMethod = true;
	
	string EnsembleName = m_pNewMap->m_MStorage.m_AcceptedIdString[m_pNewMap->m_MStorage.m_ActualEnsemble];
	if (m_Ensembles.size() <= m_pNewMap->m_MStorage.m_ActualEnsemble) {
		POS_ENSEMBLE ensemble_inf;
		ensemble_inf.pos = 0;
		ensemble_inf.name = EnsembleName;
		ensemble_inf.valid = false;
		ensemble_inf.EnsembleFileRead = false;
		m_Ensembles.push_back(ensemble_inf);
	}
	if (!m_Ensembles[m_pNewMap->m_MStorage.m_ActualEnsemble].EnsembleFileRead) {
		AlloResVector();
		EnsembleReadWrite(true, m_pNewMap->m_MStorage.m_ActualEnsemble);
		m_Ensembles[m_pNewMap->m_MStorage.m_ActualEnsemble].EnsembleFileRead = false;
	}
	else 
		m_Ensembles[m_pNewMap->m_MStorage.m_ActualEnsemble].EnsembleFileRead = false;

#ifndef COUPSTD
	CMainFrame *p_mainframe;
	p_mainframe = MySingle::instance().GetFramePointer();
#endif
	if (m_pNewMap->m_MStorage.m_AcceptedRuns.size() != m_Ensembles.size()) {
		m_Ensembles.resize(m_pNewMap->m_MStorage.m_AcceptedRuns.size());
	}

	for (size_t iens = 0; iens < m_pNewMap->m_MStorage.m_AcceptedRuns.size(); iens++) {
		m_IsUpdated = true;
		for (size_t i = 0; i < m_Ensembles.size(); i++) {
			if (!m_Ensembles[i].EnsembleFileRead) {
				m_IsUpdated = false;
			}
		}


		if (!m_IsUpdated) {
			m_NumAccepted = m_pNewMap->m_MStorage.m_AcceptedRuns[iens].size();
			size_t bayesmeannumber;
			bayesmeannumber = m_TotNoRuns - BURN_PERIOD;
			int RelPscan = 0;
			vector <float> Allv;

			for (size_t ii = 0; ii < m_NumVar*m_ScalingNumVarWithDim1; ii++) {
				if (!m_IsUpdated) {
					size_t ifile;
					CResiduals *pRes;
					size_t irun_int;
					vector<vector<float>> AcceptedResiduals;
					AcceptedResiduals.resize(m_NumAccepted);

					size_t var_to_read=ii;
					size_t run_to_start = 0;
					if (m_ScalingNumVarWithDim1 > 1) {
						var_to_read = 0;
						run_to_start = ii;
					}
					for (size_t i = 0; i < m_NumAccepted; i+= m_ScalingNumVarWithDim1) {
						AcceptedResiduals[i].resize(m_RecL);
						size_t iaccept = i;
						size_t irun = m_pNewMap->m_MStorage.m_AcceptedRuns[iens][iaccept];
 						ifile = m_pNewMap->MR_GetAddedFiles(irun);
 						size_t run_to_read = irun;
						if (ifile == 0)
							AcceptedResiduals[i] = GetResidualVector(run_to_read, var_to_read);
						else {
							pRes = m_pNewMap->GetValidResidualPointer(m_FileNo, ifile);
							irun_int = m_pNewMap->MR_GetInternalRunNo(irun);
							run_to_read = irun_int;
							if (m_ScalingNumVarWithDim1 > 1) run_to_read = (irun_int / m_ScalingNumVarWithDim1)*m_ScalingNumVarWithDim1 + ii % m_ScalingNumVarWithDim1;

							if (pRes == nullptr) {
								return false;
							}
							else if (run_to_read == 216&& ifile == 6 ) {
								int i1 = 4;

							}
							AcceptedResiduals[i] = pRes->GetResidualVector(run_to_read, var_to_read);
							//value = pRes->GetResiduals(irun_int, ii, index);
						}
					}



					for (size_t index = 0; index < m_RecL; index++) {
						if (index % 200 == 0) {
#ifndef COUPSTD	
							float scale;
							scale = float(index) / float(m_RecL) / float(m_NumVar) + float(ii) / float(m_NumVar);
							p_mainframe->UpdateResidualProgress(ii + 1, m_NumVar, scale);
#endif
						}
						vector<float> FArray;
						FArray.clear();
						for (size_t i = 0; i < m_NumAccepted; i+=m_ScalingNumVarWithDim1) {
							float value = AcceptedResiduals[i][index];
							if (abs(value) < abs(MISSING))
								FArray.push_back(value);
						}
						if (m_MeanPost.mean.size() <= ii) {
							AlloResVector();


						}
						if (FArray.size() >= 3) {
							//BAYESIANCALIB_mp_DIST();
							DistStat Stat;
							size_t ncheck;
							StatUtil::SortArray(&FArray[0], FArray.size());
							MeanStat(&FArray[0], FArray.size(), &ncheck, &Stat);
							m_MeanPost.mean[ii][index] = Stat.Mean;
							m_MeanPost.min[ii][index] = Stat.Min;
							m_MeanPost.max[ii][index] = Stat.Max;
							m_MeanPost.std[ii][index] = Stat.Std;
							m_MeanPost.p10[ii][index] = Stat.P5;
							m_MeanPost.p50[ii][index] = Stat.P50;
							m_MeanPost.p90[ii][index] = Stat.P95;
						}
						else {

							m_MeanPost.mean[ii][index] = float(MISSING);
							m_MeanPost.min[ii][index] = float(MISSING);
							m_MeanPost.max[ii][index] = float(MISSING);
							m_MeanPost.std[ii][index] = float(MISSING);
							m_MeanPost.p10[ii][index] = float(MISSING);
							m_MeanPost.p50[ii][index] = float(MISSING);
							m_MeanPost.p90[ii][index] = float(MISSING);
						}
						if (ii == 1) {

							int koll = 3;

						}
					}

				}

			} 

			//BAYESIANCALIB_mp_DIST_END();
			m_NewPrior = false;
			if (!AcceptedOnly) 	m_NewPrior = true;

			//	m_pNewMap->m_MStorage.m_AcceptedIdString[m_pNewMap->m_MStorage.m_ActualEnsemble]
			EnsembleReadWrite(false, iens);
			
			m_Ensembles[iens].valid = true;
			m_MeanDefined = true;

			if (!AcceptedOnly) m_IsAllUpdated = true;
			//	SaveMeanValues();

		}



	}

#ifndef COUPSTD

	MyCug *pCug;
	pCug = MySingle::instance().GetCugPointer();
	pCug->OnSheetSetup(6);
	p_mainframe->UpdateResidualProgress(12, m_NumVar, 1.0);
	p_mainframe->m_wndEvalMulti.m_ValidationCug.OnSetup();
#endif
#ifndef COUPSTD
	m_Calculating = false;
#endif
	return true;
}
size_t CResiduals::GetNumAccepted()
{
	return m_NumAccepted;
}
bool CResiduals::ReWriteExistingFile(size_t NoRunsToKeep)
{

	if(m_IsOpen)
		CloseFile();
	m_ResidualFile.open(m_FileName, ios::binary|ios::out|ios::in);
	if(!m_ResidualFile.is_open())
	{
		return false;
	}

	bool check;
	check=true;
	// Read existingFile
	size_t NumRec, NumVar;
	m_ResidualFile.read((char*)&NumRec,4);
	if(!m_ResidualFile.good()) check=false; 
	m_ResidualFile.read((char*)&m_RecL,4);
	if(!m_ResidualFile.good()) check=false; 
	m_ResidualFile.read((char*)&NumVar,4);
	m_TotNoRuns=NumRec/m_NumVar;
	size_t RecL=m_RecL;

	
	m_NumRec=NoRunsToKeep*m_NumVar;
	AlloResVector();

	m_Residuals.resize(RecL);
	m_Allocated=true;


	m_ResidualFile.seekp(0,ios::beg);
    m_ResidualFile.write((char*)&m_NumRec,4);

	std::streampos newpos=(m_RecL*m_NumVar+2)*4*NoRunsToKeep;
	//m_ResidualFile.SetLength(newpos);
	m_ResidualFile.close();
	m_IsOpen=false;


	return true;
}
bool CResiduals::AddExistingFile(string AddFileName,  void *pScanner)
{
	if(!FUtil::IsFileExisting(AddFileName)) 
		return false;
	if(m_IsOpen)
		CloseFile();
#ifndef COUPSTD
	CFileScanner *pScan;
	pScan=(CFileScanner*)pScanner;
#endif
	m_ResidualFile.open(m_FileName, ios::binary|ios::out|ios::in);

	if(m_ResidualFile.is_open())
	{
		return false;
	}

	bool check;
	check=true;
	// Read existingFile
	size_t  NumRec, NumVar;
	m_ResidualFile.read((char*)&NumRec,4);
	if(!m_ResidualFile.good()) check=false; 
	m_ResidualFile.read((char*)&m_RecL,4);
	if(!m_ResidualFile.good()) check=false; 
	m_ResidualFile.read((char*)&NumVar,4);
	m_TotNoRuns=NumRec/m_NumVar;
	size_t RecL=m_RecL;
	// Read New File
	m_AddFile.read((char*)&m_NumRecAdd,4);
	if(!m_AddFile.good()) check=false; 
	m_AddFile.read((char*)&m_RecLAdd,4);
	if(!m_AddFile.good()) check=false; 
	m_ResidualFile.read((char*)&m_NumVarAdd,4);
	if(m_NumVar!=m_NumVarAdd||m_RecL!=m_RecLAdd){
			
		
	};

	m_TotNoRuns=(NumRec+m_NumRecAdd)/m_NumVar;
	m_OldRuns=NumRec/m_NumVar;
	m_NewRuns=m_NumRecAdd/m_NumVar;
	m_Residuals.resize(RecL);
	AlloResVector();

	m_Allocated=true;


	std::streamsize newpos=m_RecL+m_OldRuns*m_RecL*m_NumVar;
	std::streamsize pos;
	m_ResidualFile.seekp(newpos*4,ios::beg);
	pos=m_ResidualFile.tellp();
	std::streamsize newposAdd=m_RecL*4;
	std::streamsize actpos;
	m_AddFile.seekg(newposAdd,ios::beg);
	m_AddFile.tellg();
	

	for(size_t i=0; i<m_NewRuns;i++) {
		for(size_t ivar=0;ivar<m_NumVar;ivar++) {
			m_AddFile.read((char*)&m_Residuals[0],m_RecL*4);
			auto nbyte=m_AddFile.tellg();
			float v;
			v=m_Residuals[0];
			m_ResidualFile.write((char*)&m_Residuals[0], m_RecL*4);
			actpos=m_ResidualFile.tellp();
			actpos=m_AddFile.tellg();
		}
		if(i%100==0) {
			
#ifndef COUPSTD
			size_t RelPscan=i*100/m_NewRuns;
			if(pScan!=nullptr) pScan->Update(RelPscan);
#endif
		}
	}
	m_ResidualFile.seekp(0,ios::beg);
    
	NumRec=m_TotNoRuns*m_NumVar;

	m_ResidualFile.write((char*)&NumRec,4);
	m_NumRec=NumRec;
	m_AddFile.close();
	m_IsOpen=true;


	return true;
}
float CResiduals::GetMeanResiduals( size_t iv, size_t index) {	return m_MeanPost.mean[iv][index]; }
float CResiduals::GetMinResiduals(size_t iv, size_t index)   {	return m_MeanPost.min[iv][index]; }
float CResiduals::GetMaxResiduals(size_t iv, size_t index)   {	return m_MeanPost.max[iv][index]; }
float CResiduals::GetStdResiduals(size_t iv,size_t index){	return m_MeanPost.std[iv][index];}
float CResiduals::GetP10Residuals(size_t iv,size_t index){	return m_MeanPost.p10[iv][index];}
float CResiduals::GetP90Residuals(size_t iv,size_t index){	return m_MeanPost.p90[iv][index];}
float CResiduals::GetP50Residuals(size_t iv,size_t index){	return m_MeanPost.p50[iv][index];}
float CResiduals::GetMeanAllResiduals( size_t iv, size_t index){return m_MeanPrior.mean[iv][index];}
float CResiduals::GetMinAllResiduals(size_t iv, size_t index){	return m_MeanPrior.min[iv][index];}
float CResiduals::GetMaxAllResiduals(size_t iv, size_t index){	return m_MeanPrior.max[iv][index];}
float CResiduals::GetStdAllResiduals(size_t iv,size_t index) {	return m_MeanPrior.std[iv][index];}
float CResiduals::GetP10AllResiduals(size_t iv,size_t index) {	return m_MeanPrior.p10[iv][index];}
float CResiduals::GetP90AllResiduals(size_t iv,size_t index) {	return m_MeanPrior.p90[iv][index];}
float CResiduals::GetP50AllResiduals(size_t iv,size_t index) {	return m_MeanPrior.p50[iv][index];}
bool CResiduals::ToBeUpdated()
{
	bool hit = false;
	string EnsembleName=m_pNewMap->m_MStorage.m_AcceptedIdString[m_pNewMap->m_MStorage.m_ActualEnsemble];
	for (size_t i = 0; i < m_Ensembles.size(); i++) {
		if (EnsembleName == m_Ensembles[i].name) {
			m_Ensembles[i].valid = false;
			m_Ensembles[i].EnsembleFileRead = false;
			hit = true;
			continue;
		}
		else if (m_Ensembles[i].name.size() == 0) {
			m_Ensembles[i].valid = false;
			m_Ensembles[i].name = m_pNewMap->m_MStorage.m_AcceptedIdString[i];
//			m_Ensembles[i].name = EnsembleName;
		}
	}
	if (m_Ensembles.size() == 0||( m_pNewMap->m_MStorage.m_AcceptedIdString.size()>m_Ensembles.size()&&!hit)) {

			POS_ENSEMBLE ensemble_inf;
			ensemble_inf.pos = 0;
			ensemble_inf.name = EnsembleName;
			ensemble_inf.valid = false;
			ensemble_inf.EnsembleFileRead = false;
			m_Ensembles.push_back(ensemble_inf);

	}
	else if (!hit&&m_pNewMap->m_MStorage.m_AcceptedIdString.size()==m_Ensembles.size()) {
		for (size_t i = 0; i < m_pNewMap->m_MStorage.m_AcceptedIdString.size(); i++) {
			if(m_pNewMap->m_MStorage.m_AcceptedIdString[i].size()==0)  m_pNewMap->m_MStorage.m_AcceptedIdString[i] = m_Ensembles[i].name;
			m_Ensembles[i].valid = false;
		}
	}

	m_MeanDefined = false;
	m_IsUpdated=false;
	return true;
}
bool CResiduals::UpdateStatisticsOfIndicators(bool AcceptedOnly, void *pScanner )
{
	float sumv, minv, maxv;
	float sumv_a, minv_a, maxv_a;
	float value;
#ifndef COUPSTD
	CWinApp* pApp = AfxGetApp();
	pApp->BeginWaitCursor();
#endif

	bool BayesMeanMethod;
	BayesMeanMethod=false;
	int BURN_PERIOD=100;

	if(!m_IsUpdated_Extra||!m_IsAllUpdated_Extra) {
		
		float values[13];
		m_NumAccepted=m_pNewMap->m_MStorage.GetNumAccepted();
		bool residualswitch;
		
		for (size_t jj=0;jj<2;jj++) {
			residualswitch=false;
			if(jj==1) residualswitch=true;
			for(size_t ii=0;ii<m_NumVar;ii++) {
#ifndef COUPSTD
				CFileScanner *pScan;
				pScan=(CFileScanner*)pScanner;
				size_t RelPscan=ii*100/m_NumVar;
				if(pScan!=nullptr) pScan->Update(RelPscan);
#endif

				for(int index=0; index<13;index++) {
						sumv_a=sumv=0;
						maxv_a=maxv=float(MISSING);
						minv_a=minv=1.1E38f;
						size_t irun, irunprev, countmean;
						
						if(index%13==0) {
#ifndef COUPSTD
							size_t RelPscan=ii*100/m_NumVar;
							if(pScan!=nullptr) pScan->Update(RelPscan);
#endif
						}
						if(!m_IsUpdated_Extra) {
							irunprev=0;
							countmean=0;
							//BAYESIANCALIB_mp_DIST_ALLO(&m_NumAccepted);
							float *FArray;
							FArray= new float [m_NumAccepted];
							for(size_t i=0; i<m_NumAccepted;i++) {
									
									irun=m_pNewMap->m_MStorage.Get_AcceptedRuns(i);
									switch (index) {
										case (0):
											value=GetLogLiMean(irun, ii, residualswitch);
											break;
										case (1):
											value=GetLogLiCV(irun, ii, residualswitch);
											break;
										case (2):
											value=GetLogLiPMIN(irun, ii, residualswitch);
											break;
										case (3):
											value=GetLogLiPMAX(irun, ii, residualswitch);
											break;
										case (4):
											value=GetLogLiP5(irun, ii, residualswitch);
											break;
										case (5):
											value=GetLogLiP50(irun, ii, residualswitch);
											break;
										case (6):
											value=GetLogLiP95(irun, ii, residualswitch);
											break;
										case (7):
											value=GetZ_ScoreLogLiP5(irun,ii,residualswitch) ;
											break;
										case (8):
											value=GetLogLiMean(irun, ii, residualswitch);
											break;
										case (9):
											value=GetZ_ScoreLogLiP95(irun,ii, residualswitch);
											break;
										case (10):
											value=GetLogLiP5(irun, ii, residualswitch)-GetZ_ScoreLogLiP5(irun,ii,residualswitch) ;
											break;
										case (11):
											value=GetLogLiP50(irun, ii, residualswitch)-GetLogLiMean(irun, ii, residualswitch);
											break;
										case (12):
											value=GetLogLiP95(irun, ii, residualswitch)-GetZ_ScoreLogLiP95(irun,ii, residualswitch);
											break;
									}
									if(value<1.E38&&value>-1.E38&&abs(value)>1.E-20) {			
										//BAYESIANCALIB_mp_ADDX(&value);	
										*(FArray+countmean)=value;	
										irunprev=irun;
										countmean++;
									}
									else {
										int counte;
										counte=1;							
									}						
							}
							if(countmean>=1) {
								DistStat Stat;
								size_t ncheck;
								StatUtil::SortArray(FArray,countmean);
								MeanStat(FArray,countmean, &ncheck,&Stat);
								//BAYESIANCALIB_mp_DIST();

								values[0]=Stat.Mean;
								values[1]=Stat.Std;
									values[2]=Stat.Min;
									values[3]=Stat.Max;
									values[4]=Stat.P5;
									values[5]=Stat.P50;
									values[6]=Stat.P95;
									if(!residualswitch)
										m_pNewMap->m_MStorage.SetValidDynLogLi(ii, index,0, &values[0]);
									else
										m_pNewMap->m_MStorage.SetValidDynResiduals(ii, index,0, &values[0]);

							}
							else {
								m_MeanPost.mean[ii][index]=float(MISSING);

							}
							delete [] FArray;
							//BAYESIANCALIB_mp_DIST_END();
						}

						if(!AcceptedOnly&&!m_IsAllUpdated_Extra) {
							float *FArray;
							FArray= new float [m_TotNoRuns];
							countmean=0;
							//BAYESIANCALIB_mp_DIST_ALLO(&m_TotNoRuns);
						
							for(size_t i=0; i<m_TotNoRuns;i++) {
									
									switch (index) {
										case (0):
											value=GetLogLiMean(i, ii, residualswitch);
											break;
										case (1):
											value=GetLogLiCV(i, ii, residualswitch);
											break;
										case (2):
											value=GetLogLiPMIN(i, ii, residualswitch);
											break;
										case (3):
											value=GetLogLiPMAX(i, ii, residualswitch);
											break;
										case (4):
											value=GetLogLiP5(i, ii, residualswitch);
											break;
										case (5):
											value=GetLogLiP50(i, ii, residualswitch);
											break;
										case (6):
											value=GetLogLiP95(i, ii, residualswitch);
											break;
										case (7):
											value=GetZ_ScoreLogLiP5(i,ii,residualswitch) ;
											break;
										case (8):
											value=GetLogLiMean(i, ii, residualswitch);
											break;
										case (9):
											value=GetZ_ScoreLogLiP95(i,ii, residualswitch);
											break;
										case (10):
											value=GetLogLiP5(i, ii, residualswitch)-GetZ_ScoreLogLiP5(i,ii,residualswitch) ;
											break;
										case (11):
											value=GetLogLiP50(i, ii, residualswitch)-GetLogLiMean(i, ii, residualswitch);
											break;
										case (12):
											value=GetLogLiP95(i, ii, residualswitch)-GetZ_ScoreLogLiP95(i,ii, residualswitch);
											break;
									}
									*(FArray+countmean)=value;	
									countmean++;
							}
							
	
							if(m_TotNoRuns>2) {
									DistStat Stat;
									size_t ncheck;

									StatUtil::SortArray(FArray,countmean);
									MeanStat(FArray,countmean, &ncheck,&Stat);

									values[0]=Stat.Mean;
									values[1]=Stat.Std;
									values[2]=Stat.Min;
									values[3]=Stat.Max;
									values[4]=Stat.P5;
									values[5]=Stat.P50;
									values[6]=Stat.P95;


									if(!residualswitch)
										m_pNewMap->m_MStorage.SetValidDynLogLi(ii, index,1, &values[0]);
									else
										m_pNewMap->m_MStorage.SetValidDynResiduals(ii, index,true, &values[0]);
									
	

							}
							delete[] FArray;
						
						}
						
				
						
				}			
			}
		}
		

		if(AcceptedOnly) {
			m_IsUpdated_Extra=true;
		}
		if(!AcceptedOnly) m_IsAllUpdated_Extra=true;
	}
#ifndef COUPSTD
	pApp->EndWaitCursor();
#endif
	return true;


}

// CResiduals member functions
