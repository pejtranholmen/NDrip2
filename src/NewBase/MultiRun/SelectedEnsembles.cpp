// SelectedEnsembles.cpp : implementation file
//
#ifndef COUPSTD
#include "../stdafx.h"
#endif
#include "../../Util/FUtil.hpp"
#include "../../Util/StatUtil.h"

#include "../NewMap.h"
#include "SelectedEnsembles.h"

#ifndef COUPSTD
#include "../NewBase/FileScanner.h"

#endif


#include <limits>

// SelectedEnsembles


SelectedEnsembles::SelectedEnsembles()
{
	m_SV_StatMemory.AcceptedRuns = false;
	m_SV_StatMemory.AllRuns = false;

}

SelectedEnsembles::~SelectedEnsembles()
{

}

bool SelectedEnsembles::ResetFirstSection()
{
	m_IsOpen = false;

	return true;
}

unsigned long SelectedEnsembles::GetNumSplitPeriods()
{
	return m_NumSplitPeriod;
}



unsigned long SelectedEnsembles::UpdateAccepted(unsigned long ip, unsigned long isum)
{
	if(ip!=-1) {
		for(unsigned long i=0; i<m_TotalNumberRuns;i++) {
			if((GetValid(i, ip)<GetValidLow(ip)||GetValid(i,ip)>GetValidHigh(ip))&&GetAcceptedRunNo(i))
				SetAcceptedRunNo(i, false);
		}
	}
	if(isum!=-1) {
		for(unsigned long i=0; i<m_TotalNumberRuns;i++) {
			if((GetValidSum(i, isum)<GetValidSumLow(isum)||GetValidSum(i,isum)>GetValidSumHigh(isum))&&GetAcceptedRunNo(i))
				SetAcceptedRunNo(i, false);
		}
	}

	m_TotalNumAccepted=0;
	m_AcceptedRuns[m_ActualEnsemble].clear();
	m_Accepted.clear();
	for(unsigned long i=0;i<m_TotalNumberRuns;i++) {
		if(GetAcceptedRunNo(i)) {
			m_AcceptedRuns[m_ActualEnsemble].push_back(i);
		}
	}
	EvaluateFile(false);
	return m_TotalNumAccepted;
					
}

void SelectedEnsembles::UpdateBayesPostDist() {
	//unsigned long naccepted=m_MStorage.GetNumAccepted();



}
void SelectedEnsembles::SetAllToAcceptedRuns() {

	//UpdateSumVarStatReset();

	if (m_Valid_TSV[m_ActualEnsemble].v[0].size() == 0 && m_NumValid > 0) 
		AllocateValid_TSV(m_ActualEnsemble);
	if (m_NumValid > 0) {
		if (m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_MIN][0] == 0 && m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_MAX][0] == 0)
			Update_TSV_VarStat(false);

		for (unsigned long split = 0; split <= m_NumSplitPeriod; split++) {
			for (unsigned long i = 0; i < m_NumValid + m_NumExtraValid; i++) {
				size_t index = i + split*(m_NumValid + m_NumExtraValid);
				m_Valid_TSV[m_ActualEnsemble].v[E_SELECT_LOW][index] = m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_MIN][index];
				m_Valid_TSV[m_ActualEnsemble].v[E_SELECT_HIGH][index] = m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_MAX][index];
				m_Valid_TSV[m_ActualEnsemble].criteria[index] = 0;
			}
		}

		for (unsigned long i = 0; i < m_Valid_TSV[m_ActualEnsemble].meancriteria.size(); i++)
			m_Valid_TSV[m_ActualEnsemble].meancriteria[i] = 0;
	}




	if (m_Valid_SV[m_ActualEnsemble].v[0].size() == 0) return;
	if (!m_pNewMap->Are_SV_Values_Defined()) return;


	if (m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MIN][0] == m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MAX][0]) {
			Update_SV_VarStat(false); 
	};
	//if (Are_SV_Values_Defined()) {
		size_t index;
		for (size_t jx = 0; jx < 7; jx++) {
			for (unsigned long i = 0; i < m_pNewMap->GetNumberofSumVariables(); i++) {
				index = jx*m_pNewMap->GetNumberofSumVariables() + i;

				m_Valid_SV[m_ActualEnsemble].v[E_POST_MEAN][index] = m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MEAN][index];
				m_Valid_SV[m_ActualEnsemble].v[E_POST_CV][index] = m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_CV][index];
				m_Valid_SV[m_ActualEnsemble].v[E_POST_MIN][index] = m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MIN][index];
				m_Valid_SV[m_ActualEnsemble].v[E_POST_MAX][index] = m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MAX][index];

				m_Valid_SV[m_ActualEnsemble].v[E_SELECT_LOW][index] = m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MIN][index];
				m_Valid_SV[m_ActualEnsemble].v[E_SELECT_HIGH][index] = m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MAX][index];
				m_Valid_SV[m_ActualEnsemble].criteria[index] = 0;
			}
			m_Valid_SV_Mean[m_ActualEnsemble].v[E_SELECT_LOW][jx] = m_Valid_SV_Mean[m_ActualEnsemble].v[E_PRIOR_MIN][jx];
			m_Valid_SV_Mean[m_ActualEnsemble].v[E_SELECT_HIGH][jx] = m_Valid_SV_Mean[m_ActualEnsemble].v[E_PRIOR_MAX][jx];

			m_Valid_SV_Mean[m_ActualEnsemble].v[E_POST_MEAN][jx] = m_Valid_SV_Mean[m_ActualEnsemble].v[E_PRIOR_MEAN][jx];
			m_Valid_SV_Mean[m_ActualEnsemble].v[E_POST_CV][jx] = m_Valid_SV_Mean[m_ActualEnsemble].v[E_PRIOR_CV][jx];
			m_Valid_SV_Mean[m_ActualEnsemble].v[E_POST_MIN][jx] = m_Valid_SV_Mean[m_ActualEnsemble].v[E_PRIOR_MIN][jx];
			m_Valid_SV_Mean[m_ActualEnsemble].v[E_POST_MAX][jx] = m_Valid_SV_Mean[m_ActualEnsemble].v[E_PRIOR_MAX][jx];





			m_Valid_SV_Mean[m_ActualEnsemble].criteria[jx] = 0;

		}
	//}

}

