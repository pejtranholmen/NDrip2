
#include "../../std.h"
#include "GetPFCurve.h"
#include "../../Util/FUtil.hpp"

#include "GausKreuger.h"
#include "../../Util/StatUtil.h"
#include <sys/stat.h>
#ifndef COUPSTD
#include "../../Util/MFC_Util.h"
#endif

//#include "..\..\Model\SOIL\PedoFunctions.h"
char buffer[84];

GetPFCurve::GetPFCurve(void)
{
	m_NumMeasured_PFSteps=0;
	m_ActProfile.KeyProf=0;
	m_ActProfile.NumLayers=0;
	m_Convertor=new GaussKreuger(0);
	m_TopRow=0;
	m_ConvCondUnit=240.;
}


GetPFCurve::~GetPFCurve(void)
{
	delete m_Convertor;
}
void GetPFCurve::ReadHeaderFile()
{
	m_HeaderFile.seekg(0,ios::end);
	auto n=m_HeaderFile.tellg();

	m_HeaderFile.seekg(0,ios::beg);
	m_HeaderFile.read(&buffer[0],NSizeHeader);
	short int imaxp;
	imaxp=*(int*)&buffer[0];
	size_t imaxpp=0;
	std::streamsize n2=0;
	if(m_NewHeaderFile.is_open()) {
		m_NewHeaderFile.seekg(0,ios::end);
		n2=m_NewHeaderFile.tellg();
		
		m_NewHeaderFile.seekg(0,ios::beg);
		if(n2>0)
			m_NewHeaderFile.read((char*)&imaxpp,4);


	}
	size_t count = 0;
	if (m_RwNewHeader.GetInputFileSize()==string::npos) {

		if (m_NewHeaderFile.is_open() && n2>0) { /// Reading New Header File
			m_ActProfile.County.resize(2);
			m_ActProfile.Country.resize(2);
			size_t numplace;
			auto koll = sizeof(unsigned int);
			int byte_toread = 48 + koll;
			while (m_NewHeaderFile.tellg()<n2&&count<imaxpp) {
				m_NewHeaderFile.read((char*)&m_ActProfile.KeyProf, 4);
				m_NewHeaderFile.read((char*)&numplace, 4);
				m_ActProfile.Name.resize(numplace);
				m_NewHeaderFile.read((char*)&m_ActProfile.Name[0], numplace);
				m_NewHeaderFile.read((char*)&m_ActProfile.County[0], 2);
				m_NewHeaderFile.read((char*)&m_ActProfile.Country[0], 2);
				m_NewHeaderFile.read((char*)&m_ActProfile.Lat, byte_toread);
				auto recp = m_Records.insert(pair<size_t, size_t>(m_ActProfile.KeyProf, m_ActProfile.RecLayers));
				if (recp.second == false) continue;
				m_HeaderRec.insert(pair<size_t, size_t>(m_ActProfile.KeyProf, count));
				m_HeaderPos.push_back(m_ActProfile.RecHeader);
				m_ActProfile.OldKey = FUtil::ItoNumAscii(m_ActProfile.KeyProf / 100);
				m_ActProfile.OldKey = m_ActProfile.OldKey.substr(2);
				m_ActProfile.OldKey += ":";
				string s;
				s = FUtil::ItoNumAscii(m_ActProfile.KeyProf % 100);
				s = s.substr(3);
				m_ActProfile.OldKey += s;
				m_AllProfiles.push_back(m_ActProfile);
				count++;
			}
			IsNewFormatFile = true;
			SaveNewFormat();
			return;
		}

	}

	if (m_RwNewHeader.IsFileInGood()) {
		m_AllProfiles.clear();

		m_Records.clear();
		m_HeaderRec.clear();
		size_t start = 0;
		m_RwNewHeader.SetGetPosition(&start);
		size_t  numprof, version;
		m_RwNewHeader.rsize_t(&version);
		m_RwNewHeader.rsize_t(&numprof);

		while (m_RwNewHeader.IsFileInGood() &&count<numprof) {
			ReadWrite_NewHeaderLine(true);
			auto recp = m_Records.insert(pair<size_t, size_t>(m_ActProfile.KeyProf, m_ActProfile.RecLayers));
			m_HeaderRec.insert(pair<size_t, size_t>(m_ActProfile.KeyProf, count));
			m_HeaderPos.push_back(m_ActProfile.RecHeader);

			m_ActProfile.OldKey = FUtil::ItoNumAscii(m_ActProfile.KeyProf / 100);
			m_ActProfile.OldKey = m_ActProfile.OldKey.substr(2);
			m_ActProfile.OldKey += ":";
			string s;
			s = FUtil::ItoNumAscii(m_ActProfile.KeyProf % 100);
			s = s.substr(3);
			m_ActProfile.OldKey += s;
			m_AllProfiles.push_back(m_ActProfile);
			count++;
		}
		IsNewFormatFile = true;
	}
	


	if(count==0)  {  //Reading Old Header File
		m_Records.clear();
		m_HeaderRec.clear();
		m_AllProfiles.clear();
		while(m_HeaderFile.tellg()<n) {
			m_HeaderFile.read(&buffer[0],NSizeHeader);
			string str;
			str.assign(&buffer[0],3);
			auto nump=FUtil::AtoInt(str);
			str.assign(&buffer[4],2);
			auto numr=FUtil::AtoInt(str);
			string place;
			place.assign(&buffer[6],16);
			string county;
			string country;
			county.assign(&buffer[22],3);
			country.assign(&buffer[25],2);
			string lat;
			lat.assign(&buffer[28],7);
			int ilat=FUtil::AtoInt(lat);
			string lon;
			lon.assign(&buffer[36],7);
			int ilong=FUtil::AtoInt(lon);
			float ptex[6];
			ptex[0]=*(float*)&buffer[44];
			ptex[1]=*(float*)&buffer[48];
			ptex[2]=*(float*)&buffer[52];
			ptex[3]=*(float*)&buffer[56];
			ptex[4]=*(float*)&buffer[60];
			ptex[5]=*(float*)&buffer[64];
			short int actnuml=*(short int*)&buffer[68];
			short int bldep=*(short int*)&buffer[70];
			short int year=*(short int*)&buffer[72];
			short int irecl=*(short int*)&buffer[74];
			string comfil;

			comfil.assign(&buffer[76],8);
			size_t key=GetKey(nump,numr);
			m_ActProfile.KeyProf=key;
			m_ActProfile.Name=place;
			//m_ActProfile.Lat=lat;
			//m_ActProfile.Long=lon;
			m_ActProfile.NumLayers=actnuml;
			m_ActProfile.Year=year;
			m_ActProfile.Country=country;
			m_ActProfile.County=county;   
			m_ActProfile.Upper_Clay=ptex[0];
			m_Records.insert(pair<size_t,size_t>(key, size_t(irecl)));
			m_HeaderRec.insert(pair<size_t, size_t>(key, count));
			count++;
		}
		IsNewFormatFile = false;
	}
//	int profnum=(str[0]);
}
bool GetPFCurve::SaveNewFormat()
{
	InitSaveRecord(m_Records.size());
	if(m_AllProfiles.size()==0) {
		for(size_t i=0; i<m_Records.size(); i++) {
			m_ActProfile=GetHeader(i+1,false);
			m_AllProfiles.push_back(m_ActProfile);
		}
	}
	auto  PrevProfile=m_ActProfile;
	for(size_t i=0; i<m_AllProfiles.size();i++) {
		m_ActProfile=m_AllProfiles[i];
		if(!SaveRecord(false)) return false; // Saving all profiles in a sequence
	}
	if(PrevProfile.KeyProf>0) m_ActProfile=PrevProfile;
	m_RwNewHeader.CloseFileOut();
	return true;
}
bool GetPFCurve::SaveRecord(bool New)
{
	if (!m_NewHeaderFile.good() || !m_NewHeaderFile.is_open()) {
		if (New) {
			m_NewHeaderFile.seekp(m_ActProfile.RecHeader, ios::beg);
			auto check = m_NewHeaderFile.tellp();
			if (m_NewHeaderFile.good()) {
				if ((unsigned int)check != m_ActProfile.RecHeader) return false;
				if (!UpdateHeaderFile()) return false;
			}
		}
		else {
			m_ActProfile.RecHeader = m_NewHeaderFile.tellp();
		}
		auto koll = sizeof(unsigned long);
		int byte_towrite = 48 + koll;
		if (m_NewHeaderFile.good()) {
			m_NewHeaderFile.write((char*)&m_ActProfile.KeyProf, 4);
			size_t numchar = m_ActProfile.Name.size();
			m_NewHeaderFile.write((char*)&numchar, 4);
			m_NewHeaderFile.write((char*)&m_ActProfile.Name[0], m_ActProfile.Name.size());
			m_NewHeaderFile.write((char*)&m_ActProfile.County[0], 2);
			m_NewHeaderFile.write((char*)&m_ActProfile.Country[0], 2);
			m_NewHeaderFile.write((char*)&m_ActProfile.Lat, byte_towrite);
		}
	}

	if (!New) {
		return ReadWrite_NewHeaderLine(false);

	}
	return true;

	//return ReadWrite_NewHeaderLine(false);
}
bool GetPFCurve::InitSaveRecord(size_t numprofile)
{
	if (m_RwNewHeader.IsFileInGood() || m_RwNewHeader.IsFileOutGood()) {
		m_RwNewHeader.wInt(&m_PF_FileVersion);
		m_RwNewHeader.wInt(&numprofile);
		return true;
	}
	if(!m_NewHeaderFile.good()) return false;
	m_NewHeaderFile.seekp(0, ios::beg);
	m_NewHeaderFile.write((char*)&numprofile,4);

	return true;
}
bool GetPFCurve::InitNewScan(bool Extra) {
	fstream *stream;
	if(Extra) 
		stream=&m_HeaderExtraFile;
	else 
		stream=&m_NewHeaderFile;


	if(!stream->good()) return false;
	stream->seekg(0,ios::beg);
	stream->read((char*)&m_scansize,4);
	stream->seekg(4, ios::beg);
	m_scancount=0;
	return true;

}

