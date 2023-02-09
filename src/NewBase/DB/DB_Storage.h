#pragma once
#include <vector>
#include "../../Util/FUtil.hpp"
class Doc;
struct DB_OldHeader
{
	size_t Type;
	string Name;
	string SiteName;
	string Latitude;
	string Longitude;
	string Country;
	string DriveFile;
	string Details;
	string IdUser;
	size_t IdNum;
	string keyId;
};
struct DB_NewHeader
{
	string ModelVersion;
	string CreateDate;
	string ModifiedDate;
	string CreatedBy;
	string ModifiedBy;
	bool CompareGroup;

};
class DB_Storage
{
public:
	DB_Storage(void);
	~DB_Storage(void);
	Doc *m_pSimDoc;
	size_t GetNumGroups(size_t db_index) {return m_Group[db_index].size();};

	string GetGroupString(size_t db_index, size_t index);
	size_t GetCurrentDBIndex(bool AsIs=false) { if (m_DB_Index > 0&&!AsIs) return m_DB_Index + 1; else return m_DB_Index; };
	void SetCurrentDBIndex(size_t index) { m_DB_Index=index;};
	size_t GetCurrentKeyIndex(size_t db_index) {return m_DB_KeyIndex[db_index];};
	size_t GetCurrentKeyIndex() {return  m_DB_KeyIndex[m_DB_Index];};
	void SetCurrentKeyIndex(size_t index) {m_DB_KeyIndex[m_DB_Index]=index;};
	void SetCurrentKeyLink(size_t db_index, size_t keyindex) { m_DB_Index = db_index; m_DB_KeyIndex[m_DB_Index] = keyindex;
		SetCompareLinkNewHeader(db_index, keyindex, true);
	; };
	size_t GetNumOriginal(size_t db_index) {return m_NumOriginalRec[db_index];};
	vector<string> m_PlantType[6];
	size_t m_PlantTypeIndex[9];
	size_t GetPlantTypeIndex(size_t db_index, string name);
	bool SetCompareLinkNewHeader(size_t db_index, size_t keyindex, bool Compare) {
		if (db_index<9 && db_index >= 0) {
			if (keyindex<m_NewHeader[db_index].size()) {
				m_NewHeader[db_index][keyindex].CompareGroup = Compare;
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}
	vector<size_t> GetCompareLinkVector(size_t db_index) {
		vector<size_t> IndexToCompare;
		if (db_index<9 && db_index >= 0) {
			for (size_t index = 0; index < m_NewHeader[db_index].size(); index++) {
				if(m_NewHeader[db_index][index].CompareGroup) IndexToCompare.push_back(index);
			}
		}
		return IndexToCompare;
	};


	bool SetHeader(size_t db_index, size_t keyindex, DB_OldHeader newheader) {
		if(db_index<9&&db_index>=0) {
			if(keyindex<m_OldHeader[db_index].size()) {
				m_OldHeader[db_index][keyindex]=newheader;
				return true;
			}
			else
				return false;
		}
		else
			return false;

	};
	bool SetNewHeader(size_t db_index, size_t keyindex, DB_NewHeader newheader) {
		if (db_index<9 && db_index >= 0) {
			if (keyindex<m_NewHeader[db_index].size()) {
				m_NewHeader[db_index][keyindex] = newheader;
				return true;
			}
			else
				return false;
		}
		else
			return false;

	};
	DB_OldHeader GetHeader(size_t db_index, size_t keyindex) {
		if(db_index<9&&m_OldHeader[db_index].size()>keyindex)
			return m_OldHeader[db_index][keyindex];
		else {
			DB_OldHeader noheader;
			noheader.IdUser="";
			noheader.IdNum=0;
			return noheader;
		}

	}
	bool IsDBModified(size_t db_index) { return m_EditHeaders[db_index];};
	void SetSavedDB(size_t db_index) { m_EditHeaders[db_index]=false;};
	void SetDBModified(size_t db_index) { m_EditHeaders[db_index]=true;};


protected:
	bool m_IniDataBaseMade;
	bool m_EditHeaders[9];
	vector<DB_OldHeader> m_OldHeader[9];
	vector<DB_NewHeader> m_NewHeader[9];
	
	vector<size_t> m_Group[9];
	int m_NumOriginalRec[9]; 
	bool m_ReadInDone[9];
	map<string, DB_NewHeader> m_ValidNewHeader[9];
	
	fstream m_HeaderFile;
	size_t m_DB_Index;
private:
	
	size_t m_DB_KeyIndex[9];

};

