#pragma once
#include <vector>

#include "./SoilStorage.h"
class GaussKreuger;
class GetSoilStorage :
	public SoilStorage
{
public:
	GetSoilStorage(void);
	~GetSoilStorage(void);
	void ReadHeaderFile();
	bool CreateSimpleDataBase();
	void ReadLevelFile();
	SOILHEADER GetSoilHeaderByKey(size_t key); 
	SOILHEADER GetSoilHeaderByNum(size_t num); 
	std::vector<float> GetSoilData(size_t key, size_t pos, size_t layer);
	bool SaveNewFormat();
	bool SaveRecord(bool NewRecord=false);
	bool InitSaveRecord(size_t numprofile);
	bool Init(std::string filename);

private:
	GaussKreuger *m_Convertor;
	int m_TopRow;
};

