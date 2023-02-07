#pragma once
#include "../../ModelTypes/NewModelType.h"
#include "../../Util/ReadWriteTypes.h"
class MR;
struct STAT_PERF;
#include "MultiStorageData.h"


class MultiStorageStatData : public MultiStorageData
{
public:
	MultiStorageStatData();
	virtual ~MultiStorageStatData();
	bool ReadWrite_Accepted(bool reading);
	bool ReadWrite_MultiPar(bool reading);
	bool ReadWrite_CoCor(bool reading);
	bool ReadWrite_Criteria(bool reading);
	bool ReadWrite_OptionalTSV_Valid(bool reading);
	bool ReadWrite_OptionalSUM_Valid(bool reading);
	bool ReadWrite_Parameters(bool reading);
	bool EvaluateFile(bool reading);

	float GetMeanValid_R2(unsigned long rec) {
		if (m_VarValidMean_R2.size() <= rec)
			return 0.;
		return m_VarValidMean_R2[rec];
	};
	float GetMeanValid_A0(unsigned long rec) {
		if (m_VarValidMean_A0.size() <= rec)
			return 0.;
		return m_VarValidMean_A0[rec];
	};
	float GetMeanValid_A1(unsigned long rec) {
		if (m_VarValidMean_A1.size() <= rec)
			return 0.;
		return m_VarValidMean_A1[rec];
	};
	float GetMeanValid_AbsME(unsigned long rec) {
		if (m_VarValidMean_AbsME.size() <= rec)
			return 0.;
		return m_VarValidMean_AbsME[rec];
	};
	float GetMeanValid_RMSE(unsigned long rec) {
		if (m_VarValidMean_RMSE.size() <= rec)
			return 0.;
		return m_VarValidMean_RMSE[rec];
	};
	float GetMeanValid_NSE(unsigned long rec) {
		if (m_VarValidMean_NSE.size() <= rec)
			return 0.;
		return m_VarValidMean_NSE[rec];
	};

	float GetSumValid_LogLi(unsigned long rec) {
		if (m_VarValidSum_LogLi.size() <= rec)
			return 0.;
		else
			return m_VarValidSum_LogLi[rec];
	}


	float GetMinMeanValid_R2() {return m_ValidMin[0];};
	float GetMinMeanValid_A0() { return m_ValidMin[1]; };
	float GetMinMeanValid_A1() { return m_ValidMin[2]; };
	float GetMinMeanValid_AbsME() { return m_ValidMin[3]; };
	float GetMinMeanValid_RMSE() { return m_ValidMin[4]; };
	float GetMinMeanValid_NSE() {return m_ValidMin[6];};
	float GetMinSumValid_LogLi() { return m_ValidMin[5];};
	float GetMaxMeanValid_R2() { return m_ValidMax[0]; };
	float GetMaxMeanValid_A0() { return m_ValidMax[1]; };
	float GetMaxMeanValid_A1() { return m_ValidMax[2]; };
	float GetMaxMeanValid_AbsME() { return m_ValidMax[3]; };
	float GetMaxMeanValid_RMSE() {	return m_ValidMax[4];};
	float GetMaxMeanValid_NSE() {return m_ValidMax[6];};
	float GetMaxSumValid_LogLi() {return m_ValidMax[5];	};
	bool  GetAcceptedRunNo(unsigned long index) {
		float v = GetMultiP(index, m_NumMulti - 10);
		if (v <= 0.)
			return false;
		else
			return true;
	};
	size_t Get_AcceptedRuns(unsigned long index) {
		if (index<m_AcceptedRuns[m_ActualEnsemble].size())
			return m_AcceptedRuns[m_ActualEnsemble][index];
		else
			return string::npos;
	};
	bool CompleteProcessingOfInputFiles_MultiStorage();
	bool SetActualPerformanceVector_TSV_Variables(size_t ActualPerformance=string::npos);

	bool SetActual_AcceptedEnsemble();
	bool Check_Change_PerformanceIndicator_ToActual_Ensemble();
	bool m_IsStatOpen;


protected:

	void UpdateMeanValidValues(bool SV_Variables=false);



	bool m_ResetTransformedPerformance;



private :
	ReadWriteTypes m_EvaluationFile;
	size_t m_PosOptional_TSV_Start;
	size_t m_PosOptional_SUM_Start;
};


