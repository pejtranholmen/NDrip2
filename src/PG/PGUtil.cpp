#pragma once
#include "../std.h"
#include "./PGUtil.h"
#include "../Util/FUtil.hpp"
#ifndef NO_MS_GUI
#include "../Util/ModelUtil.h"
#endif
//Changing
extern "C" void cdatum(unsigned int &julian)

{  int jul, jalpha, ja, jb, jc, jd, je;
   int  year, month, day, hour, minut;
   char buffer[20];


   jul = julian;
   minut = jul % (60 * 24);
   hour =  minut / 60;
   minut -= hour * 60;
   jul = (jul - minut - hour * 60) / 60 / 24 + BASE;
   ja = jul;

   if (jul >= 2299161L)
   {  jalpha = size_t((double(jul - 1867216L) - 0.25) / 36524.25);
      ja = jul + 1 + jalpha - jalpha/4;
   }

   jb=ja+1524;
   jc = 6680 + size_t((double(jb - 2439870L) - 122.1) / 365.25);
   jd = 365 * jc + jc/4;
   je = size_t(double(jb - jd) / 30.6001);
   day = jb - jd - size_t(30.6001 * double(je));
   month = je - 1;

   if (month > 12)
      month -= 12;
   
   year = jc - 4715;
   
   if (month > 2)
      year -= 1;

   if (year <= 0)
      year -= 1;

	//stmp=newdate
	//stmp=(PCXSTR)newdate;
//	stmp=newdate;
//	return smtp;
	snprintf(buffer,12, "%04u%02u%02u%02u%02u", year, month, day, hour, minut);

	//return buffer;
  // ;memcpy(sdate, stmp, 12);
}
extern "C" unsigned int cminut(char *date)
//char  *date;
{  int jy,jm,ja,jd;
   int   iyear, imonth, iday, ihour, iminut;
/*   string Datum;
   Datum=date;
   iyear=AtoInt(Datum.Mid(0,4));
   imonth=AtoInt(Datum.Mid(4,2));
   iday=AtoInt(Datum.Mid(6,2));
   ihour=AtoInt(Datum.Mid(8,2));
   iminut=AtoInt(Datum.Mid(10,2));*/
#ifdef MS_CODE
   sscanf_s(date,     "%4u", &iyear);
   sscanf_s(&date[4], "%2u", &imonth);
   sscanf_s(&date[6], "%2u", &iday);
   sscanf_s(&date[8], "%2u", &ihour);
   sscanf_s(&date[10],"%2u", &iminut);
#else
   sscanf(date, "%4u", &iyear);
   sscanf(&date[4], "%2u", &imonth);
   sscanf(&date[6], "%2u", &iday);
   sscanf(&date[8], "%2u", &ihour);
   sscanf(&date[10], "%2u", &iminut);
#endif

   if (iyear < 0) 
      iyear++;

   if (imonth > 2)
   {  jy = iyear;
      jm = imonth + 1;
   }
   else
   {  jy = iyear - 1;
      jm = imonth + 13;
   }

   jd = int(365.25 * jy) + int (30.6001 * jm) + iday + 1720995L;

   if (iday + 31L * (imonth + 12L * iyear) > 588829L)
   {  ja =  jy/100;
      jd += 2 - ja + int(0.25 * ja);
   }
   jd -= BASE;
   jd = jd * 24 * 60 + ihour * 60 + iminut;
   return jd;
}
PGUtil::PGUtil(void)
{

}


