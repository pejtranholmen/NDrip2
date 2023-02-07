#ifndef COUPSTD
#include <iostream>
#endif
#include "../../std.h"
#include "PFCurve.h"
#include "GausKreuger.h"
#ifndef COUPSTD
#include "../../Util/MFC_Util.h"
#endif
#include "../../Util/FUtil.hpp"
#include "./GetPFCurve.h"
using namespace std;


PFCurve::PFCurve(void)
{
	m_IniPressure.resize(6);
	m_IniPressure[0]=10000.f;
	m_IniPressure[1]=1000.f;
	m_IniPressure[2]=500.f;
	m_IniPressure[3]=100.f;
	m_IniPressure[4]=50.f;
	m_IniPressure[5]=10.f;
	m_theta_ini.resize(6);
	m_theta_ini[0]=6.4f;
	m_theta_ini[1]=14.f;
	m_theta_ini[2]=22.f;
	m_theta_ini[3]=34.f;
	m_theta_ini[4]=40.f;
	m_theta_ini[5]=52.f;
	m_texture_ini.resize(8);
	m_texture_ini[0]=7.f;
	m_texture_ini[1]=10.f;
	m_texture_ini[2]=13.f;
	m_texture_ini[3]=16.f;
	m_texture_ini[4]=49.f;
	m_texture_ini[5]=80.f;
	m_texture_ini[6]=97.f;
	m_texture_ini[7]=3.f;
	m_IniHeader.Country="  ";
	m_IniHeader.County="  ";
	m_IniHeader.Name="MySite";
	m_IniHeader.LowDepth=100;
	m_IniHeader.Long=0.0;
	m_IniHeader.Lat=0.0;
	m_IniHeader.Upper_Clay=7.;
	m_IniHeader.Upper_Sand=80.;
	m_IniHeader.Upper_Org=3.;
	m_IniHeader.Lower_Clay=7.;
	m_IniHeader.Lower_Sand=80.;
	m_IniHeader.Lower_Org=3.;
	m_IniHeader.Year=2014;
	m_IniHeader.KeyProf=30501;
	m_IniHeader.NumLayers=3;
	m_pfcoef_ini.AirEntry=13.5;
	m_pfcoef_ini.Lambda=0.325f;
	m_pfcoef_ini.Saturation=54.7f;
	m_pfcoef_ini.Residual=4.f;
	m_pfcoef_ini.MatConductivity=100.f;
	m_pfcoef_ini.TotConductivity=10000.f;
	m_pfcoef_ini.Wilting=2.f;
	m_pfcoef_ini.Gen_Alfa=1/m_pfcoef_ini.AirEntry;
	m_pfcoef_ini.Gen_M=0.f;
	m_pfcoef_ini.Gen_N=0.f;
	m_pfcoef_ini.Mualem_n=1.f;
	m_pfcoef_ini.UpperBoundary=10000.f;
	m_pfcoef_ini.LowerBoundary=4.f;
	m_pfcoef_ini.N_SrCoef=8.f;
	m_pfcoef_ini.N_SECoef=8.f;
	m_ActProfile=m_IniHeader;
	//MySingle::instance().SetPlotPFPointer(this);
}


