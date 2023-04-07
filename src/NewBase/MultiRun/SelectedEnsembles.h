#pragma once
#include "../../ModelTypes/NewModelType.h"
class MR;
struct STAT_PERF;
#include "SelectedAddEnsembles.h"
struct UPDATEMEMORY {
	bool AllRuns;
	bool AcceptedRuns;
};


class SelectedEnsembles : public SelectedAddEnsembles
{
public:
	SelectedEnsembles();
	virtual ~SelectedEnsembles();



	

	bool ResetFirstSection();
	


	unsigned long GetMultiRunNo(unsigned long);
	float GetMinValidPar(unsigned long);
	float GetMaxValidPar(unsigned long);



	float GetValidSum(unsigned long, unsigned long);
	void  SetValidSum(unsigned long, unsigned long, float value);
	float GetValidSumAll(unsigned long, unsigned long);
	void  SetValidLow(unsigned long, unsigned long split, float);
	void  SetValidHigh(unsigned long, unsigned long split, float);
	float GetValidSumLow(unsigned long);
	float GetValidSumHigh(unsigned long);
	void  SetValidSumLow(unsigned long, float);
	void  SetValidSumHigh(unsigned long, float);
	
	unsigned long  UpdateAccepted(unsigned long, unsigned long);

	void Update_SV_VarStat(bool AcceptedOnly=true);
	void Update_TSV_VarStat(bool AcceptedOnly = true);
	
	unsigned long  UpdateAcceptedRuns(bool Reset=false, bool SaveToFile=false);
	bool Check_Adjust_ActualEnsemble();


	float GetCVSumValid(unsigned long);
	float GetMaxSumValid(unsigned long);

	float GetMinSumValidAll(unsigned long);
	float GetMeanSumValidAll(unsigned long);
	float GetCVSumValidAll(unsigned long);
	float GetMeanValidAll(unsigned long, unsigned long split = 0);
	float GetMinValidAll(unsigned long, unsigned long split = 0);
	float GetMaxValidAll(unsigned long, unsigned long split = 0);
	float GetCV_ValidAll(unsigned long, unsigned long split = 0);





	
	float GetMinMeanValidAll_R2();
	float GetMinMeanValidAll_A0();
	float GetMinMeanValidAll_A1();
	float GetMinMeanValidAll_AbsME();
	float GetMinMeanValidAll_RMSE();
	float GetMinMeanValidAll_NSE();
	float GetMinSumValidAll_LogLi();
	float GetMaxMeanValidAll_R2();
	float GetMaxMeanValidAll_A0();
	float GetMaxMeanValidAll_A1();
	float GetMaxMeanValidAll_AbsME();
	float GetMaxMeanValidAll_RMSE();
	float GetMaxMeanValidAll_NSE();
	float GetMaxSumValidAll_LogLi();

	float GetMeanValidLow_R2();
	float GetMeanValidLow_A0();
	float GetMeanValidLow_A1();
	float GetMeanValidLow_AbsME();
	float GetMeanValidLow_RMSE();
	float GetMeanValidLow_NSE();
	float GetSumValidLow_LogLi();
	float GetMeanValidHigh_R2();
	float GetMeanValidHigh_A0();
	float GetMeanValidHigh_A1();
	float GetMeanValidHigh_AbsME();
	float GetMeanValidHigh_RMSE();
	float GetMeanValidHigh_NSE();
	float GetSumValidHigh_LogLi();

	void SetMeanValidLow_R2(float);
	void SetMeanValidLow_A0(float);
	void SetMeanValidLow_A1(float);
	void SetMeanValidLow_AbsME(float);
	void SetMeanValidLow_RMSE(float);
	void SetMeanValidLow_NSE(float);
	void SetSumValidLow_LogLi(float);
	
	void SetMeanValidLow_Sum_AbsME(float);	
	void SetMeanValidLow_Sum_RMSE(float);
	void SetSumValidLow_Sum_LogLi(float);

	void SetMeanValidHigh_R2(float);
	void SetMeanValidHigh_A0(float);
	void SetMeanValidHigh_A1(float);
	void SetMeanValidHigh_AbsME(float);
	void SetMeanValidHigh_RMSE(float);
	void SetMeanValidHigh_NSE(float);
	void SetSumValidHigh_LogLi(float);

