#ifndef COUPSTD
#include "../stdafx.h"
#include "../util/MFC_Util.h"
#else
#include "../std.h"
#endif


#include "../Util/FUtil.hpp"
#include "./ModelFiles.h"

ModelFiles::ModelFiles() {
	m_NumValidFiles = 0;
	m_ResidualFileVector.resize(m_NumValidFiles+1);
	for (size_t i = 0; i < m_ResidualFileVector.size(); i++) {
		for (size_t ii = 0; ii < 1; ii++) {
			CResiduals *pNew = new CResiduals;
			m_ResidualFileVector[i].push_back(pNew);
		}
	}
}
ModelFiles::~ModelFiles() {
	ResidualFile_Reset();

};
string ModelFiles::GetNewOutputFileName(size_t ValidIndex, bool LastFile, bool MultiModeFile, size_t MeanPGType, size_t MultiIndex)
{
	return GetOutputFileName(ValidIndex, LastFile, MultiModeFile, MeanPGType, MultiIndex, true);
}
string ModelFiles::GetOldOutputFileName(size_t ValidIndex, bool LastFile, bool MultiModeFile, size_t MeanPGType, size_t MultiIndex)
{
	string filename = GetOutputFileName(ValidIndex, LastFile, MultiModeFile, MeanPGType, MultiIndex);

	if (FUtil::IsFileExisting(filename))
		return filename;
	else
		return "";

}
string ModelFiles::GetOutputFileName(size_t ValidIndex, bool LastFile, bool MultiModeFile, size_t MeanPGType, size_t MultiIndex, bool NewFile)
{

	string filename;
	string stdbinfilename;

	string dir;

	size_t pos = m_CurrentFile.rfind("\\");
	pos = m_DocFileName.rfind("\\");
	if (pos>0)
		dir = m_DocFileName.substr(0, pos + 1);
	else {
		string dir = GetCurrentWorkDirectory();
		//dir=::GetProfileString("UserDirectory", m_UserDirectory);
	}

	string str;
	string numstr;
	bool FirstTry, SecondTry;
	long actRunNo;
	numstr = "";

	// Get Directory and ActRunNo and String of RunNo of MultiRuns
	if (IsMultiSimulation()) {
		string NewFolder = m_DocFileName;
		size_t ipp = NewFolder.find("_Multi");
		size_t ipe = NewFolder.rfind(".Sim");
		if(ipe==string::npos) ipe = NewFolder.rfind(".xml");
		size_t ll;
		ll = 5;
		if (ipe == string::npos || ipp == string::npos) {
#ifndef COUPSTD
		//	MFC_Util::MessageBox(NewFolder);
#endif

		}
		if (ipe>ipp) ll = ipe - ipp - 7;
		numstr = NewFolder.substr(ipp + 7, ll);
		if (MeanPGType == 0) {
			dir = NewFolder.substr(0, ipp) + "\\";
			actRunNo = MR_Get_ActualRunNumber();
		}
		else if (MeanPGType == 1) {
			dir += NewFolder.substr(0, ipp) + "_MEAN_ALL_" + numstr + ".Bin";

		}
		else if (MeanPGType == 2) {
			dir += NewFolder.substr(0, ipp) + "_MEAN_ACCEPTED_" + NewFolder.substr(ipp + 1, 5) + ".Bin";
		}
	}
	else
		actRunNo = m_DocFile.m_SimulationRunNo;

	string test = m_DocFileName;

	bool OldNumInFile, NewNumInFile;
	NewNumInFile=OldNumInFile = false;
	size_t ipos_sim = test.find(".Sim");
	if(ipos_sim==string::npos)	ipos_sim = test.find(".xml");
	size_t iposext = test.rfind("_");
	if (ipos_sim - iposext == 4) {
		numstr = test.substr(iposext+1, 3); OldNumInFile = true;
	}
	else if (ipos_sim - iposext == 7) {
		NewNumInFile = true;
		numstr = test.substr(iposext + 1, 6); NewNumInFile = true;

	}
	else if (ipos_sim - iposext == 6) {
		NewNumInFile = true;
		numstr = test.substr(iposext+1, 5); NewNumInFile = true;

	}
	else if (ipos_sim - iposext == 5) {
		NewNumInFile = true;
		numstr = test.substr(iposext+1, 4); NewNumInFile = true;
	}
	if (!IsMultiSimulation()&&numstr.size()>2) {
		str = numstr;
	}
	else if (IsMultiSimulation() && LastFile&&ValidIndex == 0) {
		size_t in = m_DocFile.m_SimulationRunNo - actRunNo + MR_Get_TotalNumberofRuns();
		if (in<0) in += 99999;
		str = FUtil::STD_ItoAscii(in);
	}
	else if (ValidIndex == 0 && IsMultiSimulation() && MultiModeFile&&MultiIndex>0) {
		int in = m_MStorage.GetFirstRunNoAddedRuns(MultiIndex - 1);
		str = FUtil::STD_ItoAscii(in);
	}
	else if (ValidIndex>0 && IsMultiSimulation() && (ValidationFilePointer(ValidIndex)->m_Exist) || MultiModeFile || MeanPGType>0) {
		str = numstr;
	}
	else
		str = FUtil::ItoNumAscii(m_DocFile.m_SimulationRunNo);


	stdbinfilename = "COUP_" + str + ".Bin";

	FirstTry = true;
	SecondTry = true;

	//	if(FUtil::IsFileExisting(

	if (ValidIndex>0) {
		filename = dir + "V" + FUtil::STD_ItoAscii(ValidIndex) + "_" + stdbinfilename;
	}
	else if (MultiModeFile) {
		filename = dir + "Multi_" + stdbinfilename;
	}
	else if (MeanPGType>0)
		filename = dir;
	else {
		filename = dir + stdbinfilename;
	}


	if (!NewFile&&!FUtil::IsFileExisting(filename)) {
		while (filename.rfind("_0") != string::npos&&!FUtil::IsFileExisting(filename)) {
			filename.replace(filename.rfind("_0"), 2, "_", 1);
		}
		
	}
	if (ValidIndex == 0 && FUtil::IsFileExisting(filename)) SetOutputFileNameToOutputFile(filename);
    
    cout <<"final file name: " << filename << endl;
	return filename;
}

