#ifndef COUPSTD
#include "../Newbase/doc.h"
#endif
#include "../std.h"
#include "FUtil.h"

#include <sys/stat.h>
#include "./Register.h"
#ifndef COUPSTD
#include "../NewBase/SimArchive.h"

#endif
char delimchar = ';';

#include "../ModelTypes/OutVector.h"
#include "../ModelTypes/OutSingle.h"
#include "../ModelTypes/SimB.h"
#include "../ModelTypes/Ps.h"
#include "../ModelTypes/Sw.h"
#include "../ModelTypes/Tab.h"
#include "../std.h"
#include "FUtil.h"

#include <cmath>

#include <string>

struct TestAlfaFunctor
{
	bool stringIsCorrect;
	TestAlfaFunctor()
		:stringIsCorrect(true)
	{}

	void operator() (char ch)
	{
		if (stringIsCorrect && !((ch <= 'z' && ch >= 'a') || (ch <= 'Z' && ch >= 'A')))
			stringIsCorrect = false;
	}
};




FUtil::FUtil(void)
{
#ifndef COUPSTD
	CWinApp* m_pApp = AfxGetApp();
#endif
	

}


FUtil::~FUtil(void)
{
}
 std::string FUtil::GetProfileStringStd(std::string item, std::string value) {

#ifndef COUPSTD
	CWinApp* pApp = AfxGetApp();
	CString Comp=pApp->GetProfileString(_T("CoupModel"), CString(item.c_str()), CString(value.c_str()));
	return CT2A(Comp).m_psz;
#else 
#ifndef NO_MS_GUI
    return MySingle::instance().GetRegisterPointer()->GetString(item);
#endif
#endif
	return value;
}
 int FUtil::GetProfileIntNo(string item, int value) {
#ifndef COUPSTD
	CWinApp* pApp = AfxGetApp();
	int Comp= pApp->GetProfileInt(_T("CoupModel"), CString(item.c_str()), value);
	return Comp;
#else
#ifndef NO_MS_GUI
	 return MySingle::instance().GetRegisterPointer()->GetInt(item);
#endif
#endif
	return value;
}

