#ifndef NOSIMFILES
//#include "../stdafx.h"
#include "../std.h"
#endif
#include "../Util/FUtil.hpp"
#include "./PGUtil.h"

#include "PGFile.h"


#ifndef COUPSTD
#include "../CoupModelDoc.h"
#include "../CUG/SimDoc/MyCug.h"
#include "../CoupModelView.h"
#include "../MainFrm.h"
#include "../NewBase/FileScanner.h"
#endif
extern void *pGlobalCug;

//#include "../BasicExcel/mexcel.h"
//using namespace miniexcel;
//
//#include "../BasicExcel/BasicExcel.hpp"
//using namespace YExcel;

#define SAVEPATH "c:\\temp\\a.xls"


size_t CPGFile::GetHitPos(size_t VariableNo, size_t minst, size_t minend) {
	// This function is used for checking the best possible  position for 
	// making mean values of simulated variable. The m_ActHit will shift after
	// call to GetPos and will shift to the closest time points
	if(m_ActHit[VariableNo]>pg.Shape.NumRecords) m_ActHit[VariableNo]=pg.Shape.NumRecords;
	size_t actmin=GetLongTime(m_ActHit[VariableNo]);
	if(actmin<minend&&actmin>=minst)	
		return m_ActHit[VariableNo];
	else
		return 0;
}
bool CPGFile::Check_Update_ActHit(size_t varno, size_t minut) {
	bool change=false;
	if(m_ActHit[varno]>pg.Shape.NumRecords) return change;
	size_t actmin = GetLongTime(m_ActHit[varno]);
	while(minut>actmin) {
		m_ActHit[varno]++;
		if (m_ActHit[varno] == pg.Shape.NumRecords+1) return true;
		if (m_ActHit[varno]> GetNumRecords()) return false;
	
		change=true;
		actmin = GetLongTime(m_ActHit[varno]);
		if(m_ActHit[varno]>pg.Shape.NumRecords) return change;
	}
	return change;
}
void CPGFile::ResetPos()
{
	m_ActRecord.clear();
	for (size_t i = 0; i < pg.Shape.NumVar+1; i++) {
		m_ActRecord.push_back(1);
		m_ActHit.push_back(1);
	}
}
PGPOS CPGFile::GetPos(size_t minut, size_t varno ) 
{
	PGPOS out;
	size_t min_prior, min_post;




	/*if (m_EquiDistance > 0) {
		if (minut <= GetLongTimeStart()) {
			out.first = 1;
			out.second = 2;
			out.weight = 0.;


		}
		else {
			double dpos = (minut - GetLongTimeStart()) / m_EquiDistance + 1;
			out.first = dpos;
			out.second = dpos + 1;
			out.weight = double(dpos - out.first);
		}
		if(out.weight<0.501)
			m_ActHit=out.first;
		else
			m_ActHit=out.second;
	}
	else {*/

	if (varno != string::npos)
		while (GetVarValue(varno, m_ActRecord[varno]) <= float(MISSING) && m_ActRecord[varno] < pg.Shape.NumRecords) m_ActRecord[varno]++;
	else
		varno = 0;
		out.first = m_ActRecord[varno] - 1;

		if(varno!=string::npos) {
			while(int(minut-GetLongTime(m_ActRecord[varno]))>0&&m_ActRecord[varno]<pg.Shape.NumRecords) {
				m_ActRecord[varno]++;
				if(varno>0)
					while(GetVarValue(varno, m_ActRecord[varno])<=float(MISSING)&&m_ActRecord[varno]<pg.Shape.NumRecords) m_ActRecord[varno]++;
			}
		}
		else {
			while (minut>=GetLongTime(m_ActRecord[varno])&& m_ActRecord[varno] < pg.Shape.NumRecords) {
				//auto koll = GetLongTime(m_ActRecord);
				//auto koll2 = minut - koll;
				m_ActRecord[varno]++;
			}
			//	m_ActRecord++;
		}
		out.second=m_ActRecord[varno];


		if(m_ActRecord[varno] ==1) {
			out.first=1;
			out.second=1;
			out.weight=double(int(minut-GetLongTimeStart()));
			m_ActHit[varno] =m_ActRecord[varno];
		}
		else if(m_ActRecord[varno]<pg.Shape.NumRecords) {
			out.first=m_ActRecord[varno] -1;
			min_post=GetLongTime(out.first);
			min_prior=GetLongTime(out.second);
			if(min_post>minut)
				out.weight=0;
			else if(min_prior>min_post) 
				out.weight=double(minut-min_post)/double(min_prior-min_post);
			else
				out.weight=0.5;
			if(out.weight<0.5)
				m_ActHit[varno] =m_ActRecord[varno] -1;
			else
				m_ActHit[varno] =m_ActRecord[varno];

		}
		else if(m_ActRecord[varno] ==pg.Shape.NumRecords) {
			out.first=m_ActRecord[varno];
			out.weight=0;
			out.second=m_ActRecord[varno];
			m_ActHit[varno]=m_ActRecord[varno];
		}
		else {
			out.first=m_ActRecord[varno];
			out.weight=0;
			out.second=m_ActRecord[varno];
			m_ActHit=m_ActRecord;
		}
	//}
	if(out.first>pg.Shape.NumRecords) {
		out.first=pg.Shape.NumRecords;
		out.weight=0;
	}
	else if(out.first<1) {
		out.first=1;
		out.weight=0;
	}
	if(out.second>pg.Shape.NumRecords) {
		out.second=out.first;
		out.weight=0;
	}

	out.shift=true;
	return out;
};

void* CPGFile::GetRecordPointer(size_t var_rec, size_t rep)
{

	std::streamsize newpos=std::streamsize(pg.Shape.RecordSize)*var_rec+(rep-1)*pg.Shape.RecordSize_Repetion*4;
	m_MainPGStreamReadWrite.seekg(newpos,ios::beg);
	m_MainPGStreamReadWrite.read((char*)&pg.Var.v[0],pg.Shape.RecordSize);
	if(m_MainPGStreamReadWrite.good())
		return &pg.Var.v[0];
	return 
		nullptr;

}
vector<float> CPGFile::GetRecordDataVector(size_t var_rec, size_t rep)
{
	vector<float> vfloat;

	std::streamsize newpos = std::streamsize(pg.Shape.RecordSize) * var_rec + (rep - 1) * pg.Shape.RecordSize_Repetion * 4;
	m_MainPGStreamReadWrite.seekg(newpos, ios::beg);
	m_MainPGStreamReadWrite.read((char*)&pg.Var.v[0], pg.Shape.RecordSize);
	for (size_t i = 0; i <= pg.Shape.NumVar; i++)
		vfloat.push_back(pg.Var.v[i]);
	return vfloat;
}
bool CPGFile::SetPointerToAllData(float *varx) {
	for(size_t rec=0;rec<pg.Shape.NumRecords; rec++) {
		for(size_t i=0; i<=pg.Shape.NumVar;i++) {
			size_t ipos=i+rec*(pg.Shape.NumVar+1);
			if(ipos+pg.status.a.LocalStartIndex<pg.Var.v.size())
				pg.Var.v[ipos+pg.status.a.LocalStartIndex]=*(varx+ipos);
			else
				return false;
		}
	}
	return true;
}
void CPGFile::SetRecordPointer(float *varx)
{
	for(size_t i=0; i<=pg.Shape.NumVar;i++)
		pg.Var.v[i]=*(varx+i);
	//m_varx=varx;
}
void CPGFile::SetDatePointer(float *varx, size_t ist)
{
		pg.Var.v[0+ist]=*varx;
}
void CPGFile::SetDatePointer(unsigned int *pminute, size_t ist)
{
	pg.Var.v[0 + ist] = *(float*)pminute;
}
bool CPGFile::SetWritePosition(size_t var_rec)
{
	auto newpos=pg.Shape.RecordSize*var_rec;
	m_MainPGStreamReadWrite.seekp(newpos,ios::beg);
	return m_MainPGStreamReadWrite.good();
	//m_varx=varx;
}


void CPGFile::SetMissingValueRecordPointer(size_t ist, size_t num)
{
	for(size_t i=0; i<num;i++)
		pg.Var.v[ist+i+1]=float(MISSING);
}
void CPGFile::SetRecordPointer(float *varx, size_t ist, size_t num)
{
	for(size_t i=0; i<num;i++)
		pg.Var.v[ist+i+1]=*(varx+i+1);
}
void CPGFile::SetRecordPointer_2Mix(float *varx1, float *varx2, size_t ist, size_t num)
{
	for(size_t i=0; i<num;i++) {
		pg.Var.v[ist+i*2+1]=*(varx1+i+1);
		pg.Var.v[ist+i*2+2]=*(varx2+i+1);
	}
}



