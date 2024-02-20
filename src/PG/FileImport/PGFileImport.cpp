#include "../../Util/FUtil.hpp"
#include "PGFileImport.h"
#include "../NewBase/ModelMap.h"

#ifndef COUPSTD

#include "../../NewBase/FileScanner.h"
#endif
#ifndef BASE
#define BASE   1721424L  /* Jan 1, AD 1 */
#endif
using namespace std;


PGFileImport::PGFileImport(size_t NumFiles)
{
	m_nprevrec=0;
	m_linebreak="\r\n";
	m_countf=0;
	m_newstartpos=0;
	m_ShowMessage=false;
	m_SwissCoord=false;
	m_CoordMapExist=false;
	m_NumToImport=NumFiles;
	m_datestring="";
	if(NumFiles<=1) 
		m_CollectedFile=false;
	else
		m_CollectedFile=true;
	m_ScanningMade = false;
	m_minst_a = PGUtil::MinutConv("200001011200");; m_minend_a = 1440;
	m_itimbase = m_minst_a;
	m_ExtraFileScanning = false;
	AddInfoLinkIndex.resize(5);
	m_FilterTestColumn = string::npos;
	m_FilterToUseId = "";

}
PGFileImport::~PGFileImport(void)
{
}
bool PGFileImport::ImportFromFile(string filename, bool AddFromFile, size_t iFormat) 
{
		string str;

	if(iFormat==4) iFormat=7;
	if(iFormat==5) iFormat=9;
	if(iFormat==6) iFormat=10;  // Month col trans MONTH_COL_TRANS=10
	if (iFormat == 7) iFormat = 11;
	m_format=formats(iFormat);
//	m_format=formats(9); // quick fix
	m_AddFromFile=AddFromFile;
	m_filename=filename;
	m_FileText.open(filename,ios::in|ios::binary);

	std::streamsize nbyte=1024;

	size_t nrec=0;
	size_t nvar=0;

	InPGFiles m_CurrentFile;

	string StatNo;
	
	FILEINFO Input;	

	Input=ScanningInputFile();


	
	m_minst_a=0; m_minend_a=1440, m_totrec_a;
	
	m_CreateMultiFile=false;

	Input=GetFileInfo(Input,0);
	m_time_red=0;
	if (Input.NumHeads == 0 ||m_format==STANDARD|| m_format == SEPARATED_YMDHM || m_format == YMD_HM || m_format == SWISS || m_format == CZECH_COL_TRANS || m_format == MONTH_COL_TRANS || m_format == PROFOUND)
		m_time_red = 0;
	else
		m_time_red=1;

	size_t choice;
#ifndef COUPSTD
	if(m_ShowMessage) {
		if(m_format==NOAA) 
			choice=MFC_Util::MessageBoxReturn(FUtil::STD_ItoAscii(m_ImportedFileVector.size())+" File with "+FUtil::STD_ItoAscii(Input.TotRec)+" and "+FUtil::STD_ItoAscii(Input.MaxVar)+" Variables \nContinue to read the file and convert to bin format ?","Txt file scanned",MB_YESNO|MB_ICONEXCLAMATION);
		else
			choice=MFC_Util::MessageBoxReturn("File with "+FUtil::STD_ItoAscii(Input.TotRec)+" and "+FUtil::STD_ItoAscii(Input.MaxVar)+" Variables \nContinue to read the file and convert to bin format ?","Txt file scanned",MB_YESNO|MB_ICONEXCLAMATION);
		if(choice!=IDYES)
			return false;
	}
	str="Reading content of txt file: ";
	str+=filename;

	
#endif
	std::streampos  actpos;	
	if(!m_CollectedFile) m_ImportedFileVector.clear();
	if(m_ImportedFileVector.size()==0||(m_CollectedFile&&m_ImportedFileVector.size()==0))
		m_ImportedFileVector.push_back(m_CurrentFile);

	size_t filestart=0;
	if(m_CollectedFile&&m_format!=PROFOUND) 
		filestart=m_ImportedFileVector.size()-1;



	for(m_countf=filestart; m_countf<m_ImportedFileVector.size(); m_countf++) {
		if(m_ImportedFileVector.size()>1) {
			nrec=m_totrec_a+1;
		}
		if(!CreateNewFileAndDefineSize(Input, m_countf)) return false;
		bool koll=m_MainPGStreamReadWrite.good();
		koll=m_MainPGStreamReadWrite.is_open();
		actpos=m_MainPGStreamReadWrite.tellp();
		m_itimbase=GetStartMinute();

		if(m_countf==0)
			m_nrec=0;

		koll=m_MainPGStreamReadWrite.good();
		InitForSettingMinMaxMissingValues();
		m_HeadingRead=false;
		if(!ReadHeading(Input, m_countf)){
			string text;
			text=m_filename;
            #ifndef COUPSTD
			choice=MFC_Util::MessageBoxReturn("Error when reading file:"+text,"File Read",MB_YESNO|MB_ICONEXCLAMATION);
            #endif
			return false;
		}
		koll=m_MainPGStreamReadWrite.good();
		ReadingVariableValues(Input, m_countf);

		if (m_format != PROFOUND) {
			m_min_next = 0;
			actpos = m_MainPGStreamReadWrite.tellp();

			m_nrec = size_t(actpos / pg.Shape.RecordSize) - 1;
			if (m_nrec < m_totrec_a*(m_countf + 1) - Input.NumHeads)
				WriteMissingRegularTime();

			actpos = m_MainPGStreamReadWrite.tellp();
			auto nrec_koll = actpos / pg.Shape.RecordSize - 1;

			if (m_format != CZECH_COL_TRANS) {
				for (size_t j = 1; j <= pg.Shape.NumVar; j++) {
					SetVarMin(j, m_vmin[j - 1], m_countf + 1);
					SetVarMax(j, m_vmax[j - 1], m_countf + 1);
				}

			}
		}
		if (m_format == PROFOUND && !Input.EqualSizeOfSiteRecords) {
			for (size_t j = 1; j <= pg.Shape.NumVar; j++) {
				SetVarMin(j, m_vmin[j - 1], 1);
				SetVarMax(j, m_vmax[j - 1], 1);
			}
			WritePGFileHeader(GetNumRecords(), pg.Shape.NumRepetitions);
			CloseFile();

		}

		else if(m_countf==m_ImportedFileVector.size()-1||m_format==CZECH_COL_TRANS) {
			bool Close=false;
			if(m_format==NOAA) {
				pg.Shape.NumRepetitions=m_ImportedFileVector.size();
				FindKeyValues();
				Close=true;
			}
			else if(m_format==CZECH_COL_TRANS) {
				Close=true;
				m_countf=m_NumToImport-1;
			}
			if(m_countf==m_NumToImport-1|| m_NumToImport==0) Close=true;
			if(Close) {
				WritePGFileHeader(GetNumRecords(), pg.Shape.NumRepetitions);
				CloseFile();
				m_Closed=true;

			}
			else {

				m_nprevrec=(m_countf+1)*m_totrec_a;
			}
		}
		m_MainPGStreamReadWrite.seekp(actpos, ios::beg);
		koll=m_MainPGStreamReadWrite.good();
	}
	m_FileText.close();
	if(pg.Shape.NumRepetitions>3&&GetNumRecords()>1000) {
		SetCompleteRead(false);
	}
	else
		SetCompleteRead(true);
	size_t po=pg.status.FileName.rfind('.');
	if(!m_ShowMessage) return true;
#ifndef COUPSTD
	choice=MFC_Util::MessageBoxReturn("File converted to bin format \nDo want to open the file ?","File Read",MB_YESNO|MB_ICONEXCLAMATION);
	if(choice!=IDYES)
		return false;
#endif
	return true;
}

