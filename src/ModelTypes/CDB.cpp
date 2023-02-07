
#include "CDB.h"
CDB::CDB() :SimB(DB) {
	
	ChaDatelong.resize(0);
	ChaParIndex.resize(0);
	ChaParSelection.resize(0);
	ChaParKeySelection.resize(0);
	MR_KeySelection.clear();
	DB_Index = 0;

}
CDB::~CDB() {}


CDB::CDB(string group)
{
	ID_Group=group;
	ID_Selection="";
	m_MR_Dimension=-1;
}
CDB::CDB(DB_types index)
{
	switch(index) {
	case Plant_Hydrology:
		ID_Group="Plant_Hydrology";
		DB_Index = 0;
		break;
	case Soil_Properties:
		ID_Group="Soil Properties";
		break;
	case Soil_Evaporation:
		ID_Group="Soil evaporation";
		DB_Index = 1;
		break;
	case Snow:
		ID_Group="Snow";
		DB_Index = 2;
		break;
	case Soil_Freezing:
		ID_Group="Soil frost";
		DB_Index = 3;
		break;
	case Soil_Water_Flows:
		ID_Group="Soil water flows";
		DB_Index = 4;
		break;
	case Plant_Growth:
		ID_Group="Plant Growth";
		DB_Index = 5;
		break;
	case Soil_Manage:
		ID_Group="Site Management";
		DB_Index = 6;
		break;

	case Soil_Organic:
		ID_Group="Soil Organic Processes";
		DB_Index = 7;
		break;
	case Soil_InOrganic:
		ID_Group="Soil Mineral N Processes";
		DB_Index = 8;
		break;
	default:
		ID_Group="";
	}
	ID_Selection="";
	m_MR_Dimension=-1;
	ChaDatelong.resize(0);
	ChaParIndex.resize(0);
	ChaParSelection.resize(0);
	ChaParKeySelection.resize(0);
	MR_KeySelection.clear();
}
CDB::CDB(int index):SimB(DB)
{
	if (index > 1)
		DB_Index = index - 1;
	else
		DB_Index = index;
	switch(index) {
	case 0:
			ID_Group="Plant";
			break;
	case 1:
			ID_Group="Soil Properties";
			break;
	case 2:
			ID_Group="Soil evaporation";
			
			break;
	case 3:
			ID_Group="Snow";
			break;
	case 4:
			ID_Group="Soil frost";
			break;
	case 5:
			ID_Group="Soil water flows";
			break;
	case 6:
		ID_Group="Plant Growth";
		break;
	case 7:
		ID_Group="Site Management";
		break;
	case 8:
		ID_Group="Soil organic processes";
		break;
	case 9:
		ID_Group="Soil Mineral N processes";
		break;
	default:
		ID_Group="";
	}
	m_LastSelectedRepetition=0;
	ID_Selection="";
	m_MR_Dimension=-1;
	ChaDatelong.resize(0);
	ChaParIndex.resize(0);
	ChaParSelection.resize(0);
	ChaParKeySelection.resize(0);
	MR_KeySelection.clear();
}



CDB::CDB(string group, string selection)
{
	ID_Group=group;
	while(selection.find('\t')>-1)
		selection[selection.find('\t')]=',';
	ID_Selection=selection;
}
bool CDB::SetValue(string str)
{
	while(str.find('\t')>-1)
		str[str.find('\t')]=',';
	ID_Selection=str;
	return true;
}
bool CDB::SetChaDateAndValues(long newdate, string str, string key){


	while(str.find("\t")>-1)
		str[str.find("\t")]=',';


	long test;
	for(size_t i=0;i<ChaDatelong.size();i++){
		test=ChaDatelong[i];
		if(test>newdate) {
			ChaDatelong[i]=newdate;
			ChaParSelection[i]=str;
			ChaParKeySelection[i]=key;
			ChaParIndex[i]=-1;
			return true;
		}
		else if(test==newdate) {
			ChaDatelong[i]=newdate;
			ChaParSelection[i]=str;
			ChaParKeySelection[i]=key;
			return true;
		}
	}
	ChaDatelong.push_back(newdate);
	ChaParSelection.push_back(str);
	ChaParKeySelection.push_back(key);
	ChaParIndex.push_back(-1);
	return false;
}
bool CDB::ResetChaDates(){	
	ChaDatelong.clear();
	ChaParIndex.clear();
	ChaParSelection.clear();
	ChaParKeySelection.clear();
	return true;
}
void CDB::MR_Set_Method(enum MR_METHOD value)
{
   m_MR_Method=value;
}

void CDB::MR_Set_Dim(int value)
{
   m_MR_Dimension=value;
}

MR_METHOD CDB::MR_Get_Method()
{
	return m_MR_Method;
}

int CDB::MR_Get_Dim()
{
	return m_MR_Dimension;

}
size_t CDB::GetNumChaDates(){
	return ChaDatelong.size();
}
string CDB::GetChaParValue(size_t index){
	if(ChaParSelection.size()>index)
		return ChaParSelection[index];
	else
		return "";
}
string CDB::GetChaParKey(size_t index){
	if(ChaParKeySelection.size()>index)
		return ChaParKeySelection[index];
	else
		return "";
}

int CDB::GetChaParIndex(size_t index) {
	if(ChaParIndex.size()>index)
		return ChaParIndex[index];
	else
 		return -1;
}
long CDB::GetChaDate(size_t index){
	if(ChaDatelong.size()>index)
	{
		return ChaDatelong[index];
	}
	else
		return -1;
}
bool CDB::DeleteChaDate(size_t index){
	if(ChaDatelong.size()>index) {
		ChaDatelong.erase(ChaDatelong.begin()+index);
		if (ChaParSelection.size() > index) ChaParSelection.erase(ChaParSelection.begin()+index);
		if (ChaParKeySelection.size() > index) ChaParKeySelection.erase(ChaParKeySelection.begin()+index);
		if (ChaParIndex.size() > index) ChaParIndex.erase(ChaParIndex.begin()+index);
	}
	return true;
}
string CDB::GetName()
{
   return ID_Group;
}
void CDB::SetName(string str)
{
   ID_Group=str;
}
string CDB::GetStringValue()
{
   return ID_Selection;
}
void CDB::MR_AddSelection(string des, string key)
{
	  MR_ParSelection.push_back(des);
	  MR_KeySelection.push_back(key);
}
void CDB::MR_SetSelection(size_t index,string des, string key)
{
	if(index<MR_ParSelection.size()) {
		MR_ParSelection[index]=des;
		MR_KeySelection[index]=key;
		m_LastSelectedRepetition=index;
	}
}
void CDB::MR_ResetSelection()
{
	  MR_ParSelection.clear();
	  MR_KeySelection.clear();
	  m_LastSelectedRepetition=0;
}
string CDB::MR_GetKeySelection(size_t index)
{
	if (MR_KeySelection.size() > index)
		return MR_KeySelection[index];
	else if (MR_KeySelection.size()!=0)
		return MR_KeySelection[MR_KeySelection.size()-1];
	else
	  return "";
}
string CDB::MR_GetSelection(size_t index)
{
	if(MR_ParSelection.size()>index)
	  return MR_ParSelection[index];
	else if (MR_ParSelection.size() != 0)
		return MR_ParSelection[MR_ParSelection.size()-1];
	else
	  return "";
}
size_t CDB::MR_GetNumberOfSelections()
{
	return MR_KeySelection.size();	
}
