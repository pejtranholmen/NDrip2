#include "PG_Stream.h"
#include "../Util/FUtil.hpp"
#include "./PGUtil.h"
#ifndef COUPSTD
#include "../NewBase\FileScanner.h"
#include "../MainFrm.h"
#endif


PG_Stream::PG_Stream()
{
	m_time1900 = PGUtil::MinutConv(string("1899-12-30 00:00"));
	m_time1900_Zero = 2880;
	m_BackUpFileName = "";
	m_IsBackUpCreated=false;
	m_UseMainStream = false;
}


PG_Stream::~PG_Stream()
{
	
}
bool PG_Stream::SetActBufferReset()
{
	m_ResetBuffer = true;
	return true;
}
bool PG_Stream::SetActBuffer(size_t ind_buf)
{
	pg.status.b.PrevBuf = string::npos;
	if (ind_buf == 0) return false;
	if (!pg.status.CompleteFileRead)
		return ReadNewBuffer(ind_buf);
	else
		return false;
}
bool PG_Stream::SetActBufferSize(size_t ind_buf)
{
	pg.status.b.PrevBuf = string::npos;
	pg.status.b.buf_TotSize = ind_buf*pg.Shape.NumVar;
	pg.status.b.buf_Size = ind_buf;

	pg.status.b.NumberOfBuf = (pg.Shape.NumRecords - 1) / pg.status.b.buf_Size + 1;
	if (!pg.status.CompleteFileRead)	pg.Var.v.resize(pg.Shape.RecordIndexSize*pg.status.b.buf_Size);
	else pg.status.b.buf_Size = ind_buf;

	if (m_MainPGStreamReadWrite.good())
		return true;
	else
		return false;
}


vector<float> PG_Stream::ReadVarVector(size_t varno)
{
	std::vector <float> vut;
	vut.resize(pg.Shape.NumRecords);
	vector<float> vread;
	vread.resize(pg.Shape.RecordSize);
	if (!m_MainPGStreamReadWrite.good()) m_MainPGStreamReadWrite.clear();
	std::streamsize pos = pg.Shape.RecordSize + varno * 4;
	m_MainPGStreamReadWrite.seekg(pos + pg.status.a.LocalStartIndex * 4, ios::beg);
	for (size_t j = 1; j <= pg.Shape.NumRecords; j++) {
		m_MainPGStreamReadWrite.read((char*)&vread[0], pg.Shape.RecordSize);
		if (m_MainPGStreamReadWrite.good())
			vut[j - 1] = vread[0];
		else
			vut[j - 1] = float(MISSING);
	}
	return vut;
}

bool PG_Stream::AdjustDataShapeAndWriteNewRecords()
{
	size_t mindiff = pg.status.NormalTimeInterval;
	if (pg.Shape.OldNumRecords<pg.Shape.NumRecords) {
		//To make new records with new time points
		// and missing values a new values to the new records
		CloseFile();
		ReOpen(true);

		std::streamsize newpos = pg.Shape.RecordSize*(pg.Shape.OldNumRecords + 1);

		size_t prevend = GetLongTime(pg.Shape.OldNumRecords);
		m_MainPGStreamReadWrite.seekp(newpos, ios::beg);
		if (m_MainPGStreamReadWrite.good()) {
			for (size_t i = pg.Shape.OldNumRecords; i<pg.Shape.NumRecords; i++) {
				size_t t = prevend + (i + 1 - pg.Shape.OldNumRecords)*mindiff;
				pg.Var.v[0] = *(float*)&t;
				for (size_t ii = 0; ii<pg.Shape.NumVar; ii++)
					pg.Var.v[ii + 1] = float(MISSING);
				WriteNewPGFileRecord();
			}
		}
		if (!m_MainPGStreamReadWrite.good()) {
			pg.Shape.NumRecords = pg.Shape.OldNumRecords;
			return false;

		}
	}

	pg.Shape.TotalNumberOfRecords = pg.Shape.NumRecords*pg.Shape.NumRepetitions;
	pg.Var.v.resize((pg.Shape.NumVar + 1)*pg.status.b.buf_Size);
	pg.status.b.NumberOfBuf = (pg.Shape.TotalNumberOfRecords - 1) / pg.status.b.buf_Size + 1;
	return false;
}

size_t PG_Stream::GetLongTime(size_t recordno)
{
	return size_t(4);
}

fstream PG_Stream::GetLocalReadOnlyStream(string filename)
{
	fstream	driveFile;
	filename = PGUtil::CheckedForExistingBinFileName(filename);
	driveFile.open(filename, ios::in | ios::binary);
	if (!driveFile.is_open()) {
		string UserDirectory = "";
		UserDirectory = FUtil::GetProfileStringStd("UserDirectory", UserDirectory);
		size_t numo;
		numo = UserDirectory.size();
		if (UserDirectory.rfind("\\") != numo - 1) {
			UserDirectory += "\\";
			numo++;
		}
		size_t n = filename.rfind("\\");
		filename = filename.substr(n + 1);
		pg.status.FileName = UserDirectory + filename;
		driveFile.open(pg.status.FileName, ios::in | ios::binary);

		if (!driveFile.good()) {
			driveFile.clear();
			driveFile.open(pg.status.FileName, ios::in | ios::binary);
		}

		if (!driveFile.is_open()) {
			string UserDirectory = "";
			UserDirectory = FUtil::GetProfileStringStd("UserSubDirectory", UserDirectory);
			string testname;
			size_t ust = filename.rfind("\\");

			string file = filename.substr(ust + 1);



			testname = UserDirectory + file;
			driveFile.open(testname, ios::in | ios::binary);
			if (driveFile.is_open()) {
#ifndef COUPSTD
				TRACE("Can't Open file %s´\n", pg.status.FileName);
				//Display error message
				string str = "The PG-file: ";
				str += testname; str += " couldn´t be opened.";
				if (m_ShowError) MFC_Util::MessageBox(str.c_str(), "File open error", MB_OK | MB_ICONEXCLAMATION);
#endif
				return driveFile;
				// Actions without file name ?
			}
			pg.status.FileName = testname;
		}
		filename = pg.status.FileName;


	}
	return driveFile;
}

bool PG_Stream::ReadPGStartHeaderFromStream(fstream *pReadOnlyStream) {
	pg.Shape.RepetitionMode = false;
	pReadOnlyStream->seekg(0, ios::end);
	auto posend = pReadOnlyStream->tellg();
	pReadOnlyStream->seekg(0, ios::beg);


	pReadOnlyStream->read((char*)&pg.Shape.NumRecords, 4);
	auto OrgRec = pg.Shape.NumRecords;
	//size_t recsize, totsizedata;
	unsigned int ivar = 0;
	pReadOnlyStream->read((char*)&ivar, 4);
	pg.Shape.NumVar = ivar / 4 - 1;
	
	unsigned int nrep = 0;
	if (pg.Shape.NumVar>0)
		pReadOnlyStream->read((char*)&nrep, 4);
	if (nrep>1 && pg.Shape.NumRecords>nrep)
		SetRepititionMode(true, nrep);
	else
		nrep = 1;

	if ((nrep == 4||nrep==64) && (!Check_PGFileSize(pReadOnlyStream, OrgRec/nrep, pg.Shape.NumVar, nrep)||nrep>OrgRec/2)) {
		nrep = 1;
		pg.Shape.NumRecords = OrgRec;
		SetRepititionMode(false, 1);
	}
	
	std::streamsize newpos=ivar;
	pReadOnlyStream->seekg(newpos, ios::beg);
	if (pReadOnlyStream->good()) {
		pg.Shape=SetDataShape(pg.Shape.NumRecords, pg.Shape.NumVar, nrep);
		return true;
	}
	else {
		pg.Shape.NumRecords=pg.Shape.OldNumRecords;
		pg.Shape.NumVar = pg.Shape.OldNumVar;
		pg.Shape.NumRepetitions = pg.Shape.OldNumRepetitions;
		return false;
	};
}

