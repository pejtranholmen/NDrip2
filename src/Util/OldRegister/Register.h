#pragma once
#include "../std.h"
class Register{
public:
	Register();
	~Register();
	string GetString(string item) {
		if (m_MapString.empty()) return "";
		auto it = m_MapString.find(item);
		if (it != m_MapString.end())
			return it->second;
		else
			return "";
	};
	int GetInt(string item) {
		if (m_MapInt.empty()) return -9999;
		auto it = m_MapInt.find(item);
		if (it != m_MapInt.end())
			return it->second;
		else
			return -9999;
	};
	void SetString(string item, string str);
	void SetInt(string item, int value);
	bool IsRegisterOpen() {return m_IsOpen;};
	void OpenRegister(bool read = true);

private:
	size_t ReadRegisterFile();
	size_t WriteRegisterFile();
	bool m_IsOpen;
	fstream m_register;
	map<string, string> m_MapString;
	map<string, int> m_MapInt;

};