string FUtil::check_open_newcsvfile(fstream *filestream, string filename) {
	 string fileout;
	 filestream->open(filename, ios::out);
	 if (!filestream->is_open()) {

		 string UserDirectory = "";
		 UserDirectory = FUtil::GetProfileStringStd("UserDirectory", UserDirectory);
		 string testname;
		 unsigned long ipos = filename.find("\\");
		 while (ipos != string::npos) {
			 filename = filename.substr(ipos + 1);
			 ipos = filename.find("\\");
		 }
		 testname = UserDirectory + filename;
		 filestream->open(testname, ios::out);
		 if (!filestream->is_open()) {
#ifndef COUPSTD
			 TRACE("Can't Open file %s´\n", filename);
			 //Display error message
			 string str;
			 str = "For some reason the file (";
			 str += testname;
			 str += ") specified couldn´t be opened.";
			 //	 MFC_Util::MessageBox(str, "File Open Error", MB_OK | MB_ICONEXCLAMATION);
#endif		
			 //m_IsMissing=true;	
			 return "";
			 // Actions without file name ?
		 }

		 filename = testname;
		 fileout = filename;
	 }

	 return fileout;
 }
 string FUtil::check_open_newfile(std::fstream *filestream, string filename) {
	 string fileout;
	 filestream->open(filename, ios::binary | ios::in);
	 if (!filestream->is_open()) {

		 string UserDirectory = "";
		 UserDirectory = FUtil::GetProfileStringStd("UserDirectory", UserDirectory);
		 string testname;
		 unsigned long ipos = filename.find("\\");
		 while (ipos != string::npos) {
			 filename = filename.substr(ipos + 1);
			 ipos = filename.find("\\");
		 }
		 testname = UserDirectory + filename;
		 filestream->open(testname, ios::in | ios::binary);
		 if (!filestream->is_open()) {
#ifndef COUPSTD
			 TRACE("Can't Open file %s´\n", filename);
			 //Display error message
			 string str;
			 str = "For some reason the file (";
			 str += testname;
			 str += ") specified couldn´t be opened.";
		//	 MFC_Util::MessageBox(str, "File Open Error", MB_OK | MB_ICONEXCLAMATION);
#endif		
			 //m_IsMissing=true;	
			 return "";
			 // Actions without file name ?
		 }

		 filename = testname;
		 fileout = filename;
	 }

	 return fileout;
 }


 struct rgb_color
 {
	 int red;
	 int green;
	 int blue;
 };



 struct DistStat
 {
	 float Mean;
	 float Std;
	 float Min;
	 float Max;
	 float P5;
	 float P50;
	 float P95;
	 size_t NumData;
 };


 void FUtil::WriteProfileInt(string item, int value) {
#ifndef COUPSTD
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileInt(_T("CoupModel"), CString(item.c_str()), value);
#else
#ifndef NO_MS_GUI
	 MySingle::instance().GetRegisterPointer()->SetInt(item, value);
#endif
#endif
}
 void FUtil::WriteProfileStringStd(string item, string value) {
#ifndef COUPSTD
	CWinApp* pApp = AfxGetApp();
	pApp->WriteProfileString(_T("CoupModel"), CString(item.c_str()), CString(value.c_str()));
#else  
#ifndef NO_MS_GUI
	 MySingle::instance().GetRegisterPointer()->SetString(item, value);
#endif
#endif
}
void FUtil::trim(string& str)
{
  string::size_type pos = str.find_last_not_of(" ");
  if(pos != string::npos) {
    str.erase(pos + 1);
    pos = str.find_first_not_of(" ");
    if(pos != string::npos) str.erase(0, pos);
  }
  else str.erase(str.begin(), str.end());
}
void FUtil::trim_datestr(string& str)
{
  size_t pos=str.find(" ");
  while (pos!=string::npos) {
	  str.erase(pos,1);
	  pos=str.find(" ");
  }
  pos=str.find(":");
  while (pos!=string::npos) {
	  str.erase(pos,1);
	  pos=str.find(":");
  }
  pos=str.find("-");
  while (pos!=string::npos) {
	  str.erase(pos,1);
	  pos=str.find("-");
  }
}
void FUtil::trim_space(string& str)
{
  size_t pos=str.find(" ");
  while (pos!=string::npos) {
	  str.erase(pos,1);
	  pos=str.find(" ");
  }
}
void FUtil::trim_index(string& str)
{
	size_t pos1,pos2;
	pos2=str.rfind(')');
	if(str.size()==pos2+1) {
		pos1=str.rfind('(');
			if(pos1>0&&pos2>pos1) 
				str=str.substr(0,pos1);
	}
}
void FUtil::trim_xmlstring(string& str)
{
	
	string newstr;
	for (size_t i = 0; i < str.size(); i++) {
		char ss = str[i];
		if (ss < 32) {
			str.replace( i, 1," ");

		}

	}
	

	auto pos = str.find('//');
	while (pos != string::npos) {
		str.erase(pos, 1);
		str.insert(pos, "_");
		pos = str.find('//');
	}
	pos = str.find('ø');
	while (pos != string::npos) {
		str.erase(pos, 1);
		str.insert(pos, "");
		pos = str.find('ø');
	}

}
bool FUtil::CopyFile_Std(string srcFileName, string destFileName)
{
    std::ifstream ifs(srcFileName, std::ios::binary | std::ios::in);
    std::ofstream ofs(destFileName, std::ios::binary | std::ios::out);
 
    if(ifs.is_open() == false || ofs.is_open() == false)
        return false;
 
    // Opening istream in binary mode and writing in text mode will
    // increase the file size as \r\n are treated two characters in
    // binary mode where as one character in text mode
 
    const int len = 4096;
    char buf[4096];
    while(1)
    {
        if(ifs.eof())
            break;
        ifs.read(buf, len);
        auto nBytesRead = ifs.gcount();
        if(nBytesRead <= 0)
            break;
        ofs.write(buf, nBytesRead);
    }
    ifs.close();
    ofs.close();
 
    return true;
}

string FUtil::STD_FtoAsciiPane(float v)
{
	//return string(FtoAsciiPane(v));
	return "";
}



int	FUtil::AtoInt(string str) {
	int i;
#ifdef MS_CODE
	sscanf_s(str.data(), "%d", &i);
#else
	sscanf(str.data(),"%d",&i);
#endif
	return i;

}

