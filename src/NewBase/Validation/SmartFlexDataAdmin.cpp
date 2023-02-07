#include "SmartFlexDataAdmin.h"
#include "../MR.h"
#include "../../Util/FUtil.hpp"



SmartFlexDataAdmin::SmartFlexDataAdmin()
{
	m_pSimDoc = dynamic_cast<MR*>(this);
	if (m_pSimDoc != nullptr) {
		m_pMultiStorage = &m_MStorage;
		m_Are_MeanValues_SV_VariablesEstimated = false;

	}
}


SmartFlexDataAdmin::~SmartFlexDataAdmin()
{
}

bool SmartFlexDataAdmin::CalculateSumMeanValidValues_SV_Variables()
{
	unsigned long NumValidSumVar = GetNumberofSumVariables();
	float dsum[7];


//	if (!Are_SV_Values_Defined()) return false;
	if(m_Are_MeanValues_SV_VariablesEstimated) return true;
	if (m_MStorage.m_Valid_SV_Mean.size() <= m_MStorage.m_ActualEnsemble) {

		m_MStorage.m_ActualEnsemble = m_MStorage.m_Valid_SV_Mean.size() - 1;

	}
	vector< vector<float> > MeanPerf;
	MeanPerf.resize(7);
	for (size_t jx = 0; jx < 7; jx++) {
		m_MStorage.m_Valid_SV_Mean[m_MStorage.m_ActualEnsemble].MeanPerFomanceIndicators[jx].resize(MR_Get_TotalNumberofRuns());
		MeanPerf[jx].resize(GetNumberofSumVariables());
	}


#ifndef COUPSTD
	MFC_Util mfcutil;
#endif
	for (unsigned long i = 0; i<MR_Get_TotalNumberofRuns(); i++) {

		Get_SV_MeanAllPerformance(i, MeanPerf);
#ifndef COUPSTD
	 if(i%1000==0)	mfcutil.SetPaneMessage("Estimating Mean value of all SV Performance indicators at each run:" + FUtil::STD_ItoAscii(size_t(i + 1)) + " ("+ FUtil::STD_ItoAscii(size_t(MR_Get_TotalNumberofRuns()))+")");
#endif		
	 for (size_t jx = 0; jx<7; jx++) {
			dsum[jx] = 0.;	
			for (size_t j = 0; j<GetNumberofSumVariables(); j++) {
				if (jx == 3)
					dsum[jx] += fabs(MeanPerf[jx][j]);
				else
					dsum[jx] += MeanPerf[jx][j];
			}
			switch (jx) {
				case 0: case 1: case 2: case 3: case 6:
					m_MStorage.m_Valid_SV_Mean[m_MStorage.m_ActualEnsemble].MeanPerFomanceIndicators[jx][i] = dsum[jx] / GetNumberofSumVariables();
					break;
				case 4: case 5:
					m_MStorage.m_Valid_SV_Mean[m_MStorage.m_ActualEnsemble].MeanPerFomanceIndicators[jx][i] = dsum[jx];
					break;
				}
			}
	}

	m_Are_MeanValues_SV_VariablesEstimated = true;
	return	true;
}
float SmartFlexDataAdmin::GetValidSumHigh(unsigned long ip)
{
	float v=m_MStorage.m_VarValidSumHigh[ip];
	if(v==-numeric_limits<double>::infinity( ) || v<-4.3E8)
		return MISSING;
	else
		return v;
}
float SmartFlexDataAdmin::GetValidSumLow(unsigned long ip)
{
	float v=m_MStorage.m_VarValidSumLow[ip];
	if(v==-numeric_limits<double>::infinity( )|| v<-4.3E8)
		return MISSING;
	else
		return v;
}float SmartFlexDataAdmin::GetMeanValid_Sum_AbsME(unsigned long rec) {

	if (m_MStorage.m_VarValidSumMean_AbsME.size() <= rec)
			return 0.;
		return m_MStorage.m_VarValidSumMean_AbsME[rec];
}
float SmartFlexDataAdmin::GetMeanValid_Sum_RMSE(unsigned long rec) {
		if (m_MStorage.m_VarValidSumMean_RMSE.size() <= rec)
			return 0.;
		return m_MStorage.m_VarValidSumMean_RMSE[rec];
}


float SmartFlexDataAdmin::GetValidSum(unsigned long rec, unsigned long ip)
{
	if(rec<0) return MISSING;
	return m_MStorage.m_VarValidSum[ip+ m_MStorage.m_NumSumValid*rec];
}
void SmartFlexDataAdmin::SetValidSum(unsigned long rec, unsigned long ip, float value)
{
//	auto koll=m_VarValidSum.size();
	m_MStorage.m_VarValidSum[ip+ m_MStorage.m_NumSumValid*rec]=value;
}
float SmartFlexDataAdmin::GetValidSumAll(unsigned long rec, unsigned long ip)
{
	if(rec<0) return MISSING;
	return m_MStorage.m_VarValidSum[ip+ m_MStorage.m_NumSumValid*rec];
}
float SmartFlexDataAdmin::GetSumValid_Sum_LogLi(unsigned long rec) {

	//if (m_VarMulti.size() <= rec)//
		//return 0.;
	//return m_VarMulti[m_NumMulti*(rec + 1) - 9];
	return MISSING;


}
size_t SmartFlexDataAdmin::GetNumberofSumVariables() {
	return m_ValSum_Array.size();
}

