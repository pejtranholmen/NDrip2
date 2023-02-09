#pragma once
#include <vector>
#include <map>
#include <string>
#include <fstream>

//#include "../Util/ReadWriteTypes.h"

struct SOILHEADER
{
	size_t KeyProf;
	size_t NumLayers;
	size_t RecordPos;
	double Lat;
	double Long;
	std::string Name;
	std::string Type;
	size_t NumProf;

};

struct CSVFILEINFO
{
	size_t NumLines;
	size_t NumVar;
	std::streamsize SizeOfFile;
	std::string EndLine;
	std::string Delim;
};


class SoilStorage
{
public:
	SoilStorage(void);
	~SoilStorage(void);
	CSVFILEINFO ScanningInputFile();
	std::vector<std::string> ReadLine();

	bool OpenMainFile(std::string name, bool write=false);
	bool OpenExtraFile(std::string name, bool write=false);
	bool OpenEditMainDataBase();
	bool ReOpenLevelFile();
	bool OpenEditExtraDataBase();
	void CloseMainDataBase();
	void CloseExtraDataBase();
	bool WriteLevelFile(std::vector<std::string> cols);
	bool WriteHeaderFile(std::vector<std::string> cols, size_t LevelPos);

	bool ReadNewHeaderFile();


	bool IsMainWriteEnabled() {return IsMainEditOpen;};
	bool IsExtraWriteEnabled() {return IsMainEditOpen;};

	bool IsOpen() {return IsMainEditOpen;};
	bool NewFormatFile() {return IsNewFormatFile;};
	std::string GetDataBaseDirectory() {return m_DataBaseDir;};
	void SetDataBaseDirectory(std::string dir) {m_DataBaseDir=dir;};
	std::string GetMainDataBaseName(bool OnlyTitle) {
		if(OnlyTitle) {
			size_t pos=m_MainStoreName.rfind("\\");
			if(pos!=std::string::npos)
				return ".."+m_MainStoreName.substr(pos);
			else
				return m_MainStoreName;
		}
		else
			return m_MainStoreName;};
	std::string GetExtraDataBaseName(bool OnlyTitle) {
		if(OnlyTitle) {
			size_t pos=m_ExtraStoreName.rfind("\\");
			if(pos!=std::string::npos)
				return ".."+m_ExtraStoreName.substr(pos);
			else
				return m_ExtraStoreName;
		}
		else		
			return m_ExtraStoreName;};
	size_t GetNumProfiles() {return m_HeaderRec.size();};
	SOILHEADER GetProfile(size_t index) {if(m_AllProfiles.size()>index) return m_AllProfiles[index]; 
		else if(m_AllProfiles.size()>0) return m_AllProfiles[0]; 
		else return m_IniHeader;};
	SOILHEADER m_ActProfile;
	bool IniNewDataBase() {
		
		//return m_Rw.OpenDocFileIn(m_MainStoreName+".dba");	
		return false;
		
	};
	
protected:
	std::fstream m_HeaderFile, m_HeaderExtraFile, m_NewHeaderFile;
	std::fstream m_LevelFile, m_LevelExtraFile, m_NewLevelFile;
	std::map<size_t,size_t> m_Records; // Keep pairs with the NewKey and the Record in the Level File
	std::map<size_t,size_t> m_HeaderRec; //// Keep pairs with the NewKey and the Record in the Header File
	size_t m_scancount, m_scansize;
	std::vector<size_t> m_HeaderPos;
	bool IsMainEditOpen, IsExtraEditOpen, IsNewFormatFile;
	
	std::vector<SOILHEADER> m_AllProfiles;
	SOILHEADER m_IniHeader;
	CSVFILEINFO m_FileInfo;
	size_t m_MaxCharLine;
	//ReadWriteTypes m_Rw;
	size_t m_NumProfile;
	std::vector<float> m_LevelValues;


	std::string m_MainStoreName, m_ExtraStoreName, m_DataBaseDir;
	

};