string	FUtil::STD_ItoAscii(int v)  {
		char buf[100];
	auto n=snprintf(buf,60, "%d",v);
	string str;
	str.assign(buf,n);
	return str;
}
float FUtil::AtoFloat(string str) {
	if (str[0] == char(0)) return -1.1E38f;
	float f=MISSING;
	trim(str);
	auto pos=str.find(",");
	if(pos!=string::npos) {
		str.replace(pos,1,".");

	}

#ifdef MS_CODE
	if (sscanf_s(str.data(), "%G", &f) == -1) return MISSING;
#else
	if (sscanf(str.data(), "%G", &f) == -1) return MISSING;
#endif

	return f;
}
vector<float> FUtil::GetFloatVectorFromString(string str, size_t num) {
	vector<float> vv;


	
	size_t ipos;
	ipos = str.find(' '); if (ipos == string::npos) ipos= str.find(';'); if (ipos == string::npos) ipos = str.find('\t');
	while (ipos == 0 && str[0] == ' ') { str = str.substr(1); ipos = str.find(' '); }
	while (ipos != string::npos) {
		string sub;
		sub = str.substr(0, ipos);
		while (ipos == 0 && str[0] ==' ') {
			str=str.substr(1); ipos=str.find(' ');
		}
		sub = str.substr(0, ipos);
		float value = FUtil::AtoFloat(sub);
		vv.push_back(value);
		if (str[0] == '\n'&& ipos == 0) {
			vv.push_back(MISSING);
			ipos = string::npos;
		}
		else {
			str = str.substr(ipos + 1);
			ipos = str.find(' '); if (ipos == string::npos) ipos = str.find(';'); if (ipos == string::npos) ipos = str.find('\t');


			if (ipos == string::npos) {
				float value = FUtil::AtoFloat(str);
				vv.push_back(value);
			}
		}

	}

	if (str.size() > 0 && ipos == string::npos) {
		float value = FUtil::AtoFloat(str);
		vv.push_back(value);
	}
	while (vv.size() < num) vv.push_back(MISSING);
	vv.resize(num);

	return vv;
}
vector<string> FUtil::GetStringVectorFromStringLine(string str, size_t num) {
	vector<string> outvector;
	size_t ipos;
	ipos = str.find(';'); if (ipos == string::npos) ipos = str.find('\t');
	while (ipos != string::npos) {
		string sub;
		sub = str.substr(0, ipos);
		while (ipos == 0 && str[0] == ' ') {
			str = str.substr(1); ipos = str.find(' ');
		}
		sub = str.substr(0, ipos);
		outvector.push_back(sub);
		if (str[0] == '\n'&& ipos == 0) {
			outvector.push_back("");
			ipos = string::npos;
		}
		else {
			str = str.substr(ipos + 1);
			ipos = str.find(';'); if (ipos == string::npos) ipos = str.find('\t');
			if (ipos == string::npos) 	outvector.push_back(str);	
		}
	}
	if (str.size() > 0 && ipos == string::npos) outvector.push_back(str);
	while (outvector.size() < num) outvector.push_back("");
	return outvector;
}
char FUtil::GetDelimChar()
{
	return delimchar;
}
void FUtil::SetDelimChar(char delim)
{ 
	delimchar = delim; 
}