unsigned long SelectedEnsembles::UpdateAcceptedRuns(bool Reset, bool SaveToFile)
{

#ifndef COUPSTD	
	CWinApp* pApp = AfxGetApp();
	pApp->BeginWaitCursor();
	


	MFC_Util mfc_util;
	if (Reset)
		mfc_util.SetPaneMessage("Resetting of all runs to accepted");
	else
		mfc_util.SetPaneMessage("Find Posterior ensemble of accepted runs");
#endif

	auto setall_valid = [&]() {
		for(size_t i=0; i<6; i++)
			m_MeanTSV_Accepted[i]=m_MeanTSV_All[i];
		for (unsigned long i = 0; i<m_TotalNumberRuns; i++) {
			SetAcceptedRunNo(i, true);
		}
	};

	size_t index_trans = string::npos;
	if (m_Valid_TSV[m_ActualEnsemble].VarTranformed > 0) {

		auto it = MapOfStoredTSV.find(m_Valid_TSV[m_ActualEnsemble].VarTranformed);
		if (it != MapOfStoredTSV.end())
			index_trans = (*it).second;

		if (m_VarValidTransformed.size() == 0)
			index_trans = string::npos;
		else if (m_VarValidTransformed[index_trans].cumvalue != m_Valid_TSV[m_ActualEnsemble].VarTranformed)
			index_trans = string::npos;


	};
	unsigned long ind, ind_rmse, ind_a1;
	float rmse_o, value;

	auto GetValue = [&](size_t irun, size_t ip) {
		float fval;
		fval = MISSING;
		if (ip < m_NumValid) {
			if (index_trans != string::npos) {
				size_t indextsv = ip + irun*m_NumValid;
				fval = m_VarValidTransformed[index_trans].VarValidVector[indextsv];
			}
			else
				fval = GetValid(irun, ip);
		}
		else if (rmse_o != 0) {
			float vtest;
			if (index_trans != string::npos) {
				size_t indextsv = ind_rmse + irun*m_NumValid;
				vtest = 1 - m_VarValidTransformed[index_trans].VarValidVector[indextsv];
			}
			else
				vtest = 1. - GetValid(irun, ind_rmse)*GetValid(irun, ind_rmse) / rmse_o;
			if (vtest > MISSING&&vtest < 1.00001)
				fval = float(vtest);
		}



		return fval;
	};



	auto testcond_tsv = [&](size_t iv, size_t isplit) {
#ifndef COUPSTD
		mfc_util.SetPaneMessage("Find Posterior ensemble of accepted runs using SV Criteria " + FUtil::STD_ItoAscii(iv + 1) + " (" + FUtil::STD_ItoAscii(size_t(m_NumValid + m_NumExtraValid)) + ")");
#endif
		if (GetValidLow(iv, isplit) > GetMinValidAll(iv, isplit) || GetValidHigh(iv, isplit) < GetMaxValidAll(iv, isplit)) {
			if (isplit == 0) {


				if (iv >= m_NumValid) {
					ind = iv - m_NumValid;
					ind_rmse = 4 * m_NumValid / 6 + ind;
					rmse_o = m_pNewMap->m_ValidationData.GetValRMSE_O(ind) / m_pNewMap->m_ValidationData.GetValn(ind);

				}
				ind_a1 = 2 * m_NumValid / 6 + iv % (m_NumValid / 6);
				float a1_value;


				for (unsigned long i = 0; i < m_TotalNumberRuns; i++) {
					a1_value = GetValid(i, ind_a1);
					if (iv < m_NumValid)
						value = GetValue(i, iv);
					else
						value = 1 - GetValue(i, ind_rmse)*GetValue(i, ind_rmse) / rmse_o;
					if ((value<GetValidLow(iv) || value>GetValidHigh(iv)) && GetAcceptedRunNo(i))
						SetAcceptedRunNo(i, false);
					/*else if(a1_value==0)
					SetAcceptedRunNo(i, false);*/
				}
			}
			else {
				float value;
				if (GetValidHigh(iv, isplit) < m_Valid_TSV[0].v[E_SELECT_HIGH][LOGLI_STAT]) {

					SetValidHigh(iv, isplit, m_Valid_TSV[0].v[E_PRIOR_MAX][LOGLI_STAT]);


				}
				for (unsigned long i = 0; i < m_TotalNumberRuns; i++) {
					value = GetSplitValid(i, iv, isplit - 1);
					if ((value<GetValidLow(iv, isplit) || value>GetValidHigh(iv, isplit)) && GetAcceptedRunNo(i))
						SetAcceptedRunNo(i, false);
				}


			}
		}
	};
	auto testcond_tsv_mean = [&]() {
		for (unsigned long i = 0; i<m_TotalNumberRuns; i++) {
			float fval;
			if (GetAcceptedRunNo(i)) {
				for (size_t iper = 0; iper < 6; iper++) {
					fval = m_MeanTSV_All[iper][i];
					if (fval > MISSING && (fval<m_MeanTSV_STAT[SELECT_LOW][iper] || fval>m_MeanTSV_STAT[SELECT_HIGH][iper]))
						SetAcceptedRunNo(i, false);
				}
				
			}

		}
	};
	auto testcond_ValSumValues_Stored = [&](size_t index) {
		for (unsigned long i = 0; i < m_TotalNumberRuns; i++) {
			float fval;
			VALSUM valsum = m_pNewMap->m_MStorage.m_ValSumValues_Stored[index];
			if (GetAcceptedRunNo(i)) {


				fval = m_pNewMap->m_MStorage.GetSumV(i, valsum.SumIndex-6);
					if (fval > MISSING && (fval<valsum.ValueMin|| fval>valsum.ValueMax))
						SetAcceptedRunNo(i, false);
				}

			}

	};
	auto testcond_sv = [&](size_t iv) {
#ifndef COUPSTD	
		mfc_util.SetPaneMessage("Find Posterior ensemble of accepted runs using SV Criteria "+FUtil::STD_ItoAscii(iv+1)+ " ("+FUtil::STD_ItoAscii(m_pNewMap->GetNumberofSumVariables())+")");
#endif
		VALSUMv valsum;
		valsum = m_pNewMap->GetValSumStruct(iv);
		for (size_t jx = 0; jx < 7; jx++) {
			size_t index = jx*m_pNewMap->GetNumberofSumVariables() + iv;
			for (size_t i = 0; i < m_TotalNumberRuns; i++) {
				switch (jx) {
				case 0:
					if(valsum.R2_Multi[i]<m_Valid_SV[m_ActualEnsemble].v[E_SELECT_LOW][index]||
						valsum.R2_Multi[i]>m_Valid_SV[m_ActualEnsemble].v[E_SELECT_HIGH][index])
						SetAcceptedRunNo(i, false);
					break;
				case 1:
					if (valsum.InterCept_Multi[i]<m_Valid_SV[m_ActualEnsemble].v[E_SELECT_LOW][index] ||
						valsum.InterCept_Multi[i]>m_Valid_SV[m_ActualEnsemble].v[E_SELECT_HIGH][index])
						SetAcceptedRunNo(i, false);
					break;
				case 2:
					if (valsum.Slope_Multi[i]<m_Valid_SV[m_ActualEnsemble].v[E_SELECT_LOW][index] ||
						valsum.Slope_Multi[i]>m_Valid_SV[m_ActualEnsemble].v[E_SELECT_HIGH][index])
						SetAcceptedRunNo(i, false);
					break;
				case 3:
					if (valsum.ME[i]<m_Valid_SV[m_ActualEnsemble].v[E_SELECT_LOW][index] ||
						valsum.ME[i]>m_Valid_SV[m_ActualEnsemble].v[E_SELECT_HIGH][index])
						SetAcceptedRunNo(i, false);
					break;
				case 4:
					if (valsum.RMSE[i]<m_Valid_SV[m_ActualEnsemble].v[E_SELECT_LOW][index] ||
						valsum.RMSE[i]>m_Valid_SV[m_ActualEnsemble].v[E_SELECT_HIGH][index])
						SetAcceptedRunNo(i, false);
					break;
				case 5:
					if (valsum.LogLi[i]<m_Valid_SV[m_ActualEnsemble].v[E_SELECT_LOW][index] ||
						valsum.LogLi[i]>m_Valid_SV[m_ActualEnsemble].v[E_SELECT_HIGH][index])
						SetAcceptedRunNo(i, false);
					break;
				case 6:
					if (valsum.NSE[i]<m_Valid_SV[m_ActualEnsemble].v[E_SELECT_LOW][index] ||
						valsum.NSE[i]>m_Valid_SV[m_ActualEnsemble].v[E_SELECT_HIGH][index])
						SetAcceptedRunNo(i, false);
					break;
				}			

			}
		}
	};
	auto testcond_sv_mean = [&]() {
		for(size_t jx=0; jx<7; jx++){
			for (size_t i = 0; i < m_TotalNumberRuns; i++) {
				if (m_Valid_SV_Mean[m_ActualEnsemble].MeanPerFomanceIndicators[jx][i] > m_Valid_SV_Mean[m_ActualEnsemble].v[E_SELECT_HIGH][jx] ||
					m_Valid_SV_Mean[m_ActualEnsemble].MeanPerFomanceIndicators[jx][i] < m_Valid_SV_Mean[m_ActualEnsemble].v[E_SELECT_LOW][jx])
					SetAcceptedRunNo(i, false);


			}
		}
	};


	if(m_pNewMap->MR_Get_NumTotalPar()>=0&&!m_pNewMap->IsBayesianCalibration()) {
		setall_valid();

		if(!Reset) {
			// individual indicators
			for(unsigned long isplit=0; isplit<=m_NumSplitPeriod; isplit++) {
				for (unsigned long iv = 0; iv < m_NumValid + m_NumExtraValid; testcond_tsv(iv, isplit), iv++);

			// Mean Indicators
				if (m_NumValid > 6) testcond_tsv_mean(); 
				
			}

			for (unsigned long iv = 0; iv < m_pNewMap->GetNumberofSumVariables(); testcond_sv(iv), iv++);

			if (m_pNewMap->GetNumberofSumVariables() > 1) testcond_sv_mean();

			for (unsigned long iv = 0; iv < m_pNewMap->m_MStorage.m_ValSumValues_Stored.size(); testcond_ValSumValues_Stored(iv), iv++);
			IsMinUpdated=false;
			IsCVUpdated=false;
			IsMeanUpdated=false;
			IsMaxUpdated=false;
		}
		else {
			SetAllToAcceptedRuns();		
		}
	}
#ifndef COUPSTD	
	CFileScanner ScanMeanProg(nullptr);
	ScanMeanProg.m_Info = "Scanning criterias";
	ScanMeanProg.Create(IDD_PROGRESS, nullptr);
	ScanMeanProg.ShowWindow(SW_SHOW);
#endif

	m_TotalNumAccepted = FindAcceptedRunsFromFlagInPar();
	SetMinMaxOfPerformanceIndicators();
	

	SetCoParUpdated(false);
	

	if(SaveToFile) {
		if(!Reset) m_pNewMap->UpdateCoVar();
#ifndef COUPSTD
		mfc_util.SetPaneMessage("Saving Data on Accepted Runs to SBin File:");
#endif
		EvaluateFile(false);
	}

#ifndef COUPSTD	
	pApp->EndWaitCursor();
	mfc_util.SetPaneMessage(FUtil::STD_ItoAscii(size_t(m_TotalNumAccepted)) + " Accepted Runs identified");
#endif
	return m_TotalNumAccepted;
	
					
}

bool SelectedEnsembles::Check_Adjust_ActualEnsemble()
{
	bool orig = true;
	if (m_ActualEnsemble >= m_Valid_TSV.size() && m_NumValid > 0) {
		m_ActualEnsemble = 0;
		orig = false;
	}
	else if (m_ActualEnsemble >= m_Valid_SV.size() && m_pNewMap->m_ValSum_Array.size() > 0) {
		m_ActualEnsemble = 0;
		orig = false;
	}
	return orig;
}



void SelectedEnsembles::SetValidHigh(unsigned long ip,unsigned long ind_split, float value)
{
	m_Valid_TSV[m_ActualEnsemble].v[E_SELECT_HIGH][ip + ind_split*(m_NumValid + m_NumExtraValid)] = value;
	
}
void SelectedEnsembles::SetValidLow(unsigned long ip, unsigned long ind_split, float value)
{
	m_Valid_TSV[m_ActualEnsemble].v[E_SELECT_LOW][ip + ind_split*(m_NumValid + m_NumExtraValid)] = value;
}


void SelectedEnsembles::SetValidSumHigh(unsigned long ip, float value)
{
	if(m_Valid_SV[m_ActualEnsemble].v[E_SELECT_HIGH].size()>ip)
	m_Valid_SV[m_ActualEnsemble].v[E_SELECT_HIGH][ip] = value;
}
void SelectedEnsembles::SetValidSumLow(unsigned long ip, float value)
{
	if (m_Valid_SV[m_ActualEnsemble].v[E_SELECT_LOW].size()>ip)
	m_Valid_SV[m_ActualEnsemble].v[E_SELECT_LOW][ip] = value;

}

