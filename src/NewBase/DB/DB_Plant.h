
#pragma once


// DB_Plant.h : header file
//
class Sw;
/////////////////////////////////////////////////////////////////////////////
// DB_Plant

class DB_Plant 
{


// Construction
public:
	DB_Plant(){};
	DB_Plant(string pszCaption, bool, void *pDoc) ;
	DB_Plant(string pszCaption, void *pDoc, string key="");

// Attributes
public:

// Operations
public:



// Implementation
public:
	void Init(string pszCaption, bool, void *pDoc);
	void Init(string pszCaption,   string key="");
	virtual ~DB_Plant();
	void Actual_DB();
	void DB_Actual();
	bool DB_Struc_Actual(string Name);
	bool ReadWrite(string filename, bool writing=true);
	bool m_ReadOnly;

	bool m_DuringRun;


protected:

	void *m_pDoc;
	string m_Group; 
	string mm_name;
	string m_DB_key;
	Sw *m_pSwTimeRes;
	Sw *m_pSwPlantType;

};


