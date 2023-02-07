// Residuals.cpp : implementation file
//

#ifndef COUPSTD
#include "../stdafx.h"
#endif
#include "../../std.h"
#include "../../Util/FUtil.hpp"
#include "../../Util/ReadWriteTypes.h"
#include "./ResidualsBase.h"
#include "../../NewBase/NewMap.h"

#ifndef COUPSTD
#include "../NewBase/FileScanner.h"
#include "../MainFrm.h"
#endif
using namespace std;


struct RES_STAT {
	vector<float> accepted_mean;
	vector<float> accepted_min;
	vector<float> accepted_max;
	vector<float> accepted_std;
	vector<float> accepted_p10;
	vector<float> accepted_p50;
	vector<float> accepted_p90;
	vector<float> all_mean;
	vector<float> all_min;
	vector<float> all_max;
	vector<float> all_std;
};



// ResidualsBase

ResidualsBase::ResidualsBase()
{
	m_RecL=m_NumRec=m_NumVar=0;
	m_ScalingNumVarWithDim1 = 1;
	m_MeanDefined = false;

	//m_EnsembleFileRead = false;

}

ResidualsBase::~ResidualsBase()
{

}
bool ResidualsBase::EnsembleReadWrite(bool reading, size_t ensembleno) {
	 if(ensembleno==string::npos&&reading) {
		 bool validfile = true;
		for(size_t i=0; i<m_Ensembles.size(); i++) {
			auto filename = GetXStatFileName(ensembleno);
			if (!EnsembleReadWrite(reading, filename, "", i)) validfile = false;

		} 
		if (m_Ensembles.size() > 0) {
			m_MeanDefined = true;
			return validfile;
		}
		else return false;
	 }
	 else if(ensembleno<m_Ensembles.size()) {
		 string filename = GetXStatFileName(ensembleno);
		 return EnsembleReadWrite(reading, filename, "", ensembleno);
	 }
	 return false;
}
bool ResidualsBase::EnsembleReadWrite(bool reading, string EnsembleName, string NewEnsembleName, size_t ensembleno) {
	auto filename = GetXStatFileName(ensembleno);
	bool good;
	m_EnsembleName = EnsembleName;
	if (reading) {

		good = m_EnsembleResFile.OpenDocFileIn(filename);

		if (!good) {
			return false;
		}
		m_TwoFilesOpen = false;
	}
	else {

		if (FUtil::IsFileExisting(filename)) {
			m_TempOutFile = filename.substr(0, filename.find(".SXBin")) + ".XXBin";
			good = m_EnsembleResFile.OpenDocFileIn(filename);
			good = m_EnsembleResFile.OpenDocFileout(m_TempOutFile);
			m_TwoFilesOpen = true;
		}
		else {
			m_TempOutFile.clear();
			good = m_EnsembleResFile.OpenDocFileout(filename);
			m_TwoFilesOpen = false;
		}
	}
	if (!good) return false;

	m_NewEnsembleName = NewEnsembleName;

	if (ReadWrite_Header(reading)) {
		//m_IsUpdated = true;
		if (reading) {		
			m_EnsembleResFile.CloseFileIn();

		}
		else {
			if (m_TwoFilesOpen) {
				m_EnsembleResFile.CloseFileIn();
				m_EnsembleResFile.CloseFileOut();
				remove(filename.c_str());
				vector< vector<size_t>> koll;
				float value = m_MeanPost.mean[0][0];
				//float value2 = m_MeanPost.mean[1][0];
				for (size_t ivar = 0; ivar < m_NumVar;ivar++) {
					vector<size_t> koll2;
					koll.push_back(koll2);
					for (size_t irec = 0; irec < m_RecL; irec++) {
						if (m_MeanPost.mean[ivar][irec] != m_MeanTest.mean[ivar][irec]) {
							koll[ivar].push_back(irec);



						}
					}
				}


				if (!rename(m_TempOutFile.data(), filename.data())) return false;
			}
			else {
				m_EnsembleResFile.CloseFileOut();
			}
		}


	}
	else {
		if(reading)
			m_EnsembleResFile.CloseFileIn();

		else {

			if (m_TwoFilesOpen) {
				m_EnsembleResFile.CloseFileIn();
				m_EnsembleResFile.CloseFileOut();
			}
			else
				m_EnsembleResFile.CloseFileOut();
		}



	}
	m_EnsembleName = EnsembleName;
	if(ensembleno<m_Ensembles.size())
	m_Ensembles[ensembleno].EnsembleFileRead = true;
	return true;
}
bool ResidualsBase::ReadWrite_Header(bool reading) {

	bool good;
	if(m_MeanTest.mean.size()==0) AlloResVector();

	if (reading) {
		size_t num;
		bool hitmade=false;
		m_EnsembleResFile.rsize_t(&num);
	//	m_Ensembles.clear();
		m_MapofEnsemblePositions.clear();
		for (size_t i = 0; i < num; i++) {
			string name;
			m_EnsembleResFile.rString(&name);
			size_t VarToRead = m_NumVar;
			if (m_MeanTest.mean.size() > m_NumVar) VarToRead = m_MeanTest.mean.size();
			for (size_t var = 0; var < VarToRead; var++)	m_MeanTest.mean[var]=m_EnsembleResFile.rFloatArray2(m_RecL);
			for (size_t var = 0; var < VarToRead; var++)	m_MeanTest.min[var] = m_EnsembleResFile.rFloatArray2( m_RecL);
			for (size_t var = 0; var < VarToRead; var++)	m_MeanTest.max[var] = m_EnsembleResFile.rFloatArray2( m_RecL);
			for (size_t var = 0; var < VarToRead; var++)	m_MeanTest.std[var] = m_EnsembleResFile.rFloatArray2( m_RecL);
			for (size_t var = 0; var < VarToRead; var++)	m_MeanTest.p10[var] = m_EnsembleResFile.rFloatArray2( m_RecL);
			for (size_t var = 0; var < VarToRead; var++)	m_MeanTest.p50[var] = m_EnsembleResFile.rFloatArray2( m_RecL);
			for (size_t var = 0; var < VarToRead; var++)	m_MeanTest.p90[var] = m_EnsembleResFile.rFloatArray2( m_RecL);
			good = m_EnsembleResFile.IsFileInGood();
			if (!good) return false;

			POS_ENSEMBLE ensemble_inf;
			ensemble_inf.pos = i;
			ensemble_inf.name = name;
			ensemble_inf.valid = true;
		//	m_Ensembles.push_back(ensemble_inf);

			if (name == m_EnsembleName) {
				if (name == "Prior")
					m_MeanPrior = m_MeanTest;
				else
					m_MeanPost = m_MeanTest;
				hitmade= true;
				m_MeanDefined = true;
			}

		}
		return hitmade;
	}
	else {
		if (m_TwoFilesOpen) {
			size_t num;
			//	m_Ensembles.clear();
			m_EnsembleResFile.rsize_t(&num);

			m_EnsembleResFile.wInt(&num);
			bool match = false;
			for (size_t i = 0; i < num; i++) {
				string name;
				m_EnsembleResFile.rString(&name);
				vector<float> kalle;
				kalle.resize(m_RecL);


				//m_EnsembleResFile.rFloatArray(&kalle[0], m_RecL);
				size_t VarToRead = m_NumVar;
				if (m_MeanTest.mean.size() > m_NumVar) VarToRead = m_MeanTest.mean.size();
				for (size_t var = 0; var < VarToRead; var++)	m_MeanTest.mean[var] = m_EnsembleResFile.rFloatArray2(m_RecL);;
				for (size_t var = 0; var < VarToRead; var++)	m_MeanTest.min[var] = m_EnsembleResFile.rFloatArray2( m_RecL);
				for (size_t var = 0; var < VarToRead; var++)	m_MeanTest.max[var] = m_EnsembleResFile.rFloatArray2( m_RecL);
				for (size_t var = 0; var < VarToRead; var++)	m_MeanTest.std[var] = m_EnsembleResFile.rFloatArray2( m_RecL);
				for (size_t var = 0; var < VarToRead; var++)	m_MeanTest.p10[var] = m_EnsembleResFile.rFloatArray2( m_RecL);
				for (size_t var = 0; var < VarToRead; var++)	m_MeanTest.p50[var] = m_EnsembleResFile.rFloatArray2( m_RecL);
				for (size_t var = 0; var < VarToRead; var++)	m_MeanTest.p90[var] = m_EnsembleResFile.rFloatArray2( m_RecL);
				good = m_EnsembleResFile.IsFileInGood();
				if (!good) return false;
				if (name == m_EnsembleName) {
					match = true;
					if (name == "Prior") {
						m_EnsembleResFile.wString(&name);
						size_t VarToWrite = m_NumVar;
						if (m_MeanPrior.mean.size() > m_NumVar) VarToWrite = m_MeanPrior.mean.size();
						for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPrior.mean[var], m_RecL);
						for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPrior.min[var], m_RecL);
						for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPrior.max[var], m_RecL);
						for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPrior.std[var], m_RecL);
						for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPrior.p10[var], m_RecL);
						for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPrior.p50[var], m_RecL);
						for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPrior.p90[var], m_RecL);
						good = m_EnsembleResFile.IsFileOutGood();
					}
					else  if (m_NewEnsembleName.size() > 0) {
						// Conditions when changing only Ensemble Name
						m_EnsembleResFile.wString(&m_NewEnsembleName);
						size_t VarToWrite = m_NumVar;
						if (m_MeanTest.mean.size() > m_NumVar) VarToWrite = m_MeanTest.mean.size();
						for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanTest.mean[var], m_RecL);
						for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanTest.min[var], m_RecL);
						for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanTest.max[var], m_RecL);
						for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanTest.std[var], m_RecL);
						for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanTest.p10[var], m_RecL);
						for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanTest.p50[var], m_RecL);
						for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanTest.p90[var], m_RecL);
						good = m_EnsembleResFile.IsFileOutGood();

					}
					else {
						m_EnsembleResFile.wString(&name);
						for (size_t var = 0; var < m_MeanPost.min.size(); var++)	m_EnsembleResFile.wFloatArray2(m_MeanPost.mean[var], m_RecL);
						for (size_t var = 0; var < m_MeanPost.min.size(); var++)	m_EnsembleResFile.wFloatArray2(m_MeanPost.min[var], m_RecL);
						for (size_t var = 0; var < m_MeanPost.min.size(); var++)	m_EnsembleResFile.wFloatArray2(m_MeanPost.max[var], m_RecL);
						for (size_t var = 0; var < m_MeanPost.min.size(); var++)	m_EnsembleResFile.wFloatArray2(m_MeanPost.std[var], m_RecL);
						for (size_t var = 0; var < m_MeanPost.min.size(); var++)	m_EnsembleResFile.wFloatArray2(m_MeanPost.p10[var], m_RecL);
						for (size_t var = 0; var < m_MeanPost.min.size(); var++)	m_EnsembleResFile.wFloatArray2(m_MeanPost.p50[var], m_RecL);
						for (size_t var = 0; var < m_MeanPost.min.size(); var++)	m_EnsembleResFile.wFloatArray2(m_MeanPost.p90[var], m_RecL);
						good = m_EnsembleResFile.IsFileOutGood();
					}
				}
				else {
					bool namexist = false;
					for (size_t i = 0; i < m_pNewMap->m_MStorage.m_AcceptedIdString.size(); i++) {
						if (name == m_pNewMap->m_MStorage.m_AcceptedIdString[i]) namexist = true;
					}
					if (!namexist) {
						// Set Default Name if not exist in MStorage.
						name = "Ensemble (";
						name += FUtil::STD_ItoAscii(m_Ensembles.size() + 1);
						name += ")";
						if(m_pNewMap->m_MStorage.m_AcceptedIdString.size()>m_Ensembles.size())
								m_pNewMap->m_MStorage.m_AcceptedIdString[m_Ensembles.size()] = name;
					}
					m_EnsembleResFile.wString(&name);
					size_t VarToWrite = m_NumVar;
					if (m_MeanTest.mean.size() > m_NumVar) VarToWrite = m_MeanTest.mean.size();
					for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanTest.mean[var], m_RecL);
					for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanTest.min[var], m_RecL);
					for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanTest.max[var], m_RecL);
					for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanTest.std[var], m_RecL);
					for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanTest.p10[var], m_RecL);
					for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanTest.p50[var], m_RecL);
					for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanTest.p90[var], m_RecL);
					good = m_EnsembleResFile.IsFileOutGood();
				}
				POS_ENSEMBLE ensemble_inf;
				ensemble_inf.pos = i;
				ensemble_inf.name = name;
				ensemble_inf.valid = false;

				//m_Ensembles.push_back(ensemble_inf);

			}
			if (!match) {
				num++;
				if (m_EnsembleName == "Prior") {
					m_EnsembleResFile.wString(&m_EnsembleName);
					size_t VarToWrite = m_NumVar;
					if (m_MeanPrior.mean.size() > m_NumVar) VarToWrite = m_MeanPrior.mean.size();
					for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPrior.mean[var], m_RecL);
					for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPrior.min[var], m_RecL);
					for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPrior.max[var], m_RecL);
					for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPrior.std[var], m_RecL);
					for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPrior.p10[var], m_RecL);
					for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPrior.p50[var], m_RecL);
					for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPrior.p90[var], m_RecL);
					good = m_EnsembleResFile.IsFileOutGood();

				}
				else {
					if (m_EnsembleName.size() == 0) {



					}
					m_EnsembleResFile.wString(&m_EnsembleName);
					size_t VarToWrite = m_NumVar;
					if (m_MeanPost.mean.size() > m_NumVar) VarToWrite = m_MeanPost.mean.size();

					for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPost.mean[var], m_RecL);
					for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPost.min[var], m_RecL);
					for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPost.max[var], m_RecL);
					for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPost.std[var], m_RecL);
					for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPost.p10[var], m_RecL);
					for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPost.p50[var], m_RecL);
					for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPost.p90[var], m_RecL);
					good = m_EnsembleResFile.IsFileOutGood();
				}
				size_t num1 = 0;
				if (!m_EnsembleResFile.SetPutPosition(&num1)) return false;
				if (!m_EnsembleResFile.wInt(&num)) return false;
			}

			if (m_EnsembleResFile.IsFileOutGood() && m_EnsembleResFile.IsFileInGood())
				return true;
			else
				return false;
		}
		else {
			// New single file.
			size_t num = 1;
			m_EnsembleResFile.wInt(&num);
			if (m_EnsembleName.size() == 0) m_EnsembleName = "Ensemble (1)";
			m_EnsembleResFile.wString(&m_EnsembleName);

			size_t VarToWrite = m_NumVar * m_ScalingNumVarWithDim1;
			if (VarToWrite > m_MeanPost.mean.size()) VarToWrite = m_NumVar;

			for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPost.mean[var], m_RecL);
			for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPost.min[var], m_RecL);
			for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPost.max[var], m_RecL);
			for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPost.std[var], m_RecL);
			for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPost.p10[var], m_RecL);
			for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPost.p50[var], m_RecL);
			for (size_t var = 0; var < VarToWrite; var++)	m_EnsembleResFile.wFloatArray2(m_MeanPost.p90[var], m_RecL);
			good = m_EnsembleResFile.IsFileOutGood();

			//m_Ensembles.clear();
			POS_ENSEMBLE ensemble_inf;
			ensemble_inf.pos = 0;
			ensemble_inf.name = m_EnsembleName;
			ensemble_inf.valid = false;

			//m_Ensembles.push_back(ensemble_inf);

			return good;
		}
	}
}
string ResidualsBase::GetXStatFileName(size_t EnsembleNo) {
	if (m_pNewMap != nullptr) {
		string str;
		auto ipos = m_pNewMap->m_DocFile2.m_Multi_MBin_File.find(".MBin");
		if (ipos != string::npos) {
			str = m_pNewMap->m_DocFile2.m_Multi_MBin_File.substr(0, ipos);
			str += "_V" + FUtil::STD_ItoAscii(m_FileNo)+"_Eno"+ FUtil::STD_ItoAscii(EnsembleNo+1);
			str += ".SXBin";
			return str;
		}
	}
	return "";
}
bool ResidualsBase::AlloResVector() {
	if (m_RecL == 0 || m_NumRec == 0 || m_NumVar == 0) return false;

	m_Residuals.resize(m_RecL);
	m_Mean.resize(m_NumVar*m_RecL);
	m_MeanPrior.mean.resize(m_NumVar*m_ScalingNumVarWithDim1);
	m_MeanPrior.min.resize(m_NumVar*m_ScalingNumVarWithDim1);
	m_MeanPrior.max.resize(m_NumVar*m_ScalingNumVarWithDim1);
	m_MeanPrior.std.resize(m_NumVar*m_ScalingNumVarWithDim1);
	m_MeanPrior.p10.resize(m_NumVar*m_ScalingNumVarWithDim1);
	m_MeanPrior.p50.resize(m_NumVar*m_ScalingNumVarWithDim1);
	m_MeanPrior.p90.resize(m_NumVar*m_ScalingNumVarWithDim1);

	m_MeanPost.mean.resize(m_NumVar*m_ScalingNumVarWithDim1);
	m_MeanPost.min.resize(m_NumVar*m_ScalingNumVarWithDim1);
	m_MeanPost.max.resize(m_NumVar*m_ScalingNumVarWithDim1);
	m_MeanPost.std.resize(m_NumVar*m_ScalingNumVarWithDim1);
	m_MeanPost.p10.resize(m_NumVar*m_ScalingNumVarWithDim1);
	m_MeanPost.p50.resize(m_NumVar*m_ScalingNumVarWithDim1);
	m_MeanPost.p90.resize(m_NumVar*m_ScalingNumVarWithDim1);

	m_MeanTest.mean.resize(m_NumVar*m_ScalingNumVarWithDim1);
	m_MeanTest.min.resize(m_NumVar*m_ScalingNumVarWithDim1);
	m_MeanTest.max.resize(m_NumVar*m_ScalingNumVarWithDim1);
	m_MeanTest.std.resize(m_NumVar*m_ScalingNumVarWithDim1);
	m_MeanTest.p10.resize(m_NumVar*m_ScalingNumVarWithDim1);
	m_MeanTest.p50.resize(m_NumVar*m_ScalingNumVarWithDim1);
	m_MeanTest.p90.resize(m_NumVar*m_ScalingNumVarWithDim1);
	
	for (size_t i = 0; i < m_NumVar*m_ScalingNumVarWithDim1; i++) {
		m_MeanPrior.mean[i].resize(m_RecL);
		m_MeanPrior.min[i].resize(m_RecL);
		m_MeanPrior.max[i].resize(m_RecL);
		m_MeanPrior.std[i].resize(m_RecL);
		m_MeanPrior.p10[i].resize(m_RecL);
		m_MeanPrior.p50[i].resize(m_RecL);
		m_MeanPrior.p90[i].resize(m_RecL);

		m_MeanPost.mean[i].resize(m_RecL);
		m_MeanPost.min[i].resize(m_RecL);
		m_MeanPost.max[i].resize(m_RecL);
		m_MeanPost.std[i].resize(m_RecL);
		m_MeanPost.p10[i].resize(m_RecL);
		m_MeanPost.p50[i].resize(m_RecL);
		m_MeanPost.p90[i].resize(m_RecL);
		
		m_MeanTest.mean[i].resize(m_RecL);
		m_MeanTest.min[i].resize(m_RecL);
		m_MeanTest.max[i].resize(m_RecL);
		m_MeanTest.std[i].resize(m_RecL);
		m_MeanTest.p10[i].resize(m_RecL);
		m_MeanTest.p50[i].resize(m_RecL);
		m_MeanTest.p90[i].resize(m_RecL);
	}
	m_SizeOfMeanVector = m_NumVar*m_RecL;
	return true;
}

