#pragma once
#include "./PGBase.h"





class PG_Memory :
	public CPGBase
{
public:
	PG_Memory();
	~PG_Memory();
	void	SetInfoMode();

	bool	AdjustSize(size_t irec, size_t varnumber, size_t repit = 1);

	bool ReAllocateVarVector();
	bool ReAllocateDescription();
	size_t GetVarIndex(size_t ivar, size_t irec) { return (irec - 1) * pg.Shape.RecordIndexSize + ivar; };
protected :
	bool m_varxMinutes, m_varxHours, m_varxDays, m_varxMonths, m_varxYears;
	float* m_varx_minute;
	float* m_varx_hour;
	float* m_varx_days;
	vector<float> m_varx_months;
	float* m_varx_years;


	size_t m_NumberOfYears;
	size_t *m_YearIndexStart, *m_YearRecords;
	size_t m_NumberOfDays, m_ActualSection;
	size_t *m_DayIndexStart, *m_DayRecords;
	double *m_YearShift, *m_DayShift;

	char *m_str;
	
};

