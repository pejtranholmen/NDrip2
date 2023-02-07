#pragma once

struct PGSIZE {
	size_t numvar;
	size_t numrec;
	size_t nrep;
};
class CPG;
class NewAddMixPGFiles {
public:
	NewAddMixPGFiles();
	~NewAddMixPGFiles();
	void AddPGFiles();
	CPG* AddFileToNewPGFile();
	CPG* MixFileToNewPGFile();
	CPG* MergeFileToNewPGFile();
	bool DeleteFiles();
	bool DeleteFile(size_t index);

	void DefineOutPutFileName(string filename);
	bool SetOutPutFileFormat();
	bool SetSizeOfMixFile();

	enum pgfilemethod m_PGMethod;
	vector<CPG*> m_PGPointerVector;
	vector<int>  m_AddScalingCoefVector;

	CPG* m_pPGADD_Out, *m_pPGMix_Out, *m_pPGMerge_Out;
	bool m_Interpolate;
	PGSIZE m_OutSize;
};