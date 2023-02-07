#include "./GetDB.h"
#include "../../Util/FUtil.hpp"
#include "../Doc.h"
#include "./DB_Header.h"


GetDB::GetDB(void)
{
	
}


GetDB::~GetDB(void)
{
}

string GetDB::GetDBSimFileDate(string group, string Key, bool modified)
{
	string file="";
#ifndef COUPSTD
	file = MFC_Util::GetProfileStringStd("DataBaseDirectory", file);
#else
	file = FUtil::GetProfileStringStd("DataBaseDirectory", file);
#endif
	file += group + "\\DB_" + group + "_" + Key;
	file += ".Sim";
#ifndef COUPSTD
	return MFC_Util::GetWindowFileDate(file, modified);
#else
	return "";
#endif
	
}


bool GetDB::Ini_DBBase() {
	  string directory;
#ifndef COUPSTD
	  directory =MFC_Util::GetProfileStringStd("DataBaseDirectory", directory);
#else
	  directory=FUtil::GetProfileStringStd("DataBaseDirectory", directory);
#endif
	  size_t icount=0;
	  if(m_pSimDoc==nullptr) return false;
	  if(m_IniDataBaseMade) return false;
	  bool koll=false;
	  for(size_t i=0; i<9;i++) if(!m_ReadInDone[icount]) koll=true;
	  if(!koll) return false;
	  bool newformat = true;
	  for(int i=0;i<10;i++) {
			string str=m_pSimDoc->DB_GetNames(i).c_str();
			
			if(i!=1) {
				str += "\\DB_" + str + ".";
				if(m_Rw.OpenDocFileIn(directory+str+"DB")) {
					ReadOpenHeaderFile(i,false, true);
					m_Rw.CloseFileIn();
				}
				else {
					if (m_Rw.OpenDocFileIn(directory + str+"DBM")) {
						ReadOpenHeaderFile(i, false, false);
						m_Rw.CloseFileIn();
						newformat = false;
					}
				}
				icount++;
			}
	  }
	  if (!newformat) {
		  icount = 0;
		  for (int i = 0; i < 10; i++) {
			  string str = m_pSimDoc->DB_GetNames(i).c_str();
			  str += "\\DB_" + str + ".DBS";
			  if (i != 1) {

				  if (m_Rw.OpenDocFileIn(directory + str)) {
					  ReadOpenHeaderFile(i, true, false);
					  m_Rw.CloseFileIn();
				  }
				  icount++;
			  }
		  }
	  }


	  m_IniDataBaseMade=true;
	  return true;

}
bool GetDB::ReadOpenHeaderFile(size_t index, bool add, bool newformat) {
	if(!m_Rw.IsFileInGood()) return false;
	size_t adjust = 0;
	if (index > 0) adjust = 1;
	if(!add) m_OldHeader[index-adjust].clear();
	int numadd=0;
	int count=0;

	if(!add) {
		m_Rw.rInt(&m_NumOriginalRec[index-adjust]);
		numadd=m_NumOriginalRec[index-adjust];
	}
	else
		m_Rw.rInt(&numadd);
	m_Rw.m_DocFileIn.seekg(0,ios::end);
	auto koll=m_Rw.m_DocFileIn.tellg();
	m_Rw.m_DocFileIn.seekg(4,ios::beg);
	while(m_Rw.IsFileInGood()&&(count<numadd||newformat)) {
		DB_OldHeader head;
		m_Rw.rsize_t(&head.Type);
		if (m_Rw.IsFileInGood()) {
			m_Rw.rString(&head.Name);
			m_Rw.rString(&head.SiteName);
			m_Rw.rString(&head.Latitude);
			m_Rw.rString(&head.Longitude);
			m_Rw.rString(&head.Country);
			m_Rw.rString(&head.DriveFile);
			koll = m_Rw.m_DocFileIn.tellg();
			m_Rw.rString(&head.Details);
			koll = m_Rw.m_DocFileIn.tellg();
			m_Rw.rString(&head.IdUser);
			m_Rw.rsize_t(&head.IdNum);
			DB_NewHeader headnew;
			if (newformat) {

				m_Rw.rString(&headnew.ModelVersion);
				m_Rw.rString(&headnew.CreateDate);
				m_Rw.rString(&headnew.ModifiedDate);
				m_Rw.rString(&headnew.CreatedBy);
				m_Rw.rString(&headnew.ModifiedBy);
				headnew.CompareGroup = false;

			}
			else {
	
				headnew.ModelVersion = "Prior 2008";
				string str = m_pSimDoc->DB_GetNames(index).c_str();
	
				string date = GetDBSimFileDate(str, head.IdUser + FUtil::STD_ItoAscii(head.IdNum), false);


				headnew.CreateDate = date;
				date = GetDBSimFileDate(str, head.IdUser + FUtil::STD_ItoAscii(head.IdNum), true);
				headnew.ModifiedDate = date;
				headnew.CreatedBy="";
				headnew.ModifiedBy="";
				headnew.CompareGroup = false; 

	

			}
			string key = GetKeyString(head.IdUser, head.IdNum);
			auto valid = m_ValidNewHeader[index-adjust].insert(pair<string, DB_NewHeader>(key, headnew));
			if (valid.second) {
				if (m_Rw.IsFileInGood()) {
					m_NewHeader[index - adjust].push_back(headnew);
				}
				if (m_Rw.IsFileInGood()) m_OldHeader[index - adjust].push_back(head);

			}
			count++;
		}
	}
	if(m_OldHeader[index-adjust].size()==m_NumOriginalRec[index-adjust]) return true;
	else if(m_OldHeader[index-adjust].size()==m_NumOriginalRec[index-adjust]+numadd) return true;
	return false;
}
bool GetDB::WriteHeaderFile(size_t index) {
	 string directory;
#ifndef COUPSTD
	 directory = MFC_Util::GetProfileStringStd("DataBaseDirectory", directory);
#else
	 directory=FUtil::GetProfileStringStd("DataBaseDirectory", directory);
#endif

	 size_t adjust = 0;
	 if (index > 0) adjust = 1;
	 string str=m_pSimDoc->DB_GetNames(index+adjust).c_str();
	 str += "\\DB_" + str + ".DB";
	 bool check = true;
	 //Original File
	 if (m_Rw.OpenDocFileout(directory + str)) {
		 int numadd;
		 numadd =  m_NumOriginalRec[index];
		 m_Rw.wInt(&numadd);

		 for (size_t i = 0; i<m_NewHeader[index].size(); i++) {
			 DB_OldHeader head = m_OldHeader[index][i];
			 DB_NewHeader headnew = m_NewHeader[index][i];
			 string key=GetKeyString(head.IdUser, head.IdNum);
			 auto valid=m_ValidNewHeader[index].insert(pair<string, DB_NewHeader>(key, headnew));
			 if (valid.second) {
				 m_Rw.wInt(&head.Type);
				 m_Rw.wString(&head.Name);
				 m_Rw.wString(&head.SiteName);
				 m_Rw.wString(&head.Latitude);
				 m_Rw.wString(&head.Longitude);
				 m_Rw.wString(&head.Country);
				 m_Rw.wString(&head.DriveFile);
				 auto koll = m_Rw.m_DocFileOut.tellp();
				 m_Rw.wString(&head.Details);
				 koll = m_Rw.m_DocFileOut.tellp();
				 m_Rw.wString(&head.IdUser);
				 m_Rw.wInt(&head.IdNum);
				 m_Rw.wString(&headnew.ModelVersion);
				 m_Rw.wString(&headnew.CreateDate);
				 m_Rw.wString(&headnew.ModifiedDate);
				 m_Rw.wString(&headnew.CreatedBy);
				 m_Rw.wString(&headnew.ModifiedBy);
			 }
		 }
	 }
	 else
		 check = false;
	 m_Rw.CloseFileOut();

	 return check;
}

