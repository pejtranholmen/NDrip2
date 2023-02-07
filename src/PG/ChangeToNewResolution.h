#pragma once
#include "../std.h"
#include "PG.H"
struct DOUBLEMATCHVAR {int main_source_var; int main_scaling_var; int second_source_var;int second_scaling_var;};
struct MONTHPAIR {float value; long minute; size_t orgindex;};
struct MONTHVECTORS {
	vector<MONTHPAIR> Temp[12], Prec[12];
};
class ChangeToNewResolution
{
public:
	ChangeToNewResolution(void) {m_StartOut=0; m_EndOut=0;};
	~ChangeToNewResolution(void) {;};
	bool CreateNewFile(CPG *pPG,CPG *pPGScaling, int option, int extra_varoption);
	string m_DestFileName;
	vector<string> m_ResultSum;

protected:

	MONTHVECTORS EstimateMeanMonthValues(int irep);
	bool WriteOneMonthToSource(int irep, long minsource, long mindest, long  longmindestend, float sourcetemp, float newtemp, double PrecScaling, int extra_utvar);
	CPG *m_pPGSource, *m_pPGDest;
	long m_recout;

public:
	long m_StartOut, m_EndOut;
	DOUBLEMATCHVAR m_MatchingVar;

}
;
