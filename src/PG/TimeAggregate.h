#pragma once
#include "../std.h"
#include "PG.H"

struct TIMESTAT
{	int VarSub;
	int VarSubRep;
	int nSub;
	int VarUsed;
	int VarUsedRep;
	float R2;
	float A0;
	float A1;
};
class TimeAggregate
{
public:
	TimeAggregate(void) {;};
	~TimeAggregate(void) {;};
	bool CreateNewAggregatedFile(CPG *pPG, int option);

	string m_DestFileName;
	vector<string> m_ResultSum;

protected:

	CPG *m_pPGSource;

}

;

