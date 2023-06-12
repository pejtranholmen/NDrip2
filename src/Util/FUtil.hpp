#pragma once
#include <vector>
#include <cmath>
#ifdef MS_CODE
#include <direct.h>
#else
#include <sys/stat.h>
#endif
#include <filesystem>
#include <string>
#include <fstream>
#include <ios>
#include "../Util/Register.h"


//#include "../NewBase/ModelMap.h"


#ifndef MISSING
#define MISSING -1.1E38f
#endif

using namespace std;

struct rgb_color
{
	int red;
	int green;
	int blue;
};
struct connection_par {
	string dbname;
	string user;
	string password;
	string port;
	string host;
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

namespace FUtil
{
	static bool LocalHost = true;
	static char delimchar = ',';
	static string cur_path;

	static void SetDocumentPath(string path) {
		cur_path = path;
	}
	static std::string GetDocumentPath() {
		return cur_path;
	}


	static std::string GetCurrentPath() {
		std::string path;
#ifdef MS_CODE
		char* buff;
		buff = _getcwd(nullptr, 0);
		if (buff == nullptr) return "";
		path = buff; path += "\\";
#else
		path = "temp";
		path += "/";
#endif
		return path;
	};


	static vector<string> GetFileList(string type) {
		vector<string> out;
		struct stat sb;
		string path;

#ifdef LINUX2
		path = "/temp";
		return out;

#else
		namespace fs = std::filesystem;
		path = GetCurrentPath();


		// Looping until all the items of the directory are
		// exhausted
		if (type.size() == 0) type = ".";

		for (const auto& entry : fs::directory_iterator(path)) {

			// Converting the path to const char * in the
			// subsequent lines
			std::filesystem::path outfilename = entry.path();
			std::string outfilename_str = outfilename.string();
			const char* path = outfilename_str.c_str();

			// Testing whether the path points to a
			// non-directory or not If it does, displays path
			if (stat(path, &sb) == 0 && !(sb.st_mode & S_IFDIR) && outfilename_str.find(type) != string::npos)
				out.push_back(outfilename_str);
		}
		return out;
#endif

	}





#ifndef COUPSTD
	static std::string GetProfileStringStd(std::string item, std::string value) {
		CWinApp* pApp = AfxGetApp();
		CString Comp = pApp->GetProfileString(_T("CoupModel"), CString(item.c_str()), CString(value.c_str()));
		return CT2A(Comp).m_psz;
		return value;
	}
#else 
	static pair<std::string, unique_ptr<Register>> GetProfileStringStd(std::string item, std::string value, unique_ptr<Register>  p_Register=nullptr) {

		if (item == "UserDirectory") {

			value = FUtil::GetDocumentPath();

		}
		if (p_Register == nullptr) 	p_Register = make_unique<Register>();
		auto koll = p_Register->GetString(item);
		if (koll.size() == 0) {
			p_Register->SetString(item, value);
		}
		return pair<string, unique_ptr<Register>>(p_Register->GetString(item), move(p_Register));
	}
#endif


#ifndef COUPSTD
	static int GetProfileIntNo(string item, int value) {
		CWinApp* pApp = AfxGetApp();
		int Comp = pApp->GetProfileInt(_T("CoupModel"), CString(item.c_str()), value);
		return Comp;
#else
#ifdef MS_CODE
	static pair<int, unique_ptr<Register>> GetProfileIntNo(string item, int value, unique_ptr<Register>  p_Register=nullptr) {
		int koll = p_Register->GetInt(item);
		if (koll < 0) p_Register->SetInt(item, value);
		return pair<int, unique_ptr<Register>>(p_Register->GetInt(item), move(p_Register));
	}

#endif
#endif


#ifndef COUPSTD
	static void WriteProfileInt(string item, int value) {

		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileInt(_T("CoupModel"), CString(item.c_str()), value);
}
#else
	static unique_ptr<Register> WriteProfileInt(string item, int value, unique_ptr<Register> p_Register) {

		if (p_Register == nullptr) {
			p_Register = make_unique<Register>();
		}
		p_Register->SetInt(item, value);
		p_Register->SaveToFile();
		return move(p_Register);
	}

#endif

	
#ifndef COUPSTD
	static 	void WriteProfileStringStd(string item, string value) {
		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileString(_T("CoupModel"), CString(item.c_str()), CString(value.c_str()));
	}
#else  
	static 	unique_ptr<Register> WriteProfileString(string item, string value, unique_ptr<Register> p_Register) {
	
		p_Register->SetString(item, value);
		p_Register->SaveToFile();
		return move(p_Register);
	};
#endif



	static std::string arrayToString(std::vector <float> v) {
#ifndef LINUX2

		std::stringstream ss;
		for (size_t i = 0; i < v.size(); ++i)
		{
			if (i != 0)
				ss << ",";
			ss << v[i];
		}
		std::string s = ss.str();
		return s;
#endif
		return "";
}

	static void CreateSubDirectory(string dir) {
#ifdef MS_CODE
		namespace fs = std::filesystem;
		string str = ".\\" + dir;
		fs::create_directory(str);
#endif

	}




	static string check_open_newcsvfile(fstream* filestream, string filename) {
		string fileout;
		filestream->open(filename, ios::out);
		if (!filestream->is_open()) {
			auto UserDirectory = FUtil::GetCurrentPath();
			auto testname = UserDirectory + filename;
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
	static string check_open_newfile(std::fstream* filestream, string filename) {
		string fileout;
		filestream->open(filename, ios::binary | ios::in);
		if (!filestream->is_open()) {
			auto UserDirectory = FUtil::GetCurrentPath();
			auto testname = UserDirectory + filename;
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
	static bool check_if_empty_file(fstream* filestream, string filename) {
		filestream->seekg(0, ios::beg);
		//HeaderStart = filestream->tellg();
		filestream->seekg(0, ios::end);
		auto actpos = filestream->tellg();
		auto nbyte = filestream->tellg();
		filestream->seekg(actpos - std::streampos(4), ios::beg);
		unsigned long cumperformance;
		filestream->read((char*)&cumperformance, 4);
		nbyte = filestream->tellg();

#ifndef COUPSTD 
		if (nbyte == std::streamsize(0)) {

			TRACE("Can't Open file %s�\n", filename);
			//Display error message
			string str;
			str = "For some reason the file (";
			str += filename;
			str += ") is empty";
			// MFC_Util::MessageBox(str, "File Open Error", MB_OK | MB_ICONEXCLAMATION);
			filestream->close();

			return true;
		}
#endif
		return false;
	}
	static void trim(string& str)
	{
		string::size_type pos = str.find_last_not_of(" ");
		if (pos != string::npos) {
			str.erase(pos + 1);
			pos = str.find_first_not_of(" ");
			if (pos != string::npos) str.erase(0, pos);
		}
		else str.erase(str.begin(), str.end());
	}
	static void trim_datestr(string& str)
	{
		size_t pos = str.find(" ");
		while (pos != string::npos) {
			str.erase(pos, 1);
			pos = str.find(" ");
		}
		pos = str.find(":");
		while (pos != string::npos) {
			str.erase(pos, 1);
			pos = str.find(":");
		}
		pos = str.find("-");
		while (pos != string::npos) {
			str.erase(pos, 1);
			pos = str.find("-");
		}
	}
	static void trim_space(string& str)
	{
		size_t pos = str.find(" ");
		while (pos != string::npos) {
			str.erase(pos, 1);
			pos = str.find(" ");
		}
	}
	static void trim_index(string& str)
	{
		size_t pos1, pos2;
		pos2 = str.rfind(')');
		if (str.size() == pos2 + 1) {
			pos1 = str.rfind('(');
			if (pos1 > 0 && pos2 > pos1)
				str = str.substr(0, pos1);
		}
	}
	static void trim_xmlstring(string& str)
	{

		string newstr;

		auto pos = str.find(0x2f);
		while (pos != string::npos) {
			str.erase(pos, 1);
			str.insert(pos, "_");
			pos = str.find(0x2f);
		}
		pos = str.find(0xf8);
		while (pos != string::npos) {
			str.erase(pos, 1);
			str.insert(pos, "");
			pos = str.find(0xf8);
		}


		for (size_t i = 0; i < str.size(); i++) {
			char ss = str[i];
			if (ss == -42) {
				str.erase(str.begin() + i);
				str.insert(i, "Ö");
				i++;
			}
			else if (ss == -10) {
				str.erase(str.begin() + i);
				str.insert(i, "ö");
				i++;
			}
			else if (ss == -28) {
				str.erase(str.begin() + i);
				str.insert(i, "ä");
				i++;
			}
			else if (ss == -27) {
				str.erase(str.begin() + i);
				str.insert(i, "å");
				i++;
			}
			else if (ss == -59) {
				str.erase(str.begin() + i);
				str.insert(i, "Å");
				i++;
			}
			else if (ss == -60) {
				str.erase(str.begin() + i);
				str.insert(i, "Ä");
				i++;
			}
			else if (ss < 32) {

				str.erase(str.begin() + i);
				//str.replace(i, 1, " ");

			}
			else if (ss == 39) {
				str.erase(str.begin() + i);
				str.insert(i, " ");
			}
		}



	}
	static bool CopyFile_Std(string srcFileName, string destFileName)
	{
		std::ifstream ifs(srcFileName, std::ios::binary | std::ios::in);
		std::ofstream ofs(destFileName, std::ios::binary | std::ios::out);

		if (ifs.is_open() == false || ofs.is_open() == false)
			return false;

		// Opening istream in binary mode and writing in text mode will
		// increase the file size as \r\n are treated two characters in
		// binary mode where as one character in text mode

		const int len = 4096;
		char buf[4096];
		while (1)
		{
			if (ifs.eof())
				break;
			ifs.read(buf, len);
			auto nBytesRead = ifs.gcount();
			if (nBytesRead <= 0)
				break;
			ofs.write(buf, nBytesRead);
		}
		ifs.close();
		ofs.close();

		return true;
	}

	static string STD_FtoAsciiPane(float v)
	{
		//return string(FtoAsciiPane(v));
		return "";
	}



	static int	AtoInt(string str) {
		int i;
#ifdef MS_CODE
		sscanf_s(str.data(), "%d", &i);
#else
		sscanf(str.data(), "%d", &i);
#endif
		return i;

	}
	static string bool_toAscii(bool value) {
		if (value) return "true";
		else return "false";
	}
	static string	STD_ItoAscii(int v) {
		char buf[100]; 
		auto n = snprintf(buf, 60, "%d", v);
		string str;
		str.assign(buf, n);
		return str;
	}
	static double AtoDouble(string str) {
		if (str[0] == char(0)) return -1.1E38f;
		return stod(str);
	}
	static float AtoFloat(string str) {
		if (str[0] == char(0)) return -1.1E38f;
		float f = MISSING;
		trim(str);
		auto pos = str.find(",");
		if (pos != string::npos) {
			str.replace(pos, 1, ".");

		}

#ifdef MS_CODE
		if (sscanf_s(str.data(), "%G", &f) == -1) return MISSING;
#else
		if (sscanf(str.data(), "%G", &f) == -1) return MISSING;
#endif

		return f;
	}
	static vector<float> GetFloatVectorFromString(string str, size_t num) {
		vector<float> vv;

		size_t ipos;
		ipos = str.find(' '); if (ipos == string::npos) ipos = str.find(';'); if (ipos == string::npos) ipos = str.find('\t');
		while (ipos == 0 && str[0] == ' ') { str = str.substr(1); ipos = str.find(' '); }
		while (ipos != string::npos) {
			string sub;
			sub = str.substr(0, ipos);
			while (ipos == 0 && str[0] == ' ') {
				str = str.substr(1); ipos = str.find(' ');
			}
			sub = str.substr(0, ipos);
			float value = AtoFloat(sub);
			vv.push_back(value);
			if (str[0] == '\n' && ipos == 0) {
				vv.push_back(MISSING);
				ipos = string::npos;
			}
			else {
				str = str.substr(ipos + 1);
				ipos = str.find(' '); if (ipos == string::npos) ipos = str.find(';'); if (ipos == string::npos) ipos = str.find('\t');


				if (ipos == string::npos) {
					float value = AtoFloat(str);
					vv.push_back(value);
				}
			}

		}

		if (str.size() > 0 && ipos == string::npos) {
			float value = AtoFloat(str);
			vv.push_back(value);
		}
		while (vv.size() < num) vv.push_back(MISSING);
		vv.resize(num);

		return vv;
	}
	static vector<int> GetIntVectorFromString(string str) {
		vector<int> vv;
		size_t ipos;
		ipos = str.find("(");
		if (ipos != string::npos) str = str.substr(ipos + 1);
		ipos = str.rfind(")");
		if (ipos != string::npos)  str = str.substr(0, ipos);
		ipos = str.find(','); 
		while (ipos != string::npos) {
			string sub;
			sub = str.substr(0, ipos);
			int value = AtoInt(sub);
			vv.push_back(value);
			str = str.substr(ipos + 1);
			ipos = str.find(',');
		}
		if (str.size() > 0) vv.push_back(AtoInt(str));
		return vv;
	}
	static string GetStringFromIntVector(vector<int> vint) {
		string str="(";
		size_t count = 0;
		for (auto i : vint) {
			str += to_string(i);
			if (vint.size() - 1 > count) str += ",";
			count++;
		}
		str += ")";
		return str;
	}
	static string getspace(size_t n) { string out; string space = " ";; for (size_t i = 0; i < n; i++) out += space; return out; };
	static vector<string> GetStringVectorFromStringLine(string str, size_t num) {
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
			if (str[0] == '\n' && ipos == 0) {
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
	static char GetDelimChar()
	{
		return delimchar;
	}
	static void SetDelimChar(char delim)
	{
		delimchar = delim;
	}

	static string FileNameOnly(string urlname, bool NoExtension=false)
	{
		auto rpos = urlname.rfind("\\");
		if (rpos == string::npos) {
			rpos = urlname.rfind("/");
		}
		if (rpos != string::npos) {
			string str = urlname.substr(rpos + 1);
			if (NoExtension) {
				auto posend = str.rfind(".");
				if (posend != string::npos) return str.substr(0,posend);
			}
			return str;
		}
			
		return urlname;
	}
	static string DirectoryNameOnly(string urlname)
	{
		auto rpos = urlname.rfind("\\");
		if (rpos != string::npos) {

			string str = urlname.substr(0, rpos + 1);
			return str;
		}
		return urlname;
	}
	static float GetNormalValue(float value, float minvalue, float maxvalue) // return a value between 0 and 1.
	{
		if (maxvalue > minvalue && value >= minvalue && value <= maxvalue)
			return (value - minvalue) / (maxvalue - minvalue);
		else
			return 0.5;
	}
	static string GetWindowUser()
	{
#ifndef COUPSTD
		TCHAR username[UNLEN + 1];
		DWORD username_len = UNLEN + 1;
		GetUserName(username, &username_len);
		return CT2A(username).m_psz;
#endif
		return "PEJ";
	}
	static string GetCurrentDateTime()
	{
		time_t now = time(0);
		struct tm  tstruct;


		char       buf[80];
#ifdef MS_CODE
		localtime_s(&tstruct, &now);
#else
		localtime_r(&now, &tstruct);
#endif
		strftime(buf, 24, "%Y-%m-%d %H:%M", &tstruct);

		string koll = string(buf);
		return koll;
	}
	static rgb_color Get_RGB(float value)
	{
		rgb_color out;
		size_t light = 100;
		out.red = int((float)(255) * value);
		if (value <= 0.5)
			out.green = int(255 * value * 2);
		else
			out.green = int(255 * (1 - value) * 2);
		out.blue = int((float)(255) * (1. - value));
		return out;
	}
	static bool IsValidNumber(string str)
	{
		bool number = false;
		bool alpha = false;
		if (str.size() == 0) return false;
		auto test = str.begin();
		for (size_t i = 0; i < str.size(); i++) {
			if (size_t(str[i]) > 64 && size_t(str[i]) != 69) alpha = true;
			if (size_t(str[i]) > 46 && size_t(str[i]) < 58) number = true;
		}
		if (number && !alpha) return true;

		return false;
	}
	static string NameRevision2018(string name)
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
			string add = "";
			if (name.size() > pos + 5) {


				add = name.substr(pos + 5);
			}
			name = name.substr(0, pos) + str;
			if (add.size() > 0) name += add;
		}
		if (name.find("CH4") == 0 || name.find("CO2") == 0) return name;
		if (name.find("CN Ratio") == 0 || name.find("Cloud") == 0) return name;
		else if (name.find("C") == 0 && name.find(" ") != 1 && name.find("COS") != 0 && name.find("Tot") != 1 && name.find("Carbon") != 0) {
			name = "C " + name.substr(1);
		}
		return name;
	}
	static string STD_FtoAscii(float v)
	{
		char buf[14];

#ifdef MS_CODE
		if (v > 0.9 * MISSING)
			sprintf_s(buf, "%G", v);
		else
			sprintf_s(buf, "");
#else
		if (v > 0.9 * MISSING)
			sprintf(buf, "%G", v);
		else
			return "";

#endif


		return buf;
	}
	static string STD_DtoAscii(double v)
	{
		char buf[14];
		snprintf(buf, 14, "%G", v);

		return buf;
	}
	static string ItoNumAscii(int v) {
		//CString Num;
		char buf[100];
		v = v % 1000000;
#ifdef MS_CODE
		sprintf_s(buf, "%06u", v);
#else
		sprintf(buf, "%06u", v);
#endif
		return buf;
	}
	static string ItoNum2Ascii(int v) {
		//CString Num;
		char buf[100];
		v = v % 100;
#ifdef MS_CODE
		sprintf_s(buf, "%02u", v);
#else
		sprintf(buf, "%02u", v);
#endif
		return buf;
	}
	static string GetString(char* buf) {
		string out;
		size_t i = 0;
		while (buf + i != nullptr) {
			out += buf + i;
			i++;
		}
		return out;

	}





	static double Decimal(int dec) {
		double tmp;
		tmp = dec;
		while (tmp >= 1) tmp /= 10;
		return tmp;
	}
	static bool IsFileExisting(string str)
	{


		// struct stat stFileInfo; 
		// bool blnReturn; 
		 //int intStat; 

		 // Attempt to get the file attributes 
		ifstream file(str);
		return file.good();

	}
	static int GetOptionIndex(string optionlist, string str)
	{

		int  index=0;
		vector<string> v;
		auto ipos = optionlist.find("\n");
		size_t pos = 0;
		while (ipos > 0) {
			v.push_back(optionlist.substr(pos, ipos));
			optionlist = optionlist.substr(ipos + 1);
			ipos = optionlist.find("\n");
			if (str == v[v.size() - 1]) index = int(v.size() - 1);
		}
		v.push_back(optionlist);
		if (str == v[v.size() - 1]) index = int( v.size() - 1);

		return index;



	}
	static string GetOptionString(string optionlist, int Index)
	{
		size_t ipos = optionlist.find("\n");
		int count = 0;
		string str = "";
		while (ipos > 0&&ipos!=string::npos) {
			str = optionlist.substr(0, ipos);
			optionlist = optionlist.substr(ipos + 1);
			ipos = optionlist.find("\n");
			if (count == Index)
				return str;
			count++;
		}
		return str;

	}
	static double PedoFunc_AirEntry(double SAND, double CLAY, double PORO) {

		double C[13] = { 5.3396738,   0.1845038, -2.48394546, -0.00213853,
		-0.04356349, -0.61745089, 0.00143598, -0.00855375,
		-0.00001282,  0.00895359, -0.00072472, 0.0000054, 0.50028060 };
		double A = C[0] + C[1] * CLAY + C[2] * PORO + C[3] * CLAY * CLAY + C[4] * SAND * PORO
			+ C[5] * PORO * CLAY + C[6] * SAND * SAND * PORO * PORO + C[7] * CLAY * CLAY * PORO * PORO
			+ C[8] * SAND * SAND * CLAY + C[9] * CLAY * CLAY * PORO
			+ C[10] * SAND * SAND * PORO + C[11] * CLAY * CLAY * SAND + C[12] * PORO * PORO * CLAY;
		return exp(A);
	}
	static double PedoFunc_Lambda(double SAND, double CLAY, double PORO) {
		double C[12] = { -0.7842831, 0.0177544, -1.062468, -0.00005304,
		-0.00273493, 1.11134946, -0.03088295, 0.00026587,
		-0.00610522, -0.00000235, 0.00798746,-0.00674491 };

		double A;
		A = C[0] + C[1] * SAND + C[2] * PORO + C[3] * SAND * SAND + C[4] * CLAY * CLAY
			+ C[5] * PORO * PORO + C[6] * SAND * PORO + C[7] * SAND * SAND * PORO * PORO
			+ C[8] * CLAY * CLAY * PORO * PORO + C[9] * SAND * SAND * CLAY
			+ C[10] * CLAY * CLAY * PORO + C[11] * CLAY * PORO * PORO;
		return exp(A);

	}
	static double PedoFunc_Residual(double SAND, double CLAY, double PORO) {
		double C[9] = { -0.0182482 , 0.00087269, 0.00513488, 0.02939286,
		  -0.00015395,-0.0010827 ,-0.00018233, 0.00030703,
		  -0.0023584 };
		double  A;
		A = C[0];
		A += C[1] * SAND;
		A += C[2] * CLAY;
		A += C[3] * PORO;
		A += C[4] * CLAY * CLAY;
		A += C[5] * SAND * PORO;
		A += C[6] * CLAY * CLAY * PORO * PORO;
		A += C[7] * CLAY * CLAY * PORO;
		A += C[8] * CLAY * PORO * PORO;
		A = max(0., 100. * A);
		return A;

	}
	static double PedoFunc_HydraulicCond(double SAND, double CLAY, double PORO) {
		double C[13] = { 19.52348, -8.96847 , -0.028212, 0.00018107,
		-0.0094125, -8.395215, 0.077718, -0.00298,
		-0.019492,  0.0000173, 0.02733, 0.001434, -0.0000035 };
		double A;
		A = PORO;	  A = A * C[0];	  A = A + C[1];	  A = A + C[2] * CLAY;  A = A + C[3] * SAND * SAND;
		A = A + C[4] * CLAY * CLAY;	  A = A + C[5] * PORO * PORO;
		A = A + C[6] * SAND * PORO;	  A = A + C[7] * SAND * SAND * PORO * PORO;
		A = A + C[8] * CLAY * CLAY * PORO * PORO;	  A = A + C[9] * SAND * SAND * CLAY;
		A = A + C[10] * CLAY * CLAY * PORO;	  A = A + C[11] * SAND * SAND * PORO;
		A = A + C[12] * CLAY * CLAY * SAND;
		return 240. * exp(A);

	}
	static double PedoFunc_WiltingPoint(double CLAY) {
		return 0.45 * CLAY + 2;
	}


	static int GetPositionSumInitialFromOutSheet()
	{
		return 7;
	}
#ifndef NO_MS_GUI
	vector<Doc*> GetReadyDoc(Doc* CurrentDoc) {
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
	static vector<SIMB> SortSimB_ByGroup(vector<SIMB> v_in) {


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

	static vector<SIMB> SortSimB_ByName(vector<SIMB> v_in) {
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
	static vector<SIMB> SortSimB_ByProcess(vector<SIMB> v_in) {
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
	static vector<SIMB> SortSimB_ByElement(vector<SIMB> v_in) {
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
	static vector<SIMB> SortSimB_ByType(vector<SIMB> v_in) {
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

	static vector<Ps*> SortSimB_ByGroup(vector<Ps*> v_in) {
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
	static vector<Ps*> SortSimB_ByName(vector<Ps*> v_in) {
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
	static vector<Ps*> SortSimB_ByProcess(vector<Ps*> v_in) {
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
	static vector<Ps*> SortSimB_ByElement(vector<Ps*> v_in) {
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

	static vector<Sw*> SortSimB_ByGroup(vector<Sw*> v_in) {
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
	static vector<Sw*> SortSimB_ByName(vector<Sw*> v_in) {
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
	static vector<Sw*> SortSimB_ByProcess(vector<Sw*> v_in) {
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
	static vector<Sw*> SortSimB_ByElement(vector<Sw*> v_in) {
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

	static vector<Tab*> SortSimB_ByGroup(vector<Tab*> v_in) {
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
	static vector<Tab*> SortSimB_ByName(vector<Tab*> v_in) {
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
	static vector<Tab*> SortSimB_ByProcess(vector<Tab*> v_in) {
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
	static vector<Tab*> SortSimB_ByElement(vector<Tab*> v_in) {
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


	static int GetPositionSumInitialFromOutSheet()
	{
		return 7;
	}
#ifndef COUPSTD
#ifndef NO_MS_GUI
	vector<Doc*> GetReadyDoc(Doc* CurrentDoc) {
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


	static SIMB GetPointerforNumberinList(vector<SIMB> v_in, size_t num) {
		if (v_in.size() > num)
			return v_in[num];
		else
			return v_in.back();
	}

#endif
	static bool SetOutPutFlag_FromLineInput(string line, Doc* pSimDoc) {
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
			index = AtoInt(ind);
			if (index > 0 && index < 10000) {
				is_vector = true;
				name = name.substr(0, posp_s);
			}
		}
		bool valid = false;
		if (is_vector) {

			OutVector* pVector;
			SimB* pBase = ((ModelBase*)pSimDoc)->GetPtr(type, group, name);
			pVector = dynamic_cast<OutVector*>(pBase);
			if (pVector != nullptr) {
				pVector->SetStoreFlag(index - 1, AtoInt(outflag));
				valid = true;
			}
		}
		else {
			//Scalar
			SimB* pBase = ((ModelBase*)pSimDoc)->GetPtr(type, group, name);
			OutSingle* pSingle;
			pSingle = dynamic_cast<OutSingle*>(pBase);
			if (pSingle != nullptr) {
				pSingle->SetStoreFlag(AtoInt(outflag));
				valid = true;

			}

		}
		return valid;
#endif // !NO_MS_GUI
		return true;


	}

#endif


}