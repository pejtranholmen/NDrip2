#pragma once
#include "./ModelBase.h"

class ModelFiles :public ModelBase {

public:
	ModelFiles();
	~ModelFiles();

	string GetOutputFileName(size_t ValidIndex = 0, bool Last = false, bool MultiModeFile = false, size_t PGMean = 0, size_t SimRunNo = 0, bool NewFile = false);
	string GetNewOutputFileName(size_t ValidIndex = 0, bool Last = false, bool MultiModeFile = false, size_t PGMean = 0, size_t SimRunNo = 0);
	string GetOldOutputFileName(size_t ValidIndex = 0, bool Last = false, bool MultiModeFile = false, size_t PGMean = 0, size_t SimRunNo = 0);


	F* ValidationFilePointer(size_t ValFileIndex = 1);
	CResiduals* GetResidualPointer(size_t iFile = 1, size_t addedfiles = 0);
	CResiduals* GetValidResidualPointer(size_t iFile = 1, size_t addedfiles = 0);
	bool	MR_XBin_IsOpen(size_t, size_t addfile = 0);
	bool  ResidualFile_Reset();
	bool ResidualFile_AddFile(size_t num);
	bool ResidualFile_RemoveFile(size_t index);
	bool ResidualFile_AddAttachedFiles(size_t num);
	bool ResidualFile_SetNumAttachedFiles(size_t num);

	vector< vector<CResiduals*>> m_ResidualFileVector;


protected:
	size_t m_NumValidFiles;

};
