#include "GetSoilStorage.h"
#include "../Util/FUtil.hpp"
#include "../SoilData/PlotPF/GausKreuger.h"
#include "../Util/StatUtil.h"
#include <sys/stat.h>
#include "../Util/ReadWriteTypes.h"
//#include "..\..\Model\SOIL\PedoFunctions.h"


GetSoilStorage::GetSoilStorage(void)
{
	m_ActProfile.KeyProf=0;
	m_ActProfile.NumLayers=0;
	m_Convertor=new GaussKreuger(0);
//	MySingle::instance().SetGetSoilPointer(this);
	m_TopRow=0;
	
}
bool GetSoilStorage::Init(string filename) {


	
	OpenMainFile(filename, true); 
	CreateSimpleDataBase();
	IniNewDataBase();
	ReadNewHeaderFile();
	IsMainEditOpen=true;

	return true;
}

GetSoilStorage::~GetSoilStorage(void)
{
	delete m_Convertor;
}
void GetSoilStorage::ReadHeaderFile()
{
	
}
bool GetSoilStorage::SaveNewFormat()
{

	return true;
}
bool GetSoilStorage::SaveRecord(bool)
{

	return m_NewHeaderFile.good();
}
bool GetSoilStorage::InitSaveRecord(size_t numprofile)
{

	return true;
}


void GetSoilStorage::ReadLevelFile()
{
	m_LevelFile.seekg(0,ios::end);
	auto n=m_LevelFile.tellg();
	m_LevelFile.seekg(0,ios::beg);
/*	vector<float> values;
	values.resize(37);
	m_LevelFile.read((char*)&values[0],NSizeLevel);

	
	while(m_LevelFile.tellg()<n) {
		m_LevelFile.read((char*)&values[0],NSizeLevel);


	}
//	int profnum=(str[0]);*/
}
bool GetSoilStorage::CreateSimpleDataBase() {
	CSVFILEINFO info=ScanningInputFile();
/*	vector<string> cols;
	cols=ReadLine();
	cols=ReadLine();
	size_t lines=2;
	m_NumProfile=0;
	m_LevelValues.resize(34);
	if(!m_Rw.OpenDocFileout(m_MainStoreName+".dba")) return false;
	while(lines<info.NumLines) {
		cols=ReadLine();
		if(cols.size()>0) {
			WriteHeaderFile(cols, (lines-2)*34*4);
			WriteLevelFile(cols);
			lines++;
			auto NumLayer=FUtil::AtoInt(cols[5]);
			for(int i=1; i<NumLayer; i++) {
				cols=ReadLine();
				if(cols.size()>0) {
					WriteLevelFile(cols);
					lines++;
				}
			
			}
		}


	}
		//WriteNewHeaderFile();
		//WriteNewLevelFile();



*/
	//m_Rw.CloseFileOut();
	return true;
}
SOILHEADER GetSoilStorage::GetSoilHeaderByKey(size_t key) {
	SOILHEADER out;
/*	out.NumLayers = 0;
	auto it=m_HeaderRec.find(key);
	bool test=true;
	if(it!=m_HeaderRec.end()) {
		auto pos=it->second;		
		m_Rw.SetGetPosition(&it->second);
		
		if(!m_Rw.rsize_t(&m_ActProfile.RecordPos)) test=false;
		m_Rw.rsize_t(&m_ActProfile.KeyProf);
		m_Rw.rDouble(&m_ActProfile.Long);
		m_Rw.rDouble(&m_ActProfile.Lat);
		m_Rw.rString(&m_ActProfile.Name);
		m_Rw.rString(&m_ActProfile.Type);
		m_Rw.rsize_t(&m_ActProfile.NumLayers);
		m_Rw.rsize_t(&m_ActProfile.NumProf);
		out=m_ActProfile;
	}
	*/
	return out;

}
SOILHEADER GetSoilStorage::GetSoilHeaderByNum(size_t key) {
	SOILHEADER out;
	auto it=m_HeaderRec.begin();
	for(size_t i=0; i<key; i++) 
		it++;
		
	auto pos=it->second;
	return GetSoilHeaderByKey(it->first);
//	m_Rw.SetGetPosition(&it->second);
//	return out;
}
vector<float> GetSoilStorage::GetSoilData(size_t key, size_t pos, size_t layer)
{
	vector<float> out;
	out.resize(34);
	m_LevelFile.seekg(pos+layer*136, ios::beg);
	m_LevelFile.read((char*)&out[0], 136);
	return out;

}