bool ResidualsBase::OpenResidualFile(bool reading) {

	if (reading) {
		if (!m_ResidualFile.OpenDocFileIn(m_FileName)) return false;
		m_FileSize = m_ResidualFile.GetStdInputFileSize();
		if (m_FileSize == 0 || m_FileSize == string::npos) {
			return false;


		}
		Residuals_ReadWrite_Header(true);
		m_Residuals.resize(m_RecL);
		if(m_pNewMap->m_FileVersionNumberRead < 65)
			AlloResVector();
	}
	else
		if (!m_ResidualFile.OpenDocFileout(m_FileName)) return false;
	
	return true;
}
bool ResidualsBase::Residuals_ReadWrite_Header(bool reading) {
	if (reading) {
		if (!m_ResidualFile.rsize_t(&m_NumRec)) return false;
		if (!m_ResidualFile.rsize_t(&m_RecL)) return false;
		if (!m_ResidualFile.rsize_t(&m_NumVar)) return false;
		m_MaxNumRec = size_t(m_FileSize / (m_RecL*m_NumVar * 4));
		if (m_pNewMap != nullptr) {
			if (m_pNewMap->m_MultiRun_Array[0].NumberOfRepeatitionsWithinDimension < m_NumRec) {
				if (m_pNewMap->m_MultiRun_Array.size() == 1) m_NumRec = m_pNewMap->m_MultiRun_Array[0].NumberOfRepeatitionsWithinDimension * m_NumVar;
				else m_NumRec = 0;
				for (size_t i = 1; i < m_pNewMap->m_MultiRun_Array.size(); i++) {
					m_NumRec += m_pNewMap->m_MultiRun_Array[i].NumberOfRepeatitionsWithinDimension*m_NumVar;
				}
			}
		}
		if (m_MaxNumRec<m_NumRec / m_NumVar) {
#ifndef COUPSTD
			if (m_pNewMap != nullptr) {
				m_pNewMap->m_ErrorCount++;
				if (m_pNewMap->m_ErrorCount<10) {
					string str;
					str = "For some reason the file \n("; str += m_FileName; str += ")\n contains only :" + FUtil::STD_ItoAscii(m_MaxNumRec) + " Runs";
					str += "\nAdjust the Number of Multi Runs to correspond with that number";
					str += "\nExpected Number was: ";
					str += FUtil::STD_ItoAscii(m_NumRec / m_NumVar);
					MFC_Util::MessageBox(str.c_str(), "Lacking data in XBin", MB_OK);
				}
			}
#endif		

		}
	}
	else {
		if (!m_ResidualFile.wInt(&m_NumRec)) return false;
		if (!m_ResidualFile.wInt(&m_RecL)) return false;
		if (!m_ResidualFile.wInt(&m_NumVar)) return false;
	}
	return true;
}
bool ResidualsBase::CloseResidualFile(bool reading) {

	if (reading)
		m_ResidualFile.CloseFileIn();
	else
		m_ResidualFile.CloseFileOut();

	return true;
}
bool ResidualsBase::IsOpen(bool reading) {
	if (reading) {
		if (m_ResidualFile.IsFileInGood() && m_IsOpen)
			return true;
		else
			return false;
	}
	else {
		if (m_ResidualFile.IsFileOutGood() && m_IsOpen)
			return true;
		else
			return false;
	}
}
bool ResidualsBase::SetNewEnsembleByNumber(size_t EnsembleNo) {
	if (EnsembleNo < m_pNewMap->m_MStorage.m_AcceptedRuns.size())
		return EnsembleReadWrite(true, EnsembleNo);
	else
		return false;
}
bool ResidualsBase::ShiftEnsembleName(string oldname, string newname) {
	bool out=false;


	for (size_t i = 0; i < m_Ensembles.size(); i++) {
		if (m_Ensembles[i].name == oldname) {
			m_Ensembles[i].name = newname;
			out = true;
		}
	}
	if (out) return EnsembleReadWrite(false, oldname, newname);

	return out;
}
bool ResidualsBase::EnableEstimatesMade(size_t enableno) {
	if (enableno < m_Ensembles.size()) {
		bool made = true;
		for (size_t i = 0; i < m_Ensembles.size(); i++)
			if (!m_Ensembles[enableno].EnsembleFileRead) made = false;
		return made;


	}
	else if (m_pNewMap->m_MStorage.m_AcceptedRuns.size() >= enableno) {
		m_Ensembles.resize(m_pNewMap->m_MStorage.m_AcceptedRuns.size());
		return false;


	}
	else return false;
};