void CPGFile::SetVarPointer(float *varx, size_t ist, size_t num)
{
	if(ist+num<pg.Var.v.size())
	for(size_t i=0; i<num;i++)
		pg.Var.v[ist+i+1]=*(varx+i);


}
void CPGFile::SetVarPointer_2Mix(float *varx1, float *varx2, size_t ist, size_t num)
{
	for(size_t i=0; i<num;i++) {
		pg.Var.v[ist+i*2+1]=*(varx1+i);
		pg.Var.v[ist+i*2+2]=*(varx2+i+1);
	}
}
void CPGFile::SetRecordPointerInterpolate(float *varx, size_t ist, size_t num, float weight, const vector<float> &vfloat)
{
	for(size_t i=0; i<num;i++)
		pg.Var.v[ist+i+1]=*(varx+i+1)*weight+vfloat[i+1]*(1-weight);
}
void CPGFile::SetRecordInterpolated(const vector<float>& vprior, size_t ist, float weight, const vector<float>& vfloat)
{
	for (size_t i = 0; i < vprior.size(); i++)
		pg.Var.v[ist + i + 1] =vprior[i + 1] * weight + vfloat[i + 1] * (1 - weight);
}
std::vector <float> CPGFile::GetVarVector(size_t varno) {
	std::vector <float> vut;
	if (pg.status.CompleteFileRead) {
		vut.resize(pg.Shape.NumRecords);
		for (size_t j = 1; j <= pg.Shape.NumRecords; j++)
			vut[j - 1] = pg.Var.v[(j - 1)*pg.Shape.RecordIndexSize + varno];
	}
	else
		vut = ReadVarVector(varno);
	return vut;

}
std::vector <double> CPGFile::GetVarDVector(size_t varno) {
	std::vector <double> vut;
	vut.resize(pg.Shape.NumRecords);
	vector<float> vin = GetVarVector(varno);
	for (size_t i = 0; i < vut.size(); i++)
		vut[i] = vin[i];
	return vut;

}
CPGFile::CPGFile(void)
{
	m_IdField_Sensitive = true; m_LatLong_FieldSensitive=true;
	pg.status.b.NumberOfBuf=0;
	m_WriteEnabled=false;
	m_ResetBuffer=true;
	m_ActRecord.clear();
	m_ActRecord.push_back(1);
	pg.Shape.NumRecords=0;
	pg.Shape.NumVar=0;
	pg.status.FileName="MyNewFile";
	pg.status.CompleteFileRead=false;
	m_Closed=false;
		pg.Shape.RepetitionMode=false;
	m_NumberOfYears=string::npos;
	m_YearIndexStart=nullptr;
	m_YearRecords=nullptr;
	m_SectionYear=true; // false== HOUR
	m_YearShift=nullptr;
	m_DayShift=nullptr;
	m_ActualSection=0;
	m_EquiDistance=0;

	m_OutFileStatus=false;
		m_str=nullptr;
	m_IsOpen=false;
	m_startRec=1;
	m_varxMinutes=m_varxHours=m_varxDays=m_varxMonths=m_varxYears=false;
	m_varx_minute=nullptr;
	m_varx_hour=nullptr;
	m_varx_days=nullptr;
	m_varx_years=nullptr;
	pg.Shape.RecordSize=0;
	m_DataInFocus=0;
		GapProblemFinded=true;
	pg.status.NormalTimeInterval=1440;
	m_MissingTested=false;
	m_ActualSequence=0;
	pg.Shape.NumRepetitions=1;
	pg.status.FileVersion=1;
	m_IncreaseTimeTest= m_IncreaseTime=false;
	m_YearIndexStart=nullptr;
	m_YearRecords=nullptr;

}
CPGFile::~CPGFile(void)
{
if(m_str!=nullptr) delete [] m_str;
}


void CPGFile::SetVarValue(size_t varno, size_t var_rec, float vv)
{	
	if(pg.status.CompleteFileRead&&!pg.Shape.RepetitionMode)
		pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+varno]=vv;
	else if(pg.status.CompleteFileRead&&pg.Shape.RepetitionMode) {
		size_t koll=pg.Var.v.size();
		size_t ind=(var_rec-1+pg.status.a.LocalStartRecord)*pg.Shape.RecordIndexSize+varno;
		if(ind<koll)
			pg.Var.v[(var_rec-1+pg.status.a.LocalStartRecord)*pg.Shape.RecordIndexSize+varno]=vv;
	}
	else if(pg.Shape.RepetitionMode&&pg.status.b.PrevBuf==string::npos) 
		WritePGFileVarValue(varno, var_rec, vv, m_ActualSequence);
	else if(GetNumBuffers()<=1) {
		size_t CurrentRec=(var_rec-1)%pg.status.b.buf_Size*pg.Shape.RecordIndexSize;
		pg.Var.v[varno+CurrentRec]=vv;
	}
	else	{
		var_rec=var_rec+pg.status.a.LocalStartRecord;
		size_t CurrentBuf, CurrentRec;
		CurrentBuf=(var_rec-1)/pg.status.b.buf_Size+1;
		CurrentRec=(var_rec-1)%pg.status.b.buf_Size*pg.Shape.RecordIndexSize;

		if(pg.status.b.PrevBuf>0&&CurrentBuf!=pg.status.b.PrevBuf) {
			if(!WriteNewBuffer(CurrentBuf))
				return;
		}
		pg.Var.v[varno+CurrentRec]=vv;
	}

	
}
void CPGFile::SetDate(size_t var_rec, string str)
{
	size_t min=PGUtil::MinutConv(str);
	SetLongTime(var_rec, min);

}
bool CPGFile::SetLongTime(size_t var_rec, size_t t, bool WithinBuffer)
// Changed pg.status.b.PrevBuf from 0 to 1.
{
	if(pg.Var.v.size()>(var_rec-1)*pg.Shape.RecordIndexSize&&WithinBuffer) {	
		pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize]=*(float*)&t;
	}
	else {
		size_t CurrentBuf, CurrentRec;
		CurrentBuf=(var_rec-1)/pg.status.b.buf_Size+1;
		CurrentRec=(var_rec-1)%pg.status.b.buf_Size*pg.Shape.RecordIndexSize+1;
		if (CurrentBuf != pg.status.b.PrevBuf&&pg.status.b.PrevBuf != string::npos) {

			if(!WriteNewBuffer(CurrentBuf)) {
				return false;
			}
		}
		if (pg.Var.v.size() > CurrentRec)
			pg.Var.v[CurrentRec-1] = *(float*)&t;
		else
			return false;
		

	}
	return true;

}

bool CPGFile::SetRecordMissing(size_t var_rec)
{
		size_t CurrentBuf, CurrentRec;
		CurrentBuf = (var_rec - 1) / pg.status.b.buf_Size + 1;
		CurrentRec = (var_rec - 1) % pg.status.b.buf_Size*pg.Shape.RecordIndexSize + 1;

		if (pg.Var.v.size() >= CurrentRec+ pg.Shape.RecordIndexSize) {
			for (size_t ivar = CurrentRec; ivar < CurrentRec+pg.Shape.RecordIndexSize+1; ivar++)
				pg.Var.v[ivar] = MISSING;
		}
		else
			return false;
	return true;
}