bool GetPFCurve::ScanNewHeaders(NEWHEADER *header, bool Extra) {

	int numplace;
	fstream *stream;
	if(Extra) 
		stream=&m_HeaderExtraFile;
	else 
		stream=&m_NewHeaderFile;




	if(!stream->good()||m_scancount==m_scansize) return false;
	auto koll = sizeof(unsigned long);
	int byte_toread = 48 + koll;
		stream->read((char*)&header->KeyProf,4);
		stream->read((char*)&numplace,4);
		header->Name.resize(numplace);
		stream->read((char*)&header->Name.at(0) ,numplace);
		stream->read((char*)&header->County.at(0) ,2);
		stream->read((char*)&header->Country.at(0) ,2);
		stream->read((char*)&header->Lat ,byte_toread);
		header->OldKey=FUtil::ItoNumAscii(header->KeyProf/100);
		header->OldKey+=":";
		header->OldKey+=FUtil::ItoNumAscii(header->KeyProf%100);
		m_scancount++;
		

	return stream->good();
}
NEWHEADER GetPFCurve::GetHeader(std::streampos rec, bool Extra) {
	return GetHeader(size_t(rec), Extra);
}
NEWHEADER GetPFCurve::GetHeader(size_t rec, bool Extra)
{
	fstream *stream;
	if(Extra) 
		stream=&m_HeaderExtraFile;
	else 
		stream=&m_NewHeaderFile;
	NEWHEADER ut;
	if(IsNewFormatFile&&m_AllProfiles.size()<rec) {
			m_ActProfile.County.resize(2);
			m_ActProfile.Country.resize(2);
			size_t numplace;
			stream->seekg(rec,ios::beg);
			stream->read((char*)&m_ActProfile.KeyProf,4);
			stream->read((char*)&numplace,4);
			m_ActProfile.Name.resize(numplace);
			stream->read((char*)&m_ActProfile.Name[0] ,numplace);
			stream->read((char*)&m_ActProfile.County[0] ,2);
			stream->read((char*)&m_ActProfile.Country[0] ,2);
			stream->read((char*)&m_ActProfile.Lat ,52);
			m_ActProfile.OldKey=FUtil::ItoNumAscii(m_ActProfile.KeyProf/100);
			m_ActProfile.OldKey+=":";
			m_ActProfile.OldKey+=FUtil::ItoNumAscii(m_ActProfile.KeyProf%100);
			ut=m_ActProfile;
	}
	else if(rec<m_AllProfiles.size()) {
		ut=m_AllProfiles[rec];

	}
	else {
		size_t recb;
		recb=rec*84;
		m_HeaderFile.seekg(recb,ios::beg);
		m_HeaderFile.read(&buffer[0],NSizeHeader);
		string str;
		str.assign(&buffer[0],3);
		auto nump=FUtil::AtoInt(str);
		str.assign(&buffer[4],2);
		auto numr=FUtil::AtoInt(str);
		string place;
		place.assign(&buffer[6],16);
		string county;
		string country;
		county.assign(&buffer[22],3);
		country.assign(&buffer[25],2);
		string lat;
		lat.assign(&buffer[28],7);
		int ilat=FUtil::AtoInt(lat);
		string lon;
		lon.assign(&buffer[36],7);
		int ilong=FUtil::AtoInt(lon);
		float ptex[6];
		ptex[0]=*(float*)&buffer[44];
		ptex[1]=*(float*)&buffer[48];
		ptex[2]=*(float*)&buffer[52];
		ptex[3]=*(float*)&buffer[56];
		ptex[4]=*(float*)&buffer[60];
		ptex[5]=*(float*)&buffer[64];
		short int actnuml=*(short int*)&buffer[68];
		short int bldep=*(short int*)&buffer[70];
		short int year=*(short int*)&buffer[72];
		short int irecl=*(short int*)&buffer[74];

		ut.KeyProf=GetKey(nump,numr);
		ut.Name=place;
		ut.Country=country;
		XY k;
		if(ilat>=0&&ilong<=0) 
			k.x=k.y=-1.1E38;
		else if (ilat < 1000) {
			k.y = ilong;
			k.x = ilat;

		}
		else if(ilat<1000000L&&ilong<1000000L)
			k=m_Convertor->grid_to_geodetic(double(ilat*100), double(ilong*100));
		else
			k=m_Convertor->grid_to_geodetic(double(ilat), double(ilong));
		ut.Lat=float(k.x);
		ut.Long=float(k.y);
		ut.County=county;
		ut.Upper_Clay=ptex[0];
		ut.Upper_Sand=ptex[1];
		ut.Upper_Org=ptex[2];
		ut.Lower_Clay=ptex[3];
		ut.Lower_Sand=ptex[4];
		ut.Lower_Org=ptex[5];
		ut.NumLayers=actnuml;
		ut.LowDepth=bldep;
		ut.Year=year;
		ut.RecLayers=irecl;
		ut.RecHeader=rec;
		m_ActProfile=ut;
	}
	return ut;
}
const size_t GetPFCurve::GetKey(int nump, int numr)
{
	return nump*100+numr;
}
const size_t GetPFCurve::GetNewKey(string str)
{
	size_t nump, numr;
	auto pos=str.find(":");
	if(pos!=string::npos) {
		nump=FUtil::AtoInt(str.substr(0,pos));
		str=str.substr(pos+1);
		numr=FUtil::AtoInt(str);
		return nump * 100 + numr;
	}
	return 101;
}
void GetPFCurve::ReadLevelFile()
{
	m_LevelFile.seekg(0,ios::end);
	auto n=m_LevelFile.tellg();
	m_LevelFile.seekg(0,ios::beg);
	vector<float> values;
	values.resize(37);
	m_LevelFile.read((char*)&values[0],NSizeLevel);

	
	while(m_LevelFile.tellg()<n) {
		m_LevelFile.read((char*)&values[0],NSizeLevel);


	}
//	int profnum=(str[0]);
}
vector<float> GetPFCurve::GetPF_Pressure(size_t rec)
{
	size_t start;
	start=(rec-1)*NSizeLevel;
	if(!m_LevelFile.good()) {
		ReOpenLevelFile();

	}
	m_LevelFile.seekg(start,ios::beg);
	vector<float> values;
	values.resize(12);
	m_LevelFile.read((char*)&values[0],48);

	size_t count=0;
	for(size_t i=0; i<values.size(); i++) { 
		if(values[i]>-1.E37) count++;
	};
	values.resize(count);
	m_NumMeasured_PFSteps=count;
	return values;

//	int profnum=(str[0]);
}
vector<float> GetPFCurve::GetPF_Theta(size_t rec)
{
	size_t start;
	start=(m_ActProfile.RecLayers)*NSizeLevel+(rec)*NSizeLevel+40;
	m_LevelFile.seekg(start,ios::beg);
	vector<float> values;
	values.resize(m_NumMeasured_PFSteps);
	if(values.size()>0)
		m_LevelFile.read((char*)&values[0],m_NumMeasured_PFSteps*4);

	return values;
//	int profnum=(str[0]);
}
PFCOEF GetPFCurve::GetPF_Coef(size_t rec)
{   PFCOEF ut;
	size_t start;
	vector<float> values;
	values.resize(37);

	start=(m_ActProfile.RecLayers)*NSizeLevel+rec*NSizeLevel;
	if(!m_LevelFile.good()) {
		ReOpenLevelFile();

	}
	m_LevelFile.seekg(0,ios::end);
	auto n_end=m_LevelFile.tellg();

	m_LevelFile.seekg(start,ios::beg);
	bool ReadPrevious=false;
	if(start>=n_end) {
		ReadPrevious=true;
		auto pos=n_end;
		pos-=NSizeLevel;
		m_LevelFile.seekg(pos, ios::beg);
		m_LevelFile.read((char*)&values[0],NSizeLevel);
		m_LevelFile.seekp(n_end,ios::beg);
		auto diff=values[1]-values[0];
		values[0]=values[1];
		values[1]=values[0]+diff;
		m_LevelFile.write((char*)&values[0], NSizeLevel);
		m_LevelFile.seekg(start,ios::beg);
	}

	m_LevelFile.read((char*)&values[0],148);
	ut.UpperDepth=values[0]/100;
	ut.LowerDepth=values[1]/100;
	ut.Mualem_n=values[2];
	ut.MatConductivity=values[3]*float(m_ConvCondUnit);
	ut.TotConductivity=values[9]*float(m_ConvCondUnit);
	ut.Lambda=values[4];
	ut.Residual=values[5];
	ut.Saturation=values[6];
	ut.AirEntry=values[7];
	ut.Wilting=values[8];
	ut.Gen_Alfa=values[22];
	ut.Gen_N=values[23];
	ut.Gen_M=values[24];
	ut.LowerBoundary=values[34];
	ut.UpperBoundary=values[33];
	ut.N_SrCoef=values[35];
	ut.N_SECoef=values[36];

	return ut;
//	int profnum=(str[0]);
}
vector<float> GetPFCurve::GetTexture(size_t rec)
{
	size_t start;
	start=(m_ActProfile.RecLayers)*NSizeLevel+rec*NSizeLevel+100;
	m_LevelFile.seekg(start,ios::beg);
	vector<float> values;
	values.resize(8);

	m_LevelFile.read((char*)&values[0],32);

	return values;
//	int profnum=(str[0]);
}
const size_t GetPFCurve::GetRecord(size_t key)
{ // Returns the Record in the Level File
	auto it=m_Records.find(key);
	if(it!=m_Records.end())
		return it->second;
	else
		return string::npos;

}
const size_t GetPFCurve::GetHeaderRecord(size_t key)
{ // Return the position in the Header File
	auto it=m_HeaderRec.find(key);
	if(it!=m_HeaderRec.end())
		return it->second;
	else
		return string::npos;

}
//const size_t GetPFCurve::GetHeader
const std::streampos GetPFCurve::GetInsertRecord(size_t rec) {
	if(rec<m_HeaderPos.size())
		return m_HeaderPos[rec];
	else if(m_HeaderRec.size()>0) {
		auto it=m_HeaderRec.begin();
		return it->second;
	}
	return string::npos;

}

