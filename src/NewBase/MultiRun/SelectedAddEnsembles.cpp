// SelectedAddEnsembles.cpp : implementation file
//
#ifndef COUPSTD
#include "../stdafx.h"
#endif
#include "../../Util/FUtil.hpp"
#include "../../NewBase/NewMap.h"
#include "SelectedAddEnsembles.h"

#ifndef COUPSTD
#include "../NewBase/FileScanner.h"

#endif


#include <limits>


SelectedAddEnsembles::SelectedAddEnsembles()
{
	

}

SelectedAddEnsembles::~SelectedAddEnsembles()
{


}
bool SelectedAddEnsembles::SetNewPerformanceVector(size_t cumulatedPerformance, vector<float> VarValidVector) {


	if (cumulatedPerformance == 0)
		return EvaluateFile(true);

	size_t newindex;
	VARTRANSFORMED vartrans;
	bool addnew = false;

	if (MapOfStoredTSV.empty()) {
		newindex = m_VarValidTransformed.size();
		addnew = true;
	}
	else {
		auto it = MapOfStoredTSV.find(cumulatedPerformance);
		if (it != MapOfStoredTSV.end()) {
			newindex = (*it).second;
		}
		else {
			newindex = m_VarValidTransformed.size();
			addnew = true;
		}
	}
	vartrans.cumvalue = cumulatedPerformance;
	vartrans.VarValidVector = VarValidVector;
	if (addnew) {
		m_VarValidTransformed.push_back(vartrans);
		MapOfStoredTSV.insert(pair<size_t, size_t>(cumulatedPerformance, m_VarValidTransformed.size() - 1));
	}
	else {
		m_VarValidTransformed[newindex] = vartrans;
	}
	if (cumulatedPerformance != 0)
		m_CumulatedPerformance = cumulatedPerformance;
	else {
		m_ResetTransformedPerformance=true;
		EvaluateFile(true);
		m_CumulatedPerformance = 0;
		m_ResetTransformedPerformance = false;
	}
	m_Valid_TSV[m_ActualEnsemble].VarTranformed = m_CumulatedPerformance;

	return CompleteProcessingOfInputFiles_MultiStorage();
	

}
bool SelectedAddEnsembles::AddNewEnsemble(bool KeepPreviousCriteria) {

	if (!KeepPreviousCriteria) {
		vector<size_t> acceptedruns;
		acceptedruns.resize(m_TotalNumberRuns);
		for (size_t i = 0; i < m_TotalNumberRuns; i++)
			acceptedruns[i] = i;
		m_AcceptedRuns.push_back(acceptedruns);
	}
	else {

		m_AcceptedRuns.push_back(m_AcceptedRuns[m_AcceptedRuns.size() - 1]);


	}

	string str="Ensemble (" + FUtil::STD_ItoAscii(m_AcceptedIdString.size()+1)+ ")";
	m_AcceptedIdString.push_back(str);
		
	m_ActualEnsemble = m_AcceptedIdString.size() - 1;

	m_Valid_TSV.resize(m_AcceptedRuns.size());
	AllocateValid_TSV(m_ActualEnsemble);
	m_Valid_SV.resize(m_AcceptedRuns.size());
	m_Valid_SV_Mean.resize(m_AcceptedRuns.size());
	AllocateValid_SUM(m_ActualEnsemble);

	if (m_ActualEnsemble >= 1) {
		m_Valid_TSV[m_ActualEnsemble] = m_Valid_TSV[m_ActualEnsemble - 1];
		m_Valid_TSV[m_ActualEnsemble].VarTranformed = m_CumulatedPerformance;
		m_Valid_SV[m_ActualEnsemble] = m_Valid_SV[m_ActualEnsemble - 1];
		m_Valid_SV_Mean[m_ActualEnsemble] = m_Valid_SV_Mean[m_ActualEnsemble - 1];

	}
	if(!KeepPreviousCriteria)
		m_pNewMap->m_MStorage.SetAllToAcceptedRuns();
	

	if (m_ActualEnsemble != string::npos)
		return true;
	else
		return false;
}
bool SelectedAddEnsembles::ChangeEnsembleNames(size_t index, string newname) {
	bool out = false;
	map<int, CResiduals*> ResidualMap;
	for (size_t i = 0; i < m_pNewMap->GetNumAll_TSV_ValVariables(); i++) {
		auto FileNo = m_pNewMap->m_ValidationData.GetValFileNumber(i);
		CResiduals *pRes = m_pNewMap->GetValidResidualPointer(FileNo);
		ResidualMap.insert(pair<int, CResiduals *>(FileNo, pRes));
	}
	for (auto it = ResidualMap.begin(); it != ResidualMap.end(); it++) {
		auto pres = it->second;
		if (pres->ShiftEnsembleName(m_AcceptedIdString[index], newname)) out = true;
	}
	return out;
}
bool SelectedAddEnsembles::RemoveEnsemble(size_t index) {

	bool out = false;
	if (index < m_Valid_TSV.size()) m_Valid_TSV.erase(m_Valid_TSV.begin() + index);
	if (index < m_Valid_SV.size()) m_Valid_SV.erase(m_Valid_SV.begin() + index);
	if (index < m_AcceptedRuns.size()) m_AcceptedRuns.erase(m_AcceptedRuns.begin() + index);
	if (index < m_AcceptedIdString.size()) {
		m_AcceptedIdString.erase(m_AcceptedIdString.begin() + index);
		out = true;
	}
	return out;
}