float SelectedEnsembles::GetValidSumHigh(unsigned long ip)
{
	if (m_VarValidSumHigh.size() <= ip)  return MISSING;
	if (m_Valid_SV[m_ActualEnsemble].v[E_SELECT_HIGH].size() > ip)
		return m_Valid_SV[m_Valid_SV.size() - 1].v[E_SELECT_HIGH][ip];
	else
		return MISSING;
}
float SelectedEnsembles::GetValidSumLow(unsigned long ip)
{
	if (m_VarValidSumHigh.size() <= ip)  return MISSING;
	if (m_Valid_SV[m_ActualEnsemble].v[E_SELECT_LOW].size() > ip)
		return m_Valid_SV[m_Valid_SV.size() - 1].v[E_SELECT_LOW][ip];
	else
		return MISSING;
}
float SelectedEnsembles::GetValidSum(unsigned long rec, unsigned long ip)
{
	if(rec<0) return MISSING;
	if (m_pNewMap == nullptr) m_pNewMap = dynamic_cast<MR*>(this);
	if (m_pNewMap == nullptr) {
		return m_VarValidSum[ip + m_NumSumValid*rec];
	}
	size_t ipar;
	ipar = ip / 3;
	ip = ip % 3;
	if (rec >= m_pNewMap->m_ValSum_Array[ipar].ME.size()) return MISSING;
	switch (ip) {
	case 0:
		return m_pNewMap->m_ValSum_Array[ipar].ME[rec];
		break;
	case 1:
		return m_pNewMap->m_ValSum_Array[ipar].RMSE[rec];
		break;
	case 2:
		return m_pNewMap->m_ValSum_Array[ipar].LogLi[rec];
		break;

	}
	
	
	
	ip = ip % 3;
	switch (ip) {
	case 0:
		return m_pNewMap->m_ValSum_Array[rec].ME[0];
		break;
	case 1:
		return m_pNewMap->m_ValSum_Array[rec].RMSE[0];
		break;
	case 2:
		return m_pNewMap->m_ValSum_Array[rec].LogLi[0];
		break;

	}

	return MISSING;
}
void SelectedEnsembles::SetValidSum(unsigned long rec, unsigned long ip, float value)
{
	auto koll=m_VarValidSum.size();
	if (koll <= ip + m_NumSumValid*rec) return;
	m_VarValidSum[ip+m_NumSumValid*rec]=value;
}
float SelectedEnsembles::GetValidSumAll(unsigned long rec, unsigned long ip)
{
	if(rec<0) return MISSING;
	size_t ipar;
	ipar = ip / 3;
	ip = ip % 3;
	if (rec >= m_pNewMap->m_ValSum_Array[ipar].ME.size()) return MISSING;
	switch (ip) {
	case 0:
		return m_pNewMap->m_ValSum_Array[ipar].ME[rec];
		break;
	case 1:
		return m_pNewMap->m_ValSum_Array[ipar].RMSE[rec];
		break;
	case 2:
		return m_pNewMap->m_ValSum_Array[ipar].LogLi[rec];
		break;

	}




















	return m_VarValidSum[ip+m_NumSumValid*rec];
}
float SelectedEnsembles::GetSumV(unsigned long rec, unsigned long ip)
{
	return m_VarSum[ip+m_NumSum*rec];
}
float SelectedEnsembles::GetSumV_Selected(unsigned long rec, unsigned long ip)
{
	unsigned long ipp=SelSumV_Index_Get(ip);
	if(ipp>=0)
		return m_VarSum[ipp+m_NumSum*rec];
	else
		return MISSING;
}
unsigned long SelectedEnsembles::GetNumAccepted() 
{
	if ((m_TotalNumAccepted == 0 && m_TotalNumberRuns != (unsigned long)(string::npos) || m_TotalNumAccepted>m_TotalNumberRuns)&&m_TotalNumberRuns!=(unsigned long)(string::npos)) {
		unsigned long icount = 0;

		for (unsigned long i = 0; i < m_TotalNumberRuns; i++)
			if (GetMultiP(i, m_NumMulti - 10)>0)
				icount++;
		m_TotalNumAccepted = icount;
	}
	else if (m_TotalNumAccepted == string::npos)
		m_TotalNumAccepted = 0;

	return m_TotalNumAccepted;
}
unsigned long SelectedEnsembles::GetNumBayesAccepted(unsigned long num_burn_in) 
{
	if ((m_TotalNumBayesAccepted == 0 || m_TotalNumBayesAccepted > m_TotalNumberRuns)&&m_TotalNumberRuns != string::npos) {
		unsigned long icount = 0;

		for (unsigned long i = num_burn_in; i < m_TotalNumberRuns; i++)
			if (GetMultiP(i, m_NumMulti - 10)>0)
				icount++;
		m_TotalNumBayesAccepted = icount;
	}
	else if (m_TotalNumBayesAccepted == string::npos)
		m_TotalNumBayesAccepted = 0;
	return m_TotalNumBayesAccepted;
}

