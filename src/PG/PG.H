

#pragma once
#ifndef MISSING
#define MISSING -1.1E38f
#endif
#ifndef COUPSTD
//#include "../include/win32/chartdir.h"
#endif
#include "./PGView.h"





enum COMPARETYPE { SMALLER, SMALLER_EQUAL, EQUAL, GREATER_EQUAL, GREATER };
enum TIMECALCFUNCTION {NO_USE_FUNC,DAYNUMBERFUNC, MINUTENUMBERFUNC};

struct IF_CONDITIONS {
	COMPARETYPE comp_method;
	TIMECALCFUNCTION Function;
	bool LeftTimeFuncPos;
	float LeftValue;
	float RightValue;
	size_t LeftVariable;
	size_t RightVariable;
};
class DoubleArray;
class Doc;

class CPG : public CPGView 
{



public:
	CPG();	
	CPG(string filename);						// Default constructor

	virtual ~CPG();

	bool RemoveFile();

	void SetXTGDIndex(size_t,size_t,size_t,size_t,size_t,size_t,size_t,size_t);


	bool SetDataInFocus(size_t datatype);
	void SetViewPointer(void *pObject);
	void AddDummySequenceTimeVector();
	void AddDummyTimeVector(DoubleArray Time);
	void AddTimeVector(DoubleArray Time);
	void AddVariableVector(DoubleArray Variable, size_t i);
	void AddVariableVector(vector<float> *VarVector, size_t i);
	bool AddNewRepititionDescriptions(size_t add);

	size_t GetNumberofMissing(size_t varno=-1, size_t repit=1);
	size_t	 GetNumberofVariablesWithMissings();

	bool SetCommonPeriod(size_t,size_t);
	bool SetCommonPeriod(string,string);


	size_t GetVarIndex(string str, size_t irep=1);
	size_t GetVarIndex(string str, string str_id, size_t irep=1);

	void ResetSelections();
	void RemoveSelection(size_t);
	bool AddAllSelections();
	void AddSelection(size_t varindex, float Value=1.);
	void SetSelection(size_t seqindex, size_t varindex);
	void SetSelectionWeight(size_t seqindex,float v);
	float GetSelectionWeight(size_t index);
	size_t GetNumOfSelected();
	size_t GetNumToBeCalc();
	size_t GetSelection(size_t);
	void SetReplace(bool);
	size_t GetCalcVar();
	void SetCalcVar(size_t);
	size_t GetCalcVarNew();
	void SetCalcVarNew(size_t);
	void SetVarUnitType(size_t VarNo, size_t Repit, UNIT_TYPES unit);
	void SetVarElementType(size_t VarNo, size_t Repit, elements elem);

	bool FileExist();
	void Reset_Start();

	size_t	GetVarKey(size_t, size_t repit=1);
	double	GetVarLat(size_t, size_t repit=1);
	double	GetVarLong(size_t, size_t repit=1);
	double	GetVarAlt(size_t, size_t repit=1);
	string	GetVarCountry(size_t, size_t repit=1);
	string	GetVarStation(size_t, size_t repit=1);

	elements GetVarElement(size_t, size_t repit = 1);
	UNIT_TYPES GetVarUnitType(size_t, size_t repit = 1);

	size_t	GetVarMissing(size_t, size_t repit=1);
	size_t	GetVarSubstituted(size_t, size_t repit=1);
	void	SetAllToBeSaved(bool);
	void	SetToBeSaved(size_t, bool);
	void	SetToBeSavedBelow(size_t, bool);
	bool	GetToBeSaved(size_t);
	void	SetAllToBeCalc(bool);
	void	SetToBeCalc(size_t, bool);
	void	SetToBeSavedCalc(size_t, bool);
	bool	GetToBeCalc(size_t);
	void    ResetIfVector() { m_IfVector.clear();};
	size_t  SetIfConditions(IF_CONDITIONS cond) {
		m_IfVector.push_back(cond); return  m_IfVector.size();
	};
	bool  SetIfConditions(size_t i, IF_CONDITIONS cond) {
		if (i < m_IfVector.size()) {
			m_IfVector[i] = cond; return  true;;
		}
		else
			return false;
	};
	vector<IF_CONDITIONS> GetIfConditions() {return m_IfVector;	};
	size_t GetNumOfIfConditions() { return m_IfVector.size(); };
	void   SetApplyConditions(bool value) { m_ApplyConditionCheck = value; };
	bool   IsApplyConditions() { return m_ApplyConditionCheck; };
	void	SetOnlyNoneMissingToBeSaved(bool);
	bool	GetOnlyNoneMissingToBeSaved();