string CPGFile::ExportToFile(bool secondhalf)
{
		string strNewFileName;
		

		//strNewFileName=UserDirectory;
		//CStdioFile NewFileDlg;
		string pszFileName ;

		pszFileName=pg.status.FileName;
		size_t pos=0;
		pos=pszFileName.find('.');
		if(pos!=string::npos)
			pszFileName=pszFileName.substr(0,pos);

#ifndef COUPSTD	

		FILEDLG FileDlg = MFC_Util::GetFileDialog("txt", "", "Ascii File (*.txt)|*.txt||");
		
		if(FileDlg.opengood==IDOK) {
			string str=FileDlg.filename;
			size_t pos=str.find("Multi_");
			bool split=false;
			if(pos>0&&pos<1000)
				split=true;
			return ExportToFile(str, split, secondhalf); 


		}
#endif

	return "";
}
string CPGFile::ExportToFile(string filename, bool split, bool secondhalf) 
{
	fstream file;
	string fileout, filebase;
	size_t splitno=1;

	if(split) {
		size_t pos;
		pos=filename.find(".txt");
		filebase=filename.substr(0,pos);
		filebase+="_";
		fileout=filebase;
		fileout+=FUtil::STD_ItoAscii(splitno);
		fileout+=".txt";
	}
	else
		fileout=filename;
	if(FUtil::IsFileExisting(fileout))
		file.open(fileout,ios::out|ios::in|ios::binary);
	else
		file.open(fileout,ios::out|ios::binary);

	if(!file.is_open()) return "";
	char buf[10000];
	size_t nbyte=1000;
	string str;
	size_t nrec=0;
	size_t nvar=0;
	size_t maxvar=0;
	size_t itimbase=GetLongTimeStart();
	string linebreak="\r\n";
#ifndef COUPSTD	
	CWinApp* pApp = AfxGetApp();
	pApp->BeginWaitCursor();
#endif	
	for(size_t i=0;i<9;i++) {
		str.clear();
		if(i==0)
			str="Variable Names:";
		else if(i==1)
			str="Variable Units:";
		else if(i==2)
			str="Variable Id:";
		else if(i==3)
			str="Variable Pos:";
		else if (i == 4)
			str = "Variable Station:";
		else if (i == 5)
			str = "Variable Country:";
		else if (i == 6)
			str = "Variable Alt:";
		else if (i == 7)
			str = "Variable Lat:";
		else if (i == 8)
			str = "Variable Long:";
		//for(size_t nrep=0;nrep<pg.Shape.NumRepetitions;nrep++) {
		size_t nrep=0;

			for(size_t j=0;j<GetNumVariables();j++) {
				str+=",";
				size_t add = 0;
				add = nrep*pg.Shape.NumVar;
				auto addindex = j+add;
				if(i==0)
					str+=GetVarName(j+1, nrep+1);
				else if(i==1)
					str+=GetVarUnit(j+1, nrep+1);
				else if(i==2)
					str+=GetVarId(j+1, nrep+1);
				else if(i==3)
					str+=GetVarPos(j+1, nrep+1);
				else if (i == 4)
					str+= pg.Des.addinfo[addindex].StationName;
				else if (i == 5)
					str += pg.Des.addinfo[addindex].CountryCode;
				else if (i == 6)
					str += FUtil::STD_DtoAscii(pg.Des.addinfo[addindex].Alt);
				else if (i == 7)
					str += FUtil::STD_DtoAscii(pg.Des.addinfo[addindex].Lat);
				else if (i == 8)
					str += FUtil::STD_DtoAscii(pg.Des.addinfo[addindex].Long);

			}
		//}
		str+=linebreak;
		size_t nc=str.size();
		file.write((char*)&str[0],nc);
		if(!file.good()) {
			return "";

		}
	}
	size_t prev, current;
	prev=0;
	size_t Num_To_Save;
	Num_To_Save=GetNumRecords();
	if(secondhalf) Num_To_Save=GetNumRecords()/2;
	ReOpen();
	bool failure=false;
	for(size_t nrep=0;nrep<pg.Shape.NumRepetitions;nrep++) {
		SetRepititionSequence(nrep+1);
		for(size_t i=0;i<Num_To_Save;i++) {
			if(!failure) str=GetDate(i+1);
			if(str==""||failure) {
					if(!m_MainPGStreamReadWrite.good()) {
						m_MainPGStreamReadWrite.clear();
						m_MainPGStreamReadWrite.close();
						//ReOpen();
						m_MainPGStreamReadWrite.open(pg.status.FileName, ios ::in|ios::binary);
						failure=true;
					}

					std::streamsize newpos;

					newpos=pg.Shape.RecordSize*i+pg.Shape.RecordSize;
	
					m_MainPGStreamReadWrite.seekg(newpos,ios::beg);
					m_MainPGStreamReadWrite.read((char*)&pg.Var.v[0],pg.Shape.RecordSize);
					if(m_MainPGStreamReadWrite.good()) {
						size_t i4;
						i4=*(size_t*)&pg.Var.v[0];
						str=PGUtil::StringDatum(i4);
						str+=',';
						str+=FUtil::STD_FtoAscii(pg.Var.v[1]);
						str+=linebreak;
						size_t nc=str.size();
						for(size_t jj=0;jj<nc;jj++)
							buf[jj]=str.at(jj);
						file.write((char*)&buf,nc);
					}





			}
			else {
				if(split) {
					current=GetLongTime(i+1);
					if(current<prev) {


	#ifndef COUPSTD
						CString txt;
						txt="Writing to txt file : ";
						txt+=MFC_Util::ItoAscii(splitno);
						//if(pGlobalCug!=nullptr) {
							//MySingle::instance().GetCugPointer()->UpdateStatusBar(txt);
							MySingle::instance().GetCugPointer()->UpdateStatusBar(txt);
						//}
	#endif

						file.close();
						fileout=filebase;
						splitno++;
						fileout+=FUtil::STD_ItoAscii(splitno);
						fileout+=".txt";
						file.open(fileout,ios::out);
						for(size_t il=0;il<9;il++) {
							if (il == 0)
								str = "Variable Names:";
							else if (il == 1)
								str = "Variable Units:";
							else if (il == 2)
								str = "Variable Id:";
							else if (il == 3)
								str = "Variable Pos:";
							else if (il == 4)
								str = "Variable Station:";
							else if (il == 5)
								str = "Variable Country:";
							else if (il == 6)
								str = "Variable Alt:";
							else if (il == 7)
								str = "Variable Lat:";
							else if (il == 8)
								str = "Variable Long:";
								//for(size_t nrep=0;nrep<pg.Shape.NumRepetitions;nrep++) {
								size_t nrep=0;
									for(size_t j=0;j<GetNumVariables();j++) {
										size_t add = 0;
										add = nrep*pg.Shape.NumVar;
										auto addindex = j + add;
										str+=",";
										if(il==0)
											str+=GetVarName(j+1, nrep+1);
										else if(il==1)
											str+=GetVarUnit(j+1, nrep+1);
										else if(il==2)
											str+=GetVarId(j+1, nrep+1);
										else if(il==3)
											str+=GetVarPos(j+1, nrep+1);
										else if (il == 3)
											str += GetVarPos(j + 1, nrep + 1);
										else if (il == 4)
											str += pg.Des.addinfo[addindex].StationName;
										else if (il == 5)
											str += pg.Des.addinfo[addindex].CountryCode;
										else if (il == 6)
											str += FUtil::STD_DtoAscii(pg.Des.addinfo[addindex].Alt);
										else if (il == 7)
											str += FUtil::STD_DtoAscii(pg.Des.addinfo[addindex].Lat);
										else if (il == 8)
											str += FUtil::STD_DtoAscii(pg.Des.addinfo[addindex].Long);
									}
								//}
								str+=linebreak;
								size_t nc=str.size();
								for(size_t jj=0;jj<nc;jj++)
									buf[jj]=str.at(jj);
								file.write((char*)&buf,nc);
							}
							str=GetDate(i+1);
					}
					prev=current;
				}
				for(size_t j=0;j<GetNumVariables();j++) {
					size_t Record_No;
					Record_No=i+1;
					if(secondhalf)
						Record_No=Num_To_Save+i+1;


					if(GetVarValue(j+1,Record_No)>-1.E37)
						str+=","+FUtil::STD_FtoAscii(GetVarValue(j+1,Record_No));
					else
						str+=",";
				}
				str+=linebreak;
				size_t nc=str.size();
				for(size_t jj=0;jj<nc;jj++)
					buf[jj]=str.at(jj);
				file.write((char*)&buf,nc);
			}
		}
	}
	if(file.good()) {
		file.close();


#ifndef COUPSTD	
		pApp->EndWaitCursor();
#endif
		return fileout;
	}
	else
		return "";
}

string CPGFile::GetFileName(bool OnlyTitle)
{
	if(OnlyTitle) {
		size_t pos=pg.status.FileName.rfind("\\");
		if(pos!=string::npos)
			return ".."+pg.status.FileName.substr(pos);
		else
			return pg.status.FileName;
	}
	else
		return pg.status.FileName;
}

bool CPGFile::SetRepititionIdNo(size_t Id, bool ResetIndex)
{
	size_t test;
	size_t sequence;
	for(size_t i=0; i<GetNumRepititions();i++) {
		string str=GetVarId(1,i+1);
		FUtil::trim(str);
		test=FUtil::AtoInt(str);
		if(test==Id) {
			sequence=i+1;
			return SetRepititionSequence(sequence, ResetIndex);
		}
	}
	return false;
}

