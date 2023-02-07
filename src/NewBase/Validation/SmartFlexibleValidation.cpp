#include "SmartFlexibleValidation.h"
#include "../MR.h"
#include "../../Util/FUtil.hpp"
#include "../../Util/Constants.h"
#include "../../Util/StatUtil.h"
#include "../../Util/ReadWriteTypes.h"




SmartFlexibleValidation::SmartFlexibleValidation()
{
	m_IsPerformanceSmartFlexibleFile_Read = false;
}


SmartFlexibleValidation::~SmartFlexibleValidation()
{
}
bool SmartFlexibleValidation::UpdateValidationPerformance_SmartFlexVariables() {
	if (m_IsPerformanceSmartFlexibleFile_Read) return false;
	if (m_ValSum_Array.size()>0) {
		m_pSimDoc = dynamic_cast<MR*>(this);
		if(m_pSimDoc->IsRunning()) return false;
		size_t RunNo;
		OUTPUT SumValue;
		RunNo = 0;
		bool filevalid=false;
		if (m_pSimDoc->IsFinishedSimulation()) {
			filevalid=m_pSimDoc->MR_Storage_Open();
		}

		if (!filevalid) return false;
		//m_MStorage.ReWriteExistingFile(0);
#ifndef COUPSTD
		MFC_Util mfc_util;
			mfc_util.SetPaneMessage("Estimating Performance indicators for Smart Flexible Variables");
#endif


		for (size_t i = 0; i<m_ValSum_Array.size(); i++) {
#ifndef COUPSTD
			mfc_util.SetPaneMessage("Reading Values from various sources to define performance Indicator: "+FUtil::STD_ItoAscii(i + 1) + "(" + FUtil::STD_ItoAscii(m_ValSum_Array.size()) + ")");
#endif
			auto valsum = m_pSimDoc->GetValSumStruct(i);
			bool vectortype = false;
			SIMB sumv;
			sumv.pBase = GetPtrFromIntType(valsum.OutputType, valsum.Group, valsum.Name);
			sumv.TabIndex = valsum.LocalIndex;
			float simvalue;
			vector<float> sumsimvector;
			if (valsum.SimValueSource == SIMSUMSOURCE(0)) {
				if (sumv.pBase->Is_Vector()) {
					SumValue = ((OutVector*)sumv.pBase)->GetSumList(sumv.TabIndex);
					auto sumindex = ((OutVector*)sumv.pBase)->GetSumVarIndex(sumv.TabIndex - 1, valsum.SumVarType + 7);
					for (size_t runno = 0; runno < m_MStorage.GetNumAccepted(); runno++) {
						simvalue = m_MStorage.GetSumV(runno, sumindex - 6);
						sumsimvector.push_back(simvalue);
					}
				}
				else {
					SumValue = ((OutSingle*)sumv.pBase)->GetSumList();
					auto sumindex = ((OutSingle*)sumv.pBase)->GetSumVarIndex(valsum.SumVarType + 7);
					for (size_t runno = 0; runno < m_MStorage.GetNumAccepted(); runno++) {
						simvalue = m_MStorage.GetSumV(runno, sumindex - 6);
						sumsimvector.push_back(simvalue);
					}

				}
			}
			else if (valsum.SimValueSource == SIMSUMSOURCE(1)) {
				if (valsum.Sim_Value_Residual_Index < m_ValSum_Array.size()) {
					auto valsumres = GetValSumStruct(valsum.Sim_Value_Residual_Index);
					for (size_t runno = 0; runno < m_MStorage.GetNumAccepted(); runno++) {
						simvalue = m_MStorage.GetValidSum(runno, valsum.Sim_Value_Residual_Index % 3);
						//simvalue = valsumres.SimValueVector[runno] - valsumres.ObsValueVector[runno];


						sumsimvector.push_back(simvalue);
					}
				}
			}
			else if (valsum.SimValueSource == SIMSUMSOURCE(2)) {
				CPG *pMulti_PG_File;
				pMulti_PG_File = &m_PG_MultiOutputFile;
				// Reading values from multiple bin file
				pMulti_PG_File->ReOpen();
				for (size_t runno = 0; runno < m_MStorage.GetNumAccepted(); runno++) {
					
					m_MStorage.GetMultiPGFileForCorrectRunNo(runno, pMulti_PG_File);

					pMulti_PG_File->SetRepititionSequence(runno + 1, true);
					size_t ist =pMulti_PG_File->GetRecord(valsum.MultiBin_StartMin);
					size_t iend = pMulti_PG_File->GetRecord(valsum.MultiBin_EndMin);
					float sumacc = 0;
					size_t count = 0;
					for (size_t ii = ist; ii < iend; ii++) {
						sumacc += pMulti_PG_File->GetVarValue(valsum.Sim_Value_Residual_Index + 1, ii);
						count++;
					}
					if (count > 0) {
						simvalue = sumacc / count;
						sumsimvector.push_back(simvalue);
					}
					else {
						sumsimvector.push_back(MISSING);
					}
				}
				pMulti_PG_File->CloseFile();

			}

			//simvalue = SumValue.Initial;
			vector<float> Obsvector;

			if (valsum.FlexSumVar) {
				Obsvector = GetObsValueFromValSumFlex(valsum.IDFileName, valsum.IDVarName);
			}




			float simsum = 0., obssum = 0.;
			double rmsesum = 0;
			size_t count = 0;
			for (size_t runno = 0; runno < m_MStorage.GetNumAccepted(); runno++) {
				if (Obsvector[runno] > 0.9*MISSING&&sumsimvector[runno]> -4.3E8) {
					float me = float(sumsimvector[runno] - Obsvector[runno]);
					obssum += Obsvector[runno];
					simsum += sumsimvector[runno];
					rmsesum += (Obsvector[runno] - sumsimvector[runno])*(Obsvector[runno] - sumsimvector[runno]);
					count++;
					m_MStorage.SetValidSum(runno, i * 3 + 0, me);
					m_MStorage.SetValidSum(runno, i * 3 + 1, me*me);
				}
				else {
					m_MStorage.SetValidSum(runno, i * 3 + 0, MISSING);
					m_MStorage.SetValidSum(runno, i * 3 + 1, MISSING);

				}

			}
			if (count > 0) {
				m_ValSum_Array[i].ObsValue = obssum / count;
				m_ValSum_Array[i].SimValue = simsum / count;
				m_ValSum_Array[i].n_RegMulti = count;
			}






		}


	}
	CalculatePerformanceAndSetValues();
	return true;
}
vector<float> SmartFlexibleValidation::GetObsValueFromValSumFlex(string IDFileName, string  IDVarName) {
	vector<float> out;
	float fvalue;
	VALSUMLINK valsumlink;
	size_t index_var, index_run, index_file, index_key_file;
	index_file = string::npos;
	index_run = string::npos;
	for (size_t i = 0; i<m_ValsumFlexFiles.size(); i++) {
		if (IDFileName == m_ValsumFlexFiles[i].FileName) {
			index_file = i;
			continue;
		}
		else if (IDFileName == "")
			index_file = 0;
	}
	index_var = 0;
	size_t koll = IDVarName.size();
	auto it = MapFlexId[index_file].find(IDVarName.c_str());
	if (it != MapFlexId[index_file].end()) {
		index_var = (*it).second;
	};
	index_key_file = 0;
	auto it2 = MapFlexId[index_file].find("ID_PLOT");
	if (it2 != MapFlexId[index_file].end()) {
		index_key_file = (*it2).second;
	};
// Test if RunCode is in first position of MultiRun Parameters
	bool RunCodeValid = true;
	if (m_MStorage.GetMultiP(0, 0) == m_MStorage.GetMultiP(m_MStorage.GetNumAccepted() - 1, 0)) RunCodeValid = false;
	for (size_t runno = 0; runno < m_MStorage.GetNumAccepted(); runno++) {
		float runcode = m_MStorage.GetMultiP(runno, 0);
		if (!RunCodeValid) runcode = runno + 1;
		size_t icode = size_t(runcode + 0.01);
		auto it = MapFlexVal[index_file].find(icode);
		if (it != MapFlexVal[index_file].end()) {
			index_run = (*it).second;
			fvalue = m_ValFlexValue[index_file][index_run][index_var];

		}
		else
			fvalue = MISSING;
		out.push_back(fvalue);
	}


	return out;

}

