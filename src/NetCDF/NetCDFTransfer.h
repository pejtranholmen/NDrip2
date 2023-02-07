#pragma once
#include "../PG/PG.H"

class NetCDFTransfer
{
public:
	NetCDFTransfer();
	NetCDFTransfer(size_t iopt);
	~NetCDFTransfer();
	
	string m_FileBase;
	vector<int> m_LandId;
	vector<float> m_Lat, m_Long;
	vector<float> m_Tair, m_Wind, m_Hum, m_Prec, m_Sw, m_Lw;
	bool ReadFile(string file, string var, vector<float> *pv);
	bool Read3HFile(string file,int superindex, string var, vector<float> *pv);
	void WFDEI();
	float GetValue(string file, string var, size_t index);
	CPG m_PGFile;
	bool m_Ready[7];
	string m_Var[7];
	int m_SuperIndex;
	
};