bool CPGFile::IsEquiDistance()
{
	if(m_EquiDistance>0)
		return true;
	else
		return false;
}
size_t CPGFile::RemoveMissingRecords() 
{
	size_t icount=0, irec=0;
	size_t jj;
	bool valid;

	std::vector <float> vbuf;
	vbuf.resize((pg.Shape.NumVar+1)*pg.Shape.NumRecords);
	for(size_t i=0;i<pg.Shape.NumRecords;i++) {
		
		valid=false;
		jj=0;
		while(jj<pg.Shape.NumVar&&!valid) {
			if(abs(pg.Var.v[i*pg.Shape.RecordIndexSize+jj+1])<1.E37)
				valid=true;
			jj++;
		}
		if(valid) {
			vbuf[irec*pg.Shape.RecordIndexSize]=pg.Var.v[i*pg.Shape.RecordIndexSize];
			for(size_t ii=0;ii<pg.Shape.NumVar;ii++) {
				vbuf[irec*pg.Shape.RecordIndexSize+ii+1]=pg.Var.v[i*pg.Shape.RecordIndexSize+ii+1];
			}
			irec++;
		}
	}

	size_t iremoved=pg.Shape.NumRecords-irec;
	pg.Shape.NumRecords=irec;

	for(size_t i=0;i<pg.Shape.NumRecords;i++) {
		pg.Var.v[i*pg.Shape.RecordIndexSize]=vbuf[i*pg.Shape.RecordIndexSize];
			for(size_t ii=0;ii<pg.Shape.NumVar;ii++) {
				pg.Var.v[i*pg.Shape.RecordIndexSize+ii+1]=vbuf[i*pg.Shape.RecordIndexSize+ii+1];
			}
	}
	return iremoved;
}
bool CPGFile::RemoveRepetition(size_t repno)
{
	if(!pg.status.CompleteFileRead&&repno<=pg.Shape.NumRepetitions&&pg.Shape.NumRepetitions>1) {
		if(!MakeNewPGFileName()) return false;
		CloseFile();
		FUtil::CopyFile_Std(pg.status.FileName, pg.status.NewFileName);
		ReOpen(true);
		m_ExtraPGStreamWrite.open(pg.status.NewFileName, ios ::out|ios::binary);
		if(!m_ExtraPGStreamWrite.is_open()) {
			return false;
		}		
		pg.Shape.NumRepetitions--;
		size_t nrecords;
		m_MainPGStreamReadWrite.read((char*)&nrecords,4);
		nrecords-=pg.Shape.NumRecords;
		m_ExtraPGStreamWrite.write((char*)&nrecords,4);
		size_t ivar;
		m_MainPGStreamReadWrite.read((char*)&ivar,4);
		m_ExtraPGStreamWrite.write((char*)&ivar,4);
		size_t rep;
		m_MainPGStreamReadWrite.read((char*)&rep,4);
		rep--;
		m_ExtraPGStreamWrite.write((char*)&rep,4);


		for (size_t k=0; k<pg.Shape.NumRepetitions+1; k++) {
			for (size_t i=0; i<pg.Shape.NumRecords; i++) {
				m_MainPGStreamReadWrite.read((char*)&pg.Var.v[0], pg.Shape.RecordSize);
				if(k!=repno-1)
					m_ExtraPGStreamWrite.write((char*)&pg.Var.v[0], pg.Shape.RecordSize);
			}
		}
		auto koll=m_ExtraPGStreamWrite.tellg();
		pg.Des.base.Name.erase(pg.Des.base.Name.begin()+(repno-1)*pg.Shape.NumVar,pg.Des.base.Name.begin()+(repno)*pg.Shape.NumVar);
		pg.Des.base.Unit.erase(pg.Des.base.Unit.begin()+(repno-1)*pg.Shape.NumVar,pg.Des.base.Unit.begin()+(repno)*pg.Shape.NumVar);
		pg.Des.base.Id.erase(pg.Des.base.Id.begin()+(repno-1)*pg.Shape.NumVar	,pg.Des.base.Id.begin()+(repno)*pg.Shape.NumVar);
		pg.Des.base.Pos.erase(pg.Des.base.Pos.begin()+(repno-1)*pg.Shape.NumVar	,pg.Des.base.Pos.begin()+(repno)*pg.Shape.NumVar);
		pg.Des.base.var_min.erase(pg.Des.base.var_min.begin()+(repno-1)*pg.Shape.NumVar,pg.Des.base.var_min.begin()+(repno)*pg.Shape.NumVar);
		pg.Des.base.var_max.erase(pg.Des.base.var_max.begin()+(repno-1)*pg.Shape.NumVar,pg.Des.base.var_max.begin()+(repno)*pg.Shape.NumVar);
		if(pg.Des.addinfo.size()>=(repno+1)*pg.Shape.NumVar) {
				pg.Des.addinfo.erase(pg.Des.addinfo.begin()+(repno-1)*pg.Shape.NumVar,pg.Des.addinfo.begin()+repno*pg.Shape.NumVar);
		}


		std::streampos newpos=pg.Shape.RecordSize*(nrecords+1);
		m_ExtraPGStreamWrite.seekg(newpos,ios::beg);
		koll=m_ExtraPGStreamWrite.tellg();
		size_t descsize=pg.Shape.NumVar*60;
		char buf2[100];
		size_t ind;
		for(size_t nrep=0; nrep<rep; nrep++) {
			for(size_t i=0;i<pg.Shape.NumVar;i++) {
				//string buf;
				ind=nrep*pg.Shape.NumVar+i;
				for(size_t ii=0;ii<20;ii++) {
					if(pg.Des.base.Name[ind].size()<ii)
						buf2[ii]=' ';	
					else
						buf2[ii]=pg.Des.base.Name[ind].at(ii);	
				}
				//buf=pg.Des.base.Unit[ind];
				for(size_t ii=0;ii<8;ii++) {
					if(pg.Des.base.Unit[ind].size()<ii)
						buf2[ii+20]=' ';	
					else
						buf2[ii+20]=pg.Des.base.Unit[ind].at(ii);	
				}
				for(size_t ii=0;ii<20;ii++) {
					if(pg.Des.base.Id[ind].size()<ii)
						buf2[ii+28]=' ';	
					else
						buf2[ii+28]=pg.Des.base.Id[ind].at(ii);	
				}

				//buf=pg.Des.base.Pos[ind];
				for(size_t ii=0;ii<4;ii++) {
					if(pg.Des.base.Pos[ind].size()<ii)
						buf2[ii+48]=' ';	
					else
						buf2[ii+48]=pg.Des.base.Pos[ind].at(ii);	
				}
				m_ExtraPGStreamWrite.write((char*)&buf2,52);
				float fmin,fmax;
				if(pg.Des.base.var_min.size()>ind)
					fmin=pg.Des.base.var_min[ind];
				else
					fmin=100.;
				if(pg.Des.base.var_max.size()>ind)
					fmax=pg.Des.base.var_max[ind];
				else
					fmax=-1.;
				m_ExtraPGStreamWrite.write((char*)&fmin,4);
				m_ExtraPGStreamWrite.write((char*)&fmax,4);
			}

		}
		if(pg.status.FileVersion>0) {
			newpos+=rep*pg.Shape.NumVar*60;
			m_ExtraPGStreamWrite.seekg(newpos,ios::beg);
			auto xx=m_ExtraPGStreamWrite.gcount();
			m_ExtraPGStreamWrite.write((char*)&pg.status.FileVersion,4);
			for(size_t nrep=0; nrep<rep; nrep++) {
				for(size_t i=0;i<pg.Shape.NumVar;i++) {
					ind=nrep*pg.Shape.NumVar+i;
					xx=m_ExtraPGStreamWrite.gcount();
					if(m_ExtraPGStreamWrite.good()) {
						m_ExtraPGStreamWrite.write((char*)&pg.Des.addinfo[ind].Lat,8);
						m_ExtraPGStreamWrite.write((char*)&pg.Des.addinfo[ind].Long,8);
						m_ExtraPGStreamWrite.write((char*)&pg.Des.addinfo[ind].Alt,8);
						unsigned int n=pg.Des.addinfo[ind].CountryCode.size();
						m_ExtraPGStreamWrite.write((char*)&n,4);
						m_ExtraPGStreamWrite.write(pg.Des.addinfo[ind].CountryCode.data(),n);
						n=pg.Des.addinfo[ind].StationName.size();
						m_ExtraPGStreamWrite.write((char*)&n,4);
						m_ExtraPGStreamWrite.write(pg.Des.addinfo[ind].StationName.data(),n);
						m_ExtraPGStreamWrite.write((char*)&pg.Des.addinfo[ind].num_missing,4);
						m_ExtraPGStreamWrite.write((char*)&pg.Des.addinfo[ind].num_substituted,4);
					}
				}
			}
		}
		m_MainPGStreamReadWrite.close();
		m_ExtraPGStreamWrite.close();
		auto res=remove(pg.status.FileName.data());
		if (res != 0) 
			res=rename(pg.status.NewFileName.data(), pg.status.FileName.data());
		ReOpen();
		if (res != 0)
			return true;
		else
			return false;
		
	}
	else if(repno<=pg.Shape.NumRepetitions&&pg.Shape.NumRepetitions>1) {
		SetRepititionSequence(repno);
		size_t ind=(pg.status.a.LocalStartRecord)*pg.Shape.NumVar;
		pg.Var.v.erase(pg.Var.v.begin()+ind,pg.Var.v.begin()+ind+pg.Shape.NumRecords*pg.Shape.NumVar);
		pg.Des.base.Name.erase(pg.Des.base.Name.begin()+repno*pg.Shape.NumVar,pg.Des.base.Name.begin()+(repno+1)*pg.Shape.NumVar);
		pg.Des.base.Unit.erase(pg.Des.base.Unit.begin()+repno*pg.Shape.NumVar,pg.Des.base.Unit.begin()+(repno+1)*pg.Shape.NumVar);
		pg.Des.base.Id.erase(pg.Des.base.Id.begin()+repno*pg.Shape.NumVar	,pg.Des.base.Id.begin()+(repno+1)*pg.Shape.NumVar);
		pg.Des.base.Pos.erase(pg.Des.base.Pos.begin()+repno*pg.Shape.NumVar	,pg.Des.base.Pos.begin()+(repno+1)*pg.Shape.NumVar);
		pg.Des.base.var_min.erase(pg.Des.base.var_min.begin()+repno*pg.Shape.NumVar,pg.Des.base.var_min.begin()+(repno+1)*pg.Shape.NumVar);
		pg.Des.base.var_max.erase(pg.Des.base.var_max.begin()+repno*pg.Shape.NumVar,pg.Des.base.var_max.begin()+(repno+1)*pg.Shape.NumVar);
		pg.Des.addinfo.erase(pg.Des.addinfo.begin()+repno*pg.Shape.NumVar,pg.Des.addinfo.begin()+(repno+1)*pg.Shape.NumVar);
		pg.Shape.NumRepetitions--;
		return true;
	}

	return false;
}
bool CPGFile::AddRepetition(size_t repno)
{
	if(!pg.status.CompleteFileRead) return false;
		size_t GetRepitionsSequence();
		SetRepititionSequence(repno);
		size_t ind=(pg.status.a.LocalStartRecord)*pg.Shape.RecordIndexSize;
		AddInfo A;
		A.Key=0;
		A.Long=-1.;
		A.Lat=-1.;
		A.Alt=-1.;
		A.CountryCode="";
		A.StationName="";
		A.num_missing=0;
		A.num_substituted=0;

		pg.Var.v.insert(pg.Var.v.begin()+ind, pg.Shape.NumRecords*pg.Shape.RecordIndexSize, MISSING);
		pg.Des.base.Name.insert(pg.Des.base.Name.begin()+repno*pg.Shape.NumVar, pg.Shape.NumVar,"");
		pg.Des.base.Unit.insert(pg.Des.base.Unit.begin()+repno*pg.Shape.NumVar, pg.Shape.NumVar,"");
		pg.Des.base.Id.insert(pg.Des.base.Id.begin()+repno*pg.Shape.NumVar	, pg.Shape.NumVar,"");
		pg.Des.base.Pos.insert(pg.Des.base.Pos.begin()+repno*pg.Shape.NumVar	, pg.Shape.NumVar,"");
		pg.Des.base.var_min.insert(pg.Des.base.var_min.begin()+repno*pg.Shape.NumVar, pg.Shape.NumVar,MISSING);
		pg.Des.base.var_max.insert(pg.Des.base.var_max.begin()+repno*pg.Shape.NumVar, pg.Shape.NumVar, MISSING);
		pg.Des.addinfo.insert(pg.Des.addinfo.begin()+repno*pg.Shape.NumVar, pg.Shape.NumVar, A);
		pg.Shape.NumRepetitions++;
	return true;
}