FILEINFO PGFileImport::ScanningInputFile(string filename){


//    MFC_Util m;

	FILEINFO out;
	out.CopyMode = false;
	out.MaxVar=0;
	out.NumHeads=0;
	out.FixedFormat=true;
	out.TimeColumn = string::npos;
	out.HourColumn = string::npos;
	out.TotRec=0;
	out.RedNumVar=0;
	out.SkipBeforeVar = 0;
	out.SkipAfterVar = string::npos;



	string str;
	auto CheckStreamAndReOpen = [&]() {
		m_ImportedFileVector.clear();
		if (filename.size() > 0) {
			m_FileText.close();
			m_FileText.clear();
			m_FileText.open(filename, ios::in | ios::binary);
			if (!m_FileText.is_open()) {
				str = "Failed to open the file: ";
				str += filename;
				str += "  No scanning was made";
//                m.SetPaneMessage(str);
				return false;
			}
			if (filename.find("smhi-opendata") == 0) {
				out.IdentifiedFormat = SMHI_OPENDATA;


			}
		}
		else {
			m_FileText.close();
			m_FileText.clear();
			m_FileText.open(m_filename, ios::in | ios::binary);
			if (!m_FileText.is_open()) {
				str = "Failed to open the file: ";
				str += m_filename;
				str += "  No scanning was made";
//                m.SetPaneMessage(str);
				return false;
			}
			if (m_filename.find("smhi-opendata") != string::npos) {
				out.IdentifiedFormat = SMHI_OPENDATA;


			}
			out.InputfileName = m_filename;
		}
		return true;
	};
	if (!CheckStreamAndReOpen()) return out;
	out = CheckFixedNOOAFormat(out);
	out = MakeScanning(out);
	out = CheckHeadingAndFileInformation(out);
	
	str = "Completed Scanning of File - Total Number of Records";
	str += FUtil::STD_ItoAscii(out.TotRec);
	str+=" and "+ FUtil::STD_ItoAscii(out.FirstValidHeads.size())+ " Variables";
	
//    m.SetPaneMessage(str);
	return out;
}
FILEINFO PGFileImport::CheckFixedNOOAFormat(FILEINFO out)
{
	char buf[10000];
	string str;
	string StatNo;

	m_FileText.read((char*)&buf, 139);
	for (size_t i = 0; i<139; i++)
		str += buf[i];
	if (str.find("STN---") == 0) {
		StatNo.clear();
		m_FileText.read((char*)&buf, 139);
		for (size_t i = 0; i<6; i++)
			StatNo += buf[i + 1];
		out.IdentifiedFormat = NOAA;
		m_format = NOAA;
		m_CurrentFile.StatNo = StatNo;
		m_CurrentFile.RecStart = 2;
		m_CurrentFile.NumRec = 0;
	}
	else if (out.IdentifiedFormat == SMHI_OPENDATA) {
		m_format = out.IdentifiedFormat;



	}
	else if (str.find("[weather.") == 0) {
		m_format = SILO;
		out.IdentifiedFormat = SILO;


	}

	m_FileText.clear();
	m_FileText.seekg(0, ios::end);
	m_totsizeInputFile = m_FileText.tellg();

	m_linebreak = "\r\n";
	m_delimiters = ",;\t ";
	string datestring;

	if (m_format == NOAA) m_linebreak = "\n";


	return out;
}
FILEINFO PGFileImport::MakeScanning(FILEINFO out)
{


	std::streampos pos, pos2, pos1, nbyte;
	std::streamsize rrec = 32384;
	size_t nrec=0;
	size_t SMHI_HeadLineNo = 15;
	size_t SMHI_StatLineNo = 2;
	size_t SMHI_ParLineNo = 5;
	size_t SMHI_PeriodLineNo = 8;
	size_t SMHI_TidPosStart = 1;

	size_t nhead = 0;
	size_t col_info_rec = 0;
	size_t nvar;
	string varstr;
	bool Linebreaktest = false;
	bool UnitsHeaderFind = false;
	char buf[40000];
	pos2 = 0;
	size_t SiteRecCount = 0;;
	size_t SiteId_Index = string::npos;
	size_t CurrentSite;
	string string_buf;
	m_FilterCount.clear();
	m_FilterMap.clear();
	m_CheckedTimeMap.clear();

	map<size_t, size_t> TimeCheckMap, MultiTimeMap;
	size_t MultiTimeCount = 0;
	size_t PrevTime=0, PrevRecord=0;
	pair<string, size_t> NextLine;
	

	auto SetTimeMap = [&]() {
		if (m_CurrentMin < PrevTime) {
			MultiTimeMap.insert(pair <size_t, size_t>(m_CurrentMin, nrec));
			MultiTimeCount++;
		}
		else {
			auto it = TimeCheckMap.find(m_CurrentMin);
			if (it == TimeCheckMap.end()|| TimeCheckMap.size()==0) {
				if (PrevTime > m_CurrentMin) {
					int koll = 1;

				}
				if (nrec != PrevRecord+1) {
					int gap = 1;
				}
				TimeCheckMap.insert(pair <size_t, size_t>(m_CurrentMin, nrec));
				m_CheckedTimeMap.insert(pair <size_t, size_t>(nrec, m_CurrentMin));

			}
			else {
				MultiTimeMap.insert(pair <size_t, size_t>(m_CurrentMin, nrec));
				MultiTimeCount++;
			}
			PrevTime = m_CurrentMin;
			PrevRecord = nrec;
		}
	};
	auto ResetTimeMap = [&]() {
		size_t count = 0;
		struct STEPS {
			size_t Min;
			size_t Rec;
		};
		TIMECHECK b;
		b.CommonTimeStep = true;
		b.CommonRecStep = true;
		b.NumUniqueSteps = TimeCheckMap.size();
		STEPS a;
		vector<STEPS> steps;
		size_t lowlim, highlim;
		size_t initValue, initRecord;
		size_t InitValueStep = string::npos;
		size_t InitRecordStep = string::npos;
		for (auto it = TimeCheckMap.begin(); it != TimeCheckMap.end(); ++it) {
			if (count == 0) {
				initValue = (*it).first;
				initRecord = (*it).second;
				out.IdentifiedStartMin = initValue;
				b.ValueStart = initValue;
			}
			else {
				a.Min = (*it).first - initValue;
				a.Rec = (*it).second - initRecord;
				steps.push_back(a);
				initValue = (*it).first;
				initRecord = (*it).second;
				if (steps.size()== 1) {
					InitValueStep =  steps[0].Min;
					lowlim = steps[0].Min -steps[0].Min / 365;
					highlim = steps[0].Min + steps[0].Min / 365;
					InitRecordStep =  steps[0].Rec;
				}
				else if (steps.size() > 1) {
					if (a.Min <lowlim||a.Min>highlim) b.CommonTimeStep = false;
					if (a.Rec != InitRecordStep) b.CommonRecStep = false;
				}
				b.ValueEnd= (*it).first;
			}
			count++;
		}
		b.RecordStep=b.ValueStep = string::npos;	
		if (b.CommonTimeStep) b.ValueStep = InitValueStep;
		if (b.CommonRecStep) b.RecordStep = InitRecordStep;
		
		if (InitValueStep == string::npos)
			out.IdentifiedMinDiff = 1440;
		else
			out.IdentifiedMinDiff = InitValueStep;
		
		
		TimeCheckMap.clear();
		return b;
	};
	auto GetStringFromStream = [&]() {
		string localstr;
		pos1 = m_FileText.tellg();
		auto nextpos = pos1;
		nextpos += 32384;
		if (m_totsizeInputFile<nextpos)
			rrec = m_totsizeInputFile - pos1;

		m_FileText.read((char*)&buf, rrec);
		pos2 = m_FileText.tellg();
		nbyte = pos2 - pos1;
		if (nbyte>32384)
			nbyte = 32384;
		for (size_t i = 0; i<nbyte; i++) {
			if (buf[i]>0||(buf[i]<-70&&buf[i]>-90)) localstr += buf[i];
		}

		return localstr;
	};
	auto FindAndSetDelimitter = [&](string headerline) {
		m_ndelim = 0;
		size_t n = string::npos;
		size_t Icount = 0;
		auto IsHeaderlineValid = [&]() {
			size_t numdig = 0;

			for (size_t i = 0; i < headerline.size(); i++) {
				if (isdigit(headerline[i])) {
					numdig++;
				}
				else {
					if (isalpha(headerline[i])) return true;


				}
			}
			if (numdig == headerline.size()) return false;
			return false;
		};

		//if (!IsHeaderlineValid()) return string::npos;
		while (n == string::npos&&Icount<4) {
			string dd = m_delimiters.substr(Icount, 1);
			n = headerline.find(dd);
			Icount++;
		}
		if (n != string::npos)
			return Icount - 1;
		else
			return string::npos;

	};
	auto CheckSizeOfValidLine = [&](string str) {
		size_t ich = str.find(m_linebreak);
		if (ich == string::npos && !Linebreaktest) {
			if (m_linebreak == "\r\n")
				m_linebreak = "\n";
			else if (m_linebreak == "\n")
				m_linebreak = "\r\n";
			Linebreaktest = true;
			ich = str.find(m_linebreak);

		}

		return ich;
	};
	auto SetValidTimeIdVariables = [&]() {
		bool ClimSelect = true;
		bool FluxSelect = false;
		bool SoilSelect = false;
		bool DateTimeIdentified = false;
		m_ProFoundOption = 0;
		out.HourColumn = out.TimeColumn = out.MinuteColumn = out.DayColumn = out.DayNumberColumn = out.MonthColumn = out.YearColumn = out.DateColumn = string::npos;
		if (m_format == STANDARD) {
			out.DateColumn = 1;
			out.CopyMode = true;
			DateTimeIdentified = true;


		}
		else if (m_format == PERRENIAL) {
			out.CopyMode = false;
			DateTimeIdentified = true;
		}

		for (size_t i = 0; i < out.FirstValidHeads.size(); i++) {
			if (out.FirstValidHeads[i] == "date" || out.FirstValidHeads[i] == "DATE" || out.FirstValidHeads[i] == "Date" ||
				out.FirstValidHeads[i] == "Datum" || out.FirstValidHeads[i] == "Variable Names:") {
				out.ValidVariables.push_back(false);
				out.DateColumn = out.ValidVariables.size();
				if (out.FirstValidHeads[i] == "Variable Names:") out.CopyMode = true;
				DateTimeIdentified = true;
			}
			else if (out.FirstValidHeads[i] == "timestamp" || out.FirstValidHeads[i] == "TIMESTAMP"||out.FirstValidHeads[i] == "Representativt dygn") {
				out.ValidVariables.push_back(false);
				out.DateColumn = out.ValidVariables.size();
				DateTimeIdentified = true;
			}
			else if (out.FirstValidHeads[i] == "year" || out.FirstValidHeads[i] == "Year") {
				out.ValidVariables.push_back(false);
				out.YearColumn = out.ValidVariables.size();
				DateTimeIdentified = true;
			}
			else if (out.FirstValidHeads[i] == "month" || out.FirstValidHeads[i] == "Month") {
				out.ValidVariables.push_back(false);
				out.MonthColumn = out.ValidVariables.size();
				DateTimeIdentified = true;
			}
			else if (out.FirstValidHeads[i] == "dayno" || out.FirstValidHeads[i] == "Dayno") {
				out.ValidVariables.push_back(false);
				out.DayNumberColumn = out.ValidVariables.size();
				DateTimeIdentified = true;
			}
			else if (out.FirstValidHeads[i] == "day" || out.FirstValidHeads[i] == "Day") {
				out.ValidVariables.push_back(false);
				out.DayColumn = out.ValidVariables.size();
				DateTimeIdentified = true;
			}
			else if (out.FirstValidHeads[i] == "time" || out.FirstValidHeads[i] == "Time" || out.FirstValidHeads[i] == "TIME" || out.FirstValidHeads[i] == "Tid (UTC)") {
				out.ValidVariables.push_back(false);
				out.TimeColumn = out.ValidVariables.size();
				DateTimeIdentified = true;
			}
			else if (out.FirstValidHeads[i] == "hour" || out.FirstValidHeads[i] == "Hour") {
				out.ValidVariables.push_back(false);
				out.HourColumn = out.ValidVariables.size();
				DateTimeIdentified = true;
			}

			else if (out.FirstValidHeads[i] == "minute" || out.FirstValidHeads[i] == "Minute") {
				out.ValidVariables.push_back(false);
				out.MinuteColumn = out.ValidVariables.size();
				DateTimeIdentified = true;
			}
			else if (m_format == PERRENIAL) {
				if(i>0&&i<124) 
					out.ValidVariables.push_back(true);
				else
					out.ValidVariables.push_back(false);
			}
			else if(out.CopyMode)
				out.ValidVariables.push_back(true);
			else
				out.ValidVariables.push_back(false);
		}

		if (out.YearColumn = string::npos && out.MonthColumn != string::npos && out.DayColumn != string::npos) out.IdentifiedDateFormat = YMD;
		
		/*if (out.Des.base.Name.size() > 0) {
			size_t origsize = out.Des.base.Name.size();
			for (size_t j = 0; j <origsize ; j++) {

				for (size_t jj = 0; jj < out.FirstValidHeads.size(); jj++) {				
					if (out.Des.base.Name[j] == out.FirstValidHeads[jj]) {
						if (out.Des.base.Name.size() < jj + 1) out.Des.base.Name.resize(jj + 1);
						//out.Des.base.Name[j] = "";
						out.Des.base.Name[jj] = out.FirstValidHeads[jj];
						if (out.Des.base.Unit.size() < jj + 1) out.Des.base.Unit.resize(jj + 1);
						out.Des.base.Unit[jj] = out.Des.base.Unit[j];
						
					}
				 }
			}


		} */
		
		
		m_CurrentFileInfo = out;





		/*		else if (ClimSelect&&(out.FirstValidHeads[i].find("pF_m") == 0 || out.FirstValidHeads[i].find("wsF_m") == 0
		||out.FirstValidHeads[i].find("vpdF_h") == 0||out.FirstValidHeads[i].find("lwInF_W") == 0||out.FirstValidHeads[i].find("swInF_W") == 0||
		out.FirstValidHeads[i].find("taF_d") == 0)
		)
		{
		m_ProFoundOption = 1;
		out.ValidVariables.push_back(true);

		}
		else if (FluxSelect&&(out.FirstValidHeads[i].find("neeVutRef_") == 0 || out.FirstValidHeads[i].find("recoNtVutRef_") == 0
		|| out.FirstValidHeads[i].find("gppNtVutRef_") == 0 || out.FirstValidHeads[i].find("leCORR_") == 0 || out.FirstValidHeads[i].find("hCORR_") == 0 )	)
		{
		out.ValidVariables.push_back(true);
		m_ProFoundOption = 2;

		}
		else if (SoilSelect && (out.FirstValidHeads[i].find("tsFMDS") == 0 || out.FirstValidHeads[i].find("swcFMDS") == 0))
		{
		out.ValidVariables.push_back(true);
		m_ProFoundOption = 3;

		}
		/*		else if (out.FirstValidHeads[i].find("site")==0|| out.FirstValidHeads[i].find("record")==0 || out.FirstValidHeads[i] == "timestepStart" || out.FirstValidHeads[i] == "timestepEnd"
		|| out.FirstValidHeads[i] == "year" || out.FirstValidHeads[i] == "day" || out.FirstValidHeads[i] == "mo" || out.FirstValidHeads[i] == "mo"
		|| out.FirstValidHeads[i].find("tmin")==0 || out.FirstValidHeads[i].find("tmax")==0 || out.FirstValidHeads[i].find("airpress") == 0 || out.FirstValidHeads[i].find("timestamp") == 0) {
		out.ValidVariables.push_back(false);
		}*/

		out.RedNumVar = 0;
		for (size_t i = 0; i < out.ValidVariables.size(); i++) 	if (!out.ValidVariables[i]) out.RedNumVar++;
		
		return DateTimeIdentified;

	};

	auto GetNextDelPos = [&](string line) {
		return line.find(m_delimiters.at(m_ndelim));
	};
	auto GetNextLine = [&](size_t breakpos) {
		pair<string, size_t> out;
		if (breakpos == string::npos) {
			out.first = string_buf;		
		}
		else {
			out.first = string_buf.substr(0, breakpos);
		}
		nrec++;
		out.second = nrec;
		if (breakpos != string::npos)
			string_buf = string_buf.substr(breakpos + m_linebreak.size());
		else
			string_buf.clear();
		return out;
	};
	auto CountVarsAndSetPos = [&](size_t breakpos, string &line) {
		line = line.substr(breakpos + 1);
		size_t npos = line.find(m_delimiters.substr(m_ndelim, 1));
		nvar++;
		return npos;
		

	};
    enum class ReadVar  {StatNamn, ParNamn, Period, DatumStart, No};
	auto ReadStatus = ReadVar::No;
	size_t Valid_Line = 0;
	int countvarhead = 0;
	auto CountHeads = [&]() {
	
		if (m_format == SITES) {
			
			if (nhead == string::npos) {

				if (varstr.find("LATITUDE:") == 0) {
					out.dec_cord.y = FUtil::AtoFloat(varstr.substr(10));

				}
				else if (varstr.find("LONGITUDE:") == 0) {
					out.dec_cord.x = FUtil::AtoFloat(varstr.substr(11));

				}
				else if (varstr.find("ALTITUDE:") == 0) {
					if (varstr.find("m a.s.l.") != string::npos) {
						varstr = varstr.substr(0, varstr.find("m a.s.l."));
					}
					out.dec_cord.z = FUtil::AtoFloat(varstr.substr(10));
				}
				else if (varstr.find("####") == 0) {
					nhead = nrec + 1;
				}
				else if (varstr.find("PARAMETER AND COLUMN INFO") == 0) {
					col_info_rec = nrec + 1;
					out.Des.base.Id.clear();
					out.Des.base.Name.clear();
					out.Des.base.Unit.clear();
					out.Des.base.Pos.clear();
					out.Des.addinfo.clear();

				}
				else if (col_info_rec > 0) {
					if (nvar == 1) {
						AddInfo addinfo;
						addinfo.CountryCode = "SE";
						addinfo.StationName = out.StatName;
						addinfo.Lat = out.dec_cord.y;
						addinfo.Long = out.dec_cord.x;
						addinfo.Alt = out.dec_cord.z;
						size_t start_namepos = varstr.find(" - ");

						size_t start_unit_pos = varstr.find("(");
						size_t end_unit_pos = varstr.find(")");

						size_t depth_pos = varstr.find("cm depth");
						size_t depth_pos_start = varstr.find("@");
						string depth = "";
						if (depth_pos != string::npos) depth = varstr.substr(depth_pos_start + 1, depth_pos - depth_pos_start - 1);
						FUtil::trim(depth);

						string name = varstr.substr(0, start_namepos);
						if (start_namepos == string::npos) {
							if (start_unit_pos != string::npos)
								name = varstr.substr(0, start_unit_pos);
							else
								name = varstr;


						}
						else {
							if (start_unit_pos != string::npos) name = varstr.substr(start_namepos + 2, start_unit_pos - start_namepos - 2);
						}
						FUtil::trim(name);


						string unit = "";
						if (start_unit_pos != string::npos && end_unit_pos != string::npos) unit = varstr.substr(start_unit_pos + 1, end_unit_pos - start_unit_pos - 1);
						FUtil::trim(unit);
						string id = "";
						if (start_namepos < 15) id = varstr.substr(0, start_namepos);
						FUtil::trim(id);
						out.Des.base.Id.push_back(id);
						out.Des.base.Name.push_back(name);
						out.Des.base.Unit.push_back(unit);
						out.Des.base.Pos.push_back(depth);
						out.Des.addinfo.push_back(addinfo);
					}
					else if (nvar == 2) {
						size_t depth_pos = varstr.find("meters above ground");
						size_t depth_pos_start = varstr.find("@");
						string height = "";
						if (depth_pos_start != string::npos) height = varstr.substr(depth_pos_start + 1, depth_pos - depth_pos_start - 1);
						FUtil::trim(height);
						size_t trimp = height.find(".");
						if (trimp != string::npos) {
							string add = height.substr(trimp + 1);
							height = height.substr(0, trimp)+add;

						}
						out.Des.base.Pos[out.Des.base.Pos.size()-1]=height;
						

					}
				}
			}
			else if (nrec == nhead && nrec > 0) {
				if (nvar == 1) out.FirstValidHeads.clear();
				out.FirstValidHeads.push_back(varstr);
				if (varstr == "TIMESTAMP")	out.DateColumn = nvar;
				out.MaxVar = nvar;
			}
			else if (nrec == 0) nhead = string::npos;
			else if (varstr.find("SITES STATION:") == 0) {
				out.StatName = varstr.substr(15);
				nhead = string::npos;
			}
			else if (nrec > nhead) return;
			return;

		}
		else if (m_format == SMHI_OPENDATA) {
			if (varstr == "Stationsnamn") {
				ReadStatus = ReadVar::StatNamn;
				Valid_Line = nrec+1;
				SMHI_StatLineNo = nrec + 1;

			}
			else if (varstr == "Parameternamn") {
				ReadStatus = ReadVar::ParNamn;
				Valid_Line = nrec + 1;
				SMHI_ParLineNo = nrec + 1;
				nhead = nrec;
			}
			else if (varstr == "Frn Datum Tid (UTC)") {
				ReadStatus = ReadVar::DatumStart;
				Valid_Line = nrec + 1;
				SMHI_HeadLineNo = nrec;
				SMHI_TidPosStart = 3;
				out.DateColumn = nvar+2;
				out.SkipBeforeVar = 2;
				m_CurrentFileInfo.DateColumn = out.DateColumn;
				out.HourColumn = string::npos;
				out.TimeColumn = string::npos;
				m_CurrentFileInfo.IdentifiedDateFormat = YMD;
				nhead = nrec;
			}
			else if (varstr == "Datum") {
				ReadStatus = ReadVar::DatumStart;
				Valid_Line = nrec + 1;
				SMHI_HeadLineNo = nrec;
				SMHI_TidPosStart = 1;
				nhead = nrec;
				out.DateColumn = nvar;
			}
			else if (varstr == "Tid (UTC)") {
				out.HourColumn = nvar;
				out.TimeColumn = nvar;


			}
			else if (varstr == "Tidsperiod") {
				ReadStatus = ReadVar::Period;
				Valid_Line = nrec + 1;
				SMHI_PeriodLineNo = nrec + 1;
			}
			if (Valid_Line == nrec&&nvar==1) {
				switch (ReadStatus) {
					case ReadVar::StatNamn:
						out.StatName = varstr;
					break;
					case ReadVar::ParNamn:
						out.Des.base.Name.push_back(varstr);
						break;
					case ReadVar::Period:

						break;
					case ReadVar::DatumStart:


						break;

				}



			}
			if (ReadStatus == ReadVar::ParNamn) {
				if (Valid_Line == nrec) {
					if (nvar == 3) {
						out.Des.base.Unit.push_back(varstr);
					 }
				}
				if (Valid_Line+1 == nrec) {
					if (nvar == 3) {
						out.Des.base.Unit.push_back(varstr);
					}
				}



			}
			if (nrec == SMHI_HeadLineNo && nvar >= SMHI_TidPosStart && varstr!=""&&varstr!="Tidsutsnitt:") out.FirstValidHeads.push_back(varstr);

		}
		else if (m_format == PERRENIAL) {
			countvarhead++;
			nhead = min(size_t(2), nrec);
			if(nrec==1)  out.FirstValidHeads.push_back(varstr);
			if(nrec==2)  out.UnitsFromHead.push_back(varstr); ;
		}
		else {
			if (varstr.find("SITES STATION:") != string::npos) {
				out.StatName = varstr.substr(15);
				m_format = SITES;
				nhead = string::npos;
				return;
			}
			else if (varstr.find("TITLE:") != string::npos && nrec==1) {
				nhead = string::npos;
				return;
			}
			if (nrec < 14 && varstr == "NaN") return;
			if (nrec <= 9 && varstr.find("Variable ") != string::npos||m_format==STANDARD) {
				nhead = min(size_t(9), nrec);
				out.DateColumn = 1;
				m_format = STANDARD;
				if (nrec == 1 && varstr != "Variable Names:") 	out.FirstValidHeads.push_back(varstr);
				out.SkipBeforeVar = 1;
			}
			else if (nrec < SMHI_HeadLineNo && m_format == SMHI_OPENDATA) {
				nhead = nrec;
				if (nrec >= 10 && varstr == "Datum") SMHI_HeadLineNo = nrec;
				if (nrec >= 10 && varstr.find("Datum Tid") != string::npos) SMHI_HeadLineNo = nrec;

				if (nrec != SMHI_HeadLineNo) out.FirstValidHeads.clear();

			}
			else if (nrec <= 14){
				if (varstr.find("Date") != string::npos || varstr.find("DATE") != string::npos || varstr.find("TIMESTAMP") != string::npos) {
					nhead = nrec;
					out.DateColumn = out.FirstValidHeads.size() + 1;
				}
				else if (((varstr.find("DATE") != string::npos || varstr.find("Date") != string::npos) || varstr.find("date") != string::npos)) {
					nhead = nrec;
					out.DateColumn = out.FirstValidHeads.size() + 1;
				}
				else if ((varstr.find("ISOD") == 0 || varstr.find("date") == 0 || varstr.find("DATUM") == 0)) {
					nhead = nrec;
					out.DateColumn = out.FirstValidHeads.size() + 1;
				}
				else if ((varstr.find("year") == 0 || varstr.find("Year") == 0)) {
					nhead = nrec;
					out.YearColumn = out.FirstValidHeads.size() + 1;
				}
				else if ((varstr.find("julian") == 0 || varstr.find("Julian") == 0)) {
					nhead = nrec;
					out.DayNumberColumn = out.FirstValidHeads.size() + 1;
					m_format = STANDARD;
				}
				else if ((varstr.find("hour") == 0 || varstr.find("Hour") == 0)) {
					nhead = nrec;
					out.HourColumn = out.FirstValidHeads.size() + 1;
				}
				else if ((varstr.find("time") == 0 || varstr.find("Time") == 0) || varstr.find("TIME") == 0) {
					nhead = nrec;
					out.TimeColumn = out.FirstValidHeads.size() + 1;
				}
				else if (varstr.find("mon") && m_format == SEPARATED_YMDH)
					nhead = nrec;
				else if (varstr.find("date") == 0)
					nhead = nrec;
				else if (varstr.find("DATUM") == 0)
					nhead = nrec;
				else if (varstr.find("code") == 0)
					nhead = nrec;

			}
			else if (nrec == 1 && varstr.find("mon") == 0 && m_format != SEPARATED_YMDHM)
			{
				nhead = nrec;
			} // Swiss climate file

			else if (nrec <= 15 && varstr.find("TE") == 0) {
				if (nrec == 6 && nhead == 1) { m_format = SMHI; nhead = 5; };
			}
			else if (varstr.find("TIME") == 0 || varstr.find("Time") == 0 || varstr.find("Tid") == 0 || varstr.find("time") == 0) {
				out.TimeColumn = out.FirstValidHeads.size() + 1;
				nhead = nrec;
			}
			else if (nrec <= 15 && size_t(varstr[0]) >= 64 && size_t(varstr[0]) < 96 && nhead == 0) {
				size_t ival = size_t(varstr[0]);
				nhead = nrec;
			}
			else if (varstr == "site_id") {
				SiteId_Index = out.FirstValidHeads.size();
				nhead = nrec;
			}
			
			
			if (nrec == nhead + 1 && !FUtil::IsValidNumber(varstr) && nvar < 2) {
				if (varstr == "dd/mm/yyyy") UnitsHeaderFind = true;
				nhead = nrec;
			}
	
			if (UnitsHeaderFind&& nrec==2)	 out.UnitsFromHead.push_back(varstr);
			else if (nhead != 0 && nrec ==nhead  && m_format != SMHI_OPENDATA&&m_format!=STANDARD) 	out.FirstValidHeads.push_back(varstr);
			else if (m_format == SMHI_OPENDATA && nrec == SMHI_HeadLineNo&& nvar>= SMHI_TidPosStart &&varstr!=""&&varstr!="Tidsutsnitt:") out.FirstValidHeads.push_back(varstr);
		}

	};
	size_t DescriptionType = 0;
	AddInfo ainfo;
	auto SetDescription = [&]() {
		if (m_format == SMHI_OPENDATA) {
			if (nrec == SMHI_StatLineNo && nvar == 1) out.StatName = varstr;
			if (nrec == SMHI_StatLineNo && nvar == 2) out.StatNo = varstr;
			if (nrec == SMHI_PeriodLineNo) {
				if (nvar == 3) out.dec_cord.z = FUtil::AtoFloat(varstr);
				if (nvar == 4) out.dec_cord.y = FUtil::AtoFloat(varstr);
				if (nvar == 5) out.dec_cord.x = FUtil::AtoFloat(varstr);

			}
			else if (nrec == SMHI_HeadLineNo) {

			}
			else if (nrec == SMHI_ParLineNo) {
				if (nvar == 1) {
					//out.Des.base.Name.push_back(varstr);
				}
				else if (nvar == 3) {
					//out.Des.base.Unit.push_back(varstr);
				}


			}
			out.MaxVar=out.FirstValidHeads.size();
	//		out.MaxVar = 3;
		}
		else if (m_format == SILO) {

			if (nvar == 3) out.dec_cord.z = FUtil::AtoFloat(varstr);

		}
		else if (nrec > nhead) return;
		else if (nrec <= 10 && nvar == 1 && varstr.find("Variable Names:") != string::npos) {
			DescriptionType = 0;
		}
		else if (nrec <= 10 && nvar == 1 && (varstr.find("Variable Units:") != string::npos|| varstr.find("dd")!=string::npos)) {
			DescriptionType = 1;
			if(varstr.find("dd") != string::npos&& out.Des.base.Unit.size()==0) 	out.Des.base.Unit.push_back(varstr);
		}
		else if (nrec <= 10 && nvar == 1 && varstr.find("Variable Id:") != string::npos) {
			DescriptionType = 2;
		}
		else if (nrec <= 10 && nvar == 1 && varstr.find("Variable Pos:") != string::npos) {
			DescriptionType = 3;
		}
		else if (nrec <= 10 && nvar == 1 && varstr.find("Variable Station:") != string::npos) {
			DescriptionType = 4;
		}
		else if (nrec <= 10 && nvar == 1 && varstr.find("Variable Country:") != string::npos) {
			DescriptionType = 5;
		}
		else if (nrec <= 10 && nvar == 1 && varstr.find("Variable Alt:") != string::npos) {
			DescriptionType = 6;
		}
		else if (nrec <= 10 && nvar == 1 && varstr.find("Variable Lat:") != string::npos) {
			DescriptionType = 7;
		}
		else if (nrec <= 10 && nvar == 1 && varstr.find("Variable Long:") != string::npos) {
			DescriptionType = 8;
		}
		else if (nrec >= 1 && nrec<9&&nrec == nhead && nvar == 1) {
			DescriptionType = nrec - 1;
			out.Des.base.Name.push_back(varstr);
			if (DescriptionType == 1) out.Des.base.Unit.push_back("");
			else if (DescriptionType == 2) out.Des.base.Id.push_back("");
			else if (DescriptionType == 3) out.Des.base.Pos.push_back("");

		}
		else if ((nvar > 1&&out.Des.base.Name.size()<nvar)|| (DescriptionType>0&&DescriptionType<9)) {
			switch (DescriptionType) {
			case 0:
				out.Des.base.Name.push_back(varstr);

				break;
			case 1:
				out.Des.base.Unit.push_back(varstr);

				break;
			case 2:
				out.Des.base.Id.push_back(varstr);

				break;
			case 3:
				out.Des.base.Pos.push_back(varstr);

				break;
			case 4:
				ainfo.StationName = varstr;
				out.Des.addinfo.push_back(ainfo);
				break;
			case 5:
				out.Des.addinfo[nvar - 2].CountryCode = varstr;
				break;
			case 6:
				if (out.Des.addinfo.size() > nvar - 2)
					out.Des.addinfo[nvar - 2].Alt = FUtil::AtoFloat(varstr);
				else {
					ainfo.Alt = FUtil::AtoFloat(varstr);
					out.Des.addinfo.push_back(ainfo);
				}
				break;
			case 7:
				out.Des.addinfo[nvar - 2].Lat = FUtil::AtoFloat(varstr);
				break;
			case 8:
				out.Des.addinfo[nvar - 2].Long = FUtil::AtoFloat(varstr);
				break;
			};
		}
		
		if(m_format!=SMHI_OPENDATA)	out.MaxVar = nvar;


	};

	LINKED_FILEINFO link;
	link.Records = 1;
	link.RecordsSelected = 0;
	link.Ids = 0;
	link.FilterItems.clear();
	string CurrentFilterId = "";
	map<string, size_t> LocalFilterMap;
	vector<size_t> LocalFilterCount;


	auto CountSitesAndSiteRecords = [&]() {
		if (nrec == 2) {
			vector<size_t> numsiterec;
			CurrentSite = FUtil::AtoInt(varstr);
			SiteRecCount++;
			link.Ids = CurrentSite;
			out.LinkedSites.push_back(link);
		}
		else if (CurrentSite != string::npos) {
			if (CurrentSite == size_t(FUtil::AtoInt(varstr))) {
				SiteRecCount++;
				out.LinkedSites.back().Records = SiteRecCount;
			}
			else if (out.LinkedSites.size() > 0) {
				out.LinkedSites.back().FilterItems.clear();

				for (auto it = LocalFilterMap.begin(); it != LocalFilterMap.end(); ++it) {
					out.LinkedSites.back().FilterItems.push_back((*it).first);
				}
				out.LinkedSites.back().TimeCheck = ResetTimeMap();
				LocalFilterMap.clear();
				link.Ids = CurrentSite = FUtil::AtoInt(varstr);
				SiteRecCount = 1;
				out.LinkedSites.push_back(link);
			}
		}
	};



	auto CheckNumberOfRecordsAfterFilterTest = [&]() {
		if (CurrentFilterId == m_FilterToUseId) {
			//if (out.LinkedSites.size() == 0) out.LinkedSites.push_back(link);
			if (out.LinkedSites.size() > 0) out.LinkedSites.back().RecordsSelected++;
		};
	};
	auto CountFilterCases = [&]() {
		if (nrec == 1) {
			return;
		}
		else if (nrec == 2) {
			m_FilterCount.push_back(0);
			LocalFilterCount.push_back(0);
			m_FilterMap.insert(pair<string, size_t>(varstr, 0));
			LocalFilterMap.insert(pair<string, size_t>(varstr, 0));
		}
		CurrentFilterId = varstr;
		auto it = m_FilterMap.find(varstr);
		if (it != m_FilterMap.end()) {
			m_FilterMap.insert(pair<string, size_t>(varstr, m_FilterCount.size() - 1));
			m_FilterCount[(*it).second]++;
		}
		else {
			m_FilterCount.push_back(0);
			m_FilterMap.insert(pair<string, size_t>(varstr, m_FilterCount.size() - 1));

		}
		it = LocalFilterMap.find(varstr);
		if (it != LocalFilterMap.end()) {
			LocalFilterMap.insert(pair<string, size_t>(varstr, LocalFilterCount.size() - 1));
			LocalFilterCount[(*it).second]++;
		}
		else {
			LocalFilterCount.push_back(0);
			LocalFilterMap.insert(pair<string, size_t>(varstr, LocalFilterCount.size() - 1));
		}


	};
	auto CheckAndSetDateFormats = [&]() {
		if (out.DateColumn == nvar) {
			if (varstr.find("/") > 0 && varstr.find("/") < 3 && varstr.rfind("/") > varstr.find("/")) {
				out.IdentifiedDateFormat = DMY_SLASH;
				out.IdentifiedStartMin = PGUtil::MinutConv(PGUtil::DateConv(out.IdentifiedDateFormat, varstr));
			}
			else if (varstr.find("\\") == 2 && varstr.rfind("\\") == 5) {
				out.IdentifiedDateFormat = DMY_SLASH;
				out.IdentifiedStartMin = PGUtil::MinutConv(PGUtil::DateConv(out.IdentifiedDateFormat, varstr));
			}
			else if (varstr.find('-') < varstr.rfind('-') && varstr.find('-') >= 3 && varstr.size() < 12) {
				out.IdentifiedDateFormat = YMD;
				out.IdentifiedStartMin = PGUtil::MinutConv(PGUtil::DateConv(out.IdentifiedDateFormat, varstr));
			}
			else {
				out.IdentifiedDateFormat = YMDHM;
				out.IdentifiedStartMin = PGUtil::MinutConv(PGUtil::DateConv(out.IdentifiedDateFormat, varstr));

			}
		}
		else if (out.TimeColumn == nvar - 1|| out.TimeColumn == nvar) {
			size_t minut;
			minut = out.IdentifiedStartMin - 720;
			string datum = PGUtil::STD_StringDatum(minut);
			datum = datum.substr(0, 11) + varstr;
			out.IdentifiedStartMin = PGUtil::MinutConv(datum);
			m_CurrentFileInfo.TimeColumn = out.TimeColumn;
			m_CurrentFileInfo.HourColumn = out.HourColumn;
		}
		else if (out.YearColumn == nvar) {

		}
		else if (out.DayNumberColumn == nvar) {

		}

		else if (nvar == 2&& m_format !=SMHI_OPENDATA) {
			if (PGUtil::CheckForPossiblePGDateString(varstr)) {
				out.DateColumn = nvar - 1;
				if (varstr.find("/") == 2 && varstr.rfind("/") == 5) {
					out.IdentifiedDateFormat = DMY_SLASH;
					out.IdentifiedStartMin = PGUtil::MinutConv(PGUtil::DateConv(out.IdentifiedDateFormat, varstr));
				}
				else if (varstr.find("\\") == 2 && varstr.rfind("\\") == 5) {
					out.IdentifiedDateFormat = DMY_SLASH;
					out.IdentifiedStartMin = PGUtil::MinutConv(PGUtil::DateConv(out.IdentifiedDateFormat, varstr));
				}
				else if (varstr.find('-') < varstr.rfind('-') && varstr.find('-') >= 3 && varstr.size() < 12) {
					out.IdentifiedDateFormat = YMD;
					out.IdentifiedStartMin = PGUtil::MinutConv(PGUtil::DateConv(out.IdentifiedDateFormat, varstr));
				}
				else {
					out.IdentifiedDateFormat = YMDHM;
					out.IdentifiedStartMin = PGUtil::MinutConv(PGUtil::DateConv(out.IdentifiedDateFormat, varstr));
				}
				m_CurrentFileInfo.DateColumn = out.DateColumn;
				m_CurrentFileInfo.IdentifiedDateFormat = out.IdentifiedDateFormat;
				m_CurrentFileInfo.IdentifiedStartMin = out.IdentifiedStartMin;
			}
			else
				return ;
		}
		else if (nvar == 1 && m_format == STANDARD) {
			out.IdentifiedDateFormat = YMDHM;
			out.IdentifiedStartMin = PGUtil::MinutConv(PGUtil::DateConv(out.IdentifiedDateFormat, varstr));
			out.DateColumn = 1;

		}
		else if (nvar == 1 && m_format == PERRENIAL) {
			out.IdentifiedDateFormat = YMDHM;
			out.IdentifiedStartMin = PGUtil::MinutConv(PGUtil::DateConv(out.IdentifiedDateFormat, varstr));
			out.DateColumn = 1;
		}
		m_CurrentFileInfo.DateColumn = out.DateColumn;
		m_CurrentFileInfo.HourColumn = out.HourColumn;
		m_CurrentFileInfo.TimeColumn = out.TimeColumn;
		m_CurrentFileInfo.DayNumberColumn = out.DayNumberColumn;
		m_CurrentFileInfo.YearColumn = out.YearColumn;

		m_CurrentFileInfo.IdentifiedDateFormat = out.IdentifiedDateFormat;
		m_CurrentFileInfo.IdentifiedStartMin = out.IdentifiedStartMin;
	};
	m_FileText.seekg(0, ios::beg);
	string_buf.clear();
	if (m_AddFromFile)	m_nprevrec = GetNumRecords();

	size_t ich;


	while (pos2<m_totsizeInputFile||string_buf.size()>0) {
		string_buf += GetStringFromStream();
		ich = CheckSizeOfValidLine(string_buf);
		string headerline;
		if (nrec == 0) {
			/*if (m_format == PERRENIAL) {
				auto next = ich+2;
				auto add = next;
				headerline.clear();
				for (size_t i = 0; i < 3; i++) {
					string next_str = string_buf.substr(next);
					headerline += next_str;
					next += CheckSizeOfValidLine(next_str)+2;
				}
				ich = next-2;
				headerline.substr(2, ich);
			}
			else */
			if (string_buf.find(m_linebreak)>0) {
				headerline = string_buf.substr(0, string_buf.find(m_linebreak));
			}
			m_ndelim = FindAndSetDelimitter(headerline);
            
			if (m_ndelim == string::npos&&m_format!=SILO) {
                #ifndef COUPSTD
				CWinApp* pApp = AfxGetApp();
				pApp->BeginWaitCursor();
				pApp->EndWaitCursor();
				MFC_Util::MessageBox("No valid delimiter (,)(;) (tab) or (space) found in input file", "Error", MB_ICONERROR);
                #endif
                return out;
			}
            
		}
		else {
			if (ich < out.MaxVar&&nrec>10000) {
				ich = string::npos;
				string_buf.clear();
				continue;
			}
		}
		bool DefinedDate = false;
		string firstcolumn;
		size_t npos = 0;
		
		NextLine = GetNextLine(ich);
		while (NextLine.first.size() > 0) {

			string line = NextLine.first;
			ich = NextLine.second;

			npos = GetNextDelPos(line);

			auto GetVarString = [&](size_t breakpos) {
				if (breakpos != string::npos) return (line.substr(0, breakpos));
				else return line;
			};
			nvar = 1;
			bool ValidTime=false;

			while (line.size() > 0) {
				varstr = GetVarString(npos);
				CountHeads();
				if(nhead!= string::npos) SetDescription();

				if (m_format != IDSCAN&&nhead!=string::npos) {
					if (nrec == nhead + 1&&!ValidTime) {
						CheckAndSetDateFormats();

 					}
					if (nrec >= nhead + 1&& !ValidTime) {
						ValidTime = SetTimeVariable(nrec, varstr, nvar);
 					}
					else {
						int koll=1;
					}
				}

 				if (npos != string::npos) {
					npos = CountVarsAndSetPos(npos, line);
				}
				else {
					varstr = GetVarString(npos);
					line.clear();
				}
			}
			

			if (SiteId_Index != string::npos && nrec > 1) CountSitesAndSiteRecords();
			if (nvar - 1 == m_FilterTestColumn) CountFilterCases();

			if (nrec % 100 == 0) {
#ifndef COUPSTD
				MFC_Util m;
				m.SetPaneMessage("Records read: " + FUtil::STD_ItoAscii(nrec));
#endif
			}
			if(ValidTime) SetTimeMap();

 
			ich = CheckSizeOfValidLine(string_buf);
			if (ich != string::npos) {
				NextLine = GetNextLine(ich);
			}
			else
				NextLine.first.clear();
		}

	}
	SetValidTimeIdVariables();
	
	if (SiteRecCount > 1) {
		out.LinkedSites.back().FilterItems.clear();
		out.LinkedSites.back().TimeCheck = ResetTimeMap();
		for (auto it = LocalFilterMap.begin(); it != LocalFilterMap.end(); ++it) {
			out.LinkedSites.back().FilterItems.push_back((*it).first);
		}
	}
	else {
		if (out.LinkedSites.size() == 0) {
			LINKED_FILEINFO link;
			link.TimeCheck = ResetTimeMap();
			link.Ids = string::npos;
			link.Records = link.TimeCheck.NumUniqueSteps;
			link.RecordsSelected = link.Records;
			if (link.TimeCheck.RecordStep == string::npos) {
				link.TimeCheck.RecordStep = 1;
				out.RedNumVar = 0;
				if (out.ValidVariables.size() > 0) out.ValidVariables[0] = true;
				while (out.ValidVariables.size() < out.FirstValidHeads.size() && nhead == 0) {
					out.ValidVariables.push_back(true);
				}
			}
			

			out.LinkedSites.push_back(link);
			if (link.Records > 1) {
				out.MaxVar *= link.TimeCheck.RecordStep;
				out.RedNumVar *= link.TimeCheck.RecordStep;
			}
			else {
			
			
			
			}
		}
		else
			out.LinkedSites.back().TimeCheck = ResetTimeMap();
	}

	if (nrec > 1800000) {   // Stupid very high number to be changed when need another output format
		out.TotRec = nrec;
		out.MaxVar = 32;
		out.NumHeads = 1;
	}
	else {
		out.NumHeads = nhead;
		out.TotRec = nrec;
	}

	return out;
}
FILEINFO PGFileImport::CheckHeadingAndFileInformation(FILEINFO out)
{

	auto CheckFilterRecords = [&]() {
		out.FilterColumnToUse = string::npos;
		out.GlobalFilterItems.clear();
		out.FilterItemToUse = "";
		if (m_FilterMap.size() > 0) {
			out.FilterColumnToUse = m_FilterTestColumn;
			
		}
		for (auto it = m_FilterMap.begin(); it != m_FilterMap.end(); ++it) {
			out.GlobalFilterItems.push_back((*it).first);
		}
		if(m_FilterMap.size()>0) out.FilterItemToUse = out.GlobalFilterItems[0];

	};

	bool IsTimeIdentified = true;
	auto CheckIdentifiedTime = [&]() {

		if (out.DateColumn == string::npos&&out.HourColumn == string::npos&&out.TimeColumn == string::npos&&
			out.DayColumn == string::npos&&out.YearColumn == string::npos) {
			out.IdentifiedStartMin = PGUtil::MinutConv("2010-01-01 12:00");
			IsTimeIdentified = false;
		}
	};

	auto CheckCountedSiteRecords = [&]() {
		out.EqualSizeOfSiteRecords = true;
		for (size_t i = 1; i<out.LinkedSites.size(); i++) {
			if (out.LinkedSites[0].Records != out.LinkedSites[i].Records) out.EqualSizeOfSiteRecords = false;
		}
		if (out.LinkedSites.size() == 0) out.EqualSizeOfSiteRecords = false;
	};
	auto CheckTimeIncrements = [&]() {
		for (size_t i = 0; i < out.LinkedSites.size(); i++) {
			int check = out.LinkedSites[i].Records - out.LinkedSites[i].TimeCheck.RecordStep*out.LinkedSites[i].TimeCheck.NumUniqueSteps;
			if (check == 0)
				out.LinkedSites[i].TimeCheck.CommonRecStep = true;
			else
				out.LinkedSites[i].TimeCheck.CommonRecStep = false;
		}

	};


	auto SetIdentifiedDelimiters = [&]() {
		out.IdentifiedDelimiter = m_ndelim;
		out.IdentifiedLineEnd = m_linebreak.size();
	};


	CheckIdentifiedTime();
	CheckCountedSiteRecords();
	CheckFilterRecords();
	CheckTimeIncrements();
	SetIdentifiedDelimiters();


	auto SetOutputVariablesAndSetFormat = [&]() {
		if(out.IdentifiedFormat==SMHI_OPENDATA) return;
		if (IsTimeIdentified) {
			out.IdentifiedFormat = STANDARD;
			if (out.DateColumn != string::npos) out.IdentifiedFormat = STANDARD;
			if (out.YearColumn != string::npos && out.MonthColumn != string::npos && out.DayColumn != string::npos && out.HourColumn != string::npos &&
				out.MinuteColumn != string::npos) out.IdentifiedFormat = SEPARATED_YMDHM;
			else if (out.YearColumn != string::npos && out.MonthColumn != string::npos && out.DayColumn != string::npos && out.HourColumn != string::npos)
				out.IdentifiedFormat = SEPARATED_YMDH;
			else if (out.YearColumn != string::npos && out.MonthColumn != string::npos && out.DayColumn != string::npos) out.IdentifiedFormat = SEPARATED_YMD;
			
		}
		else
			out.IdentifiedFormat = UNDEFINEDFORMAT;

		if (out.LinkedSites.size() > 1) out.IdentifiedFormat = PROFOUND;
	};

	auto SetSizeOfDescriptions = [&]() {

		size_t num = out.FirstValidHeads.size()*out.LinkedSites.back().TimeCheck.RecordStep;
		if (out.LinkedSites.back().TimeCheck.RecordStep == string::npos) num = out.FirstValidHeads.size();
		out.Des.addinfo.resize(num);
		for_each(out.Des.addinfo.begin(), out.Des.addinfo.end(), [](AddInfo& info) {
			info.Unit =UNIT_TYPES::NO_UNIT; info.Element = GENERAL; } );
		out.Des.base.Name.resize(num);
		out.Des.base.Unit.resize(num);
		out.Des.base.Id.resize(num);
		out.Des.base.Pos.resize(num);

	};

	SetSizeOfDescriptions();
	SetOutputVariablesAndSetFormat();

	

	if (m_format == SEPARATED_YMDHM)
		out.NumHeads = 1;


	if (m_format == CZECH_COL_TRANS) {
		out.NumHeads = 1;
		out.TotRec = out.MaxVar;
		out.MaxVar = 1;
		out.FixedFormat = true;
	}
	else if (m_format == MONTH_COL_TRANS) {
		out.NumHeads = 1;
		out.TotRec = (out.TotRec - 1) * 12 + 1;
		out.MaxVar = 1;
		out.FixedFormat = true;
	}
	else if (m_format == SILO) {
		out.FixedFormat = true;
	}

	else {
		out.FixedFormat = false;
	}

	return out;
}
vector<string> PGFileImport::CreatePGFileFromDefinedInputFiles()
{
	//if(m_filename.size()>0)	return CreatePGFileFromIButtonFiles();
	vector<string> CreatedPGFileName;
	m_minend_a = 1440, m_totrec_a;
	FILEINFO Input=m_CurrentFileInfo;
	string filename = m_filename;
	m_CreateMultiFile = false;
	std::streamsize nbyte = 1024;
	m_HeadingRead = true;



	size_t nrec = 0;
	size_t nvar = 0;

	Input = GetFileInfo(Input, 0);
	m_time_red = 0;
	if (Input.NumHeads == 0 || m_format == STANDARD || m_format == SEPARATED_YMDH || m_format == SEPARATED_YMD || m_format == SMHI_OPENDATA || m_format == SEPARATED_YMDHM || m_format == YMD_HM || m_format == SWISS || m_format == CZECH_COL_TRANS || m_format == MONTH_COL_TRANS || m_format == PROFOUND)
		m_time_red = 0;
	else
		m_time_red = 1;

	size_t choice;


	string str;
	str = "Reading content of txt file: ";
	str += filename;
    #ifndef COUPSTD
	MFC_Util m;
	m.SetPaneMessage(str);
#endif
	std::streampos  actpos;
	if (!m_CollectedFile) m_ImportedFileVector.clear();
	if (m_ImportedFileVector.size() == 0 || (m_CollectedFile&&m_ImportedFileVector.size() == 0))
		m_ImportedFileVector.push_back(m_CurrentFile);

	size_t filestart = 0;
	if (m_CollectedFile&&m_format != PROFOUND)
		filestart = m_ImportedFileVector.size() - 1;

	m_NumToImport = m_ImportedFileVector.size();



	for (m_countf = filestart; m_countf<m_ImportedFileVector.size(); m_countf++) {
		if (m_ImportedFileVector.size() > 1) {
			str = "Reading and Writing from file (" + FUtil::STD_ItoAscii(m_countf + 1) + ") in a sequence of " + FUtil::STD_ItoAscii(m_ImportedFileVector.size()) + " Files";
            #ifndef COUPSTD
			m.SetPaneMessage(str);
#endif
		}
		if (m_ImportedFileVector.size()>1) {
			nrec = m_totrec_a + 1;
		}
		if (!CreateNewFileAndDefineSize(Input, m_countf)) return CreatedPGFileName;

		if (m_countf == 0)
			m_nrec = 0;

		SetFileDescriptions();
		InitForSettingMinMaxMissingValues();


		ReadingVariableValues(Input, m_countf);

		auto SetMinAndMax = [&]() {
			if (!Input.EqualSizeOfSiteRecords&&m_format == PROFOUND) {
				for (size_t j = 1; j <= pg.Shape.NumVar; j++) {
					SetVarMin(j, m_vmin[j - 1], 1);
					SetVarMax(j, m_vmax[j - 1], 1);
				}
			}
			else if(pg.Shape.NumRepetitions>= m_countf + 1) {
				for (size_t j = 1; j <= pg.Shape.NumVar; j++) {
					SetVarMin(j, m_vmin[j - 1], m_countf + 1);
					SetVarMax(j, m_vmax[j - 1], m_countf + 1);
				}
			}
		};

		auto CloseFileWithoutRepetions = [&]() {;
			SetMinAndMax();
			WritePGFileHeader(GetNumRecords(), pg.Shape.NumRepetitions);
			CreatedPGFileName.push_back(pg.status.FileName);
			CloseFile();
		};

		auto CloseFileWithRepetions = [&]() {
			bool Close = false;
			if (m_format == NOAA) {
				pg.Shape.NumRepetitions = m_ImportedFileVector.size();
				FindKeyValues();
				Close = true;
			}
			else if (m_format == CZECH_COL_TRANS) {
				Close = true;
				m_countf = m_NumToImport - 1;
			}
			if (m_countf == m_NumToImport - 1 || m_NumToImport == 0) Close = true;
			if (Close) {
				WritePGFileHeader(GetNumRecords(), pg.Shape.NumRepetitions);
				CloseFile();
				CreatedPGFileName.push_back(pg.status.FileName);
				m_Closed = true;
			}
			else {
				SetMinAndMax();
			}

		};

		if (m_format != PROFOUND) {
			m_min_next = 0;
			actpos = m_MainPGStreamReadWrite.tellp();
			m_nrec = size_t(actpos / pg.Shape.RecordSize) - 1;
			if (m_nrec < m_totrec_a*(m_countf + 1) - Input.NumHeads)
				WriteMissingRegularTime();
			actpos = m_MainPGStreamReadWrite.tellp();
			auto nrec_koll = actpos / pg.Shape.RecordSize - 1;
		}
		SetMinAndMax();
		if (m_format == PROFOUND && !Input.EqualSizeOfSiteRecords) 
			CloseFileWithoutRepetions();
		else if (m_countf == m_ImportedFileVector.size() - 1 || m_format == CZECH_COL_TRANS) 
			CloseFileWithRepetions();
		else
			CloseFileWithRepetions();

	}
	m_FileText.close();
	if (pg.Shape.NumRepetitions>3 && GetNumRecords()>1000) {
		SetCompleteRead(false);
	}
	else
		SetCompleteRead(true);


	str = "Created " + FUtil::STD_ItoAscii(CreatedPGFileName.size()) + " Files" ;
    #ifndef COUPSTD
	m.SetPaneMessage(str);
#endif

	return CreatedPGFileName;
}

