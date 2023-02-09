#pragma once
#include <map>
#include <fstream>
#include <string>

class Register{
public:
	Register();
	~Register();
	std::string GetString(std::string item) {
		if (m_MapString.empty()) return "";
		auto it = m_MapString.find(item);
		if (it != m_MapString.end())
			return it->second;
		else
			return "";
	};
	int GetInt(std::string item) {
		if (m_MapInt.empty()) return -9999;
		auto it = m_MapInt.find(item);
		if (it != m_MapInt.end())
			return it->second;
		else
			return -9999;
	};
	void SetString(std::string item, string str);
	void SetInt(std::string item, int value);
	bool IsRegisterOpen() {return m_IsOpen;};
	void OpenRegister(bool read = true);

private:
	size_t ReadRegisterFile();
	size_t WriteRegisterFile();
	bool m_IsOpen;
	fstream m_register;
	map<std::string, std::string> m_MapString;
	map<std::string, int> m_MapInt;

};