	void*	GetVarValuePointer(size_t varno);
	float*  GetMeanPointer(size_t MeanOption);

	vector<float> Get_AllRep_VarValue(size_t varno, size_t var_rec);
	void	SetMeanReset(size_t MeanOption);

	
	float	GetVarValueMinuteMean(size_t varno, size_t recno, size_t minutes);
	float	GetVarValueHourMean(size_t varno, size_t recno, size_t hours);
	float	GetVarValueDaysMean(size_t varno, size_t recno, size_t days);
	float	GetVarValueMonthsMean(size_t varno, size_t recno, size_t months);
	float	GetVarValueYearsMean(size_t varno, size_t recno, size_t years);
	
	string	GetVarDateMinuteMean(size_t recno, size_t minutes);
	string	GetVarDateHourMean(size_t recno, size_t hours);
	string	GetVarDateDaysMean(size_t recno, size_t days);
	string	GetVarDateMonthsMean(size_t recno, size_t months);
	string	GetVarDateYearsMean(size_t recno, size_t years);

	float	GetVarValue_WithinYearMean(size_t varno, size_t recno, bool res=false);
	float	GetVarValue_WithinYearSD(size_t varno, size_t recno, bool res=false);
	float	GetVarValue_WithinYearMin(size_t varno, size_t recno, bool res=false);
	float	GetVarValue_WithinYearMax(size_t varno, size_t recno, bool res=false);
	float	GetVarValue_WithinYearP5(size_t varno, size_t recno, bool res=false);
	float	GetVarValue_WithinYearP50(size_t varno, size_t recno, bool res=false);
	float	GetVarValue_WithinYearP95(size_t varno, size_t recno, bool res=false);
	
	float	GetVarValue_WithinDayMean(size_t varno, size_t recno, size_t im);
	float	GetVarValue_WithinDaySD(size_t varno, size_t recno, size_t im);
	float	GetVarValue_WithinDayMin(size_t varno, size_t recno, size_t im);
	float	GetVarValue_WithinDayMax(size_t varno, size_t recno, size_t im);
	float	GetVarValue_WithinDayP5(size_t varno, size_t recno, size_t im);
	float	GetVarValue_WithinDayP50(size_t varno, size_t recno, size_t im);
	float	GetVarValue_WithinDayP95(size_t varno, size_t recno, size_t im);

	float	GetVarValue_MultiMean(size_t varno, size_t recno, bool all=false);
	float	GetVarValue_MultiSD(size_t varno, size_t recno, bool all=false);
	float	GetVarValue_MultiMin(size_t varno, size_t recno, bool all=false);
	float	GetVarValue_MultiMax(size_t varno, size_t recno, bool all=false);
	float	GetVarValue_MultiP5(size_t varno, size_t recno, bool all=false);
	float	GetVarValue_MultiP50(size_t varno, size_t recno, bool all=false);
	float	GetVarValue_MultiP95(size_t varno, size_t recno, bool all=false);


	float	GetVarValue_MinPos(size_t varno, size_t minutv);
	size_t		GetVarFunction();
	size_t		GetVarFunctionType();
	void	SetVarFunction(size_t expression);
	void	SetVarFunctionType(size_t expression_type);
	void	SetCalcStart(size_t);
	void	SetCalcStop(size_t);
	size_t	GetCalcStart();
	size_t	GetCalcStop();
	void	SetDateSelection(bool);
	bool	GetDateSelection();
	void	SetTimeConvUnit(size_t);
	size_t		GetTimeConvUnit();
	void	SetTimeConvBase(size_t);
	size_t	GetTimeConvBase();
	
	bool	CalcVarScaling(size_t var_in, size_t var_out=-1, size_t repn=1);
	bool	CalcVarDifference(size_t var_in, size_t var_out=-1, size_t repn=1);
	bool	CalcVarSmoothing(size_t var_in, size_t var_out=-1, size_t repn=1);
	size_t	CalcVarMissing(size_t var_in, size_t var_out=-1, size_t repn=1);
	size_t 	CalcVarMissingSubstitute(size_t var_in, size_t var_out=-1, bool VariableValue=false, size_t repn=1, size_t min_gap=1440000);
	size_t	CalcVarMissingByRandomPeriod(size_t var_in, size_t repn=1);

	bool	CalcTimeChange(size_t repn=1);
	bool	CalcVarTimeDerivate(size_t repn=1);
	
	
	bool	CalcVarMean(size_t varno, size_t repn=1);
	bool	CalcVarSum(size_t varno, size_t repn=1);
	bool	CalcVarMultiply(size_t varno, size_t repn=1);
	bool	CalcVarDivide(size_t varno, size_t repn=1);
	bool	CalcTimeToVar(size_t varno, size_t repn=1);
	bool	AddNewVariable(size_t varno);
	bool	RemoveVariable(size_t varno);