vector<string> PGFileImport::CreatePGFileFromIButtonFiles()
{
	vector<string> CreatedPGFileName;
	m_minend_a = 1440, m_totrec_a;
	FILEINFO Input = m_CurrentFileInfo;
	string filename = m_filename;
	m_CreateMultiFile = false;
	std::streamsize nbyte = 1024;
	m_HeadingRead = true;
	m_time_red = 0;

	auto p = m_filename.find("Test.");

	m_ExtraInfoFileName = m_filename.substr(0, p) + "CoordEthiopia.csv";
	AddInfoLinkIndex[0] = 1;
	AddInfoLinkIndex[1] = 2;
	AddInfoLinkIndex[2] = 3;
	AddInfoLinkIndex[3] = 4;
	AddInfoLinkIndex[4] = 5;
	DefineAddInfoMap();
	size_t nrec = 0;
	size_t nvar = 0;

	Input = GetFileInfo(Input, 0);


	Input.LinkedSites.resize(60);
	Input.LinkedSites[0].Records = 6100;
	if (!CreateNewFileAndDefineSize(Input, m_countf)) return CreatedPGFileName;

	if (m_countf == 0)
		m_nrec = 0;

	//SetFileDescriptions();



	string line;
	line.resize(20000);
	ifstream istream;
	vector<float> varvalues;
	varvalues.resize(33);

	vector< vector< vector<float>> > AllVarValues;
	AllVarValues.resize(6100);
	for (size_t i = 0; i < 60; i++) {
		AllVarValues[i].resize(6100);
		for (size_t ii = 0; ii < 6100; ii++) {
			AllVarValues[i][ii].resize(33);
			size_t minvalue = 1060896960 + ii * 180;
			AllVarValues[i][ii][0] = *(float*)&minvalue;

			for (size_t iii = 1; iii < 33; iii++)
				AllVarValues[i][ii][iii] = MISSING;
		}

	}


	istream.open(m_filename, ifstream::in);


	InitForSettingMinMaxMissingValues();
	size_t ich = string::npos;
	size_t CountLine = 0;
	size_t failure = 0;
	size_t success = 0;

	auto GetLineFromStream = [&]() {
		line.clear();
		line.resize(20000);
		//string line;
		//if(ich>0) line=str.substr(0,ich);
		istream.getline(&line[0], 20000);
		size_t koll = line.size();
		ich = line.find('\0');
		line.resize(ich);
		if (ich <= 0) {
			if (istream.good()) {
				line.clear();
				line.resize(20000);
				istream.getline(&line[0], 20000);
				ich = line.find('\0');
				line.resize(ich);
			}
			else
				return false;
		}
		CountLine++;
		return true;
	};
	auto SetMinAndMax = [&]() {
		if (pg.Shape.NumRepetitions >= m_countf + 1) {
			for (size_t j = 1; j <= pg.Shape.NumVar; j++) {
				SetVarMin(j, m_vmin[j - 1], m_countf + 1);
				SetVarMax(j, m_vmax[j - 1], m_countf + 1);
			}
		}
	};

	size_t valid_var_index = 1;
	auto LastLinetoRead = Input.TotRec;
	
	while (istream.good() && CountLine < LastLinetoRead) {


		if (!GetLineFromStream()) return CreatedPGFileName;
		size_t npos;
		string varstr;
		size_t i_site=0;
		size_t i_var=0;
		size_t i_record = 0;

		
		size_t nvar = 0;
		float value = MISSING;
		size_t min = 0;
		if (m_ndelim != string::npos&&CountLine>1) {
			npos = line.find(m_delimiters.substr(m_ndelim, 1));
			while (npos != string::npos ) {

				if (npos != string::npos)
					varstr = line.substr(0, npos);

				else
					varstr = line;
				switch (nvar) {
				case 2:
					IPGTID koll;
					
					koll = PGUtil::Ato_IPGTID(varstr.substr(1,10));
					min = PGUtil::MinutConv(koll);
					break;
				case 3:
					min += FUtil::AtoInt(varstr) * 180;
					i_record = (min - 1060896960) / 180;
					break;
				case 4:
					value = FUtil::AtoFloat(varstr);
					break;
				case 5:
					if (varstr.find("Ge") != string::npos) {
						i_site = 0;
						string numstr = varstr.substr(3,2);
						i_site += FUtil::AtoInt(numstr);
					}
					else if (varstr.find("Go")!= string::npos) {
						i_site = 30;
						string numstr = varstr.substr(3,2);
						i_site += FUtil::AtoInt(numstr);
					}
					break;
				case 6:
					i_var= FUtil::AtoInt(varstr.substr(1,2));
					if (i_var < 1 || i_var>32) {
						size_t koll = 2;
						while (varvalues[koll] > 0.001&&koll>30) {
							koll++;
						}
						i_var = koll;
					}
					break;
				case 8:
					if (varstr.find("Soil")!=string::npos) i_var += 16;
					break;
				}


				nvar++;
				line = line.substr(npos+1);
				npos = line.find(m_delimiters.substr(m_ndelim, 1));

			}
			if (i_var != string::npos&&i_record<6100&&i_site>0) {
				varvalues[i_var - 1] = value;
				AllVarValues[i_site - 1][i_record][i_var] = value;
				success++;
			}
			else {
				failure++;

			}

			
		}
	}

	


	m_MainPGStreamReadWrite.seekp(pg.Shape.RecordSize, ios::beg);

	for (size_t i = 0; i < 60; i++) {
		InitForSettingMinMaxMissingValues();

		for (size_t ii = 0; ii < 6100; ii++) {
			pg.Var.vv[0]= AllVarValues[i][ii][0];
			auto koll = m_MainPGStreamReadWrite.tellp();
			for(size_t jvar=1; jvar<33; jvar++)
			pg.Var.vv[jvar] = AllVarValues[i][ii][jvar];
			//m_MainPGStreamReadWrite.write((char*)&AllVarValues[i][ii][0], pg.Shape.RecordSize);
			WriteCompleteRecord();
		}
		m_countf = i;
		size_t index_out;
		index_out = m_countf * pg.Shape.NumVar;
		string tunit= p_ModelInfo->GetUnitString(UNIT_TYPES::TEMP_UNIT);
		
		for (size_t jj = 0; jj < 32; jj++) {
			pg.Des.base.Name[index_out+jj] = "Temperature";
			if(jj<16) pg.Des.base.Id[index_out + jj] = "Canopy"+FUtil::STD_ItoAscii(jj+1);
			else  pg.Des.base.Id[index_out + jj] = "Soil" + FUtil::STD_ItoAscii(jj + 1-16);
			pg.Des.base.Unit[index_out+jj] = tunit;
		}



		SetAddInfoFromDefinedMap(i + 1);

		SetMinAndMax();
	}

	WritePGFileHeader(6100, 60);


	CloseFile();// SaveAs(pg.status.FileName);

	filename=pg.status.FileName;

	CreatedPGFileName.push_back(filename);


	return CreatedPGFileName;
}