PGUtil::~PGUtil(void)
{
}
string PGUtil::DateConv(dateformats d, string instr)
{
		string outstr;
		auto p1 = instr.find("\\");
		auto p2 = instr.rfind("\\");
		if (p1 == string::npos) {
			p1 = instr.find("/");
			p2 = instr.rfind("/");
		}



		if (p1!=string::npos) {
			if (d == DMY_SLASH) {
				outstr = instr.substr(p2 + 1); outstr += "-";
				if (p2 - p1 == 2) outstr += "0";
				outstr += instr.substr(p1 + 1, p2 - p1 - 1);
				outstr += "-";
				if (p1 == 1) outstr += "0";
				outstr += instr.substr(0, p1);


			}
			else {
				outstr = instr.substr(p2 + 1); outstr += "-";
				if (p1 == 1) outstr += "0";
				outstr += instr.substr(0, p1);
				outstr += "-";
				if (p2 - p1 == 2) outstr += "0";
				outstr += instr.substr(p1 + 1, p2 - p1 - 1);
			}

			

			return outstr;
		}
		return instr;
}
/*void PGUtil::setpgaddinfo(char *filename, int *irep, float *x, float *y, float *z)
{
	string s_filename=filename;
	trim(s_filename);
	int i=s_filename.size();
	s_filename.append(".bin");
	CPG pg;
	if(pg.Open(s_filename)) {
		pg.SetInfoMode();
			if(*irep>1) {
				int add=(*irep-1)*pg.GetNumVariables();
				pg.AddNewRepititionDescriptions(add);
			}
		
			for(int i=0; i<pg.GetNumVariables();i++) {
			pg.SetVarAlt(i+1, double(*z), *irep); 
			pg.SetVarLong(i+1, double(*x), *irep); 
			pg.SetVarLat(i+1, double(*y), *irep); 

		}
		pg.SaveAs(s_filename);
		pg.CloseFile();
	}
}
long PGUtil::MinutConv2(char* str)
{
	string Datum;
	Datum=str;
	return MinutConv(Datum);
}*/
IPGTID PGUtil::Ato_IPGTID(string Datum) {
	IPGTID out;
   auto num=Datum.size();
   auto nhourpos = Datum.find(":");
   auto secondhourpos = Datum.rfind(":");
   
   auto ndatepos = Datum.find("-");
   if (secondhourpos > nhourpos && ndatepos == string::npos) {
	   out.iyear = 1900;
	   out.imonth = 1;
	   out.iday = 1;
	   out.ihour = AtoInt(Datum.substr(0, 2));
	   out.iminut = AtoInt(Datum.substr(nhourpos + 1, 2));

   }
   else if (nhourpos == string::npos&&ndatepos == string::npos) {
	   if (num <= 4) {
		   out.iyear = AtoInt(Datum.substr(0, 4)); out.imonth = 1; out.iday = 1;
		   out.ihour = 12;
		   out.iminut = 0;
	   }
	   else if (num <= 6) {


		   out.iyear = AtoInt(Datum.substr(0, 4));
		   if (out.iyear < 1900) {
			   out.iyear = AtoInt(Datum.substr(0, 2))+2000;
			   out.imonth = AtoInt(Datum.substr(2, 2)); 
			   out.iday = AtoInt(Datum.substr(4, 2));
			   out.ihour = 12;
			   out.iminut = 0;
		   }
		   else {
			   out.imonth = AtoInt(Datum.substr(4, 2));  out.iday = 1;
			   out.ihour = 12;
			   out.iminut = 0;
		   }
	   }
	   else if (num <= 8) {
		   out.iyear = AtoInt(Datum.substr(0, 4));
		   out.imonth = AtoInt(Datum.substr(4, 2));
		   out.iday = AtoInt(Datum.substr(6, 2));
		   out.ihour = 12;
		   out.iminut = 0;
	   }
	   else if (num <= 11) {
		   out.iyear = AtoInt(Datum.substr(0, 4));
		   out.imonth = AtoInt(Datum.substr(4, 2));
		   out.iday = AtoInt(Datum.substr(6, 2));
		   out.ihour = AtoInt(Datum.substr(8, 2));
	   }
	   else if (num <= 13) {
		   out.iyear = AtoInt(Datum.substr(0, 4));
		   out.imonth = AtoInt(Datum.substr(4, 2));
		   out.iday = AtoInt(Datum.substr(6, 2));
		   out.ihour = AtoInt(Datum.substr(8, 2));
		   out.iminut = AtoInt(Datum.substr(10, 2));
	   }

   } 
   else if (nhourpos == string::npos){
	   if(num>=12) {
			out.iyear=AtoInt(Datum.substr(0,4));
			out.imonth=AtoInt(Datum.substr(4,2));
			out.iday=AtoInt(Datum.substr(6,2));
			out.ihour=AtoInt(Datum.substr(8,2));
			out.iminut=AtoInt(Datum.substr(10,2));
			return out;
	   }
	   else if(num>8) {
		out.iyear=AtoInt(Datum.substr(0,4));
		out.imonth=AtoInt(Datum.substr(5,2));
		out.iday=AtoInt(Datum.substr(8,2));
	   }
	   else if(num>5) {
		out.iyear=AtoInt(Datum.substr(0,4));
		out.imonth=AtoInt(Datum.substr(4,2));
		out.iday=AtoInt(Datum.substr(6,2));
	   }
	   out.ihour=12;
	   out.iminut=0;

   }
   else {
	   if (num <= 4) {
		   out.iyear = AtoInt(Datum.substr(0, 4));
	   }
	   else if (num<=5){
		   out.ihour = AtoInt(Datum.substr(nhourpos - 2, 2));
		   out.iminut = AtoInt(Datum.substr(nhourpos + 1, 2));
	   }
	 else if (num>10&&num <= 14) {
		   out.iyear = AtoInt(Datum.substr(0, 4));
		   out.imonth = AtoInt(Datum.substr(4, 2));
		   out.iday = AtoInt(Datum.substr(6, 2));
		   out.ihour = AtoInt(Datum.substr(8, 2));
		   out.iminut = AtoInt(Datum.substr(10, 2));

	   }
	   else {
		   out.iyear = AtoInt(Datum.substr(0, 4));
		   out.imonth = AtoInt(Datum.substr(5, 2));
		   out.iday = AtoInt(Datum.substr(8, 2));
		   out.ihour = AtoInt(Datum.substr(nhourpos - 2, 2));
		   out.iminut = AtoInt(Datum.substr(nhourpos + 1, 2));
	   }
   }
   return out;

}
unsigned int PGUtil::MinutConv(string Datum, bool Daily, bool Hourly)
{ 
  // int iyear,imonth,iday, ihour, iminut;
	IPGTID tid=Ato_IPGTID(Datum);
	if(Daily) {
		tid.ihour=0;
		tid.iminut=0;
	}
	else if(Hourly) tid.iminut=0;

	return MinutConv(tid);

}
unsigned int PGUtil::MinutConv(IPGTID tid)
{ 
	int jy,jm,ja,jd;
  // int iyear,imonth,iday, ihour, iminut;

   if (tid.iyear < 0) 
      tid.iyear++;

   if (tid.imonth > 2)
   {  jy = tid.iyear;
      jm = tid.imonth + 1;
   }
   else
   {  jy = tid.iyear - 1;
      jm = tid.imonth + 13;
   }

   jd = size_t (365.25 * jy) + size_t (30.6001 * jm) + tid.iday + 1720995L;

   if (tid.iday + 31L * (tid.imonth + 12L * tid.iyear) > 588829L)
   {  ja = jy/100;
      jd += 2 - ja +  ja/4;
   }
   jd -= BASE;
   jd = jd * 24 * 60 + tid.ihour * 60 + tid.iminut;
   return (jd);
}
string PGUtil::StringDatumSec(double sec) {
	size_t min=size_t(sec/60.)+2880;
	return StringDatum(min);
}