string CPGFile::GetVarDes(size_t index, size_t repit)
{   size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(index>0)
		return pg.Des.base.Name[index-1]+"\t"+pg.Des.base.Unit[index-1]+"\t"+pg.Des.base.Id[index-1]+"\t"+pg.Des.base.Pos[index-1];
	else
		return "";
}
string CPGFile::GetVarName(size_t index, size_t repit)
{   size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(index>0&&pg.Des.base.Name.size()+1>index)
		return pg.Des.base.Name[index-1];
	else
		return "";
}
string CPGFile::GetVarUnit(size_t index, size_t repit)
{   size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(index>0&&pg.Des.base.Unit.size()+1>index)
		return pg.Des.base.Unit[index-1];
	else
		return "";
}
string CPGFile::GetVarId(size_t index, size_t repit)
{	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(index>0&&pg.Des.base.Id.size()+1>index)
		return pg.Des.base.Id[index-1];
	else
		return "";
}
string CPGFile::GetVarPos(size_t index, size_t repit)
{	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(index>0&&index<pg.Des.base.Pos.size()+1)
		return pg.Des.base.Pos[index-1];
	else
		return "";
}

void CPGFile::SetVarName(size_t index, string str, size_t repit)
{	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	size_t ipos=str.size();

	for (size_t i=ipos;i<20;i++) 
		str+=" ";
	
	if(pg.Des.base.Name.size()>=index)
		pg.Des.base.Name[index-1]=str;
	else
		pg.Des.base.Name.push_back(str);
}
void CPGFile::SetVarUnit(size_t index, string str, size_t repit)
{   size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	size_t ipos=str.size();
	for (size_t i=ipos;i<8;i++) 
		str+=" ";
	if(pg.Des.base.Unit.size()>=index)
		pg.Des.base.Unit[index-1]=str;
	else
		pg.Des.base.Unit.push_back(str);

}
void CPGFile::SetVarId(size_t index, string str, size_t repit)
{	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	size_t ipos=str.size();
	for (size_t i=ipos;i<20;i++) 
		str+=" ";
	if(pg.Des.base.Id.size()>=index)
		pg.Des.base.Id[index-1]=str;
	else
		pg.Des.base.Id.push_back(str);
}
void CPGFile::SetVarPos(size_t index, string str, size_t repit)
{	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	size_t ipos=str.size();
	for (size_t i=ipos;i<4;i++) 
		str+=" ";
	if(pg.Des.base.Pos.size()>=index)
		pg.Des.base.Pos[index-1]=str;
	else
		pg.Des.base.Pos.push_back(str);
}