size_t GetDB::GetNumOfRec(size_t index) {

	return m_OldHeader[index].size();

}
DB_OldHeader GetDB::GetHeader(size_t index_type, size_t irow) {
	DB_OldHeader header;
	if (index_type<9 && m_OldHeader[index_type].size()>irow) {
		header = m_OldHeader[index_type][irow];
	}
	else
		header.IdNum = -10;
	return header;
}
DB_NewHeader GetDB::GetNewHeader(size_t index_type, size_t irow) {
	DB_NewHeader headernew;
	if (index_type<9 && m_NewHeader[index_type].size()>irow) {
		headernew = m_NewHeader[index_type][irow];
	}
	else
		headernew.ModelVersion = "";
	return headernew;
}
string GetDB::GetTypeString(size_t index_type, size_t typ) {
	if(m_PlantTypeIndex[index_type]<5&& typ<m_PlantType[m_PlantTypeIndex[index_type]].size())
		return m_PlantType[m_PlantTypeIndex[index_type]][typ];
	else
		return m_PlantType[m_PlantTypeIndex[index_type]][m_PlantType[m_PlantTypeIndex[index_type]].size()-1];

}
string GetDB::GetKeyString(string ID, size_t idnum) {
	ID+=FUtil::STD_ItoAscii(idnum);
	return ID;
}
string GetDB::GetKeyString() {
	size_t index = GetCurrentKeyIndex();
	DB_OldHeader header;
	header = m_OldHeader[m_DB_Index][index];
	return GetKeyString(header.IdUser, header.IdNum);
};
size_t GetDB::CreateNewHeader(size_t db_index) {
	DB_OldHeader head;	
	DB_NewHeader headnew;
	head.Type=0;
	head.Name="";
	head.SiteName="";
	head.DriveFile="";

	Ps *pPs;
	if(m_pSimDoc!=nullptr) {
		auto file= FUtil::FileNameOnly(m_pSimDoc->m_CurrentFile);
		auto pos = file.find(".");
		if (pos != string::npos)
			head.Name = file.substr(0,pos);
		headnew.ModelVersion = m_pSimDoc->m_DocFile2.m_ExeFileDate;
		head.Details="Sim File:"+ m_pSimDoc->m_CurrentFile+"\n";


		pPs=dynamic_cast<Ps*>(m_pSimDoc->GetPtr(PAR_SINGLE,"Y-coordinate"));
		if(pPs->GetValue()==0) pPs = dynamic_cast<Ps*>(m_pSimDoc->GetPtr(PAR_SINGLE, "Latitude"));
		if(pPs!=nullptr) head.Latitude=FUtil::STD_DtoAscii(pPs->GetValue());
		pPs=dynamic_cast<Ps*>(m_pSimDoc->GetPtr(PAR_SINGLE,"X-coordinate"));
		if(pPs!=nullptr) head.Longitude=FUtil::STD_DtoAscii(pPs->GetValue());
	}
	else
		head.Details = "";
	head.Country="";
	
	head.Details+="Edit this text to include method of estimation and \r\nreferences to publications/reports. \r\n";
	//CWinApp* pApp = AfxGetApp();
	string signature=FUtil::GetProfileStringStd( "Signature", "NN");
	
	headnew.CreateDate = FUtil::GetCurrentDateTime();
	headnew.ModifiedDate = "";
	headnew.CreatedBy=FUtil::GetWindowUser();
	headnew.ModifiedBy = "";
	headnew.CompareGroup = true;


	size_t maxidnum=0;
	for(size_t i=0; i<GetNumOfRec(db_index); i++) 
		if(maxidnum<m_OldHeader[db_index][i].IdNum&&signature==m_OldHeader[db_index][i].IdUser) maxidnum=m_OldHeader[db_index][i].IdNum;
	head.IdNum=maxidnum+1;
	head.IdUser=signature;

	m_OldHeader[db_index].push_back(head);
	m_NewHeader[db_index].push_back(headnew);

	return m_OldHeader[db_index].size()-1;
}