	size_t	GetRecord(size_t minutv, bool exact=true);
	size_t	GetNumRecordsMinute(size_t minutes, size_t repn=1);
	size_t	GetNumRecordsHour(size_t days, size_t repn=1);
	size_t	GetNumRecordsDays(size_t days, size_t repn=1);
	size_t	GetNumRecordsMonth(size_t months, size_t repn=1);
	size_t	GetNumRecordsYear(size_t years, size_t repn=1);
	size_t	GetNumMonths();

	size_t     GetYear(size_t);
	size_t     GetMonth(size_t);
	size_t     GetDay(size_t);
	size_t     GetHour(size_t);
	size_t     GetMinute(size_t);
	size_t     GetDayNumber(size_t);
	size_t		GetMinuteNumber(size_t);

	string	GetStartDate();
	string	GetEndDate();


	double  GetFloatTime(size_t);
	double  GetDoubleTime(size_t);

	double	GetDoubleTimeStart();
	double	GetDoubleTimeYearStart(size_t var_rec);
	double	GetDoubleTimeYearEnd(size_t var_rec);
	double	GetDoubleTimeEnd();
#ifndef COUPSTD
	class CFile;
	bool    Write(CFile);
#endif
	bool	WriteSelectionToExcel();
	string  WriteExcelFile();
	bool	GetOutFileStatus();
	string GetOutFileStart();
	string GetOutFileEnd();
	size_t	GetOutFileRecords();
	size_t	GetOutFileVar();
	size_t		GetColor(size_t);
	bool	GetPicked(size_t);
	bool	GetPickedMean(size_t);
	bool	GetPickedMin(size_t);
	bool	GetPickedMax(size_t);


	void	SetColor(size_t, size_t);
	void	SetPicked(size_t, bool);
	void	SetPickedMin(size_t, bool);
	void	SetPickedMax(size_t, bool);
	void	SetPickedMean(size_t, bool);

	bool	CreateYearIndex();
	size_t	GetNumberofYears();
	size_t	GetYearIndex(size_t index);
	size_t	GetRecordsInYear(size_t index);
	bool	CreateDayIndex();
	size_t	GetNumberofDays();
	size_t	GetDayIndex(size_t index);
	size_t	GetRecordsInDay(size_t index);
	size_t	GetRecordsInSection(size_t index);
	size_t	GetIndexInSection(size_t index);
	size_t	GetMinuteStartThisYear(size_t var_rec);
	size_t	GetMinuteStartNextYear(size_t var_rec);
	size_t	GetMinuteStartNextDay(size_t var_rec);
	void	SetYearSectionStyle(bool value);
	void	SetSectionNumber(size_t value);
	size_t	GetNumberofSections();
	size_t	GetNumberOfCommonPoints();
	double	GetDoubleShiftTime(size_t index);

	bool	EstimateWithinYearStatistics(Doc *pDoc=nullptr);
	void	EstimateWithinYearClear(size_t rep);
	bool	EstimateWithinDayStatistics();
	bool	EstimateMultiStatistics(bool all=false, vector<size_t> *acceptedruns=nullptr);
	bool	GetReplaceMode();


	bool	m_UpdatedMultiAccepted,m_UpdatedMultiAll;

protected:

	vector<size_t> m_Selected, m_Color;
	vector<bool> m_Picked, m_PickedMin, m_PickedMax, m_PickedMean;
	vector<float> m_SelectedCoef;

	size_t m_expression, m_expression_type,m_time_unit;
	size_t m_time_base;
	bool m_Replace, m_DateSelection;
	size_t m_CalcVar, m_CalcVarNew;
	size_t m_CalcStart,m_CalcStop;

	size_t  m_PlotStartIndex, m_PlotNumRecords;
	size_t m_Xind_Start, m_Xind_End;
	size_t m_Tind_Start, m_Tind_End;
	size_t m_Gind_Start, m_Gind_End;
	size_t m_Dind_Start, m_Dind_End;
	void *m_pView;
	size_t m_NumWithinYear, m_NumWithinDay,m_numWithinDayTot;
	size_t m_withinYearAllocated;
	bool m_WithinYearForResiduals;
private:
	vector<IF_CONDITIONS> m_IfVector;
	bool CheckConditions(size_t var_rec);
	bool m_ApplyConditionCheck;

};
