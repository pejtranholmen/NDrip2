#include "PGCalc.h"


PGCalc::PGCalc(void)
{
}


PGCalc::~PGCalc(void)
{
}
vector<size_t> PGCalc::GetYearStartIndex()
{
	vector<size_t> out;

	size_t min=GetMinuteStartThisYear(1);
	size_t rec=GetRecord(min, false);
	while(min>0&&rec!=string::npos) {	
		out.push_back(rec);
		min=GetMinuteStartNextYear(rec);
		rec=GetRecord(min, false);
	}
	return out;
}
vector<size_t> PGCalc::GetDayStartIndex()
{
	vector<size_t> out;
	long min=GetMinuteStartThisYear(1);
	long rec=GetRecord(min,false);
	if(IsEquiDistance()) {
		long frequence=1440/m_EquiDistance;
		while(min>0&&rec>0&&rec<pg.Shape.NumRecords) {	
			out.push_back(rec);
			//min=GetMinuteStartNextDay(rec);
			rec+=frequence;
		}
	}
	else {
		while(min>0&&rec!=string::npos) {	
			out.push_back(rec);
			min=GetMinuteStartNextDay(rec);
			rec=GetRecord(min,false);
		}

	}
	return out;
}