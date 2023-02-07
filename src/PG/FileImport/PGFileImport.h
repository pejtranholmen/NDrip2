#pragma once
#include "./PGFileImportBase.h"


class PGFileImport :
	public PGFileImportBase
{
public:
	bool	ImportFromFile(string filename, bool AddFromFile=false, size_t iFormat=0);
	PGFileImport(size_t NumFiles=0);

	~PGFileImport(void);
	FILEINFO ScanningInputFile(string filename="");

	FILEINFO CheckFixedNOOAFormat(FILEINFO);
	FILEINFO MakeScanning(FILEINFO);
	FILEINFO CheckHeadingAndFileInformation(FILEINFO);

	vector<string> CreatePGFileFromDefinedInputFiles();
	vector<string> CreatePGFileFromIButtonFiles();


	void SetFilterTestColumn(size_t column) {m_FilterTestColumn = column; };
	void SetFilterToUseId(string Id) { m_FilterToUseId = Id; };
	void ApplyHeaderLineAsNamesInPGFile(bool UnitsInstead = false, bool Clean_Instead=false);

protected:
	FILEINFO GetFileInfo(FILEINFO input, size_t i);
	private:
		map<string, size_t> m_FilterMap;
		vector<size_t> m_FilterCount;
		size_t m_FilterTestColumn;
		string m_FilterToUseId;

};

