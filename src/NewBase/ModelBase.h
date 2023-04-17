#pragma once
class SimB;
class Sw;
class F;
class CDB;
class PFCurve;

#include "./MultiRun/Residuals.h"
#include "./MultiRun/SelectedEnsembles.h"
#include "../ModelTypes/NewModelType.h"
#include "./ModelMap.h"
#include "../PG/PG.H"
#include "./DB/DB_Storage.h"
enum class SIMSUMSOURCE { MULTIFILESTAT, CSVFLEXFILE, MULTI_OUTPGFILE };
enum SUM_PERFORMANCE { SUM_R2, SUM_INTERCEPT, SUM_SLOPE, SUM_ME, SUM_RMSE, SUM_LOGLI, SUM_OBS, SUM_SIM, SUM_NSE, SUM_NOVALUE };

struct CONSTRAIN_BY {
	size_t IndepMultiPar;
	double MinRangeValue, MaxRangeValue;
};
class MRv{
public:
	size_t NumberOfRepeatitionsWithinDimension;
	size_t nCount;
};

class VALv{
public:
	size_t	ValidationFileIndex;
	size_t	OutputType;
	string	Group;
	string	Name;
	size_t	LocalIndex;
	size_t	nNumber;
	float   R2;
	float	A0;
	float   A1;
	float   ME;
	float   RMSE;
	float   MeanSim;
	float   MeanVal;
	float	P_Error;
	float	A_Error;
	double	LogLi;
	bool	AccTest;
	size_t	ValidationFileNumber;
	bool	LogTrans;
	size_t	ValidationFileResultIndex;
	size_t	Duration;
	size_t	Monitoring;
	float   NSE;
	float	P_NewError;
	float	A_NewError;
	float	NewBoxCox_Power;
	float	NewBoxCox_Offset;
};

class VALSUMv
{
public:
	size_t	SumVarType=0; // 0 = Original, 1- Obs Scaled, 2- Sim Scaled
	enum  SIMSUMSOURCE  SimValueSource=SIMSUMSOURCE::MULTI_OUTPGFILE; // 0 = Sim Sum Values, 1 - Residuals from other Sum Variables, 2 - Multi Bin file
	bool m_xmlFileToUse{ false };
	size_t  Sim_Value_Residual_Index=0;
	size_t	OutputType=0;
	unsigned int  MultiBin_StartMin=0, MultiBin_EndMin=0;
	string	Group;
	string	Name;
	size_t	LocalIndex=0;
	vector<float>   ME;
	vector<float>   RMSE;
	vector<float> R2_Multi;
	vector<float> InterCept_Multi;
	vector<float> Slope_Multi;
	vector<float>	LogLi;
	vector<float> NSE;
	float ME_mean=0.f;
	float RMSE_mean=0.f;
	float R2_Multi_mean=1.f;
	float InterCept_Multi_mean=0.f;
	float Slope_Multi_mean=1.f;
	float NSE_mean=0.f;
	double LogLi_mean=0.;
	int  n_RegMulti=0;
	float   SimValue=MISSING;
	float   ObsValue=MISSING;
	float	P_Error =0.1f;
	float	A_Error = 0.001f;
	bool	Monitoring=false;
	bool	FlexSumVar=false;
	string  IDVarName;
	string  IDFileName;
	double  ObsScalingFactor= 1.;
	size_t	ObsScaling=0;
	vector<float> ObsValueVector;
	vector<float> SimValueVector;
 };
struct VALSUMLINK {
	string FileName;
	vector<string> VarName;
	string IDVarKey;
	size_t MultiSimNumKey;
};
struct MULTI_PARFILES {
	string FileName;
	vector<string> ParNames;
	string FileId;
};
struct DocFile2 {
	string m_OriginFileName;
	string m_ExeFileDate;
	string m_DateRun;
	string m_Comments;
	string m_RunId;
	string m_Multi_MBin_File;
};

struct InformationCriteria {
	double SSE;
	size_t n_points;
	size_t n_parameters;
	double BIC;
	double AIC;
	size_t n_aggregateNumber;
};