bool PG_Stream::ReadVariableValuesFromStream(fstream *pReadOnlyStream) {
	if (pg.status.CompleteFileRead) {
		pReadOnlyStream->seekg(pg.Shape.RecordSize, ios::beg);
		if (!pReadOnlyStream->good()) return false;
		pg.Var.v.resize(pg.Shape.RecordIndexSize*pg.Shape.TotalNumberOfRecords);
		vector<float> var = pg.Var.v;
		pReadOnlyStream->read((char*)&var[0], pg.Var.v.size() * sizeof(float));

		if (pReadOnlyStream->good()) {
			pg.Var.v = var;
			pg.status.MinStart = *(size_t*)&pg.Var.v[0];
			pg.status.MinEnd = *(size_t*)&pg.Var.v[(pg.Shape.NumRecords - 1)*pg.Shape.RecordIndexSize];
			if (pg.Shape.NumRecords > 1)
				pg.status.NormalTimeInterval = (pg.status.MinEnd - pg.status.MinStart) / (pg.Shape.NumRecords - 1);
			return true;
		}
		else
			return false;
	}
	else {
		pg.Var.v.resize(pg.Shape.RecordIndexSize*pg.status.b.buf_Size);
		ReadFirst_Buffer();


		auto newpos = pg.Shape.RecordSize;
		pReadOnlyStream->seekg(newpos, ios::beg);
		auto ikoll = pReadOnlyStream->tellg();
		if (!pReadOnlyStream->good()) return false;
		pReadOnlyStream->read((char*)&pg.status.MinStart, 4);
		pg.status.CompleteFileRead = false;

		newpos = pg.Shape.RecordSize*pg.Shape.NumRecords;
		if (!pReadOnlyStream->good()) return false;
		pReadOnlyStream->seekg(newpos, ios::beg);
		ikoll = pReadOnlyStream->tellg();
		pReadOnlyStream->read((char*)&pg.status.MinEnd, 4);
		if (!pReadOnlyStream->good()) return false;
		if (pg.Shape.NumRecords>1)
			pg.status.NormalTimeInterval = (pg.status.MinEnd - pg.status.MinStart) / (pg.Shape.NumRecords - 1);

		newpos = pg.Shape.RecordSize*(pg.Shape.TotalNumberOfRecords + 1); //End of data
		pReadOnlyStream->seekg(newpos, ios::beg);
		if (!pReadOnlyStream->good()) return false;
		auto actpos = pReadOnlyStream->tellg();
		if (!pReadOnlyStream->good()) return false;
	}

	return true;
}

