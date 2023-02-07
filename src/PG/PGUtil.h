#pragma once
#include "./PGBase.h"
#ifndef BASE
#define BASE   1721424L  
#endif
//* Jan 1, AD 1 
enum formats { STANDARD, SEPARATED_YMDHM, SEPARATED_YMD, YMD_HM, NOAA, SMHI, SWISS, SEPARATED_YMDH, CZECH, CZECH_COL_TRANS, MONTH_COL_TRANS, PROFOUND, IDSCAN, UNDEFINEDFORMAT, SMHI_OPENDATA, SILO, SITES };
enum dateformats { YMDHM, YMD, DMY_SLASH };

class GlobalInfo;
struct TIMECHECK {
	bool CommonTimeStep;
	bool CommonRecStep;
	size_t ValueStart;
	size_t ValueEnd;
	size_t ValueStep;
	size_t RecordStep;
	size_t NumUniqueSteps;
};

struct LINKED_FILEINFO {
	size_t Records;
	size_t RecordsSelected;
	size_t Ids;
	vector<string> FilterItems;
	TIMECHECK TimeCheck;
};
struct DEC_COORD
{
	double x;
	double y;
	double z;
};


struct FILEINFO
{	
	string InputfileName;
	bool CopyMode;
	size_t MaxVar;
	size_t NumHeads;
	bool FixedFormat;
	size_t SkipBeforeVar;
	size_t SkipAfterVar;
	size_t TotRec;
	size_t RedNumVar;
	size_t DateColumn;
	size_t YearColumn;
	size_t MonthColumn;
	size_t DayNumberColumn;
	size_t DayColumn;
	size_t TimeColumn;
	size_t HourColumn;
	size_t MinuteColumn;
	vector<bool> ValidVariables;
	vector<size_t> ValidVariableIndex;
	vector<string> FirstValidHeads;
	DESCRIPTIONS Des;
	vector<string> UnitsFromHead;
	vector<string> IdFromHead;
	vector<string> PosFromHead;
	vector<LINKED_FILEINFO> LinkedSites;
	bool EqualSizeOfSiteRecords;
	size_t IdentifiedDelimiter;
	size_t IdentifiedLineEnd;
	size_t IdentifiedStartMin;
	size_t IdentifiedMinDiff;
	formats IdentifiedFormat;
	dateformats IdentifiedDateFormat;
	string StatNo;
	string StatName;
	DEC_COORD dec_cord;
	size_t FilterColumnToUse;
	vector<string> GlobalFilterItems;
	string FilterItemToUse;
};

struct SWISSCORD
{
	size_t x;
	size_t y;
	size_t z;
};



struct IPGTID {
	int iyear;
	int imonth;
	int iday;
	int ihour;
	int iminut;
};



#include "../std.h"
class PGUtil
{
public:
	PGUtil(void);
	virtual ~PGUtil(void);
	
	static float AtoFloat(string str);
	static string DateConv(enum dateformats, string instr);
	static string ItoNumAscii(int);
	static string	STD_FtoAscii(float v) ;
	static string STD_FtoAsciiPane(float v);
	static string	STD_DtoAscii(double v) ;
	static string	STD_ItoAscii(int v) ;
	static string	STD_ItoAsciiHex(int v);
	static int	AtoInt(string str);
	static unsigned int cminutx(int, int,int, int,int);
	static IPGTID Ato_IPGTID(string str);
	static IPGTID MinToPGTID(unsigned int min);
	static unsigned int MinutConv(IPGTID);
	static unsigned int MinutConv(string str, bool daily=false, bool hourly=false);  
	//static string StringDatum(unsigned int min) { return StringDatum(unsigned int(min)); };
	static string StringDatum(unsigned int);
	static string StringDatumSec(double);
	static string StringDatumStart(unsigned int, unsigned int range);
	static string StringDatumStartEstimate(unsigned int, unsigned int end);
	static string StringDatumEnd(unsigned int, unsigned int start, int step=0);
	static string STD_StringDatum(unsigned int) ;
	static string STD_StringDatumStart(unsigned int min, unsigned int range) ;
	static string STD_StringDatumStartEstimate(unsigned int min, unsigned int end);
	static string STD_StringDatumEnd(unsigned int min, unsigned int start, int step=0);
	static size_t YearFunction(unsigned int);
	static double DayNumFunction(unsigned int);
	static int YearNumFunction(unsigned int);
	static int MonthFunction(unsigned int);
	static int DayFunction(unsigned int);
	static int HourFunction(unsigned int);
	static int MinOfHourFunction(unsigned int);
	static unsigned int cminut(char *date);
	static string cdatum(unsigned int &julian);
	static int LengthOfMonth(unsigned int min);
	static string CheckedForExistingBinFileName(string filename);
	static bool CheckForPossiblePGDateString(string str);

};

