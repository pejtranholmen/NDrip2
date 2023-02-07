#pragma once
#include "Sim.h"
#include "../ModelTypes/SimB.h"
struct SelectPG
{
	F *pF;
	CPG *pPG;
	int PGIndex;
	int Repit;
	int subchart;
	int row;
};
struct SelectMultiPG
{
	F *pF;
	CPG *pPG;
	int PGIndex;
	int Repit;
	int subchart;
	int row;
	enum vartype FirstType;
	enum vartype SecondType;
	enum vartype DependentType;
};
struct SelectOUT
{
	SimB *pBase;
	int IType;
	int LIndex;
	int row;
	int subchart;
	enum  ENSEMBLE_TYPE MeanIndicator;
};
struct SelectVAL
{
	int col;
	int row;
	int RunNo;
	int RunNoRow;
	int subchart;
	enum  ENSEMBLE_TYPE MeanIndicator;
	//int MeanIndicator;
};
struct MultiVAL
{
	int col;
	int row;
	int Index;
	int RunNo;
	int subchart;
	DATA_TYPES type; // will replace Itype
	SimB *pBase;
	int LIndex;
	int PGSection;
	size_t index_cycle;
	SUM_PERFORMANCE Performance_Var;
};
struct PGDES
{
	string Name;
	string Unit;
	string Index;
};

class CCoupModelView;
class Doc:public Sim
{
public:
	Doc();
	~Doc();


	vector <Sw*>m_DefinedSwitches;
	vector <SelectPG>m_SelectModelFile;
	vector <SelectPG>m_SelectPGFile;
	vector <SelectMultiPG>m_SelectMultiPGFile;
	vector <SelectOUT>m_SelectOut;
	vector <SelectVAL>m_SelectValTime;
	vector <MultiVAL>m_SelectMultiVal;
	vector <MultiVAL>m_SelectMultiSum/*, m_SelectMultiSum_X, m_SelectMultiSum_Y*/;
	vector <MultiVAL>m_SelectMultiPar;

	vector <Doc*>m_SelectedDocuments;
	vector <Doc*>m_Selected_All_Documents;
	size_t m_DocumentType;
	CHART_TYPES m_ActualDocChartType;
	bool m_NewChart;
	int m_PrevSheet;
	int m_SumIndicator;
	int m_NumValidValFiles;
	int GetNumValidValFiles();
	size_t GetDocumentType() {return m_DocumentType;};

	string GetMultiBinFileName();
	bool IsOutputBinFileValid();
	void SetOutputBinFileMissing();
	void SetActualDocChartType(CHART_TYPES charttype) { m_ActualDocChartType = charttype; };
	CHART_TYPES GetActualDocChartType() { return m_ActualDocChartType; };
	bool IsOutputValidBinFileValid();
	void SetOutputValidBinFileMissing();
	void SetOutputBinFileExist();
	void SetDefaultAnnimationSelection();
	void ResetAnnimationSelection();
	int GetNumberAnnimationCharts();

	Sw*  GetSwPointer(Sw*, Doc* pDoc);
	Ps*  GetParPointer(Ps*, Doc* pDoc);
	P*   GetParTabPointer(P*, Doc* pDoc);

	vector <size_t>GetValIndexByGroup(size_t group);
	vector <size_t>GetValIndexByType(size_t type);
	bool m_IsValIndexUpdated;
	void UpdateValIndexList();
	PGDES GetSumOutPutDescription(int Ilocal);

	

	int Get_TotalNumResidualTimeRecords();
	CResiduals* GetResidualPointer(size_t iFile=1, int addedfiles=0);
	bool OpenOutputFile(int value=0);
	bool OpenSimDocument(long RunNo, bool NoMulti=false);
	void LockMultiStorageFile();
	void UnLockMultiStorageFile();
	bool IsMultiStorageFileLocked();

	void OutputBinFileCheck();

	time_t m_DB_ImportTime[10];
//#ifndef COUPSTD

	string DB_GetImportDate(int index);
//#endif
	void SetTimeCreated(time_t);
	void SetTimeModified(time_t);
	void TimeCreated();
	void DB_SetImportTime(int index, time_t time);
	void TimeModified();
	void DB_ImportDate(int inded);
	void SetDocLinkedNumber(int value) {m_LinkedDocNumber=value;}
	int GetDocLinkedNumber() {return m_LinkedDocNumber;};
	bool CheckSelectedDocumentsValidity();
	bool GetGroupEnable(size_t index);
	void SetGroupEnable(size_t index, bool value);
	void SetModelView(CCoupModelView *pView) {m_pModelView=pView;};
	CCoupModelView *GetModelView() {return m_pModelView;};
	int GetCurrentSheet() {return m_CurrentSheet;};
	void SetCurrentSheet(int sheet) {m_CurrentSheet=sheet;};

	void	SetMonitor1View(int);
	void    SetMonitor2View(int);
	int	    GetMonitor1View();
	int     GetMonitor2View();
	int     m_Monitor1View,m_Monitor2View;

protected:
	int m_LinkedDocNumber;
	CCoupModelView *m_pModelView;
private:
	int m_CurrentSheet;


};
