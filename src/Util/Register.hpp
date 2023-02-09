#pragma once

#include <map>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
using namespace std;

namespace Register {

static bool m_IsOpen = true;
static fstream m_register;
static	map<string, string> m_MapString;
static	map<string, int> m_MapInt;

static size_t WriteRegisterFile() {
		if (!m_register.good()) return 0;
		char buffer[100];
		int n;
		m_register.seekp(0);
		n = snprintf(buffer, 100, "%zu,%zu\n", m_MapInt.size(), m_MapString.size());
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
static	size_t ReadRegisterFile() {
		string line, header;
		size_t count = 0;
		if (m_register.good()) getline(m_register, header);
		int Num_int, Num_string;
		istringstream ss(header);
		vector <string> record;
		string s, item, numstr;
		getline(ss, s, ',');
		Num_int = atoi(s.c_str());
		getline(ss, s, ',');
		Num_string = atoi(s.c_str());
		while (m_register.good()) {
			getline(m_register, line);
			istringstream split(line);
			if (line.size() > 3) {
				getline(split, item, ',');
				getline(split, numstr, ',');

				if (count < Num_int) {
					m_MapInt.insert(pair<string, int>(item, atoi(numstr.c_str())));
				}
				else {
					m_MapString.insert(pair<string, string>(item, numstr));
				}
				count++;
			}
		}
		return count;
	}



static void OpenRegister(bool read) {
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
static 	void SetInt(string item, int value) {
		auto it = m_MapInt.find(item);
		if (it != m_MapInt.end()) {
			it->second = value;
		}
		else {
			m_MapInt.insert(pair<string, int>(item, value));
		}


	}

static 	string GetString(string item) {
		if (m_MapString.empty()) return "";
		auto it = m_MapString.find(item);
		if (it != m_MapString.end())
			return it->second;
		else
			return "";
	};
static 	int GetInt(string item) {
		if (m_MapInt.empty()) return -9999;
		auto it = m_MapInt.find(item);
		if (it != m_MapInt.end())
			return it->second;
		else
			return -9999;
	};

static 	void SetString(string item, string str) {
		auto it = m_MapString.find(item);
		if (it != m_MapString.end()) {
			it->second = str;
		}
		else {
			m_MapString.insert(pair<string, string>(item, str));
		}
	}


}