PFCurve::~PFCurve(void)
{
}
bool PFCurve::Set_Header(NEWHEADER head, bool Extra)
{
	if(!IsMainEditOpen&&!Extra) return false;

	auto start=GetHeaderRecord(head.KeyProf);
	//start=(m_ActProfile.RecLayers-1)*NSizeHeader;
	m_NewHeaderFile.seekp(start,ios::beg);
	auto actpos=m_NewHeaderFile.tellp();
	if(actpos==std::streampos(start)&&start!=string::npos) {
		m_ActProfile=head;
		return SaveRecord(false);
	//	m_NewHeaderFile.write((char*)&head,100);

	}
	else {

		m_NewHeaderFile.seekp(0,ios::end);
		auto endpos=m_NewHeaderFile.tellp();
		head.RecHeader=endpos;
		m_LevelFile.seekp(0,ios::end);
		endpos=m_LevelFile.tellp();
		head.RecLayers=size_t(endpos/NSizeLevel);
		m_ActProfile=head;
		if(!SaveRecord(true)) return false;
		if(!SetPF_Pressure(m_IniPressure)) return false;
		float thick=0.1f;
		m_pfcoef_ini.UpperDepth=0.f;
		m_pfcoef_ini.LowerDepth=0.1f;

		for(size_t i=0; i<m_ActProfile.NumLayers;i++) {
			if(!SetPF_Coef(i, m_pfcoef_ini)) return false;
			if(!SetPF_Theta(i, m_theta_ini)) return false;
			if(!SetPF_Texture(i, m_texture_ini)) return false;

			
			m_pfcoef_ini.UpperDepth+=thick;
			m_pfcoef_ini.LowerDepth+=thick;
		}
		//m_LevelFile.write((char*)&m_IniPressure[0], m_IniPressure.size()*4);

		return true;



	}



}