size_t SmartFlexibleValidation::ImportSelectedParamtersFromCSVFile(FCSV *pCSVFile)
{
	
	vector<SIMB> simv;
	vector<double> values, min_values, max_values;
	// Specify a file Name
	// Check if File is already in Model Files by Paramter By Any of CalibrationsModes 

	//auto ans=MFC_Util::MessageBoxReturn("Do you want to use any of already defined CSV files or do you want to specify a new one", "Reading files", 1);
	//Selected Stream;

	if (pCSVFile == nullptr) {

		ifstream stream;
		FCSV file(&stream, FILEDEFINEDPARAMETERS, GENERAL, NOFPROC, NOBPROC, NORMAL);
		pCSVFile = &file;
#ifndef COUPSTD
		MFC_Util::MessageBox("Specify File Name for Multi Run Parameter CSV File");
		FILEDLG FileDlg = MFC_Util::GetFileDialog("*.csv", "", "Comma separated file (*.csv)|*.csv|");
		if (FileDlg.opengood != IDOK) {
			return false;
		}

		string filename = FileDlg.filename;
		pCSVFile->SetValue(filename);
		pCSVFile->SetNumHeadings(7);
		pCSVFile->CheckFileName(this);
#endif

		pCSVFile->AssignSIMB_PointersFromHeader();
		pCSVFile->AssignKey_WithValues();
		simv = pCSVFile->GetSIMB_vector();


		if (!pCSVFile->WithTableValues()) {
			dynamic_cast<MR*>(this)->SetParametersRandomForMultiRun(simv, pCSVFile);
		}
		else {
			dynamic_cast<MR*>(this)->SetParametersTableForMultiRun(simv, pCSVFile);
		
		}


	}



	return size_t(1);
}
bool SmartFlexibleValidation::CalculatePerformanceAndSetValues() {
	bool multivalid;
	MR *pDoc;
#ifndef COUPSTD
	MFC_Util mfc_util;
#endif
	pDoc = dynamic_cast<MR*>(this);
	if (pDoc == nullptr) return false;


	if (pDoc->IsFinishedSimulation())
		multivalid = pDoc->MR_Storage_Open();
	else
		multivalid = false;


	if (m_IsPerformanceSmartFlexibleFile_Read) return true;
	if (multivalid&&pDoc->m_ValSum_Array.size()>0) {

		SetSimObsRatio();

		// Check if sizem_ValSum_Array is matching m_NumSumValid
		//if (pDoc->m_MStorage. != pDoc->m_ValSum_Array.size()) {

	//	pDoc->m_MStorage.ReWriteExistingFile(pDoc->MR_Get_TotalNumberofRuns());

		//}
		for (size_t i = 0; i<pDoc->m_ValSum_Array.size(); i++) {
#ifndef COUPSTD
			mfc_util.SetPaneMessage("Estimating Performance indicators for CSV file with " + FUtil::STD_ItoAscii(m_pSimDoc->MR_Get_TotalNumberofRuns())+ " Runs"+ FUtil::STD_ItoAscii(i + 1) + "(" + FUtil::STD_ItoAscii(pDoc->m_ValSum_Array.size()) + ")");
#endif
			auto valsum = GetValSumStruct(i);

			valsum.ObsValueVector.clear();
			valsum.SimValueVector.clear();
			valsum.ME.clear();
			valsum.RMSE.clear();
			valsum.Slope_Multi.clear();
			valsum.R2_Multi.clear();
			valsum.InterCept_Multi.clear();
			valsum.LogLi.clear();
			valsum.NSE.clear();

			SIMB sumv;
			sumv.pBase = pDoc->GetPtrFromIntType(valsum.OutputType, valsum.Group, valsum.Name);
			sumv.TabIndex = valsum.LocalIndex;
			bool vectortype = false;
			size_t sumindex = 6;
			if (sumv.pBase == nullptr) {



			}
			else {
				vectortype = sumv.pBase->Is_Vector();

				if (sumv.pBase->Is_Vector()) {
					sumindex = ((OutVector*)sumv.pBase)->GetSumVarIndex(sumv.TabIndex - 1, valsum.SumVarType + 7);
				}
				else {
					sumindex = ((OutSingle*)sumv.pBase)->GetSumVarIndex(valsum.SumVarType + 7);
				}
			}
			vector<float> residual_vector;
			float simvalue;
			VALSUMv valsumres;
			if (valsum.Sim_Value_Residual_Index < pDoc->m_ValSum_Array.size()) {
				valsumres = GetValSumStruct(valsum.Sim_Value_Residual_Index);
			}
			size_t imore;
			vector<float> simlowestdim, obslowestdim;
		
			 for(size_t irun=0; irun<m_pSimDoc->MR_Get_TotalNumberofRuns(); irun++) {
#ifndef COUPSTD
				 if(irun%1000==0) mfc_util.SetPaneMessage("Estimating Performance indicators for CSV file " + FUtil::STD_ItoAscii(irun) + " Runs" + FUtil::STD_ItoAscii(i + 1) + "(" + FUtil::STD_ItoAscii(pDoc->m_ValSum_Array.size()) + ")");
#endif
					if (valsum.SimValueSource == SIMSUMSOURCE(0))
						simvalue = pDoc->m_MStorage.GetSumV(irun, sumindex - 6);
					else if (valsum.SimValueSource == SIMSUMSOURCE(1)) {
						if (irun < valsumres.SimValueVector.size() && valsumres.ObsValueVector[irun] >= 0.9*MISSING&&valsumres.SimValueVector[irun] >= 0.9*MISSING)
							simvalue = valsumres.SimValueVector[irun] - valsumres.ObsValueVector[irun];
						else
							simvalue = MISSING;
					}
					else if (valsum.SimValueSource == SIMSUMSOURCE(2)) {
						CPG *pMulti_PG_File;
						pMulti_PG_File = &m_PG_MultiOutputFile;
						pMulti_PG_File->ReOpen();

						m_MStorage.GetMultiPGFileForCorrectRunNo(irun, pMulti_PG_File);

						pMulti_PG_File->SetRepititionSequence(irun + 1, true);
						size_t ist = pMulti_PG_File->GetRecord(valsum.MultiBin_StartMin);
						size_t iend = pMulti_PG_File->GetRecord(valsum.MultiBin_EndMin);

						float sumacc = 0;
						size_t count = 0;
						for (size_t ii = ist; ii < iend; ii++) {
							sumacc += pMulti_PG_File->GetVarValue(valsum.Sim_Value_Residual_Index + 1, ii);
							count++;
						}
						if (count > 0) {
							simvalue = sumacc / count;
						}
						else {
							simvalue = MISSING;
						}
						pMulti_PG_File->CloseFile();
					}


					float obsvalue;
					if (valsum.FlexSumVar) {
						obsvalue = pDoc->GetActualObsValueFromValSumFlex(valsum.IDFileName, valsum.IDVarName, irun);
					}
					else
						obsvalue = valsum.ObsValue;

					if (valsum.ObsScaling == 1 && obsvalue > 0.9*MISSING)
						obsvalue *= float(valsum.ObsScalingFactor);
					else if (valsum.ObsScaling == 2)
						simvalue /= float(valsum.ObsScalingFactor);

					if (obsvalue > 0.9*MISSING&&simvalue > -4.2E8) {
						float me_value = simvalue - obsvalue;
						pDoc->m_MStorage.SetValidSum(irun, i * 3, me_value);
						pDoc->m_MStorage.SetValidSum(irun, i * 3 + 1, me_value*me_value);
						double likelivalue = SumLikelihoodSingleValue(i, simvalue, obsvalue);
						pDoc->m_MStorage.SetValidSum(irun, i * 3 + 2, float(likelivalue));
					}
					else {
						pDoc->m_MStorage.SetValidSum(irun, i * 3, MISSING);
						pDoc->m_MStorage.SetValidSum(irun, i * 3 + 1, MISSING);
						pDoc->m_MStorage.SetValidSum(irun, i * 3 + 2, MISSING);
						obsvalue = MISSING;
					}
					valsum.ObsValueVector.push_back(obsvalue);
					residual_vector.push_back(simvalue - obsvalue);
					valsum.SimValueVector.push_back(simvalue);

					simlowestdim.push_back(simvalue);
					obslowestdim.push_back(obsvalue);

					if (irun%m_pSimDoc->m_MultiRun_Array[0].NumberOfRepeatitionsWithinDimension == m_pSimDoc->m_MultiRun_Array[0].NumberOfRepeatitionsWithinDimension -1) {
						STAT_PERF a;
						a = Get_StatComp_ObsIndep(&simlowestdim, &obslowestdim);
						double likelivalue = 0;

						for (size_t i2 = 0; i2 < simlowestdim.size(); i2++) {
							if (simlowestdim[i2] > 0.5*MISSING&&obslowestdim[i2] >= .5*MISSING)
								likelivalue += SumLikelihoodSingleValue(i, simlowestdim[i2], obslowestdim[i2]);
						}
						for (size_t iall = 0; iall < m_MultiRun_Array[0].NumberOfRepeatitionsWithinDimension; iall++) {
							valsum.n_RegMulti = a.n;
							valsum.R2_Multi.push_back(a.r2);
							valsum.InterCept_Multi.push_back(a.intercept);
							valsum.Slope_Multi.push_back(a.slope);
							valsum.ME.push_back(a.me);
							valsum.RMSE.push_back(a.rmse);
							valsum.NSE.push_back(a.nse);
							valsum.LogLi.push_back(likelivalue);
						}
						simlowestdim.resize(0);
						obslowestdim.resize(0);

						
			
						

					}

				}

		
			//a = Get_StatComp_ObsIndep(&residual_vector, &valsum.ObsValueVector );

			SetValSum(i, valsum);
		}
		if (CalculateMeanPerformanceAndSetValues())
			m_IsPerformanceSmartFlexibleFile_Read = true;
		
		return 	m_IsPerformanceSmartFlexibleFile_Read;

	}
	else
		return false;
}
bool SmartFlexibleValidation::CalculateMeanPerformanceAndSetValues()
{
	bool multivalid;
	if (m_IsPerformanceSmartFlexibleFile_Read) return true;

	m_pSimDoc = dynamic_cast<MR*>(this);
	if (m_pSimDoc->IsFinishedSimulation())
		multivalid = m_pSimDoc->MR_Storage_Open();
	else
		multivalid = false;

	double meansum, meanvalue;
	size_t mean_n;
	auto mean = [&](double dvalue) {
		meansum += dvalue;
		mean_n++;
	};
#ifndef COUPSTD
	MFC_Util mfc_util;
#endif
	if (multivalid&&m_pSimDoc->m_ValSum_Array.size() > 0) {
		for (size_t i = 0; i < m_pSimDoc->m_ValSum_Array.size(); i++) {
			auto valsum = GetValSumStruct(i);
#ifndef COUPSTD
			mfc_util.SetPaneMessage("Estimating  Mean value of all runs - SV Performance indicators " + FUtil::STD_ItoAscii(m_pSimDoc->MR_Get_TotalNumberofRuns()) + " Runs" + FUtil::STD_ItoAscii(i + 1) + "(" + FUtil::STD_ItoAscii(m_pSimDoc->m_ValSum_Array.size()) + ")");
#endif
			meansum = 0; mean_n = 0;
			for (auto it = valsum.ME.begin(); it != valsum.ME.end(); mean(*it), it++);
				if (mean_n != 0) valsum.ME_mean = meansum / mean_n;
			meansum = 0; mean_n = 0;
			for (auto it = valsum.R2_Multi.begin(); it != valsum.R2_Multi.end(); mean(*it), it++);
				if (mean_n != 0) valsum.R2_Multi_mean = meansum / mean_n;
			meansum = 0; mean_n = 0;
			for (auto it = valsum.InterCept_Multi.begin(); it != valsum.InterCept_Multi.end(); mean(*it), it++);
				if (mean_n != 0) valsum.InterCept_Multi_mean = meansum / mean_n;
			meansum = 0; mean_n = 0;
			for (auto it = valsum.Slope_Multi.begin(); it != valsum.Slope_Multi.end(); mean(*it), it++);
				if (mean_n != 0) valsum.Slope_Multi_mean = meansum / mean_n;

			for (auto it = valsum.LogLi.begin(); it != valsum.LogLi.end(); mean(*it), it++);
				if (mean_n != 0) valsum.LogLi_mean = meansum / mean_n;
			meansum = 0; mean_n = 0;
			for (auto it = valsum.RMSE.begin(); it != valsum.RMSE.end(); mean(*it), it++);
				if (mean_n != 0) valsum.RMSE_mean = meansum / mean_n;
			
			meansum = 0; mean_n = 0;
			for (auto it = valsum.NSE.begin(); it != valsum.NSE.end(); mean(*it), it++);
			if (mean_n != 0) valsum.NSE_mean = meansum / mean_n;

			for (auto it = valsum.ObsValueVector.begin(); it != valsum.ObsValueVector.end(); mean(*it), it++);
				if (mean_n != 0) valsum.ObsValue = meansum / mean_n;
			meansum = 0; mean_n = 0;
			for (auto it = valsum.NSE.begin(); it != valsum.NSE.end(); mean(*it), it++);
				if (mean_n != 0) valsum.NSE_mean = meansum / mean_n;

			valsum.SimValue = valsum.ObsValue + valsum.ME_mean;


			SetValSum(i, valsum);
		}
		
	}

	return true;
}
bool SmartFlexibleValidation::SetSimObsRatio() {
	bool multivalid;
	m_pSimDoc = dynamic_cast<MR*>(this);
	if (m_pSimDoc->IsFinishedSimulation())
		multivalid = m_pSimDoc->MR_Storage_Open();
	else
		multivalid = false;
#ifndef COUPSTD
	MFC_Util mfc_util;
#endif
	if (multivalid&&m_pSimDoc->m_ValSum_Array.size() > 0) {

		for (size_t i = 0; i < m_pSimDoc->m_ValSum_Array.size(); i++) {
			auto valsum = GetValSumStruct(i);
#ifndef COUPSTD
			mfc_util.SetPaneMessage("Estimating  Ratio between simulated and Measured Mean value of all runs - SV Performance indicators " + FUtil::STD_ItoAscii(m_pSimDoc->MR_Get_TotalNumberofRuns()) + " Runs" + FUtil::STD_ItoAscii(i + 1) + "(" + FUtil::STD_ItoAscii(m_pSimDoc->m_ValSum_Array.size()) + ")");
#endif
			valsum.ObsValueVector.clear();
			valsum.SimValueVector.clear();

			SIMB sumv;
			sumv.pBase = m_pSimDoc->GetPtrFromIntType(valsum.OutputType, valsum.Group, valsum.Name);
			sumv.TabIndex = valsum.LocalIndex;
			bool vectortype = false;
			size_t sumindex=6;
			if (sumv.pBase != nullptr) {
				vectortype = sumv.pBase->Is_Vector();
				if (sumv.pBase->Is_Vector()) {
					sumindex = ((OutVector*)sumv.pBase)->GetSumVarIndex(sumv.TabIndex - 1, valsum.SumVarType + 7);
				}
				else {
					sumindex = ((OutSingle*)sumv.pBase)->GetSumVarIndex(valsum.SumVarType + 7);
				}
			}
			double SimAcc, ObsAcc;
			SimAcc = ObsAcc = 0.;
			size_t count = 0;
			VALSUMv valsumres;
			if (valsum.Sim_Value_Residual_Index < m_pSimDoc->m_ValSum_Array.size()) {
				valsumres = GetValSumStruct(valsum.Sim_Value_Residual_Index);
			}

			for (size_t iii = 0; iii < m_pSimDoc->MR_Get_TotalNumberofRuns(); iii++) {
				float simvalue;
				if(valsum.SimValueSource== SIMSUMSOURCE::MULTIFILESTAT)
					simvalue= m_pSimDoc->m_MStorage.GetSumV(iii, sumindex - 6);
				else if (valsum.SimValueSource == SIMSUMSOURCE::CSVFLEXFILE) {
					if (iii < valsumres.SimValueVector.size()&& valsumres.SimValueVector[iii]>0.9*MISSING&&valsumres.ObsValueVector[iii]>0.9*MISSING)
						simvalue = valsumres.SimValueVector[iii] - valsumres.ObsValueVector[iii];
					else
						simvalue = MISSING;
				}
				else if (valsum.SimValueSource == SIMSUMSOURCE::MULTI_OUTPGFILE) {
					m_pSimDoc->m_PG_MultiOutputFile.ReOpen();

						m_pSimDoc->m_PG_MultiOutputFile.SetRepititionSequence(iii + 1, true);
						size_t ist = m_pSimDoc->m_PG_MultiOutputFile.GetRecord(valsum.MultiBin_StartMin);
						size_t iend = m_pSimDoc->m_PG_MultiOutputFile.GetRecord(valsum.MultiBin_EndMin);
						float sumacc = 0;
						size_t count = 0;
						for (size_t ii = ist; ii < iend; ii++) {
							sumacc += m_pSimDoc->m_PG_MultiOutputFile.GetVarValue(valsum.Sim_Value_Residual_Index + 1, ii);
							count++;
						}
						if (count > 0) {
							simvalue = sumacc / count;
						}
						else {
							simvalue=MISSING;
						}
					m_pSimDoc->m_PG_MultiOutputFile.CloseFile();







				}
				else {

					simvalue = 25;
					valsum.SimValueSource = SIMSUMSOURCE::MULTIFILESTAT;
				}

				
				float obsvalue;
				if (valsum.FlexSumVar) {
					obsvalue = m_pSimDoc->GetActualObsValueFromValSumFlex(valsum.IDFileName, valsum.IDVarName, iii);
				}
				else
					obsvalue = valsum.ObsValue;

				if (obsvalue > 0.9*MISSING&&simvalue > -4.2E8) {
					SimAcc += simvalue;
					ObsAcc += obsvalue;
					count++;

				}

			}
			if (ObsAcc != 0.) {
				valsum.ObsScalingFactor = SimAcc / ObsAcc;
				valsum.SimValue =float(SimAcc/count);
				valsum.ObsValue =float(ObsAcc/count);

				SetValSum(i, valsum);
			}

		}
		return true;
	}

	return false;



}
//double LOG_PI = 0.918939;
//#define LOG_PI 0.918939