string PGUtil::StringDatum(unsigned int julian)
{  int jul, jalpha, ja, jb, jc, jd, je;
   int  year, month, day, hour, minut;


   string Datum;
   char tmp[20];

   jul = julian;
   minut = jul % (60 * 24);
   hour =  minut / 60;
   minut -= hour * 60;
   jul = (jul - minut - hour * 60) / 60 / 24 + BASE;
   ja = jul;

   if (jul >= 2299161L)
   {  jalpha = int(((jul - 1867216L) - 0.25) / 36524.25);
      ja = jul + 1 + jalpha - int (0.25 * jalpha);
   }

   jb=ja+1524;
   jc = int(6680.0 + ((jb - 2439870L) - 122.1) / 365.25);
   jd = 365 * jc + int(0.25 * jc);
   je = int((jb - jd) / 30.6001);
   day = jb - jd - int(30.6001 * je);
   month = je - 1;

   if (month > 12)
      month -= 12;
   
   year = jc - 4715;
   
   if (month > 2)
      year -= 1;

   if (year <= 0)
      year -= 1;

   if(minut<0) minut=1;
  // tmp.Format(_T("%04u"), year);
   if(julian<0) 
	   return "";
   else {
		snprintf(tmp,17,"%04u-%02u-%02u %02u:%02u",year,month,day,hour,minut);
		return tmp;
   }
}
string PGUtil::STD_StringDatum(unsigned int min) {
	return string(StringDatum(min));
}
double PGUtil::DayNumFunction(unsigned int min) {
	string dat=STD_StringDatum(min);
	string ny=dat.substr(0,4)+"01010000";
	auto minst=MinutConv(ny);
	return (min-minst)/1440.+1;
}
int PGUtil::YearNumFunction(unsigned int min) {
	string dat = STD_StringDatum(min);
	int year = AtoInt(dat.substr(0,4));
	return year;
}
size_t  PGUtil::YearFunction(unsigned int julian)
{  unsigned int jul, jalpha, ja, jb, jc, jd, je;
   unsigned int  year, month, day, hour, minut;
   string Datum; string tmp;

   jul = julian;
   minut = jul %1440;
   hour =  minut / 60;
   minut -= hour * 60;
   jul = (jul - minut - hour * 60) / 60 / 24 + BASE;
   ja = jul;

   if (jul >= 2299161L)
   {  jalpha = size_t(((jul - 1867216L) - 0.25) / 36524.25);
      ja = jul + 1 + jalpha - jalpha/4;
   }

   jb=ja+1524;
   jc = size_t(6680.0 + ((jb - 2439870L) - 122.1) / 365.25);
   jd = 365 * jc + jc/4;
   je = size_t((jb - jd) / 30.6001);
   day = jb - jd - size_t(30.6001 * je);
   month = je - 1;

   if (month > 12)
      month -= 12;
   
   year = jc - 4715;
   
   if (month > 2)
      year -= 1;

   if (year <= 0)
      year -= 1;

  return year;
}
int  PGUtil::MonthFunction(unsigned int julian)
{  unsigned int jul, jalpha, ja, jb, jc, jd, je;
   int  year, month, day, hour, minut;
   string Datum; string tmp;

   jul = julian;
   minut = jul % (60 * 24);
   hour =  minut / 60;
   minut -= hour * 60;
   jul = (jul - minut - hour * 60) / 60 / 24 + BASE;
   ja = jul;

   if (jul >= 2299161L)
   {  jalpha = int(((jul - 1867216L) - 0.25) / 36524.25);
      ja = jul + 1 + jalpha - int (0.25 * jalpha);
   }

   jb=ja+1524;
   jc = long(6680.0 + ((jb - 2439870L) - 122.1) / 365.25);
   jd = 365 * jc + ((long) (0.25 * jc));
   je = long((jb - jd) / 30.6001);
   day = jb - jd - ((long) (30.6001 * je));
   month = je - 1;

   if (month > 12)
      month -= 12;
   
   year = jc - 4715;
   
   if (month > 2)
      year -= 1;

   if (year <= 0)
      year -= 1;



  return month;
}
int  PGUtil::DayFunction(unsigned int julian)
{  int jul, jalpha, ja, jb, jc, jd, je;
   int  year, month, day, hour, minut;
   string Datum; string tmp;

   jul = julian;
   minut = jul % (60 * 24);
   hour =  minut / 60;
   minut -= hour * 60;
   jul = (jul - minut - hour * 60) / 60 / 24 + BASE;
   ja = jul;

   if (jul >= 2299161L)
   {  jalpha = int(((jul - 1867216L) - 0.25) / 36524.25);
      ja = jul + 1 + jalpha - int (0.25 * jalpha);
   }

   jb=ja+1524;
   jc = int(6680.0 + ((jb - 2439870L) - 122.1) / 365.25);
   jd = 365 * jc + int (0.25 * jc);
   je = int((jb - jd) / 30.6001);
   day = jb - jd - int( 30.6001 * je);
   month = je - 1;

   if (month > 12)
      month -= 12;
   
   year = jc - 4715;
   
   if (month > 2)
      year -= 1;

   if (year <= 0)
      year -= 1;


  return day;
}
string PGUtil::StringDatumStart(unsigned int value, unsigned int range)
{

	string strSt;
	int longSt ;
	string str=StringDatum(value);
	if(range<720) 
		return str=str.substr(0,13)+"00";
	else if(range<=1440) {
		longSt=MinutConv(str.substr(0,10)+"00:00");
		int n=(value-longSt)/120;
		value=longSt+n*120;
		return StringDatum(value);
	}
	else if(range<=4320) {
		longSt=MinutConv(str.substr(0,10)+"00:00");
		int n=(value-longSt)/240;
		value=longSt+n*240;
		return StringDatum(value);
	}
	else if(range<=21600) {
		longSt=MinutConv(str.substr(0,7)+"01 00:00");
		int n=(value-longSt)/1440;
		value=longSt+n*1440;
		return StringDatum(value);
	}
	else if(range<=43200) {
		longSt=MinutConv(str.substr(0,7)+"01 00:00");
		int n=(value-longSt)/4320;
		value=longSt+n*4320;
		return StringDatum(value);
	}
	else if(range<=129600) {
		longSt=MinutConv(str.substr(0,7)+"01 00:00");
		int n=(value-longSt)/14400;
		value=longSt+n*14400;
		return StringDatum(value);
	}
	else if(range<=1152000) {
		longSt=MinutConv(str.substr(0,7)+"01 00:00");
		value=longSt;
		return StringDatum(value);
	}
	else {
		return str.substr(0,5)+"01-01 00:00";
	}


}
string PGUtil::STD_StringDatumStart(unsigned int min, unsigned int range) {
	return string(StringDatumStart(min,range)  ); 
} 

