#pragma once

//


class CCoupModelDoc;
class Link {
	
public:
	Link();
	~Link();
#ifndef COUPSTD
	void ReportReady(CCoupModelDoc *pDoc=nullptr);
	void ReportUpdate(CCoupModelDoc *pDoc=nullptr);

	void WriteExport();
	bool WriteFile(string);
	int ReadFile(string);
	int ReadImport();
	bool WriteCSV_File(string str="");

	void ReadExcel();

	string GetExcelDriver();
	string GetExcelFile();

#ifndef WIN64
	bool SQL_CREATE();
	bool SQL_UPDATEVERSION();
	bool SQL_WRITE();
	string GetMySQLDriver();
	bool SQL_Call(string Query, string DefError);
	bool SQL_Check();
    int SQL_RES();
    int SQL_RES2();
	int SQL_GetNumberofRows();
	int SQL_GetNumberofRows2();
	int SQL_GetNumberofFields();
	string SQL_GetRow(int i);
	string SQL_GetItem(int irow, int icol);
	string SQL_GetItem2(int irow, int icol);
#endif

	NewMap *m_Map;

protected:
	string m_ExcelSumFile;
	int m_readflag;
	vector<string> m_SQL_Lines;
	vector<string> m_SQL_Lines2;
	int m_SQL_fields1;
	int m_SQL_fields2;
#endif
	
};