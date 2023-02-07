#pragma once
#include "./PGFile.h"

struct DataView
{
	size_t NumVar;
	size_t Repit;
};

struct RecordViewSplit
{
	size_t Period;
	vector<size_t> SplitStops;
	size_t Length;
};


class CPGView :
	public CPGFile
{
public:
	CPGView(void);
	~CPGView(void);
	size_t GetNumToView() {return m_DataView.size();};
	size_t GetViewRepetition(size_t index) {if(index<m_DataView.size()) return m_DataView[index].Repit; else return 0;};
	size_t GetViewVariable(size_t index) {if(index<m_DataView.size()) return m_DataView[index].NumVar; else return 0;};
	bool GetViewOrder() {return m_RepetitionsFirst;};
	void SetViewOrder(bool value) {m_RepetitionsFirst=value; UpdateViewOrder();};
	bool GetToBeViewed(size_t i);
	void UpdateViewOrder();
	void SetToBeViewed(size_t i, bool value);
	size_t GetRecordViewStart();
	size_t GetRecordViewSize();
	void SetRecordViewSize(size_t RecordsToView) { m_RecordSplit.Length = RecordsToView; };
	size_t GetMaxRecordViewSize() { return m_RecordSplit.Length; };
	void UpdateRecordsToView();
	void ResetRecordsToView() { pg.status.RecordsToView = false; };
	size_t GetNumberofSplitPeriodsToView() { return m_RecordSplit.SplitStops.size(); };
	void incrementCurrentPeriodToView();
	void SetCurrentPeriodToView(size_t PeriodToView);
	size_t GetCurrentPeriodToView() { return m_RecordSplit.Period; };
	
protected:
	vector<DataView> m_DataView;
	bool m_RepetitionsFirst;
	RecordViewSplit m_RecordSplit;


};

