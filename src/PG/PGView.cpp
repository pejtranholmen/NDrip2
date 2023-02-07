#include "../std.h"
#include "./PGView.h"


CPGView::CPGView(void)
{
	m_RepetitionsFirst=false;
	m_RecordSplit.Length = 1000;


}


CPGView::~CPGView(void)
{
}

bool CPGView::GetToBeViewed(size_t ii)
{
	if(ii<pg.status.a.ToBeViewed.size()) 
		if(pg.status.a.ToBeViewed.at(ii)==1)
			return true;
		else
			return false;
	else
		return true;
}
void CPGView::SetToBeViewed(size_t i, bool value)
{
	if(i<pg.status.a.ToBeViewed.size()) 
		pg.status.a.ToBeViewed[i]= value;
}
size_t CPGView::GetRecordViewStart()
{
	if (m_RecordSplit.Period == 1) return 1;
	else if(m_RecordSplit.SplitStops.size()>m_RecordSplit.Period-2) return m_RecordSplit.SplitStops[m_RecordSplit.Period - 2];
	return 1;
}
size_t CPGView::GetRecordViewSize()
{
	if (m_RecordSplit.Length*m_RecordSplit.Period <=pg.Shape.NumRecords)
		return m_RecordSplit.Length;
	else if (m_RecordSplit.Length*m_RecordSplit.Period>pg.Shape.NumRecords)
		return pg.Shape.NumRecords-m_RecordSplit.Length*(m_RecordSplit.Period-1);
	else
		return m_RecordSplit.Length;
}
void CPGView::UpdateRecordsToView()
{
	if (pg.status.RecordsToView&&m_RecordSplit.SplitStops.size()>0) return;
	size_t count=1;
	

	m_RecordSplit.SplitStops.clear();
	if (pg.Shape.NumRepetitions > 100 && pg.Shape.NumRepetitions < 1000&& m_RecordSplit.Length==1000) m_RecordSplit.Length = 10;
	else if (pg.Shape.NumRepetitions > 1000 && m_RecordSplit.Length == 1000) m_RecordSplit.Length = 2;
	while (pg.Shape.NumRecords > m_RecordSplit.Length*count) {
		m_RecordSplit.SplitStops.push_back(m_RecordSplit.Length*count + 1);
		count++;
	}
	m_RecordSplit.Period= 1;
	pg.status.RecordsToView = true;
}
void CPGView::incrementCurrentPeriodToView()
{
	if (m_RecordSplit.SplitStops.size() > size_t(m_RecordSplit.Period - 1))
		m_RecordSplit.Period = m_RecordSplit.Period++;
	else
		m_RecordSplit.Period = 1;
}
void CPGView::SetCurrentPeriodToView(size_t PeriodToView) {
	m_RecordSplit.Period = PeriodToView;
}
void CPGView::UpdateViewOrder()
{
		m_DataView.clear();
		DataView data;
		size_t ind;
		if(m_RepetitionsFirst) {
			for (size_t i=0;i<pg.Shape.NumVar;i++) {
				for (size_t nrep=0; nrep<pg.Shape.NumRepetitions; nrep++) {
						ind=nrep*pg.Shape.NumVar+i;
						if(pg.status.a.ToBeViewed[ind]) {
							data.NumVar=i+1;
							data.Repit=nrep+1;
							m_DataView.push_back(data);
						}
				}
			}

		}
		else {
			for (size_t nrep=0; nrep<pg.Shape.NumRepetitions; nrep++) {
				for (size_t i=0;i<pg.Shape.NumVar;i++) {
						ind=nrep*pg.Shape.NumVar+i;
						if(pg.status.a.ToBeViewed[ind]) {
							data.NumVar=i+1;
							data.Repit=nrep+1;
							m_DataView.push_back(data);
						}
				}
			}
		}

}