BC_ESTIMATE GetPFCurve::Estimate_BC(vector<double>* Theta,vector<double>* Psi,  double ThetaMax, double FixResidual)
{
	BC_ESTIMATE out;

	  out.R2=0.;
	  out.n=Theta->size();
	  out.good=false;
	  
	  ThetaMax=max(5., ThetaMax);
	  double TheMin=30.;
	  vector<double> x, y;	
	  x.resize(Theta->size());
	  y.resize(Theta->size());

	  for(size_t i=0; i<Theta->size(); i++) {		
	    TheMin=min(Theta->at(i), TheMin);
		ThetaMax=max(ThetaMax, Theta->at(i));
		x[i]=log10(1./Psi->at(i));
	  }

      for(size_t i=0; i<100;i++) {
			double VIKT=double(i)/101.;
			double residual;
			if(FixResidual<=0.) {
				residual=VIKT*TheMin;
				residual=min(ThetaMax-5., residual);
			}
			else
				residual=FixResidual;
		for(size_t i=0; i<Theta->size();i++)
			y[i]=log10((Theta->at(i)-residual)/(ThetaMax-residual)+0.000001);
		STAT_PERF res;

		res=StatUtil::Get_StatComp(&x, &y);
		if(res.r2>out.R2) {
			out.R2=res.r2;
			out.Lambda=max(0.03f, res.slope);
			out.AirEntry=float(pow(10., res.intercept/out.Lambda));
			out.Residual=float(residual);
			out.good=true;
		}
	  }


return out;
}