float SelectedEnsembles::GetMinValidPar(unsigned long ip)
{
	if (m_ParValidMin.size() == 0) return 0.;
	if (!IsMinUpdated) {
		float v = 1.1E38f;
		for (unsigned long i = 0; i<m_TotalNumAccepted; i++) {
			unsigned long irun = Get_AcceptedRuns(i);
			if (GetMultiP(irun, ip)<v)
				v = GetMultiP(irun, ip);
		}
		m_ParValidMin[ip] = v;
		return v;
	}
	else
		return m_ParValidMin[ip];

}
float SelectedEnsembles::GetMaxValidPar(unsigned long ip)
{
	if (m_ParValidMax.size() == 0) return 0.;
	if (!IsMinUpdated) {
		float v = float(MISSING);
		for (unsigned long i = 0; i<m_TotalNumAccepted; i++) {
			unsigned long irun = Get_AcceptedRuns(i);
			if (GetMultiP(irun, ip)>v)
				v = GetMultiP(irun, ip);
		}
		m_ParValidMax[ip] = v;
		return v;
	}
	else
		return m_ParValidMax[ip];

}
float SelectedEnsembles::GetMinValidAll(unsigned long ip, unsigned long ind_split)
{
	if(m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_MIN].size()==0) return 0.;
	if (m_NumValid == 0) return MISSING;
	if (!IsMinUpdated) {
		float v = 1.1E38f;
		if (ind_split == 0) {
			unsigned long ind, ind_rmse, ind_a1;
			float rmse_o, vtest;
			if (ip >= m_NumValid) {
				ind = ip - m_NumValid;
				ind_rmse = 4 * m_NumValid / 6 + ind;
				rmse_o = m_pNewMap->m_ValidationData.GetValRMSE_O(ind) / m_pNewMap->m_ValidationData.GetValn(ind);
			}
			ind_a1 = 2 * m_NumValid / 6 + ip % (m_NumValid / 6);
			for (unsigned long i = 0; i < m_TotalNumberRuns; i++) {
				if (ip < m_NumValid) {
					if (GetValid(i, ip) < v/*&&GetValid(i, ind_a1)!=0*/)
						v = GetValid(i, ip);
				}
				else {
					if (rmse_o != 0) {
						vtest = 1 - GetValid(i, ind_rmse)*GetValid(i, ind_rmse) / rmse_o;
						if (vtest < v/*&&GetValid(i, ind_a1)!=0*/)
							v = vtest;
					}
					else
						v = MISSING;
				}
			}
		//	m_VarValidMinAll[ip] = v;
			m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_MIN][ip] = v;
			return v;
		}
		else {
			for (unsigned long i = 0; i < m_TotalNumberRuns; i++) {
				if (GetSplitValid(i, ip, ind_split - 1) < v)
					v = GetSplitValid(i, ip, ind_split - 1);

			}
		//	m_VarValidMinAll[ip + ind_split*(m_NumValid + m_NumExtraValid)] = v;
			m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_MIN][ip + ind_split*(m_NumValid + m_NumExtraValid)]  = v;
			return v;

		}
	}
	else
		return m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_MIN][ip + ind_split*(m_NumValid + m_NumExtraValid)];
		//return m_VarValidMinAll[ip+ind_split*(m_NumValid+m_NumExtraValid)];


}
unsigned long SelectedEnsembles::GetMeanCriteria(unsigned long index)
{
	if(m_Valid_TSV[m_ActualEnsemble].meancriteria.size()<=index) return false;
	return m_Valid_TSV[m_ActualEnsemble].meancriteria[index];
}
unsigned long SelectedEnsembles::GetMeanSumCriteria(unsigned long index)
{
	if(m_Valid_SV[m_ActualEnsemble].meancriteria.size()<=index) return false;
	return m_Valid_SV[m_ActualEnsemble].meancriteria[index];
}	
bool SelectedEnsembles::SetMeanCriteria(unsigned long index, unsigned long value)
{
	if (m_Valid_TSV.size() == 0) return false;
	if(m_Valid_TSV[m_ActualEnsemble].meancriteria.size()<=index||index>=7) return false;
	m_Valid_TSV[m_ActualEnsemble].meancriteria[index] = value;
	return true;
}
bool SelectedEnsembles::SetMeanSumCriteria(unsigned long index, unsigned long value)
{
	if (m_Valid_SV.size() == 0) return false;
	if(m_Valid_SV[m_ActualEnsemble].meancriteria.size()<=index||index>=7) return false;
	m_Valid_SV[m_ActualEnsemble].meancriteria[index]=value;
	return true;
}
void SelectedEnsembles::ResetMeanCriteria()
{
     for(unsigned long i=0;i<7;i++)
		 m_Valid_TSV[m_ActualEnsemble].meancriteria[i]=0;
}
void SelectedEnsembles::ResetMeanSumCriteria()
{
     for(unsigned long i=0;i<3;i++)
		 m_Valid_SV[m_ActualEnsemble].meancriteria[i]=0;
}
unsigned long SelectedEnsembles::GetCriteria(unsigned long index, unsigned long ind_split)
{
	size_t max = m_Valid_TSV[m_ActualEnsemble].criteria.size();
	if(max<=index) return false;
	return  m_Valid_TSV[m_ActualEnsemble].criteria[index+ind_split*(m_NumValid+m_NumExtraValid)];
}
bool SelectedEnsembles::SetCriteria(unsigned long index, unsigned long value, unsigned long ind_split)
{
	if(m_Valid_TSV[m_ActualEnsemble].criteria.size() <=index||index>(m_NumValid+m_NumExtraValid)) return false;
	m_Valid_TSV[m_ActualEnsemble].criteria[index+ind_split*(m_NumValid+m_NumExtraValid)]=value;
	return true;
}
void SelectedEnsembles::ResetCriteria()
{
     for(unsigned long i=0;i<(m_NumValid+m_NumExtraValid)*(m_NumSplitPeriod+1);i++)
		 m_Valid_TSV[m_ActualEnsemble].criteria[i]=0;
}
unsigned long SelectedEnsembles::GetSumCriteria(unsigned long index)
{
	if(m_Valid_SV[m_ActualEnsemble].criteria.size()<=index) return false;
	return m_Valid_SV[m_ActualEnsemble].criteria[index];
}
bool SelectedEnsembles::SetSumCriteria(unsigned long index, unsigned long value)
{
	if(m_Valid_SV[m_ActualEnsemble].criteria.size()<=index) return false;
	m_Valid_SV[m_ActualEnsemble].criteria[index]=value;
	return true;
}
void SelectedEnsembles::ResetSumCriteria()
{
     for(unsigned long i=0;i<m_NumSumValid;i++)
		 m_Valid_SV[m_ActualEnsemble].criteria[i]=0;
	if( m_Valid_SV_Mean[m_ActualEnsemble].criteria.size() > 0) {
		 for (size_t i = 0; i < 7; i++)
			 m_Valid_SV_Mean[m_ActualEnsemble].criteria[i] = 0;
	 }
}
float SelectedEnsembles::GetMaxValidAll(unsigned long ip, unsigned long ind_split)
{
		if(m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_MAX].size()<=ip) return 0.;
		if (m_NumValid == 0) return MISSING;
		if(!IsMaxUpdated) {
			float v=float(MISSING);
			if(ind_split==0) {
				unsigned long ind, ind_rmse;
				float rmse_o, vtest;
				unsigned long ind_a1=2*m_NumValid/6+ip%(m_NumValid/6);
				if(ip>=m_NumValid) {
					ind=ip-m_NumValid;
					ind_rmse=4*m_NumValid/6+ind;
					rmse_o=m_pNewMap->m_ValidationData.GetValRMSE_O(ind)/m_pNewMap->m_ValidationData.GetValn(ind);
				}
				for(unsigned long i=0;i<m_TotalNumberRuns;i++) {
					if(ip<m_NumValid) { 
						if(GetValid(i,ip)>v/*&&GetValid(i, ind_a1)!=0*/)
							v=GetValid(i,ip);
					}
					else {
						vtest=1-GetValid(i, ind_rmse)*GetValid(i, ind_rmse)/rmse_o;
						if(vtest>v/*&&GetValid(i, ind_a1)!=0*/)
							v=vtest;

					}
				}
					//m_VarValidMaxAll[ip]=v;
					m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_MAX][ip] = v;
					return v;
			}
			else {
				for(unsigned long i=0;i<m_TotalNumberRuns;i++) {
					if(GetSplitValid(i,ip, ind_split-1)>v)
						v=GetSplitValid(i,ip, ind_split-1);						
				}



			//	m_VarValidMaxAll[ip+ind_split*(m_NumValid+m_NumExtraValid)]=v;
				m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_MAX][ip + ind_split*(m_NumValid + m_NumExtraValid)] = v;
				return v;
			}
		}
		else
			return m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_MAX][ip + ind_split*(m_NumValid + m_NumExtraValid)];


}
float SelectedEnsembles::GetMeanValidAll(unsigned long ip, unsigned long ind_split)
{
	if(m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_MEAN].size()<=ip) return 0.;
	unsigned long count=0;
	if(!IsMeanUpdated) {
		float v=1.1E38f;
		double sum=0.;
		if(ind_split==0) {
			unsigned long ind, ind_rmse;
			float rmse_o;
			unsigned long ind_a1=2*m_NumValid/6+ip%(m_NumValid/6);
			if(ip>=m_NumValid) {
				ind=ip-m_NumValid;
				ind_rmse=4*m_NumValid/6+ind;
				rmse_o=m_pNewMap->m_ValidationData.GetValRMSE_O(ind)/m_pNewMap->m_ValidationData.GetValn(ind);
			}
			for(unsigned long i=0;i<m_TotalNumberRuns;i++) {
				//if(GetValid(i, ind_a1)!=0) {
					count++;
					if(ip<m_NumValid) 
						sum+=GetValid(i,ip);
					else
						sum+=1-GetValid(i, ind_rmse)*GetValid(i, ind_rmse)/rmse_o;
				//}
			}
			//m_VarValidMeanAll[ip+ind_split*(m_NumValid+m_NumExtraValid)]=float(sum/count);
			m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_MEAN][ip + ind_split*(m_NumValid + m_NumExtraValid)]= float(sum / count);
			return m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_MEAN][ip + ind_split*(m_NumValid + m_NumExtraValid)] = float(sum / count);
		}
		else {
			double sum=0.;
			unsigned long count=0;
			float value;

			for(unsigned long i=0;i<m_TotalNumberRuns;i++) {
				value=GetSplitValid(i,ip,  ind_split-1);
				if(abs(value)<0.9E38) {
					sum+=value;
					count++;
				}
			}
			if(count>0) 
				m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_MEAN][ip + ind_split*(m_NumValid + m_NumExtraValid)] = float(sum / count);

			else
				m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_MEAN][ip + ind_split*(m_NumValid + m_NumExtraValid)]= float(MISSING);

		}
	}
	else
		return m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_MEAN][ip + ind_split*(m_NumValid + m_NumExtraValid)];
	return MISSING;
}
float SelectedEnsembles::GetMeanValid(unsigned long ip, unsigned long ind_split)
{
	if(m_Valid_TSV[m_ActualEnsemble].v[E_POST_MEAN].size()<=ip) return 0.;
	if(!IsMeanUpdated) {
		float v=1.1E38f;
		double sum=0.;
		if(ind_split==0) {
			unsigned long ind, ind_rmse, count;
			float rmse_o;
			if(ip>=m_NumValid) {
				ind=ip-m_NumValid;
				ind_rmse=4*m_NumValid/6+ind;
				rmse_o=m_pNewMap->m_ValidationData.GetValRMSE_O(ind)/m_pNewMap->m_ValidationData.GetValn(ind);
			}
			float fval;
			count=0;
			for(unsigned long i=0;i<m_TotalNumAccepted;i++) {
				unsigned long irun=Get_AcceptedRuns(i);
				if(ip<m_NumValid) 
					fval=GetValid(irun,ip);
				else
					fval=MISSING;
				if(ip<m_NumValid&&fval>MISSING) {
					sum+=GetValid(irun,ip);
					count++;
				}
				else if(ip>=m_NumValid) {
					if(GetValid(irun, ind_rmse)>MISSING) {
						sum+=1-GetValid(irun, ind_rmse)*GetValid(irun, ind_rmse)/rmse_o;
						count++;
					}
				}
			}
			if(count>0) 
				m_Valid_TSV[m_ActualEnsemble].v[E_POST_MEAN][ip]=float(sum/m_TotalNumAccepted);
			else
				m_Valid_TSV[m_ActualEnsemble].v[E_POST_MEAN][ip]=float(MISSING);
		}
		else {
			double sum=0.;
			unsigned long count=0;
			float value;
			for(unsigned long i=0;i<m_TotalNumAccepted;i++) {
				unsigned long irun=Get_AcceptedRuns(i);
				value=GetSplitValid(irun,ip,  ind_split-1);
				if(abs(value)<0.9E38) {
					sum+=value;
					count++;
				}
				
			}
			if(count>0)
				m_Valid_TSV[m_ActualEnsemble].v[E_POST_MEAN][ip+ind_split*(m_NumValid+m_NumExtraValid)]=float(sum/count);
			else
				m_Valid_TSV[m_ActualEnsemble].v[E_POST_MEAN][ip+ind_split*(m_NumValid+m_NumExtraValid)]=float(MISSING);

		}
		return m_Valid_TSV[m_ActualEnsemble].v[E_POST_MEAN][ip+ind_split*(m_NumValid+m_NumExtraValid)];
	}
	else
		return m_Valid_TSV[m_ActualEnsemble].v[E_POST_MEAN][ip+ind_split*(m_NumValid+m_NumExtraValid)];
}
float SelectedEnsembles::GetCV_Valid(unsigned long ip, unsigned long ind_split)
{
	if(m_Valid_TSV[m_ActualEnsemble].v[E_POST_CV].size() <=ip) return 0.;
	if (m_NumValid == 0) return MISSING;
	if (!IsCVUpdated) {
		float v = 1.1E38f;
		double sum = 0.;
		unsigned long count = 0;
		if (ind_split == 0) {
			unsigned long ind, ind_rmse;
			float rmse_o;
			unsigned long ind_a1 = 2 * m_NumValid / 6 + ip % (m_NumValid / 6);
			if (ip >= m_NumValid) {
				ind = ip - m_NumValid;
				ind_rmse = 4 * m_NumValid / 6 + ind;
				rmse_o = m_pNewMap->m_ValidationData.GetValRMSE_O(ind) / m_pNewMap->m_ValidationData.GetValn(ind);
			}

			for (unsigned long i = 0; i < m_TotalNumAccepted; i++) {
				unsigned long irun = Get_AcceptedRuns(i);
				if (irun < 0) return MISSING;
				//if(GetValid(i, ind_a1)!=0) {
				count++;
				if (ip < m_NumValid)
					sum += pow(GetValid(irun, ip) - GetMeanValid(ip), 2);
				else
					sum += pow((1 - GetValid(irun, ind_rmse)*GetValid(irun, ind_rmse) / rmse_o) - GetMeanValid(ip), 2);
				//}
			}
			if (count > 2 && abs(GetMeanValid(ip))>0) {
				//m_VarValidCV[ip] = float(sqrt(sum / (count - 1))) / abs(GetMeanValid(ip));
				m_Valid_TSV[m_ActualEnsemble].v[E_POST_CV][ip] = float(sqrt(sum / (count - 1))) / abs(GetMeanValid(ip));

			}
			else {
				m_Valid_TSV[m_ActualEnsemble].v[E_POST_CV][ip] = float(sqrt(sum / (count - 1))) / abs(GetMeanValid(ip));

				//m_VarValidCV[ip] = float(MISSING);
			}

			return m_Valid_TSV[m_ActualEnsemble].v[E_POST_CV][ip];
		}
		else {
			unsigned long ind_a1 = 2 * m_NumValid / 6 + ip % (m_NumValid / 6);
			for (unsigned long i = 0; i < m_TotalNumAccepted; i++) {
				unsigned long irun = Get_AcceptedRuns(i);
				if (GetSplitValid(i, ind_a1, ind_split - 1) != 0) {
					count++;
					sum += pow(GetSplitValid(irun, ip, ind_split - 1) - GetMeanValid(ip, ind_split), 2);
				}
			}
			if (count > 2 && abs(GetMeanValid(ip, ind_split)) > 0)
				m_Valid_TSV[m_ActualEnsemble].v[E_POST_CV][ip + ind_split*(m_NumValid + m_NumExtraValid)]= float(sqrt(sum / (count - 1)) / abs(GetMeanValid(ip, ind_split)));
			else
				m_Valid_TSV[m_ActualEnsemble].v[E_POST_CV][ip + ind_split*(m_NumValid + m_NumExtraValid)] = float(MISSING);




		}
	}
	else
		return m_Valid_TSV[m_ActualEnsemble].v[E_POST_CV][ip + ind_split*(m_NumValid + m_NumExtraValid)];
		//return m_VarValidCV[ip+ind_split*(m_NumValid+m_NumExtraValid)];
	return MISSING;
}
float SelectedEnsembles::GetCV_ValidAll(unsigned long ip, unsigned long ind_split)
{
	if(m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_CV].size() <=ip) return 0.;
	if (m_NumValid == 0) return MISSING;
	if(!IsCVUpdated) {
		double sum=0.;
		unsigned long count=0;
		if(ind_split==0) {
			unsigned long ind, ind_rmse;
			float rmse_o;
			unsigned long ind_a1=2*m_NumValid/6+ip%(m_NumValid/6);
			if(ip>=m_NumValid) {
				ind=ip-m_NumValid;
				ind_rmse=4*m_NumValid/6+ind;
				rmse_o=m_pNewMap->m_ValidationData.GetValRMSE_O(ind)/m_pNewMap->m_ValidationData.GetValn(ind);
			}
			for(unsigned long i=0;i<m_TotalNumberRuns;i++) {
				//if(GetValid(i, ind_a1)!=0) {
					count++;
					if(ip<m_NumValid)
						sum+=pow(GetValid(i,ip)-GetMeanValidAll(ip),2);
					else
						sum+=pow((1-GetValid(i, ind_rmse)*GetValid(i, ind_rmse)/rmse_o)-GetMeanValid(ip),2);
				//}
			}
			if(count>2&&abs(GetMeanValidAll(ip))>0)
				m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_CV][ip]=float(sqrt(sum/(count-1)))/abs(GetMeanValidAll(ip));
			else
				m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_CV][ip]=0.f;

			return m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_CV][ip];
		}
		else {
			unsigned long ind_a1=2*m_NumValid/6+ip%(m_NumValid/6);
			for(unsigned long i=0;i<m_TotalNumberRuns;i++) {
				if(GetSplitValid(i, ind_a1,ind_split-1)!=0) {
					count++;
					sum+=pow(GetSplitValid(i,ip,  ind_split-1)-GetMeanValid(ip, ind_split),2);
				}
			}
			if(count>2&&abs(GetMeanValid(ip, ind_split))>0)
				m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_CV][ip+ind_split*(m_NumValid+m_NumExtraValid)]=float(sqrt(sum/(count-1)))/abs(GetMeanValid(ip, ind_split));
			else
				m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_CV][ip+ind_split*(m_NumValid+m_NumExtraValid)]=float(MISSING);



		}
	}
	else
		return m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_CV][ip+ind_split*(m_NumValid+m_NumExtraValid)];
	return MISSING;
}
float SelectedEnsembles::GetMinValid(unsigned long ip, unsigned long ind_split)
{
	if(m_Valid_TSV[m_ActualEnsemble].v[E_POST_MIN].size()<=ip) return 0.;
	if(!IsMinUpdated) {
		float fval;
		float v=1.1E38f;
		if (m_NumValid == 0) return MISSING;
		if(ind_split==0) {
			unsigned long ind, ind_rmse, ind_a1;
			float rmse_o, vtest;
			
			if(ip>=m_NumValid) {
				ind=ip-m_NumValid;
				ind_rmse=4*m_NumValid/6+ind;
				rmse_o=m_pNewMap->m_ValidationData.GetValRMSE_O(ind)/m_pNewMap->m_ValidationData.GetValn(ind);
			}
			
			ind_a1=2*m_NumValid/6+ip%(m_NumValid/6);

			for(unsigned long i=0;i<m_TotalNumAccepted;i++) {
				unsigned long irun=Get_AcceptedRuns(i);
				if(irun<0) return MISSING;
				if(ip<m_NumValid) { 
					fval=GetValid(irun,ip);
					if(fval<v&&fval>MISSING/*&&GetValid(irun, ind_a1)!=0*/)
						v=fval;
				}
				else {

					vtest=1-GetValid(irun, ind_rmse)*GetValid(irun, ind_rmse)/rmse_o;
					if(vtest<v/*&&GetValid(irun, ind_a1)!=0*/)
						v=vtest;
				}
			}
			//m_VarValidMin[ip]=v;
			m_Valid_TSV[m_ActualEnsemble].v[E_POST_MIN][ip] = v;
			return v;
		}
		else {
			for(unsigned long i=0;i<m_TotalNumAccepted;i++) {
				unsigned long irun=Get_AcceptedRuns(i);	
				fval=GetSplitValid(irun,ip, ind_split-1);
				if(fval<v&&fval>MISSING)
						v=fval;			
			
			}
			//m_VarValidMin[ip+ind_split*(m_NumValid+m_NumExtraValid)]=v;
			m_Valid_TSV[m_ActualEnsemble].v[E_POST_MIN][ip] = v;
			return v;

		}

	}
	else 
		return m_Valid_TSV[m_ActualEnsemble].v[E_POST_MIN][ip]; 
	//m_VarValidMin[ip+ind_split*(m_NumValid+m_NumExtraValid)];


}
float SelectedEnsembles::GetMaxValid(unsigned long ip, unsigned long ind_split)
{
		if(m_Valid_TSV[m_ActualEnsemble].v[E_POST_MAX].size()<=ip) return 0.;
		if (m_NumValid == 0) return MISSING;
		if(!IsMaxUpdated) {
			float v=float(MISSING);
			if(ind_split==0) {
				unsigned long ind, ind_rmse,  ind_a1;
				float rmse_o, vtest;
				if(ip>=m_NumValid) {
					ind=ip-m_NumValid;
					ind_rmse=4*m_NumValid/6+ind;
					rmse_o=m_pNewMap->m_ValidationData.GetValRMSE_O(ind)/m_pNewMap->m_ValidationData.GetValn(ind);
				}
				ind_a1=2*m_NumValid/6+ip%(m_NumValid/6);
				for(unsigned long i=0;i<m_TotalNumAccepted;i++) {
					unsigned long irun=Get_AcceptedRuns(i);
					if(irun<0) return MISSING;
					if(ip<m_NumValid) { 
						if(GetValid(irun,ip)>v/*&&GetValid(i, ind_a1)!=0*/)
							v=GetValid(irun,ip);
					}
					else {
						vtest=1-GetValid(irun, ind_rmse)*GetValid(irun, ind_rmse)/rmse_o;
						if(vtest>v)
							v=vtest;

					}
				}
					//m_VarValidMax[ip]=v;
					m_Valid_TSV[m_ActualEnsemble].v[E_POST_MAX][ip]=v;
					return v;
			}
			else {
				for(unsigned long i=0;i<m_TotalNumAccepted;i++) {
					unsigned long irun=Get_AcceptedRuns(i);	
					if(GetSplitValid(irun,ip, ind_split-1)>v)
							v=GetSplitValid(irun,ip, ind_split-1);			
				
				}
				//m_VarValidMax[ip+ind_split*(m_NumValid+m_NumExtraValid)]=v;
				m_Valid_TSV[m_ActualEnsemble].v[E_POST_MAX][ip]=v;
				return v;
			}
		}
		else
			return m_Valid_TSV[m_ActualEnsemble].v[E_POST_MAX][ip];
		//	m_VarValidMax[ip+ind_split*(m_NumValid+m_NumExtraValid)];


}
size_t SelectedEnsembles::FindAcceptedRunsFromFlagInPar() {

	m_TotalNumAccepted = 0;
	m_Accepted.resize(0);
	m_AcceptedRuns[m_ActualEnsemble].resize(0);
	m_AcceptedUpdated = true;

	for (unsigned long i = 0; i<m_TotalNumberRuns; i++) {

		if (GetAcceptedRunNo(i)) {
			m_AcceptedRuns[m_ActualEnsemble].push_back(i);
		}
	}
	m_TotalNumAccepted = m_AcceptedRuns[m_ActualEnsemble].size();
	return m_TotalNumAccepted;
}
void SelectedEnsembles::SetMinMaxOfPerformanceIndicators() {
	IsMinUpdated = false;
	IsMaxUpdated = false;
	IsCVUpdated = false;
	IsMeanUpdated = false;
	for (unsigned long i = 0; i<m_NumMulti; i++) {
		GetMinPar(i);
		GetMaxPar(i);
		GetMinValidPar(i);
		GetMaxValidPar(i);

	}
	for (unsigned long split = 0; split <= m_NumSplitPeriod; split++) {
		for (unsigned long i = 0; i<m_NumValid + m_NumExtraValid; i++) {
			GetMinValid(i, split);
			GetMinValidAll(i, split);
			GetMaxValid(i, split);
			GetMaxValidAll(i, split);
		}
	}
	IsMinUpdated = true;
	/*IsMeanUpdated = false;
	for (unsigned long split = 0; split <= m_NumSplitPeriod; split++) {
		for (unsigned long i = 0; i<m_NumValid + m_NumExtraValid; i++) {
			GetMeanValid(i, split);
			GetMeanValidAll(i, split);
		}
	}*/

	IsMeanUpdated = true;
	for (unsigned long split = 0; split <= m_NumSplitPeriod; split++) {
		for (unsigned long i = 0; i<m_NumValid + m_NumExtraValid; i++) {
			GetCV_Valid(i, split);
			GetCV_ValidAll(i, split);
		}
	}
	IsCVUpdated = true;


	for (unsigned long i = 0; i<m_NumSumValid; i++) {
		GetMinSumValid(i);
		GetMaxSumValid(i);
	}

	Update_TSV_VarStat(false);
	Update_TSV_VarStat(true);


	IsMinUpdated = true;
	IsMaxUpdated = true;
	  //false = update of prior distribution - true =posterior
	Update_SV_VarStat(true);	//false = update of prior distribution

}
float SelectedEnsembles::GetMinSumValid(unsigned long ip)
{
	if(m_VarValidMinSum.size()<=0) return 0.;
	if(!IsMinSumUpdatedAll) {
		float v=1.1E38f;
		float value;

			for(unsigned long i=0;i<m_TotalNumAccepted;i++) {
			unsigned long irun=Get_AcceptedRuns(i);
			value = GetValidSum(irun, ip);
			if(value<v&&value>-4.3E8)
				v=value;
		}
		m_VarValidMinSum[ip]=v;
		return v;
	}
	else
		return m_VarValidMinSum[ip];


}

