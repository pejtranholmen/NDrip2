#include "../../std.h"
#include "../../Util/FUtil.hpp"
#include "./PGFileImportBase.h"



PGFileImportBase::PGFileImportBase()
{
	m_PreviousMin = string::npos;
	m_CurrentMinStep = string::npos;
}


PGFileImportBase::~PGFileImportBase()
{
}
size_t PGFileImportBase::GetStartMinute()
{
	//size_t itimbase=GetLongTimeStart();

	return m_itimbase;

}

bool PGFileImportBase::ConvColLine(vector<float> *vv,string line, size_t option) {
//	Converting to Numbers for a fixed formatted file
// varstr is the substring of the line that hold the full line info
	string varstr;
	if(m_format==CZECH_COL_TRANS) {
		string newstr;
		//ConvLine(vv, line, m_format);
		for(size_t i=0; i<line.size(); i++) {
			if(line[i]!='\0'&&line[i]>0) newstr.push_back(line[i]);
		}
		auto npos=newstr.find(m_delimiters.substr(m_ndelim,1));
		if(string::npos==npos||npos==0) {
			return false;

		}
		varstr="";
	
		npos=newstr.find(m_delimiters.substr(m_ndelim,1));
		newstr = newstr.substr(npos + 1);
		npos = newstr.find(m_delimiters.substr(m_ndelim, 1));
		varstr=newstr.substr(0,npos);
		
		
		varstr+="-01-01 12:00";
		m_MinStartCol=PGUtil::MinutConv(varstr,false,false);

		newstr=newstr.substr(npos+1);
		size_t i=0;
		vv->at(i)=*(float*)&m_MinStartCol;	
		return true;
	}
	return false;
}
bool PGFileImportBase::ConvLine(vector<float> *vv,string line, size_t option) {
//	Converting to Numbers for a fixed formatted file
// varstr is the substring of the line that hold the full line info
	string varstr;
	string newstr;
	size_t index_of_string=0;

	auto CleanLineFromNoise = [&](string line) {
		string newstr;
		for (size_t i = 0; i<line.size(); i++) {
			if (line[i] != '\0'&&line[i]>0 && line[i] != 34) newstr.push_back(line[i]);
		}
		return newstr;
	};
	auto SetDateFromLine=[&](string newstr){

		auto npos = newstr.find(m_delimiters.substr(m_ndelim, 1));
		
		auto nhourcolumn = newstr.rfind(':');

		varstr = newstr.substr(0, npos);

		if (nhourcolumn > npos&&nhourcolumn!=string::npos) {
			newstr = newstr.substr(npos + 1);
			npos = newstr.find(m_delimiters.substr(m_ndelim, 1));
			varstr += ' ' + newstr.substr(0, npos);
		}
		size_t minut = PGUtil::MinutConv(varstr);
		index_of_string = 0;

		vv->at(0) = *(float*)&minut;

	};
	auto GetVarStrAndKeepLine = [&]() {
		string varstr;
		index_of_string++;
		auto npos = newstr.find(m_delimiters.substr(m_ndelim, 1));
		while (npos == 0 && m_ndelim == 3 && newstr.size() > 1) {
			newstr = newstr.substr(npos + 1);
			npos = newstr.find(m_delimiters.substr(m_ndelim, 1));
		}
		if (npos == string::npos) {
			if (newstr.size() == 1 && varstr[0] == 32)
				varstr = "";
			else
				varstr = newstr;
			newstr = "";
		}
		else if (npos == 0 && m_ndelim == 3) {



		}
		else {
			varstr = newstr.substr(0, npos);
			newstr = newstr.substr(npos+1);
		}


		return varstr;

	};
	auto SetValueFromString = [&](size_t index, string varstr) {
		if (varstr.find("-9999") == 0 || varstr.find("NaN") == 0 || (varstr.size() == 1 && varstr[0] == 32)||varstr.size()==0|| varstr.find("-2147483648")==0)
			vv->at(index) = MISSING;
		else
			vv->at(index) = FUtil::AtoFloat(varstr);

	};

	if(m_format==NOAA) {

		//Temperature
		varstr=line.substr(24,6);
		if(varstr=="9999.9")
			vv->at(1)=float(MISSING);
		else {
			vv->at(1)=FUtil::AtoFloat(varstr);
			vv->at(1)=float((vv->at(1)-32.)*5./9.);
		}


		//DewPoint
		varstr=line.substr(35,6);
		if(varstr=="9999.9")
			vv->at(2)=float(MISSING);
		else {
			vv->at(2)=FUtil::AtoFloat(varstr);
			vv->at(2)=float((vv->at(2)-32.)*5./9.);
		}

		//Visibility
		varstr=line.substr(68,5);
		if(varstr=="999.9")
			vv->at(3)=float(MISSING);
		else {
			vv->at(3)=FUtil::AtoFloat(varstr);
			vv->at(3)=float(vv->at(3)*1609.3);
		}

		//Wind Speed
		varstr=line.substr(78,5);
		if(varstr=="999.9")
			vv->at(4)=float(MISSING);
		else {
			vv->at(4)=FUtil::AtoFloat(varstr);
			vv->at(4)=float(vv->at(4)*1.852/3.6);
		}

			
		//Max Temp
		varstr=line.substr(102,6);
		if(varstr=="9999.9")
			vv->at(5)=float(MISSING);
		else {
			vv->at(5)=FUtil::AtoFloat(varstr);
			vv->at(5)=float((vv->at(5)-32.)*5./9.);
		}

		//Min Temp
		varstr=line.substr(110,6);
		if(varstr=="9999.9")
			vv->at(6)=float(MISSING);
		else {
			vv->at(6)=FUtil::AtoFloat(varstr);
			vv->at(6)=float((vv->at(6)-32.)*5./9.);
		}
		// Temperature Amplitude
		if(vv->at(5)>MISSING&&vv->at(6)>MISSING) 
			vv->at(10)=vv->at(5)-vv->at(6);
		else
			vv->at(10)=float(MISSING);

		//Precipitation
		varstr=line.substr(118,5);
		if(varstr=="99.99")
			vv->at(7)=float(MISSING);
		else {
			vv->at(7)=FUtil::AtoFloat(varstr);
			vv->at(7)=float(vv->at(7)*25.4);

		}

		//Precipitation Flag
		varstr=line.substr(123,1);
		char ascii;
		ascii=*varstr.data();
		if(varstr=="")
			vv->at(8)=float(MISSING);
		else
			vv->at(8)=float((size_t)ascii-65);

		//Snow Depth
		varstr=line.substr(125,5);
		if(varstr=="999.9")
			vv->at(9)=float(MISSING);
		else
			vv->at(9)=FUtil::AtoFloat(varstr);


		return true;



	}
	else if (option==SMHI) {
		varstr=line.substr(1,8);
		varstr+=line.substr(10,2);
		if(varstr[8]==' ') varstr[8]='0';
		varstr+="00";
		size_t minut;
		minut=PGUtil::MinutConv(varstr);

		vv->at(0)=*(float*)&minut;

		varstr=line.substr(139,6);  //Cloud
		if(varstr=="-999.0")
			vv->at(5)=float(MISSING);
		else {
			vv->at(5)=FUtil::AtoFloat(varstr);

			vv->at(5)/=8;
		}


		varstr=line.substr(265,6);  //Temp
		if(varstr=="-999.0")
			vv->at(1)=float(MISSING);
		else
			vv->at(1)=FUtil::AtoFloat(varstr);

	   varstr=line.substr(97,6); //Wind
		if(varstr=="-999.0")
			vv->at(3)=float(MISSING);
		else
			vv->at(3)=FUtil::AtoFloat(varstr);
	   
		varstr=line.substr(307,6);  //EE
		if(varstr=="-999.0")
			vv->at(2)=float(MISSING);
		else {
			vv->at(2)=FUtil::AtoFloat(varstr);

			vv->at(2)*=100;
		}

		varstr=line.substr(209,6);  //Prec
		if(varstr=="-999.0")
			vv->at(4)=float(MISSING);
		else {
			vv->at(4)=FUtil::AtoFloat(varstr);
			if(vv->at(4)<0) vv->at(4)=0;
			vv->at(4)*=8;
		}



		return true;
	}
	else if (m_format==SWISS) {
		varstr=line.substr(1,4);
		varstr+=line.substr(8,2);
		varstr+=line.substr(13,2);
		if(varstr[4]==' ') varstr[4]='0';
		if(varstr[6]==' ') varstr[6]='0';
		varstr+="1200";
		size_t minut;
		minut=PGUtil::MinutConv(varstr);

		vv->at(0)=*(float*)&minut;

		varstr=line.substr(16,6);  
		if(varstr=="-999.0")
			vv->at(1)=float(MISSING);
		else {
			vv->at(1)=FUtil::AtoFloat(varstr);

			//vv->at(1)/=8;
		}


		varstr=line.substr(23,6);// Temp
		if(varstr=="-999.0")
			vv->at(2)=float(MISSING);
		else
			vv->at(2)=FUtil::AtoFloat(varstr);

/*	   varstr=line.substr(30,6); //TempMin
		if(varstr=="-999.0")
			vv->at(3)=float(MISSING);
		else
			vv->at(3)=FUtil::AtoFloat(varstr);
	   
		varstr=line.substr(37,6);  //TempMax
		if(varstr=="-999.0")
			vv->at(4)=float(MISSING);
		else {
			vv->at(4)=FUtil::AtoFloat(varstr);
		}*/

		varstr=line.substr(44,6);  //Global
		if(varstr=="-999.0")
			vv->at(5)=float(MISSING);
		else {
			vv->at(5)=FUtil::AtoFloat(varstr);
			if(vv->at(5)<0) vv->at(5)=0;
			vv->at(5)*=86400.;
		}
		varstr=line.substr(51,6);  //wind
		if(varstr=="-999.0")
			vv->at(4)=float(MISSING);
		else {
			vv->at(4)=FUtil::AtoFloat(varstr);

		}
		varstr=line.substr(58,6);  //Relative
		if(varstr=="-999.0")
			vv->at(3)=float(MISSING);
		else {
			vv->at(3)=FUtil::AtoFloat(varstr);
		}
/*		varstr=line.substr(65,6);  
		if(varstr=="-999.0")
			vv->at(8)=float(MISSING);
		else {
			vv->at(8)=FUtil::AtoFloat(varstr);
			vv->at(8)*=86400.;
		}
		varstr=line.substr(73,6);  //Prec
		if(varstr=="-999.0")
			vv->at(9)=float(MISSING);
		else {
			vv->at(9)=FUtil::AtoFloat(varstr);
		}
		varstr=line.substr(80,6);  //Prec
		if(varstr=="-999.0")
			vv->at(10)=float(MISSING);
		else {
			vv->at(10)=FUtil::AtoFloat(varstr);
		}*/


		return true;
	}
	else if(m_format==SEPARATED_YMDH) {
		string datemin;
		varstr=line.substr(0,4);
		datemin=varstr;
		auto npos=line.find(m_delimiters.substr(m_ndelim,1));
		line=line.substr(npos+1);
		npos=line.find(m_delimiters.substr(m_ndelim,1));
		varstr=line.substr(0,npos);
		//Month
		if(varstr.size()==1) datemin+="0";
		datemin+=varstr;


		line=line.substr(npos+1);
		npos=line.find(m_delimiters.substr(m_ndelim,1));
		varstr=line.substr(0,npos);
		//Day
		if(varstr.size()==1) datemin+="0";
		datemin+=varstr;

		line=line.substr(npos+1);
		npos=line.find(m_delimiters.substr(m_ndelim,1));
		varstr=line.substr(0,npos);
		//Hr
		if(varstr.size()==1) datemin+="0";
		datemin+=varstr;
		datemin+="30";		
	
		size_t minut;
		minut=PGUtil::MinutConv(datemin);

		vv->at(0)=*(float*)&minut;


		varstr=line.substr(0,npos);
		line=line.substr(npos+1);
		size_t i=0;

		while(varstr.size()>0) {
			i++;
			npos=line.find(m_delimiters.substr(m_ndelim,1));
			auto koll=line.size();
			if(npos==string::npos||npos>line.size()) {
				vv->at(i)=FUtil::AtoFloat(line);
				varstr="";
			}
			else {
				varstr=line.substr(0,npos);
				vv->at(i)=FUtil::AtoFloat(varstr);
				line=line.substr(npos+1);
			}
		}


	}
	else if(m_format==SEPARATED_YMDHM) {
		string datemin;
		varstr=line.substr(0,4);
		datemin=varstr;
		auto npos=line.find(m_delimiters.substr(m_ndelim,1));
		line=line.substr(npos+1);
		npos=line.find(m_delimiters.substr(m_ndelim,1));
		varstr=line.substr(0,npos);
		//Month
		if(varstr.size()==1) datemin+="0";
		datemin+=varstr;


		line=line.substr(npos+1);
		npos=line.find(m_delimiters.substr(m_ndelim,1));
		varstr=line.substr(0,npos);
		//Day
		if(varstr.size()==1) datemin+="0";
		datemin+=varstr;

		line=line.substr(npos+1);
		npos=line.find(m_delimiters.substr(m_ndelim,1));
		varstr=line.substr(0,npos);
		//Hr
		if(varstr.size()==1) datemin+="0";
		datemin+=varstr;


		line=line.substr(npos+1);
		npos=line.find(m_delimiters.substr(m_ndelim,1));
		varstr=line.substr(0,npos);
		//Hr
		if(varstr.size()==1) datemin+="0";
		datemin+=varstr;



	
		size_t minut;
		minut=PGUtil::MinutConv(datemin);

		vv->at(0)=*(float*)&minut;


		varstr=line.substr(0,npos);
		line=line.substr(npos+1);
		size_t i=0;

		while(varstr.size()>0) {
			i++;
			npos=line.find(m_delimiters.substr(m_ndelim,1));
			auto koll=line.size();
			if(npos==string::npos||npos>line.size()) {
				vv->at(i)=FUtil::AtoFloat(line);
				varstr="";
			}
			else {
				varstr=line.substr(0,npos);
				vv->at(i)=FUtil::AtoFloat(varstr);
				line=line.substr(npos+1);
			}
		}


	}
	else if(m_format==CZECH) {
		string newstr;

		for(size_t i=0; i<line.size(); i++) {
			if(line[i]!='\0'&&line[i]>0) newstr.push_back(line[i]);
		}
		auto npos=newstr.find(m_delimiters.substr(m_ndelim,1));
		if(string::npos==npos||npos==0) {
			return false;

		}
		varstr="";
		newstr=newstr.substr(npos+1);		
		npos=newstr.find(m_delimiters.substr(m_ndelim,1));
		varstr=newstr.substr(0,npos);
		for(size_t i=0; i<2;i++) {
			newstr=newstr.substr(npos+1);
			npos=newstr.find(m_delimiters.substr(m_ndelim,1));
			if(npos==1)
				varstr+="-0"+newstr.substr(0,npos);
			else
				varstr+="-"+newstr.substr(0,npos);		
		}
		varstr+=" 12:00";
		size_t minut=PGUtil::MinutConv(varstr,false,false);

		newstr=newstr.substr(npos+1);
		size_t i=0;
		vv->at(i)=*(float*)&minut;	
		while(newstr.size()>0) {
			i++;
			npos=newstr.find(m_delimiters.substr(m_ndelim,1));
			if(npos==string::npos) {
				vv->at(i)=FUtil::AtoFloat(newstr);
				if(i==10) vv->at(8)=vv->at(10)*3600.f;
				
				newstr="";
			}
			else {
				varstr=newstr.substr(0,npos);
				if(npos==0)
					vv->at(i)=MISSING;
				else 
					vv->at(i)=FUtil::AtoFloat(varstr);

				if(i==5) vv->at(5)=vv->at(5)*100.f;

				newstr=newstr.substr(npos+1);
			}
		}



	}
	else if(m_format==CZECH_COL_TRANS) {
		string newstr;
		//ConvLine(vv, line, m_format);
		for(size_t i=0; i<line.size(); i++) {
			if(line[i]!='\0'&&line[i]>0) newstr.push_back(line[i]);
		}
		auto npos=newstr.find(m_delimiters.substr(m_ndelim,1));
		if(string::npos==npos||npos==0) {
			return false;

		}
		varstr=newstr.substr(0, npos);
		size_t irep=PGUtil::AtoInt(varstr);
		newstr=newstr.substr(npos+1);		
		npos=newstr.find(m_delimiters.substr(m_ndelim,1));
		varstr=newstr.substr(0,npos);
		
		size_t irec=1;
		auto minut=m_MinStartCol;
		auto koll=m_MainPGStreamReadWrite.tellp();
		auto year=PGUtil::YearFunction(minut);
		float min, max;
		vv->at(0)=*(float*)&minut;
		vv->at(1)=FUtil::AtoFloat(varstr);//vv->at(1)=vv->at(1)*.014f;
		min=max=vv->at(1);
		m_MainPGStreamReadWrite.write((char*)&vv->at(0),pg.Shape.RecordSize);


		npos=newstr.find(m_delimiters.substr(m_ndelim,1));
		newstr = newstr.substr(npos + 1);

		while(npos!=string::npos) {
			npos = newstr.find(m_delimiters.substr(m_ndelim, 1));
			irec++;
			if (npos != string::npos)
				varstr = newstr.substr(0, npos);
			else
				varstr = newstr;
			newstr=newstr.substr(npos+1);
			size_t i=0;
			string datum=PGUtil::StringDatum(minut);
			year=PGUtil::YearFunction(minut);
			year++;
			minut=PGUtil::cminutx(year,1,1,12,0);
			vv->at(0)=*(float*)&minut;	
			vv->at(1)=FUtil::AtoFloat(varstr);//vv->at(1)=vv->at(1)*.014f;
			m_MainPGStreamReadWrite.write((char*)&vv->at(0),pg.Shape.RecordSize);
			if(min>vv->at(1)) min=vv->at(1);
			if(max<vv->at(1)) max=vv->at(1);
	
		}
		SetVarMax(1, max, irep);
		SetVarMin(1, min, irep);

	}
	else if(m_format==MONTH_COL_TRANS) {
		string newstr;
		//ConvLine(vv, line, m_format);
		for(size_t i=0; i<line.size(); i++) {
			if(line[i]!='\0'&&line[i]>0) newstr.push_back(line[i]);
		}
		auto npos=newstr.find(m_delimiters.substr(m_ndelim,1));
		if(string::npos==npos||npos==0) {
			return false;

		}
		varstr=newstr.substr(0, npos);
			size_t irep=PGUtil::AtoInt(varstr);
		auto minut=PGUtil::cminutx(irep,1,1,12,0);
		newstr=newstr.substr(npos+1);		
		npos=newstr.find(m_delimiters.substr(m_ndelim,1));
		varstr=newstr.substr(0,npos);
		

		auto koll=m_MainPGStreamReadWrite.tellp();
		auto year=PGUtil::YearFunction(minut);
		vv->at(0)=*(float*)&minut;
		vv->at(1)=FUtil::AtoFloat(varstr);

		auto koll2=m_MainPGStreamReadWrite.tellp();
		if(vv->at(1)<m_vmin[0]) m_vmin[0]=vv->at(1);
		if(vv->at(1)>m_vmax[0]) m_vmax[0]=vv->at(1);
		m_MainPGStreamReadWrite.write((char*)&vv->at(0),pg.Shape.RecordSize);

		newstr=newstr.substr(npos+1);
		npos=newstr.find(m_delimiters.substr(m_ndelim,1));
		for(size_t imon=2; imon<=12; imon++) {
			varstr=newstr.substr(0,npos);
			newstr=newstr.substr(npos+1);
			size_t i=0;
			minut=PGUtil::cminutx(year,imon,1,12,0);
			vv->at(0)=*(float*)&minut;	
			vv->at(1)=FUtil::AtoFloat(varstr);
			if(vv->at(1)<m_vmin[0]) m_vmin[0]=vv->at(1);
			if(vv->at(1)>m_vmax[0]) m_vmax[0]=vv->at(1);
			m_MainPGStreamReadWrite.write((char*)&vv->at(0),pg.Shape.RecordSize);
			npos=newstr.find(m_delimiters.substr(m_ndelim,1));
		}

	}
	else if (m_format == PROFOUND) {
		newstr = CleanLineFromNoise(line);
		SetDateFromLine(newstr);
		size_t i = 0;
		while (newstr.size()>0 && i<vv->size() - 1) {
			varstr = GetVarStrAndKeepLine();
			if (index_of_string >= 0) {
				i++;
				SetValueFromString(i, varstr);
			}
		}


	}
	else if (m_format == SILO) {

		size_t i = 0;
		vector<float> temp; temp.resize(10);
		newstr = CleanLineFromNoise(line);
		while (newstr.size()>0 && i<vv->size() - 1) {
			varstr = GetVarStrAndKeepLine();
			if (index_of_string >= 0) {
				i++;
				SetValueFromString(i, varstr);
			}
		}
		size_t minut;
		minut = PGUtil::cminutx(int(vv->at(1)), 1, 1, 12, 0);
		int day = int(vv->at(2));
		minut += (day - 1) * 1440;
		m_CurrentMin = minut;
		temp[1] = vv->at(3);
		temp[2] = (vv->at(4) + vv->at(5)) / 2.;
		temp[3] = vv->at(6);
		temp[4] = vv->at(7);
		temp[5] = vv->at(8);
		temp[6] = vv->at(9);
		for (size_t i = 1; i < 7; i++)
			vv->at(i) = temp[i];




	/*	varstr = line.substr(4, 4);
		int year = FUtil::AtoInt(varstr);



		varstr+="01011200";
		
		minut = PGUtil::MinutConv(varstr);

		string dayno;
		dayno = line.substr(13, 3);
		int day = FUtil::AtoInt(dayno);
		minut += (day-1)*1440;
		m_CurrentMin = minut;

		vv->at(0) = *(float*)&minut;

		vv->at(1) = FUtil::AtoFloat(line.substr(18,6));
		vv->at(2) =( FUtil::AtoFloat(line.substr(26, 6))+ FUtil::AtoFloat(line.substr(33, 7)))/2.;
		vv->at(3) = FUtil::AtoFloat(line.substr(41, 7));
		vv->at(4) = FUtil::AtoFloat(line.substr(49, 7));
		vv->at(5) = FUtil::AtoFloat(line.substr(57, 7));
		vv->at(6) = FUtil::AtoFloat(line.substr(65, 7));
		vv->at(7) = 1.;*/

	}
	else {
		newstr=CleanLineFromNoise(line);

		SetDateFromLine(newstr);
		size_t i = 0;
		while(newstr.size()>0&&i<vv->size()-1) {
			varstr = GetVarStrAndKeepLine();
			if (index_of_string >= 0) {
				i++;
				SetValueFromString(i,varstr);
			}
		}

	}
	return true;
}