bool PG_Stream::ReadDescriptionsFromStream(fstream *pReadOnlyStream)
{
	auto SetStartPos = [&]() {
		std::streampos newpos = pg.Shape.RecordSize*(pg.Shape.TotalNumberOfRecords+1);
		pReadOnlyStream->seekg(newpos, ios::beg);
		auto actpos = pReadOnlyStream->tellg();
		if(actpos==newpos) return true;
		else return false;
	};


	if (!SetStartPos()) return false;

	size_t descsize = pg.Shape.NumVar * 60;
	ReAllocateDescription();

	string xs;
	size_t ind = 0;
	std::streamsize actpos;
	vector <char> buf;
	buf.resize(descsize);
	actpos = pReadOnlyStream->tellg();
	float* minx, *maxx;
	auto CheckAndSetMinMax = [&](size_t i) {
		if (*maxx <= *minx) {
			// Check new max and min values
			float fmax, fmin;
			fmax = -1.e38f; fmin = 1.e38f;
			for (size_t ii = 0; ii<pg.Shape.NumRecords; ii++) {
				float fval = pg.Var.v[GetVarIndex(i + 1, ii + 1)];
				if (fval<fmin&&fval>-1.E37) fmin = fval;
				if (fval>fmax&&fval>-1.E37) fmax = fval;
			}
			*minx = fmin;
			*maxx = fmax;
		}

	};
	auto SetDescriptionsFromFixBuf = [&](size_t i, size_t nrep) {
		ind = i + nrep*pg.Shape.NumVar;
		minx = (float*)&buf[i * 60 + 52];
		pg.Des.base.var_min[ind] = *minx;
		maxx = (float*)&buf[i * 60 + 56];
		pg.Des.base.var_max[ind] = *maxx;
		//CheckAndSetMinMax(i);
		if (i * 60 + 20 <= descsize)
			pg.Des.base.Name[ind].assign((char*)&buf[i * 60], 20);
		if (i * 60 + 40 <= descsize)
			pg.Des.base.Unit[ind].assign(&buf[i * 60 + 20], 8);
		if (i * 60 + 48 <= descsize)
			pg.Des.base.Id[ind].assign(&buf[i * 60 + 28], 20);
		if (i * 60 + 52 <= descsize)
			pg.Des.base.Pos[ind].assign(&buf[i * 60 + 48], 4);

		pg.status.a.ToBeSaved[ind] = true;
		pg.status.a.ToBeViewed[ind] = true;
		pg.status.a.ToBeCalc[ind] = false;
	
	};
	for (size_t nrep = 0; nrep<pg.Shape.NumRepetitions; nrep++) {
		pReadOnlyStream->read((char*)&buf[0], descsize);
		actpos = pReadOnlyStream->tellg();
		for (size_t i = 0; i<pg.Shape.NumVar; i++) {
			SetDescriptionsFromFixBuf(i, nrep);
		}
	}
	size_t firsthead = pg.Shape.NumVar*pg.Shape.NumRepetitions * 60;
	std::streamsize next = firsthead + pg.Shape.RecordSize + pg.Shape.RecordTotalSizeData;
	std::streamsize actpos2 = pReadOnlyStream->tellg();

	pReadOnlyStream->seekg(next, ios::beg);
	actpos = pReadOnlyStream->tellg();

	if (pReadOnlyStream->good()) {
		pReadOnlyStream->read((char*)&pg.status.FileVersion, 4);
		if (!pReadOnlyStream->good() || (int)pg.status.FileVersion > 1 || (int)pg.status.FileVersion < 0) {
			pg.status.FileVersion = 0;
			pReadOnlyStream->clear();
		}
		else if (pg.status.FileVersion == 1) {// Read
			unsigned int n1, n2, n;
			SetInfoMode();
			vector<AddInfo> Add = pg.Des.addinfo;
			auto SetAddInfoValue = [&](size_t i, size_t nrep) {
              size_t ind = nrep*pg.Shape.NumVar + i;
                pReadOnlyStream->read((char*)&Add[ind].Lat, 8);
                pReadOnlyStream->read((char*)&Add[ind].Long, 8);
                pReadOnlyStream->read((char*)&Add[ind].Alt, 8);
                actpos2 = pReadOnlyStream->tellg();
                pReadOnlyStream->read((char*)&n1, 4);
                Add[ind].CountryCode.resize(n1);
                if (n1 != 4)
                    pReadOnlyStream->read((char*)&Add[ind].CountryCode[0], n1);
                else
                    pReadOnlyStream->read((char*)&n, 4);
                pReadOnlyStream->read((char*)&n2, 4);
                Add[ind].StationName.resize(n2);
                if (!(n2 == 4 && n1 == 4))
                    pReadOnlyStream->read((char*)&Add[ind].StationName[0], n2);
                else
                    pReadOnlyStream->read((char*)&n, 4);
                pReadOnlyStream->read((char*)&Add[ind].num_missing, 4);
                pReadOnlyStream->read((char*)&Add[ind].num_substituted, 4);
                actpos2 = pReadOnlyStream->tellg();
                if (n1 == 4 && n2 == 4) {
                    Add.erase(Add.begin() + ind);
                    Add.insert(Add.begin() + ind, pg.Des.defaultAddInfo);
                }
			};
			for (size_t nrep = 0; nrep < pg.Shape.NumRepetitions; nrep++) {
				for (size_t i = 0; i < pg.Shape.NumVar; i++) {				
					SetAddInfoValue(i, nrep);
				}
			}
			if (pReadOnlyStream->good()) pg.Des.addinfo = Add;
		}
	}

	if (pReadOnlyStream->good()) 	
		return true;
	else 
		return false;
}
bool PG_Stream::WriteDescriptionsToStream(fstream * pWriteEnabledStream, size_t RepNoInput_ToStore)
{
	
	if (pWriteEnabledStream == nullptr) pWriteEnabledStream = &m_MainPGStreamReadWrite;
	if (pWriteEnabledStream == nullptr) {
		if (m_UseMainStream) pWriteEnabledStream = &m_MainPGStreamReadWrite;
		else  pWriteEnabledStream = &m_ExtraPGStreamWrite;
	}
	auto CheckAndSetStartPosition = [&]() {
		if (RepNoInput_ToStore == string::npos) {
			std::streampos pos = (pg.Shape.TotalNumberOfRecords + 1) * pg.Shape.RecordSize;
			pWriteEnabledStream->seekp(pos, ios::beg);
			auto pos2 = pWriteEnabledStream->tellp();
		}
		else {
			std::streampos pos = (pg.Shape.NumRecords + 1) * pg.Shape.RecordSize;
			pWriteEnabledStream->seekp(pos, ios::beg);
			auto pos2 = pWriteEnabledStream->tellp();
		}
		return true;
	};
	
	
	
	
	size_t descsize = pg.Shape.NumVar * 60;
	char buf2[100];
	auto WriteFixedDescription = [&](size_t i, size_t nrep) {
		string buf;
		size_t ind = nrep*pg.Shape.NumVar + i;
		buf = pg.Des.base.Name[ind];
		for (size_t ii = 0; ii<20; ii++) {
			if (buf.size() <= ii)
				buf2[ii] = ' ';
			else
				buf2[ii] = buf.at(ii);
		}
		buf = pg.Des.base.Unit[ind];
		for (size_t ii = 0; ii<8; ii++) {
			if (buf.size() <= ii)
				buf2[ii + 20] = ' ';
			else
				buf2[ii + 20] = buf.at(ii);
		}

		buf = pg.Des.base.Id[ind];
		for (size_t ii = 0; ii<20; ii++) {
			if (buf.size() <= ii)
				buf2[ii + 28] = ' ';
			else
				buf2[ii + 28] = buf.at(ii);
		}

		buf = pg.Des.base.Pos[ind];
		for (size_t ii = 0; ii<4; ii++) {
			if (buf.size() <= ii)
				buf2[ii + 48] = ' ';
			else
				buf2[ii + 48] = buf.at(ii);
		}

		float fmin, fmax;
		if (pg.Des.base.var_min.size()>ind)
			fmin = pg.Des.base.var_min[ind];
		else
			fmin = 100.;
		if (pg.Des.base.var_max.size()>ind)
			fmax = pg.Des.base.var_max[ind];
		else
			fmax = -1.;

		pWriteEnabledStream->write((char*)&buf2, 52);
		pWriteEnabledStream->write((char*)&fmin, 4);
		pWriteEnabledStream->write((char*)&fmax, 4);
	};

	CheckAndSetStartPosition();

	if (RepNoInput_ToStore != string::npos) {
		for (size_t i = 0; i < pg.Shape.NumVar; i++) {
			WriteFixedDescription(i, RepNoInput_ToStore);
		}

	}
	else {
		for (size_t nrep = 0; nrep < pg.Shape.NumRepetitions; nrep++) {
			for (size_t i = 0; i < pg.Shape.NumVar; i++) {
				WriteFixedDescription(i, nrep);
			}
		}
	}
	if (pg.status.FileVersion>0) {

		auto WriteToStream = [&](size_t ind) {
			pWriteEnabledStream->write((char*)&pg.Des.addinfo[ind].Lat, 8);
			pWriteEnabledStream->write((char*)&pg.Des.addinfo[ind].Long, 8);
			pWriteEnabledStream->write((char*)&pg.Des.addinfo[ind].Alt, 8);
			unsigned int n = pg.Des.addinfo[ind].CountryCode.size();
			pWriteEnabledStream->write((char*)&n, 4);
			pWriteEnabledStream->write(pg.Des.addinfo[ind].CountryCode.data(), n);
			n = pg.Des.addinfo[ind].StationName.size();
			pWriteEnabledStream->write((char*)&n, 4);
			pWriteEnabledStream->write(pg.Des.addinfo[ind].StationName.data(), n);
			pWriteEnabledStream->write((char*)&pg.Des.addinfo[ind].num_missing, 4);
			pWriteEnabledStream->write((char*)&pg.Des.addinfo[ind].num_substituted, 4);
		};

		bool good;
	
		auto actpos = pWriteEnabledStream->tellp();
		pWriteEnabledStream->write((char*)&pg.status.FileVersion, 4);
		actpos = pWriteEnabledStream->tellp();
		good = pWriteEnabledStream->good();

		if (good) {
			if (pg.Des.addinfo.size()<pg.Shape.NumRepetitions*pg.Shape.NumVar) {
				pg.Des.addinfo.resize(pg.Shape.NumRepetitions*pg.Shape.NumVar);
			}
			if (RepNoInput_ToStore == string::npos) {
				for (size_t nrep = 0; nrep < size_t(pg.Shape.NumRepetitions); nrep++) {
					for (size_t i = 0; i < pg.Shape.NumVar; i++) {
						size_t ind = nrep * pg.Shape.NumVar + i;
						WriteToStream(ind);
					}
				}
			}
			else {
				for (size_t i = 0; i < pg.Shape.NumVar; i++) {
					size_t ind = RepNoInput_ToStore * pg.Shape.NumVar + i;
					WriteToStream(ind);
				}

			}
		}
		return pWriteEnabledStream->good();
	}
	return false;
}
bool PG_Stream::WritePGFileStartHeader(size_t nrecords, size_t nrepititions, size_t InputRepNo, fstream *pPGOutStream)
{
	if (pPGOutStream == nullptr) {
		if (m_UseMainStream) pPGOutStream = &m_MainPGStreamReadWrite;
		else  pPGOutStream = &m_ExtraPGStreamWrite;
	}
	
	unsigned int lengthRecord = pg.Shape.NumVar * 4 + 4;
	std::streampos newpos = 0;
	std::streampos actpos;
	unsigned int totnumrecords;

	if (nrepititions>0 && InputRepNo != string::npos)
		totnumrecords = nrecords*nrepititions;
	else if(nrepititions>0) 
		totnumrecords = nrecords * nrepititions;
	else
		totnumrecords = nrecords;

	pPGOutStream->seekp(newpos, ios::beg);
	actpos = pPGOutStream->tellp();
	pPGOutStream->write((char*)&totnumrecords, 4);
	pPGOutStream->write((char*)&lengthRecord, 4);

	if (InputRepNo == string::npos&&nrepititions<=1) {
		unsigned int NewNumberOfRepititions = 1;
		pPGOutStream->write((char*)&NewNumberOfRepititions, 4);
	}
	else if (pg.Shape.NumVar>1) {
		pPGOutStream->write((char*)&pg.Shape.NumRepetitions, 4);
	}
	if (pg.Shape.NumVar == 0) return false;

	for (size_t i = 1; i<pg.Shape.NumVar - 1; i++) {
		unsigned int ldum = 0;
		actpos = pPGOutStream->tellp();
		pPGOutStream->write((char*)&ldum, 4);
	}

	return pPGOutStream->good();

}
bool PG_Stream::IsOpen() const
{
	if (m_MainPGStreamReadWrite.is_open())
		return true;
	else
		return false;
}
bool PG_Stream::MakeFileToEnableWriting(bool MakeCopy)
{

	if (m_WriteEnabled&&IsOpen()) return true;
	CloseFile();
	;	if (MakeCopy) MakeBackUpCopy();
	return ReOpen(true);
}
bool PG_Stream::MakeBackUpCopy()
{
	string filename = pg.status.FileName;
	auto pos = filename.rfind('.');
	size_t Num = 1;
	if (pos != string::npos) {
		filename = pg.status.FileName.substr(0, pos) + "_Copy" + FUtil::STD_ItoAscii(Num)+".bin";
		while (FUtil::IsFileExisting(filename)) {
			Num++;
			filename = pg.status.FileName.substr(0, pos) + "_Copy" + FUtil::STD_ItoAscii(Num) + ".bin";
		}
		if (FUtil::CopyFile_Std(pg.status.FileName, filename)) {
			m_BackUpFileName=filename;
			m_IsBackUpCreated = true;
		}
	}
	return false;
}
bool PG_Stream::ReturnToBackUpCopy(bool Yes)
{
	if (Yes) {
		if (FUtil::CopyFile_Std(m_BackUpFileName, pg.status.FileName)) {
	
			if (remove(m_BackUpFileName.data())) {
				m_BackUpFileName = "";
				return true;
			}
			else
				return false;
		}
	}
	else if (m_BackUpFileName.size() > 0) {
		if (remove(m_BackUpFileName.data())) {
			m_BackUpFileName = "";
			return true;
		}
		else
			return false;


	}
	return false;
}
bool PG_Stream::IsGood()
{
	if (m_MainPGStreamReadWrite.good())
		return true;
	else
		return false;
}
bool PG_Stream::ClearFile() 
{
	m_MainPGStreamReadWrite.clear();
	pg.status.b.PrevBuf=string::npos;
	m_IsOpen=false;
	return ReOpen(m_WriteEnabled);
}
void PG_Stream::CloseFile()
{
	m_MainPGStreamReadWrite.clear();
	if(m_MainPGStreamReadWrite.good()) {
		m_MainPGStreamReadWrite.close();
		m_Closed=true;
		m_IsOpen=false;

	}
	m_ExtraPGStreamWrite.clear();
	m_ExtraPGStreamWrite.close();

}
bool PG_Stream::Save(string filename)
{
	pg.status.FileName=filename;
	fstream driveFile;
	if(FUtil::IsFileExisting(filename))
	 	m_MainPGStreamReadWrite.open(filename, ios::in|ios::out|ios::binary);
	else {
		m_MainPGStreamReadWrite.open(filename, ios::out|ios::binary);
		m_MainPGStreamReadWrite.close();
		driveFile.open(pg.status.FileName, ios::out|ios::in|ios::binary);
	}
	if(!driveFile.is_open())
		{
#ifndef COUPSTD
			TRACE("Can't Open file %s´\n", pg.status.FileName); 
					string str;
					str="The PG-file: ";str+=pg.status.FileName; str+=" couldn´t be opened.";
			//Display error message
			MFC_Util::MessageBox(str.c_str(),"File Open Error",MB_OK|MB_ICONEXCLAMATION);
			return false;
#endif
		}
	driveFile.close();
	m_Closed=true;
	return true;
}
bool PG_Stream::SaveAs(string filename, bool NewFile)
{
	bool oldFileOpen = false;
	pg.status.FileName = filename;

    cout << "SAVE AS: " << filename << "\n";
	if (m_MainPGStreamReadWrite.good()) {
		ReOpen();
		oldFileOpen = true;
	}


	if (NewFile || !oldFileOpen) {
		if (m_MainPGStreamReadWrite.good())
			m_MainPGStreamReadWrite.close();
		if (FUtil::IsFileExisting(filename))
			m_MainPGStreamReadWrite.open(filename, ios::in | ios::out | ios::binary);
		else
			m_MainPGStreamReadWrite.open(filename, ios::out | ios::binary);

		if (!m_MainPGStreamReadWrite.is_open()) {

			m_ExtraPGStreamWrite.close();
			m_MainPGStreamReadWrite.open(filename, ios::out | ios::in | ios::binary);
			if (!m_MainPGStreamReadWrite.is_open()) {
#ifndef COUPSTD
				TRACE("Can't Open file %s´\n", filename);
				string str;
				str = "The PG-file: "; str += filename; str += " couldn´t be opened.";
				//Display error message
				MFC_Util::MessageBox(str.c_str(), "File Open Error", MB_OK | MB_ICONEXCLAMATION);
#endif				
				return false;
			}
			if(NewFile) m_UseMainStream=true;
			// Actions without file name ?
		}
		else
			if (NewFile) m_UseMainStream = true;

		if (pg.Shape.RecordSize != 0) {
			if (pg.status.CompleteFileRead) {
				WritePGFile();
				m_MainPGStreamReadWrite.close();
			}
			else {
				if (pg.Shape.RepetitionMode)
					WritePGFileHeader(pg.Shape.NumRecords, pg.Shape.NumRepetitions);
				else
					WritePGFileHeader(pg.Shape.NumRecords, 1);
			}


		}
	}
	else {
		m_ExtraPGStreamWrite.open(filename, ios::in | ios::out | ios::binary);
		if (!m_ExtraPGStreamWrite.is_open()) {
			m_ExtraPGStreamWrite.close();
			m_ExtraPGStreamWrite.open(filename, ios::out | ios::in | ios::binary);
			if (!m_ExtraPGStreamWrite.is_open()) {
#ifndef COUPSTD
				TRACE("Can't Open file %s´\n", filename);
				string str;
				str = "The PG-file: "; str += filename; str += " couldn´t be opened.";
				//Display error message
				MFC_Util::MessageBox(str.c_str(), "File Open Error", MB_OK | MB_ICONEXCLAMATION);
#endif
				return false;
			}
			// Actions without file name ?
		}
		if (pg.Shape.NumRecords>0 && oldFileOpen) {
			WritePGFileFrame();
			m_IsOpen = true;
			m_MainPGStreamReadWrite.close();
			m_ExtraPGStreamWrite.close();

		}
	}



	//pg.status.FileName=filename;
	return true;
}