CDB* GetDB::CreateNewDBObject_FromCSVFile(string FileName)
{
	ifstream stream(FileName);

	string NewOutPutFileName;
	string line;
	string DataBaseName="";
	size_t linecount = 0;
	char delim = ';';
	CDB *pCDB=nullptr;
	DB_OldHeader header;
	DB_NewHeader headnew;

	string group, type, name, strvalue;
	auto checkdelim_DBType = [&]()	{
		if (line.find(delim) == string::npos) {	
			delim = ',';
			if (line.find(delim) == string::npos) return pCDB;
		}

		string type = line.substr(line.find(delim)+1);
		pCDB= m_pSimDoc->DB_GetPointer(type);
		if (pCDB == nullptr) return pCDB;
		DataBaseName = type;
		
		size_t index = CreateNewHeader(pCDB->Get_DB_Index());
		SetCurrentKeyLink(pCDB->Get_DB_Index(), index);
		header = GetHeader(pCDB->Get_DB_Index(), index);
		header.Details = "";
		headnew = GetNewHeader(pCDB->Get_DB_Index(), index);
		return pCDB;
	
	};
	size_t NumOfDetails=string::npos;
	auto readheader = [&](size_t lineno) {
		string item = line.substr(0, line.find(delim));
		string value = line.substr(line.find(delim) + 1);
		switch(lineno) {
		case 1:
			header.IdUser = value;
			break;
		case 2:
			header.Type = GetPlantTypeIndex(pCDB->Get_DB_Index(), value);
			break;
		case 3:
			header.Name = value;
			break;
		case 4:
			header.SiteName = value;
			break;
		case 5:
			header.Latitude = value;
			break;
		case 6:
			header.Longitude = value;
			break;
		case 7:
			header.Country = value;
			break;
		case 8:
			headnew.CreateDate= value;
			break;
		case 9:
			NumOfDetails = FUtil::AtoInt(value);
			for (size_t l = 0; l < NumOfDetails; l++) {
				getline(stream, line);
				string value = line.substr(line.find(delim) + 1);
				header.Details += value+'\n';
			}
			if (SetHeader(pCDB->Get_DB_Index(), GetCurrentKeyIndex(), header)) {
				headnew.ModifiedDate = FUtil::GetCurrentDateTime();
				headnew.ModifiedBy = FUtil::GetWindowUser();
				SetNewHeader(pCDB->Get_DB_Index(), GetCurrentKeyIndex(), headnew);
				
				NewOutPutFileName= FUtil::GetProfileStringStd("DataBaseDirectory", "");
				NewOutPutFileName += DataBaseName + "\\DB_" + DataBaseName + "_" + GetKeyString(header.IdUser, header.IdNum);
				NewOutPutFileName += ".Sim";
				m_Rw.OpenDocFileout(NewOutPutFileName);
			}
			break;

		}
	};
	auto read_db_values = [&]() {
		size_t iread = 0;
		if (line.find("Start")==0) {
			m_Rw.wString( &line);
			getline(stream, line);
			iread = FUtil:: AtoInt(line);
			m_Rw.wInt(&iread);
		}
		else if (line.find("Switches")==0) {
			m_Rw.wString(&line);
			getline(stream, line);
			iread = FUtil::AtoInt(line);
			m_Rw.wInt(&iread);
			for (size_t i = 0; i < iread; i++) {
				getline(stream, line);
				type = line.substr(0, line.find(delim));
				line = line.substr(line.find(delim) + 1);
				group = line.substr(0, line.find(delim));
				m_Rw.wString(&group);
				line = line.substr(line.find(delim) + 1);
				name = line.substr(0, line.find(delim));
				m_Rw.wString(&name);
				strvalue = line.substr(line.find(delim) + 1);
				Sw* pSw = dynamic_cast<Sw*>(m_pSimDoc->GetPtr(type, group, name));
				if (pSw == nullptr&&type == "Start") {
					getline(stream, line);
					continue;
				}
				else
					pSw->SetDBStrValue(strvalue);
					int value = pSw->GetDBIntValue();
					m_Rw.wInt(&value);



			}


		}
		else if (line.find("Parameters")==0) {
			m_Rw.wString(&line);
			getline(stream, line);
			iread = FUtil::AtoInt(line);
			m_Rw.wInt(&iread);
			for (size_t i = 0; i < iread; i++) {
				getline(stream, line);
				type = line.substr(0, line.find(delim));
				line = line.substr(line.find(delim) + 1);
				group = line.substr(0, line.find(delim));
				m_Rw.wString(&group);
				line = line.substr(line.find(delim) + 1);
				name = line.substr(0, line.find(delim));
				m_Rw.wString(&name);
				strvalue = line.substr(line.find(delim) + 1);
				Ps* pPs = dynamic_cast<Ps*>(m_pSimDoc->GetPtr(type, group, name));
				if (pPs != nullptr) {
					float fvalue = FUtil::AtoFloat(strvalue);
					pPs->SetDBValue(FUtil::AtoFloat(strvalue));
					m_Rw.wFloat(&fvalue);
				}
			}


		}
		else if (line.find("Tables")==0) {
			m_Rw.wString(&line);
			getline(stream, line);
			iread = FUtil::AtoInt(line);
			m_Rw.wInt(&iread);
			for (size_t i = 0; i < iread; i++) {
				getline(stream, line);
				type = line.substr(0, line.find(delim));
				line = line.substr(line.find(delim) + 1);
				group = line.substr(0, line.find(delim));
				m_Rw.wString(&group);
				line = line.substr(line.find(delim) + 1);
				name = line.substr(0, line.find(delim));
				m_Rw.wString(&name);
				strvalue = line.substr(line.find(delim) + 1);
				Tab* pTab = dynamic_cast<Tab*>(m_pSimDoc->GetPtr(type, group, name));
				int nrows = FUtil::AtoInt(strvalue);
				pTab->SetDB_Row(FUtil::AtoInt(strvalue));

				getline(stream, line);
				type = line.substr(0, line.find(delim));
				line = line.substr(line.find(delim) + 1);
				strvalue = line.substr(0, line.find(delim));
				int ncol = FUtil::AtoInt(strvalue);
				m_Rw.wInt(&ncol);
				for (size_t col = 0; col < ncol; col++) {
					getline(stream, line);
					type = line.substr(0, line.find(delim));
					line = line.substr(line.find(delim) + 1);
					m_Rw.wString(&line);
				}

				m_Rw.wInt(&nrows);


				for (size_t row = 0; row < nrows; row++) {
					for (size_t col = 0; col < ncol; col++) {
						getline(stream, line);
						type = line.substr(0, line.find(delim));
						line = line.substr(line.find(delim) + 1);
						group = line.substr(0, line.find(delim));
						line = line.substr(line.find(delim) + 1);
						name = line.substr(0, line.find(delim));
						FUtil::trim_index(name);
						strvalue = line.substr(line.find(delim) + 1);
						P* pP = dynamic_cast<P*>(m_pSimDoc->GetPtr(type, group, name));
						if (pP != nullptr);
						float fvalue = FUtil::AtoFloat(strvalue);
						pP->SetDBValue(row,double(fvalue));
						m_Rw.wFloat(&fvalue);
					}
				}
			}
		}
		else if (line.find("Files")==0) {
			m_Rw.wString(&line);
			getline(stream, line);
			iread = FUtil::AtoInt(line);
			m_Rw.wInt(&iread);
			for (size_t i = 0; i < iread; i++) {
				getline(stream, line);
				type = line.substr(0, line.find(delim));
				line = line.substr(line.find(delim) + 1);
				group = line.substr(0, line.find(delim));
				m_Rw.wString(&group);
				line = line.substr(line.find(delim) + 1);
				name = line.substr(0, line.find(delim));
				m_Rw.wString(&name);
				strvalue = line.substr(line.find(delim) + 1);
				F* pF = dynamic_cast<F*>(m_pSimDoc->GetPtr(type, group, name));
				if (pF != nullptr) {
					pF->SetDBValue(strvalue);
					m_Rw.wString(&strvalue);
				}
			}


		}
		else if (line.find("End") == 0) {
			m_Rw.wString(&line);
		}


	};
	std::getline(stream, line);

	checkdelim_DBType();

	while(std::getline(stream, line)) {
		linecount++;
		if (line.size() > 0) {
			if(NumOfDetails==string::npos)
				readheader(linecount);
			else {
				read_db_values();

			}



		}
	}
	m_Rw.CloseFileOut();
	return pCDB;
}
