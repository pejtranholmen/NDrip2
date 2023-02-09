#pragma once

#include <vector>
#include <map>
#include "../../Util/ReadWriteTypes.h"

// ResidualsBase command target
class NewMap;

struct RESIDUAL_MEAN{
	std::vector<std::vector <float>> mean;
	std::vector<std::vector <float>> min;
	std::vector<std::vector <float>> max;
	std::vector<std::vector <float>> std;
	std::vector<std::vector <float>> p10;
	std::vector<std::vector <float>> p50;
	std::vector<std::vector <float>> p90;
};
struct POS_ENSEMBLE {
	bool valid;
	size_t pos;
	std::string name;
	bool EnsembleFileRead;
};

class ResidualsBase 
{
public:
	ResidualsBase();
	virtual ~ResidualsBase();
	bool EnsembleReadWrite(bool reading, std::string EnsembleName, std::string NewEnsembleName="", size_t ensembleno=std::string::npos);
	bool EnsembleReadWrite(bool reading, size_t ensembleno);

	bool ReadWrite_Header(bool reading);
	bool ReadWrite_PriorStat();
	bool ReadWrite_Ensemble(std::string EnsembleName);
	bool OpenResidualFile(bool reading=false);
	bool CloseResidualFile(bool reading);
	bool Residuals_ReadWrite_Header(bool reading);
	bool SetNewEnsembleByNumber(size_t ensembleno);
	bool AlloResVector();
	std::string GetXStatFileName(size_t ensembleno=std::string::npos);
	bool IsOpen(bool reading=true);
	bool EnableEstimatesMade(size_t enableno=0);
	bool m_IsUpdated, m_IsUpdated_Extra;
	bool ShiftEnsembleName(std::string oldname, std::string newname);
	void SetResidualBaseFileNumVarScaling(size_t NumVarScaling) { m_ScalingNumVarWithDim1 = NumVarScaling; };
	bool m_MeanDefined;

protected:
	NewMap *m_pNewMap;
	std::vector<float> m_Mean;
	RESIDUAL_MEAN m_MeanPrior;
	RESIDUAL_MEAN m_MeanPost;
	RESIDUAL_MEAN m_MeanTest;
	std::vector<POS_ENSEMBLE> m_Ensembles;
	std::vector<float> m_Residuals;
	size_t m_RecL, m_NumRec, m_NumVar, m_ScalingNumVarWithDim1;
	std::string m_FileName, m_ExtraFileName, m_SortedFileName;
	bool m_IsOpen, m_NewPrior;
	bool m_EnsembleFileRead;
	int m_FileNo;
	std::string m_NewEnsembleName;

private :
	ReadWriteTypes m_EnsembleResFile, m_ResidualFile, m_ExtraFile, m_SortedFile;
	bool meanValid;
	size_t m_MaxNumRec;
	std::streamsize m_FileSize;
	std::map<std::string, size_t> m_MapofEnsemblePositions;
	std::streampos m_PosToRead, m_PosToWrite;
	std::string m_EnsembleName;
	std::string m_TempOutFile;
	bool m_TwoFilesOpen;
	size_t m_SizeOfMeanVector;



};


