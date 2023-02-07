#pragma once
#include "../std.h"
#include "../ModelTypes/SimB.h"
#ifndef DistStat
struct DistStat;

#endif


struct InPGFiles
{
	string StatNo;
	size_t NumRec;
	size_t RecStart;
	size_t MinSt;
	size_t MinEnd;
	size_t NumRecCont;
};
struct AddInfo
{
	size_t   Key;
	double Lat;
	double Long;
	double Alt;
	string CountryCode;
	string StationName;
	size_t	num_missing;
	size_t	num_substituted;
	elements Element;
	UNIT_TYPES Unit;
};
struct DATASHAPE {
	unsigned int RecordSize = 0;
	unsigned int RecordIndexSize = 0;
	unsigned int RecordSize_Repetion = 0;
	unsigned int RecordTotalSizeData = 0;

	unsigned int OldRecordSize = 0;
	unsigned int OldRecordIndexSize = 0;

	unsigned int TotalNumberOfRecords = 0;

	unsigned int NumRepetitions = 0;
	unsigned int OldNumRepetitions = 0;

	unsigned int NumRecords = 0;
	unsigned int OldNumRecords = 0;

	unsigned int NumVar = 0;
	unsigned int OldNumVar = 0;

	bool RepetitionMode;
};

struct DATACONTENT {
	vector<float> v;
	vector<float> vv;
};
struct BaseInfo {

	vector<string> Name, Unit, Id, Pos;
	std::vector <float>  var_min, var_max;

};
struct DATATYPES {
	enum datatype type;
	enum elements elem;
	enum fysprocess fproc;
	enum bioprocess bproc;
};
struct DESCRIPTIONS {
	BaseInfo base;
	vector<AddInfo> addinfo;
	vector<DATATYPES> data;
	AddInfo defaultAddInfo;
	std::vector <size_t> var_missing;
	std::vector <size_t> JumpBack;
};

struct PGACTIONS {
	vector<size_t> ToBeSaved;
	vector<size_t> ToBeCalc;
	vector<size_t> ToBeViewed;
	size_t OutRecordStart, OutRecordEnd;
	size_t LocalStartRecord;
	size_t LocalStartIndex;

	
};
struct PGFILE_BUF {
	size_t buf_Size = 0;
	size_t buf_TotSize = 0;
	size_t CurrentBuf = 0;
	size_t NumRecordsInBuffer = 0;
	size_t PrevBuf = 0;
	size_t NumberOfBuf = 0;
	size_t buf_OrgSize = 0;
	bool   CurrentBufferIsRead;
};
struct PGFILE_STATUS {
	PGFILE_BUF b;
	PGACTIONS a;
	unsigned int MinutStart;
	unsigned int MinStart;
	unsigned int MinEnd;
	unsigned int NormalTimeInterval;
	bool CompleteFileRead;
	bool CompleteRepRead;
	bool OnlyNoneMissing;
	bool FileModified;
	bool AllInMemory;
	bool RecordsToView;
	size_t FileVersion;
	string FileName;
	string NewFileName;
	unsigned long long SizeOfFileInMainStream;
};
struct ALLPGSTRUCT {
	DATASHAPE Shape;
	DATACONTENT Var;
	DESCRIPTIONS Des;
	PGFILE_STATUS status;
};

class CPGBase
{
public:
	CPGBase(void);
	virtual ~CPGBase(void);
	bool IsFileModified() { return pg.status.FileModified; };
	void SetNewReadofFile() { pg.status.FileModified = false; };
	void ResetBuf() {pg.status.b.buf_TotSize= pg.status.b.buf_OrgSize;};
	ALLPGSTRUCT pg;
    ALLPGSTRUCT& GetRef() { ALLPGSTRUCT& rr = pg; return rr;};
protected:
	

	DATASHAPE SetDataShape(size_t irec, size_t ivar, size_t irep);
};