float SmartFlexDataAdmin::Get_SV_Value(size_t variable, size_t SV_Index, size_t RunNo)
{
	float fvalue;
	if (SV_Index >= m_ValSum_Array.size()) return MISSING;
	VALSUMv valsum = m_ValSum_Array[SV_Index];
	switch (variable) {
	case 0:
		if (valsum.R2_Multi.size() > RunNo)
			fvalue = valsum.R2_Multi[RunNo];
		else
			fvalue = MISSING;
		break;
	case 1:
		if (valsum.InterCept_Multi.size() > RunNo)
			fvalue = valsum.InterCept_Multi[RunNo];
		else
			fvalue = MISSING;
		break;
	case 2:
		if (valsum.Slope_Multi.size() > RunNo)
			fvalue = valsum.Slope_Multi[RunNo];
		else
			fvalue = MISSING;
		break;
	case 3:
		if (valsum.ME.size() > RunNo)
			fvalue = valsum.ME[RunNo];
		else
			fvalue = MISSING;
		break;
	case 4:
		if (valsum.RMSE.size() > RunNo)
			fvalue = valsum.RMSE[RunNo];
		else
			fvalue = MISSING;
		break;
	case 5:
		if (valsum.LogLi.size() > RunNo)
			fvalue = valsum.LogLi[RunNo];
		else
			fvalue = MISSING;
		break;
	case 6:
		if (valsum.NSE.size() > RunNo)
			fvalue = valsum.NSE[RunNo];
		else
			fvalue = MISSING;
		break;
	}

	return fvalue;
}

bool SmartFlexDataAdmin::Get_SV_MeanAllPerformance(size_t RunNo, vector<vector<float>>& Values)
{
	if (m_ValSum_Array.size() > 0 && m_ValSum_Array[0].R2_Multi.size() == 0) return false;
	for (size_t i = 0; i < m_ValSum_Array.size(); i++) {	
		Values[0][i] = m_ValSum_Array[i].R2_Multi[RunNo];
		Values[1][i] = m_ValSum_Array[i].InterCept_Multi[RunNo];
		Values[2][i] = m_ValSum_Array[i].Slope_Multi[RunNo];
		Values[3][i] = m_ValSum_Array[i].ME[RunNo];
		Values[4][i] = m_ValSum_Array[i].RMSE[RunNo];
		Values[5][i] = m_ValSum_Array[i].LogLi[RunNo];
		Values[6][i] = m_ValSum_Array[i].NSE[RunNo];		
	}

	return false;
}



bool SmartFlexDataAdmin::Get_SV_Value_Accepted(vector<vector<float>>& values)
{
	size_t numvalsum = m_ValSum_Array.size();
	for (size_t i = 0; i < m_ValSum_Array.size(); i++) {
		for (size_t irun = 0; irun <m_pSimDoc->m_MStorage.m_AcceptedRuns[m_pSimDoc->m_MStorage.m_ActualEnsemble].size(); irun++) {
			size_t runno = m_pSimDoc->m_MStorage.m_AcceptedRuns[m_pSimDoc->m_MStorage.m_ActualEnsemble][irun];
			values[i][irun] = m_ValSum_Array[i].R2_Multi[runno];
			values[numvalsum + i][irun] = m_ValSum_Array[i].InterCept_Multi[runno];
			values[numvalsum* 2 + i][irun] = m_ValSum_Array[i].Slope_Multi[runno];
			values[numvalsum *3 + i][irun] = m_ValSum_Array[i].ME[runno];
			values[numvalsum *4 + i][irun] = m_ValSum_Array[i].RMSE[runno];
			values[numvalsum *5 + i][irun] = m_ValSum_Array[i].LogLi[runno];
			values[numvalsum *6 + i][irun] = m_ValSum_Array[i].NSE[runno];
		}
	}
	return false;
}

bool SmartFlexDataAdmin::Get_SV_Value_All(vector<vector<float>>& values)
{
	size_t numvalsum = m_ValSum_Array.size();
	for (size_t i = 0; i < m_ValSum_Array.size(); i++) {;

			values[i] = m_ValSum_Array[i].R2_Multi;
			values[numvalsum+i] = m_ValSum_Array[i].InterCept_Multi;
			values[2*numvalsum+i] = m_ValSum_Array[i].Slope_Multi;
			values[3*numvalsum+i] = m_ValSum_Array[i].ME;
			values[4 * numvalsum+i] = m_ValSum_Array[i].RMSE;
			values[5 * numvalsum+i] = m_ValSum_Array[i].LogLi;
			values[6 * numvalsum+i] = m_ValSum_Array[i].NSE;
	}
	return false;
}

bool SmartFlexDataAdmin::Are_SV_Values_Defined(size_t variable)
{
	size_t SV_Index = 0;
	if (m_ValSum_Array.size() > SV_Index) {
		VALSUMv valsum = m_ValSum_Array[SV_Index];
		switch (variable) {
		case 0:
			if (valsum.R2_Multi.size() == 0) return false;
			break;
		case 1:
			if (valsum.InterCept_Multi.size() == 0) return false;
			break;
		case 2:
			if (valsum.Slope_Multi.size() == 0) return false;
			break;
		case 3:
			if (valsum.ME.size() == 0) return false;
			break;
		case 4:
			if (valsum.RMSE.size() == 0) return false;
			break;
		case 5:
			if (valsum.LogLi.size() == 0) return false;
			break;
		case 6:
			if (valsum.NSE.size() == 0) return false;
			break;
		}
		return true;
	}
	return false;
}

