#pragma once
#include "PG.h"
class PGCalc :
	public CPG
{
public:
	PGCalc(void);
	~PGCalc(void);
	vector<size_t> GetYearStartIndex();
	vector<size_t> GetDayStartIndex();
};