float CPGFile::GetVarMax(size_t index, size_t repit)
{	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(pg.Des.base.var_max.size()>=index&&index>0)
		return pg.Des.base.var_max[index-1];
	else
		return MISSING;
}
float CPGFile::GetVarMin(size_t index, size_t repit)
{	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(pg.Des.base.var_min.size()>=index&&index>0)
		return pg.Des.base.var_min[index-1];
	else
		return MISSING;
}
void CPGFile::SetVarMax(size_t index, float vnew, size_t repit)
{	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	pg.Des.base.var_max[index-1]=vnew;
}
void CPGFile::SetVarMin(size_t index, float vnew, size_t repit)
{	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	pg.Des.base.var_min[index-1]=vnew;
}
bool CPGFile::ReCalculateMinMax(size_t repit)
{
	float vmin,vmax, vnew;
	for(size_t i=1;i<=pg.Shape.NumVar;i++)  {
		vmin=abs(MISSING);
		vmax=MISSING;
		for(size_t ii=1;ii<=pg.Shape.NumRecords;ii++) {
				vnew=GetVarValue(i, ii);
				if(vnew>-1.E37) {
					if(vnew>vmax)
						vmax=vnew;
					if(vnew<vmin)
						vmin=vnew;
				}
		}
		SetVarMin(i,vmin, repit);
		SetVarMax(i,vmax, repit);

	}
	return m_MainPGStreamReadWrite.good();
}
float CPGFile::GetVarValue(size_t varno, size_t var_rec)
{
	//return *m_var[varno-1][var_rec-1];
	if(var_rec>pg.Shape.NumRecords)
		return MISSING;
	if(pg.status.CompleteFileRead&&!pg.Shape.RepetitionMode)
		return pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize+varno];
	else if(pg.status.CompleteFileRead&&pg.Shape.RepetitionMode) {
		size_t koll=pg.Var.v.size();
		size_t ind=(var_rec-1+pg.status.a.LocalStartRecord)*pg.Shape.RecordIndexSize+varno;
		if(ind<koll)
			return pg.Var.v[ind];
		else
			return MISSING;
	}
	else {
		if(pg.Shape.RepetitionMode) {
			size_t koll=pg.Var.v.size();
			size_t ind=(var_rec-1)*pg.Shape.RecordIndexSize+varno;
			if(ind<koll)
				return pg.Var.v[ind];
			else
				return MISSING;			
		}
		else
			return GetVarValueRead(varno, var_rec);
	}
}
float CPGFile::GetVarValueRead(size_t varno, size_t var_rec)
{
	//return *m_var[varno-1][var_rec-1];
	size_t CurrentBuf, CurrentRec;
	CurrentBuf=(var_rec-1)/pg.status.b.buf_Size+1;
	CurrentRec=(var_rec-1)%pg.status.b.buf_Size*pg.Shape.RecordIndexSize+1;
	
	if(CurrentBuf!=pg.status.b.PrevBuf||m_ResetBuffer) {
		if(!ReadNewBuffer(CurrentRec, CurrentBuf))	return MISSING;
	/*	size_t CurrentRecRead;
		if(CurrentBuf<pg.status.b.NumberOfBuf) 
			CurrentRecRead=pg.status.b.buf_Size;
		else
			CurrentRecRead=(pg.Shape.NumRecords-1)%pg.status.b.buf_Size+1;


		m_driveFile.seekg(0,ios::end);
		std::streamsize endpos=m_driveFile.tellg();
		std::streamsize newpos=pg.Shape.RecordSize*(CurrentBuf-1)*pg.status.b.buf_Size+pg.Shape.RecordSize;
		m_driveFile.seekg(newpos,ios::beg);
		if(pg.Shape.RecordSize*CurrentRecRead+newpos<=endpos)
			m_driveFile.read((char*)&pg.Var.v[0],pg.Shape.RecordSize*CurrentRecRead);
		pg.status.b.PrevBuf=CurrentBuf;
		m_ResetBuffer=false;*/
	}

	if(m_MainPGStreamReadWrite.good())
		return pg.Var.v[varno+CurrentRec-1];
	return MISSING;
}
bool CPGFile::ReAllocate(size_t irec, size_t ivar, size_t repit)
{
	Reset();
	CreateNewFile(ivar, irec, repit);
	return true;
}
bool CPGFile::SetNumRecords(size_t irec)
{
	pg.Shape.NumRecords=irec;

	return true;
}
size_t CPGFile::GetNumRecords()
{
	return 	pg.Shape.NumRecords;
}
size_t CPGFile::GetNumVariables()
{
	return pg.Shape.NumVar;
}
size_t CPGFile::GetNumRecordsBufSize() 
{
	return pg.status.b.buf_Size;
}
size_t CPGFile::GetNumBuffers() 
{
	return pg.status.b.NumberOfBuf;
}
size_t CPGFile::GetPrevBuffer() 
{
	return pg.status.b.PrevBuf;
}
void CPGFile::Reset()
{
	if(m_varx_years!=nullptr) delete [] m_varx_years;
	if(m_varx_months.size()>0) m_varx_months.clear();
	if(m_varx_days!=nullptr) delete [] m_varx_days;
	if(m_varx_hour!=nullptr) delete [] m_varx_hour;
	if(m_varx_minute!=nullptr) delete [] m_varx_minute;
	if(m_YearIndexStart!=nullptr) delete [] m_YearIndexStart;
	if(m_YearRecords!=nullptr) delete [] m_YearRecords;


	v_WithinYear.clear();
	v_WithinDay.clear();

	if(m_str!=nullptr) delete [] m_str;
	
	v_accepted.clear();
	v_all.clear();
	pg.Var.v.clear();
	

	//pg.Des.base.var_max.RemoveAll();
	//pg.Des.base.var_min.RemoveAll();
	pg.Des.base.Name.clear();
	pg.Des.base.Unit.clear();
	pg.Des.base.Id.clear();
	pg.Des.base.Pos.clear();
	pg.status.a.ToBeSaved.clear();
	pg.status.a.ToBeViewed.clear();
	pg.status.a.ToBeCalc.clear();
	pg.Des.addinfo.clear();

	m_str=nullptr;
	m_IsOpen=false;
	m_startRec=1;
	m_varxMinutes=m_varxHours=m_varxDays=m_varxMonths=m_varxYears=false;
	m_varx_minute=nullptr;
	m_varx_hour=nullptr;
	m_varx_days=nullptr;
	m_varx_years=nullptr;
	m_YearIndexStart=nullptr;
	m_YearRecords=nullptr;


	pg.Shape.RecordSize=0;

	if(m_MainPGStreamReadWrite.is_open())
		m_MainPGStreamReadWrite.close();
	GapProblemFinded=true;


	m_IncreaseTimeTest=false;
	pg.Shape.NumVar=8;
	pg.status.b.buf_TotSize=32000;
	pg.status.b.buf_Size=pg.status.b.buf_TotSize/pg.Shape.NumVar;
	pg.status.b.PrevBuf=string::npos;
	pg.status.b.NumberOfBuf=1;


}
string CPGFile::GetDate(size_t var_rec)
{
	size_t i4;
	size_t index=(var_rec-1)*pg.Shape.RecordIndexSize;
	//return PGUtil::StringDatum(*m_time[var_rec-1]);
	if((pg.status.CompleteFileRead||pg.Shape.RepetitionMode)&&index<pg.Var.v.size()) {
		i4=*(size_t*)&pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize];
		return PGUtil::StringDatum(i4);
	}
	else if(!pg.status.CompleteFileRead) {

		size_t CurrentBuf, CurrentRec;
		if(pg.Shape.RecordIndexSize==pg.Var.v.size()) pg.status.b.buf_Size=1;

			CurrentBuf=(var_rec-1)/pg.status.b.buf_Size+1;
			CurrentRec=(var_rec-1)%pg.status.b.buf_Size*pg.Shape.RecordIndexSize+1;
	
			if(CurrentBuf!=pg.status.b.PrevBuf||m_ResetBuffer) {
				if(!ReadNewBuffer(CurrentRec, CurrentBuf))
					return "";
			/*	size_t CurrentRecRead;
				if(CurrentBuf<pg.status.b.NumberOfBuf) 
					CurrentRecRead=pg.status.b.buf_Size;
				else
					CurrentRecRead=(pg.Shape.NumRecords-1)%pg.status.b.buf_Size+1;

				std::streamsize newpos=pg.Shape.RecordSize*var_rec;
				m_driveFile.seekg(newpos,ios::beg);
				m_driveFile.read((char*)&pg.Var.v[0],pg.Shape.RecordSize*CurrentRecRead);
				pg.status.b.PrevBuf=CurrentBuf;
				m_ResetBuffer=false;*/
			}	
			if(CurrentRec<=pg.Var.v.size())
				return PGUtil::StringDatum(*(size_t*)&pg.Var.v[CurrentRec-1]);
			else
				return "";
		
	}
	else
		return "";
}
unsigned int CPGFile::GetLongTimeEnd()
{
	return pg.status.MinEnd;
	//return GetLongTime(pg.Shape.NumRecords);
}
string CPGFile::GetVarDateHourMean(size_t var_rec, size_t hour)
{
	//return *m_var[varno-1][var_rec-1];
	if(!m_varxHours) {
		GetNumRecordsHour(hour);
	}
	if(var_rec<=m_numrecords_Hours)
		return PGUtil::StringDatum(*(size_t*)&m_varx_hour[(var_rec-1)*pg.Shape.RecordIndexSize]);
	else
		return ""; //
}
size_t CPGFile::GetNumRecordsHour(size_t hour)
{
	if(!m_varxHours) {
		m_numrecords_Hours=(GetLongTimeEnd()-GetLongTimeStart())/(hour*60)+1;
		m_varx_hour= new float[pg.Shape.RecordSize*m_numrecords_Hours];
		unsigned int currentmin;
		size_t ist,next;
		size_t *inum=new size_t[pg.Shape.NumVar];
		float *sumv=new float[pg.Shape.NumVar];
		size_t count;
		currentmin=GetLongTimeStart();
		string datumstart=PGUtil::StringDatum(currentmin);
		datumstart=datumstart.substr(0,14);
		datumstart+="00";

		currentmin=PGUtil::MinutConv(datumstart);
		ist=1;
		next=currentmin+60*hour;
		count=0;
		while (ist<=pg.Shape.NumRecords) {	
			for(size_t i=0;i<pg.Shape.NumVar;i++){
				*(sumv+i)=float(0);
				*(inum+i)=size_t(0);

			}
			while(currentmin<next&&ist<=pg.Shape.NumRecords) {
				for(size_t i=0;i<pg.Shape.NumVar;i++) {
					if(pg.Var.v[(ist-1)*pg.Shape.RecordIndexSize+i+1]>-1.e37) {
						*(inum+i)+=1;
						*(sumv+i)+=pg.Var.v[(ist-1)*pg.Shape.RecordIndexSize+i+1];
					}					
				}
				ist++;
				if(ist<=pg.Shape.NumRecords)
					currentmin=GetLongTime(ist);
			}
			for(size_t i=0;i<pg.Shape.NumVar;i++){
				if(*(inum+i)>0) {
					*(m_varx_hour+(count)*pg.Shape.RecordIndexSize+i+1)=*(sumv+i)/size_t(*(inum+i));
					*(sumv+i)=float(0);
					*(inum+i)=size_t(0);
				}
				else
					*(m_varx_hour+count*pg.Shape.RecordIndexSize+i+1)=float(MISSING);
			}	

			size_t t=next-(hour*60)/2;
			*(m_varx_hour+ count*pg.Shape.RecordIndexSize)=*(float*)&t;
			count++;
			next+=hour*60;
		}
		delete []inum;
		delete []sumv;
		m_varxHours=true;
		return m_numrecords_Hours;
	}
	else
		return -1;
}
bool CPGFile::FindKeyValues()
{
	ifstream file;
	string filename;


  	string lDataBaseDirectory;
	lDataBaseDirectory=FUtil::GetProfileStringStd("DataBaseDirectory",lDataBaseDirectory);
	filename=lDataBaseDirectory;
	filename+="PG_Keys\\ISH-HISTORY.TXT";
 
	file.open(filename,ios::in);
	if(!file.good()) {
		string str, str2;
		str=" Problems when reading file with coordinates";
		str2="Please check following file from Noaa :"+filename;
#ifndef COUPSTD
		MFC_Util::MessageBox(str.c_str(),str2.c_str(),IDOK);
#endif
		return false;

	}

	string line;
	size_t nrep=GetNumRepititions();
	SetInfoMode();

	map <size_t, size_t> IDStatMap;
	map <size_t, size_t>::iterator it;
	for(size_t i=0; i<nrep;i++) {
		string IDS=GetVarId(1,i+1);
		IDStatMap[FUtil::AtoInt(IDS.substr(0,6))]=i;
	}
	size_t nvar=pg.Shape.NumVar;

	if(!file.is_open()) return false;

	file.seekg(0,ios::end);
	std::streamsize totsize=file.tellg();
	file.seekg(0,ios::beg);
	while(file.good()) {
		getline(file,line);
		if(line.find("USAF")==0&&line.find("WBAN")==7) {
			getline(file,line);

			while(file.good()) {
				getline(file,line);
				size_t ID, Lat, Long, Elev;
				double dLat, dLong, dElev;
				ID=FUtil::AtoInt(line.substr(0,6));

				it=IDStatMap.find(ID);
				if(it!=IDStatMap.end()&&line.size()>90) {
					size_t iv=it->second;	
					string StatName, CountryIDm, Date_St, Date_End;
					StatName=line.substr(13,20);
					FUtil::trim(StatName);
					CountryIDm=line.substr(43,2);
					Lat=FUtil::AtoInt(line.substr(58,6));
					dLat=double(Lat)/1000;
					Long=FUtil::AtoInt(line.substr(65,7));
					dLong=double(Long)/1000;
					Elev=FUtil::AtoInt(line.substr(73,6));
					dElev=double(Elev)/10;
					Date_St=line.substr(83,8);
					Date_End=line.substr(92,8);
					//pPG->CheckMissingNumbers();
					for(size_t j=0;j<nvar;j++) {
						SetVarLat(j+1, dLat, iv+1);
						SetVarLong(j+1, dLong, iv+1);
						SetVarAlt(j+1, dElev, iv+1);
						SetVarStation(j+1, StatName, iv+1);
						SetVarCountry(j+1, CountryIDm, iv+1);
						//pPG->SetVarMissing(j+1, 0, iv+1);
						//pPG->SetVarSubstituted(j+1, 0, iv+1);
					}
				}
			}
		}
	}
	file.close();
	string str, str2;
	str=" File have been updated with precise information of lat, long and Altitude";
	str2="Reading file"+filename;
#ifndef COUPSTD
	MFC_Util::MessageBox(str.c_str(),str2.c_str(),IDOK);
#endif

	return true;

}