BC_ESTIMATE GetPFCurve::Estimate_BC_Texture(double sand, double clay, double poro)
{
	BC_ESTIMATE out;
	out.R2=0;
	out.Lambda=float(FUtil::PedoFunc_Lambda(sand,clay, poro));
	out.AirEntry=float(FUtil::PedoFunc_AirEntry(sand,clay, poro));
	out.Residual=float(FUtil::PedoFunc_Residual(sand,clay, poro));
	out.good=true;
return out;
}

double GetPFCurve::Estimate_HydraulicCond_Texture(double sand, double clay, double poro) {
	return FUtil::PedoFunc_HydraulicCond(sand,clay, poro);
}
size_t GetPFCurve::SetComments(string kalle)
{	string comfilename, filbas;
	fstream fil;
	size_t nCount;

	comfilename=GetComFileName();
	fil.open(comfilename,ios::in|ios::out);
	if(!fil.good()) {
		fil.open(comfilename,ios::out);
	}
	if(fil.good()){
		nCount=kalle.size();
		if(nCount>0) {
			fil.write((char*)&kalle[0],kalle.size());
		}
		fil.close();
	}
	else {
		fil.clear();
		nCount=0;
	}
	return nCount;
}
vector<string> GetPFCurve::GetCommentsLines() {
	vector<string> outcom;
	string line = GetComments();
	while (line.size() > 0) {
		auto pos = line.find('\n');
		if (pos != string::npos) {
			outcom.push_back(line.substr(0, pos));
			line = line.substr(pos + 2);
			if (line[0] == '\r') line = line.substr(1);
		}
		else {
			outcom.push_back(line);
			line.resize(0);
		}
	}
	return outcom;
}
string GetPFCurve::GetComments()
{
	string comfilename, name2, filbas;
	//CFile comfile;
	//CFileException perror, perror2;

	string m_comdetails;

	bool copy;
	copy=false;
	long nCount;
	unsigned char buf[10000];

	comfilename=GetComFileName();
	ifstream comfile(comfilename);

	if(comfile.good()){
		comfile.seekg(0,ios::end);
		unsigned long long totsize=comfile.tellg();
		comfile.seekg(0,ios::beg);
		comfile.read((char*)&buf,totsize);

		for(int i=0;i<totsize;i++){
			if(buf[i]==0x8F) buf[i]= 0xE5; // å
			else if(buf[i]==0x84)	buf[i]=0xE4;		//ä
			else if(buf[i]==0x94)	buf[i]=0xF6;		//ö
			else if(buf[i]==0xF8)	buf[i]=0xC5;		//Å
			else if(buf[i]==0xE8)	buf[i]=0xC4;		//Ä
			else if(buf[i]==0x99)	buf[i]=0xD6;		//Ö
			if(buf[i]>=32&&buf[i]!=0xcc)
				m_comdetails+=buf[i];
			else if(buf[i]=='\n') {
				m_comdetails+='\n';m_comdetails+='\r';
			}
			else if(buf[i]=='\n'||buf[i]=='\r'||buf[i]=='\t')
				m_comdetails+=buf[i];
		}
		if(copy) {
			fstream file;
			file.open(comfilename,ios::out);
			file.write((char*)&buf,totsize);
			file.close();
		}

	}
	else {
		nCount=0;	
		m_comdetails="No Information";
	}
	
	return m_comdetails;

}


