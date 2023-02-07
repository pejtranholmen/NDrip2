#pragma once
class CCoupModelDoc;
class CCoupModelDoc;
class CPG;
class PGFile {public : CPG *pFile; bool Visible; CCoupModelDoc *pDoc;};
class PGFileArchive
{
public:
	PGFileArchive();
	virtual ~PGFileArchive();
	std::vector <PGFile> m_PGFileArray;
	size_t GetNumPGFile() {return m_PGFileArray.size();};
	void AddPGFile(PGFile fil) {m_PGFileArray.push_back(fil);};
	void ErasePGFile(size_t i) {m_PGFileArray.erase(m_PGFileArray.begin()+i);};
	bool IsVisible(size_t index) {if(m_PGFileArray.size()>index) return m_PGFileArray[index].Visible; else return false;};
	void SetInVisible(size_t index) {if(m_PGFileArray.size()>index) m_PGFileArray[index].Visible=false; };
	void SetInVisible() {for(size_t i=0; i<m_PGFileArray.size(); i++) m_PGFileArray[i].Visible=false; };
	void SetVisible(size_t index) {if(m_PGFileArray.size()>index) m_PGFileArray[index].Visible=true; };
	size_t GetIndex(CCoupModelDoc *pDoc) {for(size_t i=0; i<m_PGFileArray.size(); i++)  if(m_PGFileArray[i].pDoc==pDoc) return i; return string::npos;}; 
	CPG *GetPGFile(size_t i=0);
	CCoupModelDoc *GetDocPointer(size_t i=0);
};