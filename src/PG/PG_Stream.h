#pragma once
#include "./PG_MEMORY.h"
class PG_Stream : public PG_Memory
{
public:
	PG_Stream(void);
	~PG_Stream(void);


bool	WritePGFileStartHeader(size_t nrecords, size_t nrepititions = 1, size_t InputRepNo = -1, std::fstream *pOutStream=nullptr);
bool    IsGood();
bool	ClearFile();
bool	IsOpen() const;	
bool	MakeFileToEnableWriting(bool CreateCopy=false);
bool    MakeBackUpCopy();
bool	ReturnToBackUpCopy(bool Yes=false);
bool	CreateNewFile(size_t numVar, size_t numRec, size_t repit=1);
void	CloseFile();
bool	ReOpen(bool writefile=false);
bool    Open(std::string filename, bool ShowError=true);
bool    Save(std::string filename);
bool    SaveAs(std::string filename, bool newfile = false);

void	SetNewFileName(std::string filename, bool RemoveOld = false);
void	SetFileName(std::string filename);

bool    WriteNewPGFileAs(std::string filename, bool newfile = false);
bool	WritePGFileFrame(size_t InputRepNo = std::string::npos, std::fstream *pPGOutStream=nullptr);
bool	WriteSeparateSplitFiles();
bool	WriteMinMax(size_t varno, float fmin, float fmax, size_t repit = 1);
bool	WritePGFileEnd(size_t nrecords, size_t nrepititions = 1, size_t InputRepNo = -1, bool newdrive = false);
bool	WritePGFileHeader(size_t nrecords, size_t nrepititions = 1);
bool	WritePGFileData(size_t nrecords, std::fstream *pPGOutStream = nullptr);
bool	WritePGFile();
bool	MakeNewPGFileName();
bool	GetAllFileInfo(std::string filename, bool ShowError = true);
bool    ReadContentOfPGFileAndOpenMainPGStreamForReading(std::string filename);

bool	CheckIfCurrentRecordFitsCurrentBuffer(size_t Record);
bool	ReadNewBuffer(size_t Buffer);

bool	WritePGFileRecord(size_t irec, size_t repit = 1);
bool	WritePGFileVarValue(size_t varno, size_t irec, float v, size_t repit = 1);
std::streamsize	WriteNewPGFileRecord(size_t rec = std::string::npos);

std::string  GetDateFromVarRecord(size_t Record);
bool   SetMinToVarRecord(size_t Record, size_t Min);

bool	ReadFirst_Buffer();
bool	ReadNewBuffer(size_t CurrentRec, size_t CurrentBuf);
bool	WriteNewBuffer(size_t CurrentBuf);
bool	WriteNewDriveNewBuffer(size_t CurrentBuf);

bool	SetActBufferReset();
bool	SetActBuffer(size_t abuffer);
bool	SetActBufferSize(size_t abuffer);

	bool	SetRepititionSequence(size_t, bool resetindex = true);
	size_t	GetRepititionSequence();
	void	SetRepititionMode(bool, size_t repititions = 1);
	size_t	CheckAndEstimateRepetions();
	bool WriteDescriptionsToStream(std::fstream *pWriteEnabledStream=nullptr, size_t RepNoInput_ToStore= std::string::npos);

bool	m_IsOpen;
bool	m_Closed;
bool	m_WriteEnabled;
std::fstream m_MainPGStreamReadWrite, m_ExtraPGStreamWrite;

protected:

	std::vector<float> ReadVarVector(size_t varno);
	bool AdjustDataShapeAndWriteNewRecords();
	size_t GetLongTime(size_t recordno);

	std::fstream GetLocalReadOnlyStream(std::string filename);
	bool ReadPGStartHeaderFromStream(std::fstream *driveFile);

	bool ReadVariableValuesFromStream(std::fstream *driveFile);
	bool ReadDescriptionsFromStream(std::fstream *driveFile);

	size_t  m_ActualSequence;
	bool m_ResetBuffer;

	std::vector<size_t> m_ActRecord, m_ActHit;
	std::vector<float> m_FirstValid, m_LastValid;
	
	std::ofstream m_OutPutStream;
	size_t m_numrecords_Minutes, m_numrecords_Hours, m_numrecords_Days, m_numrecords_Months, m_numrecords_Years;
	size_t m_DataInFocus;
	std::string m_FailedFileName;


	bool m_OutFileStatus;
	std::string m_OutFileStart, m_OutFileEnd;

	size_t m_OutFileRecord, m_OutFileVar;
	bool m_SectionYear;
	size_t	m_startRec;
	size_t	m_time1900, m_time1900_Zero;

private:
	bool OpenMainPGStreamAsReadOnly(std::string filename);
	bool m_OnlyNewFile;
	bool m_UseMainStream;
	bool Check_PGFileSize(std::fstream *f, size_t numrec, size_t ivar, size_t nrep);
	bool m_ShowError;
	std::string m_BackUpFileName;
	bool   m_IsBackUpCreated;
};