void CPGFile::SetVarKey(size_t index, size_t value, size_t repit)
{
	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(pg.Des.addinfo.size()>=index)
		pg.Des.addinfo[index-1].Key=value;
}
void CPGFile::SetVarLat(size_t index, double value, size_t repit)
{
	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(pg.Des.addinfo.size()>=index)
		pg.Des.addinfo[index-1].Lat=value;
}
void CPGFile::SetVarLong(size_t index, double value, size_t repit)
{
	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(pg.Des.addinfo.size()>=index)
		pg.Des.addinfo[index-1].Long=value;
}
void CPGFile::SetVarAlt(size_t index, double value, size_t repit)
{
	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(pg.Des.addinfo.size()>=index)
		pg.Des.addinfo[index-1].Alt=value;
}
void CPGFile::SetVarCountry(size_t index, string value, size_t repit)
{
	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(pg.Des.addinfo.size()>=index)
		pg.Des.addinfo[index-1].CountryCode=value;
}
void CPGFile::SetVarStation(size_t index, string value, size_t repit)
{
	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(pg.Des.addinfo.size()>=index)
		pg.Des.addinfo[index-1].StationName=value;
}
void CPGFile::SetVarMissing(size_t index, size_t value, size_t repit)
{
	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(pg.Des.addinfo.size()>=index)
		pg.Des.addinfo[index-1].num_missing=value;
}
void CPGFile::SetVarSubstituted(size_t index, size_t value, size_t repit)
{
	size_t add=0;
	add=(repit-1)*pg.Shape.NumVar;
	index+=add;
	if(pg.Des.addinfo.size()>=index)
		pg.Des.addinfo[index-1].num_substituted=value;
}
unsigned int CPGFile::GetLongTime(size_t var_rec, bool WithinBuffer)
{
	//return *m_time[var_rec-1];
	if(pg.status.CompleteFileRead||WithinBuffer) 
		return *(unsigned int*)&pg.Var.v[(var_rec-1)*pg.Shape.RecordIndexSize];
	else if(pg.Shape.RepetitionMode) {
			if(pg.Var.v[0]==0) {
				size_t CurrentBuf, CurrentRec;
				CurrentBuf=(var_rec-1)/pg.status.b.buf_Size+1;
				CurrentRec=(var_rec-1)%pg.status.b.buf_Size*pg.Shape.RecordIndexSize+1;
	
				if(CurrentBuf!=pg.status.b.PrevBuf) {
					ReadNewBuffer(CurrentRec, CurrentBuf);	
				}


			}

			size_t koll=pg.Var.v.size();
			size_t ind=(var_rec-1)*pg.Shape.RecordIndexSize;
			if(ind<koll)
				return *(unsigned int*)&pg.Var.v[ind];
			else
				return *(unsigned int*)&pg.Var.v[pg.Var.v.size()-1];
	}
	else {
		if(!m_MainPGStreamReadWrite.good()) return string::npos;

		size_t CurrentBuf, CurrentRec;
		CurrentBuf=(var_rec-1)/pg.status.b.buf_Size+1;
		CurrentRec=(var_rec-1)%pg.status.b.buf_Size*pg.Shape.RecordIndexSize+1;
	
		if (CurrentBuf != pg.status.b.PrevBuf&&pg.status.b.PrevBuf != string::npos) {
			ReadNewBuffer(CurrentRec, CurrentBuf);
			
		}
		return *(unsigned int*)&pg.Var.v[CurrentRec-1];
	}
}