bool GetPFCurve::DeleteActHeader()
{
	size_t irec=GetHeaderRecord(m_ActProfile.KeyProf);
	if(irec==string::npos) return false;
	auto itr=m_Records.find(m_ActProfile.KeyProf);
	if(itr!=m_Records.end()) {
		m_Records.erase(itr);
	}
	/*auto ith=m_HeaderRec.find(m_ActProfile.KeyProf);
	if(ith!=m_HeaderRec.end()) {
		m_HeaderRec.erase(ith);
	}*/
	//Update second argument in Header map
	//scan all and check if second header is greater then irec - then minus 1

	

	m_AllProfiles.erase(m_AllProfiles.begin()+irec);

	m_HeaderRec.clear();
	for(size_t i=0; i<m_AllProfiles.size(); i++) {
		m_HeaderRec.insert(pair<size_t, size_t>(m_AllProfiles[i].KeyProf, i));
	}


	return true;
}
string GetPFCurve::GetComFileName() {
		string filbas=FUtil::ItoNumAscii(m_ActProfile.KeyProf/100);
		filbas=filbas.substr(3);
		filbas+="_";
		string repstr;
		repstr=FUtil::ItoNumAscii(m_ActProfile.KeyProf%100);
		repstr=repstr.substr(4);
		filbas+=repstr;
	    string directory=GetDataBaseDirectory();
	    return directory+"Comments\\"+filbas+".pfn";
}
bool GetPFCurve::UpdateHeaderFile() {
	// Needs to rewrite headerFile after m_ActProfile
	// Needs copy of previous file:
	
	auto retlevels=m_Records.insert(pair<size_t,size_t>(m_ActProfile.KeyProf, m_ActProfile.RecLayers));
	if(retlevels.second==false) {
		return false;
	}
	auto retheaders=m_HeaderRec.insert(pair<size_t, size_t>(m_ActProfile.KeyProf, m_AllProfiles.size()));
	if(retheaders.second==false) {
		return false;
	}

	m_AllProfiles.push_back(m_ActProfile);
	return true;

}

