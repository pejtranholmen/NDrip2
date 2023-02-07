#include "PG_MEMORY.h"
#include <cmath>


PG_Memory::PG_Memory()
{
}


PG_Memory::~PG_Memory()
{
}
void PG_Memory::SetInfoMode()
{
	pg.status.FileVersion = 1;

	if (pg.Des.addinfo.size() != pg.Shape.NumVar*pg.Shape.NumRepetitions) {

		while (pg.Des.addinfo.size()<pg.Shape.NumVar*pg.Shape.NumRepetitions)
			pg.Des.addinfo.push_back(pg.Des.defaultAddInfo);
		while (pg.Des.addinfo.size()>pg.Shape.NumVar*pg.Shape.NumRepetitions)
			pg.Des.addinfo.resize(pg.Shape.NumVar*pg.Shape.NumRepetitions);

	}

}

bool PG_Memory::AdjustSize(size_t irecnumber, size_t varnumber, size_t repit)
{
	SetInfoMode();
	DATASHAPE shape = SetDataShape(irecnumber, varnumber, repit);
	pg.Shape = shape;
	return true;
}
bool PG_Memory::ReAllocateVarVector(){
	size_t localrec = 0, localrec_old = 0;
	size_t mindiff;
	size_t ut, in;
	mindiff = pg.status.NormalTimeInterval;
	
	if (pg.status.CompleteFileRead || pg.status.b.buf_Size >= pg.Shape.NumRecords) {
		std::vector <float> vbuf;
		if (pg.Shape.OldNumRecords*(pg.Shape.OldNumVar + 1) > pg.Var.v.size()) pg.Var.v.resize(pg.Shape.OldNumRecords*(pg.Shape.OldNumVar + 1));
		vbuf.resize((pg.Shape.NumVar + 1)*pg.Shape.NumRecords*pg.Shape.NumRepetitions);
		//float *varbuf=new float[m_recsize*pg.Shape.NumRecords];
		for (size_t irep = 0; irep < pg.Shape.NumRepetitions; irep++) {
			localrec = pg.Shape.NumRecords*(pg.Shape.NumVar + 1)*irep;
			localrec_old = pg.Shape.OldNumRecords*(pg.Shape.OldNumVar + 1)*irep;
			for (size_t i = 0; i < pg.Shape.NumRecords; i++) {
				if (i < pg.Shape.OldNumRecords&&irep < pg.Shape.OldNumRepetitions) {
					ut = localrec + i*pg.Shape.RecordIndexSize;
					in = localrec_old + i*pg.Shape.OldRecordIndexSize;
					vbuf[ut] =pg.Var.v[in];
					if (pg.Shape.OldNumVar > pg.Shape.NumVar) {
						for (size_t ii = 0; ii < pg.Shape.NumVar; ii++) {
							ut = localrec + i*pg.Shape.RecordIndexSize + ii + 1;
							in = localrec_old + i*pg.Shape.OldRecordIndexSize + ii + 1;
							vbuf[ut] =pg.Var.v[in];
						}
					}
					else {
						for (size_t ii = 0; ii < pg.Shape.OldNumVar; ii++) {
							ut = localrec + i*pg.Shape.RecordIndexSize + ii + 1;
							in = localrec_old + i*pg.Shape.OldRecordIndexSize + ii + 1;
							vbuf[ut] =pg.Var.v[in];

						}
						for (size_t ii = pg.Shape.OldNumVar + 1; ii <= pg.Shape.NumVar; ii++) {
							ut = localrec + i*pg.Shape.RecordIndexSize + ii;
							vbuf[ut] = float(MISSING);
						}

					}
				}
				else /*if(i>=pg.Shape.OldNumRecords)*/ {
					in = (pg.Shape.OldNumRecords - 1)*pg.Shape.OldRecordIndexSize;
					ut = localrec + i*pg.Shape.RecordIndexSize;
					size_t t = *(size_t*)&pg.Var.v[in] + (i + 1 - pg.Shape.OldNumRecords)*mindiff;
					vbuf[ut] = *(float*)&t;
					for (size_t ii = 0; ii < pg.Shape.NumVar; ii++) {
						ut = localrec + i*pg.Shape.RecordIndexSize + ii + 1;
						vbuf[ut] = float(MISSING);
					}

				}
			}
		}
		pg.Var.v.swap(vbuf);

	}
	return true;
}
bool PG_Memory::ReAllocateDescription()
{
	DESCRIPTIONS Previous;
	Previous = pg.Des;
	PGFILE_STATUS PrevStatus;
	PrevStatus = pg.status;

	pg.Des.base.Name.resize(pg.Shape.NumVar*pg.Shape.NumRepetitions);
	pg.Des.base.Unit.resize(pg.Shape.NumVar*pg.Shape.NumRepetitions);
	pg.Des.base.Id.resize(pg.Shape.NumVar*pg.Shape.NumRepetitions);
	pg.Des.base.Pos.resize(pg.Shape.NumVar*pg.Shape.NumRepetitions);
	pg.Des.base.var_max.resize(pg.Shape.NumVar*pg.Shape.NumRepetitions);
	pg.Des.base.var_min.resize(pg.Shape.NumVar*pg.Shape.NumRepetitions);
	pg.status.a.ToBeSaved.resize(pg.Shape.NumVar*pg.Shape.NumRepetitions);
	pg.status.a.ToBeViewed.resize(pg.Shape.NumVar*pg.Shape.NumRepetitions);
	pg.status.a.ToBeCalc.resize(pg.Shape.NumVar*pg.Shape.NumRepetitions);
	pg.Des.addinfo.resize(pg.Shape.NumVar*pg.Shape.NumRepetitions);

	size_t index_ut, index_in, index_inmax = 0;

	for (size_t irep = 0; irep<pg.Shape.NumRepetitions; irep++) {
		index_in = min(Previous.base.Name.size(), size_t(irep*pg.Shape.OldNumVar));
		index_inmax = min(Previous.base.Name.size(), size_t((irep + 1)*pg.Shape.OldNumVar));
		for (size_t ii = 0; ii<pg.Shape.NumVar; ii++) {
			index_ut = irep*pg.Shape.NumVar + ii;
			if (Previous.base.Name.size()>index_ut&&index_in < index_inmax) {
					pg.Des.base.Name[index_ut] = Previous.base.Name[index_in];
					pg.Des.base.Unit[index_ut] = Previous.base.Unit[index_in];
					pg.Des.base.Id[index_ut] =	Previous.base.Id[index_in];
					pg.Des.base.Pos[index_ut] = Previous.base.Pos[index_in];
					pg.status.a.ToBeSaved[index_ut] = PrevStatus.a.ToBeSaved[index_in];
					pg.status.a.ToBeViewed[index_ut] = PrevStatus.a.ToBeViewed[index_in];
					pg.status.a.ToBeCalc[index_ut] = PrevStatus.a.ToBeCalc[index_in];
					pg.Des.base.var_min[index_ut] = abs(MISSING);
					pg.Des.base.var_max[index_ut] = MISSING;
					pg.Des.addinfo[index_ut] = Previous.addinfo[index_in];
					index_in++;			
			}
			else if(index_in>0) {
				pg.Des.base.Name[index_ut] = Previous.base.Name[index_in-1];
				pg.Des.base.Unit[index_ut] = Previous.base.Unit[index_in-1];
				pg.Des.base.Id[index_ut] = Previous.base.Id[index_in-1];
				pg.Des.base.Pos[index_ut] = Previous.base.Pos[index_in-1];
				pg.status.a.ToBeSaved[index_ut] = PrevStatus.a.ToBeSaved[index_in-1];
				pg.status.a.ToBeViewed[index_ut] = PrevStatus.a.ToBeViewed[index_in-1];
				pg.status.a.ToBeCalc[index_ut] = PrevStatus.a.ToBeCalc[index_in-1];
				pg.Des.base.var_min[index_ut] = abs(MISSING);
				pg.Des.base.var_max[index_ut] = MISSING;
				pg.Des.addinfo[index_ut] = Previous.addinfo[index_in-1];
			}
			else {
				pg.Des.base.Name[index_ut] = "";
				pg.Des.base.Unit[index_ut] = "";
				pg.Des.base.Id[index_ut] = "";
				pg.Des.base.Pos[index_ut] = "";
				pg.status.a.ToBeSaved[index_ut] =1;
				pg.status.a.ToBeViewed[index_ut] =1;
				pg.status.a.ToBeCalc[index_ut] = 0;
				pg.Des.base.var_min[index_ut] = abs(MISSING);
				pg.Des.base.var_max[index_ut] = MISSING;
				pg.Des.addinfo[index_ut] = pg.Des.defaultAddInfo;
			}
		}
	}

/*	/// New space for data description part
	while (pg.Shape.NumVar*pg.Shape.NumRepetitions>pg.Des.base.Name.size()) {

		pg.Des.base.var_max.push_back(abs(MISSING));
		pg.Des.base.var_min.push_back(MISSING);
		pg.Des.base.Name.push_back("");
		pg.Des.base.Unit.push_back("");
		pg.Des.base.Id.push_back("");
		pg.Des.base.Pos.push_back("");
		pg.status.a.ToBeSaved.push_back(1);
		pg.status.a.ToBeViewed.push_back(1);
		pg.status.a.ToBeCalc.push_back(0);
		pg.Des.addinfo.push_back(pg.Des.defaultAddInfo);
	}*/

	return true;
}


