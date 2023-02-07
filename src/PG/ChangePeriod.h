#pragma once
#include "../std.h"
#include "PG.H"


class ChangePeriod
{
public:
	ChangePeriod(void) {m_StartOut=0; m_EndOut=0;};
	~ChangePeriod(void) {;};
	bool CreateNewFile(CPG *pPG, int option);
	string m_DestFileName;
	vector<string> m_ResultSum;

protected:


	CPG *m_pPGSource;

public:
	long m_StartOut, m_EndOut;

}
;
