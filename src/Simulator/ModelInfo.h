#pragma once
#include "../PG/PG.H"
#include "./ModelConst.h"

#include <time.h>
class P;
class ValidationData;

class ModelInfo: public ModelConst{
	public:
	ModelInfo();
	~ModelInfo();
	
	void AddModel(string, void*);
	void* GetModelPtr(string);

	void SetAbioticTimeSteps(size_t TimeResolutionOption);
	
	
	double GetRemainingTime(bool Multi=false);
	string GetRemainingTimeString(bool Multi=false);
	double GetTimeUsed();
	string GetTimeUsedString();
	void SetPause() {m_Run_Pause=true;} ;
	void SetGoSim() {m_Run_Pause=false;} ;
	bool IsPause() {return m_Run_Pause;};
	double GetRelRun() {return RelRunTimeSingle;};
	size_t GetRunDelay() {return m_Run_Delay;};
	void SetRunDelay(size_t delay) {m_Run_Delay=delay;};
	double GetSimCurrentTime() {return T_Current;};
	double GetSecSimTime() {return T_Current*86400-213300.;};
	

	 bool StartStateValue;
	 map<string, void*> ptrModule;
	 double XFMAX,CAPILMAX;	
	 size_t NumMethod;
	 double XNLev;
	
	 double T_Step, T_StepOrg, T_StepSnow, T_Step_NC, T_Shift;
	 double  T_Current, T_Start, T_End, T_Elapsed, T_SimPeriod, T_StartSimPeriod;
	 double RelRunTimeSingle, RelRunTimeMulti;
	 long T_TotNumIter, T_CountIter, T_CountIterOrg, T_CountAdd;
	 size_t T_RatioInput, T_RatioNC, T_NumIterNC_Day;
	 size_t T_RatioOrgStep;

	 size_t m_OutRecord; // Total number of records to store for Entire Sim period
	 size_t m_OutInterval;
	 size_t m_OutRecordSimPer; // Number of records to store for Standard sim period
	 size_t m_NumOutputs, m_Repit, m_RecOutput;
	 size_t m_ActMin, m_MinStore;
	 size_t m_ActMinRead; // The minutes number to read DrivingFiles


	 size_t NoDimCounters;	//! Number of Dimensions in multiruns - 0 = No multirun
	 size_t DimCounters[10];	//! Counter for each of up to 10-dimensions
	 size_t TotalNoMulRun, ActualNoMulRun;






bool FIRST, NC_Step;

bool Run_Res_Id;

	public:
	void Set_SimStart_Parameters() {m_Running=true; m_Run_Pause=false; m_Run_Step=false;m_Run_Delay=0; };
	void Set_Sim_EndParameters() {m_Running=false;  };
	ValidationData *m_pValData;
	clock_t	m_SimStart, m_UsedTime;
	size_t m_Run_Delay;
	bool m_Run_Pause;
	bool m_Run_Step;
	bool m_Running;

};