F* ModelFiles::ValidationFilePointer(size_t Index)
{
	F* pF = nullptr;

	string name = "Validation File";
	if (Index>0 && Index <= MAXSIMVAL)
		name += FUtil::STD_ItoAscii(Index);
	else
		name += "1";
	string grp = "Validation";
	string typ = "Files";
	pF = dynamic_cast<F*>(GetPtr(simtype::PGFILE, name));
	return pF;
}
CResiduals* ModelFiles::GetResidualPointer(size_t ifile, size_t addedfiles) {
	if (ifile <= m_ResidualFileVector.size()) {
		if (addedfiles<m_ResidualFileVector[ifile - 1].size())
			return m_ResidualFileVector[ifile - 1][addedfiles];
	}
	return nullptr;
}


CResiduals* ModelFiles::GetValidResidualPointer(size_t ifile, size_t addedfiles)
{
	if (!MR_XBin_IsOpen(ifile, addedfiles))
		return nullptr;
	return GetResidualPointer(ifile, addedfiles);

}
bool ModelFiles::MR_XBin_IsOpen(size_t ifile, size_t addfile)
{
	string name;
	if (ifile == 6) {
		name = GetXBinFileName(ifile, addfile);
		

	}
	if (!m_OutputValidXBinFileExist || !IsMultiSimulation()) return false;
	name = GetXBinFileName(ifile, addfile);
	if (ValidationFilePointer(ifile)->m_XBinExist) return true;
	else if (m_ResidualFileVector.size() == 0) return false;
	if (m_ResidualFileVector.size() <= ifile - 1) return false;

	
	return m_ResidualFileVector[ifile - 1][addfile]->OpenFile(name.c_str(), true);
}
bool ModelFiles::ResidualFile_Reset() {
	for (size_t i = 0; i < m_ResidualFileVector.size(); i++) {
		for (size_t ii = 0; ii < m_ResidualFileVector[i].size(); ii++) {
			delete 	m_ResidualFileVector[i][ii];
		}
		m_ResidualFileVector[i].clear();
	}
	m_ResidualFileVector.clear();
	return true;
}
bool ModelFiles::ResidualFile_AddFile(size_t num) {
	size_t numadd;
	size_t init = m_ResidualFileVector.size();

	if (m_ResidualFileVector.size() > 0)
		numadd = m_ResidualFileVector[0].size();
	else
		numadd = 1;
	vector<CResiduals*> addvector;

	for (size_t i = 0; i <= num; i++ ) {

		addvector.clear();
		for (size_t ii = 0; ii < numadd; ii++) {
			CResiduals * pRes = new CResiduals;
			pRes->InitDoc(((NewMap*)this),init+1+ i);
			addvector.push_back(pRes);
		}

		m_ResidualFileVector.push_back(addvector);
	}
	if (m_ResidualFileVector.size() - init == num)
		return true;
	else
		return false;
}
bool ModelFiles::ResidualFile_RemoveFile(size_t index) {

	if (m_ResidualFileVector.size() > index) {
		vector<CResiduals*> addvector;
		addvector = m_ResidualFileVector[index];
		for (size_t i = 0; i < addvector.size(); i++)
			delete addvector[i];
		m_ResidualFileVector.erase(m_ResidualFileVector.begin() + index);
		return true;
	}
	return false;
}
bool ModelFiles::ResidualFile_AddAttachedFiles(size_t num) {
	for (size_t i = 0; i < m_ResidualFileVector.size(); i++) {
		for (size_t ii = 0; ii < num; ii++) {
			CResiduals *pRes = new CResiduals;
			pRes->InitDoc(((NewMap*)this), i + 1);
			m_ResidualFileVector[i].push_back(pRes);
		}
	}
	return true;
}
bool ModelFiles::ResidualFile_SetNumAttachedFiles(size_t num) {
//Reset to no attached files
	bool needupdate = false;
	for (size_t i = 0; i < m_ResidualFileVector.size(); i++) {
		if (m_ResidualFileVector[i].size() != num+1) needupdate = true;
		while (m_ResidualFileVector[i].size()>1&&needupdate) {
			delete m_ResidualFileVector[i][m_ResidualFileVector[i].size() - 1];
			m_ResidualFileVector[i].erase(m_ResidualFileVector[i].begin()+ m_ResidualFileVector[i].size() - 1);
		}
	}
//add new files
	if (needupdate) {
		for (size_t i = 0; i < m_ResidualFileVector.size(); i++) {
			for (size_t ii = 0; ii < num; ii++) {
				CResiduals *pRes = new CResiduals;
				pRes->InitDoc(((NewMap*)this), i + 1);
				m_ResidualFileVector[i].push_back(pRes);
			}
		}
	}
	return true;
}
