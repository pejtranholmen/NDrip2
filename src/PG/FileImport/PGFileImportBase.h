#pragma once
#include "../PGFile.h"
#include "../PGUtil.h"
#include "./ExtraLinkedInformation.h"





class PGFileImportBase :
	public ExtraLinkedInformation
{
public:
	PGFileImportBase();
	~PGFileImportBase();

	void SetMessage(bool valid=false);
	bool OpenInputFile(string filename) {
		m_filename = filename;
		m_FileText.open(filename, ios::in | ios::binary);
		if (m_FileText.is_open()) return true;
		else return false;
	};
	void SetCollectedFile(bool valid=false);

	void UpdateNumberOfValidVariables() { m_CurrentFileInfo.RedNumVar = 0;
	for (size_t i = 0; i < m_CurrentFileInfo.ValidVariables.size(); i++) 
		if (!m_CurrentFileInfo.ValidVariables[i]) {
			m_CurrentFileInfo.RedNumVar++;
		}

		m_CurrentFileInfo.RedNumVar*= m_CurrentFileInfo.LinkedSites.back().TimeCheck.RecordStep;
	};
	void SetStartMinuteWhenImport(size_t min) { m_itimbase = min; m_minst_a = min; };

	vector<string> m_SuccessFullImportedFileNames;




	bool m_AddFromFile;
	bool m_ScanningMade;
	map<size_t, size_t> m_CheckedTimeMap;





protected:
	size_t m_MinStartCol;
	bool ConvColLine(vector<float> *vv, string line, size_t option);
	bool ConvLine(vector<float> *vv, string line, size_t option);


	bool CreateNewFileAndDefineSize(FILEINFO, size_t ActualId = 0);
	bool ReadHeading(FILEINFO, size_t ActualId = 0);
	bool SettingStartPositioninInputFile();
	bool SettingCheckingStartPositioninOutputFile();
	void InitForSettingMinMaxMissingValues();
	bool ReadingVariableValues(FILEINFO, size_t ActualId=0);
	size_t GetReadBufferSize(FILEINFO);
	bool WriteMissingRegularTime();
	void AdjustValuesToNewUnits();



	size_t m_nprevrec;
	string m_linebreak;
	string m_datestring;


protected:

	bool SetTimeVariable(size_t nrec, string varstr, size_t nvar, bool shift=false);
	bool SetDefinedTime(size_t nrec);
	void SetPGTimeVar_Minut_To_PG_Var();
	void SetValidVariable(size_t index, string varstr);
	void SetMissingVariable(size_t index);



	size_t GetMinutFromLine(string line);
	void WriteCompleteRecord();


	
	size_t GetStartMinute();
	fstream m_FileText;


	
	size_t m_newstartpos;
	size_t m_totsizeInputFile;
	bool m_CreateMultiFile;
	size_t m_nrec;
	size_t m_min_next;
	bool m_HeadingRead;
	size_t m_time_red;
	std::streampos m_actpos;
	size_t m_itimbase;
	size_t m_ndelim;

	size_t m_ProFoundOption;
	size_t m_CurrentMin, m_PreviousMin, m_CurrentMinStep;

	std::vector <float> m_vmin, m_vmax;
	size_t m_minst_a, m_minend_a, m_totrec_a;
	string m_delimiters;
	

	bool m_ShowMessage;
private: 
	IPGTID Currentlinetime;






};