string PGUtil::StringDatumEnd(unsigned int value, unsigned int start, int step)
{
int range=value-start;
	unsigned int newvalue;
//	int StMonth;
	unsigned int longSt=start;
	value=value-1;
	string str=StringDatum(value);
	string strst=StringDatum(start);
	int iyear=AtoInt(strst.substr(0,4));
	int imonth=AtoInt(strst.substr(5,2));
	if(imonth>12) {
		iyear=iyear+1;
		imonth=1;
	}
	char strmonth[2], stryear[4];
	//strmonth.Format(_T("%02u"), imonth); stryear.Format(_T("%04u"),iyear);
	snprintf(strmonth,2,"%02u",imonth);
	snprintf(stryear,4,"%04u",iyear);
	if(range<720) {
		int n=(value-longSt)/60+1+step;
		newvalue=longSt+n*120;
		return StringDatum(newvalue);
	}
	else if(range<=1440) {
		int n=(value-longSt)/120+1+step;
		value=longSt+n*120;
		return StringDatum(value);
	}
	else if(range<=4320) {
		int n=(value-longSt)/240+1+step;
		value=longSt+n*240;
		return StringDatum(value);
	}
	else if(range<=21600) {
		int n=(value-longSt)/1440+1+step;
		value=longSt+n*1440;
		return StringDatum(value);
	}
	else if(range<=43200) {
		int n=(value-longSt)/4320+1+step;
		value=longSt+n*4320;
		return StringDatum(value);
	}
	else if(range<=129600) {
		int n=(value-longSt)/14400+1+step;
		value=longSt+n*14400;
		return StringDatum(value);
	}
	else if(range<=345600) {
		str=stryear;str+="-";str+=strmonth;str+="-01 00:00";
		longSt=MinutConv(str);
		while(value>longSt){
			imonth++;
			if(imonth>12) {
				iyear=iyear+1;
				imonth=1;
			}

			str=stryear;str+="-";str+=strmonth;str+="-01 00:00";
			longSt=MinutConv(str);
		}
		str=StringDatum(longSt);
		imonth=imonth+step;
		if(imonth>12) {
			iyear=iyear+1;
			imonth=imonth-12;
		}
		else if(imonth<1) {
			iyear=iyear-1;
			imonth=imonth+12;
		}
		str=stryear;str+="-";str+=strmonth;str+="-01 00:00";
		longSt=MinutConv(str);
		return StringDatum(longSt);
	}
 	else if(range<=576000) {
		str=stryear;str+="-";str+=strmonth;str+="-01 00:00";
		longSt=MinutConv(str);
		while(value>longSt){
			imonth=imonth+2;
			if(imonth>12) {
				iyear=iyear+1;
				imonth=imonth-12;
			}
			str=stryear;str+="-";str+=strmonth;str+="-01 00:00";
			longSt=MinutConv(str);
		}
		str=StringDatum(longSt);
		imonth=imonth+step*2;
		if(imonth>12) {
			iyear=iyear+1;
			imonth=imonth-12;
		}
		else if(imonth<1) {
			iyear=iyear-1;
			imonth=imonth+12;
		}
		str=stryear;str+="-";str+=strmonth;str+="-01 00:00";
		longSt=MinutConv(str);
		return StringDatum(longSt);
	}
	else if(range<=1152000) {
		str=stryear;str+="-";str+=strmonth;str+="-01 00:00";
		longSt=MinutConv(str);
		while(value>longSt){
			imonth=imonth+3;
			if(imonth>12) {
				iyear=iyear+1;
				imonth=imonth-12;
			}
			str=stryear;str+="-";str+=strmonth;str+="-01 00:00";
			longSt=MinutConv(str);


		}
		str=StringDatum(longSt);
		imonth=imonth+step*3;
		if(imonth>12) {
			iyear=iyear+1;
			imonth=imonth-12;
		}
		else if(imonth<1) {
			iyear=iyear-1;
			imonth=imonth+12;
		}

		str=stryear;str+="-";str+=strmonth;str+="-01 00:00";
		longSt=MinutConv(str);

		return StringDatum(longSt);

	}
	else {
		longSt=MinutConv(str.substr(0,7)+"01 00:00");
		while(value>longSt) {
	 		int iyear=AtoInt(str.substr(0,4))+1;
			snprintf(stryear,4, "%04u",iyear);
			str=stryear;str+="-01-01 00:00";
			longSt=MinutConv(str);
			value=longSt;
		}
		str=StringDatum(value);
		if(step<0) {
			int iyear=AtoInt(str.substr(0,4))-1;
			snprintf(stryear,4, "%04u",iyear);
			str=stryear;str+="-01-01 00:00";
			longSt=MinutConv(str);
			value=longSt;
		}
		else if(step>0) {
			int iyear=AtoInt(str.substr(0,4))+1;
			snprintf(stryear,4, "%02u",iyear);
			str=stryear;str+="-01-01 00:00";
			longSt=MinutConv(str);
			value=longSt;
		}
		return StringDatum(value);
	}


}
string PGUtil::STD_StringDatumEnd(unsigned int min, unsigned int start, int step){
	return string(StringDatumEnd(min,start, step));
}
string PGUtil::StringDatumStartEstimate(unsigned int value, unsigned int start)
{
	unsigned int range=start-value;
	int newvalue;
//	int StMonth;
	int longSt=start;
	value=value+1;
	string str=StringDatum(value);
	string strst=StringDatum(start);
	int iyear=AtoInt(strst.substr(0,4));
	int imonth=AtoInt(strst.substr(5,2));
	if(imonth>12) {
		iyear=iyear+1;
		imonth=1;
	}
	char strmonth[2], stryear[4];
	//strmonth.Format(_T("%02u"), imonth); stryear.Format(_T("%04u"),iyear);
	snprintf(strmonth,2,"%02u",imonth);
	snprintf(stryear,4, "%04u",iyear);

	if(range<720) {
		int n=(value-longSt)/60-1;
		newvalue=longSt+n*120;
		return StringDatum(newvalue);
	}
	else if(range<=1440) {
		int n=(value-longSt)/120-1;
		value=longSt+n*120;
		return StringDatum(value);
	}
	else if(range<=4320) {
		int n=(value-longSt)/240-1;
		value=longSt+n*240;
		return StringDatum(value);
	}
	else if(range<=21600) {
		int n=(value-longSt)/1440-1;
		value=longSt+n*1440;
		return StringDatum(value);
	}
	else if(range<=43200) {
		int n=(value-longSt)/4320-1;
		value=longSt+n*4320;
		return StringDatum(value);
	}
	else if(range<=129600) {
		int n=(value-longSt)/14400-1;
		value=longSt+n*14400;
		return StringDatum(value);
	}
	else if(range<=345600) {
		str=stryear;str+="-";str+=strmonth;str+="-01 00:00";

		longSt=MinutConv(str);
		while(value<longSt){
			imonth--;
			if(imonth<1) {
				iyear=iyear-1;
				imonth=imonth+12;
			}
			snprintf(strmonth,2,"%02u",imonth);
			snprintf(stryear,4, "%04u",iyear);
			str=stryear;str+="-";str+=strmonth;str+="-01 00:00";
			//strmonth.Format(_T("%02u"), imonth); stryear.Format(_T("%04u"),iyear);
			longSt=MinutConv(str);
		}
		return StringDatum(longSt);
	}
 	else if(range<=576000) {
		str=stryear;str+="-";str+=strmonth;str+="-01 00:00";
		longSt=MinutConv(str);
		while(value<longSt){
			imonth=imonth-2;
			if(imonth<1) {
				iyear=iyear-1;
				imonth=imonth+12;
			}
			snprintf(strmonth,2,"%02u",imonth);
			snprintf(stryear,4, "%04u",iyear);
			str=stryear;str+="-";str+=strmonth;str+="-01 00:00";
			longSt=MinutConv(str);

		}
		return StringDatum(longSt);
	}
	else if(range<=1152000) {
		str=stryear;str+="-";str+=strmonth;str+="-01 00:00";
		longSt=MinutConv(str);
		while(value<longSt){
			imonth=imonth-3;
			if(imonth<1) {
				iyear=iyear-1;
				imonth=imonth+12;
			}
			snprintf(strmonth,2,"%02u",imonth);
			snprintf(stryear,4, "%04u",iyear);
			str=stryear;str+="-";str+=strmonth;str+="-01 00:00";
			longSt=MinutConv(str);

		}
		return StringDatum(longSt);
	}
	else {

		snprintf(stryear,4, "%04u",iyear);
		str=stryear;str+="-01-01 00:00";
		longSt=MinutConv(str);
		if(value<longSt) {
	 		iyear--;
			snprintf(stryear,4,"%04u",iyear);
			str=stryear;str+="-01-01 00:00";
			longSt=MinutConv(str);
		}
		return StringDatum(longSt);
	}

}

