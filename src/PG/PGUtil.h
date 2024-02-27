#pragma once
#include "./PGBase.h"

#include <map>
#include <string>
#ifndef BASE
#define BASE   1721424L  
#endif
//* Jan 1, AD 1 
enum formats { STANDARD, SEPARATED_YMDHM, SEPARATED_YMD, YMD_HM, NOAA, SMHI, SWISS, SEPARATED_YMDH, CZECH, CZECH_COL_TRANS, MONTH_COL_TRANS, PROFOUND, IDSCAN, UNDEFINEDFORMAT, SMHI_OPENDATA, SILO, SITES, PERRENIAL };
enum dateformats { YMDHM, YMD, DMY_SLASH };
class GlobalInfo;
class Doc;
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
	std::vector<std::string> FilterItems;
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
	std::string InputfileName;
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
	std::vector<bool> ValidVariables;
	std::vector<size_t> ValidVariableIndex;
	std::vector<std::string> FirstValidHeads;
	DESCRIPTIONS Des;
	std::vector<std::string> UnitsFromHead;
	std::vector<std::string> IdFromHead;
	std::vector<std::string> PosFromHead;
	std::vector<LINKED_FILEINFO> LinkedSites;
	bool EqualSizeOfSiteRecords;
	size_t IdentifiedDelimiter;
	size_t IdentifiedLineEnd;
	size_t IdentifiedStartMin;
	size_t IdentifiedMinDiff;
	formats IdentifiedFormat;
	dateformats IdentifiedDateFormat;
	std::string StatNo;
	std::string StatName;
	DEC_COORD dec_cord;
	size_t FilterColumnToUse;
	std::vector<std::string> GlobalFilterItems;
	std::string FilterItemToUse;
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


class PGUtil
{
public:
	PGUtil(void);
	virtual ~PGUtil(void);
	UNIT_TYPES GetUnitType(std::string key);
	static float AtoFloat(std::string str);
	static double AtoDouble(std::string str);
	static std::string DateConv(enum dateformats, std::string instr);
	static std::string ItoNumAscii(int);
	static std::string	STD_FtoAscii(float v) ;
	static std::string STD_FtoAsciiPane(float v);
	static std::string	STD_DtoAscii(double v) ;
	static std::string	STD_ItoAscii(int v) ;
	static std::string	STD_ItoAsciiHex(int v);
	static int	AtoInt(std::string str);
	static unsigned int cminutx(int, int,int, int,int);
	static IPGTID Ato_IPGTID(std::string str);
	static IPGTID MinToPGTID(unsigned int min);
	static unsigned int MinutConv(IPGTID);
	static unsigned int MinutConv(std::string str, bool daily=false, bool hourly=false);  
	//static std::string StringDatum(unsigned int min) { return StringDatum(unsigned int(min)); };
	static std::string StringDatum(unsigned int);
	static std::string StringDatumSec(double);
	static std::string StringDatumStart(unsigned int, unsigned int range);
	static std::string StringDatumStartEstimate(unsigned int, unsigned int end);
	static std::string StringDatumEnd(unsigned int, unsigned int start, int step=0);
	static std::string STD_StringDatum(unsigned int) ;
	static std::string STD_StringDatumStart(unsigned int min, unsigned int range) ;
	static std::string STD_StringDatumStartEstimate(unsigned int min, unsigned int end);
	static std::string STD_StringDatumEnd(unsigned int min, unsigned int start, int step=0);
	static size_t YearFunction(unsigned int);
	static double DayNumFunction(unsigned int);
	static int YearNumFunction(unsigned int);
	static int MonthFunction(unsigned int);
	static int DayFunction(unsigned int);
	static int HourFunction(unsigned int);
	static int MinOfHourFunction(unsigned int);
	static unsigned int cminut(char *date);
	static std::string cdatum(unsigned int &julian);
	static int LengthOfMonth(unsigned int min);
	static std::string CheckedForExistingBinFileName(std::string filename);
	static bool CheckForPossiblePGDateString(std::string str);
	static bool CreateOutputCSVFile(std::string filename, std::string filenameinput, Doc* pDoc);
	static std::string createInputBinFile(std::string csvFileName, size_t filetype=0);

private:
	void SetUnitMap(UNIT_TYPES type, std::string str) {
		m_TypeBasedUnits.insert(std::pair<UNIT_TYPES, std::string>(type, str));
		m_StringBasedUnits.insert(std::pair<std::string, UNIT_TYPES>(str, type));
	};
	std::map<std::string, UNIT_TYPES> m_StringBasedUnits;
	std::map<UNIT_TYPES, std::string> m_TypeBasedUnits;
};