void PG_Stream::SetNewFileName(string filename, bool RemoveOld)
{
	size_t result;
	if(RemoveOld) {
		result=rename(pg.status.FileName.data(), filename.data());  
	
		if(result!=0) {
			result=remove("Dump.bin");
			result=rename(pg.status.FileName.data(), filename.data());  
		}
	}
	else {
		result=FUtil::CopyFile_Std(pg.status.FileName, filename);

	}

	pg.status.FileName=filename;
}
void PG_Stream::SetFileName(string filename)
{
	pg.status.FileName=filename;
}
bool PG_Stream::Open(string filename, bool ShowError)
{
#ifndef COUPSTD
	CWinApp* pApp=AfxGetApp();
#endif
	m_OnlyNewFile=false;
	m_ShowError = ShowError;

	if(m_IsOpen)
		return true; // When file already open
	m_IsOpen=false;	
	filename = PGUtil::CheckedForExistingBinFileName(filename);

		
		if(pg.status.CompleteFileRead) {
			if(!GetAllFileInfo(filename, ShowError)) {
				if(ShowError) {
#ifndef COUPSTD
				TRACE("Can't Open file %s to show all information", pg.status.FileName); 
					string str;
					str="The PG-file: ";str+=pg.status.FileName; str+=" couldn´t be opened.";
			//Display error message
				MFC_Util::MessageBox(str.c_str(),"File Open Error",MB_OK|MB_ICONEXCLAMATION);
#endif
				}
				return false;		
			}
		}
		else {
			if(!ReadContentOfPGFileAndOpenMainPGStreamForReading(filename)) {
				if(ShowError) {
#ifndef COUPSTD
				TRACE("Can't Open file %s to show description of data", pg.status.FileName); 
					string str;
					str="The PG-file: ";str+=pg.status.FileName; str+=" couldn´t be opened.";
			//Display error message
				MFC_Util::MessageBox(str.c_str(),"File Open Error",MB_OK|MB_ICONEXCLAMATION);
#endif

				}
				return false;	

			}
		};
#ifndef COUPSTD
		pApp->EndWaitCursor();
#endif
	m_IsOpen=true;
	return true;
}
bool PG_Stream::ReOpen(bool write) 
{	
	bool result;
	if(!m_Closed) return false;
	if(m_MainPGStreamReadWrite.is_open()&&!m_WriteEnabled&&m_MainPGStreamReadWrite.good()&&!write) return true;
	if(write) {
		m_MainPGStreamReadWrite.clear();
		if(!FUtil::IsFileExisting(pg.status.FileName))
			m_MainPGStreamReadWrite.open(pg.status.FileName, ios::out|ios::binary);
		else
			m_MainPGStreamReadWrite.open(pg.status.FileName, ios::out|ios::in|ios::binary);
		m_WriteEnabled=true;
	}
	else {

		m_MainPGStreamReadWrite.clear();
		m_MainPGStreamReadWrite.close();
		m_MainPGStreamReadWrite.open(pg.status.FileName, ios ::in|ios::binary);
		m_WriteEnabled=false;
		if(!m_MainPGStreamReadWrite.good()) {
			m_MainPGStreamReadWrite.clear();
			m_MainPGStreamReadWrite.open(pg.status.FileName, ios ::in|ios::binary);

		}
	
	}
	result=m_MainPGStreamReadWrite.is_open();
	if(result==true) m_IsOpen=true;
	return result;
}
bool PG_Stream::CreateNewFile(size_t numv, size_t numr, size_t repit)
{
	pg.Shape.NumVar = numv;
	pg.Shape.NumRepetitions = repit;
	pg.Shape.RecordSize = (pg.Shape.NumVar + 1) * 4;
	pg.Shape.RecordIndexSize = pg.Shape.RecordSize / 4;
	pg.Shape.RecordSize_Repetion = numr*pg.Shape.RecordIndexSize;

	if (repit>1) {
		pg.Shape.NumRecords = numr*repit;
		pg.Shape.TotalNumberOfRecords = pg.Shape.NumRecords;
		pg.Shape.RepetitionMode = true;
		pg.status.b.buf_Size = pg.Shape.RecordSize_Repetion;
		pg.status.b.NumberOfBuf = 0;
		pg.status.a.LocalStartIndex = 0;
	}

	else {
		pg.Shape.TotalNumberOfRecords = numr;
		pg.Shape.NumRecords = numr;
		pg.Shape.RepetitionMode = false;
		pg.status.a.LocalStartIndex = 0;
	}


	pg.Shape.RecordTotalSizeData = pg.Shape.NumRecords*pg.Shape.RecordSize;
	size_t max = pg.Var.v.max_size();

	if (pg.status.CompleteFileRead) {
		size_t koll = pg.Shape.NumRecords*(pg.Shape.NumVar + 1);
		size_t max = pg.Var.v.max_size() / 6;
		float ratio = float(koll) / float(max);


		if (koll >= max) {
			return false;

		}
		else
			pg.Var.v.resize(koll);

	}
	else if (pg.status.CompleteRepRead) {
		if (numr*(pg.Shape.NumVar + 1) > max) {
			return false;
		}
		else

			pg.Var.v.resize(numr*(pg.Shape.NumVar + 1));


	}
	else
		pg.Var.v.resize(pg.Shape.NumVar + 1);

	pg.Shape.NumRecords = numr;

	size_t descsize = pg.Shape.NumVar * 60;
	m_str = new char[descsize];
	for (size_t i = 0; i<descsize; i++)
		*(m_str + i) = ' ';
	string xs;
	xs.assign(m_str, descsize);

	//	for(size_t i=0;i<pg.Shape.NumVar;i++) {
	pg.Des.base.Name.resize(pg.Shape.NumVar*repit);
	pg.Des.base.Id.resize(pg.Shape.NumVar*repit);
	pg.Des.base.Unit.resize(pg.Shape.NumVar*repit);
	pg.Des.base.Pos.resize(pg.Shape.NumVar*repit);
	pg.Des.base.var_max.resize(pg.Shape.NumVar*repit);
	pg.Des.base.var_min.resize(pg.Shape.NumVar*repit);
	pg.status.a.ToBeSaved.resize(pg.Shape.NumVar*repit);
	pg.status.a.ToBeViewed.resize(pg.Shape.NumVar*repit);
	pg.status.a.ToBeCalc.resize(pg.Shape.NumVar*repit);
	pg.Des.addinfo.resize(pg.Shape.NumVar*repit);
	//	}
	float* minx, *maxx;
	for (size_t nrep = 0; nrep<repit; nrep++) {
		for (size_t i = 0; i<pg.Shape.NumVar; i++) {
			size_t add = nrep*pg.Shape.NumVar;
			size_t ind = i + add;
			minx = (float*)&m_str[i * 60 + 52];
			pg.Des.base.var_min[ind] = *minx;
			maxx = (float*)&m_str[i * 60 + 56];
			pg.Des.base.var_max[ind] = *maxx;
			if (*maxx<*minx) {
				// Check new max and min values
				float fmax, fmin;
				fmax = -1.e38f; fmin = 1.e38f;
		/*		for (size_t ii = 0; ii<pg.Shape.NumRecords; ii++) {
					float fval = GetVarValue(i + 1, ii + 1);
					if (fval<fmin&&fval>-1.E37) fmin = fval;
					if (fval>fmax&&fval>-1.E37) fmax = fval;
				}*/
				*minx = fmin;
				*maxx = fmax;
			}
			pg.Des.base.Name[ind] = xs.substr(i * 60, 20);
			pg.Des.base.Unit[ind] = xs.substr(i * 60 + 20, 8);
			pg.Des.base.Id[ind] = xs.substr(i * 60 + 28, 20);
			pg.Des.base.Pos[ind] = xs.substr(i * 60 + 48, 4);
			pg.status.a.ToBeSaved[ind] = 1;
			pg.status.a.ToBeViewed[ind] = 1;
			pg.status.a.ToBeCalc[ind] = 0;
			pg.Des.addinfo[ind] = pg.Des.defaultAddInfo;
		}
	}
	m_time1900 = PGUtil::MinutConv(string("1899-12-30 00:00"));
	m_time1900_Zero = 2880;
	return true;
}
bool PG_Stream::MakeNewPGFileName()
{
	size_t ipos = pg.status.FileName.find_last_of(".bin");
	if (ipos>0) {
		string base = pg.status.FileName.substr(0, ipos - 3);
		size_t count = 1;
		pg.status.NewFileName = base;
		pg.status.NewFileName.append("_");
		pg.status.NewFileName.append(FUtil::STD_ItoAscii(count));
		pg.status.NewFileName.append(".bin");
		while (FUtil::IsFileExisting(pg.status.NewFileName)) {
			count++;
			pg.status.NewFileName = base;
			pg.status.NewFileName.append("_");
			pg.status.NewFileName.append(FUtil::STD_ItoAscii(count));
			pg.status.NewFileName.append(".bin");
		}
	}
	return true;

}
bool PG_Stream::WritePGFileRecord(size_t var_rec, size_t repit)
{ // Set Position to m_driveFile and write the record to that position
	std::streamsize newpos;
	size_t CurrentBuf;
	CurrentBuf = (var_rec - 1) / pg.status.b.buf_Size + 1;
	if (pg.Var.v.size()>pg.Shape.RecordIndexSize) {
		return WriteNewBuffer(CurrentBuf);
	}


	newpos = pg.Shape.RecordSize*var_rec + (repit - 1)*pg.Shape.RecordSize_Repetion * 4;
	m_MainPGStreamReadWrite.seekp(newpos, ios::beg);
	m_MainPGStreamReadWrite.write((char*)&pg.Var.v[0], pg.Shape.RecordSize);
	pg.status.FileModified = true;
	if (m_MainPGStreamReadWrite.good())
		return true;
	else
		return false;

}
bool PG_Stream::WritePGFileVarValue(size_t varno, size_t var_rec, float value, size_t repit)
{
	std::streamsize newpos = pg.Shape.RecordSize*var_rec + pg.Shape.RecordSize*(repit - 1)*pg.Shape.NumRecords + varno * 4;
	m_MainPGStreamReadWrite.seekg(newpos, ios::beg);
	m_MainPGStreamReadWrite.write((char*)&value, 4);
	if (m_MainPGStreamReadWrite.good())
		return true;
	else
		return false;

}
std::streamsize  PG_Stream::WriteNewPGFileRecord(size_t rec)
{
	//m_driveFile.seekp(newpos,ios::beg);
	if (rec == string::npos) {
		//return string::npos;
	}
	auto newpos = m_MainPGStreamReadWrite.tellp();
	m_MainPGStreamReadWrite.write((char*)&pg.Var.v[0], pg.Var.v.size() * 4);
	newpos = m_MainPGStreamReadWrite.tellp();
	return newpos;

}