bool PFCurve::SetPF_Pressure(vector<float> pressure, bool Extra)
{
	if(!IsMainEditOpen&&!Extra) return false;
	size_t start;
	start=(m_ActProfile.RecLayers-1)*NSizeLevel;


	m_LevelFile.seekp(start,ios::beg);
	auto checkpos=m_LevelFile.tellp();
	float Missing=-1.1E38f;
	if (pressure.size() > 0) {
		m_LevelFile.write((char*)&pressure[0], std::streamsize(pressure.size()) * 4);
		for (size_t i = 0; i < (37 - pressure.size()); i++)
			m_LevelFile.write((char*)&Missing, 4);
		checkpos = m_LevelFile.tellp();
	}

	return m_LevelFile.good();
}
bool PFCurve::SetPF_Theta(size_t row, vector<float> theta, bool Extra)
{
	if(!IsMainEditOpen&&!Extra) return false;
	size_t start;
	start=(m_ActProfile.RecLayers)*NSizeLevel+(row)*NSizeLevel+40;


	m_LevelFile.seekp(start,ios::beg);
	m_LevelFile.write((char*)&theta[0], std::streamsize(theta.size())*4);

	return m_LevelFile.good();



};
bool PFCurve::SetPF_Texture(size_t row, vector<float> texture, bool Extra)
{
	if(!IsMainEditOpen&&!Extra) return false;
	size_t start;
	start=(m_ActProfile.RecLayers)*NSizeLevel+(row)*NSizeLevel+100;


	m_LevelFile.seekp(start,ios::beg);
	m_LevelFile.write((char*)&texture[0], std::streamsize(texture.size())*4);

	return m_LevelFile.good();



};
bool PFCurve::SetPF_Coef(size_t row, PFCOEF pfcoef, bool Extra)
{

	size_t start;
	start=(m_ActProfile.RecLayers)*NSizeLevel+(row)*NSizeLevel;
	m_LevelFile.seekg(0,ios::end);
	auto end=m_LevelFile.tellg();
	m_LevelFile.seekg(start,ios::beg);
	auto koll=m_LevelFile.tellg();
	if(koll!=std::streampos(start)) {
		OpenEditMainDataBase();
		ReOpenLevelFile();
		m_LevelFile.seekg(start,ios::beg);
		end=m_LevelFile.tellg();
	}
	vector<float> values;
	values.resize(37);
	if(end>=(std::streampos(start)+ std::streampos(NSizeLevel))) m_LevelFile.read((char*)&values[0],NSizeLevel);

		values[0]=pfcoef.UpperDepth*100;
		values[1]=pfcoef.LowerDepth*100;
		values[2]=pfcoef.Mualem_n;
		values[3]=float(pfcoef.MatConductivity/m_ConvCondUnit);
		values[9]=float(pfcoef.TotConductivity/m_ConvCondUnit);
		values[4]=pfcoef.Lambda;
		values[5]=pfcoef.Residual;
		values[6]=pfcoef.Saturation;
		values[7]=pfcoef.AirEntry;
		values[8]=pfcoef.Wilting;
		values[22]=pfcoef.Gen_Alfa;
		values[23]=pfcoef.Gen_N;
		values[24]=pfcoef.Gen_M;
		values[34]=pfcoef.LowerBoundary;
		values[33]=pfcoef.UpperBoundary;
		values[35]=pfcoef.N_SrCoef;
		values[36]=pfcoef.N_SECoef;

	if(!IsMainEditOpen&&!Extra) return false;

	start=(m_ActProfile.RecLayers)*NSizeLevel+(row)*NSizeLevel;


	m_LevelFile.seekp(start,ios::beg);
	m_LevelFile.write((char*)&values[0],148);

	return m_LevelFile.good();
};
NEWHEADER PFCurve::GetNewHeader() {

	//size_t start, actpos, endpos;
	m_IniHeader.CreateDate = FUtil::GetCurrentDateTime();
	m_IniHeader.CreatedBy = FUtil::GetWindowUser();
	auto start=GetHeaderRecord(m_IniHeader.KeyProf);
	while(start!=string::npos) {
		m_IniHeader.KeyProf++;
		start=GetHeaderRecord(m_IniHeader.KeyProf);
	}
	return m_IniHeader;
}
bool PFCurve::AddNewProfile(bool Extra) {

	//m_Records.push_back(m_ActProfile.RecLayers);


	return true;
}
bool PFCurve::DeleteProfile(size_t row, bool Extra) {
	if(row<m_AllProfiles.size()) {
		NEWHEADER kill;
		kill=m_AllProfiles[row];
		m_AllProfiles.erase(m_AllProfiles.begin()+row);
		m_HeaderRec.erase(kill.KeyProf);
		m_Records.erase(kill.KeyProf);
		return true;
	}
	return false;
}
bool PFCurve::UpdateActualProfile() {
	size_t iheader=GetHeaderRecord(m_ActProfile.KeyProf);

	if(iheader<m_AllProfiles.size()&&m_AllProfiles.size()>0) {
		m_AllProfiles[iheader]=m_ActProfile;
		return true;
	}
	return false;
}
bool PFCurve::CleanMainDataBase() {

	return true;
}
bool PFCurve::Export_Import_ActualProfile_CSVFile(size_t keyno, bool Export)
{
	string filename;
	string line;
	string item;
#ifndef COUPSTD
	filename = MFC_Util::GetCSVFileName();
#else
	filename = "Kalle.csv";
#endif

	//filename = FUtil::STD_ItoAscii(keyno) + ".csv";

	ofstream fileout;
	ifstream filein;
	if (Export) {
		fileout.open(filename);

	}
	else {

		filein.open(filename);
	}
	char delim = ';';

	auto WriteItem = [&](string item, string &value) {
		if (Export) {
			line = item + ';';
			line += value + '\n';
			fileout.write((char*)&line[0], line.size());
		}
		else {
			getline(filein, line);
			if (item == line.substr(0, line.find(delim))) {
				value = line.substr(line.find(delim) + 1);
				auto pos = value.find(delim);
				if (pos != string::npos) value = value.substr(0, pos);
			}
		}
	};
	auto WriteFloatItem = [&](string item, float &value) {
		if (Export) {
			line = item + ';';
			line += FUtil::STD_FtoAscii(value) + '\n';
			fileout.write((char*)&line[0], line.size());
		}
		else {
			getline(filein, line);
			if (item == line.substr(0, line.find(delim))) {
				value = FUtil::AtoFloat(line.substr(line.find(delim) + 1));
			}
		}
	};
	auto WriteKeyItem = [&](size_t key) {
		line = "Prof no:/Rep No:;";
		line += FUtil::STD_ItoAscii(key / 100) + ';';
		line += FUtil::STD_ItoAscii(key % 100) + '\n';
		fileout.write((char*)&line[0], line.size());
	};
	auto WriteIntItem = [&](string item, size_t &value) {
		if (Export) {
			line = item + ';';
			line += FUtil::STD_ItoAscii(int(value)) + '\n';
			fileout.write((char*)&line[0], line.size());
		}
		else {
			getline(filein, line);
			if (item == line.substr(0, line.find(delim))) {
				value = FUtil::AtoInt(line.substr(line.find(delim) + 1));
			}


		}
	};
	auto WritePFCOEF = [&](PFCOEF &pfcoef) {
		if (Export) {
			line = FUtil::STD_FtoAscii(pfcoef.UpperDepth) + ';';
			line += FUtil::STD_FtoAscii(pfcoef.LowerDepth) + ';';
			line += FUtil::STD_FtoAscii(pfcoef.Lambda) + ';';
			line += FUtil::STD_FtoAscii(pfcoef.AirEntry) + ';';
			line += FUtil::STD_FtoAscii(pfcoef.Saturation) + ';';
			line += FUtil::STD_FtoAscii(pfcoef.Wilting) + ';';
			line += FUtil::STD_FtoAscii(pfcoef.Residual) + ';';
			line += FUtil::STD_FtoAscii(pfcoef.LowerBoundary) + ';';
			line += FUtil::STD_FtoAscii(pfcoef.UpperBoundary) + ';';
			line += FUtil::STD_FtoAscii(pfcoef.Gen_M) + ';';
			line += FUtil::STD_FtoAscii(pfcoef.Gen_N) + ';';
			line += FUtil::STD_FtoAscii(pfcoef.Gen_Alfa) + ';';
			line += FUtil::STD_FtoAscii(pfcoef.MatConductivity) + ';';
			line += FUtil::STD_FtoAscii(pfcoef.TotConductivity) + ';';
			line += FUtil::STD_FtoAscii(pfcoef.Mualem_n) + ';';
			line = line.substr(0, line.size() - 1);
			line += '\n';
			fileout.write((char*)&line[0], line.size());
		}
		else {
			getline(filein, line);
			auto pos = line.find(delim);
			string valuestr;
			valuestr = line.substr(0, pos);
			pfcoef.UpperDepth = FUtil::AtoFloat(valuestr);
			line = line.substr(pos + 1); pos = line.find(delim);	valuestr = line.substr(0, pos);
			pfcoef.LowerDepth = FUtil::AtoFloat(valuestr);
			line = line.substr(pos + 1); pos = line.find(delim);	valuestr = line.substr(0, pos);
			pfcoef.Lambda = FUtil::AtoFloat(valuestr);
			line = line.substr(pos + 1); pos = line.find(delim);	valuestr = line.substr(0, pos);
			pfcoef.AirEntry = FUtil::AtoFloat(valuestr);
			line = line.substr(pos + 1); pos = line.find(delim);	valuestr = line.substr(0, pos);
			pfcoef.Saturation = FUtil::AtoFloat(valuestr);
			line = line.substr(pos + 1); pos = line.find(delim);	valuestr = line.substr(0, pos);
			pfcoef.Wilting = FUtil::AtoFloat(valuestr);
			line = line.substr(pos + 1); pos = line.find(delim);	valuestr = line.substr(0, pos);
			pfcoef.Residual = FUtil::AtoFloat(valuestr);
			line = line.substr(pos + 1); pos = line.find(delim);	valuestr = line.substr(0, pos);
			pfcoef.LowerBoundary = FUtil::AtoFloat(valuestr);
			line = line.substr(pos + 1); pos = line.find(delim);	valuestr = line.substr(0, pos);
			pfcoef.UpperBoundary = FUtil::AtoFloat(valuestr);
			line = line.substr(pos + 1); pos = line.find(delim);	valuestr = line.substr(0, pos);
			pfcoef.Gen_M = FUtil::AtoFloat(valuestr);
			line = line.substr(pos + 1); pos = line.find(delim);	valuestr = line.substr(0, pos);
			pfcoef.Gen_N = FUtil::AtoFloat(valuestr);
			line = line.substr(pos + 1); pos = line.find(delim);	valuestr = line.substr(0, pos);
			pfcoef.Gen_Alfa = FUtil::AtoFloat(valuestr);
			line = line.substr(pos + 1); pos = line.find(delim);	valuestr = line.substr(0, pos);
			pfcoef.MatConductivity = FUtil::AtoFloat(valuestr);
			line = line.substr(pos + 1); pos = line.find(delim);	valuestr = line.substr(0, pos);
			pfcoef.TotConductivity = FUtil::AtoFloat(valuestr);
			valuestr = line.substr(pos + 1);
			pfcoef.Mualem_n = FUtil::AtoFloat(valuestr);
		}
	};
	auto WritePFCOEFHeading = [&]() {
		if (Export) {
			line = "Upper Depth; Lower Depth; Lambda;Air Entry;Saturation;Wilting Point;Residual; LowerBoundary; UpperBoundary; Gen M; Gen n; Gen Alfa; Mat Conductivity; Tot Conductivity;Mualem n";
			line += '\n';
			fileout.write((char*)&line[0], line.size());
		}
		else {
			getline(filein, line);


		}
	};
	auto WriteTextureHeading = [&]() {
		if (Export) {
			line = "Upper Depth; Lower Depth;<0.002;0.002-0.006;0.006-0.02;0.02-0.06;0.06-0.2;0.2-0.6;0.6-2.0; Org";
			line += '\n';
			fileout.write((char*)&line[0], line.size());
		}
		else {
			getline(filein, line);

		}
	};
	auto WritePressureHeading = [&](vector<float> &Pressure) {
		if (Export) {
			line = "Number of Measured Water Retention Points;" + FUtil::STD_ItoAscii(Pressure.size()) + '\n';
			line += "Upper Depth; Lower Depth;";
			for (size_t i = 0; i < Pressure.size(); i++) {
				line += FUtil::STD_FtoAscii(Pressure[i]) + ";";
			}
			line = line.substr(0, line.size() - 1);
			line += '\n';
			fileout.write((char*)&line[0], line.size());
		}
		else {
			getline(filein, line);
			auto pos = line.find(delim);
			line = line.substr(pos + 1);
			string valuestr;
			pos = line.find(delim);
			if (pos != string::npos)
				valuestr = line.substr(0, pos);
			else
				valuestr = line;
			string str;
			size_t numpressure = FUtil::AtoInt(valuestr);
			Pressure.resize(numpressure);
			getline(filein, line);
			pos = line.find(delim);
			line = line.substr(pos+1);
			pos = line.find(delim);
			line = line.substr(pos+1);
			pos = line.find(delim);
			for (size_t i = 0; i<numpressure; i++) {
				if (pos != string::npos)
					valuestr = line.substr(0, pos);
				else
					valuestr = line;
				Pressure[i] = FUtil::AtoFloat(valuestr);
				line = line.substr(pos + 1);
				pos = line.find(delim);
			}
			SetPF_Pressure(Pressure);
		}
	};
	auto WriteFloatVector = [&](vector<float> &floatv) {
		if (Export) {
			line.clear();
			for (size_t i = 0; i < floatv.size(); i++) {
				line += FUtil::STD_FtoAscii(floatv[i]) + ';';
			}
			line = line.substr(0, line.size() - 1);
			line += '\n';
			fileout.write((char*)&line[0], line.size());
		}
		else {
			getline(filein, line);
			floatv = FUtil::GetFloatVectorFromString(line, floatv.size());
		}
	};
	auto WriteComments = [&]() {
		if (Export) {
			vector<string> outcom = GetCommentsLines();
			auto numcomline = outcom.size();
			WriteIntItem("Number of Comments:", numcomline);
			for (size_t i = 0; i < outcom.size(); i++) {
				WriteItem("Comments(" + FUtil::STD_ItoAscii(i + 1) + ")", outcom.at(i));
			}
		}
		else {
			size_t numcomline=0;
			WriteIntItem("Number of Comments:", numcomline);
			string str;
			string add;
			for (size_t i = 0; i < numcomline; i++) {
				WriteItem("Comments(" + FUtil::STD_ItoAscii(i + 1) + ")", add);
				str += add + '\n';
			}
			str = str.substr(0, str.size() - 1);
			SetComments(str);
		}
	};
	NEWHEADER header;
	bool NewHeaderProfile = false;
	if (Export) {
		header = GetHeader(GetHeaderRecord(keyno));
		WriteKeyItem(header.KeyProf);
		if (!fileout.good()) return false;
	}
	else {
		getline(filein, line);
		auto pos = line.find(delim);
		line = line.substr(pos + 1);
		pos = line.find(delim);
		string str = line.substr(0, pos);
		int iprof = FUtil::AtoInt(str);
		line = line.substr(pos + 1);
		pos = line.find(delim);
		if (pos == string::npos)
			str = line;
		else
			str = line.substr(0, pos);
		int irep = FUtil::AtoInt(str);
		size_t newkey = GetKey(iprof, irep);

		auto startrec = GetHeaderRecord(newkey);



		if (startrec != string::npos) {

			header = GetHeader(startrec);
			header.RecHeader = startrec;


		}

		else {
			NEWHEADER nheader;
			NewHeaderProfile = true;
			header = m_IniHeader;
			header.KeyProf = newkey;
			header.RecHeader = m_AllProfiles.size();
			m_LevelFile.seekp(0, ios::end);
			auto endpos = m_LevelFile.tellp();
			header.RecLayers= size_t(endpos / NSizeLevel)+1;

		}
		if (!filein.good()) return false;
	}
	m_ActProfile.RecLayers = header.RecLayers;

	WriteItem("Name", header.Name);
	WriteItem("County", header.County);
	WriteItem("Country", header.Country);
	WriteFloatItem("Latitude", header.Lat);
	WriteFloatItem("Longitude", header.Long);
	WriteFloatItem("Upper_Clay", header.Upper_Clay);
	WriteFloatItem("Upper_Sand", header.Upper_Sand);
	WriteFloatItem("Upper_Org", header.Upper_Org);
	WriteFloatItem("Lower_Clay", header.Lower_Clay);
	WriteFloatItem("Lower_Sand", header.Lower_Sand);
	WriteFloatItem("Lower_Org", header.Lower_Org);
	WriteIntItem("Year", header.Year);
	WriteIntItem("NumLayers", header.NumLayers);
	WriteItem("Created By", header.CreatedBy);
	WriteItem("Date Created", header.CreateDate);
	
	WriteItem("Modified By", header.ModifiedBy);
	WriteItem("Date Modified", header.ModifiedDate);
	if (!Export) {
		header.ModifiedBy = FUtil::GetWindowUser();
		header.ModifiedDate = FUtil::GetCurrentDateTime();
		m_ActProfile = header;

		if(m_AllProfiles.size()> header.RecHeader&&!NewHeaderProfile) m_AllProfiles[header.RecHeader] = m_ActProfile;

		if (NewHeaderProfile) {
			if (!UpdateHeaderFile()) return false;
			if (SaveNewFormat());
			if (!SetPF_Pressure(m_IniPressure)) return false;
			float thick = 0.1f;
			m_pfcoef_ini.UpperDepth = 0.f;
			m_pfcoef_ini.LowerDepth = 0.1f;

			for (size_t i = 0; i < m_ActProfile.NumLayers; i++) {
				if (!SetPF_Coef(i, m_pfcoef_ini)) return false;
				if (!SetPF_Theta(i, m_theta_ini)) return false;
				if (!SetPF_Texture(i, m_texture_ini)) return false;


				m_pfcoef_ini.UpperDepth += thick;
				m_pfcoef_ini.LowerDepth += thick;
			}
		}

		if (SaveNewFormat());
		m_ActProfile = header;

	}
	WriteComments();
	WritePFCOEFHeading();
	for (size_t i = 0; i < header.NumLayers; i++) {
		PFCOEF pfcoef;
		pfcoef = GetPF_Coef(i);
		WritePFCOEF(pfcoef);
		SetPF_Coef(i, pfcoef);
	}
	WriteTextureHeading();
	for (size_t i = 0; i < header.NumLayers; i++) {
		PFCOEF pfcoef;
		pfcoef = GetPF_Coef(i);
		vector<float> AllValues;
		vector<float> Texture = GetTexture(i);
		pfcoef = GetPF_Coef(i);
		AllValues.resize(0);
		AllValues.push_back(pfcoef.UpperDepth);
		AllValues.push_back(pfcoef.LowerDepth);
		for (size_t j = 0; j < Texture.size(); j++) {
			if (j == 0 || j == 7) AllValues.push_back(Texture[j]);
			else if (j < 6) {
				if (Texture[j] < 0.9*MISSING) AllValues.push_back(MISSING);
				else AllValues.push_back(Texture[j] - Texture[j - 1]);
			}
			else {
				if (Texture[7] < 0.9*MISSING) AllValues.push_back(MISSING);
				else AllValues.push_back(100 - Texture[7] - Texture[5]);
			}
		}
		WriteFloatVector(AllValues);
		if (!Export) {
			Texture[0] = AllValues[2];
			for (size_t j = 1; j < Texture.size()-2; j++) {
				Texture[j]= Texture[j-1]+ AllValues[j+2];
			}
			Texture[7] = AllValues[9];
			Texture[6] = 100 - AllValues[9] - AllValues[7];
			SetPF_Texture(i, Texture);
		}



	}

	vector<float> Pressure = GetPF_Pressure(m_ActProfile.RecLayers);
	WritePressureHeading(Pressure);
    WritePressureHeading(Pressure);
//	WritePressureHeading(GetPF_Pressure(m_ActProfile.RecLayers));


	for (size_t i = 0; i < header.NumLayers; i++) {
		PFCOEF pfcoef;
		vector<float> ThetaValues = GetPF_Theta(i);
		vector<float> AllValues;
		pfcoef = GetPF_Coef(i);
		AllValues.resize(0);
		AllValues.push_back(pfcoef.UpperDepth);
		AllValues.push_back(pfcoef.LowerDepth);
		for (size_t j = 0; j < Pressure.size(); j++) {
			if (ThetaValues[j] <= 0.001) ThetaValues[j] = MISSING;
			AllValues.push_back(ThetaValues[j]);
		}
		WriteFloatVector(AllValues);
		if (!Export) {
			ThetaValues.clear();
			for (size_t j = 0; j < Pressure.size(); j++) {			
				ThetaValues.push_back(AllValues[j+2]);
				if (ThetaValues[j] <= 0.001) ThetaValues[j] = MISSING;
			}
			SetPF_Theta(i, ThetaValues);
		}

	}

	return true;
}
/*
#ifndef COUPS
int main() {
	PFCurve kalle;

	if(kalle.OpenMainFile("c:\\test\\pfprof")) {
		kalle.ReadLevelFile();
		kalle.ReadHeaderFile();

		vector<float> pressure;

		kalle.OpenEditMainDataBase();
		kalle.SaveNewFormat();
	/*	size_t ikey=kalle.GetRecord(fkey);
		size_t iheader=kalle.GetHeaderRecord(fkey);
		NEWHEADER ut=kalle.GetHeader(iheader);
		pressure=kalle.GetPF_Pressure(ikey);
		pressure[2]=339;
		bool test=kalle.SetPF_Pressure(pressure);
		pressure=kalle.GetPF_Pressure(ikey);
		vector<float> theta;
		for(size_t i=0; i<kalle.m_ActProfile.NumLayers; i++)
		pressure=kalle.GetPF_Theta(i);

		PFCOEF pf;
		for(size_t i=0; i<kalle.m_ActProfile.NumLayers; i++)
		pf=kalle.GetPF_Coef(i);

		kalle.CloseMainDataBase();
		
	}
	string k="";
	cout<< k;
	exit;

}
*/
