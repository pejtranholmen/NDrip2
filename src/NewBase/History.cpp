
#include "../std.h"
#include "../Util/FUtil.hpp"
#include "./History.h"
#include <time.h>
#ifndef COUPSTD
#include "../Util/MFC_Util.h"
#endif



History::History() {
	m_ToView[0] = true;
	m_ToView[1] = true;
	m_ToView[2] = true;
	m_ToView[3] = true;
	m_ToView[4] = true;
	m_ToView[5] = true;
	m_ToView[6] = true;
	m_ViewAll = true;
#ifndef COUPSTD
	if(MFC_Util::GetProfileIntNo("Hist0", 0)==0) m_ToView[0] = false;
	if (MFC_Util::GetProfileIntNo("Hist1", 0) == 0) m_ToView[1] = false;
	if (MFC_Util::GetProfileIntNo("Hist2", 0) == 0) m_ToView[2] = false;
	if (MFC_Util::GetProfileIntNo("Hist3", 0) == 0) m_ToView[3] = false;
	if (MFC_Util::GetProfileIntNo("Hist4", 0) == 0) m_ToView[4] = false;
	if (MFC_Util::GetProfileIntNo("Hist5", 0) == 0) m_ToView[5] = false;
	if (MFC_Util::GetProfileIntNo("Hist6", 0) == 0) m_ToView[6] = false;
	if(MFC_Util::GetProfileIntNo("HistViewAll", 0) == 0) m_ViewAll = false;
#endif

}
bool History::History_Add(SimB *pBase,int Lindex, time_t t, float v, string User) {
	int itype;
	if (pBase->GetType() == PAR_TABLE)
		itype = int(HIST_INFO::PARTABLE_INFO);
	else
		itype = int(HIST_INFO::PARSINGLE_INFO);

	return History_Add(pBase,itype, Lindex, t, v, User);
	

}
bool History::History_Add(SimB *pBase,int Lindex, time_t t, string str, string User) {
	int itype=0;

	
	if (pBase->GetType() == PAR_TABLE)
		itype = int(HIST_INFO::PARTABLE_INFO);
	else if(pBase->GetType()== SWITCH)
		itype = int(HIST_INFO::SWITCH_INFO);
	else if (pBase->GetType()== DB) 
		itype = int(HIST_INFO::DATA_BASE_INFO);
	else if (pBase->GetType() == PGFILE)
		itype = int(HIST_INFO::MODELFILE_INFO);


	return History_Add(pBase,itype, Lindex, t, str, User);

}

bool History::History_Add(SimB *pBase,int iType, int Lindex, time_t t, float v, string User)
{

#ifndef COUPSTD
	//if(p_ModelInfo->IsRunning()) return false;
#endif
	
	string strApp=m_AppString;
	HISTv Node;

	Node.pBase=pBase;
	Node.LocalIndex=Lindex;
	Node.RunInfoType=iType;

	size_t i=m_History_Array.size();
	if(Lindex<0) Lindex=0;
	pBase->AddHistoryIndex(Lindex,i);



	Node.tvalue=t;
	Node.value=v;
	Node.strvalue=FUtil::STD_FtoAscii(v);
	if(User=="")
		User=FUtil::GetProfileStringStd("Signature", User);
	string Comp=FUtil::GetProfileStringStd("ComputerName", User);

	Node.User=User;
	Node.RunNo=m_DocFile.m_SimulationRunNo;
	Node.Computer=FUtil::GetProfileStringStd("ComputerName", User);
	m_History_Array.push_back(Node);
	return true;

}
bool History::History_Add(SimB *pBase, int iType, int Lindex, time_t t, string str, string User)
{
	HISTv Node;

#ifndef COUPSTD
	//if(p_ModelInfo->IsRunning()) return false;
#endif
	Node.pBase=pBase;
	Node.LocalIndex=Lindex;
	Node.RunInfoType=iType;
	size_t i=m_History_Array.size();
	if(Lindex<0) Lindex=0;
	pBase->AddHistoryIndex(Lindex,i);
	Node.tvalue=t;
	Node.value=float(MISSING);
	Node.strvalue=str;
	Node.RunNo=m_DocFile.m_SimulationRunNo;
	if(User=="")
		User=FUtil::GetProfileStringStd("Signature", User);
	Node.User=User;
	Node.Computer=FUtil::GetProfileStringStd("ComputerName", User);
	m_History_Array.push_back(Node);
	return true;

}