bool PG_Stream::WritePGFileFrame(size_t InputRepNo, fstream *pPGOutStream)
{
	if (m_OnlyNewFile) {
		if (pPGOutStream == nullptr) pPGOutStream = &m_MainPGStreamReadWrite;
		MakeFileToEnableWriting();
	}
	else {
		pPGOutStream = &m_ExtraPGStreamWrite;
	
	}
	size_t nrecords, OutSizeData;

	if (InputRepNo != string::npos) {
		ReadNewBuffer(InputRepNo);
	}
	else {
		InputRepNo = pg.Shape.NumRecords * pg.Shape.RecordSize;


	}


	switch (m_DataInFocus) {
	case 0:
		if (pg.status.a.OutRecordEnd>0 && pg.status.a.OutRecordStart<pg.Shape.NumRecords) {
			nrecords = pg.status.a.OutRecordEnd - pg.status.a.OutRecordStart + 1;
			OutSizeData = nrecords*pg.Shape.RecordSize;
		}
		else {
			if (pg.Shape.RepetitionMode&&pg.Shape.NumRepetitions>0) {
				//	if(InputRepNo<0)
				nrecords = pg.Shape.TotalNumberOfRecords / pg.Shape.NumRepetitions;
			}
			else
				nrecords = pg.Shape.NumRecords;
			pg.status.a.OutRecordStart = 1;
			OutSizeData = nrecords*pg.Shape.RecordSize;
		}
		break;
	case 1:
		nrecords = m_numrecords_Years;
		break;
	case 2:
		nrecords = m_numrecords_Months;
		break;
	case 3:
		nrecords = m_numrecords_Days;
		break;
	case 4:
		nrecords = m_numrecords_Hours;
		break;
	case 5:
		nrecords = m_numrecords_Minutes;
		break;

	}
	bool valid = true;

	pPGOutStream->seekp(pg.Shape.RecordSize, ios::beg);


	valid = WritePGFileStartHeader(nrecords, pg.Shape.NumRepetitions, InputRepNo, pPGOutStream);
	if (valid) valid = WritePGFileData(nrecords, pPGOutStream);



	//for (size_t nrep = 2; nrep <= pg.Shape.NumRepetitions; nrep++) {
	//	ReadNewBuffer(nrep);
	//	valid = WritePGFileData(nrecords, pPGOutStream);
	//}





	/*

	if (InputRepNo == string::npos) {
		if (nrecords == pg.Shape.TotalNumberOfRecords)
			pg.Shape.RecordTotalSizeData = nrecords*pg.Shape.RecordSize;
		else
			pg.Shape.RecordTotalSizeData = nrecords*pg.Shape.RecordSize*pg.Shape.NumRepetitions;
	}
	else
		pg.Shape.RecordTotalSizeData = nrecords*pg.Shape.RecordSize;

	m_OutFileRecord = nrecords;
	m_OutFileVar = pg.Shape.NumVar;
	m_ResetBuffer = true;
	m_OutFileStart = GetDateFromVarRecord(pg.status.a.OutRecordStart);
	m_ResetBuffer = true;
	m_OutFileEnd = GetDateFromVarRecord(pg.status.a.OutRecordStart + nrecords - 1);


	std::streamsize newpos = pg.Shape.RecordSize*pg.status.a.OutRecordStart;
	std::streamsize newposInput;
	if (InputRepNo != string::npos)
		newposInput = pg.Shape.RecordSize*pg.status.a.OutRecordStart + pg.Shape.RecordTotalSizeData*InputRepNo;
	else
		newposInput = pg.Shape.RecordSize*pg.status.a.OutRecordStart;

	if (!m_OnlyNewFile) m_MainPGStreamReadWrite.seekg(newposInput, ios::beg);

	m_ExtraPGStreamWrite.seekp(newpos, ios::beg);


#ifndef COUPSTD
	CFileScanner ScanMeanProg(nullptr);
	ScanMeanProg.Create(IDD_PROGRESS, nullptr);
	ScanMeanProg.ShowWindow(SW_SHOW);
	string str = "Saving data to file : ";
	ScanMeanProg.SetWindowText(CString(str.c_str()));
#endif
	if (InputRepNo != string::npos && !m_OnlyNewFile) {
		for (size_t nrep = 0; nrep <= pg.Shape.NumRepetitions; nrep++) {
			if (!SetActBuffer(nrep)) return false;
			if (pg.status.b.NumberOfBuf>1) {
				for (size_t nbuf = 1; nbuf <= pg.status.b.NumberOfBuf; nbuf++) {
					if (pg.status.b.NumberOfBuf>1)
						if (!SetActBuffer(nbuf)) return false;
#ifndef COUPSTD
					ScanMeanProg.Update(nbuf * 100 / pg.status.b.NumberOfBuf);
#endif
					if (pg.status.b.NumberOfBuf>1)
						if (!WriteNewDriveNewBuffer(nbuf)) return false;
				}
			}
			else {
#ifndef COUPSTD
				ScanMeanProg.Update(nrep * 100 / pg.Shape.NumRepetitions);
#endif
				if (!WriteNewDriveNewBuffer(nrep)) return false;
			}

		}
	}
	else if (pg.Var.v.size() >= nrecords*pg.Shape.RecordIndexSize) {

		m_ExtraPGStreamWrite.write((char*)&pg.Var.v[0], pg.Shape.RecordSize*nrecords);

	}
	else if (pg.Var.v.size() == pg.Shape.RecordIndexSize) {
		for (size_t i = 0; i<nrecords; i++) {
			m_MainPGStreamReadWrite.read((char*)&pg.Var.v[0], pg.Shape.RecordSize);
			m_ExtraPGStreamWrite.write((char*)&pg.Var.v[0], pg.Shape.RecordSize);
#ifndef COUPSTD
			if (i % 1000 == 0) {
				ScanMeanProg.Update(i * 100 / nrecords);
			}
#endif
		}
	}
	*/
	if (valid) valid = WriteDescriptionsToStream(pPGOutStream);
	CloseFile();
	return valid;

}
bool PG_Stream::GetAllFileInfo(string filename, bool ShowError)
{
	pg.status.AllInMemory = false;
	m_ShowError = ShowError;
	fstream	driveFile= GetLocalReadOnlyStream(filename);
	pg.status.FileName = filename;
	bool valid=ReadPGStartHeaderFromStream(&driveFile);
	if (valid) valid=ReadVariableValuesFromStream(&driveFile);
	if (valid) valid= ReadDescriptionsFromStream(&driveFile);
	pg.status.AllInMemory = true;
	return true;
}
bool PG_Stream::ReadContentOfPGFileAndOpenMainPGStreamForReading(string filename)
{
#ifndef COUPSTD	
	CWinApp* pApp = AfxGetApp();
#endif
	string kalle = pg.status.FileName;
	pg.status.FileName = filename;

	CloseFile();
	bool valid;
	valid=OpenMainPGStreamAsReadOnly(filename);
	//size_t recsize, totsizedata;

	if(valid) valid=ReadPGStartHeaderFromStream(&m_MainPGStreamReadWrite);

	if(valid) valid=ReadVariableValuesFromStream(&m_MainPGStreamReadWrite);

	if (valid) valid = ReadDescriptionsFromStream(&m_MainPGStreamReadWrite);

	return valid;
}
bool PG_Stream::WritePGFileEnd(size_t nrecords, size_t nrepititions, size_t InputRepNo, bool newdrive)
{

	std::streamsize newpos, actpos;

	if (InputRepNo != string::npos || nrepititions>1)
		newpos = nrepititions*pg.Shape.RecordSize*nrecords + pg.Shape.RecordSize;
	else
		newpos = pg.Shape.RecordSize*nrecords + pg.Shape.RecordSize;

	if (newdrive) {
		m_ExtraPGStreamWrite.seekp(newpos, ios::beg);
		actpos = m_ExtraPGStreamWrite.tellp();
	}
	else {
		m_MainPGStreamReadWrite.seekp(newpos, ios::beg);
		actpos = m_MainPGStreamReadWrite.tellp();

	}

	if (newdrive)
		return WriteDescriptionsToStream(&m_ExtraPGStreamWrite);
	else
		return WriteDescriptionsToStream(&m_MainPGStreamReadWrite);





}
string PG_Stream::GetDateFromVarRecord(size_t Record)
{
	if (pg.Var.v.size() <= (Record - 1)*pg.Shape.RecordIndexSize) {
		Record = pg.Shape.NumRecords;
		if (pg.Var.v.size() <= (Record - 1)*pg.Shape.RecordIndexSize) return "";

	}
	auto min = *(unsigned int*)&pg.Var.v[(Record - 1)*pg.Shape.RecordIndexSize];
	return PGUtil::STD_StringDatum(min); 
}
bool PG_Stream::SetMinToVarRecord(size_t Record, size_t Min)
{
	if (pg.Var.v.size() > (Record - 1)*pg.Shape.RecordIndexSize&&Record>0) {
		pg.Var.v[(Record - 1)*pg.Shape.RecordIndexSize] = *(float*)&Min;
		return true;
	}
	else
		return false;
	
}
bool PG_Stream::ReadFirst_Buffer() {
	std::streamsize newpos;
	newpos = pg.Shape.RecordSize;
	m_MainPGStreamReadWrite.seekg(newpos, ios::beg);
	m_MainPGStreamReadWrite.read((char*)&pg.Var.v[0], pg.Shape.RecordSize*pg.status.b.buf_Size);
	if (m_MainPGStreamReadWrite.good()) {
		pg.status.b.PrevBuf = pg.status.b.CurrentBuf=1;
		pg.status.b.CurrentBufferIsRead = true;
		return true;
	}
	else {
		return false;
	}
}
bool PG_Stream::CheckIfCurrentRecordFitsCurrentBuffer(size_t Record) {
	size_t Buf;
	Buf = (Record - 1) / pg.status.b.buf_Size + 1;
	if (Buf == pg.status.b.CurrentBuf) return true;
	return false;
}
bool PG_Stream::ReadNewBuffer(size_t BufferNum) {
	std::streamsize newpos;
	if (pg.Var.v.size() * 4 < pg.Shape.RecordSize * pg.status.b.buf_Size) {
		if (BufferNum != 0) {
			return false;
		}
	}
	 
	if (BufferNum == 0)
		newpos = pg.Shape.RecordSize;
	else if (BufferNum <= pg.status.b.NumberOfBuf)
		newpos = pg.Shape.RecordSize*(BufferNum - 1)*pg.status.b.buf_Size + pg.Shape.RecordSize;
	else if (BufferNum <= pg.Shape.NumRepetitions)
		newpos = pg.Shape.RecordSize*(BufferNum - 1)*pg.status.b.buf_Size + pg.Shape.RecordSize;
	else
		return false;
	try { m_MainPGStreamReadWrite.seekg(newpos, ios::beg);
		if (!m_MainPGStreamReadWrite.good()) return false;
	}
	catch (const std::exception& e) {
		auto koll=e.what();

	}
	try {
		m_MainPGStreamReadWrite.read((char*)&pg.Var.v[0], pg.Shape.RecordSize * pg.status.b.buf_Size);
		if (m_MainPGStreamReadWrite.good()) {
			pg.status.b.PrevBuf = pg.status.b.CurrentBuf;
			pg.status.b.CurrentBuf = BufferNum;
			pg.status.b.CurrentBufferIsRead = true;
			return true;
		}
		else
			return false;
	}
	catch (const std::exception& e) {
		auto koll=e.what();
		return false;
	}
}
bool PG_Stream::ReadNewBuffer(size_t CurrentRec, size_t CurrentBuf)
{
	//size_t CurrentRecRead;

	if (CurrentBuf<pg.status.b.NumberOfBuf)
		pg.status.b.buf_Size = pg.status.b.buf_Size;
	else
		pg.status.b.buf_Size = (pg.Shape.NumRecords - 1) % pg.status.b.buf_Size + 1;

	std::streamsize newpos;
	if (CurrentBuf == 0)
		newpos = pg.Shape.RecordSize;
	else
		newpos = pg.Shape.RecordSize*(CurrentBuf - 1)*pg.status.b.buf_Size + pg.Shape.RecordSize;

	m_MainPGStreamReadWrite.seekg(newpos, ios::beg);
	m_MainPGStreamReadWrite.read((char*)&pg.Var.v[0], pg.Shape.RecordSize*pg.status.b.buf_Size);
	if (m_MainPGStreamReadWrite.good()) {
		pg.status.b.PrevBuf = CurrentBuf;
		m_ResetBuffer = false;
		return true;
	}
	else {
		return false;
	}
}
bool PG_Stream::WriteNewBuffer(size_t CurrentBuf)
{
	std::streamsize newpos, actpos;
	if (CurrentBuf<pg.status.b.NumberOfBuf)
		pg.status.b.buf_Size = pg.status.b.buf_Size;
	else
		pg.status.b.buf_Size = (pg.Shape.NumRecords - 1) % pg.status.b.buf_Size + 1;
	if (pg.Shape.RepetitionMode)
		newpos = pg.Shape.RecordSize*(CurrentBuf - 1)*pg.status.b.buf_Size + pg.Shape.RecordSize;
	else if (CurrentBuf>0)
		newpos = pg.Shape.RecordSize*(CurrentBuf - 1)*pg.status.b.buf_Size + pg.Shape.RecordSize;
	else
		newpos = pg.Shape.RecordSize;
	m_MainPGStreamReadWrite.seekp(newpos, ios::beg);
	actpos = m_MainPGStreamReadWrite.tellp();
	if (actpos == newpos) {
		m_MainPGStreamReadWrite.write((char*)&pg.Var.v[0], pg.Shape.RecordSize*pg.status.b.buf_Size);
		pg.status.FileModified = true;
	}
	if (m_MainPGStreamReadWrite.good()) pg.status.b.PrevBuf = CurrentBuf;
	return m_MainPGStreamReadWrite.good();

}
bool PG_Stream::WriteNewDriveNewBuffer(size_t CurrentBuf)
{
	std::streamsize newpos, actpos;
	if (CurrentBuf<pg.status.b.NumberOfBuf)
		pg.status.b.buf_Size = pg.status.b.buf_Size;
	else
		pg.status.b.buf_Size = (pg.Shape.NumRecords - 1) % pg.status.b.buf_Size + 1;
	if (CurrentBuf>0)
		newpos = pg.Shape.RecordSize*(CurrentBuf - 1)*pg.status.b.buf_Size + pg.Shape.RecordSize;
	else
		newpos = pg.Shape.RecordSize;
	m_ExtraPGStreamWrite.seekp(newpos, ios::beg);
	actpos = m_ExtraPGStreamWrite.tellp();
	if (actpos == newpos)
		m_ExtraPGStreamWrite.write((char*)&pg.Var.v[0], pg.Shape.RecordSize*pg.status.b.buf_Size);
	if (m_ExtraPGStreamWrite.good()) pg.status.b.PrevBuf = CurrentBuf;
	return m_ExtraPGStreamWrite.good();
}