void PGFileImport::ApplyHeaderLineAsNamesInPGFile(bool Units_Instead, bool Clean_Instead)
{
	size_t count = 0;

	for_each(m_CurrentFileInfo.FirstValidHeads.begin(), m_CurrentFileInfo.FirstValidHeads.end(), [&](string &str) {
		string to_use, to_Id, to_pos;
		vector<size_t> posv;
	
		size_t pos = str.find('_',0);
		while (pos != string::npos) {
			posv.push_back(pos);
			pos = str.find('_', pos+1);
		};
		if (Units_Instead) {
			if (str.rfind('_') != string::npos) {
				to_use = str.substr(str.rfind('_') + 1);
				auto pos = to_use.find("??");
				if (pos != string::npos) {
					string ne;
					ne= char(176);
					to_use.replace(pos, 2,ne);

				}
				pos = to_use.find("m-2");
				if (pos != string::npos) {
					string ne;
					ne = "/m";
					ne += char(178);
					to_use.replace(pos, 3, ne);

				}
				pos = to_use.find("ms-1");
				if (pos != string::npos) {
					string ne;
					ne = "m/s";
					to_use.replace(pos, 4, ne);

				}




			}
			else
				to_use = "";
		}
		else {
			if (posv.size() > 2) {
				to_Id = str.substr(posv[0], posv.back() - posv[0]);
				size_t l;
				l = to_Id.find('_');
				while (l != string::npos) {
					to_Id.replace(l, 1, " ");
					l = to_Id.find('_');
				}
				l = to_Id.find("cm");
				if (l != string::npos&&l<6) {
						to_pos = to_Id.substr(0, l);
						FUtil::trim(to_pos);
				}
				to_use = str.substr(0, posv[0]);
				l = to_use.find('_');
				while (l != string::npos) {
					to_use.replace(l, 1, " ");
					l = to_use.find('_');
				}
			}
			else if (posv.size() == 0)
				to_use = str;
			else
				to_use = str.substr(0, posv.back());

		}
		if (Units_Instead) {
			if (!Clean_Instead)
				m_CurrentFileInfo.Des.base.Unit[count] = to_use;
			else
				m_CurrentFileInfo.Des.base.Unit[count] = "";
		}
		else {
			if (!Clean_Instead) {
				if (posv.size() > 2) {
					m_CurrentFileInfo.Des.base.Id[count] = to_Id;
					m_CurrentFileInfo.Des.base.Name[count] = to_use;
					m_CurrentFileInfo.Des.base.Pos[count] = to_pos;
				}
				else
					m_CurrentFileInfo.Des.base.Name[count] = to_use;
			}
			else {
				m_CurrentFileInfo.Des.base.Name[count] = "";
				m_CurrentFileInfo.Des.base.Id[count] = "";
			}
		}
		count++;
	});

}