unsigned int PGUtil::cminut(char *date)
//char  *date;
{  size_t jy,jm,ja,jd;
   size_t   iyear, imonth, iday, ihour, iminut;
#ifdef MS_CODE
   sscanf_s(date, "%4u", &iyear);
   sscanf_s(&date[4], "%2u", &imonth);
   sscanf_s(&date[6], "%2u", &iday);
   sscanf_s(&date[8], "%2u", &ihour);
   sscanf_s(&date[10], "%2u", &iminut);
#else
   sscanf(date,     "%4u", &iyear);
   sscanf(&date[4], "%2u", &imonth);
   sscanf(&date[6], "%2u", &iday);
   sscanf(&date[8], "%2u", &ihour);
   sscanf(&date[10],"%2u", &iminut);
#endif
   if (iyear < 0) 
      iyear++;

   if (imonth > 2)
   {  jy = iyear;
      jm = imonth + 1;
   }
   else
   {  jy = iyear - 1;
      jm = imonth + 13;
   }

   jd = size_t( size_t(double(365.25 * jy)) + size_t(30.6001 * double(jm)) + iday) + 1720995L;

   if (iday + 31L * (imonth + 12L * iyear) > 588829L)
   {  ja =  jy/100;
      jd += 2 - ja + ja/4;
   }
   jd -= BASE;
   jd = jd * 24 * 60 + ihour * 60 + iminut;
   return (jd);
}
unsigned int PGUtil::cminutx(int iyear,int imonth,int iday,int ihour,int iminut)
//int   iyear, imonth, iday, ihour, iminut;
{  unsigned int jy,jm,ja,jd;

   if (iyear < 0) 
      iyear++;

   if (imonth > 2)
   {  jy = iyear;
      jm = imonth + 1;
   }
   else
   {  jy = iyear - 1;
      jm = imonth + 13;
   }

   jd = int(365.25 * jy) + int(30.6001 * jm) + iday + 1720995L;

   if (iday + 31L * (imonth + 12L * iyear) > 588829L)
   {  ja = jy/100;
      jd += 2 - ja + int (0.25 * ja);
   }
   jd -= BASE;
   jd = jd * 24 * 60 + ihour * 60 + iminut;
   return jd;
}
//#ifndef COUP2013
string PGUtil::cdatum(unsigned int &julian)