float SelectedEnsembles::GetMaxSumValid(unsigned long ip)
{
	if(m_VarValidMaxSum.size()<=ip) return 0.;
		if(!IsMaxSumUpdatedAll) {
			float v=float(MISSING), value;
			for(unsigned long i=0;i<m_TotalNumAccepted;i++) {
			unsigned long irun=Get_AcceptedRuns(i);
				value=GetValidSum(irun, ip);
				if(value>v)	v=value;
			}
				m_VarValidMaxSum[ip]=v;
				return v;
		}
		else
			return m_VarValidMaxSum[ip];


}
float SelectedEnsembles::GetMinSumValidAll(unsigned long ip)
{
	if(m_VarValidMinSum.size()<=0) return 0.;
	if(!IsMinSumUpdatedAll) {
		float v=1.1E38f, value;
				for(unsigned long i=0;i<m_TotalNumberRuns;i++) {
					value = GetValidSum(i, ip);
					if(value<v&&value>-4.3E8)
						v=GetValidSum(i,ip);
		}
		m_VarValidMinSum[ip]=v;
		return v;
	}
	else
		return m_VarValidMinSum[ip];


}
float SelectedEnsembles::GetMaxSumValidAll(unsigned long ip)
{
		if(m_VarValidMaxSum.size()<=ip) return 0.;
		if(!IsMaxSumUpdatedAll) {
			float v=float(MISSING), value;
			for(unsigned long i=0;i<m_TotalNumberRuns;i++) {
				value = GetValidSum(i, ip);
				if(value>v)
					v=value;
			}
			m_VarValidMaxSum[ip]=v;
			return v;
		}
		else
			return m_VarValidMaxSum[ip];


}







