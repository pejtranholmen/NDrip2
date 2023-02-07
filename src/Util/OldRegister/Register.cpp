#include pragma once

#include "..\std.h"
#include <sstream>

using namespace Register {

		m_IsOpen = true;
		bool m_IsOpen;
		fstream m_register;
		map<string, string> m_MapString;
		map<string, int> m_MapInt;
		
void OpenRegister(bool read) {
		string line_str;
		if (!m_register.is_open()) {
			m_register.open("CoupRegister.Ini", ios::in | ios::out);
			if (!m_register.is_open()) {
				m_register.open("CoupRegister.Ini", ios::out);
				WriteRegisterFile();
				m_register.close();

			}
		}
		if (m_register.is_open()) {
			if (read) ReadRegisterFile();
			else {
				m_register.clear();
				WriteRegisterFile();
			}
		}

	}
	void SetInt(string item, int value) {
		auto it = m_MapInt.find(item);
		if (it != m_MapInt.end()) {
			it->second = value;
		}
		else {
			m_MapInt.insert(pair<string, int>(item, value));
		}


	}

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

	void SetString(string item, string str) {
		auto it = m_MapString.find(item);
		if (it != m_MapString.end()) {
			it->second = str;
		}
		else {
			m_MapString.insert(pair<string, string>(item, str));
		}
	}
	size_t ReadRegisterFile() {
		string line, header;
		size_t count = 0;
		if (m_register.good()) getline(m_register, header);
		int Num_int, Num_string;
		istringstream ss(header);
		vector <string> record;
		string s, item, numstr;
		getline(ss, s, ',');
		Num_int = FUtil::AtoInt(s);
		getline(ss, s, ',');
		Num_string = FUtil::AtoInt(s);
		while (m_register.good()) {
			getline(m_register, line);
			istringstream split(line);
			if (line.size() > 3) {
				getline(split, item, ',');
				getline(split, numstr, ',');

				if (count < Num_int) {
					m_MapInt.insert(pair<string, int>(item, FUtil::AtoInt(numstr)));
				}
				else {
					m_MapString.insert(pair<string, string>(item, numstr));
				}
				count++;
			}
		}
		return count;
	}
	size_t WriteRegisterFile() {
		if (!m_register.good()) return 0;
		char buffer[100];
		int n;
		m_register.seekp(0);
		n = snprintf(buffer, 100, "%u,%u\n", m_MapInt.size(), m_MapString.size());
		m_register.write(buffer, n);
		for (auto it = m_MapInt.begin(); it != m_MapInt.end(); ++it) {
			string str;
			str = it->first;

			n = snprintf(buffer, 100, "%s,%d\n", str.data(), it->second);
			m_register.write(buffer, n);
		}
		for (auto it = m_MapString.begin(); it != m_MapString.end(); ++it) {
			n = snprintf(buffer, 100, "%s,%s\n", it->first.data(), it->second.data());
			m_register.write(buffer, n);
		}
		return 0;
	}
}