{  int jul, jalpha, ja, jb, jc, jd, je;
   int  year, month, day, hour, minut;
   char buffer[20];


   jul = julian;
   minut = jul % (60 * 24);
   hour =  minut / 60;
   minut -= hour * 60;
   jul = (jul - minut - hour * 60) / 60 / 24 + BASE;
   ja = jul;

   if (jul >= 2299161L)
   {  jalpha = size_t((double(jul - 1867216L) - 0.25) / 36524.25);
      ja = jul + 1 + jalpha - jalpha/4;
   }

   jb=ja+1524;
   jc = 6680 +size_t(double((jb - 2439870L) - 122.1) / 365.25);
   jd = 365 * jc + jc/4;
   je = size_t(double(jb - jd) / 30.6001);
   day = jb - jd - size_t((30.6001 * double(je)));
   month = je - 1;

   if (month > 12)
      month -= 12;
   
   year = jc - 4715;
   
   if (month > 2)
      year -= 1;

   if (year <= 0)
      year -= 1;

	//stmp=newdate
	//stmp=(PCXSTR)newdate;
//	stmp=newdate;
//	return smtp;
	snprintf(buffer,12, "%04u%02u%02u%02u%02u", year, month, day, hour, minut);

	return buffer;
  // ;memcpy(sdate, stmp, 12);
}
//#endif
int	PGUtil::AtoInt(string str) {
	int i;
#ifdef MS_CODE
	sscanf_s(str.data(), "%d", &i);
#else
	sscanf(str.data(),"%d",&i);
#endif
	return i;
}
int PGUtil::HourFunction(unsigned int min) {

	int minday=min%1440;
	return min/60+1;
}
int PGUtil::MinOfHourFunction(unsigned int min) {
	int minHour=min%60;
	return minHour;
}
IPGTID PGUtil::MinToPGTID(unsigned int min) {
	string datum=cdatum(min);
	return Ato_IPGTID(datum);

}
int PGUtil::LengthOfMonth(unsigned int min) {
	IPGTID tid=MinToPGTID(min);
	if(tid.imonth%2==0) {
		if(tid.imonth==2) {
			IPGTID tid2;
			tid2=tid;
			tid2.imonth=3;
			return (MinutConv(tid2)-MinutConv(tid))/1440;
		}
		else 
			return 30;
	}
	else 
		return 31;
}

