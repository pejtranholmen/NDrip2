#pragma once
#include "../PGFile.h"
class ExtraLinkedInformation: public CPGFile
{
public:
	ExtraLinkedInformation();
	~ExtraLinkedInformation();


	bool DefineAddInfoMap();
	bool SetAddInfoFromDefinedMap(size_t Key);
	
	bool ReadSwissCoord();
	bool ReadCoord();
	void SetSwissCoord(size_t stat);
	void SetCoord(size_t stat);	


	void SetFileDescriptions();
	bool SetFileDescriptionFromScannedFile();

	bool SetFileDescriptionFromLinkedFileDataBase();

	std::vector<size_t> AddInfoLinkIndex;
	bool m_ExtraFileScanning;
	bool m_CollectedFile;
	InPGFiles m_CurrentFile;
	size_t m_countf;
	std::string m_ExtraInfoFileName;
	std::vector <InPGFiles> m_ImportedFileVector;
	FILEINFO m_CurrentFileInfo, m_ExtraFileInfo;
	std::vector<FILEINFO> AllFileInfos;
	size_t m_RecordsExtraInfoFile;
	size_t m_NumToImport;
	std::vector<int> m_NewVarIndexSelected;
	enum formats m_format;
	std::string m_filename;

protected:	
	bool m_SwissCoord;
	bool m_CoordMapExist;
	std::map<size_t,SWISSCORD> m_SwissMap;
	std::map<size_t,DEC_COORD> m_CoordMap;
	std::map<size_t, AddInfo> m_AddInfoMap;

};