struct DocFile {
	bool m_Running;
	bool m_MultiRunning;
	bool m_FinishedSimulation;
	bool m_MultiStoragelocked;
	size_t m_FileVersionNumber;
	size_t m_SimulationRunNo;
	size_t m_TimePeriodScaleFactor;
	size_t ipredays;
	size_t ipostdays;
	time_t m_TimeCreated;
	time_t m_TimeModified;
	float m_SimTime;
	float m_AccSimTime;
	float m_TsVLogLi;
	float m_TsVLogLiSum;


};

 
class RUNINFO {
public:
	string Group;
	string Name;
	size_t Format;  // 0=Int, 1=Date, 2=String
	CRunInfo* pBase;
};
class Annim {
	public:

	SimB* pBase;
	size_t type;
	size_t LocalIndex;
	size_t iChart;
};
struct HistP
	{
	   SimB *pBase;
	   size_t type;
	   size_t LocalIndex;
	};
class HISTv
{
public :
	SimB* pBase;
	size_t RunInfoType;	// SimB: 0= Run Info, 1= DB object;
	size_t	LocalIndex;
	time_t	tvalue;
	float	value;
	string strvalue;
	string User;
	string Computer;
	size_t	RunNo;
};
class ModelBase : public ModelMap{
	
	public:
	ModelBase();	
	~ModelBase();

	DocFile m_DocFile;
	DocFile2 m_DocFile2;
	bool m_xmlFileToUse{ false };
	InformationCriteria m_InformCriteria;
	size_t m_FileVersionNumberRead;
	vector<MRv> m_MultiRun_Array;
	vector<VALv> m_Val_Array;
	vector<size_t> m_ValGroupIndex[NUMGROUP];
	vector<size_t> m_ValTypeIndex[4];

	vector<VALSUMv> m_ValSum_Array;
	VALSUMv GetValSumStruct(size_t index) {
		if (m_ValSum_Array.size() > index) return m_ValSum_Array[index]; else
		{
			VALSUMv out; return out;
		};
	};
	vector<VALSUMLINK> m_ValsumFlexFiles;
	vector<MULTI_PARFILES> m_MultiParFiles; // CSV files to goven change of MultiRuns during run

	vector<SIMB>m_X_Array;
	vector<SIMB>m_T_Array;
	vector<SIMB>m_G_Array;
	vector<SIMB>m_D_Array;
	vector<CDB*> m_DB_Array;
	vector<string> DB_Names;
	vector<time_t> m_DB_ImportTimeT;

	vector<bool> DB_Modified;
	vector<RUNINFO> m_RunInfo;	
	vector<HISTv> m_History_Array;
	vector<HistP> m_HistoryPar_Array;
	vector<string>m_Report_Array;
	vector<Annim>  AnnimVarList;
	vector< vector<float>> m_ValFlexValue[4];
	size_t m_AnnimChartX, m_AnnimChartY;
	

	SimB* GetPtrFromIntType(size_t itype, string group, string name);
	SimB* GetPtr(string type, string group, string name);
	SimB* GetPtr(string type, string name);
	SimB* GetPtr(simtype, string name, string group="");
	SimB* GetPtr(simtype,simtype, string name);
	SimB* GetPtrByName(string name, string group="");
	P* GetP(string name)  {return (P*)GetPtr(PAR_TABLE, name);};
	Ps* GetPs(string name) {return (Ps*)GetPtr(PAR_SINGLE, name);};
	Sw* GetSw(string name) {return (Sw*)GetPtr(SWITCH,name);};
	F* GetF(string name) { return (F*)GetPtr(PGFILE, name); };
	OutSingle* GetSingleOutputPtr(string name) { return static_cast<OutSingle*>(GetPtr(STATE_SINGLE,DRIVE_SINGLE, name)); };
	OutVector* GetVectorOutputPtr(string name) { return static_cast<OutVector*>(GetPtr(STATE, DRIVE, name)); };
	size_t GetSwValue(string name) {return ((Sw*)GetPtr(SWITCH,name))->GetIntValue();};
	size_t SetSwValue(string name, size_t value) {return ((Sw*)GetPtr(SWITCH,name))->SetIntValue(value);};


	vector<SimB*> GetPtrVector(string type, string group, bool All=false);
	vector<SimB*> GetPtrVector(simtype,  bool All=false, bool OnlyNotOriginal=true);
	vector<SimB*> GetPtrVector(simtype, size_t GroupNo,bool All);
	vector<SimB*> GetAllPtr(simtype);
	vector<SimB*> GetAllEnabledPtr(simtype);
	vector<SimB*> GetAllPtr(simtype, simtype, simtype, simtype);

