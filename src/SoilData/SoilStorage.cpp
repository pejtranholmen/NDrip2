#include "SoilStorage.h"
#include "../Util/FUtil.hpp"

SoilStorage::SoilStorage(void)
{
	IsMainEditOpen=false;
	IsExtraEditOpen=false;
	IsNewFormatFile=false;
	m_DataBaseDir="";
	m_NumProfile=0;
}


SoilStorage::~SoilStorage(void)
{
}

bool SoilStorage::OpenMainFile(string name,  bool write)
{
	if(m_HeaderFile.is_open()&&m_LevelFile.is_open()) {
		if(write) IsMainEditOpen=true;

		//m_MainStoreName=testname.substr(0,testname.size()-4);
		return m_HeaderFile.good();
	}
	else if(m_NewHeaderFile.is_open()&&m_LevelFile.is_open()) {
		if(write) IsMainEditOpen=true;

	//	m_MainStoreName=testname.substr(0,testname.size()-4);
		return m_NewHeaderFile.good();
	}


	string testname=name;
	auto pos=name.find(".csv");
	if(pos==string::npos) {



	}
	else {
		name=name.substr(0,pos);
		testname+=".dbb";
	}
	testname=name;
	testname+=".dbb";

	if(!FUtil::IsFileExisting(testname))
		m_LevelFile.open(testname,ios::binary|ios::out);
	else
		m_LevelFile.open(testname,ios::binary|ios::in|ios::out);

	testname=name;
	testname+=".dbn";
	if(!FUtil::IsFileExisting(testname)) {
		m_NewHeaderFile.open(testname, ios::binary|ios::out);
		testname=name;
		testname+=".csv";
		m_HeaderFile.open(testname,ios::binary|ios::in);
	}
	else {
		m_NewHeaderFile.open(testname, ios::binary|ios::out|ios::in);


	}
	testname=name;
	testname+=".csv";
	m_HeaderFile.open(testname,ios::binary|ios::in);

	if(m_HeaderFile.is_open()&&m_LevelFile.is_open()&&m_NewHeaderFile.is_open()) {

		m_MainStoreName=testname.substr(0,testname.size()-4);
		return true;
	}

	return false; 
}
bool SoilStorage::ReOpenLevelFile()
{
	m_LevelFile.clear();
	m_LevelFile.close();
	string testname=m_MainStoreName;
	testname+=".dbb";
	//if(write)
		m_LevelFile.open(testname,ios::binary|ios::in|ios::out);
	//else
	//	m_LevelFile.open(testname,ios::binary|ios::in);
	return m_LevelFile.is_open();
}
bool SoilStorage::OpenExtraFile(string name,  bool write)
{

	string testname=name;
	testname+=".dba";
	if(write)
		m_HeaderExtraFile.open(testname,ios::binary|ios::in|ios::out);
	else
		m_HeaderExtraFile.open(testname,ios::binary|ios::in);
	testname+=".dbb";
	if(write)
		m_LevelExtraFile.open(testname,ios::binary|ios::in|ios::out);
	else
		m_LevelExtraFile.open(testname,ios::binary|ios::in);
	if(m_HeaderExtraFile.is_open()&&m_LevelExtraFile.is_open()) {
		if(write) IsExtraEditOpen=true;
		m_ExtraStoreName=testname;
		return true;
	}
	return false; 
}
bool SoilStorage::OpenEditMainDataBase()
{ 

	if(m_LevelFile.is_open()) m_LevelFile.close();
	if(m_HeaderFile.is_open()) m_HeaderFile.close();
	if(m_NewHeaderFile.is_open()) m_NewHeaderFile.close();
	return OpenMainFile(m_MainStoreName, true);
}
bool SoilStorage::OpenEditExtraDataBase()
{

	return true;
}
void SoilStorage::CloseExtraDataBase()
{ 

	if(m_LevelExtraFile.is_open()) m_LevelExtraFile.close();
	if(m_HeaderExtraFile.is_open()) m_HeaderExtraFile.close();
}
void SoilStorage::CloseMainDataBase()
{ 
	if(m_LevelFile.is_open()) m_LevelFile.close();
	if(m_HeaderFile.is_open()) m_HeaderFile.close();
	if(m_NewHeaderFile.is_open()) m_NewHeaderFile.close();
	m_LevelFile.clear();
	m_NewHeaderFile.clear();
	m_HeaderFile.clear();



}
CSVFILEINFO SoilStorage::ScanningInputFile() {
	string str;
	CSVFILEINFO out;
	char buf[10000];
	m_HeaderFile.seekg(0,ios::end);
	auto sizeinputfile=m_HeaderFile.tellg();
	
	auto linebreak=string("\r\n");
	auto delimiter=string(",;\t ");
	m_HeaderFile.seekg(0,ios::beg);
	auto startpos=m_HeaderFile.tellg();
	std::streampos readchar=1024;
	size_t nrec=0;
	str.clear();
	out.EndLine=linebreak;
	m_MaxCharLine=0;

	bool Linebreaktest=false;
	while(startpos<sizeinputfile) {
		auto pos1=m_HeaderFile.tellg();
		if(size_t(sizeinputfile)<(size_t(pos1)+size_t(1024)))
			readchar=sizeinputfile-pos1;
			
		m_HeaderFile.read((char*)&buf,readchar);
		startpos=m_HeaderFile.tellg();
		auto nbyte = startpos;
		nbyte-=pos1;
		for (int i=0;i<nbyte;i++) {
			if(buf[i]>0) str+=buf[i];
		}
		if(nrec==0) {
			auto ndelim=0L;
			auto n=string::npos;
			auto Icount=0L;
			while(n==string::npos&&Icount<3) {				
				string dd=delimiter.substr(Icount,1);
				n=str.find(dd);
				Icount++;
			}
			if(n==-1) {
				return out;
			}
			ndelim=Icount-1;
			out.Delim=delimiter.substr(ndelim,1);
		}
		auto ich=str.find(linebreak);
		if(ich>m_MaxCharLine) m_MaxCharLine=ich;
		if(ich==string::npos&&!Linebreaktest) {
			if(linebreak=="\r\n")
				linebreak="\n";
			else if(linebreak=="\n")
				linebreak="\r\n";
			Linebreaktest=true;
			out.EndLine=linebreak;
			ich=str.find(linebreak);

		}
		while (ich>=0&&ich!=string::npos) {
			string line=str.substr(0,ich);
			if(nrec==0) {
				size_t ncount=0;
				while(line.find(out.Delim)!=string::npos) {
					ncount++;
					line=line.substr(line.find(out.Delim)+1);
				}
				out.NumVar=ncount+1;
			}
			nrec++;		
			str=str.substr(ich+linebreak.size());
			ich=str.find(linebreak);
			if(ich>m_MaxCharLine) m_MaxCharLine=ich;
		}

	}
	out.NumLines=nrec;
	out.SizeOfFile=sizeinputfile;
	m_FileInfo=out;
	m_HeaderFile.seekg(0,ios::beg);
	return out;


}
vector<string> SoilStorage::ReadLine() {
	string str;
	vector<string> out;

	char buf[5000];
	auto startpos=m_HeaderFile.tellg();
	std::streamsize readchar=1024;
	str.clear();
	size_t ilen=m_FileInfo.EndLine.size();

		auto pos1=m_HeaderFile.tellg();
		if(m_FileInfo.SizeOfFile<(pos1+std::streampos(1024)))
			readchar=m_FileInfo.SizeOfFile-pos1;
			
		m_HeaderFile.read((char*)&buf,readchar);
		auto newpos=m_HeaderFile.tellg();
		std::streamsize nbyte=newpos-pos1;
		for (std::streamsize i=0;i<nbyte;i++) {
			if(buf[i]>0) str+=buf[i];
		}
		auto ich=str.find(m_FileInfo.EndLine);
		if (ich>=0&&ich!=string::npos) {
			string line=str.substr(0,ich);
			size_t ncount=0;
			while(line.find(m_FileInfo.Delim)!=string::npos) {
					ncount++;
					auto pos=line.find(m_FileInfo.Delim);
					out.push_back(line.substr(0,pos));
					line=line.substr(pos+1);
			}
			if(line.size()>0) {
				out.push_back(line);
			}

		}
		m_HeaderFile.seekg(size_t(pos1)+ich+ilen,ios::beg);

	return out;




}
bool SoilStorage::WriteHeaderFile(vector<string> cols, size_t levelpos) {
/*if (!m_Rw.IsFileOutGood()) return false;
	size_t id;
	m_Rw.wPosition();
	id=FUtil::AtoInt(cols[0]);
	m_Rw.wInt(&id);
	double longitude=FUtil::AtoFloat(cols[1]);
	m_Rw.wDouble(&longitude);
	double latitude=FUtil::AtoFloat(cols[2]);
	m_Rw.wDouble(&latitude);
	m_Rw.wString(&cols[3]);
	m_Rw.wString(&cols[4]);
	size_t NumLay;
	NumLay=FUtil::AtoInt(cols[5]);
	m_Rw.wInt(&NumLay);
	NumLay=FUtil::AtoInt(cols[5]);
	m_Rw.wInt(&levelpos);
	m_NumProfile++;;
	return m_Rw.IsFileOutGood();*/
	return false;
}
bool SoilStorage::WriteLevelFile(vector<string> cols) {

	for(size_t i=6; i<m_FileInfo.NumVar; i++)
		if(cols[i].size()==0)
			m_LevelValues[i-6]=MISSING;
		else
			m_LevelValues[i-6]=FUtil::AtoFloat(cols[i]);
	m_LevelFile.write((char*)&m_LevelValues[0], 34*4);
	return m_NewHeaderFile.good();
}
bool SoilStorage::ReadNewHeaderFile() {
/*	if (!m_Rw.IsFileInGood()) return false;
	while(m_Rw.IsFileInGood()) {
		m_Rw.rsize_t(&m_ActProfile.RecordPos);
		m_Rw.rsize_t(&m_ActProfile.KeyProf);
		m_Rw.rDouble(&m_ActProfile.Long);
		m_Rw.rDouble(&m_ActProfile.Lat);
		m_Rw.rString(&m_ActProfile.Name);
		m_Rw.rString(&m_ActProfile.Type);
		m_Rw.rsize_t(&m_ActProfile.NumLayers);
		m_Rw.rsize_t(&m_ActProfile.NumProf);
		m_HeaderRec.insert(pair<size_t, size_t>(m_ActProfile.KeyProf, m_ActProfile.RecordPos));
	}
	m_Rw.CloseFileIn();
	IniNewDataBase();*/
	return true;
}