string PGUtil::CheckedForExistingBinFileName(string filename)
{
    //cout << "original file name: " << filename << endl;
	string filenameout;
	if (!FUtil::IsFileExisting(filename)) {


		auto iposBIN = filename.rfind(".BIN");
		if (iposBIN != string::npos) {
			auto testname = filename.substr(iposBIN+1) + ".bin";
			if (FUtil::IsFileExisting(testname)) return testname;
		}


		string UserDirectory = "";
		UserDirectory = FUtil::GetProfileStringStd("UserDirectory", UserDirectory);
		size_t numo;
		numo = UserDirectory.size();
		if (UserDirectory.rfind("\\") != numo - 1) {
			UserDirectory += "\\";
			numo++;
		}
		size_t n = filename.rfind("\\");
		if (n != string::npos)
			filename = filename.substr(n + 1);

		string testname = UserDirectory + filename;
		if (!FUtil::IsFileExisting(testname)) {
			UserDirectory = FUtil::GetProfileStringStd("UserSubDirectory", UserDirectory);
			size_t ust = testname.rfind("\\");
			string file = testname.substr(ust + 1);

			testname = UserDirectory + file;
			filename = testname;
#ifndef COUPSTD
			if (!FUtil::IsFileExisting(testname)) {
				string str;
				str = "File ";
				str += filename; str += " was not find";
				return "";

			}
			else
				return testname;
#else
            if (!FUtil::IsFileExisting(testname)) {
                
                std::string delim = ".";
                std::string tmpname = testname.substr(0, testname.find(delim));
                std::string tmpext = testname.erase(0, testname.find(delim) + delim.length());
                std::transform(tmpext.begin(), tmpext.end(), tmpext.begin(),
                               [](unsigned char c){ return std::tolower(c); });
                testname = tmpname + "." + tmpext;
                
                if (!FUtil::IsFileExisting(testname)) {
                    
                    if (filename.find("SOIL") == string::npos) {
                        cout << "File name missing: " << filename << endl;
                    }
                } else {
                    filename = testname;
                }
            }
            else
                filename = testname;


#endif

		}
		else
			filename =testname;

	}
	return filename;
}

bool PGUtil::CheckForPossiblePGDateString(string str)
{
	if (str.size() < 6) return false;
	bool valid = false;
	size_t count = 0;
	while (!valid&&count<str.size() ) {
		if (str[count] > 47 && str[count] < 58) valid = true;
		count++;
	}
	if (!valid) return false;

	IPGTID koll;
	koll = Ato_IPGTID(str);
	if (koll.iyear > 0 && koll.imonth > 0 && koll.imonth < 13 && koll.iday>0 && koll.iday < 367) return true;

	return false;
}