	vector<SimB*> GetPtrVector(string type, size_t igroup=-1);
	vector<SimB*> GetPtrVector(size_t itype=100, size_t igroup=-1); 
	vector<size_t> GetEnabledGroupNumbers(bool all=false);

	vector<SIMB> GetAllSelectedOutputs(bool MultiFlag=false);
	vector<SIMB> GetAllSelectedOutputsSortedByName(bool MultiFlag = false);
	vector<string> GetAllSelectedNameList(bool MultiFlag = false);
	void RemoveAllSelectedOutputs();
	size_t SelectAllOutputs();

	vector<SIMB> GetWShedOutputs();	
	vector<size_t> GetPressureHeadOutputIndex(vector<SIMB> allout);
	vector<SIMB> GetAllOutputs(size_t choice=string::npos, bool allout=false);
	vector<SimB*> GetAllPtrOutputs(size_t choice=string::npos);
	vector<SIMB> GetALLXOutputs() {return GetAllOutputs(0);};
	vector<SIMB> GetALLTOutputs() {return GetAllOutputs(1);};
	vector<SIMB> GetALLGutputs() {return GetAllOutputs(2);};
	vector<SIMB> GetALLDOutputs() {return GetAllOutputs(3);};
	void SetEnabledCategories(vector<int> categories) { m_EnabledCategories = categories; };
	vector<int> GetEnabledCategories() { return m_EnabledCategories; };
	vector <bool>m_GroupEnabled;
	vector<int> m_EnabledCategories;
	void SetUpdatedGroupNumbers(vector<size_t> groups) {
		m_UpdatedGroupNoVector = groups;
		UpdateEnabledElementvector();
	};
	vector<size_t> GetUpdatedGroupNumbers() { return m_UpdatedGroupNoVector; };
	void UpdateEnabledElementvector();
	void UpdateEnabledProcvectors();

	void SetUpdatedElements(vector<elements> currentElements) {
		m_UpdatedElementsVector = currentElements;
		UpdateEnabledProcvectors();
	};
	void SetUpdatedFysProcVector(vector<fysprocess> currentfysproc) {
		m_UpdatedFysProcVector = currentfysproc;
	};
	void SetUpdatedBioProcVector(vector<bioprocess> currentbioproc) {
		m_UpdatedBioProcVector = currentbioproc;
	};



	vector<elements> GetUpdatedElementsVector() { return m_UpdatedElementsVector;};
	vector<elements> GetEnabledElementsVector() { return m_EnabledElementsVector;};
	vector<fysprocess> GetUpdatedPhysProcVector() { return m_UpdatedFysProcVector; };
	vector<fysprocess> GetEnabledPhysProcVector() { return m_EnabledFysProcVector; };
	vector<bioprocess> GetUpdatedBioProcVector() { return m_UpdatedBioProcVector; };
	vector<bioprocess> GetEnabledBioProcVector() { return m_EnabledBioProcVector; };


	bool IsFinishedSimulation();
	void SetFinishedSimulation();
	void SetNewSimulation();
	bool IsMultiSimulation();
	size_t		MR_GetNumberOfRepeationsForThisDimension(size_t index);
	string GetXBinFileName(size_t ValidIndex=0, size_t addfile=0);


	void SetMultiStorageBinFileMissing();
	bool IsMultiStorageBinFileValid();
	bool IsBayesianCalibration() {return m_IsMakingBayesianCalib;};

	void SetMultiSimulation();
	void SetRunning(bool value) {m_DocFile.m_Running=value;};
	bool IsRunning() {return m_DocFile.m_Running;};
	void SetSingleSimulation();
	void SetSimTime(double usedtime) {
		if(m_DocFile.m_SimTime<0) {
				m_DocFile.m_SimTime=float(usedtime);
				m_DocFile.m_AccSimTime=float(usedtime);
		}
		else {
			float prev=m_DocFile.m_AccSimTime;
			m_DocFile.m_AccSimTime=float(usedtime);
			m_DocFile.m_SimTime=m_DocFile.m_AccSimTime-prev;
		};};
	bool SetSimPeriodFromClimateFile();
	bool SetOutputFileNameToOutputFile(string filename);
	string GetWShedFileName_In();
	string GetWShedFileName_Out();
	string GetWShedFileName(size_t inum);