bool PGFileImportBase::CreateNewFileAndDefineSize(FILEINFO input,  size_t ActualIndexId)
{

	auto ReturnFileNameBase = [&]() {
		size_t ip = m_filename.find(".txt");
		if (ip == string::npos)
			ip = m_filename.find(".csv");
		if (ip == string::npos) 
			ip = m_filename.find(".");
		if (ip != string::npos)
			return m_filename.substr(0, ip);
		else 
			return m_filename;
	};
	auto SetFileNameFromBaseName = [&]() {
		if (m_format == SWISS) {
			string stat;
			size_t ipos = pg.status.FileName.find("_day");
			size_t ipos1 = pg.status.FileName.rfind('\\');
			size_t length = ipos - ipos1 - 4;
			stat = pg.status.FileName.substr(ipos1 + 1, length);
			m_ImportedFileVector[m_countf].StatNo = stat;
			size_t istat = FUtil::AtoInt(stat);
			pg.status.FileName = pg.status.FileName.substr(0, ipos1 + 1);
			pg.status.FileName += "SwissLFI_";
			if (m_CollectedFile) {
				pg.status.FileName += FUtil::STD_ItoAscii(m_NumToImport);
				pg.status.FileName += "stations";
			}
			else
				pg.status.FileName += stat;
			pg.status.FileName += ".bin";
		}
		else if (m_format == CZECH) {
			size_t ipos = pg.status.FileName.find("_day");
			size_t ipos1 = pg.status.FileName.rfind('\\');
			size_t length = ipos - ipos1 - 4;

			pg.status.FileName = m_filename.substr(0, ipos1 + 1);
			pg.status.FileName += "Czech_";
			if (m_CollectedFile) {
				pg.status.FileName += FUtil::STD_ItoAscii(m_NumToImport);
				pg.status.FileName += "stations";
			}
			else
				pg.status.FileName += m_ImportedFileVector[m_countf].StatNo;
			pg.status.FileName += ".bin";
		}
		else if (m_format == CZECH_COL_TRANS) {
			if (!m_SwissCoord) m_SwissCoord = ReadCoord();
			pg.status.FileName = ReturnFileNameBase() + ".bin";
		}
		else if (m_format == MONTH_COL_TRANS) {
			if (!m_SwissCoord) m_SwissCoord = ReadCoord();
			pg.status.FileName = ReturnFileNameBase() + ".bin";
		}
		else if (!m_CurrentFileInfo.EqualSizeOfSiteRecords&&m_ImportedFileVector.size()>1) {
			pg.status.FileName = ReturnFileNameBase() + "_" + FUtil::STD_ItoAscii(m_CurrentFileInfo.LinkedSites[m_countf].Ids);
			pg.status.FileName += ".bin";
		}
		else  {
			pg.status.FileName = ReturnFileNameBase() + ".bin";
		}


	};
	if (m_AddFromFile) {
		SetCompleteRead(false);
		SetNumRecords(m_CurrentFile.NumRec - input.NumHeads + m_nprevrec);
		ReAllocate(input.TotRec - input.NumHeads, input.MaxVar - m_time_red - input.RedNumVar);
		size_t ip = m_filename.find(".txt");
		if (ip == string::npos)
			ip = m_filename.find(".csv");
		pg.status.FileName = m_filename.substr(0, ip) + ".bin";
		SaveAs(pg.status.FileName, true);
		pg.Var.vv.resize(pg.Shape.NumVar + 1);
		m_ExtraPGStreamWrite.open("Dump.bin", ios::in | ios::binary);
		if (!m_ExtraPGStreamWrite.is_open()) {
			return false;
		}
		//SetNumRecords(input.TotRec-input.TotRec+m_nprevrec);
		size_t oldnumvar;
		size_t oldrec;
		m_ExtraPGStreamWrite.read((char*)&oldrec, 4);
		m_ExtraPGStreamWrite.read((char*)&oldnumvar, 4);

		size_t pos = pg.Shape.RecordSize;
		if (oldnumvar != pg.Shape.RecordSize)
			return false;

		m_ExtraPGStreamWrite.seekg(pos, ios::beg);
		m_MainPGStreamReadWrite.seekg(pos, ios::beg);
		for (size_t j = 0; j < m_nprevrec; j++) {
			m_ExtraPGStreamWrite.read((char*)pg.Var.vv.data(), pg.Shape.RecordSize);
			m_MainPGStreamReadWrite.write((char*)pg.Var.vv.data(), pg.Shape.RecordSize);
		}
		m_ExtraPGStreamWrite.close();
	}
	else {
		if (m_countf == 0||( !input.EqualSizeOfSiteRecords&&m_ImportedFileVector.size()>1)) {
			SetCompleteRead(false);
			if (m_CollectedFile) {
				if(m_format == PROFOUND&&input.LinkedSites.size()>ActualIndexId)
					CPGFile::CreateNewFile(input.MaxVar - m_time_red - input.RedNumVar, input.LinkedSites[ActualIndexId].Records, 1);
				else
					CPGFile::CreateNewFile(input.MaxVar - m_time_red - input.RedNumVar,( input.TotRec - input.NumHeads)/ m_NumToImport, m_NumToImport);
			}
			else {
				if (m_format == PROFOUND||m_format==STANDARD)
					CPGFile::CreateNewFile(input.MaxVar - m_time_red - input.RedNumVar, input.LinkedSites[0].Records, input.LinkedSites.size());
				else
					CPGFile::CreateNewFile(input.MaxVar - m_time_red - input.RedNumVar, input.TotRec - input.NumHeads, m_ImportedFileVector.size());
			}			
			SetInfoMode();
			SetFileNameFromBaseName();
			SaveAs(pg.status.FileName, true); // Creating and open the Main Stream for writing 

			pg.Var.vv.resize(pg.Shape.NumVar+12);
			for(size_t i=0; i<pg.Shape.NumVar;i++)
				pg.Var.vv[i+1]=float(MISSING);

			if(m_MainPGStreamReadWrite.good()&&m_MainPGStreamReadWrite.is_open())
				return true;
			else
				return false;
		}
		else  {
				if(m_format==MONTH_COL_TRANS) {
					string stat;
					size_t ipos1=m_filename.rfind('\\');
					m_filename=m_filename.substr(0, ipos1+1)+m_ImportedFileVector[m_countf].StatNo+".txt";
				}

				if(!m_MainPGStreamReadWrite.is_open()) {
					m_MainPGStreamReadWrite.open(pg.status.FileName, ios ::out|ios::in|ios::binary);
					return m_MainPGStreamReadWrite.is_open();
				}
		}
	
	}
	return true;
}
bool PGFileImportBase::ReadHeading(FILEINFO input, size_t ActualIndexId) {
	size_t nvar;
	string varstr, unitstr, statno;
	string str;
	string teststr;
	size_t nbyte;
	size_t i_head=0;
	str="";


	if (input.NumHeads == 0) return true;
	//m_FileText.clear();
	//if(!m_FileText.good()) return false;
	if(!m_FileText.is_open()) {
		if(m_format==CZECH) {
			size_t ipos1=m_filename.rfind('\\');
			m_filename=m_filename.substr(0,ipos1+1);
			m_filename+=m_ImportedFileVector[m_countf].StatNo+".txt";
		}
		m_FileText.open(m_filename,ios::in|ios::binary);
		if(m_FileText.good()&&m_FileText.is_open()) {


		}
		else {
			return false;


		}
	}

	if (m_format == PROFOUND) {
		size_t numrep = 1;
		if (input.EqualSizeOfSiteRecords)  numrep = input.LinkedSites.size();

		for (size_t irep = 0; irep < numrep; irep++) {
			size_t countv = 1;
			string id_base="";
			for (size_t i = 0; i < input.ValidVariables.size(); i++) {
				if (input.ValidVariables[i]) {
					id_base = "";
					if (m_ProFoundOption == 1) {
						if (input.FirstValidHeads[i].find("taF_d") == 0) {
							SetVarName(countv, "Air Temperature", irep + 1);
							SetVarUnit(countv, "C", irep + 1);
							id_base = input.FirstValidHeads[i];
						}
						else if (input.FirstValidHeads[i].find("pF_m") == 0) {
							SetVarName(countv, "Precipitation", irep + 1);
							SetVarUnit(countv, "mm", irep + 1);
							id_base = input.FirstValidHeads[i];
						}
						else if (input.FirstValidHeads[i].find("vpdF_h") == 0) {
							SetVarName(countv, "Vapour Pressure Deficit", irep + 1);
							SetVarUnit(countv, "%", irep + 1);
							id_base = input.FirstValidHeads[i];
						}
						else if (input.FirstValidHeads[i].find("lwInF_W") == 0) {
							SetVarName(countv, "Long Wave Radiation", irep + 1);
							SetVarUnit(countv, "J/m2/day", irep + 1);
							id_base = input.FirstValidHeads[i];
						}
						else if (input.FirstValidHeads[i].find("swInF_W") == 0) {
							SetVarName(countv, "Global Radiation", irep + 1);
							SetVarUnit(countv, "J/m2/day", irep + 1);
							id_base = input.FirstValidHeads[i];
						}
						else if (input.FirstValidHeads[i].find("wsF_m") == 0) {
							SetVarName(countv, "Wind Speed", irep + 1);
							SetVarUnit(countv, "m/s", irep + 1);
							id_base = input.FirstValidHeads[i];
						}
					}
					else if (m_ProFoundOption == 2) {
						string name;
						string unit;
						if (countv <= 2) {
							name = "Carbon Flux";
							unit = "gC/m2/day";
							if (countv % 2 == 1) id_base = "Measured (0)-";
							id_base += FUtil::STD_ItoAscii((countv + 1) / 2);

						}
						else if (countv == 3) {
							name = "EcoSystem Respiration";
							unit = "gC/m2/day";
							id_base = "Estimated ";
							id_base += FUtil::STD_ItoAscii((countv + 1) / 2);
						}
						else if (countv == 4) {
							name = "GPP";
							unit = "gC/m2/day";
							id_base = "Estimated ";
							id_base += FUtil::STD_ItoAscii((countv + 1) / 2);

						}
						else if (countv< 7) {
							name = "Latent Heat Flux";
							unit = "J/m2/day";
							if (countv % 2 == 1) id_base = "Measured (0)-";
							id_base += FUtil::STD_ItoAscii((countv + 1) / 2);
						}
						else {
							name = "Sensible Heat Flux";
							unit = "J/m2/day";
							if (countv % 2 == 1) id_base = "Measured (0)-";
							id_base += FUtil::STD_ItoAscii((countv + 1) / 2);

						}
						SetVarName(countv, name, irep + 1);
						SetVarUnit(countv, unit, irep + 1);
						string site_id;
						site_id = id_base + " site:"; site_id += FUtil::STD_ItoAscii(input.LinkedSites[ActualIndexId].Ids);
						SetVarId(countv, site_id, irep + 1);


					}
					else if (m_ProFoundOption == 3) {
						string name;
						string unit;
						if (countv <= 10) {
							name = "Soil Temperature";
							unit = char(176); unit+= "C";
							if (countv % 2 == 1) id_base = "Measured (0)-";
							id_base+= FUtil::STD_ItoAscii((countv+1) / 2);

						}
						else if (countv <= 20) {
							name = "Water Content";
							unit = "vol %";
							if (countv % 2 == 1) id_base = "Measured (0)-";
							id_base += FUtil::STD_ItoAscii((countv-10 + 1 )/ 2);


						}
						SetVarName(countv, name, irep + 1);
						SetVarUnit(countv, unit, irep + 1);
						string site_id;
						site_id = id_base + " site:"; site_id += FUtil::STD_ItoAscii(input.LinkedSites[ActualIndexId].Ids);
						SetVarId(countv, site_id, irep + 1);

					}
					else {
						SetVarName(countv, input.FirstValidHeads[i], irep + 1);
						string site_id;
						site_id = id_base + " _"; site_id += FUtil::STD_ItoAscii(input.LinkedSites[ActualIndexId].Ids);
						SetVarId(countv, site_id, irep + 1);
					}

	

					countv++;
				}
			}
		}
		return true;
	}


	fstream::pos_type end_pos;
	
	m_FileText.seekg(0,ios::end);
	end_pos=m_FileText.tellg();

	m_FileText.seekg(0,ios::beg);
	size_t num_to_read;
    
    num_to_read=min((int)size_t(end_pos), 10000);

	teststr.resize(num_to_read);
	m_FileText.read((char*)&teststr[0],num_to_read);
	fstream::pos_type k;
	k=m_FileText.tellg();

	nbyte=size_t(k);
	if(nbyte>1000)
		nbyte=1000;
	for(size_t i=0; i<num_to_read;i++)
		if(teststr[i]>0&&teststr[i]!=34) str.push_back(teststr[i]);
		
	size_t ich=str.find(m_linebreak);

	for (size_t i = 0; i < nbyte; i++) {
		nvar = 1;
		string checkline = str.substr(0, ich);
		if (ich == string::npos) continue;
		string line;
		for (size_t jj = 0; jj < ich; jj++)
			if (checkline[jj] > 0) line.push_back(checkline[jj]);
		ich = line.size();
		if (ich == string::npos) continue;
		bool SkipFirst = false;
		FUtil::trim(line);
		if (m_format == NOAA) {
			for (size_t jj = 1; jj < 11; jj++) {
				unitstr.clear();
				switch (jj) {
				case (1):
					varstr = "Air Temperature";
					unitstr += char(176);
					unitstr += "C";
					break;
				case (2):
					varstr = "Dew Point Temperature";
					unitstr += char(176);
					unitstr.append("C");
					break;
				case (3):
					varstr = "Visibility";
					unitstr = "m";
					break;
				case (4):
					varstr = "Wind Speed";
					unitstr = "m/s";
					break;
				case (5):
					varstr = "Max Temperature";
					unitstr += char(176);
					unitstr.append("C");
					break;
				case (6):
					varstr = "Min Temperature";
					unitstr += char(176);
					unitstr.append("C");
					break;
				case (7):
					varstr = "Precipitation";
					unitstr = "mm/day";
					break;
				case (8):
					varstr = "Prec observation";
					unitstr = "-";
					break;
				case (9):
					varstr = "Snow Depth";
					unitstr = "m";
					break;
				case (10):
					varstr = "TempDiff";
					unitstr = "m";
					break;
				}
				SetVarName(jj, varstr, m_countf + 1);
				SetVarUnit(jj, unitstr, m_countf + 1);
				SetVarId(jj, m_ImportedFileVector[m_countf].StatNo, m_countf + 1);
				if (pg.status.FileVersion >= 1) {
					size_t ival = FUtil::AtoInt(m_ImportedFileVector[m_countf].StatNo);
					SetVarKey(jj, ival);
				}
			}
		}
		else if (m_format == SMHI) {

			m_newstartpos += ich;
			size_t jj = 0;
			str = str.substr(ich + 2);
			ich = str.find(m_linebreak);
			if (i == 0) {  //Line 1
				statno = line; //StatNo
				for (size_t jj = 1; jj < 6; jj++) SetVarId(jj, statno);
			}
			else if (i == 1) {
				varstr = line; varstr += " ";
				varstr += statno;
				for (size_t jj = 1; jj < 6; jj++) 	SetVarStation(jj, varstr);

			}
			else if (i == 4) { //Line in file
				varstr = line.substr(0, 7);
				float lat = FUtil::AtoFloat(varstr);
				lat /= 10000;
				varstr = line.substr(8, 8);
				float longitude = FUtil::AtoFloat(varstr);
				longitude /= 10000;
				for (size_t jj = 1; jj < 6; jj++) {
					SetVarLat(jj, lat);
					SetVarLong(jj, longitude);
					//pg.Des.addinfo[jj].Long=longitude;

				}

				for (size_t jj = 1; jj < 6; jj++) {
					unitstr.clear();
					switch (jj) {
					case (1):
						varstr = "Air Temperature";
						unitstr += char(176);
						unitstr.append("C");
						break;
					case (2):
						varstr = "Vapour Pressure";
						//unitstr.append<size_t>(1,0xB0);
						unitstr.append("Pa");
						break;
					case (3):
						varstr = "Wind Speed";
						unitstr = "m/s";
						break;

					case (4):
						varstr = "Precipitation";
						unitstr = "mm/day";
						break;
					case (5):
						varstr = "Cloudiness";
						unitstr = "-";
						break;
					}
					SetVarName(jj, varstr, m_countf + 1);
					SetVarUnit(jj, unitstr, m_countf + 1);
				}

			}
		}
		else if (m_format == SWISS) {
			for (size_t jj = 1; jj < 6; jj++) {
				unitstr.clear();
				switch (jj) {
				case (1):
					varstr = "Air Temperature";
					unitstr += char(176);
					unitstr.append("C");
					break;
				case (5):
					varstr = "Global Radiation";
					unitstr.append("J/m2/day");
					break;
				case (3):
					varstr = "Wind Speed";
					unitstr = "m/s";
					break;
					/*		case (4):
								varstr="Max Temperature";
								unitstr.append(1,0xB0);
								unitstr.append("C");
								break;
							case (3):
								varstr="Min Temperature";
								unitstr.append(1,0xB0);
								unitstr.append("C");
								break;*/
				case (4):
					varstr = "Precipitation";
					unitstr = "mm/day";
					break;
				case (2):
					varstr = "Relative Humidity";
					unitstr = "%";
					break;
					/*	case (8):
							varstr="Potential Radiation";
							unitstr="J/m2/day";
							break;
						case (9):
							varstr="Ratio AT/PT";
							unitstr="-";
							break;
						case (10):
							varstr="Ratio AT/PTC";
							unitstr="-";
							break;*/
				}
				SetVarName(jj, varstr, m_countf + 1);
				SetVarUnit(jj, unitstr, m_countf + 1);
				SetVarId(jj, m_ImportedFileVector[m_countf].StatNo, m_countf + 1);
				if (pg.status.FileVersion >= 1) {
					size_t ival = FUtil::AtoInt(m_ImportedFileVector[m_countf].StatNo);
					SetVarKey(jj, ival);
				}
			}
		}
		else if (m_format == CZECH) {
			for (size_t jj = 1; jj < 9; jj++) {
				unitstr.clear();
				switch (jj) {
				case (3):
					varstr = "Air Temperature";
					unitstr += char(176);
					unitstr.append("C");
					break;
				case (8):
					varstr = "Global Radiation";
					unitstr.append("J/m2/day");
					break;
				case (7):
					varstr = "Wind Speed";
					unitstr = "m/s";
					break;
				case (1):
					varstr = "Max Temperature";
					unitstr += char(176);
					unitstr.append("C");
					break;
				case (2):
					varstr = "Min Temperature";
					unitstr += char(176);
					unitstr.append("C");
					break;
				case (4):
					varstr = "Precipitation";
					unitstr = "mm/day";
					break;
				case (6):
					varstr = "Relative Humidity";
					unitstr = "%";
					break;
				case (5):
					varstr = "Vapour Pressure";
					unitstr = "Pa";
					break;
					/*	case (8):
							varstr="Potential Radiation";
							unitstr="J/m2/day";
							break;
						case (9):
							varstr="Ratio AT/PT";
							unitstr="-";
							break;
						case (10):
							varstr="Ratio AT/PTC";
							unitstr="-";
							break;*/
				}
				SetVarName(jj, varstr, m_countf + 1);
				SetVarUnit(jj, unitstr, m_countf + 1);
				SetVarId(jj, m_ImportedFileVector[m_countf].StatNo, m_countf + 1);
				if (pg.status.FileVersion >= 1) {
					size_t ival = FUtil::AtoInt(m_ImportedFileVector[m_countf].StatNo);
					SetVarKey(jj, ival);
				}
			}
		}
		else if (m_format == CZECH_COL_TRANS) {
			string checkline = str;
			ConvColLine(&pg.Var.vv, checkline, m_format);

			for (size_t jj = 1; jj < 2; jj++) {
				unitstr.clear();
				switch (jj) {
				case (1):
					if (pg.status.FileName.find("NO3") != string::npos)
						varstr = "NO3 Deposition Rate";
					else
						varstr = "NH4 Deposition Rate";
					unitstr = "gN/m2/yr";
					break;

				}
				for (size_t nstat = 0; nstat < 1599; nstat++) {
					SetVarName(jj, varstr, nstat + 1);
					SetVarUnit(jj, unitstr, nstat + 1);
					SetVarId(jj, FUtil::STD_ItoAscii(nstat + 1), nstat + 1);
				}
				if (pg.status.FileVersion >= 1) {
					size_t ival = FUtil::AtoInt(m_ImportedFileVector[m_countf].StatNo);
					SetVarKey(jj, ival);
				}

				return true;
			}

		}
		else if (m_format == MONTH_COL_TRANS) {
			string checkline = str.substr(ich + 3, ich);
			ConvColLine(&pg.Var.vv, checkline, m_format);

			for (size_t jj = 1; jj < 2; jj++) {
				unitstr.clear();
				switch (jj) {
				case (1):
					if (pg.status.FileName.find("sraz") != string::npos) {
						varstr = "Precipitation";
						unitstr = "mm";
					}
					else {
						varstr = "Temperature";
						unitstr = char(176);
						unitstr += "C";
					}

					break;

				}
				for (size_t nstat = 0; nstat < 1599; nstat++) {
					SetVarName(jj, varstr, nstat + 1);
					SetVarUnit(jj, unitstr, nstat + 1);
					SetVarId(jj, FUtil::STD_ItoAscii(nstat + 1), nstat + 1);
				}
				if (pg.status.FileVersion >= 1) {
					size_t ival = FUtil::AtoInt(m_ImportedFileVector[m_countf].StatNo);
					SetVarKey(jj, ival);
				}

				return true;
			}

		}

		else {
			size_t npos;
			if (m_ndelim != string::npos) 
				npos = line.find(m_delimiters.substr(m_ndelim, 1));			
			else
				npos = string::npos;
			if(npos!=string::npos) varstr = line.substr(0, npos);
			else varstr = line;
			i_head = 0;
			if (varstr.find("Name") != string::npos || varstr.find("DATE") != string::npos || varstr.find("Date") != string::npos || varstr.find("date") != string::npos || varstr.find("Time") != string::npos || varstr.find("DATUM") != string::npos)
				i_head = 1;
			else if (varstr.find("Variable Unit") != string::npos)
				i_head = 2;
			else if (varstr.find("Variable Id") != string::npos)
				i_head = 3;
			else if (varstr.find("Variable Pos") != string::npos)
				i_head = 4;
			FUtil::trim(line);
			if (m_ndelim != string::npos)
				npos = line.find(m_delimiters.substr(m_ndelim, 1));
			else
				npos = string::npos;
			while (npos != string::npos) {
				varstr = line.substr(0, npos);
				if (i_head == 1 && (input.RedNumVar == 0 || input.RedNumVar == 1)) {
					FUtil::trim(varstr);
					if ((varstr.find("Date") == 0 || varstr.find("ISOD") == 0 || varstr.find("date") == 0 || varstr.find("DATE") == 0 || varstr.find("Variable Names") == 0) && nvar == 1) {
						SkipFirst = true;
						input.RedNumVar = 0;

					}
					else if ((varstr.find("TIME") == 0 || varstr.find("time") == 0) && nvar == 2) {
						SkipFirst = true;
						input.RedNumVar = 1;
					}
					else if (!SkipFirst)
						SetVarName(nvar, varstr);
					else
						SetVarName(nvar - 1 - input.RedNumVar, varstr);
					if (varstr.find("Time") == 0 && nvar == 1) {
						SetVarUnit(1, "Seconds");
					}
				}
				else if (i_head == 1 && nvar - input.RedNumVar > 0) {
					SetVarName(nvar - input.RedNumVar, varstr);
				}
				else if (i_head == 2 && nvar > 1)
					SetVarUnit(nvar - 1, varstr);
				else if (i_head == 3 && nvar > 1) {
					SetVarId(nvar - 1, varstr);


				}
				else if (i_head == 4 && nvar > 1)
					SetVarPos(nvar - 1, varstr);
				line = line.substr(npos + 1);
				FUtil::trim(line);
				npos = line.find(m_delimiters.substr(m_ndelim, 1));
				nvar++;
			}
			varstr = line;
			if (i_head == 1) {

				if (SkipFirst)
					SetVarName(nvar - 1 - input.RedNumVar, varstr);
				else
					SetVarName(nvar - input.RedNumVar, varstr);
				if (nvar == 2) {
					FUtil::trim(varstr);
					npos = varstr.find(' ');
					SetVarName(nvar - input.RedNumVar, varstr.substr(0, npos));
					SetVarUnit(nvar - input.RedNumVar, varstr.substr(npos + 1, 2));
					npos = m_filename.find(".txt");
					if (npos != string::npos)
						SetVarId(nvar - input.RedNumVar, m_filename.substr(npos - 5, 5));
				}
			}
			else if (i_head == 2)
				SetVarUnit(nvar - 1, varstr);
			else if (i_head == 3)
				SetVarId(nvar - 1, varstr);
			else if (i_head == 4)
				SetVarPos(nvar - 1, varstr);
			str = str.substr(ich + m_linebreak.size());
			ich = str.find(m_linebreak);
			if (input.NumHeads == 1) return m_FileText.good();

		}
	}
	return m_FileText.good();
}
bool PGFileImportBase::SettingStartPositioninInputFile() {
		std::streampos StartPos=0;
		std::streampos pos1;
		if(m_countf>0&&!m_CollectedFile) {
			StartPos=140*(m_ImportedFileVector[m_countf].RecStart-2);
		}
		else if(m_newstartpos>0)
			StartPos=m_newstartpos+12;
		
		m_FileText.seekg(StartPos, ios::beg);
		pos1=m_FileText.tellg();
		m_FileText.close();
	    return true;
	
}
bool PGFileImportBase::SettingCheckingStartPositioninOutputFile()
{
	std::streampos pos;
	if (pg.Shape.NumRepetitions>1 && m_countf>0)
		pos = m_countf*pg.Shape.NumRecords*pg.Shape.RecordSize + pg.Shape.RecordSize;
	else
		pos = pg.Shape.RecordSize;
	m_MainPGStreamReadWrite.seekp(pos, ios::beg);
	m_actpos = m_MainPGStreamReadWrite.tellp();
	return m_MainPGStreamReadWrite.good();
}
bool PGFileImportBase::ReadingVariableValues(FILEINFO input, size_t ActualIndexId)
{

		if(!SettingStartPositioninInputFile()) return false;
		if (!SettingCheckingStartPositioninOutputFile()) return false;
		size_t pos1=0,pos2=0;
		size_t rrec=GetReadBufferSize(input);
		size_t nrec=0;
		size_t nbyte=0;
		size_t nread=0;
		size_t nmissing=0;
		string str;
		size_t ich = string::npos;
		string datestring;


	

		string line;
		line.resize(40000);
		ifstream istream;
		istream.open(m_filename,ifstream::in);

		auto SetStreamToStartPos = [&](size_t linetobypass) {
			for(size_t i=0;i<linetobypass;i++)
				istream.getline(&line[0], 40000);
			return linetobypass;
		};
		size_t CountLine = 0;
	
		size_t Countgaps = 0;
		bool TimeSetComplete;


		auto GetLineFromStream = [&]() {
				line.clear();
				line.resize(40000);
				//string line;
				//if(ich>0) line=str.substr(0,ich);
				istream.getline(&line[0], 40000);
				size_t koll = line.size();
				ich = line.find('\0');
				line.resize(ich);
				if (ich <= 0) {
					if (istream.good()) {
						line.clear();
						line.resize(40000);
						istream.getline(&line[0], 40000);
						ich = line.find('\0');
						line.resize(ich);
					}
					else
						return false;
				}
				CountLine++;
				return true;
			};
		auto CheckReadOfHeading = [&]() {

				if (input.NumHeads == 0) m_HeadingRead = true;
				while (nrec < input.NumHeads) {
					nrec++;
					line.clear();
					line.resize(20000);
					istream.getline(&line[0], 20000);
					ich = line.find('\0');
					line.resize(ich);

				}
				m_HeadingRead = true;
			};
		auto SetDateFromString = [&](string newstr) {

				auto npos = newstr.find(m_delimiters.substr(m_ndelim, 1));

				auto nhourcolumn = newstr.rfind(':');

				string varstr = newstr.substr(0, npos);

				if (nhourcolumn > npos&&nhourcolumn != string::npos) {
					newstr = newstr.substr(npos + 1);
					npos = newstr.find(m_delimiters.substr(m_ndelim, 1));
					varstr += ' ' + newstr.substr(0, npos);
				}
				size_t minut = PGUtil::MinutConv(varstr);

				pg.Var.vv[0] = *(float*)&minut;

			};
		auto SetDummyTimeFromRec = [&](size_t nrec) {
			m_CurrentMin = m_minst_a+ (nrec - 1) * GetNormalTimeInterval();
		};
	
		size_t linetobypass = input.NumHeads;
		if (ActualIndexId > 0) {
			for (size_t i = 0; i < ActualIndexId; i++) {
				linetobypass += input.LinkedSites[i].Records;
			}
			CountLine=SetStreamToStartPos(linetobypass);

		}
		else if (m_format == STANDARD||m_format==SMHI_OPENDATA) {
			CountLine = SetStreamToStartPos(linetobypass);

		}
		size_t LastLinetoRead;
		if (input.LinkedSites.size() > ActualIndexId)
			if(input.LinkedSites[ActualIndexId].Records>1)
				LastLinetoRead = linetobypass + input.LinkedSites[ActualIndexId].Records*input.LinkedSites[ActualIndexId].TimeCheck.RecordStep;
			else
				LastLinetoRead = input.TotRec;
		else
			LastLinetoRead = string::npos;
			
		size_t valid_var_index = 1;
		size_t extralines = 0;
		size_t extralinestot = 0;
		size_t next_validLine = 0;
		bool skipline;

		auto itmap = m_CheckedTimeMap.begin();
			while(istream.good()&&CountLine<LastLinetoRead) {


				if (!GetLineFromStream()) return false;

				size_t next_validLine = (*itmap).first;
				//if (m_format == STANDARD) next_validLine++;


				if (next_validLine < CountLine+1) {

					if (ActualIndexId == 0 && m_format == SMHI_OPENDATA)
						m_HeadingRead = true;
					else if (ActualIndexId == 0&&m_format!=STANDARD)
						CheckReadOfHeading();
					else 
						m_HeadingRead = true;

					auto SetVectorValuesFromFixedFormat = [&]() {
						size_t minut, min_next;
						TimeSetComplete = true;
						if (m_format == NOAA) {
							minut = GetMinutFromLine(line);
							while (minut > m_minst_a + (nrec - 1) * 1440) { //if minut is a head of time points fill in with missing
								min_next = m_minst_a + (nrec - 1) * 1440;
								pg.Var.vv[0] = *(float*)&min_next;
								m_MainPGStreamReadWrite.write((char*)&pg.Var.vv[0], pg.Shape.RecordSize);
								nrec++;
								nmissing++;
							}
							nread++;
							pg.Var.vv[0] = *(float*)&minut;
						}
						else if (m_format == CZECH_COL_TRANS) {
							nread++;


						}
						else if (m_format == MONTH_COL_TRANS) {
							nread++;
							nrec += 12;

						}
						else
							nread++;
						bool koll = m_MainPGStreamReadWrite.good();
						ConvLine(&pg.Var.vv, line, m_format);
					};
					string line_saved;
					auto SetVectorValuesFromNonFixedFormat = [&]() {
						size_t npos;
						size_t NumSelectedOutputs;
						NumSelectedOutputs = (input.MaxVar - input.RedNumVar);
						size_t RecordStep = input.LinkedSites[ActualIndexId].TimeCheck.RecordStep;
						string varstr;
						size_t nvar = 0;
						if ((CountLine - input.NumHeads) % RecordStep == 0) valid_var_index = 1;
						else {
							valid_var_index = (CountLine - input.NumHeads) % RecordStep *
								NumSelectedOutputs / RecordStep + 1;
						}


						if (m_ndelim != string::npos) {
							npos = line.find(m_delimiters.substr(m_ndelim, 1));
						}
						else
							npos = string::npos;
						string dateString;
						TimeSetComplete = false;

						//Skip line start 


						size_t iskip = 0;
						while (iskip < input.SkipBeforeVar) {
							varstr = line.substr(0, npos);
							line = line.substr(npos + 1);
							npos = line.find(m_delimiters.substr(m_ndelim, 1));
							iskip++;
						}
						skipline = false;

						if (extralines > 0) {
							extralines--;
						}
						else {
							while (npos != string::npos || line.size() > 0) {
								if (npos != string::npos)
									varstr = line.substr(0, npos);
								else
									varstr = line;
								if (varstr.size() > 0 && varstr.find("-9999") != 0 && varstr.find("NaN") != 0 && varstr.find("-2147483648") != 0) {
									if (input.ValidVariables[nvar]) {
										SetValidVariable(valid_var_index, varstr);
										valid_var_index++;
									}
									if (!TimeSetComplete) TimeSetComplete = SetDefinedTime(next_validLine);
									/*if (!TimeSetComplete && m_CheckedTimeMap.size() > nrec - extralinestot) {
										TimeSetComplete = SetDefinedTime((nrec - 1) * input.LinkedSites[ActualIndexId].TimeCheck.RecordStep + 1 + min((int)input.NumHeads, 1) + extralinestot);
										while (!TimeSetComplete && m_CheckedTimeMap.size() >= nrec - extralinestot) {
											TimeSetComplete = SetDefinedTime((nrec - 1) * input.LinkedSites[ActualIndexId].TimeCheck.RecordStep + 1 + min((int)input.NumHeads, 1) + extralinestot);
											extralines++;
											extralinestot++;
											LastLinetoRead++;
											line_saved = line;
											skipline = true;
										}


									} */
									else if (!TimeSetComplete && !skipline) TimeSetComplete = SetTimeVariable(nrec, varstr, nvar + 1, false);

									if (!TimeSetComplete && valid_var_index == input.MaxVar) {
										size_t Prev_Min;
										Prev_Min = m_CurrentMin;
										//SetDummyTimeFromRec(nrec+1);
										//if (Prev_Min > m_CurrentMin) m_CurrentMin = Prev_Min;
										TimeSetComplete = true;
									}

								}
								else {
									if (input.ValidVariables[nvar] && nvar < input.MaxVar / input.LinkedSites[ActualIndexId].TimeCheck.RecordStep) {
										SetMissingVariable(valid_var_index);
										valid_var_index++;
									}
									if (!TimeSetComplete) TimeSetComplete = SetDefinedTime(next_validLine);
								}
								if (npos != string::npos && line.size() > npos)
									line = line.substr(npos + 1);
								if (m_ndelim != string::npos)
									npos = line.find(m_delimiters.substr(m_ndelim, 1));

								if (input.ValidVariables.size() > nvar + 1) {
									nvar++;
								}
								else
									line = "";

							}

							if (!TimeSetComplete) {
								//SetDummyTimeFromRec(nrec);
								if (LastLinetoRead < input.TotRec)	LastLinetoRead++;
							}
						}

					};

					auto SetVectorValueFromLine = [&]() {
						if (input.FixedFormat)
							SetVectorValuesFromFixedFormat();
						else
							SetVectorValuesFromNonFixedFormat();
					};

					if (m_HeadingRead) {
						SetVectorValueFromLine();
						if (m_format == STANDARD|| m_format ==SMHI_OPENDATA) {
							if (CountLine == next_validLine && !skipline) {
								SetPGTimeVar_Minut_To_PG_Var();
								AdjustValuesToNewUnits();
								WriteCompleteRecord();
								nrec++;
							}
						}
						else if (m_format != STANDARD && m_format != CZECH_COL_TRANS && m_format != MONTH_COL_TRANS && TimeSetComplete &&
							(CountLine + 1 - input.NumHeads) % input.LinkedSites[ActualIndexId].TimeCheck.RecordStep == 0
							) {
							nrec++;
							SetPGTimeVar_Minut_To_PG_Var();
							AdjustValuesToNewUnits();
							WriteCompleteRecord();
						}


#ifndef COUPSTD
						if (nrec % 5000 == 0) {
							MFC_Util m;
							m.SetPaneMessage("Records read: (" + FUtil::STD_ItoAscii(nrec) + ")");
							/*	CFileScanner ScanMeanProg(nullptr);
								ScanMeanProg.Create(IDD_PROGRESS,nullptr);
								ScanMeanProg.ShowWindow(SW_SHOW);

								size_t iprog=size_t((float(CountLine)/float(LastLinetoRead)/float(m_ImportedFileVector.size())+float(m_countf)/float(m_ImportedFileVector.size()))*100.);
								ScanMeanProg.Update(iprog);*/
						}
#endif
					}
					itmap++;
				}
				else LastLinetoRead++;

		}
	return true;
}
size_t PGFileImportBase::GetReadBufferSize(FILEINFO input)
{
	size_t rrec=1024;
	if(input.FixedFormat) {
		if(m_format==SMHI) 
			rrec=322;
		else if(m_format==SWISS)
			rrec=93;
		else
			rrec=140;
	}
	return rrec;

}
bool PGFileImportBase::WriteMissingRegularTime()
{
	while(m_nrec<m_totrec_a*(m_countf+1)) {
			m_nrec++;
			m_min_next=m_minst_a+(m_nrec-1)*1440;
			pg.Var.vv[0]=*(float*)&m_min_next;
			m_MainPGStreamReadWrite.write((char*)&pg.Var.vv[0],pg.Shape.RecordSize);

			for(size_t j=0;j<pg.Shape.NumVar;j++) {
				size_t ind=m_countf*pg.Shape.NumVar+j;
				pg.Des.addinfo[ind].num_missing++;
			}
	}
	return true;
}
void PGFileImportBase::InitForSettingMinMaxMissingValues()
{
	m_vmin.resize(pg.Shape.NumVar);
	m_vmax.resize(pg.Shape.NumVar);

	for(size_t i=0;i<pg.Shape.NumVar;i++) {
		m_vmin[i]=1.1E38f;
		m_vmax[i]=-1.1E38f;
		if (pg.Shape.NumRepetitions>1&&pg.Des.addinfo.size()> (i + m_countf*pg.Shape.NumVar)) {
			pg.Des.addinfo[i + m_countf*pg.Shape.NumVar].num_missing = 0;
			pg.Des.addinfo[i + m_countf*pg.Shape.NumVar].num_substituted = 0;
		}
		else if (pg.Des.addinfo.size() > i) {
			pg.Des.addinfo[i].num_missing = 0;
			pg.Des.addinfo[i].num_substituted = 0;
		}

	}

}
bool PGFileImportBase::SetTimeVariable(size_t nrec, string varstr, size_t nvar, bool Shift)	{
	//size_t npos=varstr.size();
	bool CompleteSet=false;


	
if (nvar == m_CurrentFileInfo.DateColumn) {
	if (m_CurrentFileInfo.IdentifiedDateFormat == DMY_SLASH) {
		m_CurrentMin = PGUtil::MinutConv(PGUtil::DateConv(DMY_SLASH, varstr));
		if (varstr.size()>11) return true;
		else if (m_CurrentFileInfo.HourColumn == string::npos&&m_CurrentFileInfo.TimeColumn == string::npos) return true;	
	}
	else if (m_CurrentFileInfo.IdentifiedDateFormat == YMD) {
		m_CurrentMin = PGUtil::MinutConv(PGUtil::DateConv(m_CurrentFileInfo.IdentifiedDateFormat, varstr));
		if (m_CurrentFileInfo.HourColumn != string::npos || m_CurrentFileInfo.TimeColumn != string::npos) return false;

		if (varstr.size()>8) return true;
		if (m_CurrentFileInfo.HourColumn == string::npos&&m_CurrentFileInfo.TimeColumn == string::npos) return true;

	}
	else if(m_CurrentFileInfo.IdentifiedDateFormat == YMDHM) {	
		m_CurrentMin = PGUtil::MinutConv(PGUtil::DateConv(m_CurrentFileInfo.IdentifiedDateFormat, varstr));
		if (varstr.size()>=6) return true;
	}
	else {
		m_CurrentMin = PGUtil::MinutConv(varstr);
		if (varstr.size()>11) return true;
		if (m_CurrentFileInfo.HourColumn == string::npos&&m_CurrentFileInfo.TimeColumn == string::npos) return true;
	}
}
else if (nvar == m_CurrentFileInfo.YearColumn) {
	if (varstr.size() == 4) {
		varstr += "0101";
	}
	m_CurrentMin = PGUtil::MinutConv(PGUtil::DateConv(YMD, varstr));

}
else if (nvar == m_CurrentFileInfo.DayNumberColumn) {
	m_CurrentMin += (FUtil::AtoInt(varstr) - 1) * 1440;
}
else if (nvar == m_CurrentFileInfo.TimeColumn|| nvar == m_CurrentFileInfo.HourColumn) {
	m_CurrentMin-=720;
	if (varstr.size() == 0) {
		if (m_PreviousMin != string::npos&&m_CurrentMinStep!=string::npos) {
			m_CurrentMin = m_PreviousMin + m_CurrentMinStep;
				string datum = PGUtil::STD_StringDatum(m_CurrentMin);
		}


	}
	else if(varstr.find(":")!=string::npos) {
		string datum = PGUtil::STD_StringDatum(m_CurrentMin);
		datum = datum.substr(0, 11) + varstr;
		m_CurrentMin = PGUtil::MinutConv(datum);
		if (m_PreviousMin != string::npos) {
			m_CurrentMinStep = m_CurrentMin - m_PreviousMin;
		}
		m_PreviousMin = m_CurrentMin;
	}
	else {
		if (varstr.size() < 3) {
  			varstr = "00:" + varstr;
		}
		else if (varstr.size() == 3) {
			varstr = varstr.substr(0,1) + ":" + varstr.substr(1, 2);
			varstr = "0" + varstr.substr(0, 4);
		}
		else if (varstr.size() == 4) {
			varstr = varstr.substr(0, 2) + ":"+ varstr.substr(2, 2);
		}
		string datum = PGUtil::STD_StringDatum(m_CurrentMin);
		datum = datum.substr(0, 11) + varstr;
		m_CurrentMin = PGUtil::MinutConv(datum);
		if (m_PreviousMin != string::npos) {
			m_CurrentMinStep = m_CurrentMin - m_PreviousMin;
		}
		m_PreviousMin = m_CurrentMin;

	}
	return true;
}
else if (nvar == m_CurrentFileInfo.HourColumn) {
	   m_CurrentMin -= 720;
	   if (varstr.size() == 0) {
		   if (m_PreviousMin != string::npos && m_CurrentMinStep != string::npos) {
			   m_CurrentMin = m_PreviousMin + m_CurrentMinStep;
			   string datum = PGUtil::STD_StringDatum(m_CurrentMin);
		   }


	   }
	   else {
		   string datum = PGUtil::STD_StringDatum(m_CurrentMin);
		   m_CurrentMin += 60 * FUtil::AtoInt(varstr);
		   //datum = datum.substr(0, 11) + varstr;
		   //m_CurrentMin = PGUtil::MinutConv(datum);
		   if (m_PreviousMin == string::npos) {
			   m_CurrentMinStep = m_CurrentMin - m_PreviousMin;
		   }
		   m_PreviousMin = m_CurrentMin;
	   }
	   return true;
   }
else if (m_CurrentFileInfo.IdentifiedDateFormat == YMD) {
	   if (nvar == m_CurrentFileInfo.YearColumn) Currentlinetime.iyear = FUtil::AtoInt(varstr);
	   if (nvar == m_CurrentFileInfo.MonthColumn) Currentlinetime.imonth = FUtil::AtoInt(varstr);
	   if (nvar == m_CurrentFileInfo.DayColumn) {
		   Currentlinetime.iday = FUtil::AtoInt(varstr);
		   Currentlinetime.ihour = 12;
		   Currentlinetime.iminut = 0;
		   m_CurrentMin = PGUtil::MinutConv(Currentlinetime);
		   return true;
	   }

 }
else if (nvar == m_CurrentFileInfo.YearColumn&&m_CurrentFileInfo.DayColumn!=3) {
	m_CurrentMin = PGUtil::MinutConv(PGUtil::DateConv(YMD, varstr));
	if (varstr.size()> 10&& m_CurrentFileInfo.TimeColumn==string::npos&&m_CurrentFileInfo.DateColumn == string::npos)
		return true;
	else
		return false;
}
return false;
/*
if(nvar==0) {
	size_t first, second;
	first=varstr.find('.',0);
	second=varstr.rfind('.');
	if(first==2&&second==5) {
		m_datestring="";
		m_datestring=varstr.substr(6,4);
		m_datestring+="-";
		m_datestring+=varstr.substr(3,2);
		m_datestring+="-";
		m_datestring+=varstr.substr(0,2);

	}
	else if(varstr.size()>=12) {
		FUtil::trim_datestr(varstr);
		itim=PGUtil::MinutConv(varstr);
		pg.Var.vv[0]=*(float*)&itim;
	}
	else if(varstr.size()==4&&first==-1) {
			float vnew=FUtil::AtoFloat(varstr);
			if(!Shift) {
				itim=m_itimbase+size_t(nrec-1)*GetNormalTimeInterval();
				pg.Var.vv[0]=*(float*)&itim;
			}
			else { 
				m_datestring.clear();
				m_datestring=varstr;
				m_datestring+="-";
			}
			
	}
	else if(varstr.size()==8&&first==-1) {
			float vnew=FUtil::AtoFloat(varstr);

				m_datestring=varstr.substr(0,4);
				m_datestring+="-";
				m_datestring+=varstr.substr(4,2);
				m_datestring+="-";
				m_datestring+=varstr.substr(6,2);
				m_datestring+=" ";


	}
	else if(varstr.size()==10&&first==-1) {
				m_datestring=varstr.substr(0,10);
				m_datestring+=" ";
	}
	else if(varstr.size()>0) {
		float vnew=FUtil::AtoFloat(varstr);
		itim=m_itimbase+size_t(vnew*1440.);
		//SetLongTime(nrec+nprevrec,itim);
		pg.Var.vv[0]=*(float*)&itim;
	}
	else {
		itim=m_itimbase+size_t(nrec-1)*GetNormalTimeInterval();
		//SetLongTime(nrec+nprevrec,itim);
		pg.Var.vv[0]=*(float*)&itim;
	}
						
}
	
else if(nvar==1) {
	size_t first, second;
	first=varstr.find(':',0);
	second=varstr.rfind(':');
	if(first>=0&&!string::npos||(first==2&&second==2)) {
		m_datestring+=varstr.substr(0,5);
		itim=PGUtil::MinutConv(m_datestring);
		pg.Var.vv[0]=*(float*)&itim;				
	}
	else if(m_datestring.size()==10) {
		m_datestring+=" 12:00";
		itim=PGUtil::MinutConv(m_datestring);
		pg.Var.vv[0]=*(float*)&itim;
	}
	else if(m_datestring.size()==11) {
		if(varstr.size()==2)
			m_datestring+=varstr;
		else if(varstr.size()==1)
			m_datestring+="0"+varstr;
		m_datestring+=":00";
		itim=PGUtil::MinutConv(m_datestring);
		pg.Var.vv[0]=*(float*)&itim;
		pg.Var.vv[1]=FUtil::AtoFloat(varstr);
	}
	else
		itim=0;
	if(second==6) {
		pg.Var.vv[1]=float(FUtil::AtoFloat(varstr.substr(0,3))*3600.+FUtil::AtoFloat(varstr.substr(4,2))*60.+
		FUtil::AtoFloat(varstr.substr(6+1,2)));
	}
	else if(Shift) {
		if(varstr.size()==2)
			m_datestring+=varstr+"-";
		else if(varstr.size()==1)
			m_datestring+="0"+varstr+"-";

	}
	else if(second==2)
		;
	else {
		pg.Var.vv[1]=FUtil::AtoFloat(varstr);
					

		}


	}
	else if(nvar==2) {
		if(varstr.size()==2)
			m_datestring+=varstr+" ";
		else if(varstr.size()==1)
			m_datestring+="0"+varstr+" ";	
		if(m_format==SEPARATED_YMD||m_format==SWISS) {
			m_datestring+="12:00";
			itim=PGUtil::MinutConv(m_datestring);
			pg.Var.vv[0]=*(float*)&itim;					
		}
	}
	else if(nvar==3) {
		if(varstr.size()==2)
			m_datestring+=varstr+" ";
		else if(varstr.size()==1)
			m_datestring+="0"+varstr+":";	
	}
	else if(nvar==4) {
		if(varstr.size()==2)
			m_datestring+=varstr;
		else if(varstr.size()==1)
			m_datestring+="0"+varstr;	
		if(nrec==1) m_minst_a=itim;
		itim=PGUtil::MinutConv(m_datestring);
		pg.Var.vv[0]=*(float*)&itim;
	}*/
}
bool PGFileImportBase::SetDefinedTime(size_t nrec1)
{
	auto it = m_CheckedTimeMap.find(nrec1);
	if (it != m_CheckedTimeMap.end()) {

		m_CurrentMin = (*it).second;
		return true;
	}
	else {
		return false;

	}
}
size_t PGFileImportBase::GetMinutFromLine(string line) {
	string datestring;

	if(m_format==NOAA) {
		datestring=line.substr(14,4);
		datestring+="-";
		datestring+=line.substr(18,2);
		datestring+="-";
		datestring+=line.substr(20,2);
		datestring+=" 12:00";
		return PGUtil::MinutConv(datestring);
	}
	else
		return 0;
}
void PGFileImportBase::SetPGTimeVar_Minut_To_PG_Var() {
	pg.Var.vv[0] = *(float*)&m_CurrentMin;
}
void PGFileImportBase::WriteCompleteRecord() {
	for(size_t j=0;j<pg.Shape.NumVar;j++) {
		if(abs(pg.Var.vv[j+1])<1.E37) {
			if(pg.Var.vv[j+1]<m_vmin[j]) m_vmin[j]=pg.Var.vv[j+1];
			if(pg.Var.vv[j+1]>m_vmax[j]) m_vmax[j]=pg.Var.vv[j+1];
		}
		else {	
			size_t ind=m_countf*pg.Shape.NumVar+j;
			if(pg.Des.addinfo.size()>ind)
				pg.Des.addinfo[ind].num_missing++;
			else if(pg.Des.addinfo.size()>j)
				pg.Des.addinfo[j].num_missing++;
		}
	}

	//auto koll = m_MainPGStreamReadWrite.tellp();
	//auto nrec_koll = koll / pg.Shape.RecordSize - 1;
	m_MainPGStreamReadWrite.write((char*)&pg.Var.vv[0],pg.Shape.RecordSize);

	for(size_t j=0;j<pg.Shape.NumVar;j++)
		pg.Var.vv[j+1]=float(MISSING);
}
void PGFileImportBase::SetValidVariable(size_t index, string varstr) {
	if (varstr == "G") pg.Var.vv[index] = 0;
	else if (varstr == "Y") pg.Var.vv[index] = 1;
	else pg.Var.vv[index]=FUtil::AtoFloat(varstr);

}
void PGFileImportBase::SetMissingVariable(size_t index) {
pg.Var.vv[index]=MISSING;

}


