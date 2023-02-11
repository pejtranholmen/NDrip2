#pragma once
#include "./PGFileImportBase.h"


class PGFileImport :
	public PGFileImportBase
{
public:
	bool	ImportFromFile(std::string filename, bool AddFromFile=false, size_t iFormat=0);
	PGFileImport(size_t NumFiles=0);

	~PGFileImport(void);
	FILEINFO ScanningInputFile(std::string filename="");

	FILEINFO CheckFixedNOOAFormat(FILEINFO);
	FILEINFO MakeScanning(FILEINFO);
	FILEINFO CheckHeadingAndFileInformation(FILEINFO);

	std::vector<std::string> CreatePGFileFromDefinedInputFiles();
	std::vector<std::string> CreatePGFileFromIButtonFiles();


	void SetFilterTestColumn(size_t column) {m_FilterTestColumn = column; };
	void SetFilterToUseId(std::string Id) { m_FilterToUseId = Id; };
	void ApplyHeaderLineAsNamesInPGFile(bool UnitsInstead = false, bool Clean_Instead=false);

protected:
	FILEINFO GetFileInfo(FILEINFO input, size_t i);
	private:
		std::map<std::string, size_t> m_FilterMap;
		std::vector<size_t> m_FilterCount;
		size_t m_FilterTestColumn;
		std::string m_FilterToUseId;

};

