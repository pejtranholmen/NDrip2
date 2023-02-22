#include "Register.h"
#include <sstream>
#include <./FUtil.hpp>

using namespace std;

Register::Register()
{
#ifndef NOSIMMODEL
#ifndef LINUX2
	m_IsOpen = true;
	OpenRegister();
	if (!m_register.is_open()) OpenRegister();
#endif
#endif
}

Register::~Register()
{
	OpenRegister(false);
}
void Register::OpenRegister(bool read) {
	string line_str;
	if (!m_register.is_open()) {
		m_register.open("CoupRegister.Ini",ios::in | ios::out);
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
void Register::SetInt(string item, int value) {
	auto it = m_MapInt.find(item);
	if (it != m_MapInt.end()) {
		it->second = value;
	}
	else {
		m_MapInt.insert(pair<string, int>(item, value));
	}
	WriteRegisterFile();

}
void Register::SetString(string item, string str) {
	auto it = m_MapString.find(item);
	if (it != m_MapString.end()) {
		it->second = str;
		}
	else {
		m_MapString.insert(pair<string, string>(item, str));
	}
}
size_t Register::ReadRegisterFile() {
	string line, header;
	size_t count = 0;
	if (m_register.good()) getline(m_register, header);
	int Num_int, Num_string;
	istringstream ss (header);
	vector <string> record;
	string s, item, numstr;
	getline(ss, s, ',');
	Num_int=FUtil::AtoInt(s);
	getline(ss, s, ',');
	Num_string = FUtil::AtoInt(s);
	while (m_register.good()) {
		getline(m_register, line);
		istringstream split (line);
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
size_t Register::WriteRegisterFile() {
	if (!m_register.good()) return 0;
	char buffer[100];
	int n;
	m_register.seekp(0);
	n=snprintf(buffer,100, "%u,%u\n", m_MapInt.size(), m_MapString.size());
	m_register.write(buffer, n);
	for (auto it = m_MapInt.begin(); it !=m_MapInt.end(); ++it) {
		string str;
		str = it->first;

			n = snprintf(buffer,100, "%s,%d\n", str.data(), it->second);
			m_register.write(buffer, n);
	}
	for (auto it = m_MapString.begin(); it !=m_MapString.end(); ++it) {
		n = snprintf(buffer,100, "%s,%s\n", it->first.data(), it->second.data());
		m_register.write(buffer, n);
	}
	return 0;
}