size_t History::History_GetNumber()
{
	if (m_ViewAll)
		return m_History_Array.size();
	else
		return UpdateListToView();
}
bool History::History_RemoveAll()
{
	//while(!m_History_Array.IsEmpty()) {
		//	CHistoryNode *pNode=(CHistoryNode*)m_History_Array.RemoveHead();
			//delete pNode;
	//}
	m_History_Array.clear();
	return true;
}
bool History::History_Remove(size_t index)
{	
	if(index<m_History_Array.size())
		m_History_Array.erase(m_History_Array.begin()+index);
	else
		return false;


	return true;
}

SimB* History::History_GetPointer(size_t index)
{
	if(index<m_History_Array.size())
		return m_History_Array[index].pBase;
	else 
		return nullptr;
}

time_t History::History_GetTime(size_t index)
{
	if(index<m_History_Array.size())
		return m_History_Array[index].tvalue;
	else 
		return 0;
}
size_t History::History_GetRunInfoType(size_t index)
{
	if(index<m_History_Array.size())
		return m_History_Array[index].RunInfoType;
	else 
		return 0;
}
size_t History::History_GetEnumType(size_t index)
{
	if (index < m_History_Array.size())
		return size_t(HIST_INFO(m_History_Array[index].RunInfoType));
	else
		return 0;
}
string History::History_GetUser(size_t index)
{
	if(index<m_History_Array.size())
		return m_History_Array[index].User;
	else 
		return "";
}
size_t History::History_GetRunNo(size_t index)
{
	if(index<m_History_Array.size())
		return m_History_Array[index].RunNo;
	else 
		return 0;
}
size_t History::UpdateListToView()
{
	m_IndexToView.clear();
	for (size_t i = 0; i < m_History_Array.size(); i++) {
		auto a = m_History_Array[i];
		if (m_ToView[0] && HIST_INFO(a.RunInfoType) == HIST_INFO::RUN_INFO) m_IndexToView.push_back(i);
		else if(m_ToView[1] && HIST_INFO(a.RunInfoType) == HIST_INFO::DATA_BASE_INFO) m_IndexToView.push_back(i);
		else if (m_ToView[2] && HIST_INFO(a.RunInfoType) == HIST_INFO::SWITCH_INFO) m_IndexToView.push_back(i);
		else if (m_ToView[3] && HIST_INFO(a.RunInfoType) == HIST_INFO::PARSINGLE_INFO) m_IndexToView.push_back(i);
		else if (m_ToView[4] && HIST_INFO(a.RunInfoType) == HIST_INFO::NUMBER_ELEMENTS) m_IndexToView.push_back(i);
		else if (m_ToView[5] && HIST_INFO(a.RunInfoType) == HIST_INFO::PARTABLE_INFO) m_IndexToView.push_back(i);
		else if (m_ToView[6] && HIST_INFO(a.RunInfoType) == HIST_INFO::MODELFILE_INFO) m_IndexToView.push_back(i);
	}
	return size_t(m_IndexToView.size());
}
string History::History_GetComputer(size_t index)
{
	if(index<m_History_Array.size())
		return m_History_Array[index].Computer;
	else 
		return "";
}
size_t History::History_GetLocalIndex(size_t index)
{
	if(index<m_History_Array.size())
		return m_History_Array[index].LocalIndex;
	else 
		return 0;

}
string History::History_GetString(size_t index)
{
	if(index<m_History_Array.size())
		return m_History_Array[index].strvalue;
	else 
		return 0;
}
float History::History_GetFloat(size_t index)
{
	if(index<m_History_Array.size())
		return m_History_Array[index].value;
	else 
		return 0.;

}
bool History::History_Add(size_t index, string strvalue)
{
 
	size_t i=m_History_Array.size();
	SimB *pBase;
	pBase=(SimB*)m_RunInfo[index].pBase;
	if(pBase!=nullptr)
		pBase->AddHistoryIndex(i);
	HISTv Node;
	if(i>0)
		Node= m_History_Array[i-1];


 	string User,Computer;

	User=FUtil::GetProfileStringStd("Signature", User);
	Computer=FUtil::GetProfileStringStd("ComputerName", Computer);
	
	Node.tvalue=time(nullptr);
	Node.LocalIndex=index;
	Node.User=User;
	Node.Computer=Computer;
	auto ityp = size_t(HIST_INFO::RUN_INFO);// p_ModelInfo->GetTypeIndex("RunInfo");
	Node.RunInfoType=ityp;
	Node.pBase=pBase;
	Node.strvalue=strvalue;

	m_History_Array.push_back(Node);
		  
	return true;
}
void History::History_Add(string Group, string Type, string Name,string StringValue, int intValue, time_t tt, string User, string Computer)
{
   
	SimB *pBase;
	float fvalue;
	size_t RunNo;
	HISTv Node, NodePrev;
	if(intValue<0) intValue=0;
	size_t ityp;
	RunNo=m_DocFile.m_SimulationRunNo;
	if(m_History_Array.size()>0) {
		auto end=m_History_Array.size()-1;
		NodePrev=m_History_Array[end];
		RunNo=NodePrev.RunNo;
	}
	size_t i=m_History_Array.size();
	if(Type=="Run Info") {
		pBase=(SimB*)m_RunInfo[intValue].pBase;
		pBase->AddHistoryIndex(i);
		ityp = size_t(HIST_INFO::RUN_INFO);
		if(intValue==13) 
			RunNo=FUtil::AtoInt(StringValue);
	}
	else if (Type=="Data Base Object") {
		pBase=(SimB*)m_DB_Array[intValue];
		pBase->AddHistoryIndex(intValue,i);
		ityp = size_t(HIST_INFO::DATA_BASE_INFO);
	}
	else if (Type == "Validation") {
		pBase = (SimB*)GetPtr(Type, Group, Name);
		if (pBase != nullptr)
			pBase->AddHistoryIndex(intValue, i);
		ityp = p_ModelInfo->GetTypeIndex(Type);

	}
	else if (Type == "Parameters") {
		pBase = (SimB*)GetPtr(Type, Group, Name);
		if (pBase != nullptr)
			pBase->AddHistoryIndex(i);
		ityp = size_t(HIST_INFO::PARSINGLE_INFO);
	}
	else if (Type == "Parameter Tables"||Type=="Table Parameters") {
		pBase = (SimB*)GetPtrByName(Name);
		if (pBase != nullptr)
			pBase->AddHistoryIndex(intValue, i);
		ityp = size_t(HIST_INFO::PARTABLE_INFO);
	}
	else if (Type == "Switches") {
		pBase = (SimB*)GetPtr(Type, Group, Name);
		if (pBase != nullptr)
			pBase->AddHistoryIndex(intValue, i);
		ityp = size_t(HIST_INFO::SWITCH_INFO);
	}
	else if (Type == "Files") {
		pBase = (F*)GetPtrByName( Name);
		if (pBase != nullptr) {
			pBase->AddHistoryIndex(intValue, i);
			ityp = size_t(HIST_INFO::MODELFILE_INFO);
		}
		else
			return;


	}
	else {
		return;

	}


	
	Node.RunInfoType=ityp;
	Node.pBase=pBase;
	Node.LocalIndex=intValue;
	Node.tvalue=tt;
	if(pBase==nullptr) 
		fvalue=0;
	else if(StringValue.size()>0)
		fvalue=FUtil::AtoFloat(StringValue);
	else
		fvalue=0.;
	Node.value=fvalue;
	Node.strvalue=StringValue;
	Node.User=User;
	Node.Computer=Computer;
	Node.RunNo=RunNo;
	m_History_Array.push_back(Node);   
}	  
void History::History_NewAdd(size_t RunInfo, string Group, string Type, string Name,string StringValue, int intValue, time_t tt, string User, string Computer)
{

	SimB *pBase;
	float fvalue;
	size_t RunNo;
	HISTv Node, NodePrev;
	if(intValue<0) intValue=0;
	size_t ityp=0;
	RunNo=m_DocFile.m_SimulationRunNo;
	pBase=nullptr;
	if(m_History_Array.size()>0) {

		NodePrev=m_History_Array[m_History_Array.size()-1];
		RunNo=NodePrev.RunNo;
	}
	size_t i=m_History_Array.size();
	if(Type=="RunInfo"||Type=="Run Info") {
		if(intValue<14)	{
			pBase=(SimB*)m_RunInfo[intValue].pBase;
			pBase->AddHistoryIndex(i);
		}
		else
			return;

		ityp = size_t(HIST_INFO::RUN_INFO);
		if(intValue==13) 
			RunNo=FUtil::AtoInt(StringValue);
	}
	else if (Type == "Parameters") {
		pBase = (SimB*)GetPs( Name);
		if (pBase != nullptr)
			pBase->AddHistoryIndex(intValue, i);
		ityp = size_t(HIST_INFO::PARSINGLE_INFO);
	}
	else if (Type == "Table Parameters") {
		pBase = (SimB*)GetP( Name);
		if (pBase != nullptr)
			pBase->AddHistoryIndex(intValue, i);
		ityp = size_t(HIST_INFO::PARTABLE_INFO);
	}
	else if (Type == "Switches") {
		pBase = (SimB*)GetSw(Name);
		if (pBase != nullptr)
			pBase->AddHistoryIndex(intValue, i);
		ityp = size_t(HIST_INFO::SWITCH_INFO);
	}
	else if (Type == "ModelFiles"||RunInfo==6) {
		pBase = (SimB*)GetPtrByName(Name);
		if (pBase != nullptr)
			pBase->AddHistoryIndex(intValue, i);
		ityp = size_t(HIST_INFO::MODELFILE_INFO);
	}
	else if (Type=="Data Base Object") {
		if(intValue<10)	
		pBase=(SimB*)m_DB_Array[intValue];
		pBase->AddHistoryIndex(intValue,i);
		ityp=p_ModelInfo->GetTypeIndex("DataBase");
	}
	else if (Type == "Validation") {
		pBase = (SimB*)GetPtr(PAR_TABLE, Name);
		if (pBase != nullptr) {
			pBase->AddHistoryIndex(intValue, i);
			Type = "Table Parameters";
			ityp = size_t(HIST_INFO::PARTABLE_INFO);
			if (pBase->GetNumOfHistoryIndex(intValue) == 1) {
				// Create vector of Parameters in the HistoryPar_Array
				HistP element;
				element.LocalIndex = intValue;
				element.pBase = pBase;
				element.type = ityp;
				m_HistoryPar_Array.push_back(element);
			}

		}


	}
	else {
		if(Name=="Validation File") Name="Validation File1";
		pBase=(SimB*)GetPtr(Type, Group, Name);
		if(pBase!=nullptr)	  {
			pBase->AddHistoryIndex(intValue,i);
			ityp=p_ModelInfo->GetTypeIndex(Type);
			if(ityp==0) {
				ityp=p_ModelInfo->GetTypeIndex(Type);
			}		
			if(pBase->GetNumOfHistoryIndex(intValue)==1) {
			// Create vector of Parameters in the HistoryPar_Array
				HistP element;
				element.LocalIndex=intValue;
				element.pBase=pBase;
				element.type=ityp;
				m_HistoryPar_Array.push_back(element);
			}
		}
		else {
		   int i=0;
			ityp=0;
			return;
		}
	}

	
	Node.RunInfoType=ityp;
	Node.pBase=pBase;
	Node.LocalIndex=intValue;
	Node.tvalue=tt;
	if(pBase==nullptr) 
		fvalue=0;
	else if(StringValue.size()>0)
		fvalue=FUtil::AtoFloat(StringValue);
	else
		fvalue=0.;
	Node.value=fvalue;
	Node.strvalue=StringValue;
	Node.User=User;
	Node.Computer=Computer;
	Node.RunNo=RunNo;
	m_History_Array.push_back(Node);  
}
