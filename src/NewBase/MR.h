#pragma once
#include "./Validation/ValidationData.h"
#include "./Validation/SmartFlexibleValidation.h"
#include "../PG/PGBase.h"
#include "./Bayes/BayesStatistics.h"
#include "./Nelder_Mead/optimizer.h"

struct NEWHEADER;
struct PAR_OPTIONS
{
	Par *pSimB;
	size_t options;

};
enum class NelderMeadOjective{ SSE,AIC, BIC };

class MR : public BayesStatistics,public NelderMeadOptimizer, public SmartFlexibleValidation
{
	
public:

	MR();
	~MR();
	vector<size_t> m_ExtraMonitor;
	vector<float> m_PBayesMean,m_PBayesCovar, m_PBayesCocor, m_PValues;
	vector<DistStat> m_AcceptedParDist, m_AcceptedValDist;
	DistStat GetPostDist(size_t index, size_t ensemble=string::npos) {
		if (ensemble == string::npos) {
			if (index < m_AcceptedParDist.size())
				return m_AcceptedParDist[index];
			else {
				DistStat a = m_DistDefault; return a;
			}
		}
		else {

			return m_MStorage.GetParDist(index, ensemble);



		}

	
	
	};
	size_t GetNumBurnIn() const {return Num_Burn;};
	void SetNumBurnIn(size_t value) {Num_Burn=value;};
	vector<SIMB> m_P_MR_Array;
	ValidationData m_ValidationData;
	size_t MR_GetNumExtraMonitor();
	size_t MR_GetExtraMonitor(size_t);
	size_t m_MultiRunsOriginal;
	Sw* m_pValidIndexSet;
	Ps* m_pValidIndex, *m_pValidIndexRep, *m_pValidIndexLoopSize;


	size_t MR_GetAddedFiles(size_t RunNo);
	size_t MR_GetInternalRunNo(size_t RunNo);

	bool MR_RemoveExtraMonitor(size_t index);
	bool MR_ResetMonitoring();
	bool MR_ResetValMonitoring();
	void MR_SetResetMonitoring();
	void MR_SetResetValMonitoring();
	bool m_ResetMonitoring, m_ResetValMonitoring;
	void MR_AddExtraMonitor(size_t index);
	string MR_GetExtraLabel(size_t index);

	string m_HelpName;
	string m_ExeFileDate;
	string MR_GetParGroup(size_t, size_t);
	string MR_GetParName(size_t, size_t);
	SIMB MR_Get_PAR(size_t, size_t);
	SimB*	MR_Get_pPar(size_t, size_t);
	size_t	MR_Get_NumberOfParametersWithinDim(size_t);
	size_t	MR_GetMethod(size_t,size_t);
	string MR_GetMethodName(size_t, size_t);
	size_t		MR_GetTabIndex(size_t, size_t);
	size_t		MR_GetParType(size_t, size_t);
	size_t		m_NumOutputs;  



	double 	MR_GetTableValue(size_t, size_t, size_t);
	double	MR_GetStart(size_t, size_t);
	double	MR_GetMin(size_t, size_t);
	double	MR_GetMin(size_t);
	size_t	MR_GetMonitoring(size_t, size_t);
	void	MR_SetMonitoring(size_t, size_t, size_t);
	double	MR_GetMax(size_t, size_t);
	double	MR_GetMax(size_t);
	bool	MR_GetLogDist(size_t);
	string MR_GetParDependence(size_t, size_t);
	void	MR_SetParDependence(size_t, size_t, string);	
	void	MR_Background();
	float   m_BayesianStepSize;
	float MR_GetBayesianStepSize();
	void MR_SetBayesianStepSize(float);
	bool UpdateCoVar();
	bool m_MultiStopped;
	void Set_MultiStopped();
	bool IsMultiStopped();
	void SetMBinFileName(string value);
	void Set_ReReadMStorageFile();
	void MR_Reset_MeanResiduals();
	size_t MR_GetSimTimeLeft(size_t);
	size_t m_TimeOneRun;
	void MR_SetSimTimeUsed();
	void AcceptAll();

	size_t	MR_Get_TotalNumberofAddedRuns(size_t index=0);
	bool	MR_Set_TotalNumberofRuns(size_t);