string FUtil::FileNameOnly(string urlname)
{
	auto rpos = urlname.rfind("\\");
	if (rpos != string::npos) {

		string str = urlname.substr(rpos + 1);
		return str;


	}


	return urlname;
}
string FUtil::DirectoryNameOnly(string urlname)
{
	auto rpos = urlname.rfind("\\");
	if (rpos != string::npos) {

		string str = urlname.substr(0, rpos + 1);
		return str;
	}
	return urlname;
}
float FUtil::GetNormalValue(float value, float minvalue, float maxvalue) // return a value between 0 and 1.
{
	if (maxvalue > minvalue&& value >= minvalue&& value <= maxvalue)
		return (value - minvalue) / (maxvalue - minvalue);
	else
		return 0.5;
}
string FUtil::GetWindowUser()
{
#ifndef COUPSTD
	TCHAR username[UNLEN + 1];
	DWORD username_len = UNLEN + 1;
	GetUserName(username, &username_len);
	return CT2A(username).m_psz;
#endif
	return "PEJ";
}
string FUtil::GetCurrentDateTime()
{
	time_t now=time(0);
	struct tm  tstruct;


	char       buf[80];
#ifdef MS_CODE
	localtime_s( &tstruct, &now);
#else
	localtime_r(&now, &tstruct);
#endif
	strftime(buf, 24, "%Y-%m-%d %H:%M", &tstruct);

	string koll = string(buf);
	return koll;
}
rgb_color FUtil::Get_RGB(float value)
{
	rgb_color out;
	size_t light = 100;
	out.red =int( (float)(255)*value);
	if (value <= 0.5)
		out.green = int(255 * value * 2);
	else
		out.green = int(255 * (1 - value) * 2);
	out.blue = int((float)(255)* (1. - value));
	return out;
}
bool FUtil::IsValidNumber(string str)
{
	bool number = false;
	bool alpha = false;
	if (str.size() == 0) return false;
	auto test = str.begin();
	for(size_t i=0; i<str.size(); i++) {
		if (size_t(str[i]) > 64&&size_t(str[i])!=69) alpha = true;
		if (size_t(str[i]) > 46 && size_t(str[i]) < 58) number = true;
	}
	if (number && !alpha) return true;

	return false;
}
string FUtil::NameRevision2018(string name)
{
	if (name == "MC_TempCanopy") {
		name = "MC Temp Canopy";
	}
	if (name == "CN RatioPlant") {
		name = "CN Ratio Total Plant"; return name;
	}
	else if (name == "CP RatioPlant") {
		name = "CP Ratio Total Plant"; return name;

	}
	if (name.rfind("SurfaceLitter") != string::npos) {
		auto pos = name.rfind("Surface");
		if (pos != string::npos) {
			name = name.substr(0, pos) + "Litter";
		}


	}
	while (name.rfind("Roots") != string::npos) {
		string str = "Root";
		auto pos = name.rfind("Roots");
		string add="";
		if (name.size() > pos + 5) {


			add = name.substr(pos + 5);
		}
		name = name.substr(0, pos) + str;
		if (add.size() > 0) name += add;
	}
	if (name.find("CH4") == 0|| name.find("CO2") == 0) return name;
	if (name.find("CN Ratio") == 0|| name.find("Cloud")==0) return name;
	else if (name.find("C") == 0 && name.find(" ") != 1 && name.find("COS") != 0 && name.find("Tot") != 1 && name.find("Carbon") != 0) {
		name = "C " + name.substr(1);
	}
	return name;
}
string FUtil::STD_FtoAscii(float v)
{
	char buf[14];
	
#ifdef MS_CODE
	if (v > 0.9 * MISSING) 
		sprintf_s(buf, "%G", v);
	else
		sprintf_s(buf, "");
#else
	if(v > 0.9 * MISSING)
		sprintf(buf, "%G", v);
	else
	sprintf(buf, "");
#endif
	

	return buf;
}
string FUtil::STD_DtoAscii(double v)
{
	char buf[14];
	snprintf(buf,14,  "%G", v);

	return buf;
}
string FUtil::ItoNumAscii(int v) {
	//CString Num;
	char buf[100];
	 v=v%1000000;
#ifdef MS_CODE
	sprintf_s(buf, "%06u", v);
#else
	sprintf(buf,"%06u",v);
#endif
	return buf;
}
string FUtil::GetString(char* buf) {
	string out;
	size_t i = 0;
	while (buf+i != nullptr) {
		out += buf+i;
		i++;
	}
	return out;

}