bool CPGFile::IsAddInfoSet()
{
	if(pg.status.FileVersion>0)
		return true;
	else {
		if(ReOpen(true)) {
			m_MainPGStreamReadWrite.seekg(0,ios::beg);
			m_MainPGStreamReadWrite.seekg(0,ios::end);
			std::streamsize enpos=m_MainPGStreamReadWrite.gcount();
			std::streamsize firstend=(pg.Shape.TotalNumberOfRecords+1)*pg.Shape.RecordSize;
			firstend+=pg.Shape.NumVar*60;
			if(enpos<firstend) 
				return false;
			else {
				m_MainPGStreamReadWrite.seekg(firstend,ios::beg);
				m_MainPGStreamReadWrite.read((char*)&pg.status.FileVersion,4);
				if(pg.status.FileVersion>0) {
					if(!pg.Des.addinfo.size()==pg.Shape.NumVar)
						pg.Des.addinfo.resize(pg.Shape.NumVar);
					return true;
				}
				else
					return false;
			}

		}
	}
	return false;
}
bool CPGFile::Is_Id_FieldSensitive(){
	return m_IdField_Sensitive;
}
bool CPGFile::Is_LatLong_FieldSensitive() {
	return m_LatLong_FieldSensitive;
}
void CPGFile::CheckDescriptors() {
	m_LatLong_FieldSensitive = false;
	auto IdField = pg.Des.base.Id[0];
	double lat = 0.;
	double longi = 0.;
	if (pg.Des.addinfo.size() > 0) {
		lat = pg.Des.addinfo[0].Lat;
		longi = pg.Des.addinfo[0].Long;
	}
	m_IdField_Sensitive = false;
	for (size_t nrep = 0; nrep < pg.Shape.NumRepetitions; nrep++)
		for (size_t ivar = 0; ivar < pg.Shape.NumVar; ivar++) {
			auto index = ivar + nrep * pg.Shape.NumVar;
			if (pg.Des.base.Id[index] != IdField) m_IdField_Sensitive = true;
			if (pg.Des.addinfo.size() >= index) {
				if (lat != pg.Des.addinfo[index].Lat)  m_LatLong_FieldSensitive = true;
				if (longi != pg.Des.addinfo[index].Long)  m_LatLong_FieldSensitive = true;
			}
		}


}
bool CPGFile::CheckMissingNumbers()
{	
	float fval;
	if(pg.Shape.NumRepetitions>1)
		pg.Des.var_missing.resize(pg.Shape.NumVar*pg.Shape.NumRepetitions);
	else
		pg.Des.var_missing.resize(pg.Shape.NumVar);
	for(size_t nrep=0;nrep<pg.Shape.NumRepetitions;nrep++)
		for(size_t i=0;i<pg.Shape.NumVar;i++)
			pg.Des.var_missing[i+nrep*pg.Shape.NumVar]=0;
	if(pg.status.CompleteFileRead) {
		for(size_t nrep=0; nrep<pg.Shape.NumRepetitions;nrep++) {
			SetRepititionSequence(nrep+1);
			for(size_t ii=0;ii<pg.Shape.NumRecords;ii++) {
				for (size_t i=0;i<pg.Shape.NumVar;i++) {
					fval=GetVarValue(i+1,ii+1);
					if(fval<-1.E37) pg.Des.var_missing[i+nrep*pg.Shape.NumVar]++;
				}
			}
		}
	}
	else {
			for(size_t nrep=0; nrep<pg.Shape.NumRepetitions;nrep++) {
				SetRepititionSequence(nrep+1);
				for(size_t ii=0;ii<pg.Shape.NumRecords;ii++) {
					for (size_t i=0;i<pg.Shape.NumVar;i++) {
						fval=GetVarValue(i+1,ii+1);
						if(fval<-1.E37) pg.Des.var_missing[i+nrep*pg.Shape.NumVar]++;
					}
				}
			}


	}

	m_MissingValueExist=false;
	for(size_t nrep=0; nrep<pg.Shape.NumRepetitions;nrep++) {
		for (size_t i=0;i<pg.Shape.NumVar;i++) {
			if(pg.Des.var_missing[i+nrep*pg.Shape.NumVar]>0)
				m_MissingValueExist=true;
			if(pg.status.FileVersion>0)
				SetVarMissing(i+1,pg.Des.var_missing[i+nrep*pg.Shape.NumVar], nrep+1); 
		}		  		
	}

	m_MissingTested=true;
	SetRepititionSequence(1);
	return m_MissingValueExist;

}
void CPGFile::CheckEquiDistance(bool Month)
{	size_t minst;
size_t PreviousMin, CurrentMin;
	if(m_EquiDistance>0|| pg.Shape.NumRecords==0) return;
	if(Month) {
		m_EquiDistance=43830;
		pg.status.NormalTimeInterval=43830;
		m_IncreaseTimeTest=true;
		return;
	}
	PreviousMin = GetLongTime(1);
	CurrentMin = GetLongTime(2);
	minst=CurrentMin-PreviousMin;
	for (size_t i=1; i<pg.Shape.NumRecords;i++) {
		CurrentMin = GetLongTime(i + 1);
		if((CurrentMin-PreviousMin)!=minst) {
			m_EquiDistance=0;
			if(!GapProblemFinded) {
#ifndef COUPSTD
				string str="Gap in your PG file at record ("+FUtil::STD_ItoAscii(i)+")";
				MFC_Util::MessageBox(str.c_str()
					,"Can't be used for between year statistics",MB_OK|MB_ICONEXCLAMATION);
#endif
				GapProblemFinded=true;
			}
			return;
		}
		PreviousMin = CurrentMin;

	}
	pg.status.NormalTimeInterval=minst;
	m_IncreaseTimeTest=true;
	m_EquiDistance=minst;
}
bool CPGFile::IsTimeIncreasing()
{
	if(!m_IncreaseTimeTest) CheckIncreaseTime();
	return m_IncreaseTime;

}
size_t CPGFile::GetJumpBacksNum()
{
	return pg.Des.JumpBack.size();
}
string CPGFile::GetJumpBackDates(size_t rec)
{
	string date="";
	if(rec<pg.Des.JumpBack.size()) {
		date=GetDate(pg.Des.JumpBack[rec]);
	}
	return date;

}
bool CPGFile::AdjustSize(size_t irecnumber, size_t varnumber, size_t repit)
{
	SetInfoMode();

	DATASHAPE shape = SetDataShape(irecnumber, varnumber, repit);
	pg.Shape = shape;
	bool ok;
	if (pg.status.CompleteFileRead || pg.status.b.buf_Size >= pg.Shape.NumRecords) {
		ok = ReAllocateVarVector();

	}
	else {
		ok = AdjustDataShapeAndWriteNewRecords();
	}
	if (ok) ok = ReAllocateDescription();
	if (!ok) SetDataShape(shape.OldNumRecords, shape.OldNumRecords, shape.OldNumRepetitions);
	return ok;

}
void CPGFile::CheckIncreaseTime()
{	
#ifndef COUPSTD	
	CFileScanner ScanMeanProg(nullptr);
	ScanMeanProg.Create(IDD_PROGRESS,nullptr);
	ScanMeanProg.ShowWindow(SW_SHOW);
	ScanMeanProg.SetWindowText(_T("Checking increase of time in file"));
#endif

	pg.Des.JumpBack.resize(0);
	m_IncreaseTime=true;

	if(GetLongTime(1)<0) {
		ClearFile();


	}
	size_t Previous, Current;
	Previous = GetLongTime(1);
	for (size_t i=1; i<pg.Shape.NumRecords-1;i++) {
		Current = GetLongTime(i + 1);
		if(Current-Previous<0) {
			m_IncreaseTime=false;
			pg.Des.JumpBack.push_back(i+1);
		}
		if(i%10000==0) {
#ifndef COUPSTD
			ScanMeanProg.Update(i*100/pg.Shape.NumRecords);
#endif
		}
		Previous = Current;
	}

}
unsigned int CPGFile::GetNumRepititions()
{
	return pg.Shape.NumRepetitions;
}
void CPGFile::SetNormalTimeInterval(unsigned int t)
{
	pg.status.NormalTimeInterval=t;
}
unsigned int CPGFile::GetNormalTimeInterval()
{
	return pg.status.NormalTimeInterval;
}
unsigned int CPGFile::GetLongTimeStart()
{
	//return GetLongTime(1);
	return pg.status.MinStart;
}
bool CPGFile::SetAllLongTimesFromMinDiff(bool MissingAssign)
{
	size_t min = pg.status.MinStart;
	for (size_t i = 0; i < pg.Shape.NumRecords; i++) {
		if (SetMinToVarRecord(i + 1, min)) {
			SetRecordMissing(i + 1);
			min += pg.status.NormalTimeInterval;
		}
		else
			return false;
	}
	pg.status.MinEnd = min- pg.status.NormalTimeInterval;
	return true;
}
void CPGFile::SetCompleteRead(bool value)
{
	pg.status.CompleteFileRead=value;
}
void CPGFile::SetCompleteRepRead(bool value)
{
	pg.status.CompleteRepRead=value;
}
bool CPGFile::GetCompleteRead()
{
	return pg.status.CompleteFileRead;
}


void CPGFile::SetEndPoints_ValidValue() {
	m_FirstValid.resize(pg.Shape.NumVar);
	m_LastValid.resize(pg.Shape.NumVar);

	for(size_t i=0; i<pg.Shape.NumVar; i++) {
		size_t icount=1;
		while(GetVarValue(i+1, icount)<0.8*MISSING&&icount<pg.Shape.NumRecords)  {
			icount++;
		}
		if(icount<=pg.Shape.NumRecords) m_FirstValid[i]=GetVarValue(i+1, icount);
		else m_FirstValid[i]=MISSING;
		icount=pg.Shape.NumRecords;
		while(GetVarValue(i+1, icount)<0.8*MISSING&&icount>1)  {
			icount--;
		}
		if(icount>=1) m_LastValid[i]=GetVarValue(i+1, icount);
		else  m_LastValid[i]=MISSING;
	}



}
float CPGFile::GetVarMean(size_t VarNo, size_t repitno) {
	size_t actrep=GetRepititionSequence();
	if(SetRepititionSequence(repitno, true)) {
		double sum=0;
		size_t count=0;
		for(size_t i=0; i<pg.Shape.NumRecords; i++) {
			float val=GetVarValue(VarNo, i+1);
			if(val>-0.9E38) {
				sum+=val;
				count++;
			}
		}
		SetRepititionSequence(actrep, true);
		if(count>1) 
			return float(sum/count);
		else
			return MISSING;
	}
	else
		return MISSING;
}