	string GetMBinFileName();
	string GetCurrentSimFile();
	string GetCurrentWorkDirectory();
	F* OutputFilePointer();
	string m_AppString;
	size_t	MR_Get_TotalNumberofRuns();
	size_t	MR_Get_NumberofRuns();
	size_t	MR_Get_ActualRunNumber();
	size_t		MR_GetDimCounter(size_t);
	size_t		MR_GetNumberOfDimensions();
	string GetMultiSubDir();
	
	size_t GetNumOutPuts();
	void Clear_XTGD();
	size_t GetSize_XTGD();
	size_t GetSize_XTGD_Total();
	vector<SIMB> GetOutVector(size_t itype);
	vector<SIMB> GetAllOutPutVectorByCategory(int category);
	void CleanValFlags();

	size_t m_NumberSelectedValidationVar[16];

	void DB_SetChoice(DB_types, string value);
	string DB_GetChoice(DB_types);
	string DB_GetNames(size_t, bool AllIndex=true);
	CDB* DB_GetPointer(DB_types);
	CDB* DB_GetPointer(string);
	void DB_SetImportTimeT(size_t index, time_t time);
	time_t DB_GetImportTimeT(size_t index);
	void SetRunReportArray(string);

	float GetActualObsValueFromValSumFlex(string IDFileName, string  IDVarName, size_t runno);
	void CheckFileNameLinkedto_SV_Variables();
	string GetExistingDocFileName(string FileName);


	bool m_Protect;
	bool m_ChildDocument;
	bool m_SavedSum;
	bool m_SavedTimeSerie;
	size_t m_LinkedDocNumber;
	bool m_HasBeenRunning;

	bool m_OutputValidXBinFileExist;
	bool m_OutputBinFileExist, m_OutputValidBinFileExist;
	
	bool m_IsMakingBayesianCalib; bool m_IsMakingNelderMeadCalib;
	bool m_IsUsingDB_Source, m_IsUsingDB_Target;
	
	size_t m_CountError;
	size_t m_MultiRun_TotalNumber;
	size_t m_NumOutPuts;
	size_t m_UserLevel;
	string m_DocFileName;
	string m_SiteName;
	string m_UserDirectory;
	string m_CurrentFile, m_CurrentDirectory, m_StartDirectory, m_CurrentSubDirectory;
	string m_DataBaseDirectory, m_PreviousMultiMBinFile;
	string m_Multi_XBin_File, m_Multi_Bin_FileName;

	Sw* m_pValidIndexSet;

	SelectedEnsembles m_MStorage;

	bool m_ReReadMStorageFile;

	CPG m_PG_OutPutFile, m_PG_MultiOutputFile, m_PG_WShed_In, m_PG_WShed_Out;

	size_t GetNumAll_TSV_ValVariables();
	size_t m_ErrorCount;

	vector<string> m_arrGroups;
	vector<string> m_PlantTypes;
	string m_DB_choices[10];
	DB_OldHeader m_DB_header[10];
	unique_ptr<PFCurve> p_PFCurve{nullptr};
	unique_ptr<Register> m_pRegister;
	string GetTimeCreated();
	string GetTimeModified();
protected:
	bool m_MultiStorageBinFileExist;
	
	string m_CurrentSubDirectoryFile;
	vector<SIMB> SelectedOutVector(vector<SimB*> vp);
	vector<SimB*> ReturnIfSelectionMade(vector<SimB*> vp);
	vector<SIMB> CompleteOutVector(vector<SimB*> vp);

	vector<string> m_FailedNames;

	string CreateFlagString(SimB* currentParameter, size_t FlagValue=1);

private:
	vector<size_t> m_UpdatedGroupNoVector;
	vector<elements> m_UpdatedElementsVector;
	vector<elements> m_EnabledElementsVector;
	vector<fysprocess> m_UpdatedFysProcVector;
	vector<fysprocess> m_EnabledFysProcVector;
	vector<bioprocess> m_EnabledBioProcVector;
	vector<bioprocess> m_UpdatedBioProcVector;
};
