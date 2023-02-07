#include "../../std.h"
#include "../../Util/FUtil.hpp"
#include "../PGUtil.h"
#include "ExtraLinkedInformation.h"
#include "../../NewBase/CommonModelInfo.h"
#include "../../NewBase/ModelMap.h"


ExtraLinkedInformation::ExtraLinkedInformation()
{
}


ExtraLinkedInformation::~ExtraLinkedInformation()
{
}
void ExtraLinkedInformation::SetSwissCoord(size_t statno) {
	SWISSCORD cord;
	auto it=m_SwissMap.find(statno);
	if(it!=m_SwissMap.end()) {
		cord=(*it).second;
		for(size_t i=0;i<pg.Shape.NumVar;i++) {
				pg.Des.addinfo[i+m_countf*pg.Shape.NumVar].Long=double(cord.x);
				pg.Des.addinfo[i+m_countf*pg.Shape.NumVar].Lat=double(cord.y);
				pg.Des.addinfo[i+m_countf*pg.Shape.NumVar].Alt=double(cord.z);
				pg.Des.addinfo[i+m_countf*pg.Shape.NumVar].CountryCode="Ch";
		}
	}


}
bool ExtraLinkedInformation::ReadSwissCoord() {
    #ifndef COUPSTD
		CString pszFileName ;
		LPCTSTR lpszDefExt=_T("txt");
		LPCTSTR lpszFilter = _T("Text File (*.txt)|*.txt");
		pszFileName="";
 		CFileDialog	NewFileDlg(TRUE,lpszDefExt,pszFileName, OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,lpszFilter);
		if(NewFileDlg.DoModal()==IDOK) {
			CString str=NewFileDlg.GetPathName();
			ifstream inputfile;
			inputfile.open(str, ifstream::in);
			string line;
			line.resize(1200);
			inputfile.getline(&line[0],1200);
			while(inputfile.good()) {
				inputfile.getline(&line[0],1200);
				if(inputfile.good()) {
					size_t ipos;
					size_t id;
					SWISSCORD cord;

					ipos=line.find('\t');
					for(size_t i=0; i<4;i++) {
						string sub;
						sub=line.substr(0,ipos);
						size_t value=FUtil::AtoInt(sub);
						line=line.substr(ipos+1);
						ipos=line.find('\t');
						switch(i) {
						case 0:
							id=value;
							break;
						case 1:
							cord.x=value;
							break;
						case 2:
							cord.y=value;
							break;
						case 3:
							cord.z=value;
						}
					

					}
					size_t koll=line.size();
					line.clear();
					line.resize(1200);
					m_SwissMap.insert(pair<size_t,SWISSCORD>(id,cord));
				}
			}
			if(m_SwissMap.size()>0) return true;
		}
#endif
		return false;
}
void ExtraLinkedInformation::SetCoord(size_t statno) {
	DEC_COORD cord;
	auto it=m_CoordMap.find(statno);
	if(it!=m_CoordMap.end()) {
		cord=(*it).second;
		for(size_t i=0;i<pg.Shape.NumVar;i++) {
			pg.Des.addinfo[i+m_countf*pg.Shape.NumVar].Long=cord.x;
			pg.Des.addinfo[i+m_countf*pg.Shape.NumVar].Lat=cord.y;
			pg.Des.addinfo[i+m_countf*pg.Shape.NumVar].Alt=cord.z;
			pg.Des.addinfo[i+m_countf*pg.Shape.NumVar].CountryCode="Czech";
		}
	}
}
bool ExtraLinkedInformation::ReadCoord() {
#ifndef COUPSTD
		CString text;

		MFC_Util::MessageBox("You should have a text (csv) file where \n"
			"the Id for the repetitions and the respectively coordinates ares specified\n"
			"Note that the each line should corresponds to ID,Lat,Long and Alt \n"
			"First line is header ","Reading of text file with ID and coordinates", MB_OK);
	

		InPGFiles pgfile;
		CString pszFileName ;
		LPCTSTR lpszDefExt=_T("txt");
		LPCTSTR lpszFilter =_T("Text File (*.csv)|*.csv");
		pszFileName="";
		m_ImportedFileVector.clear();
 		CFileDialog	NewFileDlg(TRUE,lpszDefExt,pszFileName, OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR,lpszFilter);
		if(NewFileDlg.DoModal()==IDOK) {
			CString str=NewFileDlg.GetPathName();
			ifstream inputfile;
			inputfile.open(str, ifstream::in);
			string line;
			line.resize(1200);
			inputfile.getline(&line[0],1200);
			while(inputfile.good()) {
				inputfile.getline(&line[0],1200);
				if(inputfile.good()) {
					size_t ipos;
					size_t id;
					DEC_COORD cord;

					ipos=line.find(';');
					if(ipos==string::npos) ipos=line.find(',');
					for(size_t i=0; i<4;i++) {
						string sub;
						sub=line.substr(0,ipos);
						size_t value=FUtil::AtoInt(sub);
						line=line.substr(ipos+1);
						ipos=line.find(';');
						if(ipos==string::npos) ipos=line.find(',');
						switch(i) {
						case 0:
							value=FUtil::AtoInt(sub);
							id=value;
							break;
						case 2:

							cord.x=FUtil::AtoFloat(sub);
							break;
						case 1:
							cord.y=FUtil::AtoFloat(sub);
							break;
						case 3:
							cord.z=FUtil::AtoFloat(sub);
							break;
						}
					

					}
					pgfile=m_CurrentFile;
					if(id<10) pgfile.StatNo="000"+FUtil::STD_ItoAscii(id);
					else if(id<100)  pgfile.StatNo="00"+FUtil::STD_ItoAscii(id);
					else if(id<1000)  pgfile.StatNo="0"+FUtil::STD_ItoAscii(id);
					else pgfile.StatNo=FUtil::STD_ItoAscii(id);
					size_t koll=line.size();
					line.clear();
					line.resize(1200);
					m_CoordMap.insert(pair<size_t,DEC_COORD>(id,cord));
					m_ImportedFileVector.push_back(pgfile);
				}
			}
			if(m_CoordMap.size()>0) {
				m_CollectedFile=true;
				m_NumToImport=m_ImportedFileVector.size();
				return true;
			}
			else return false;
		}
    #endif
		return false;

    
}
bool ExtraLinkedInformation::DefineAddInfoMap() {

	m_AddInfoMap.clear();
	ifstream inputfile;
	inputfile.open(m_ExtraInfoFileName, ifstream::in);
	string line;
	line.resize(1200);
	inputfile.getline(&line[0], 1200);
	while (inputfile.good()) {
		inputfile.getline(&line[0], 1200);
		if (inputfile.good()) {
			AddInfo addinfo;
			vector<string> VarStr = FUtil::GetStringVectorFromStringLine(line, m_ExtraFileInfo.FirstValidHeads.size());
			addinfo.StationName = VarStr[AddInfoLinkIndex[0]];
			addinfo.CountryCode = VarStr[AddInfoLinkIndex[1]];
			addinfo.Key = FUtil::AtoInt(VarStr[0]);
			addinfo.Lat = FUtil::AtoFloat(VarStr[AddInfoLinkIndex[2]]);
			addinfo.Long = FUtil::AtoFloat(VarStr[AddInfoLinkIndex[3]]);
			addinfo.Alt = FUtil::AtoFloat(VarStr[AddInfoLinkIndex[4]]);
			m_AddInfoMap.insert(pair<size_t, AddInfo>(addinfo.Key, addinfo));
		}
	}
	m_RecordsExtraInfoFile = m_AddInfoMap.size();
	if(m_AddInfoMap.size()>0) return true;
	else return false;
}
bool ExtraLinkedInformation::SetAddInfoFromDefinedMap(size_t Key)
{
	AddInfo info;
	auto it = m_AddInfoMap.find(Key);
	if (it != m_AddInfoMap.end()) {
		info = (*it).second;
		for (size_t i = 0; i<pg.Shape.NumVar; i++) {
			if (pg.Shape.NumRepetitions>1&&pg.Des.addinfo.size() > i + m_countf*pg.Shape.NumVar) {
				pg.Des.addinfo[i + m_countf*pg.Shape.NumVar].Long = info.Long;
				pg.Des.addinfo[i + m_countf*pg.Shape.NumVar].Lat = info.Lat;
				pg.Des.addinfo[i + m_countf*pg.Shape.NumVar].Alt = info.Alt;
				pg.Des.addinfo[i + m_countf*pg.Shape.NumVar].CountryCode = info.CountryCode;
				pg.Des.addinfo[i + m_countf*pg.Shape.NumVar].StationName = info.StationName;
			}
			else if (pg.Des.addinfo.size() > i) {
				pg.Des.addinfo[i].Long = info.Long;
				pg.Des.addinfo[i].Lat = info.Lat;
				pg.Des.addinfo[i].Alt = info.Alt;
				pg.Des.addinfo[i].CountryCode = info.CountryCode;
				pg.Des.addinfo[i].StationName = info.StationName;
			}
		}
	}
	return false;
}
bool ExtraLinkedInformation::SetFileDescriptionFromScannedFile()
{

	//	pg.Des = m_CurrentFileInfo.Des;/
	string extrastepId;
	size_t count = 0;
	for(size_t ir=0; ir<m_CurrentFileInfo.LinkedSites.back().TimeCheck.RecordStep; ir++)
	for_each(m_NewVarIndexSelected.begin(), m_NewVarIndexSelected.end(), [&](int &index) {
		if (m_CurrentFileInfo.LinkedSites.back().TimeCheck.RecordStep > 1) {
			extrastepId = " (" + FUtil::STD_ItoAscii(ir + 1) + ")";

		}
		else
			extrastepId = "";
		size_t index_out;
		if(pg.Shape.NumRepetitions>m_countf&&pg.Des.base.Name.size()>=m_countf*m_NewVarIndexSelected.size())
			index_out= count + m_countf*m_NewVarIndexSelected.size();
		else
			index_out = count;
		if (pg.Des.base.Name.size() > index_out&&m_CurrentFileInfo.Des.base.Name.size()>index) {
			pg.Des.base.Name[index_out] = m_CurrentFileInfo.Des.base.Name[index];
			pg.Des.base.Unit[index_out] = m_CurrentFileInfo.Des.base.Unit[index];
			pg.Des.base.Id[index_out] = m_CurrentFileInfo.Des.base.Id[index]+ extrastepId;
			pg.Des.base.Pos[index_out] = m_CurrentFileInfo.Des.base.Pos[index];
			pg.Des.addinfo[index_out].StationName = m_CurrentFileInfo.Des.addinfo[index].StationName;
			pg.Des.addinfo[index_out].CountryCode = m_CurrentFileInfo.Des.addinfo[index].CountryCode;
			pg.Des.addinfo[index_out].Lat = m_CurrentFileInfo.Des.addinfo[index].Lat;
			pg.Des.addinfo[index_out].Long = m_CurrentFileInfo.Des.addinfo[index].Long;
			pg.Des.addinfo[index_out].Alt = m_CurrentFileInfo.Des.addinfo[index].Alt;
			if (m_format == SMHI_OPENDATA) {
				pg.Des.addinfo[index_out].Lat = m_CurrentFileInfo.dec_cord.y;
				pg.Des.addinfo[index_out].Long = m_CurrentFileInfo.dec_cord.x;
				pg.Des.addinfo[index_out].Alt = m_CurrentFileInfo.dec_cord.z;
				pg.Des.addinfo[index_out].StationName = m_CurrentFileInfo.StatName;
				pg.Des.base.Id[index_out] = m_CurrentFileInfo.StatNo;
			}
			count++;
		}
	});






	return false;
}