bool PG_Stream::WriteSeparateSplitFiles()
{
	bool good = true;
	string filename, bas;
	size_t pos = pg.status.FileName.find(".bin");
	bas = pg.status.FileName.substr(0, pos);
	ReOpen();

	for (size_t irep = 0; irep<pg.Shape.NumRepetitions; irep++) {
		filename = bas; filename += "_"; filename += FUtil::STD_ItoAscii(irep + 1); filename += ".bin";
		m_ExtraPGStreamWrite.clear();
		m_ExtraPGStreamWrite.open(filename, ios::out | ios::binary);
		if (!m_ExtraPGStreamWrite.good())
			good = false;

		if (!WritePGFileFrame(irep))
			good = false;
		m_ExtraPGStreamWrite.close();
		if (!good) return good;
	}
	CloseFile();
	return true;

}

bool PG_Stream::WriteMinMax(size_t varno, float fmin, float fmax, size_t repit)
{
#ifndef COUPSTD
	CFileStatus rstatus;
#endif
	std::streamsize newpos = 0;
	size_t extra;

	if (m_MainPGStreamReadWrite.good()) {
		extra = 60 * varno + 52 + pg.Shape.NumVar*(repit - 1);
		newpos = (pg.Shape.NumRecords + 1)*pg.Shape.RecordSize + extra;
		m_MainPGStreamReadWrite.seekg(newpos, ios::beg);
		m_MainPGStreamReadWrite.write((char*)&fmin, 4);
		m_MainPGStreamReadWrite.write((char*)&fmax, 4);
		if (m_MainPGStreamReadWrite.good())
			return true;
	}
	else
		return false;
	return false;
}
bool PG_Stream::WritePGFileHeader(size_t nrecords, size_t nrepititions)
{
	if (!WritePGFileStartHeader(nrecords, nrepititions))
		return false;
	if (!WritePGFileEnd(nrecords, nrepititions))
		return false;

	return true;
}
bool PG_Stream::WritePGFileData(size_t nrecords, fstream *pPGOutStream)
{
	size_t  OutSizeData;
	if (pPGOutStream == nullptr) {
		if(m_UseMainStream) pPGOutStream = &m_MainPGStreamReadWrite;
		else  pPGOutStream = &m_ExtraPGStreamWrite;
	}

	
	pg.Shape.RecordTotalSizeData = nrecords*pg.Shape.RecordSize;
	m_OutFileRecord = nrecords;
	m_OutFileVar = pg.Shape.NumVar;


	if (pg.Shape.RepetitionMode&& pg.Shape.NumRepetitions>0&& pg.Shape.NumRepetitions!=string::npos) {
		//nrecords = nrecords*pg.Shape.NumRepetitions;
		OutSizeData = nrecords*pg.Shape.RecordSize;

		pg.status.a.OutRecordStart = 1;
	}
	else if (pg.status.a.OutRecordEnd>0 && pg.status.a.OutRecordStart<pg.Shape.NumRecords) {
		nrecords = pg.status.a.OutRecordEnd - pg.status.a.OutRecordStart + 1;
		OutSizeData = nrecords*pg.Shape.RecordSize;
	}
	else {
		nrecords = pg.Shape.NumRecords;
		pg.status.a.OutRecordStart = 1;
		OutSizeData = nrecords*pg.Shape.RecordSize;
	}

	auto pos=pPGOutStream->tellp();


	// Saving only first Repetition in this case.
	if (pg.Shape.NumRepetitions == 1) {
		pPGOutStream->write((char*)&pg.Var.v[(pg.status.a.OutRecordStart - 1) * pg.Shape.RecordIndexSize], OutSizeData);
		auto min = *(unsigned int*)&pg.Var.v[(pg.status.a.OutRecordStart - 1) * pg.Shape.RecordIndexSize];
		m_OutFileStart = GetDateFromVarRecord(pg.status.a.OutRecordStart);
		m_OutFileEnd = GetDateFromVarRecord(pg.status.a.OutRecordStart + nrecords - 1);
	}
	else {
		for (size_t nrep = 1; nrep <= pg.Shape.NumRepetitions; nrep++) {
			if (ReadNewBuffer(nrep)) {
				pPGOutStream->write((char*)&pg.Var.v[(pg.status.a.OutRecordStart - 1) * pg.Shape.RecordIndexSize], OutSizeData);
				auto min = *(unsigned int*)&pg.Var.v[(pg.status.a.OutRecordStart - 1) * pg.Shape.RecordIndexSize];
				m_OutFileStart = GetDateFromVarRecord(pg.status.a.OutRecordStart);
				m_OutFileEnd = GetDateFromVarRecord(pg.status.a.OutRecordStart + nrecords - 1);
			}
			else
				return false;
		}
		ReadNewBuffer(0);
	}
	return pPGOutStream->good();
}
bool PG_Stream::WritePGFile()
{
#ifndef COUPSTD
	MFC_Util m;
#endif
	if (!MakeFileToEnableWriting()) {
#ifndef COUPSTD	
		m.SetPaneMessage("Could not Open file for writing -Error No Saving can be made");
#endif
		return false;
	}
	m_UseMainStream = true;
	size_t nrecords, OutSizeData;
	nrecords = 0;
	switch (m_DataInFocus) {
	case 0:
		if (pg.status.a.OutRecordEnd>0 && pg.status.a.OutRecordStart<pg.Shape.NumRecords) {
			nrecords = pg.status.a.OutRecordEnd - pg.status.a.OutRecordStart + 1;
			OutSizeData = nrecords*pg.Shape.RecordSize;
		}
		else {
			nrecords = pg.Shape.NumRecords;
			pg.status.a.OutRecordStart = 1;
			OutSizeData = nrecords*pg.Shape.RecordSize;
		}
		break;
	case 1:
		pg.status.a.OutRecordStart = 1;
		nrecords = m_numrecords_Years;
		break;
	case 2:
		pg.status.a.OutRecordStart = 1;
		nrecords = m_numrecords_Months;
		break;
	case 3:
		pg.status.a.OutRecordStart = 1;
		nrecords = m_numrecords_Days;
		break;
	case 4:
		pg.status.a.OutRecordStart = 1;
		nrecords = m_numrecords_Hours;
		break;
	case 5:
		pg.status.a.OutRecordStart = 1;
		nrecords = m_numrecords_Minutes;
		break;

	}


	m_MainPGStreamReadWrite.seekp(0, ios::beg);
	if (!m_MainPGStreamReadWrite.good()) return false;

	if (!WritePGFileStartHeader(nrecords, pg.Shape.NumRepetitions)) {
#ifndef COUPSTD	
		m.SetPaneMessage("Error when writing header of PG file");
#endif
		return false;
	}

	m_MainPGStreamReadWrite.seekp(pg.Shape.RecordSize, ios::beg);

	if (!WritePGFileData(nrecords)) {
#ifndef COUPSTD	
		m.SetPaneMessage("Error when writing data to PG file");
#endif
		return false;
	}

	if (!WritePGFileEnd(nrecords, pg.Shape.NumRepetitions)) {
#ifndef COUPSTD	
		m.SetPaneMessage("Error when writing description to PG file");
#endif
		return false;
	}
	SetNewReadofFile();
	CloseFile();
	return ReOpen();

}
bool PG_Stream::WriteNewPGFileAs(string newfilename, bool newfile)
{
	auto newnumvar = pg.Shape.NumVar;
	m_UseMainStream = false;

	m_ExtraPGStreamWrite.open(newfilename, ios::out | ios::in | ios::binary);
	if (!m_ExtraPGStreamWrite.is_open()) {
		m_ExtraPGStreamWrite.open(newfilename, ios::out | ios::binary);
		if (!m_ExtraPGStreamWrite.is_open()) return false;
	}
	m_MainPGStreamReadWrite.close();
	if (!m_MainPGStreamReadWrite.is_open()) {
		m_MainPGStreamReadWrite.open(pg.status.FileName, ios::in | ios::binary);

		m_MainPGStreamReadWrite.seekg(0, ios::end);
		auto end = m_MainPGStreamReadWrite.tellg();

		if (!m_MainPGStreamReadWrite.good()) {
			m_MainPGStreamReadWrite.clear();
			m_MainPGStreamReadWrite.close();
			if (!m_MainPGStreamReadWrite.good()) return false;
		}
		m_OnlyNewFile = false;
	}
	else {
		if (pg.Shape.NumRepetitions <= 1) {
			m_OnlyNewFile = true;
		}
		else
			m_OnlyNewFile = true;
	}

	if (WritePGFileFrame(0, &m_ExtraPGStreamWrite))
	{
		m_ExtraPGStreamWrite.close();
		CloseFile();
		m_IsOpen = false;
#ifndef COUPSTD
		MFC_Util m;
		if (remove("Dump.bin")) {
			m.SetPaneMessage("New File Created and  extra copy removed");
		}
		else {
			m.SetPaneMessage("New File Created - failed to remove copy ");
		}
#endif
		return true;
	}
	else
		return false;
}

