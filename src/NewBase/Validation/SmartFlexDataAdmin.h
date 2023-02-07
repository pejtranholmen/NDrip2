#pragma once
#include "../ParValueChangeRead/ChangeFromFile.h"
class MR;
class MultiStorageData;
class SmartFlexDataAdmin: public ChangeFromFile 
{
public:
	SmartFlexDataAdmin();
	~SmartFlexDataAdmin();
	bool CalculateSumMeanValidValues_SV_Variables();

	size_t GetNumberofSumVariables();

	float Get_SV_Value(size_t variable, size_t SV_Index, size_t RunNo);
	bool Get_SV_MeanAllPerformance(size_t RunNo, vector<vector<float>> &Values);

	bool Get_SV_Value_Accepted(vector<vector <float>>& values);
	bool Get_SV_Value_All(vector<vector <float>>& values);

	bool Are_SV_Values_Defined(size_t variable=0);
	bool m_Are_MeanValues_SV_VariablesEstimated;

protected:
	
	MR *m_pSimDoc;

private:

	float GetMeanValid_Sum_AbsME(unsigned long rec) ;
	float GetMeanValid_Sum_RMSE(unsigned long rec) ;
	float GetValidSum(unsigned long, unsigned long);
	void  SetValidSum(unsigned long, unsigned long, float value);
	float GetValidSumAll(unsigned long, unsigned long);	
	float GetValidSumLow(unsigned long);
	float GetValidSumHigh(unsigned long);

	MultiStorageData *m_pMultiStorage;
	float GetSumValid_Sum_LogLi(unsigned long rec);
	

};