bool GetPFCurve::ExpandToNewLayerActHeader(size_t newnum_layers) {

	size_t prevlayrec=GetRecord(m_ActProfile.KeyProf);
	size_t prevlayer=m_ActProfile.NumLayers;
	vector< vector<float>> m_PrevCoef; 
	m_PrevCoef.resize(newnum_layers+1);
	for(size_t i=0; i<newnum_layers+1; i++) 
		m_PrevCoef[i].resize(37);
	m_LevelFile.seekg(m_ActProfile.RecLayers*NSizeLevel-NSizeLevel, ios::beg);
	if(newnum_layers>prevlayer) {
		for(size_t i=0; i<prevlayer+1; i++) {
			m_LevelFile.read((char*)&m_PrevCoef[i][0], NSizeLevel);
		}
	}
	m_LevelFile.seekg(0,ios::end);
	auto n_end=m_LevelFile.tellg();
	m_LevelFile.seekp(n_end,ios::beg);
	if(newnum_layers>prevlayer) {
		for(size_t i=0; i<prevlayer+1; i++) {
			m_LevelFile.write((char*)&m_PrevCoef[i][0], NSizeLevel); 
		}

		auto diff=m_PrevCoef[prevlayer][1]-m_PrevCoef[prevlayer][0];

		for(size_t i=0; i<newnum_layers-prevlayer;i++) {
			m_PrevCoef[prevlayer+i+1][0]=m_PrevCoef[prevlayer+i][1];
			m_PrevCoef[prevlayer+i+1][1]=m_PrevCoef[prevlayer+i+1][0]+diff;
			for(size_t ii=2;ii<37;ii++) m_PrevCoef[prevlayer+i+1][ii]=m_PrevCoef[prevlayer][ii];
			m_LevelFile.write((char*)&m_PrevCoef[prevlayer+1+i][0], NSizeLevel); 
		}
		m_ActProfile.RecLayers=size_t(n_end/NSizeLevel+1);
		auto it=m_Records.find(m_ActProfile.KeyProf);
		if(it!=m_Records.end()) {
			m_Records.erase(it);
		}
		m_Records.insert(pair<size_t,size_t>(m_ActProfile.KeyProf,  m_ActProfile.RecLayers));
	}
	return true;
}
bool GetPFCurve::ExportMainDataBasetoCSVFile()
{
	ofstream fileout("file.csv");

	return false;
}