	void	MR_ResetDimCounter();
	void	MR_UpdateValues();
	bool    SetParametersRandomForMultiRun(vector<SIMB>, FCSV *pCSVFile);
	bool    SetParametersTableForMultiRun(vector<SIMB>, FCSV *pCSVFile);
	void	MR_CP_Reset(SimB* pPar);
	void	MR_CP_Reset(SimB* pPar, size_t nTabIndex);
	bool	MR_CP_Set(SimB* pPar, size_t nTabIndex, bool database=false);
	size_t	MR_GetNumberOfDimensions();
	void	MR_Reset();
	void	MR_ResetAll();
	bool	MR_IsEmpty();


	bool	MR_SetDimCounter(size_t,size_t);
	bool	MR_UpdateDimCounter();
	void	MR_DeleteDimValue(size_t index);
	void	MR_AddNewDimensiomWith11Repeatitions();
	void	MR_SetRepeatitionsOfDimension(size_t index, size_t value);
	void    MR_SetNewNumberofTotalRuns(size_t value);
	size_t	MR_GetNumOfRepeatitionsOfDim(size_t index);
    size_t     MR_ReadData();
	bool CreateSQLTables();

	void WSHED_UpdateValues();
	void WSHED_ResetCounter();
	bool WSHED_UpdateCounter();
	bool WShed_Active;
	vector<size_t> m_WShed_ID_Receiver;
	vector<size_t> m_WShed_ID;
	vector<float> m_WShed_Slope;
	bool m_WShed_Active;
	size_t m_WShed_ActiveIndex;
	size_t m_WShed_Number_X_grids;

	string GetProfileFromPlotPF(bool Add=true);
	string GetProfileFromPlotPF(bool Add, bool Message=true);
	string GetProfileFromPlotPF(string);
	void MR_ReCalculatePostDist(bool BayesMeanMethod=true);
	void MR_INIT_Bayesian_Nelder_Mead();
	void MR_CHECK_Bayesian(bool UpdateExt=false);
	void MR_CheckCalibMethod();
	void MR_RESET_Bayesian();
	bool MR_UseBayesianCoVar;
	size_t		MR_Get_NumBayesPar();
	size_t		MR_Get_NumNotTablePar();
	void	MR_Set_NumBayesPar(size_t value) {SetNum_BayesPar(size_t( value));};
	size_t		MR_Get_NumTotalPar();
	string MR_Get_CalibParName(size_t);
	string MR_Get_BayesGroup(size_t);
	size_t		MR_Get_BayesMethod(size_t);
	size_t MR_GetNoTableParIndex(size_t i_noTable) {
		if (i_noTable < m_NoTableParIndex.size())
			return m_NoTableParIndex[i_noTable];
		else
			return string::npos;
	}

	float	MR_Get_BayesCoCor(size_t, size_t);
	float	MR_Get_BayesCoVar(size_t, size_t);
	float	MR_Get_BayesMean(size_t indexp);
	void	MR_Set_BayesMean(size_t indexp, float v);
	void	MR_Set_BayesCoVar(size_t indexp,size_t indexp2, float v);
	void	MR_Set_BayesCoCor(size_t indexp,size_t indexp2, float v);
	void	MR_Init_BayesCovar(size_t ncount);
	void	MR_Init_BayesCocor(size_t ncount);
	void	MR_Storage_Init(bool UsingPostGres=false, size_t numsum=0);
	void	MR_Storage_Add();
	void	MR_Storage_Close();
	bool	MR_Storage_Open();
	bool	MR_Storage_Reset();
	bool	MR_Storage_OpenPrevious();
	bool	MR_Storage_IsOpen();
	bool	MR_Storage_InitEnsembles();
	bool	MR_Storage_InitEvaluateStat();
	size_t	MR_GetValidIndex_IndexInValFile_InDim1();
	void LockMultiStorageFile();
	void UnLockMultiStorageFile();

	void SetMC_Step(double step) {mc_step=step*m_BayesianStepSize;};

	bool	MR_UpdateValuesFromMBin(size_t runno);
	bool	m_NoUpdateRunNo;

	bool LinkedChangeToSwitches(Sw *pSw, size_t option);
	bool LinkedChangeToDB_Parameters(Par *pPar, size_t option);

	


protected:
	bool m_MultiStoragelocked;
	bool m_OutputBinFileExist, m_OutputValidBinFileExist;

	vector<PAR_OPTIONS> m_OptionsForReading;

private :
	vector<size_t> m_NoTableParIndex;
	float m_NelderMead_BestScore;
	int m_NelderMeadObjective;
	DistStat m_DistDefault;
	NelderMeadOptimizer m_NelderMead;
	Vector m_ActualSimplex;
	size_t m_ActualNumberNelderMeadSteps;
	size_t m_ActualDimension;
};
