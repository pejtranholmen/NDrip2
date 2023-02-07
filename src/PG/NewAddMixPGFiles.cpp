#pragma once
#include "../../StdAfx.h"
#include "../../Std.h"
#include "../../Util.h"
#include "./NewAddMixPGFiles.h"
NewAddMixPGFiles::NewAddMixPGFiles(void) {
	m_pPGMix_Out = nullptr;
	m_pPGADD_Out = nullptr;
	m_pPGMerge_Out = nullptr;
}
NewAddMixPGFiles::~NewAddMixPGFiles(void) {
	while (m_PGPointerVector.size() > 0) {
		delete m_PGPointerVector[0];
		m_PGPointerVector.erase(m_PGPointerVector.begin());
	}
	if (m_pPGADD_Out = nullptr) delete m_pPGADD_Out;
	if (m_pPGMix_Out = nullptr) delete m_pPGMix_Out;
	if (m_pPGMerge_Out = nullptr) delete m_pPGMerge_Out;
}
void NewAddMixPGFiles::AddPGFiles() {
	vector<string> binnames;
	binnames = MFC_Util::GetBinFileNames();
	for (size_t i = 0; i < binnames.size(); i++) {
		CPG *pPG = new CPG;
		int ScalingCoef=0;
		pPG->Open(binnames[i]);
		if (pPG->IsOpen()) {
			m_PGPointerVector.push_back(pPG);
			m_AddScalingCoefVector.push_back(ScalingCoef);
		}
	}
	if(m_PGPointerVector.size()>0) SetOutPutFileFormat();
}
CPG * NewAddMixPGFiles::AddFileToNewPGFile() {
	CPG* pPG = m_pPGADD_Out;
	long numvar, numrec;
	numrec = 0;
	numvar = 10000;
	int NumLines = m_PGPointerVector.size();
	size_t num_rep = string::npos;
	bool Valid_Rep = true;

	for (int i = 0; i<m_PGPointerVector.size(); i++) {
		CPG *pCPG;
		pCPG =m_PGPointerVector[i];
		pCPG->ReOpen();
		long nrec = pCPG->GetNumRecords();
		int  nvar = pCPG->GetNumVariables();
		long pgst = pCPG->GetLongTimeStart();
		long pgend = pCPG->GetLongTimeEnd();
		auto rep = pCPG->GetNumRepititions();
		if (num_rep != string::npos) {
			if (rep != num_rep)
				Valid_Rep = false;
		}
		else {
			num_rep = rep;
			Valid_Rep = true;
		}
		if (abs(m_AddScalingCoefVector[i]) > 0)
			numrec +=nrec+ nrec*abs(m_AddScalingCoefVector[i]);
		else
			numrec += nrec;
		if (nvar<numvar)
			numvar = nvar;

	}
	pPG->SetCompleteRead(false);
	if (pPG->GetFileName() == "MyNewFile") {
		pPG->SetFileName(MFC_Util::GetBinFileName());
	}
	pPG->SaveAs(pPG->GetFileName(), true);
	pPG->CreateNewFile(numvar, numrec, num_rep);
	for (size_t repet = 0; repet < num_rep; repet++) {
		long icount = 0;
		for (int i = 0; i<m_PGPointerVector.size(); i++) {
			CPG *pCPG;
			pCPG = m_PGPointerVector[i];
			pCPG->ReOpen();
			long nrec = pCPG->GetNumRecords();
			int  nvar = pCPG->GetNumVariables();
			auto pgst = pCPG->GetLongTimeStart();
			auto pgend = pCPG->GetLongTimeEnd();
			if (m_AddScalingCoefVector[i] < 0) {
				unsigned int mintime;
				mintime = pgst-abs(m_AddScalingCoefVector[i]) * (pgend - pgst+pCPG->GetNormalTimeInterval());
				for (long j = 0; j < nrec*abs(m_AddScalingCoefVector[i]); j++) {
					float *pvar = (float*)pCPG->GetRecordPointer(j%nrec + 1, repet + 1);
					icount = icount + 1;
					pPG->SetRecordPointer(pvar);
					pPG->SetDatePointer(&mintime,0);
					pPG->WritePGFileRecord(icount, repet + 1);
					mintime += pCPG->GetNormalTimeInterval();
				}
			}
			for (long j = 0; j < nrec; j++) {
				float *pvar = (float*)pCPG->GetRecordPointer(j + 1, repet + 1);
				icount = icount + 1;
				pPG->SetRecordPointer(pvar);
				pPG->WritePGFileRecord(icount, repet + 1);
			}
			if (m_AddScalingCoefVector[i] > 0) {
				unsigned int mintime;
				mintime = pgend + pCPG->GetNormalTimeInterval();
				for (long j = 0; j < nrec*abs(m_AddScalingCoefVector[i]); j++) {
					float *pvar = (float*)pCPG->GetRecordPointer(j%nrec + 1, repet + 1);
					icount = icount + 1;
					pPG->SetRecordPointer(pvar);
					pPG->SetDatePointer(&mintime, 0);
					pPG->WritePGFileRecord(icount, repet + 1);
					mintime += pCPG->GetNormalTimeInterval();
				}
			}



		}
	}

	for (size_t repet = 0; repet < num_rep; repet++) {
		for (int i = 0; i < numvar; i++) {
			CPG *pCPG;
			float fmin, fmax;
			fmax = MISSING;
			fmin = abs(MISSING);
			pCPG = m_PGPointerVector[0];
			pPG->SetVarName(i + 1, pCPG->GetVarName(i + 1), repet + 1);
			pPG->SetVarUnit(i + 1, pCPG->GetVarUnit(i + 1), repet + 1);
			pPG->SetVarId(i + 1, pCPG->GetVarId(i + 1), repet + 1);
			pPG->SetVarPos(i + 1, pCPG->GetVarPos(i + 1), repet + 1);
			if (pCPG->GetVarMin(i + 1) < fmin)
				fmin = pCPG->GetVarMin(i + 1);
			if (pCPG->GetVarMax(i + 1) > fmax)
				fmax = pCPG->GetVarMax(i + 1);
			for (int j = 1; j < NumLines - 1; j++) {
				pCPG = m_PGPointerVector[j];
				if (pCPG->GetVarMin(i + 1) < fmin)
					fmin = pCPG->GetVarMin(i + 1);
				if (pCPG->GetVarMax(i + 1) > fmax)
					fmax = pCPG->GetVarMax(i + 1);
			}
			auto alt=pCPG->GetVarAlt(i + 1, repet + 1);
			pPG->SetVarAlt(i + 1, alt, repet + 1);
			auto longi = pCPG->GetVarLong(i + 1, repet + 1);
			pPG->SetVarLong(i + 1, longi, repet + 1);
			auto lati = pCPG->GetVarLat(i + 1, repet + 1);
			pPG->SetVarLat(i + 1, lati, repet + 1);
			auto stati = pCPG->GetVarStation(i + 1, repet + 1);
			pPG->SetVarStation(i + 1, stati, repet + 1);
			auto counti = pCPG->GetVarCountry(i + 1, repet + 1);
			pPG->SetVarCountry(i + 1, counti, repet + 1);

			pPG->SetVarMin(i + 1, fmin, repet + 1);
			pPG->SetVarMax(i + 1, fmax, repet + 1);
		}
	}
	//pPG->WritePGFileFrame();
	if (!pPG->WritePGFileHeader(numrec, num_rep)) return nullptr;
	pPG->CloseFile();

	pPG->Open(pPG->GetFileName());
	pPG->CloseFile();


	return pPG;
}
CPG * NewAddMixPGFiles::MixFileToNewPGFile() {
	CPG *pPG=m_pPGMix_Out;
	BOOL EqualTime;
	EqualTime = FALSE;
	// make addition of files
	MFC_Util m;

	long numvar, numrec, numrectest, numrecmax;
	long  irec_count, irec, irec_index;
	unsigned int itime, pgst, pgend;
	int icount;

	int nrep, kollnrep;
	bool equal_repetition = true;

	numvar = 0;
	irec_count = 0;
	numrecmax = 0;
	vector<size_t> RecordtoRead, MaxRecords, Valid;


	int NumLines = m_PGPointerVector.size();
	numvar = pPG->GetNumVariables();
	numrec = pPG->GetNumRecords();
	nrep = pPG->GetNumRepititions();;
	pPG->SetCompleteRead(false);
	pPG->SaveAs(pPG->GetFileName(), true);
	pPG->CreateNewFile(numvar, numrec, nrep);
	
	if (EqualTime) { // Same Time no intepolation needed
		int icount;
		for (int irep = 0; irep<nrep; irep++) {
			for (long j = 0; j<numrec; j++) {
				icount = 0;
				for (int i = 0; i<NumLines; i++) {
					CPG *pCPG;
					pCPG = m_PGPointerVector[i];
					numvar = pCPG->GetNumVariables();
					float *pvar = (float*)pCPG->GetRecordPointer(j + 1, irep + 1);
					if (i == 0)
						pPG->SetRecordPointer(pvar);
					else
						pPG->SetRecordPointer(pvar, icount, numvar);
					icount += numvar;
				}
				pPG->WritePGFileRecord(j + 1, irep + 1);

			}
		}
	}
	else if (m_Interpolate) { //Interpolation based on first file
		int icount;
		CPG *pCPG;
		float *pvar;
		unsigned int itime_next;

		for (size_t i = 0; i < NumLines; i++)
			RecordtoRead.push_back(1);
		for (long j = 0; j<numrec; j++) {
			icount = 0;
			for (int jj = 0; jj < NumLines; jj++) //Set Not valid for all files
				Valid.push_back(0);
			for (int i = 0; i<NumLines; i++) {
				pCPG = m_PGPointerVector[i];

				if (i == 0) {
					numvar = pCPG->GetNumVariables();
					float *pvar = (float*)pCPG->GetRecordPointer(j + 1);
					pPG->SetRecordPointer(pvar);
					itime = pCPG->GetLongTime(j + 1);
				}
				else {
					numvar = pCPG->GetNumVariables();
					bool continueread = TRUE;
					long priortime;
					while (continueread) {
						irec = RecordtoRead[i];
						itime_next = pCPG->GetLongTime(irec);

						if (itime_next == itime) {  // exact
							pvar = (float*)pCPG->GetRecordPointer(irec);
							pPG->SetRecordPointer(pvar, icount, numvar);
							priortime = itime_next;
							continueread = FALSE;
						}
						else if (itime_next<itime) { // prior step forward
							pPG->SetMissingValueRecordPointer(icount, numvar);
							priortime = itime_next;
							RecordtoRead[i]= irec + 1;
						}
						else {   //after - do interpolation
							pPG->SetMissingValueRecordPointer(icount, numvar);
							continueread = FALSE;
							// first take time back to make start point for interpolation
							if (irec>1) {
		
								float weight;
								vector<float> vfloat;
								vector<float> vprior;
								vprior = pCPG->GetRecordDataVector(irec - 1);
								vfloat = pCPG->GetRecordDataVector(irec);
								if (itime_next>priortime)
									weight = (double(itime) - double(priortime)) / (double(itime_next) - double(priortime));
								else
									weight = 1.;
								pPG->SetRecordInterpolated(vprior, icount,  weight, vfloat);

							}
						}
					}

				}
				pPG->SetLongTime(1, itime);
				icount += numvar;
			}
			pPG->WritePGFileRecord(j + 1);

		}
	}
	else { // Mixing without interpolation
		bool ifirst;

		CPG *pCPG;
		float *pvar;
		for (size_t irep = 0; irep<nrep; irep++) {
			irec_count = 0;
			RecordtoRead.resize(NumLines);
			Valid.resize(NumLines);
			MaxRecords.resize(NumLines);
			for (size_t i = 0; i < NumLines; i++) {
				RecordtoRead[i] = 1;
				MaxRecords[i]=m_PGPointerVector[i]->GetNumRecords();
			}
			while (irec_count<numrec) {
				irec_count++;
				ifirst = TRUE;
				icount = 0;
				for (size_t jj = 0; jj<NumLines; jj++) //Set Not valid for all files
					Valid[jj]= 0;
				for (int i = 0; i<NumLines; i++) { //Determine which is leading and which other are equal

					pCPG = m_PGPointerVector[i];
					if (!pCPG->IsOpen()) pCPG->ReOpen();
					irec = RecordtoRead[i];
					if (irec <= MaxRecords[i]) {
						if (ifirst) {
							itime = pCPG->GetLongTime(irec);
							irec_index = i;
							ifirst = FALSE;
							Valid[i]= 1;
						}
						else if (pCPG->GetLongTime(irec) == itime) {
							itime = pCPG->GetLongTime(irec);
							Valid[i]=1;
						}
						else if (pCPG->GetLongTime(irec)<itime) {
							itime = pCPG->GetLongTime(irec);
							irec_index = i;
							for (int jj = 0; jj<i; jj++)
								Valid[jj]= 0;
							Valid[i]= 1;
						}
					}
				}

				for (int i = 0; i<NumLines; i++) { //Set the Values 
					pCPG = m_PGPointerVector[i];
					if (!pCPG->IsOpen()) pCPG->ReOpen();
					numvar = pCPG->GetNumVariables();

					irec = RecordtoRead[i];
					pCPG->ReadNewBuffer(irec, irep + 1);
					if (irec <= MaxRecords[i]) {
						pvar = (float*)pCPG->GetRecordPointer(irec, irep + 1);
					}
					else
						pvar = (float*)pCPG->GetRecordPointer(MaxRecords[i], irep + 1);
					if (i == 0)
						pPG->SetRecordPointer(pvar);

					if (Valid[i] == 0)
						pPG->SetMissingValueRecordPointer(icount, numvar);
					else if (i>0) {
						pPG->SetRecordPointer(pvar, icount, numvar);
					}
					icount += numvar;
				}
				pPG->SetDatePointer(&itime);
				pPG->WritePGFileRecord(irec_count, irep + 1);
				if(irec_count%100==1) m.SetPaneMessage(FUtil::STD_ItoAscii(irec_count)+"("+ FUtil::STD_ItoAscii(numrec)+")");
				for (int i = 0; i<NumLines; i++) {
					if (Valid[i]>0) {
						irec = RecordtoRead[i];
						RecordtoRead[i]= irec + 1;
					}
				}

			}
		}

	}


	for (int irep = 0; irep<nrep; irep++) {
		icount = 0;
		for (int i = 0; i<NumLines; i++) {
			CPG *pCPG;
			pCPG = m_PGPointerVector[i];
			numvar = pCPG->GetNumVariables();
			for (int i = 0; i<numvar; i++) {
				icount++;
				pPG->SetVarName(icount, pCPG->GetVarName(i + 1, irep + 1), irep + 1);
				pPG->SetVarUnit(icount, pCPG->GetVarUnit(i + 1, irep + 1), irep + 1);
				pPG->SetVarId(icount, pCPG->GetVarId(i + 1, irep + 1), irep + 1);
				pPG->SetVarPos(icount, pCPG->GetVarPos(i + 1, irep + 1), irep + 1);

				pPG->SetVarLong(icount, pCPG->GetVarLong(i + 1, irep + 1), irep + 1);
				pPG->SetVarLat(icount, pCPG->GetVarLat(i + 1, irep + 1), irep + 1);
				pPG->SetVarAlt(icount, pCPG->GetVarAlt(i + 1, irep + 1), irep + 1);
				pPG->SetVarCountry(icount, pCPG->GetVarCountry(i + 1, irep + 1), irep + 1);
				pPG->SetVarStation(icount, pCPG->GetVarStation(i + 1, irep + 1), irep + 1);
				pPG->SetVarStation(icount, pCPG->GetVarStation(i + 1, irep + 1), irep + 1);
			}
		}


	}

	//if(pPG->GetCompleteRead())

	pPG->WritePGFileHeader(numrec, nrep);



	pPG->CloseFile();
	pPG->Open(pPG->GetFileName());
	pPG->MakeFileToEnableWriting(false);
	for (int irep = 0; irep < nrep; irep++)
		pPG->ReCalculateMinMax(irep + 1);

	pPG->WritePGFileHeader(numrec, nrep);


	pPG->CloseFile();
	//else
	//pPG->WritePGFile();
	//pp->OnInitDialog();

	return pPG;
}
CPG * NewAddMixPGFiles::MergeFileToNewPGFile() {
	CPG *pPG=m_pPGMerge_Out;

	size_t numrec = 0;
	size_t numvar = 10000;
	int NumOfFilesToMerge = m_PGPointerVector.size();

	int nrepetitions = 0;

	for (int i = 0; i<NumOfFilesToMerge; i++) {
		CPG *pCPG;
		pCPG = m_PGPointerVector[i];
		long nrec = pCPG->GetNumRecords();
		int  nvar = pCPG->GetNumVariables();
		auto nrep = pCPG->GetNumRepititions();
		long pgst = pCPG->GetLongTimeStart();
		long pgend = pCPG->GetLongTimeEnd();
		numrec = nrec;
		if (nvar<numvar)
			numvar = nvar;
		nrepetitions += nrep;
	}
	pPG->SaveAs(pPG->GetFileName(), true);
	pPG->CreateNewFile(numvar, numrec, nrepetitions);
	int count_rep = 1;

	for (int i = 0; i<NumOfFilesToMerge; i++) {
		CPG *pCPG;

		pCPG = m_PGPointerVector[i];
		pCPG->ReOpen();
		long nrec = pCPG->GetNumRecords();
		int  nvar = pCPG->GetNumVariables();
		auto nrepit = pCPG->GetNumRepititions();

		long pgst = pCPG->GetLongTimeStart();
		long pgend = pCPG->GetLongTimeEnd();
		for (size_t nrep = 0; nrep<nrepit; nrep++) {
			for (long j = 0; j<nrec; j++) {
				float *pvar = (float*)pCPG->GetRecordPointer(j + 1, nrep + 1);
				pPG->SetRecordPointer(pvar);
				pPG->WritePGFileRecord(j + 1, count_rep);
			}

			for (int iv = 0; iv<numvar; iv++) {
				float fmin, fmax;
				fmax = MISSING;
				fmin = abs(MISSING);
				pPG->SetVarName(iv + 1, pCPG->GetVarName(iv + 1, nrep + 1), count_rep);
				pPG->SetVarUnit(iv + 1, pCPG->GetVarUnit(iv + 1, nrep + 1), count_rep);
				pPG->SetVarId(iv + 1, pCPG->GetVarId(iv + 1, nrep + 1), count_rep);
				pPG->SetVarPos(iv + 1, pCPG->GetVarPos(iv + 1, nrep + 1), count_rep);

				pPG->SetVarLat(iv + 1, pCPG->GetVarLat(iv + 1, nrep + 1), count_rep);
				pPG->SetVarLong(iv + 1, pCPG->GetVarLong(iv + 1, nrep + 1), count_rep);
				pPG->SetVarAlt(iv + 1, pCPG->GetVarAlt(iv + 1, nrep + 1), count_rep);
				pPG->SetVarKey(iv + 1, pCPG->GetVarKey(iv + 1, nrep + 1), count_rep);
				pPG->SetVarCountry(iv + 1, pCPG->GetVarCountry(iv + 1, nrep + 1), count_rep);
				pPG->SetVarStation(iv + 1, pCPG->GetVarStation(iv + 1, nrep + 1), count_rep);

				if (pCPG->GetVarMin(iv + 1, nrep + 1)<fmin)
					fmin = pCPG->GetVarMin(iv + 1, nrep + 1);
				if (pCPG->GetVarMax(iv + 1), nrep + 1>fmax)
					fmax = pCPG->GetVarMax(iv + 1, nrep + 1);
				for (int j = 1; j<NumOfFilesToMerge - 1; j++) {
					if (pCPG->GetVarMin(iv + 1, nrep + 1)<fmin)
						fmin = pCPG->GetVarMin(iv + 1, nrep + 1);
					if (pCPG->GetVarMax(iv + 1, nrep + 1)>fmax)
						fmax = pCPG->GetVarMax(iv + 1, nrep + 1);
				}
				pPG->SetVarMin(iv + 1, fmin, count_rep);
				pPG->SetVarMax(iv + 1, fmax, count_rep);
			}
			count_rep++;

		}

		pCPG->CloseFile();
	}
	pPG->WritePGFileHeader(numrec, nrepetitions);


	return pPG;
}
bool NewAddMixPGFiles::DeleteFile(size_t index)
{
	delete m_PGPointerVector[index];
	if (m_PGPointerVector.size() > index) {
		m_PGPointerVector.erase(m_PGPointerVector.begin() + index);
		SetOutPutFileFormat();
		return true;
	}
	else
		return false;
}
bool NewAddMixPGFiles::DeleteFiles()
{
	while (m_PGPointerVector.size() > 0) {
		delete m_PGPointerVector[0];
		m_PGPointerVector.erase(m_PGPointerVector.begin());
	}
	SetOutPutFileFormat();
	return true;
}
void NewAddMixPGFiles::DefineOutPutFileName(string FileName) {
	if (m_pPGADD_Out == nullptr) m_pPGADD_Out = new CPG();

	m_pPGADD_Out->SetFileName(FileName);

}
bool NewAddMixPGFiles::SetOutPutFileFormat()
{

	CPG* pPG = m_pPGADD_Out;
	if(pPG == nullptr) pPG=m_pPGMix_Out;
	if(pPG == nullptr) pPG=m_pPGMerge_Out;
	if (pPG == nullptr) return false;
	if (m_PGPointerVector.size() == 0) return false;

	long numvar, numrec;

	numrec = 0;
	numvar = 10000;
	int NumLines = m_PGPointerVector.size();
	size_t num_rep = string::npos;
	bool Valid_Rep = true;
	bool Valid_Merge = true;


	for (int i = 0; i<m_PGPointerVector.size(); i++) {
		CPG *pCPG;
		pCPG = m_PGPointerVector[i];
		pCPG->ReOpen();
		long nrec = pCPG->GetNumRecords();
		int  nvar = pCPG->GetNumVariables();
		long pgst = pCPG->GetLongTimeStart();
		long pgend = pCPG->GetLongTimeEnd();
		auto rep = pCPG->GetNumRepititions();




		if (m_pPGADD_Out != nullptr) {
			if (num_rep != string::npos) {
				if (rep != num_rep)
					Valid_Rep = false;
			}
			else {
				num_rep = rep;
				Valid_Rep = true;
			}
			numrec += nrec;
			if (nvar < numvar)	numvar = nvar;
		}
		else if (m_pPGMix_Out != nullptr) {

		}
		else if (m_pPGMerge_Out != nullptr) {
			if (i == 0) {
				num_rep = 0;
				numrec = nrec;
			}
			if(numrec!=nrec)
				Valid_Merge = false;
			
			if (nvar < numvar)
				numvar = nvar;
			else if (nvar != numvar)
				Valid_Merge = false;

			num_rep += rep;
		}

	}
	if (m_pPGADD_Out != nullptr) {
		if (!Valid_Rep) {
			string info;
			info = "Note that you can not Create a new file if you have \n";
			info+= "different number of repetitions in the input files";
			MFC_Util::MessageBoxW(info);
		}
		//pPG->SaveAs(pPG->GetFileName(), true);
		return pPG->CreateNewFile(numvar, numrec, num_rep);
	}
	else if (m_pPGMix_Out != nullptr) {
		 SetSizeOfMixFile();

	}
	else if (m_pPGMerge_Out != nullptr) {
		if (!Valid_Merge) {
			string info;
			info = "Note that you can not Create a new file if you have \n";
			info += "different number of Variable or different numbers of records in the input files";
			MFC_Util::MessageBoxW(info);
		}
		pPG->SaveAs(pPG->GetFileName(), true);
		return pPG->CreateNewFile(numvar, numrec, num_rep);

	}
	return true;
}