bool ExtraLinkedInformation::SetFileDescriptionFromLinkedFileDataBase()
{

	auto ReturnFileNameBase = [&]() {
		size_t ip = m_filename.find(".txt");
		if (ip == string::npos)
			ip = m_filename.find(".csv");
		if (ip == string::npos)
			ip = m_filename.find(".");
		if (ip != string::npos)
			return m_filename.substr(0, ip);
		else
			return m_filename;
	};


				if(m_format==SWISS) {
					if(!m_SwissCoord) m_SwissCoord=ReadSwissCoord();
					string stat;
					size_t ipos=pg.status.FileName.find("_day");
					size_t ipos1=pg.status.FileName.rfind('\\');
					size_t length=ipos-ipos1-4;
					stat=pg.status.FileName.substr(ipos1+1,length);
					m_ImportedFileVector[m_countf].StatNo=stat;
					size_t istat=FUtil::AtoInt(stat);
					SetSwissCoord(istat);

					pg.status.FileName=pg.status.FileName.substr(0,ipos1+1);
					pg.status.FileName+="SwissLFI_";
					if(m_CollectedFile) {
						pg.status.FileName+=FUtil::STD_ItoAscii(m_NumToImport);
						pg.status.FileName+="stations";
					}
					else
						pg.status.FileName+=stat;
					pg.status.FileName+=".bin";

				}
				else if(m_format==CZECH) {
					size_t istat=FUtil::AtoInt(m_ImportedFileVector[m_countf].StatNo);
					SetCoord(istat);
				}
				else if (m_format == SILO) {
					for (size_t i = 0; i<pg.Shape.NumVar; i++) {
						pg.Des.addinfo[i + m_countf*pg.Shape.NumVar].Long = m_CurrentFileInfo.dec_cord.x;
						pg.Des.addinfo[i + m_countf*pg.Shape.NumVar].Lat = m_CurrentFileInfo.dec_cord.y;
						pg.Des.addinfo[i + m_countf*pg.Shape.NumVar].StationName = m_CurrentFileInfo.StatName;
						pg.Des.addinfo[i + m_countf*pg.Shape.NumVar].CountryCode = "Australia";
					}
					pg.Des.base.Name[0] = "Global Radiation"; pg.Des.base.Unit[0] = p_ModelInfo->GetUnitString(UNIT_TYPES::HEATFLOW_UNIT);
					pg.Des.base.Name[1] = "Air Temperature"; pg.Des.base.Unit[1] = p_ModelInfo->GetUnitString(UNIT_TYPES::TEMP_UNIT);
					pg.Des.base.Name[2] = "Precipitation"; pg.Des.base.Unit[2] = p_ModelInfo->GetUnitString(UNIT_TYPES::WATERFLOW_UNIT);
					pg.Des.base.Name[3] = "Pan Evaporation"; pg.Des.base.Unit[3] = p_ModelInfo->GetUnitString(UNIT_TYPES::WATERFLOW_UNIT);
					pg.Des.base.Name[4] = "Vapour Pressure"; pg.Des.base.Unit[4] = p_ModelInfo->GetUnitString(UNIT_TYPES::PRESSURE_UNIT);
				}
				else if(m_format==CZECH_COL_TRANS|| m_format ==MONTH_COL_TRANS) {
					for(size_t ii=0; ii<m_ImportedFileVector.size();ii++) {
						size_t istat=FUtil::AtoInt(m_ImportedFileVector[ii].StatNo);
						m_countf=ii;
						SetCoord(istat);
					}
					m_countf=0;
				}
				else if (m_CurrentFileInfo.LinkedSites.size()>0) {
						SetAddInfoFromDefinedMap(m_CurrentFileInfo.LinkedSites[m_countf].Ids);

				}
				else {



				}







	return false;
}

void ExtraLinkedInformation::SetFileDescriptions() {

	SetFileDescriptionFromScannedFile();
	SetFileDescriptionFromLinkedFileDataBase();

}