float SelectedEnsembles::GetMeanSumValidAll(unsigned long ip)
{
	if (m_VarValidMeanSumAll.size() == 0) return 0.;
	if (!IsMeanSumUpdatedAll) {
		float v = 0, value;
		unsigned long num = 0;

		for (unsigned long i = 0; i<m_TotalNumberRuns; i++) {
			value = GetValidSum(i, ip);
			if (value >= .9*MISSING) {
				v += value;
				num++;
			}
		}
		if (num>0) {
			v = v / num;
			m_VarValidMeanSumAll[ip] = v;
			return v;
		}
		else
			return -1.1e38f;
		IsMeanSumUpdatedAll = true;
	}
	else
		return m_VarValidMeanSumAll[ip];
}

float SelectedEnsembles::GetCVSumValidAll(unsigned long ip)
{
	if (m_VarValidCVSumAll.size() == 0) return 0.;
	if (!IsCVSumUpdatedAll) {
		double sum = 0.;
		double mean = GetMeanSumValidAll(ip);
		float testvalue;
		size_t count = 0;


		for (unsigned long i = 0; i<m_TotalNumberRuns; i++) {
			unsigned long irun = Get_AcceptedRuns(i);
			testvalue = GetValidSum(irun, ip);
			if (testvalue > 0.9*MISSING) {
				count++;
				sum += pow(GetValidSum(irun, ip) - mean, 2);
			}
		}

		if (count>2 && abs(mean)>0)
			m_VarValidCVSumAll[ip] = float(sqrt(sum / (count - 1)) / abs(mean));
		else
			m_VarValidCVSumAll[ip] = 0.;

		return m_VarValidCVSumAll[ip];
	}

	else
		return m_VarValidCVSumAll[ip];
}



bool SelectedEnsembles::SelSumV_Index_Add(unsigned long index) 
{
	m_VarSumSel.push_back(index);
	return true;
}
bool SelectedEnsembles::SelSumV_Index_SelectAll() 
{
	m_VarSumSel.clear();
	for(unsigned long i=0;i<m_NumSum;i++)
		m_VarSumSel.push_back(i);
	return true;
}
void SelectedEnsembles::SelSumV_Index_Reset() 
{
	m_VarSumSel.clear();
}
unsigned long SelectedEnsembles::SelSumV_Index_Get(unsigned long index) 
{
	if(index>=m_VarSumSel.size())
		return -1;
	else
		return m_VarSumSel[index];
}
unsigned long SelectedEnsembles::SelSumV_Index_GetNum() 
{
	return m_VarSumSel.size();
}

unsigned long SelectedEnsembles::GetNumberOfAddedFiles(string str)
{
		return m_AddedFiles.size();
}
string SelectedEnsembles::GetNameAddedFiles(unsigned long index)
{
	if(index!=string::npos&&index<m_AddedFiles.size())
		return m_AddedFiles[index];
	else
		return "";

}
string SelectedEnsembles::GetNameBaseFile()
{
	return m_FileName;

}

unsigned long SelectedEnsembles::GetMultiRunNo(unsigned long index)
{
	if(index<m_OrigRuns)
		return m_OrigFirstRunNo+index;
	unsigned long count;
	count=m_OrigRuns;
	for(unsigned long i=0; i<m_AddedFiles.size();i++) {
		if(index<count+GetNumberOfAddedRuns(i)) {
			return GetFirstRunNoAddedRuns(i)+index-count;			
		}
		count+=GetNumberOfAddedRuns(i);
	}
	return index+1;
}
unsigned long SelectedEnsembles::GetAdd_MultiPGFileIndex(unsigned long simRunNo)
{
	unsigned long index=string::npos;
	unsigned long count;
	count=m_OrigFirstRunNo;
	for(unsigned long i=0; i<m_AddedFiles.size();i++) {
		if(simRunNo>=count+GetNumberOfAddedRuns(i)) {
			index=i;
		}
	}
	return index;
}

void SelectedEnsembles::SetValidDynLogLi(unsigned long iv, unsigned long idin, unsigned long all, float *pfloat)
{
	unsigned long index;
	float k;
	for(unsigned long i=0; i<7;i++) {
		index=all*(m_NumValidVar*7*13)+iv*13*7+idin*7+i;
		if(index>=m_NumValidVar*7*13*2) {
			unsigned long koll;
			koll=1;

		}
		m_ValidDynLogLi[index]=*(pfloat+i);
		k=*(pfloat+i);
	}
}
float SelectedEnsembles::GetValidDynLogLi(unsigned long iv, unsigned long idin, unsigned long idut, unsigned long all)
{ unsigned long index;
	index=all*(m_NumValidVar*7*13)+iv*13*7+idin*7+idut;
		if(index==672) {
			unsigned long koll;
			koll=1;

		}

	return m_ValidDynLogLi[index];
}

float SelectedEnsembles::GetMeanSumValid(unsigned long ip)
{
	if(m_VarValidMeanSum.size()==0) return 0.;
	if(!IsMeanSumUpdated) {
		float v=0, value;
		unsigned long num=0;

		for(unsigned long i=0;i<m_TotalNumAccepted;i++) {
			unsigned long irun=Get_AcceptedRuns(i);
			value=GetValidSum(irun,ip);
			if (value > 0.9*MISSING) {
				v += value;
				num++;

			}
		}
		if(num>0) {
			v=v/num;
			m_VarValidMeanSum[ip]=v;
			return v;
		}
		else
			return -1.1e38f;
		IsMeanSumUpdated=true;
	}
	else 
		return m_VarValidMeanSum[ip];
}

float SelectedEnsembles::GetCVSumValid(unsigned long ip)
{
	if(m_VarValidCVSum.size()==0) return 0.;
	if(!IsCVSumUpdated) {
		double sum=0.;
		double mean=GetMeanSumValid(ip);
		float testvalue;
		size_t count=0;


		for (unsigned long i = 0; i < m_TotalNumAccepted; i++) {
			unsigned long irun = Get_AcceptedRuns(i);
			testvalue = GetValidSum(irun, ip);
			if (testvalue > 0.9*MISSING) {
				count++;
				sum += pow(GetValidSum(irun, ip) - mean, 2);
			}
		}
		if(count>2&&abs(mean)>0)
			m_VarValidCVSum[ip]=float(sqrt(sum/(count-1))/abs(mean));
		else
			m_VarValidCVSum[ip]=0.;

		return m_VarValidCVSum[ip];
	}

	else
		return m_VarValidCVSum[ip];
}