void PG_Stream::SetRepititionMode(bool value, size_t repititions)
{

	if (pg.Shape.RepetitionMode&&value&&pg.Shape.NumRepetitions>1) return; // Set only repititionsmode once
	pg.Shape.RepetitionMode = value;
	if (pg.Shape.RepetitionMode) {
		pg.Shape.NumRepetitions = repititions;
		pg.Shape.TotalNumberOfRecords = pg.Shape.NumRecords;
		pg.Shape.NumRecords = pg.Shape.TotalNumberOfRecords / repititions;
		pg.Shape.RecordSize_Repetion = pg.Shape.NumRecords*pg.Shape.RecordIndexSize;
		m_ActualSequence = 1;
		pg.status.a.LocalStartRecord = 0;
		pg.status.a.LocalStartIndex = 0;
		SetActBufferSize(pg.Shape.NumRecords);
	}
	else {
		if (pg.Shape.NumRepetitions>1) {
			pg.Shape.NumRecords = pg.Shape.TotalNumberOfRecords;
			pg.Shape.NumRepetitions = 1;
		}


		pg.status.a.LocalStartRecord = 0;
		pg.status.a.LocalStartIndex = 0;
	}
}
bool PG_Stream::SetRepititionSequence(size_t sequence, bool ResetIndex)
{
	sequence = sequence%pg.Shape.NumRepetitions;
	if (sequence == 0) sequence = pg.Shape.NumRepetitions;
	m_ActualSequence = sequence;
	if (ResetIndex) {
		pg.status.a.LocalStartRecord = 0;
		pg.status.a.LocalStartIndex = 0;
	}
	else {
		pg.status.a.LocalStartRecord = (m_ActualSequence - 1)*pg.Shape.NumRecords;
		pg.status.a.LocalStartIndex = (m_ActualSequence - 1)*pg.Shape.RecordSize_Repetion;
	}

	if (!pg.status.CompleteFileRead) {
		if (pg.status.b.NumberOfBuf>1)
			pg.status.b.buf_Size = pg.status.b.buf_Size;
		else
			pg.status.b.buf_Size = (pg.Shape.NumRecords - 1) % pg.status.b.buf_Size + 1;



		std::streamsize newpos;
		newpos = pg.Shape.RecordSize*(sequence - 1)*pg.status.b.buf_Size + pg.Shape.RecordSize;
		if (!m_MainPGStreamReadWrite.good()) return false;
		m_MainPGStreamReadWrite.seekg(newpos, ios::beg);


		if (!m_MainPGStreamReadWrite.good()) return false;
		auto gpos = m_MainPGStreamReadWrite.tellg();
		m_MainPGStreamReadWrite.read((char*)&pg.Var.v[0], pg.Shape.RecordSize*pg.status.b.buf_Size);
		gpos = m_MainPGStreamReadWrite.gcount();
		pg.status.b.PrevBuf = sequence;
		if (!m_MainPGStreamReadWrite.good()) return false;
	}

	if (pg.status.a.LocalStartRecord <= pg.Shape.TotalNumberOfRecords - pg.Shape.NumRecords)
		return true;
	else
		return false;
}
size_t PG_Stream::GetRepititionSequence()
{
	return m_ActualSequence;
}
size_t PG_Stream::CheckAndEstimateRepetions() {
	unsigned int itime, iprevtime;
	m_MainPGStreamReadWrite.seekg(8, ios::beg);
	size_t count = 0;
	iprevtime = 2000000000;
	for (size_t i = 0; i<pg.Shape.NumRecords; i++) {
		m_MainPGStreamReadWrite.seekg(8 + i * 8, ios::beg);
		m_MainPGStreamReadWrite.read((char*)&itime, 4);
		if (itime<iprevtime) {
			count++;
			if (count == 2) {
				if (i>0) {
					size_t estimate = pg.Shape.NumRecords / i;
					return estimate;
				}
			}
		}
		iprevtime = itime;
	}
	return count;



}