double SmartFlexibleValidation::SumLikelihoodSingleValue(size_t index, double sim, double obsvalue) {
	double E_Meas;
	double out;

	m_pSimDoc = dynamic_cast<MR*>(this);
	if (m_pSimDoc == nullptr) return MISSING;
	size_t numsum = m_pSimDoc->m_ValSum_Array.size();
	if (index >= numsum) return MISSING;
	VALSUMv sumv = m_pSimDoc->m_ValSum_Array[index];


	m_pSimDoc->m_ValSum_Array[index].SimValue = float(sim);
	if (sumv.FlexSumVar) {
		//give obsvalue from file:
		sumv.ObsValue = float(obsvalue);

	}

	if (sumv.P_Error*abs(sumv.ObsValue)<sumv.A_Error)	E_Meas = sumv.A_Error;
	else if (sumv.ObsValue>0)				E_Meas = max(double(sumv.P_Error), 0.15)*abs(sumv.ObsValue);
	else if (abs(sim - sumv.ObsValue)>0)		E_Meas = sumv.P_Error*abs(sim);
	else E_Meas = 1;


Sw* pSw = dynamic_cast<Sw*>(GetPtr(SWITCH, "Likelihood Function"));
	

enum BAYES {GAUSSIAN, SIVIAS};
BAYES option;
	if (pSw != nullptr)
		option = BAYES(pSw->GetIntValue());
	else
		option=GAUSSIAN;


	if (option==GAUSSIAN) {
		out = -0.5*pow((sim - sumv.ObsValue) / E_Meas, 2) - Constants::C().LOG_PI - log(E_Meas);

	}
	else {	
		double ri = (sim - sumv.ObsValue) / E_Meas;
		if (abs(ri)<1.E-10)
			out = -Constants::C().LOG_PI - log(2 * E_Meas);
		else
			out = log(1 - exp(-0.5*pow(ri, 2))) - log(pow(ri, 2)) - Constants::C().LOG_PI - log(E_Meas);

	}

	return out;
}

bool SmartFlexibleValidation::SetValSum(size_t index, VALSUMv valsumv) {
	m_pSimDoc = dynamic_cast<MR*>(this);
	if (m_pSimDoc == nullptr) return MISSING;
	if (index < m_pSimDoc->m_ValSum_Array.size()) {
		m_pSimDoc->m_ValSum_Array[index] = valsumv;
		return true;
	}
	else
		return false;
}