void SelectedEnsembles::Update_SV_VarStat(bool AcceptedOnly) {
	VALSUM valsum;
#ifndef COUPSTD
	MFC_Util mfc_util;
#endif
	SimpleDistStat stat;
	size_t irun;
	size_t icount;
	bool GoodResults = false;

	size_t num_runs = m_TotalNumberRuns;
	if(AcceptedOnly) num_runs= m_TotalNumAccepted;
	vector<vector<float>> Performance;
	Performance.resize(7 * m_pNewMap->GetNumberofSumVariables());
	for (size_t i = 0; i < 7 * m_pNewMap->GetNumberofSumVariables(); i++) {
		Performance[i].resize(num_runs);
	}



	auto MakeStat = [&](  bool posterior) {
		vector<float> values;
		size_t shift_pos = 0;
		float candidate;
		size_t num_runs = m_TotalNumberRuns;
		if (posterior) {
			shift_pos = 4;
			num_runs = m_TotalNumAccepted;
			m_pNewMap->Get_SV_Value_Accepted(Performance);
			if (num_runs == m_TotalNumberRuns) {
					for (size_t i = 0; i < m_pNewMap->m_ValSum_Array.size(); i++)
						for (size_t jx = 0; jx < 7; jx++) {
							size_t index = jx*m_pNewMap->GetNumberofSumVariables() + i;
							m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MIN + shift_pos][index] = m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MIN][index];
							m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MAX + shift_pos][index] = m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MAX][index];
							m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MEAN + shift_pos][index] = m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MEAN][index];
							m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_CV + shift_pos][index] = m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_CV][index];
							m_Valid_SV[m_ActualEnsemble].v[E_SELECT_LOW][index] = m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MIN][index];
							m_Valid_SV[m_ActualEnsemble].v[E_SELECT_HIGH][index] = m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MAX][index];

						}
					return;
				}
		}		
		else {
			m_pNewMap->Get_SV_Value_All(Performance);
		}


		for (size_t i = 0; i < m_pNewMap->m_ValSum_Array.size(); i++) {
#ifndef COUPSTD
			mfc_util.SetPaneMessage("Update SV Variable Posterior Statistics:" + FUtil::STD_ItoAscii(num_runs) + " Runs - " + FUtil::STD_ItoAscii(i + 1) + "(" + FUtil::STD_ItoAscii(m_pNewMap->GetNumberofSumVariables()) + ")");
		
			if(!posterior) mfc_util.SetPaneMessage("Update SV Variable Prior Statistics:" + FUtil::STD_ItoAscii(num_runs) + " Runs - " + FUtil::STD_ItoAscii(i + 1) + "(" + FUtil::STD_ItoAscii(m_pNewMap->GetNumberofSumVariables()) + ")");
#endif	
			for (size_t jx = 0; jx < 7; jx++) {
				size_t index = jx*m_pNewMap->GetNumberofSumVariables() + i;
			
				SStat(&Performance[index][0], Performance[index].size(), &icount, &stat);			
				if (stat.Min < stat.Max) GoodResults = true;

				
					if (!posterior&&GoodResults) {
						for (size_t iens = 0; iens < m_Valid_SV.size(); iens++) {
							m_Valid_SV[iens].v[E_PRIOR_MIN][index] = stat.Min;
							m_Valid_SV[iens].v[E_PRIOR_MAX][index] = stat.Max;
							m_Valid_SV[iens].v[E_PRIOR_MEAN][index] = stat.Mean;
							if (stat.Mean != 0)
								m_Valid_SV[iens].v[E_PRIOR_CV][index] = stat.Std / stat.Mean;
						}
					}
					else {

						m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MIN + shift_pos][index] = stat.Min;
						m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MAX + shift_pos][index] = stat.Max;
						m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MEAN + shift_pos][index] = stat.Mean;
						if (stat.Mean != 0)
							m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_CV + shift_pos][index] = stat.Std / stat.Mean;
					}
			}

		}
	};
	auto MakeMeanStat = [&]( bool posterior) {
		vector<float> values;
		size_t shift_pos = 0;
		float candidate;
		size_t num_runs = m_TotalNumberRuns;
		if (posterior) {
			shift_pos = 4;
			num_runs = m_TotalNumAccepted;
#ifndef COUPSTD
			mfc_util.SetPaneMessage("Update SV Mean of Variable Posterior Statistics:" + FUtil::STD_ItoAscii(num_runs) + " Runs ");
#endif
			if (num_runs == m_TotalNumberRuns) {
				for (size_t jx = 0; jx < 7; jx++) {
						m_Valid_SV_Mean[m_ActualEnsemble].v[E_PRIOR_MIN + shift_pos][jx] = m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MIN][jx];
						m_Valid_SV_Mean[m_ActualEnsemble].v[E_PRIOR_MAX + shift_pos][jx] = m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MAX][jx];
						m_Valid_SV_Mean[m_ActualEnsemble].v[E_PRIOR_MEAN + shift_pos][jx] = m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MEAN][jx];
						m_Valid_SV_Mean[m_ActualEnsemble].v[E_PRIOR_CV + shift_pos][jx] = m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_CV][jx];
						m_Valid_SV_Mean[m_ActualEnsemble].v[E_SELECT_LOW][jx] = m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MIN][jx];
						m_Valid_SV_Mean[m_ActualEnsemble].v[E_SELECT_HIGH][jx] = m_Valid_SV[m_ActualEnsemble].v[E_PRIOR_MAX][jx];
				}
				return;
			}

		}
		else {
#ifndef COUPSTD
			mfc_util.SetPaneMessage("Update SV Mean of Variable Prior Statistics:" + FUtil::STD_ItoAscii(num_runs) + " Runs ");
#endif
		}
		for (size_t jx = 0; jx < 7; jx++) {
			for (unsigned long i = 0; i < num_runs; i++) {
				if (posterior) irun = Get_AcceptedRuns(i);
				else irun = i;
				candidate = m_Valid_SV_Mean[0].MeanPerFomanceIndicators[jx][irun];
				values.push_back(candidate);
			}
			if (values.size() > 0) {
				SStat(&values[0], values.size(), &icount, &stat);
				size_t index;
				if (stat.Min < stat.Max) GoodResults = true;

				index =jx;

				if (!posterior&&GoodResults) {
					for (size_t iens = 0; iens < m_Valid_SV.size(); iens++) {
						m_Valid_SV_Mean[iens].v[E_PRIOR_MIN][index] = stat.Min;
						m_Valid_SV_Mean[iens].v[E_PRIOR_MAX][index] = stat.Max;
						m_Valid_SV_Mean[iens].v[E_PRIOR_MEAN][index] = stat.Mean;
						if (stat.Mean != 0)
							m_Valid_SV_Mean[iens].v[E_PRIOR_CV][index] = stat.Std / stat.Mean;
					}
				}
				else {

					m_Valid_SV_Mean[m_ActualEnsemble].v[E_PRIOR_MIN + shift_pos][index] = stat.Min;
					m_Valid_SV_Mean[m_ActualEnsemble].v[E_PRIOR_MAX + shift_pos][index] = stat.Max;
					m_Valid_SV_Mean[m_ActualEnsemble].v[E_PRIOR_MEAN + shift_pos][index] = stat.Mean;
					if (stat.Mean != 0)
						m_Valid_SV_Mean[m_ActualEnsemble].v[E_PRIOR_CV + shift_pos][index] = stat.Std / stat.Mean;
				}
			}
			values.resize(0);
		}
	};










	if (!m_pNewMap->Are_SV_Values_Defined()) return;
	if (!AcceptedOnly && !m_SV_StatMemory.AllRuns) {
		GoodResults = false;
		if (m_Valid_SV[m_ActualEnsemble].v[0].size() == 0) return;
		MakeStat( false);
		MakeMeanStat(false);

		if(GoodResults) m_SV_StatMemory.AllRuns = true;
	}
	if (!m_SV_StatMemory.AcceptedRuns) {//Posterior
		GoodResults = false;
		if (m_Valid_SV[m_ActualEnsemble].v[0].size() == 0) return;
		MakeStat(true);
		MakeMeanStat(true);
		if (GoodResults) m_SV_StatMemory.AcceptedRuns = true;


	}

}
void SelectedEnsembles::Update_TSV_VarStat(bool AcceptedOnly) {

	SimpleDistStat stat;
	size_t irun;
	size_t icount;
	if (m_NumValid == 0) return;

	auto MakeStat = [&](size_t ip, size_t ind_split, bool posterior) {
		vector<float> values;
		size_t shift_pos = 0;
		float candidate;

		size_t num_runs = m_TotalNumberRuns;
		if (posterior) {
			shift_pos = 4;
			num_runs = m_TotalNumAccepted=m_AcceptedRuns[m_ActualEnsemble].size();
		}
		size_t index_trans=string::npos;
		if (m_Valid_TSV[m_ActualEnsemble].VarTranformed > 0) {
			
			auto it = MapOfStoredTSV.find(m_Valid_TSV[m_ActualEnsemble].VarTranformed);
			if (it != MapOfStoredTSV.end()) 
				index_trans = (*it).second;

			if (m_VarValidTransformed.size() == 0)
				index_trans = string::npos;
			else if (m_VarValidTransformed[index_trans].cumvalue != m_Valid_TSV[m_ActualEnsemble].VarTranformed)
				index_trans = string::npos;

			
		}
		if (ind_split == 0) {
			unsigned long ind, ind_rmse, ind_a1;
			float rmse_o, vtest;


			if (ip >= m_NumValid) {
				ind = ip - m_NumValid;
				ind_rmse = 4 * m_NumValid / 6 + ind;
				rmse_o = m_pNewMap->m_ValidationData.GetValRMSE_O(ind) / m_pNewMap->m_ValidationData.GetValn(ind);
			}
			ind_a1 = 2 * m_NumValid / 6 + ip % (m_NumValid / 6);
			for (unsigned long i = 0; i < num_runs; i++) {
				if(posterior) irun= Get_AcceptedRuns(i);
				else irun = i;
				if (ip < m_NumValid) {
						if(index_trans!=string::npos) {
							size_t indextsv = ip + irun*m_NumValid;
							candidate = m_VarValidTransformed[index_trans].VarValidVector[indextsv];
						}
						else
							candidate = GetValid(irun, ip);
		
				}
				else {
					if (rmse_o != 0) {
						if (index_trans != string::npos) {
							size_t indextsv = ind_rmse + irun*m_NumValid;
							candidate = 1 - m_VarValidTransformed[index_trans].VarValidVector[indextsv];
						}
						else
							candidate = 1 - GetValid(irun, ind_rmse)*GetValid(irun, ind_rmse) / rmse_o;
					}
					else
						candidate = MISSING;
				}
				values.push_back(candidate);
			}


		}
		else {
			for (unsigned long i = 0; i < num_runs; i++) {
				if (posterior) irun = Get_AcceptedRuns(i);
				if (index_trans != string::npos) {
					size_t indextsv = ip + irun*m_NumValid;
					candidate = m_VarValidTransformed[index_trans].VarValidVector[indextsv];
				}
				else
					candidate = GetSplitValid(irun, ip, ind_split - 1);
				values.push_back(candidate);
			}
			
		}
		if (values.size() > 0) {
			SStat(&values[0], values.size(), &icount, &stat);

			m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_MIN + shift_pos][ip] = stat.Min;
			m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_MAX + shift_pos][ip] = stat.Max;
			m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_MEAN + shift_pos][ip] = stat.Mean;
			if (stat.Mean != 0)
				m_Valid_TSV[m_ActualEnsemble].v[E_PRIOR_CV + shift_pos][ip] = stat.Std / stat.Mean;
		}
	};



	for (unsigned long isplit = 0; isplit <= m_NumSplitPeriod; isplit++) {
		if (!AcceptedOnly) for (unsigned long iv = 0; iv < m_NumValid + m_NumExtraValid; MakeStat(iv, isplit, false), iv++);
		for (unsigned long iv = 0; iv < m_NumValid + m_NumExtraValid; MakeStat(iv, isplit, true), iv++);
	}

}