void PGFileImportBase::AdjustValuesToNewUnits() {

	if(m_format==SWISS) {
		if(pg.Var.vv[5]<=-99) pg.Var.vv[5]=MISSING;
		else if(pg.Var.vv[5]<0) pg.Var.vv[5]=0.;
		else pg.Var.vv[5]=pg.Var.vv[5]*86400.f;

	}
	else if (m_format == PROFOUND) {
		if (m_ProFoundOption == 3) {
			for (size_t i = 0; i < 10; i++) {
				float value = pg.Var.vv[(i + 1) * 2 - 1];
				if (value > 0.9*MISSING) {
					if (pg.Var.vv[(i + 1) * 2] < .5) pg.Var.vv[(i + 1) * 2 - 1] = value;
					else pg.Var.vv[(i + 1) * 2 - 1] = MISSING;
					if (pg.Var.vv[(i + 1) * 2] < 1.5) pg.Var.vv[(i + 1) * 2] = value;
					else pg.Var.vv[(i + 1) * 2] = MISSING;
				}
				else
					pg.Var.vv[(i + 1) * 2 - 1] = MISSING;



			}


		}
		else if (m_ProFoundOption == 2) {

			for (size_t i = 0; i < 1; i++) {
				float value = pg.Var.vv[(i + 1) * 2 - 1]*1.02;
				if (value > 0.9*MISSING) {
					if (pg.Var.vv[(i + 1) * 2] < .5) pg.Var.vv[(i + 1) * 2 - 1] = value;
					else pg.Var.vv[(i + 1) * 2 - 1] = MISSING;
					if (pg.Var.vv[(i + 1) * 2] < 1.5) pg.Var.vv[(i + 1) * 2] = value;
					else pg.Var.vv[(i + 1) * 2] = MISSING;
				}
				else
					pg.Var.vv[(i + 1) * 2 - 1] = MISSING;
			}

			for (size_t i = 0; i < 2; i++) {
				pg.Var.vv[ 3 +i]= pg.Var.vv[3+i] * 1.02;

				float value = pg.Var.vv[(i + 1) * 2 +3]*86400.;
				if (value > 0.9*MISSING) {
					if (pg.Var.vv[(i + 1) * 2+4] < .5) pg.Var.vv[(i + 1) * 2 +3] = value;
					else pg.Var.vv[(i + 1) * 2 +3] = MISSING;
					if (pg.Var.vv[(i + 1) * 2+4] < 1.5) pg.Var.vv[(i + 1) * 2+4] = value;
					else pg.Var.vv[(i + 1) * 2+4] = MISSING;
				}
				else
					pg.Var.vv[(i + 1) * 2 +3] = MISSING;
			}
		

		}
		else if (m_ProFoundOption == 1) {
			for (size_t i = 0; i < 2; i++) {
				pg.Var.vv[2 + i] = pg.Var.vv[2 + i] * 86400.;
			}
			pg.Var.vv[4] = pg.Var.vv[4] * 100.;
			pg.Var.vv[5] = pg.Var.vv[5] * 48.;



		}



	}
	else if	(m_format == SILO) {
		pg.Var.vv[1] *= 1.E6;
		pg.Var.vv[5] *= 100.;

	}
}
