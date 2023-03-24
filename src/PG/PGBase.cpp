
#include "PGBase.h"

CPGBase::CPGBase(void)
{
	pg.Shape.NumVar=8;
	pg.status.b.buf_TotSize=32000;
	pg.status.b.buf_TotSize = pg.status.b.buf_TotSize/pg.Shape.NumVar;
	pg.status.b.PrevBuf = std::string::npos;
	pg.status.b.NumberOfBuf=1;
	pg.Des.defaultAddInfo.Key=0; pg.Des.defaultAddInfo.Lat=0.;pg.Des.defaultAddInfo.Long=0.;pg.Des.defaultAddInfo.Alt=0.;pg.Des.defaultAddInfo.CountryCode="";
	pg.Des.defaultAddInfo.Element = GENERAL; 
	pg.Des.defaultAddInfo.StationName=""; pg.Des.defaultAddInfo.num_missing=0;pg.Des.defaultAddInfo.num_substituted=0;
	pg.status.CompleteRepRead=false;
	pg.status.FileModified=false;
	pg.status.RecordsToView = false;
	pg.status.CompleteFileRead = true;
	pg.Shape = SetDataShape(0, pg.Shape.NumVar, 1);
	pg.status.a.OutRecordStart = 0;
	pg.status.a.OutRecordEnd = 0;
	pg.status.a.LocalStartIndex = 0;
	pg.status.MinEnd = pg.status.MinStart=0;
}


CPGBase::~CPGBase(void)
{

}
bool CPGBase::CheckStatus() {
	if (pg.Shape.NumRecords > 1&& pg.status.CompleteFileRead) {
		if (pg.status.MinEnd == 0) {
			pg.status.MinStart = *(size_t*)&pg.Var.v[0];
			pg.status.MinEnd = *(size_t*)&pg.Var.v[(pg.Shape.NumRecords - 1) * pg.Shape.RecordIndexSize];
			pg.status.NormalTimeInterval = (pg.status.MinEnd - pg.status.MinStart) / (pg.Shape.NumRecords - 1);
			return true;
		}
		else {
			if (pg.status.MinEnd > pg.status.MinStart) return true;
		}
		return true;
	}
	return false;
}
DATASHAPE CPGBase::SetDataShape(size_t irec, size_t ivar, size_t irep)
{
	DATASHAPE d;
	d.OldNumVar = pg.Shape.NumVar;
	d.NumVar = std::max(ivar, size_t(1));

	d.OldRecordSize = pg.Shape.RecordSize;
	d.OldRecordIndexSize = pg.Shape.RecordIndexSize;

	d.RecordSize = (d.NumVar + 1) * 4;
	d.RecordIndexSize = d.RecordSize / 4;
	d.OldNumRecords = pg.Shape.NumRecords;
	d.NumRecords = irec;

	d.OldNumRepetitions = pg.Shape.NumRepetitions;
	d.NumRepetitions = irep;

	if (irep > 1) {
		d.RepetitionMode = true;
		d.TotalNumberOfRecords = d.NumRecords*d.NumRepetitions;
		d.RecordSize_Repetion = d.NumRecords*d.RecordIndexSize;


	}
	else {
		d.TotalNumberOfRecords = d.NumRecords;
		d.RepetitionMode = false;

	}
	d.RecordTotalSizeData = d.TotalNumberOfRecords*d.RecordSize;
	if (d.NumRecords == 0) pg.status.b.buf_Size = 10000;
	else pg.status.b.buf_Size = d.NumRecords;


	return d;
}

