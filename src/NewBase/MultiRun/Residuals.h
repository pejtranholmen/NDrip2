#pragma once

#include "./ResidualsBase.h"

// CResiduals command target
class NewMap;


class CResiduals : public ResidualsBase
{
public:
	CResiduals();
	virtual ~CResiduals();
	bool OpenFile(std::string, bool);
	bool CreateFile(std::string filename, size_t NumRec, size_t RecL, size_t NumV);
	bool AddResiduals(float *pFloat);
	bool SaveMeanValues();
	bool ReadMeanValues();
	bool ReOpenFile(bool ReadOnly=false);
	bool ReadHeader();
	size_t GetMaxNumRec() {return m_MaxNumRec;};

	float GetResiduals(size_t, size_t, size_t);
	std::vector<float> GetResidualVector(size_t, size_t);
	std::vector<size_t> GetAcceptedRunVector();
	int m_ErrorCount;

	float GetLogLi(size_t, size_t, size_t, bool);

	float GetLogLiMean(size_t, size_t, bool);
	float GetLogLiCV(size_t, size_t, bool);
	float GetLogLiPMIN(size_t, size_t, bool);
	float GetLogLiP5(size_t, size_t, bool);
	float GetLogLiP50(size_t, size_t, bool);
	float GetLogLiP95(size_t, size_t, bool);
	float GetLogLiPMAX(size_t, size_t, bool);

	float GetZ_ScoreLogLiP5(size_t, size_t, bool);
	float GetZ_ScoreLogLiP95(size_t, size_t, bool);


	float GetMeanResiduals(size_t, size_t);
	float GetMinResiduals(size_t, size_t);
	float GetMaxResiduals(size_t, size_t);
	float GetStdResiduals(size_t, size_t);
	float GetP10Residuals(size_t, size_t);
	float GetP50Residuals(size_t, size_t);
	float GetP90Residuals(size_t, size_t);
	
	float GetMeanAllResiduals(size_t, size_t);
	float GetMinAllResiduals(size_t, size_t);
	float GetMaxAllResiduals(size_t, size_t);
	float GetStdAllResiduals(size_t, size_t);
	float GetP10AllResiduals(size_t, size_t);
	float GetP50AllResiduals(size_t, size_t);
	float GetP90AllResiduals(size_t, size_t);

	size_t  GetNumAccepted();
	void SetAccepted(size_t, bool);
	bool GetAccepted(size_t);
	void CloseFile();
	bool SmartUpdateStatistics(bool AcceptedOnly = false, void *pScanner = nullptr);
	bool ToBeUpdated();
	bool AddExistingFile(std::string, void *pScanner=nullptr);
	bool ReWriteExistingFile(size_t);
	void InitDoc(NewMap*, int fileno=1);

	
	void ExitAddLogLiFile();
	void InitExtraLogLiFile();
	void ResetExtraLogLiFile();
	bool IsExtraLogLiFile();
	int	 AddLogLi(float *, size_t);
	
	void ExitSortedResidualFile();
	void InitSortedResidualFile();
	void ResetSortedResidualFile();
	bool IsSortedResidualFile();
	int	 AddSortedResiduals(float *, size_t);
	void ForcedUpdateofIndicators() {m_IsUpdated_Extra=m_IsAllUpdated_Extra=false;};

	bool UpdateStatisticsOfIndicators(bool AcceptedOnly=false, void *pScanner=nullptr);
	
	bool m_Calculating;
protected:
	float p95, p50, p5, cv;
	size_t m_PrevRun, m_PrevVar;
	size_t m_TotNoRuns, m_OldRuns, m_NewRuns;
	size_t m_NumSum, m_NumValid;
	size_t m_MaxNumRec;
	size_t m_NumRecAdd, m_NumVarAdd, m_RecLAdd;
	size_t m_ExtraRecL;

	bool m_Allocated;
	std::fstream m_ResidualFile, m_AddFile, m_ExtraFile, m_SortedResidualFile;

	bool m_IsAllUpdated, m_IsAllUpdated_Extra;



	std::vector<float> m_CurrentRes;
	size_t m_NumAccepted;
	float *m_MeanOfAccepted, *m_MinOfAccepted,*m_MaxOfAccepted,*m_StdOfAccepted,*m_P10OfAccepted,*m_P90OfAccepted,*m_P50OfAccepted;
	float *m_MeanOfAll, *m_MinOfAll,*m_MaxOfAll,*m_StdOfAll, *m_P10OfAll, *m_P90OfAll,  *m_P50OfAll;
	float *m_LogLiNew;
	bool m_NewLogLiFileExist, m_NewLogLiFileOpen;
	float *m_SortedResiduals;
	bool m_SortedFileExist, m_SortedFileOpen;



private:


	size_t m_SavedEnsembles;
	std::streamsize m_FileSize;
	

};


