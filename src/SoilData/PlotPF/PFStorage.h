#pragma once
#include "../../std.h"
#include "../../Util/ReadWriteTypes.h"


struct NEWHEADER
{	size_t KeyProf;
	std::string OldKey;
	std::string Name;
	std::string County;
	std::string Country;
	float Lat;
	float Long;
	float Upper_Clay;
	float Upper_Sand;
	float Upper_Org;
	float Lower_Clay;
	float Lower_Sand;
	float Lower_Org;
	size_t NumLayers;
	size_t LowDepth;
	size_t Year;
	size_t RecLayers;
	size_t RecHeader;
	std::string CreateDate;
	std::string CreatedBy;
	std::string ModifiedBy;
	std::string ModifiedDate;
};


class PFStorage
{
public:
	PFStorage(void);
	~PFStorage(void);
	bool OpenMainFile(std::string name, bool write=false);
	bool OpenExtraFile(std::string name, bool write=false);
	bool OpenEditMainDataBase();
	bool ReOpenLevelFile();
	bool OpenEditExtraDataBase();
	bool ReadWrite_NewHeaderLine(bool read=true);

	void CloseMainDataBase();
	void CloseExtraDataBase();

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
	NEWHEADER GetProfile(size_t index) {if(m_AllProfiles.size()>index) return m_AllProfiles[index]; 
		else if(m_AllProfiles.size()>0) return m_AllProfiles[0]; 
		else return m_IniHeader;};
	NEWHEADER GetActProfile() {return m_ActProfile;};
	NEWHEADER m_ActProfile;
	
protected:
	std::fstream m_HeaderFile, m_HeaderExtraFile, m_NewHeaderFile;
	std::fstream m_LevelFile, m_LevelExtraFile, m_NewLevelFile;
	std::map<size_t,size_t> m_Records; // Keep pairs with the NewKey and the Record in the Level File
	std::map<size_t,size_t> m_HeaderRec; //// Keep pairs with the NewKey and the Record in the Header File
	size_t m_scancount, m_scansize;
	std::vector<std::streampos> m_HeaderPos;
	static const size_t NSizeHeader=84; // Old size of static header in header File
	static const size_t NSizeLevel=148; // Old and also new size of records in the Level File
	bool IsMainEditOpen, IsExtraEditOpen, IsNewFormatFile;
	
	std::vector<NEWHEADER> m_AllProfiles;
	NEWHEADER m_IniHeader;
	
	ReadWriteTypes m_RwNewHeader;
	size_t m_PF_FileVersion;

private:
	std::string m_MainStoreName, m_ExtraStoreName, m_DataBaseDir;
	

};