vector<float> SelectedEnsembles::GetPost_SV_ValidPerformanceIndicatorVector(size_t ivar, enum PERFORMANCE_VAR var) {
	float fval;
	vector<float> out;





	for (unsigned long i = 0; i < m_AcceptedRuns[m_ActualEnsemble].size(); i++) {
		unsigned long irun = Get_AcceptedRuns(i);
		if (ivar == m_pNewMap->m_ValSum_Array.size())
			fval = m_Valid_SV_Mean[0].MeanPerFomanceIndicators[var][irun];
		else if(ivar < m_pNewMap->m_ValSum_Array.size())
			fval = m_pNewMap->Get_SV_Value(size_t(var), ivar, irun);
		if(fval>MISSING)
			out.push_back(fval);
	}
	
	return out;
}

vector<float> SelectedEnsembles::GetPost_TSV_ValidPerformanceIndicatorVector(unsigned long ip, unsigned long ind_split) {
	float v = 1.1E38f;
	vector<float> out;
	size_t index_trans = string::npos;
	if (m_Valid_TSV[m_ActualEnsemble].VarTranformed > 0) {

		auto it = MapOfStoredTSV.find(m_Valid_TSV[m_ActualEnsemble].VarTranformed);
		if (it != MapOfStoredTSV.end())
			index_trans = (*it).second;

		if (m_VarValidTransformed.size() == 0)
			index_trans = string::npos;
		else if (m_VarValidTransformed[index_trans].cumvalue != m_Valid_TSV[m_ActualEnsemble].VarTranformed)
			index_trans = string::npos;


	}

	m_TotalNumAccepted = m_AcceptedRuns[m_ActualEnsemble].size();

	if (ind_split == 0) {
		unsigned long ind, ind_rmse, count;
		float rmse_o;
		if (ip >= m_NumValid) {
			ind = ip - m_NumValid;
			ind_rmse = 4 * m_NumValid / 6 + ind;
			rmse_o = m_pNewMap->m_ValidationData.GetValRMSE_O(ind) / m_pNewMap->m_ValidationData.GetValn(ind);
		}
		float fval;
		count = 0;
		for (unsigned long i = 0; i < m_TotalNumAccepted; i++) {
			unsigned long irun = Get_AcceptedRuns(i);

			fval = MISSING;
			if (ip < m_NumValid) {
				if (index_trans != string::npos) {
					size_t indextsv = ip + irun*m_NumValid;
					fval = m_VarValidTransformed[index_trans].VarValidVector[indextsv];
				}
				else
					fval = GetValid(irun, ip);
			}
			else if (rmse_o != 0) {
				float vtest;
				if (index_trans != string::npos) {
					size_t indextsv = ind_rmse + irun*m_NumValid;
					vtest = 1- m_VarValidTransformed[index_trans].VarValidVector[indextsv];
				}
				else
					vtest = 1. - GetValid(irun, ind_rmse)*GetValid(irun, ind_rmse) / rmse_o;
				if (vtest > MISSING&&vtest < 1.00001)
					fval = float(vtest);
			}
			if (fval > MISSING)
				out.push_back(fval);
		}
	}
	return out;
}
bool SelectedEnsembles::RemoveAddedFiles() {
	if (m_AddedFiles.size() > 0) {
		m_AddedFiles.clear();
		m_AddedNumRuns.clear();
		m_AddedFirstRunNo.clear();
		m_TotalNumberRuns = Get_And_UpdateTotalNumberofRuns();
		m_pNewMap->ResidualFile_SetNumAttachedFiles(0);
		return true;
	}
	return false;
}
bool SelectedEnsembles::RemoveAddedFile(size_t index) {
	if (index < m_AddedFiles.size()) {
		m_AddedFiles.erase(m_AddedFiles.begin() + index);
		m_AddedNumRuns.erase(m_AddedNumRuns.begin() + index);
		m_AddedFirstRunNo.erase(m_AddedFirstRunNo.begin() + index);
		m_TotalNumberRuns = Get_And_UpdateTotalNumberofRuns();
		m_pNewMap->ResidualFile_RemoveFile(index);
		return true;
	}
	return false;
}
bool SelectedEnsembles::AttachNewFiles() {
	vector<string> filenames;

#ifndef COUPSTD
	filenames = MFC_Util::GetMBinFileNames();
#endif
	fstream mbinstream;
	MBIN_FILE_HEADER header;
	size_t run_no;
	
	for (size_t i = 0; i < filenames.size(); i++) {
		mbinstream.open(filenames[i], ios::binary | ios::in);
		header = ReadStartDataFromAttachedFiles(&mbinstream, i);
		if (header.n_param == m_MBinHeader.n_param&&header.n_sumvalid == m_MBinHeader.n_sumvalid&&
			header.n_valid == m_MBinHeader.n_valid&&header.n_sumvar == m_MBinHeader.n_sumvar) {
			m_AddedFiles.push_back(filenames[i]);
			if (m_AddedFirstRunNo.size() > 0)
				run_no = m_AddedFirstRunNo[m_AddedFirstRunNo.size() - 1] +
				m_AddedNumRuns[m_AddedFirstRunNo.size() - 1];
			else
				run_no = m_pNewMap->m_DocFile.m_SimulationRunNo + m_MBinHeader.n_runs;
			m_AddedFirstRunNo.push_back(run_no);
			m_AddedNumRuns.push_back(header.n_runs);
			m_pNewMap->ResidualFile_AddAttachedFiles(1);
		}
		else
			return false;

		mbinstream.close();
	}
	m_TotalNumberRuns = Get_And_UpdateTotalNumberofRuns();
	return true;
}
bool SelectedEnsembles::Set_Update_ValidVar() {

	SetMinMaxOfPerformanceIndicators();
	UpdateMeanValidValues();
	UpdateMeanValidValues(true);
	
	if (!m_Lock) {
		for (unsigned long i = 0; i<m_TotalNumberRuns; i++)
			SetAcceptedRunNo(i, false);
		for (unsigned long i = 0; i<m_TotalNumAccepted; i++) {
			unsigned long irun = Get_AcceptedRuns(i);
			SetAcceptedRunNo(irun, true);
		}
	}

	SetValid_TSV_Criteria();
	if (m_pNewMap->GetNumberofSumVariables() > 0)	SetValid_SUM_Criteria();
	return true;
}
void SelectedEnsembles::SetValid_TSV_Criteria() {
if (m_Valid_TSV.size() == 0) m_Valid_TSV.resize(1);
if (m_NumValidVar <= 0) return;
  

for (size_t i = 0; i < 7; i++) {
	m_MeanTSV_STAT[SELECT_LOW][i] = m_MeanTSV_STAT[PRIOR_MIN][i];
	m_MeanTSV_STAT[SELECT_HIGH][i] = m_MeanTSV_STAT[PRIOR_MAX][i];
}

	for (size_t i = 0;i<7; i++) {
		m_Valid_TSV[m_ActualEnsemble].criteria[i]= NO_CRITERIA;
		bool MinCrit, MaxCrit;
		MinCrit = MaxCrit = false;

		if (m_MeanTSV_STAT[SELECT_LOW][i] > m_MeanTSV_STAT[PRIOR_MIN][i]) MinCrit = true;
		if (m_MeanTSV_STAT[SELECT_HIGH][i] < m_MeanTSV_STAT[PRIOR_MAX][i]) MaxCrit = true;

		if (MinCrit&&MaxCrit)
			m_Valid_TSV[m_ActualEnsemble].meancriteria[i] = ABOVE_AND_BELOW_CRITERIA;
		else if (MinCrit)
			m_Valid_TSV[m_ActualEnsemble].meancriteria[i] = ABOVE_MIN_CRITERIA;
		else if (MaxCrit)
			m_Valid_TSV[m_ActualEnsemble].meancriteria[i] = BELOW_MAX_CRITERIA;

	}
	

}
void SelectedEnsembles::SetValid_SUM_Criteria() {

	AllocateValid_SUM(0);

	for (size_t i = 0; i < 7; i++) {
		m_MeanSV_STAT[SELECT_LOW][i] = m_MeanSV_STAT[PRIOR_MIN][i];
		m_MeanSV_STAT[SELECT_HIGH][i] = m_MeanSV_STAT[PRIOR_MAX][i];
	}

	for (size_t i = 0; i<7; i++) {
		m_Valid_SV[m_ActualEnsemble].meancriteria[i] = NO_CRITERIA;
		bool MinCrit, MaxCrit;
		MinCrit = MaxCrit = false;

		if (m_MeanSV_STAT[SELECT_LOW][i] > m_MeanSV_STAT[PRIOR_MIN][i]) MinCrit = true;
		if (m_MeanSV_STAT[SELECT_HIGH][i] < m_MeanSV_STAT[PRIOR_MAX][i]) MaxCrit = true;

		if (MinCrit&&MaxCrit)
			m_Valid_SV[m_ActualEnsemble].meancriteria[i] = ABOVE_AND_BELOW_CRITERIA;
		else if (MinCrit)
			m_Valid_SV[m_ActualEnsemble].meancriteria[i] = ABOVE_MIN_CRITERIA;
		else if (MaxCrit)
			m_Valid_SV[m_ActualEnsemble].meancriteria[i] = BELOW_MAX_CRITERIA;

	}

}
bool SelectedEnsembles::SetPriorDistributions() {


	return true;
};
bool SelectedEnsembles::ResetAndReOpenFile()
{
	OpenFile();
	m_pNewMap->m_IsPerformanceSmartFlexibleFile_Read = false;
	m_pNewMap->CalculatePerformanceAndSetValues();
	m_pNewMap->m_Are_MeanValues_SV_VariablesEstimated = false;
	m_pNewMap->CalculateSumMeanValidValues_SV_Variables();
	m_SV_StatMemory.AllRuns = false;
	Update_SV_VarStat(false);
	Update_TSV_VarStat(false);
	UpdateAcceptedRuns(true, false);
	EvaluateFile(false);

	return false;
}