bool PG_Stream::OpenMainPGStreamAsReadOnly(string filename)
{

	if (filename == "") return false;
	m_MainPGStreamReadWrite.open(filename, ios::in | ios::binary);
	if (!m_MainPGStreamReadWrite.is_open()) {
		string UserDirectory = "";
		UserDirectory = FUtil::GetProfileStringStd("UserDirectory", UserDirectory);
		size_t numo;
		numo = UserDirectory.size();
		if (UserDirectory.rfind("\\") != numo - 1) {
			UserDirectory += "\\";
			numo++;
		}
		size_t n = filename.rfind("\\");
		filename = filename.substr(n + 1);

		pg.status.FileName = UserDirectory + filename;
		m_MainPGStreamReadWrite.open(pg.status.FileName, ios::in | ios::binary);
		if (!m_MainPGStreamReadWrite.is_open()) {
			string UserDirectory = "";
			UserDirectory = FUtil::GetProfileStringStd("UserSubDirectory", UserDirectory);
			string testname;
			size_t ust = filename.rfind("\\");
			string file = filename.substr(ust + 1);
			m_MainPGStreamReadWrite.open(testname, ios::in | ios::binary);
			if (!m_MainPGStreamReadWrite.is_open()) {
#ifndef COUPSTD
				TRACE("Can't Open file %s´\n", pg.status.FileName);
				m_FailedFileName = MFC_Util::GetProfileStringStd("Failed_Name", m_FailedFileName);
				//Display error message
				if (m_FailedFileName != pg.status.FileName) {
					m_FailedFileName = pg.status.FileName;
					FUtil::WriteProfileStringStd("Failed_Name", pg.status.FileName);
					string str;
					str = "The PG-file: "; str += pg.status.FileName; str += " couldn´t be opened.";

					CMainFrame *p_mainframe;
					p_mainframe = (CMainFrame*)AfxGetMainWnd();

					p_mainframe->UpdatePaneText(0, str);

					//MessageBox(nullptr,str.c_str(),"File open error",MB_OK|MB_ICONEXCLAMATION);
				}
				return false;
#endif
				// Actions without file name ?
			}
			pg.status.FileName = testname;
		}
		filename = pg.status.FileName;
	}

	pg.status.FileName = filename;
	unsigned int ivar, nrep;
	fpos_t  endpos, test_old;
    std::streamsize newpos = 0;

	m_MainPGStreamReadWrite.seekg(newpos, ios::end);
	pg.status.SizeOfFileInMainStream = m_MainPGStreamReadWrite.tellg();

	pg.Shape.RepetitionMode = false;
	m_MainPGStreamReadWrite.seekg(newpos, ios::beg);
	m_MainPGStreamReadWrite.read((char*)&pg.Shape.NumRecords, 4);
	size_t recsize;
	m_MainPGStreamReadWrite.read((char*)&recsize, 4);
	auto sizeOfDat = pg.Shape.NumRecords*recsize;
	if (sizeOfDat > pg.status.SizeOfFileInMainStream) {

		int fileiswrong = 1;

	}
	m_MainPGStreamReadWrite.read((char*)&nrep, 4);
	if (nrep > 1&&nrep<30000) {
		pg.Shape.NumRepetitions = nrep;
		pg.Shape.NumRecords /= nrep;
		pg.Shape.RepetitionMode = true;
		pg.Shape.RecordTotalSizeData = nrep * pg.Shape.NumRecords;
	}
	if (pg.Shape.NumRecords == 0) {
		CloseFile();
		return false;
	}

	return true;
}

bool PG_Stream::Check_PGFileSize(fstream *f, size_t numrec, size_t ivar, size_t nrep)
{
	std::streamsize expected_size;
	expected_size = numrec*(ivar + 1)*nrep * 4;
	f->seekg(0, ios::end);
	std::streamsize end = f->tellg();
	if (expected_size>end) return false;
	return true;
}