double FUtil::Decimal(int dec) {
	double tmp;
	tmp=dec;
	while(tmp>=1) tmp/=10;
	return tmp;
}
bool FUtil::IsFileExisting(string str) 
{


 // struct stat stFileInfo; 
 // bool blnReturn; 
  //int intStat; 

  // Attempt to get the file attributes 
  ifstream file(str);
  return file.good();

}
 int FUtil::GetOptionIndex(string optionlist, string str)
{

	 int ipos_end, index;
	 vector<string> v;
	 int ipos = optionlist.find("\n");
	 size_t pos = 0;
	 while (ipos > 0) {
		 v.push_back(optionlist.substr(pos, ipos));
		 optionlist = optionlist.substr(ipos + 1);
		 ipos = optionlist.find("\n");
		 if (str == v[v.size() - 1]) index = v.size() - 1;
	 }
	 v.push_back(optionlist);
	 if (str == v[v.size() - 1]) index = v.size() - 1;
	 
	 return index;


	/*ipos_end=optionlist.find(str);	
	if(ipos_end==0) return 0;
	optionlist=optionlist.substr(0,ipos_end+1);
	int ipos=optionlist.find("\n");
	if(ipos<0)
		index=-1;
	else
		index=0;
	while(ipos>0) {
		optionlist=optionlist.substr(ipos+1);
		ipos=optionlist.find("\n");
		index++;
	}
	return index;*/

}
string FUtil::GetOptionString(string optionlist, int Index)
{
	int ipos=optionlist.find("\n");
	int count=0;
	string str="";
	while(ipos>0) {
		str=optionlist.substr(0,ipos);
		optionlist=optionlist.substr(ipos+1);
		ipos=optionlist.find("\n");
		if(count==Index)
			return str;
		count++;
	}
	return str;

}
double FUtil::PedoFunc_AirEntry(double SAND, double CLAY, double PORO) {

	   double C[13]= {5.3396738,   0.1845038, -2.48394546, -0.00213853, 
       -0.04356349, -0.61745089, 0.00143598, -0.00855375,
       -0.00001282,  0.00895359, -0.00072472, 0.0000054, 0.50028060};
      double A= C[0]+C[1]*CLAY+ C[2]*PORO + C[3]*CLAY*CLAY +C[4]*SAND*PORO 
       +C[5]*PORO*CLAY + C[6]*SAND*SAND*PORO*PORO +C[7]*CLAY*CLAY*PORO*PORO 
       +C[8]*SAND*SAND*CLAY + C[9]*CLAY*CLAY*PORO 						  
       +C[10]*SAND*SAND*PORO  + C[11]*CLAY*CLAY*SAND +C[12]*PORO*PORO*CLAY;
	   return exp(A);
}
double FUtil::PedoFunc_Lambda(double SAND, double CLAY, double PORO) {
       double C[12]= {-0.7842831, 0.0177544, -1.062468, -0.00005304, 
       -0.00273493, 1.11134946, -0.03088295, 0.00026587,
	   -0.00610522, -0.00000235, 0.00798746,-0.00674491};

	  double A;
      A= C[0]+C[1]*SAND+ C[2]*PORO + C[3]*SAND*SAND +C[4]*CLAY*CLAY 
       +C[5]*PORO*PORO + C[6]*SAND*PORO + C[7]*SAND*SAND*PORO*PORO
       +C[8]*CLAY*CLAY*PORO*PORO + C[9]*SAND*SAND*CLAY 
       +C[10]*CLAY*CLAY*PORO   + C[11]*CLAY*PORO*PORO ;
     return exp(A);

}
double FUtil::PedoFunc_Residual(double SAND, double CLAY, double PORO) {
       double C[9]= {-0.0182482 , 0.00087269, 0.00513488, 0.02939286, 
         -0.00015395,-0.0010827 ,-0.00018233, 0.00030703,	
         -0.0023584};
	  double  A ;
	   A= C[0];
	   A+=C[1]*SAND;
	   A+=C[2]*CLAY;
	   A+=C[3]*PORO;
	   A+=C[4]*CLAY*CLAY;
	   A+=C[5]*SAND*PORO;
	   A+=C[6]*CLAY*CLAY*PORO*PORO;
	   A+=C[7]*CLAY*CLAY*PORO;
	   A+=C[8]*CLAY*PORO*PORO;
	   A=max(0.,100.*A);
	   return A;

}
double FUtil::PedoFunc_HydraulicCond(double SAND, double CLAY, double PORO) {
       double C[13]= {        19.52348, -8.96847 , -0.028212, 0.00018107, 
       -0.0094125, -8.395215, 0.077718, -0.00298,	  
       -0.019492,  0.0000173, 0.02733, 0.001434, -0.0000035};
	  double A ; 
	  A= PORO ;	  A=A*C[0];	  A=A+C[1];	  A=A+ C[2]*CLAY;  A=A +C[3]*SAND*SAND;
	  A=A +C[4]*CLAY*CLAY;	  A=A +C[5]*PORO*PORO;
	  A=A +C[6]*SAND*PORO;	  A=A +C[7]*SAND*SAND*PORO*PORO;
	  A=A +C[8]*CLAY*CLAY*PORO*PORO;	  A=A +C[9]*SAND*SAND*CLAY ;
	  A=A +C[10]*CLAY*CLAY*PORO ;	  A=A +C[11]*SAND*SAND*PORO ;
	  A=A +C[12]*CLAY*CLAY*SAND;
      return 240.*exp(A);

}
double FUtil::PedoFunc_WiltingPoint(double CLAY) {
	return 0.45*CLAY+2;
}