bool NewAddMixPGFiles::SetSizeOfMixFile()
{
	CPG *pPG = m_pPGMix_Out;
	BOOL EqualTime;
	EqualTime = TRUE;
	// make addition of files

	long numvar, numrec, numrectest, numrecmax;
	long itime, irec_count, irec, irec_index;
	long pgst, pgend;
	int icount;

	int nrep, kollnrep;
	bool equal_repetition = true;

	numvar = 0;
	irec_count = 0;
	numrecmax = 0;
	CUIntArray RecordtoRead, MaxRecords, Valid;

	int NumLines = m_PGPointerVector.size();
	numrec = -10;

	for (int i = 0; i<NumLines ; i++) {


		CPG *pCPG;
		pCPG = m_PGPointerVector[i];
		pCPG->ReOpen();
		long nrec = pCPG->GetNumRecords();
		int  nvar = pCPG->GetNumVariables();
		nrep = pCPG->GetNumRepititions();
		if (i == 0) kollnrep = nrep;
		if (kollnrep != nrep) {
			equal_repetition = false;

		}
		RecordtoRead.Add(1);
		MaxRecords.Add(nrec);
		Valid.Add(0);
		numrecmax += nrec;

		if (i == 0) {
			numrec = nrec;
			numrectest = nrec;
			itime = pCPG->GetLongTimeStart();
		}
		else {
			if (numrectest != nrec)
				EqualTime = FALSE;
			if (pgst != pCPG->GetLongTimeStart())
				EqualTime = FALSE;
			if (pgend != pCPG->GetLongTimeEnd())
				EqualTime = FALSE;
		}
		pgst = pCPG->GetLongTimeStart();
		if (itime>pgst)
			itime = pgst;
		pgend = pCPG->GetLongTimeEnd();
		if (nvar>0)
			numvar += nvar;

	}
	if (!EqualTime) {
		if (!m_Interpolate) {
			bool ifirst;
			bool continueread;
			continueread = TRUE;
			irec_count = -1;
			while (continueread) {
				irec_count++;
				ifirst = TRUE;
				for (int jj = 0; jj<NumLines; jj++) //Set Not valid for all files
					Valid.SetAt(jj, 0);
				for (int i = 0; i<NumLines; i++) {
					CPG *pCPG;
					pCPG = m_PGPointerVector[i];
					irec = RecordtoRead.GetAt(i);
					if (irec <= MaxRecords.GetAt(i)) {

						if (ifirst) {
							itime = pCPG->GetLongTime(irec);
							irec_index = i;
							Valid.SetAt(i, 1);
							ifirst = FALSE;
						}
						else if (pCPG->GetLongTime(irec)<itime) {
							itime = pCPG->GetLongTime(irec);
							irec_index = i;
							Valid.SetAt(i, 1);
							for (int jj = 0; jj<i; jj++)
								Valid.SetAt(jj, 0);
						}
						else if (pCPG->GetLongTime(irec) == itime) {
							Valid.SetAt(i, 1);

						}
					}
				}
				continueread = FALSE;
				for (int i = 0; i<NumLines; i++) {
					if (Valid.GetAt(i)>0) {
						irec = RecordtoRead.GetAt(i);
						irec++;
						RecordtoRead.SetAt(i, irec);
						continueread = TRUE;
					}
				}
			}
		}
		else {
			CPG *pCPG;
			pCPG = m_PGPointerVector[0];
			irec_count = pCPG->GetNumRecords();

		}
		numrec = irec_count;
	}
	if (numrec<0) return FALSE;
	if (!equal_repetition) nrep = 1;
	//pPG->CreateNewFile(numvar,numrec);
	pPG->SaveAs(pPG->GetFileName(), TRUE);
	pPG->SetCompleteRead(FALSE);
	return pPG->CreateNewFile(numvar, numrec, nrep);
}