FILEINFO PGFileImport::GetFileInfo(FILEINFO input, size_t ActualIndexId) {
	char buf[10000];
	string datestring;
	m_totrec_a = input.TotRec-input.NumHeads;
	if (m_format == SEPARATED_YMDHM&&input.MaxVar>6) {
		input.RedNumVar = 5;
		input.NumHeads = 1;
		m_ImportedFileVector.push_back(m_CurrentFile);
	}
	else if (m_format == SEPARATED_YMD&&input.MaxVar>4) {
		input.RedNumVar = 3;
		m_ImportedFileVector.push_back(m_CurrentFile);
	}
	else if (m_format == YMD_HM&&input.MaxVar>4) {
		input.RedNumVar = 2;
		m_ImportedFileVector.push_back(m_CurrentFile);
	}
	else if (m_format == NOAA) {
		m_ImportedFileVector.push_back(m_CurrentFile);
		input.MaxVar = 11;
		input.NumHeads = 1;
		input.FixedFormat = true;
		size_t posfil;
		for (size_t i = 0; i<m_ImportedFileVector.size(); i++) {
			posfil = 140 * (m_ImportedFileVector[i].RecStart - 1) + 14;
			m_FileText.seekg(posfil, ios::beg);
			m_FileText.read((char*)&buf, 10);
			datestring.clear();
			for (size_t j = 0; j<4; j++)
				datestring += buf[j];
			datestring += "-";
			for (size_t j = 0; j<2; j++)
				datestring += buf[j + 4];
			datestring += "-";
			for (size_t j = 0; j<2; j++)
				datestring += buf[j + 6];
			datestring += " 12:00";
			m_ImportedFileVector[i].MinSt = PGUtil::MinutConv(datestring);
			posfil = 140 * (m_ImportedFileVector[i].RecStart - 2 + m_ImportedFileVector[i].NumRec) + 14;
			datestring.clear();
			m_FileText.seekg(posfil, ios::beg);
			m_FileText.read((char*)&buf, 10);
			for (size_t j = 0; j<4; j++)
				datestring += buf[j];
			datestring += "-";
			for (size_t j = 0; j<2; j++)
				datestring += buf[j + 4];
			datestring += "-";
			for (size_t j = 0; j<2; j++)
				datestring += buf[j + 6];
			datestring += " 12:00";
			m_ImportedFileVector[i].MinEnd = PGUtil::MinutConv(datestring);
			m_ImportedFileVector[i].NumRecCont = (m_ImportedFileVector[i].MinEnd - m_ImportedFileVector[i].MinSt) / 1440 + 1;
			if (i == 0) {
				m_minst_a = m_ImportedFileVector[i].MinSt;
				m_minend_a = m_ImportedFileVector[i].MinEnd;
			}
			else {
				m_minst_a = min(m_minst_a, m_ImportedFileVector[i].MinSt);
				m_minend_a = max(m_minend_a, m_ImportedFileVector[i].MinEnd);

			}
			m_totrec_a = (m_minend_a - m_minst_a) / 1440 + 1;
		}
		m_CreateMultiFile = true;
		input.TotRec = m_totrec_a + 1;
		if (m_CreateMultiFile)
			input.TotRec = m_totrec_a*m_ImportedFileVector.size() + 1;

	}
	else if (m_format == SMHI) {
		//SMHI file

		m_CurrentFile.NumRec = input.TotRec - 6;
		m_totrec_a = input.TotRec - 6;
		m_ImportedFileVector.push_back(m_CurrentFile);
		input.MaxVar = 6;
		input.NumHeads = 6;
		input.FixedFormat = true;
	}
	else if (m_format == PERRENIAL) {
		m_ImportedFileVector.push_back(m_CurrentFile);
		input.FixedFormat = false;
		m_CurrentFile.NumRec = input.TotRec - 1;
		m_totrec_a = input.TotRec - 1;
		//m_totrec_a=nrec;


	}
	else if (m_format == SWISS) {
		m_CurrentFile.NumRec = input.TotRec - 1;
		m_totrec_a = input.TotRec - 1;
		m_ImportedFileVector.push_back(m_CurrentFile);
		input.MaxVar = 8;
		input.NumHeads = 1;
		input.FixedFormat = false;
		input.RedNumVar = 3;
		//m_totrec_a=nrec;


	}
	else if (m_format == SEPARATED_YMDH) {
		

	}
	else if (m_format == CZECH) {
		input.RedNumVar = 3;
		input.NumHeads = 1;
		m_CurrentFile.NumRec = input.TotRec - 1;
		m_totrec_a = input.TotRec - 1;
		if (m_CurrentFile.StatNo.size()>0)
			m_ImportedFileVector.push_back(m_CurrentFile);
	}
	else if (m_format == CZECH_COL_TRANS) {
		input.RedNumVar = 0;
		input.NumHeads = 1;
		m_CurrentFile.NumRec = input.TotRec - 1;
		m_totrec_a = input.TotRec - 1;
		m_ImportedFileVector.push_back(m_CurrentFile);
	}
	else if (m_format == MONTH_COL_TRANS) {
		input.RedNumVar = 0;
		input.NumHeads = 1;
		m_CurrentFile.NumRec = input.TotRec - 1;
		m_totrec_a = input.TotRec - 1;
		m_ImportedFileVector.push_back(m_CurrentFile);
	}
	else if (m_format == PROFOUND) {
		m_CollectedFile = true;
		for (size_t i = 0; i < input.LinkedSites.size(); i++) {
			m_CurrentFile.NumRec = input.LinkedSites[i].Records-input.NumHeads;
			m_ImportedFileVector.push_back(m_CurrentFile);
		}
		if(input.LinkedSites.size()>0)
			m_totrec_a = m_CurrentFile.NumRec;


	}
	else if (m_format == STANDARD) {
		m_CollectedFile = false;
		
		for (size_t i = 0; i < input.LinkedSites.size(); i++) {
			m_CurrentFile.NumRec = input.LinkedSites[i].Records - input.NumHeads;
			m_ImportedFileVector.push_back(m_CurrentFile);
		}
		if (input.LinkedSites.size()>0)
			m_totrec_a = m_CurrentFile.NumRec;


	}
	else {
		m_CurrentFile.NumRec = input.TotRec - 1;
		m_totrec_a = input.TotRec - 1;
		m_ImportedFileVector.push_back(m_CurrentFile);

	}
	return input;


}