int FUtil::GetPositionSumInitialFromOutSheet()
{
	return 7;
}
#ifndef NO_MS_GUI
vector<Doc*> FUtil::GetReadyDoc(Doc* CurrentDoc) {
	vector<Doc*> ReadyDocuments;
	SimArchive* pAr;
	pAr = MySingle::instance().GetSimArchivePointer();
	size_t i = pAr->GetNumSimDoc();
	Doc* pMySimDoc;

	for (size_t i = 0; i < pAr->GetNumSimDoc(); i++) {
		pMySimDoc = pAr->GetSimDocPointer(i);
		if (!pMySimDoc->IsFinishedSimulation() && pMySimDoc != CurrentDoc)
			ReadyDocuments.push_back(pMySimDoc);
	}
	return ReadyDocuments;
}
#endif
#ifndef NO_MS_GUI
vector<SIMB> FUtil::SortSimB_ByGroup(vector<SIMB> v_in) {
	

	vector<SIMB> v_out;
	multimap<string, SIMB> SortMap;
	for (size_t i = 0; i < v_in.size(); i++) {
		SortMap.insert(pair<string, SIMB>(v_in[i].pBase->GetGroup(), v_in[i]));
	}

	for (auto it = SortMap.begin(); it != SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}

	return v_out;

}

