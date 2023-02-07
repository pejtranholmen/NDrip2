#include "../../std.h"
#include "PFStorage.h"
#include "../../Util/FUtil.hpp"

PFStorage::PFStorage(void)
{
	IsMainEditOpen=false;
	IsExtraEditOpen=false;
	IsNewFormatFile=false;
	m_DataBaseDir="";
	m_PF_FileVersion = 0;
}


PFStorage::~PFStorage(void)
{
}
bool PFStorage::OpenMainFile(string name,  bool write)
{
	string testname = name;

	testname = name;
	if(m_HeaderFile.is_open()&&m_LevelFile.is_open()) {
		if(write) IsMainEditOpen=true;

		//m_MainStoreName=testname.substr(0,testname.size()-4);
		return m_HeaderFile.good();
	}
	else if(m_NewHeaderFile.is_open()&&m_LevelFile.is_open()) {
		if(write) IsMainEditOpen=true;

	   // m_MainStoreName=testname.substr(0,testname.size()-4);
		return m_NewHeaderFile.good();
	}



	testname+=".dbb";

	if(write)
		m_LevelFile.open(testname,ios::binary|ios::in|ios::out);
	else
		m_LevelFile.open(testname,ios::binary|ios::in);

	testname=name;
	string testname_n= testname + ".dbn";
	string testname_db = testname + ".db";
	if(!FUtil::IsFileExisting(testname_n)&& !FUtil::IsFileExisting(testname_db)) {
		//m_NewHeaderFile.open(testname_n, ios::binary|ios::out);
		testname=name;
		testname+=".dba";
		if(write)
			m_HeaderFile.open(testname,ios::binary|ios::in|ios::out);
		else
			m_HeaderFile.open(testname,ios::binary|ios::in);

		m_HeaderFile.seekp(0, ios::end);
		auto endpos = m_HeaderFile.tellp();
		m_RwNewHeader.OpenDocFileout(testname_db);

	}
	else if (FUtil::IsFileExisting(testname_n) && !FUtil::IsFileExisting(testname_db))	{
		
		m_NewHeaderFile.open(testname_n, ios::binary|ios::in);

		m_RwNewHeader.OpenDocFileout(testname_db);
		IsNewFormatFile = true;

	}
	else {
		IsNewFormatFile=true;
		if (write) {
			m_RwNewHeader.OpenDocFileout(testname_db);
		}
		else {
			if (!FUtil::IsFileExisting(testname_db)) 
				m_RwNewHeader.OpenDocFileout(testname_db);
			m_RwNewHeader.OpenDocFileIn(testname_db);
		}

	}
/*	testname=name;
	testname+=".dbl";

	if(!FUtil::IsFileExisting(testname)) 
		m_NewLevelFile.open(testname, ios::binary|ios::out);
	else {
		if(write)
			m_NewLevelFile.open(testname,ios::binary|ios::in|ios::out);
		else
			m_NewLevelFile.open(testname,ios::binary|ios::in);
	}*/

	if(m_HeaderFile.is_open()&&m_LevelFile.is_open()) {
		if(write) IsMainEditOpen=true;

		m_MainStoreName=name;
		return true;
	}
	else if((m_NewHeaderFile.is_open()|| m_RwNewHeader.IsFileInGood()|| m_RwNewHeader.IsFileOutGood()) &&m_LevelFile.is_open()) {
		if(write) IsMainEditOpen=true;
		m_MainStoreName=name;

		return true;
	}

	return false; 
}
bool PFStorage::ReOpenLevelFile()
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
bool PFStorage::OpenExtraFile(string name,  bool write)
{

	string testname=name;
	testname+=".dbn";
	if(write)
		m_HeaderExtraFile.open(testname,ios::binary|ios::in|ios::out);
	else
		m_HeaderExtraFile.open(testname,ios::binary|ios::in);
	testname = name;
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
bool PFStorage::OpenEditMainDataBase()
{ 

	if(m_LevelFile.is_open()) m_LevelFile.close();
	//if(m_HeaderFile.is_open()) m_HeaderFile.close();
	if(m_NewHeaderFile.is_open()) m_NewHeaderFile.close();
	m_RwNewHeader.CloseFileIn();
	m_RwNewHeader.CloseFileOut();

	return OpenMainFile(m_MainStoreName, true);
}
bool PFStorage::OpenEditExtraDataBase()
{

	return true;
}

bool PFStorage::ReadWrite_NewHeaderLine(bool read)
{
	if (read) {
		m_RwNewHeader.rsize_t(&m_ActProfile.KeyProf);
		m_RwNewHeader.rString(&m_ActProfile.Name);
		m_RwNewHeader.rString(&m_ActProfile.Country);
		m_RwNewHeader.rString(&m_ActProfile.County);
		m_RwNewHeader.rFloat(&m_ActProfile.Lat);
		m_RwNewHeader.rFloat(&m_ActProfile.Long);
		m_RwNewHeader.rFloat(&m_ActProfile.Upper_Clay);
		m_RwNewHeader.rFloat(&m_ActProfile.Upper_Sand);
		m_RwNewHeader.rFloat(&m_ActProfile.Upper_Org);
		m_RwNewHeader.rFloat(&m_ActProfile.Lower_Clay);
		m_RwNewHeader.rFloat(&m_ActProfile.Lower_Sand);
		m_RwNewHeader.rFloat(&m_ActProfile.Lower_Org);
		m_RwNewHeader.rsize_t(&m_ActProfile.NumLayers);
		m_RwNewHeader.rsize_t(&m_ActProfile.LowDepth);
		m_RwNewHeader.rsize_t(&m_ActProfile.Year);
		m_RwNewHeader.rsize_t(&m_ActProfile.RecLayers);
		m_RwNewHeader.rsize_t(&m_ActProfile.RecHeader);

		m_RwNewHeader.rString(&m_ActProfile.CreateDate);
		m_RwNewHeader.rString(&m_ActProfile.CreatedBy);
		m_RwNewHeader.rString(&m_ActProfile.ModifiedBy);
		m_RwNewHeader.rString(&m_ActProfile.ModifiedDate);
		return m_RwNewHeader.IsFileInGood();
	}
	else {
		m_RwNewHeader.wInt(&m_ActProfile.KeyProf);
		m_RwNewHeader.wString(&m_ActProfile.Name);
		m_RwNewHeader.wString(&m_ActProfile.Country);
		m_RwNewHeader.wString(&m_ActProfile.County);
		m_RwNewHeader.wFloat(&m_ActProfile.Lat);
		m_RwNewHeader.wFloat(&m_ActProfile.Long);
		m_RwNewHeader.wFloat(&m_ActProfile.Upper_Clay);
		m_RwNewHeader.wFloat(&m_ActProfile.Upper_Sand);
		m_RwNewHeader.wFloat(&m_ActProfile.Upper_Org);
		m_RwNewHeader.wFloat(&m_ActProfile.Lower_Clay);
		m_RwNewHeader.wFloat(&m_ActProfile.Lower_Sand);
		m_RwNewHeader.wFloat(&m_ActProfile.Lower_Org);
		m_RwNewHeader.wInt(&m_ActProfile.NumLayers);
		m_RwNewHeader.wInt(&m_ActProfile.LowDepth);
		m_RwNewHeader.wInt(&m_ActProfile.Year);
		m_RwNewHeader.wInt(&m_ActProfile.RecLayers);
		m_RwNewHeader.wInt(&m_ActProfile.RecHeader);

		m_RwNewHeader.wString(&m_ActProfile.CreateDate);
		m_RwNewHeader.wString(&m_ActProfile.CreatedBy);
		m_RwNewHeader.wString(&m_ActProfile.ModifiedBy);
		m_RwNewHeader.wString(&m_ActProfile.ModifiedDate);

		return m_RwNewHeader.IsFileOutGood();
	}
}

void PFStorage::CloseExtraDataBase()
{ 

	if(m_LevelExtraFile.is_open()) m_LevelExtraFile.close();
	if(m_HeaderExtraFile.is_open()) m_HeaderExtraFile.close();
}
void PFStorage::CloseMainDataBase()
{ 
	if(m_LevelFile.is_open()) m_LevelFile.close();
	if(m_HeaderFile.is_open()) m_HeaderFile.close();
	if(m_NewHeaderFile.is_open()) m_NewHeaderFile.close();
	m_RwNewHeader.CloseFileIn();
	m_RwNewHeader.CloseFileOut();
	m_LevelFile.clear();
	m_NewHeaderFile.clear();
	m_HeaderFile.clear();
}