	void SetMeanValidHigh_Sum_AbsME(float);
	void SetMeanValidHigh_Sum_RMSE(float);
	void SetSumValidHigh_Sum_LogLi(float);

	void SetValidDynLogLi(unsigned long iv, unsigned long idin, unsigned long all, float *pfloat);
	void SetValidDynResiduals(unsigned long iv, unsigned long idin, unsigned long all, float *pfloat) { for (unsigned long i = 0; i<7; i++) { m_ValidDynResiduals[all*(m_NumValidVar * 7 * 13) + iv * 13 * 7 + idin * 7 + i] = *(pfloat + i); } };
	float GetValidDynLogLi(unsigned long iv, unsigned long idin, unsigned long idut, unsigned long all);
	float GetValidDynResiduals(unsigned long iv, unsigned long idin, unsigned long idut, unsigned long all) { return m_ValidDynResiduals[all*(m_NumValidVar * 7 * 13) + iv * 13 * 7 + idin * 7 + idut]; };




	void UpdateBayesPostDist();
	void UpdateSumVarStatReset();


	float GetSumV(unsigned long, unsigned long);
	float GetSumV_Selected(unsigned long, unsigned long);
	unsigned long GetNumAccepted();
	unsigned long GetNumBayesAccepted(unsigned long num_burn_in);
	
	float GetMinValid(unsigned long, unsigned long split=0);
	float GetMaxValid(unsigned long, unsigned long split=0);
	float GetMeanValid(unsigned long, unsigned long split=0);
	float GetCV_Valid(unsigned long, unsigned long split=0);

	
	
	float GetMinSumValid(unsigned long);
	float GetMeanSumValid(unsigned long);


	float GetMaxSumValidAll(unsigned long);
	bool IsMinUpdated, IsMaxUpdated, IsCVUpdated, IsMeanUpdated, IsCVSumUpdated, IsMeanSumUpdated, IsMeanSumUpdatedAll;
	bool IsCVSumUpdatedAll,IsMinSumUpdatedAll, IsMaxSumUpdatedAll;

	bool SelSumV_Index_Add(unsigned long);
	void SelSumV_Index_Reset();
	unsigned long  SelSumV_Index_Get(unsigned long);
	unsigned long  SelSumV_Index_GetNum();
	bool  SelSumV_Index_SelectAll();
	unsigned long GetNumberOfAddedFiles(string str="");
	string GetNameAddedFiles(unsigned long);
	string GetNameBaseFile();
	unsigned long GetAdd_MultiPGFileIndex(unsigned long simRunNo);
	unsigned long GetNumberOfRuns() { 
		return m_TotalNumberRuns;
	};
	unsigned long GetCriteria(unsigned long, unsigned long split=0);
	bool SetCriteria(unsigned long,unsigned long value, unsigned long split=0);
	void ResetCriteria();
	unsigned long GetMeanCriteria(unsigned long);
	bool SetMeanCriteria(unsigned long, unsigned long value);
	void ResetMeanCriteria();
	void ResetMeanSumCriteria();
	unsigned long GetSumCriteria(unsigned long);
	bool SetSumCriteria(unsigned long, unsigned long value);
	void ResetSumCriteria();
	unsigned long GetMeanSumCriteria(unsigned long);
	bool SetMeanSumCriteria(unsigned long, unsigned long value);

	unsigned long GetNumSplitPeriods();
	vector<float> GetPost_TSV_ValidPerformanceIndicatorVector(unsigned long, unsigned long split = 0);
	vector<float> GetPost_SV_ValidPerformanceIndicatorVector(size_t ivar, enum PERFORMANCE_VAR var=R2_STAT );


	bool SaveSplitData(unsigned long ind_val);
	bool RemoveAddedFiles();
	bool RemoveAddedFile(size_t index);
	bool AttachNewFiles();
	bool Set_Update_ValidVar();
	void SetAllToAcceptedRuns();
	size_t FindAcceptedRunsFromFlagInPar();
	void SetMinMaxOfPerformanceIndicators();
	bool SetPriorDistributions();
	void SetValid_SUM_Criteria();
	UPDATEMEMORY m_SV_StatMemory;
	bool ResetAndReOpenFile();

protected:
	void SetValid_TSV_Criteria();





};