vector<SIMB> FUtil::SortSimB_ByName(vector<SIMB> v_in) {
	vector<SIMB> v_out;
#ifndef NO_MS_GUI
	multimap<string, SIMB> SortMap;
	for (size_t i = 0; i < v_in.size(); i++) {
		string str = v_in[i].pBase->GetName();
		SortMap.insert(pair<string, SIMB>(str, v_in[i]));
	}

	for (auto it = SortMap.begin(); it != SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
#endif
	return v_out;

}
vector<SIMB> FUtil::SortSimB_ByProcess(vector<SIMB> v_in) {
	vector<SIMB> v_out;
#ifndef NO_MS_GUI
	multimap<string, SIMB> SortMap;
	for (size_t i = 0; i < v_in.size(); i++) {
		string str = v_in[i].pBase->GetProcessName();
		SortMap.insert(pair<string, SIMB>(str, v_in[i]));
	}

	for (auto it = SortMap.begin(); it != SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
#endif
	return v_out;

}
vector<SIMB> FUtil::SortSimB_ByElement(vector<SIMB> v_in) {
	vector<SIMB> v_out;
#ifndef NO_MS_GUI
	multimap<string, SIMB> SortMap;
	for (size_t i = 0; i < v_in.size(); i++) {
		string str = v_in[i].pBase->GetElementName();
		SortMap.insert(pair<string, SIMB>(str, v_in[i]));
	}

	for (auto it = SortMap.begin(); it != SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
#endif
	return v_out;

}
vector<SIMB> FUtil::SortSimB_ByType(vector<SIMB> v_in) {
	vector<SIMB> v_out;
#ifndef NO_MS_GUI
	multimap<string, SIMB> SortMap;
	for (size_t i = 0; i < v_in.size(); i++) {
		string str = v_in[i].pBase->GetTypeName();
		SortMap.insert(pair<string, SIMB>(str, v_in[i]));
	}

	for (auto it = SortMap.begin(); it != SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
#endif
	return v_out;

}

vector<Ps*> FUtil::SortSimB_ByGroup(vector<Ps*> v_in) {
	vector<Ps*> v_out;
#ifndef NO_MS_GUI
	multimap<string, Ps*> SortMap;
	for (size_t i = 0; i < v_in.size(); i++) {
		SortMap.insert(pair<string, Ps*>(v_in[i]->GetGroup(), v_in[i]));
	}

	for (auto it = SortMap.begin(); it != SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
#endif
	return v_out;

}
vector<Ps*> FUtil::SortSimB_ByName(vector<Ps*> v_in) {
	vector<Ps*> v_out;
#ifndef NO_MS_GUI
	multimap<string, Ps*> SortMap;
	for (size_t i = 0; i < v_in.size(); i++) {
		SortMap.insert(pair<string, Ps*>(v_in[i]->GetName(), v_in[i]));
	}

	for (auto it = SortMap.begin(); it != SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
#endif
	return v_out;

}
vector<Ps*> FUtil::SortSimB_ByProcess(vector<Ps*> v_in) {
	vector<Ps*> v_out;
#ifndef NO_MS_GUI
	multimap<string, Ps*> SortMap;
	for (size_t i = 0; i < v_in.size(); i++) {
		SortMap.insert(pair<string, Ps*>(v_in[i]->GetProcessName(), v_in[i]));
	}

	for (auto it = SortMap.begin(); it != SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
#endif
	return v_out;

}
vector<Ps*> FUtil::SortSimB_ByElement(vector<Ps*> v_in) {
	vector<Ps*> v_out;
#ifndef NO_MS_GUI
	multimap<string, Ps*> SortMap;
	for (size_t i = 0; i < v_in.size(); i++) {
		SortMap.insert(pair<string, Ps*>(v_in[i]->GetElementName(), v_in[i]));
	}

	for (auto it = SortMap.begin(); it != SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
#endif
	return v_out;

}

vector<Sw*> FUtil::SortSimB_ByGroup(vector<Sw*> v_in) {
	vector<Sw*> v_out;
#ifndef NO_MS_GUI
	multimap<string, Sw*> SortMap;
	for (size_t i = 0; i < v_in.size(); i++) {
		SortMap.insert(pair<string, Sw*>(v_in[i]->GetGroup(), v_in[i]));
	}

	for (auto it = SortMap.begin(); it != SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
#endif
	return v_out;

}
vector<Sw*> FUtil::SortSimB_ByName(vector<Sw*> v_in) {
	vector<Sw*> v_out;
#ifndef NO_MS_GUI
	multimap<string, Sw*> SortMap;
	for (size_t i = 0; i < v_in.size(); i++) {
		SortMap.insert(pair<string, Sw*>(v_in[i]->GetName(), v_in[i]));
	}

	for (auto it = SortMap.begin(); it != SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
#endif
	return v_out;

}
vector<Sw*> FUtil::SortSimB_ByProcess(vector<Sw*> v_in) {
	vector<Sw*> v_out;
#ifndef NO_MS_GUI
	multimap<string, Sw*> SortMap;
	for (size_t i = 0; i < v_in.size(); i++) {
		SortMap.insert(pair<string, Sw*>(v_in[i]->GetProcessName(), v_in[i]));
	}

	for (auto it = SortMap.begin(); it != SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
#endif
	return v_out;

}
vector<Sw*> FUtil::SortSimB_ByElement(vector<Sw*> v_in) {
	vector<Sw*> v_out;
#ifndef NO_MS_GUI
	multimap<string, Sw*> SortMap;
	for (size_t i = 0; i < v_in.size(); i++) {
		SortMap.insert(pair<string, Sw*>(v_in[i]->GetElementName(), v_in[i]));
	}

	for (auto it = SortMap.begin(); it != SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
#endif
	return v_out;

}

vector<Tab*> FUtil::SortSimB_ByGroup(vector<Tab*> v_in) {
	vector<Tab*> v_out;
#ifndef NO_MS_GUI
	multimap<string, Tab*> SortMap;
	for (size_t i = 0; i < v_in.size(); i++) {
		SortMap.insert(pair<string, Tab*>(v_in[i]->GetGroup(), v_in[i]));
	}

	for (auto it = SortMap.begin(); it != SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
#endif
	return v_out;

}
vector<Tab*> FUtil::SortSimB_ByName(vector<Tab*> v_in) {
	vector<Tab*> v_out;
#ifndef NO_MS_GUI
	multimap<string, Tab*> SortMap;
	for (size_t i = 0; i < v_in.size(); i++) {
		SortMap.insert(pair<string, Tab*>(v_in[i]->GetName(), v_in[i]));
	}

	for (auto it = SortMap.begin(); it != SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
#endif
	return v_out;

}
vector<Tab*> FUtil::SortSimB_ByProcess(vector<Tab*> v_in) {
	vector<Tab*> v_out;
#ifndef NO_MS_GUI
	multimap<string, Tab*> SortMap;
	for (size_t i = 0; i < v_in.size(); i++) {
		SortMap.insert(pair<string, Tab*>(v_in[i]->GetProcessName(), v_in[i]));
	}

	for (auto it = SortMap.begin(); it != SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
#endif
	return v_out;

}
vector<Tab*> FUtil::SortSimB_ByElement(vector<Tab*> v_in) {
	vector<Tab*> v_out;
#ifndef NO_MS_GUI
	multimap<string, Tab*> SortMap;
	for (size_t i = 0; i < v_in.size(); i++) {
		SortMap.insert(pair<string, Tab*>(v_in[i]->GetElementName(), v_in[i]));
	}

	for (auto it = SortMap.begin(); it != SortMap.end(); ++it) {
		v_out.push_back((*it).second);
	}
#endif
	return v_out;

}


int FUtil::GetPositionSumInitialFromOutSheet()
{
	return 7;
}
#ifndef COUPSTD
#ifndef NO_MS_GUI
vector<Doc*> FUtil::GetReadyDoc(Doc* CurrentDoc) {
		vector<Doc*> ReadyDocuments;
		SimArchive *pAr;
		pAr=MySingle::instance().GetSimArchivePointer();
		size_t i=pAr->GetNumSimDoc();
		Doc* pMySimDoc;
	
		for(size_t i=0; i<pAr->GetNumSimDoc(); i++) {
			pMySimDoc=pAr->GetSimDocPointer(i);
			if(!pMySimDoc->IsFinishedSimulation()&&pMySimDoc!=CurrentDoc)  
				ReadyDocuments.push_back(pMySimDoc);
		}
		return ReadyDocuments;
}
#endif


SIMB FUtil::GetPointerforNumberinList(vector<SIMB> v_in, size_t num) {
	if (v_in.size() > num)
		return v_in[num];
	else
		return v_in.back();
}

#endif
bool FUtil::SetOutPutFlag_FromLineInput(string line, Doc* pSimDoc) {
#ifndef NO_MS_GUI
	string type, group, name, outflag;

	size_t pos = line.find(";");
	type = line.substr(0, pos);
	line = line.substr(pos + 1);

	pos = line.find(";");
	group = line.substr(0, pos);
	line = line.substr(pos + 1);

	pos = line.find(";");
	name = line.substr(0, pos);
	line = line.substr(pos + 1);

	outflag = line;

	size_t posp_e = name.rfind(')');
	size_t posp_s = name.rfind('(');
	bool is_vector = false;
	size_t index;

	if (posp_e + 1 == name.size() && posp_e > posp_s) {
		// Vector
		string ind = name.substr(posp_s + 1, posp_e - posp_s - 1);
		index = FUtil::AtoInt(ind);
		if (index>0 && index<10000) {
			is_vector = true;
			name = name.substr(0, posp_s);
		}
	}
	bool valid = false;
	if (is_vector) {

		OutVector *pVector;
		SimB *pBase = ((ModelBase*)pSimDoc)->GetPtr(type, group, name);
		pVector = dynamic_cast<OutVector*>(pBase);
		if (pVector != nullptr) {
			pVector->SetStoreFlag(index - 1, FUtil::AtoInt(outflag));
			valid = true;
		}
	}
	else {
		//Scalar
		SimB *pBase = ((ModelBase*)pSimDoc)->GetPtr(type, group, name);
		OutSingle *pSingle;
		pSingle = dynamic_cast<OutSingle*>(pBase);
		if (pSingle != nullptr) {
			pSingle->SetStoreFlag(FUtil::AtoInt(outflag));
			valid = true;

		}

	}
	return valid;
#endif // !NO_MS_GUI
	return true;


}

#endif

