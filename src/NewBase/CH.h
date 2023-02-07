#pragma once

#include "./DB.h"

class CHPARv{			// The selected objects
public:
	SimB* pBase;
	int Type;
	size_t TabIndex;
	size_t nCount;
	size_t NumDates;
	double StartValue;
};

class CHRunv{				//Keeping track of the CH object during run
public:
	unsigned int Ind_pP;
	unsigned int Ind_Count;
};

class CH :public DB
{

public:
	CH();
	vector<CHPARv> m_P_CH_Array;
	vector<CHRunv> m_CH_Run_Array;
	
	size_t CH_Checker_build();
	unsigned int	GetFirstHit();
	unsigned int GetHit(size_t index);
	bool CH_Checker_ChangeValue(size_t index);
	size_t GetSimCount() {return m_CH_SimCount;};
	void UpdateSimCount() {m_CH_SimCount++;};
	bool Make_ChaParCheck(unsigned int ActMin);
	size_t GetNextChaParDate() {return m_NextDateToSetChangeAt;};
	void SetNextChaParDate(size_t value) {m_NextDateToSetChangeAt=value;};
	void CH_Checker_Reset();
	bool Set_P_CH(SimB*, int type, int index=0);
	//bool Set_PAR_CH(Ps*, int type, int index=0);
	bool Reset_P_CH(SimB*);

	bool ApplyOptStartValues();

private :
	size_t m_ChaParMinute;
	size_t m_NextDateToSetChangeAt;
	size_t m_CH_SimCount;
};
