#pragma once
#include "../../ModelTypes/SimB.h"
#include "../../NewBase/ModelBase.h"
class NewMap;

class ValidationData 
{
public:

	size_t m_NewLogFunc, m_NewBoxCox;
	int m_NumberSelectedValidationVar[MAXSIMVAL];
	bool m_ResetMonitoring, m_ResetValMonitoring;
	bool m_look;
	int m_TotalNumberSelected;
	ValidationData();
	virtual ~ValidationData();
	bool Init(NewMap *pDoc, int itype=0);
	F* ValidationFilePointer(size_t index=0);
	void SetPointersToOutputValidationFiles(bool OnlyMemoryUse=false);

	bool IsSomeValidationMade() { if (GetNumberSelectedTSV_Val() > 0 || GetNumSumVarVariables() > 0)  return true; else return false; };
	
	size_t GetNumberSelectedTSV_Val();
	void SetNumberSelectedTSV_Val();
	void ClearSumVarList();
	int GetValFileNumber(size_t);
	int GetValFileIndex(size_t, size_t MR_Dim1Set=0);
	int GetValFileResultIndex(size_t);
	int GetValLocalIndex(size_t);
	int GetValNameType(size_t);
	string GetValGroup(size_t);
	string GetValName(size_t);
	int GetNumSumVarVariables();
	int GetValSumVarOutputType(size_t);
	string GetStringValSumVarOutputType(size_t);
	int GetValSumVarLocalIndex(size_t);
	bool GetValFileLogTrans(size_t);
	int GetValSumVarNameType(size_t);
	string GetValSumVarGroup(size_t);
	string GetValSumVarName(size_t);
	int GetValn(size_t);
	float GetValNSE(size_t);
	float GetValRMSE_O(size_t);
	float GetValR2(size_t);
	float GetValA0(size_t);
	float GetValA1(size_t);
	float GetValME(size_t);
	float GetValRMSE(size_t);
	float GetValMSIM(size_t);
	float GetValMVAL(size_t);
	double GetValLogLi(size_t);
	float GetValP_Error(size_t);
	float GetValA_Error(size_t);
	int GetValDuration(size_t);
	bool  GetVal_AccTestValue(size_t);

	float GetValSumVarME(size_t, size_t irep = 0);
	float GetValSumVarRMSE(size_t, size_t irep = 0);

	float GetValSumVarMVAL(size_t);
	float GetValSumVarSim(size_t);

	double GetValSumVarLogLi(size_t, size_t irep=0);
	float GetValSumVarP_Error(size_t);
	float GetValSumVarA_Error(size_t);
	bool SetValMonitoring(size_t, int);
	int  GetValMonitoring(size_t);
	float GetValP_ErrorNew(size_t);
	float GetValA_ErrorNew(size_t);
	float GetValBoxCox_PowerNew(size_t);
	float GetValBoxCox_OffsetNew(size_t);
	void SetValP_ErrorNew(size_t, float);
	void SetValA_ErrorNew(size_t, float);
	void SetValBoxCox_PowerNew(size_t, float);
	void SetValBoxCox_OffsetNew(size_t, float);
	size_t	GetValFuncNew() {return m_NewLogFunc;};
	size_t	GetValBoxCoxNew() {return m_NewBoxCox;};
	bool SetValCoef(size_t, int, float, float, float, float,float, float,float,float, float,double, float);
	bool SetValCoef(size_t, int, float, float, float, float,float, float,float,float, float,double, bool, float);
	bool SetValCoef(size_t, int, float, float, float, float,float, float,float,float, float,double, bool);
	bool SetValCoef(size_t, int, float, float, float, float,float, float,float,float, float,double);
	bool SetValCoef(size_t, int, float, float, float, float,float, float,float);
	bool SetValSumVarCoef(size_t, float, float, float, float,float, float,double);
	bool SetValSumVarCoef(size_t, float, float,double);
	bool SetValErrors(size_t, float, float);
	bool SetValErrors(size_t, float, float, bool, bool LogT=false);
	bool SetValNewLogLi(size_t, float, float, float, float);
	void SetValNewLogLiOptions(size_t func, int box){m_NewLogFunc=func; m_NewBoxCox=box;};
	bool SetValSumVarErrors(size_t,int,float,float, float);

	//bool SetValCoef(size_t, int, float, float, float, float,float, float,float);
	bool AddValList(size_t, int, int,string, string, int);
	bool AddValList(size_t, int, int,string, string, int, float, float);
	bool AddValList(size_t, int, int,string, string, int, float, float, bool);
	bool AddNewValList(size_t, int, int, int,string, string, int, float, float, bool, bool, int Dur=60);
	bool UpdateValOutIndex();
	bool AddValSumVarList(size_t, int, int, string , string ,int ,float, float, float );
	bool AddValSumVarList(size_t, SIMSUMSOURCE simsource, size_t res_index, int sumvartype, SIMB selected, float, float, float);
	bool AddValSumVarList(size_t, SIMSUMSOURCE simsource, size_t res_index, int sumvartype, SIMB selected, string, string csvfile, float, float);
	bool SetValFileLogTrans(size_t, bool);	
	bool SetValFileNumber(size_t, int);
	bool SetValDuration(size_t, int);
	bool SetValFileIndex(size_t, int);
	bool RemoveValNode(size_t);
	bool RemoveValFile(size_t);

	bool RemoveValSumVarNode(size_t);
	bool CleanValList();

	void SetSumValSet(bool value) {m_IsSumValSet=value;};
	
	bool IsSumValSet() {return m_IsSumValSet;};

	int  GetSumVarIndex(int type, SimB *pBase, int TabIndex, int OutPutType);
	SIMB GetSimPointer_TSV(size_t index);

	SIMB GetSimPointer_SV(size_t index);
	private:
	NewMap *m_pSimDoc;
	bool m_IsSumValSet;
	vector<CPG*>m_PGCreatedPGFiles;



};
