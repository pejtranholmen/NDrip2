
#include "../std.h"
#include "../Util/FUtil.hpp"
#include "./ModelBase.h"
#include "./DB/DB_Plant.h"
#include "../ModelTypes/NewModelType.h"





ModelBase::ModelBase() {
	m_UserLevel=0;
	m_GroupEnabled.resize(60);

	m_IsMakingBayesianCalib=false;
	m_AppString="CoupModel";
	m_DocFile2.m_OriginFileName.clear();
	m_DocFile2.m_ExeFileDate.clear();
	m_DocFile2.m_DateRun.clear();
	m_DocFile2.m_Multi_MBin_File.clear();
	m_DocFile2.m_Comments.clear();
	m_DocFile2.m_RunId.clear();



	m_DocFile.m_TimePeriodScaleFactor=1;
	//m_DocFile.m_FileVersionNumber=67;
	//m_DocFile.m_FileVersionNumber = 68;	  // new from Dec 2019
	m_DocFile.m_FileVersionNumber = 69;	  // new from Jan 2020
	m_FileVersionNumberRead=0;
	m_DocFile.m_SimulationRunNo=1;
	m_DocFile.m_FinishedSimulation=false;
	m_DocFile.m_MultiRunning=false;
	m_DocFile.m_Running=false;

	m_DocFile.m_TimeCreated=0;
	m_DocFile.m_TimeModified=0;


	m_DocFile.ipredays=0;
	m_DocFile.ipostdays = 0;
	m_DocFile.m_MultiStoragelocked=false;



	for(size_t i=0;i<10;i++) {
		CDB *pDB = new CDB(i);
		m_DB_Array.push_back(pDB);
		DB_Modified.push_back(false);
		m_DB_ImportTimeT.push_back(0);
		m_DB_header[i].Type = 0;
		m_DB_header[i].IdNum = 0;
	}

	RUNINFO val;
	for(size_t i=0; i<14;i++) {
		switch(i) {
			case 0:
				val.Group="General";
				val.Name="Run Number";
				val.Format=0;

				break;
			case 1:
				val.Group="General";
				val.Name="No of MultiRun";
				val.Format=0;
				break;
			case 2:
				val.Group="Simulation Period";
				val.Name="Start Date";
				val.Format=1;
				break;
			case 3:
				val.Group="Simulation Period";
				val.Name="End Date";
				val.Format=1;
				break;
			case 4:
				val.Group="Simulation Period";
				val.Name="Pre Period Date";
				val.Format=1;
				break;
			case 5:
				val.Group="Simulation Period";
				val.Name="Scaling of Period";
				val.Format=0;
				break;
			case 6:
				val.Group="Simulation Time";
				val.Name="Input Time Resolution";
				val.Format=0;
				break;
			case 7:
				val.Group="Simulation Time";
				val.Name="Output Time interval";
				val.Format=0;
				break;
			case 8:
				val.Group="Simulation Time";
				val.Name="Number of Iterations";
				val.Format=0;
				break;
			case 9:
				val.Group="Additional Info";
				val.Name="Short Id";
				val.Format=2;
				break;
			case 10:
				val.Group="Additional Info";
				val.Name="Comment";
				val.Format=2;
				break;
			case 11:
				val.Group="Executions";
				val.Name="Started";
				val.Format=2;
				break;
			case 12:
				val.Group="Executions";
				val.Name="Finnished";
				val.Format=2;
				break;
			case 13:
				val.Group="File Info Created";
				val.Name="NewRunNo";
				val.Format=2;
				break;
		}
		val.pBase = new CRunInfo(i);

		m_RunInfo.push_back(val);



	}
	m_MultiStorageBinFileExist=true;
	m_AnnimChartX=2;
	m_AnnimChartY=2;
#ifndef NO_MS_GUI
	p_PFCurve = &PlotpF::GetPointer();
	if(p_PFCurve->IsOpen()) {
	  string directory;
#ifndef STD
	  m_DataBaseDirectory=FUtil::GetProfileStringStd("DataBaseDirectory", directory);
	  p_PFCurve->SetDataBaseDirectory(m_DataBaseDirectory);
	  p_PFCurve->OpenMainFile(m_DataBaseDirectory+"PfProf");
#endif
	}
	else {
		m_DataBaseDirectory = FUtil::GetProfileStringStd("DataBaseDirectory", "");
		p_PFCurve->SetDataBaseDirectory(m_DataBaseDirectory);

	}
#endif
 

};
ModelBase::~ModelBase(){
	for(size_t i=0; i<m_DB_Array.size();i++)
		delete m_DB_Array[i];
	for(size_t i=0;i<14;i++)
		delete m_RunInfo[i].pBase;



};
void ModelBase::SetSingleSimulation()
{
	m_DocFile.m_MultiRunning=false;

}
void ModelBase::SetMultiSimulation()
{
	m_DocFile.m_MultiRunning=true;
}
bool ModelBase::IsMultiSimulation()
{
	return m_DocFile.m_MultiRunning;
}
bool ModelBase::IsFinishedSimulation()
{
	if(m_DocFile.m_FinishedSimulation)
		return true;
	else
		return false;
}
void ModelBase::SetNewSimulation()
{
	m_DocFile.m_FinishedSimulation=false;
}
size_t ModelBase::MR_Get_ActualRunNumber()
{	
	size_t CountTotal=1;
	for(size_t i= MR_GetNumberOfDimensions();i>0;i--) {
		if(MR_GetNumberOfRepeationsForThisDimension(i-1)!=string::npos){
			size_t Count=MR_GetDimCounter(i-1)-1;
			for (size_t ii = i - 1; MR_GetNumberOfRepeationsForThisDimension(ii - 1) != string::npos; ii--) 
				Count *= MR_GetNumberOfRepeationsForThisDimension(ii - 1);

			CountTotal+=Count;
		}
	}
	return CountTotal;																				 
}
size_t ModelBase::MR_Get_TotalNumberofRuns()
{	
	m_MultiRun_TotalNumber=1;
	for(size_t i=0;i<MR_GetNumberOfDimensions();i++)
		m_MultiRun_TotalNumber*=MR_GetNumberOfRepeationsForThisDimension(i);
	if(IsMultiSimulation()&&IsFinishedSimulation()&&IsMultiStorageBinFileValid()) {
		
		auto ifiles=m_MStorage.GetNumberOfAddedFiles(GetMBinFileName().c_str());
		if(ifiles!=string::npos)
			for(size_t i=0;i<ifiles;i++)
				m_MultiRun_TotalNumber+=m_MStorage.GetNumberOfAddedRuns(i);
	}
	return m_MultiRun_TotalNumber;
}
size_t ModelBase::MR_Get_NumberofRuns()
{	
	size_t MultiRun_Number=1;
	for(size_t i=0;i<MR_GetNumberOfDimensions();i++)
		MultiRun_Number*=MR_GetNumberOfRepeationsForThisDimension(i);

	return MultiRun_Number;
}


bool ModelBase::SetSimPeriodFromClimateFile()
{
	// 1 Check Switch
	auto pSW = dynamic_cast<Sw*>(GetPtr(SWITCH, "Apply Simulation Period from Sim File"));
	if (pSW != nullptr) {
		if (pSW->GetIntValue() == 0) return false;
	}
	else
		return false;
	// 2 Check PG Climate File
	CPG* pPG = nullptr;
	auto pF = dynamic_cast<F*>(GetPtr(PGFILE, "Meteorological data"));
	if (pF != nullptr) {
		pPG=pF->GetPointer();
		if (!pF->CheckFileNameAndAssignNameToPGClass()) return false;
	}

	if (pPG == nullptr) return false;
	// 3 Read Start and End from File

	//4 Read Time Resolution
	auto pSw = dynamic_cast<Sw*>(GetPtr(SWITCH, "TimeResolution"));
	// 4 Assign the values
	size_t adjustTime = 0;
	if (pSw->GetIntValue() == 1) adjustTime = 720;
	else if (pSw->GetIntValue() == 2) adjustTime = 30;
	else if (pSw->GetIntValue() == 3) adjustTime = 5;

	RunOpt.longMinEnd = pPG->GetLongTimeEnd()+adjustTime;
	if (RunOpt.longMinPostEnd < RunOpt.longMinEnd)   RunOpt.longMinPostEnd = RunOpt.longMinEnd;

	return true;
}

bool ModelBase::SetOutputFileNameToOutputFile(string filename)
{
	OutputFilePointer()->SetValue(filename);
	return false;
}

string ModelBase::GetWShedFileName_In(){
	Ps* pIn=dynamic_cast<Ps*>(GetPtrByName("HFlow Input Index"));
	size_t inum=size_t(pIn->GetValue()+0.0001);
	return GetWShedFileName(inum);
}

string ModelBase::GetWShedFileName_Out(){
	Ps* pOut=dynamic_cast<Ps*>(GetPtrByName("HFlow Output Index"));
	size_t inum=size_t(pOut->GetValue()+0.0001);
	return GetWShedFileName(inum);

}
string ModelBase::GetWShedFileName(size_t inum){
	string numstr=FUtil::ItoNumAscii(inum);

	string dir;
	
	size_t 	pos=m_DocFileName.rfind("\\");
	if(pos!=string::npos)
		dir=m_DocFileName.substr(0,pos+1);
	else {
		dir=GetCurrentWorkDirectory();
		//dir=::GetProfileString("UserDirectory", m_UserDirectory);
	}

	dir+="WShed_"+numstr+".bin";
	if(inum>0&&inum<1000000)
		return dir;
	else
		return "";

}
string ModelBase::GetCurrentSimFile()
{
	return m_CurrentFile;
}


string ModelBase::GetCurrentWorkDirectory()
{
	if(IsMultiSimulation())
		return GetMultiSubDir();
	else
		return m_CurrentDirectory;
}
string ModelBase::GetMultiSubDir()
{
	size_t ust=m_CurrentFile.rfind("\\");
	
	string file=m_CurrentFile.substr(ust+1);
	size_t upos=file.find("_");

	file=file.substr(0,upos);

	if(upos>0) 
		return m_CurrentDirectory+file+"\\";
	else
		return "";

}
size_t ModelBase::MR_GetNumberOfDimensions()
{
   return m_MultiRun_Array.size();
}
size_t ModelBase::MR_GetNumberOfRepeationsForThisDimension(size_t index)
{
   	if(m_MultiRun_Array.size()>index)
		return m_MultiRun_Array[index].NumberOfRepeatitionsWithinDimension;
	else
		return string::npos;

}
size_t ModelBase::MR_GetDimCounter(size_t index)
{
	if(m_MultiRun_Array.size()>index)
		return m_MultiRun_Array[index].nCount;
	else
		return string::npos;
}
size_t ModelBase::GetNumOutPuts()
{
	if(!IsFinishedSimulation()) {
		//PARAMETERS_mp_CAL_NUM_OUTPUTS();
		m_NumOutPuts=GetSize_XTGD_Total();//PARAMETERS_mp_NUM_OUTPUTS;
	}
	return m_NumOutPuts;
}
void ModelBase::Clear_XTGD()
{
	m_X_Array.clear();
	m_T_Array.clear();
	m_G_Array.clear();
	m_D_Array.clear();
}
size_t ModelBase::GetSize_XTGD()
{
	return m_X_Array.size()+m_T_Array.size()+m_G_Array.size()+m_D_Array.size();
}
size_t ModelBase::GetSize_XTGD_Total()
{
	size_t count=0;
	vector<SIMB> vv;
	OutVector *pVector;
	for(size_t i=0;i<4;i++) {
		switch(i) {
		case 0:
			vv=m_X_Array;
			break;
		case 1:
			vv=m_T_Array;
			break;
		case 2:
			vv=m_G_Array;
			break;
		case 3:
			vv=m_D_Array;
			break;
		}
		size_t n;
		if(m_FileVersionNumberRead<60) {
			for(size_t j=0; j<vv.size();j++) {
				pVector=dynamic_cast<OutVector*>(vv[j].pBase);
				if(pVector!=nullptr)
					n=pVector->GetNumberOfFlags();
				else
					n=1;
				count+=n;
			}
		}
		else
			count+=vv.size();
	}  
	return count;
}
vector<SIMB> ModelBase::GetOutVector(size_t itype)
{
	switch(itype) {
	case 0:
		return m_X_Array;
		break;
	case 1:
		return m_T_Array;
		break;
	case 2:
		return m_G_Array;
		break;
	case 3:
		return m_D_Array;
		break;
	}
	vector<SIMB> tom;
	return tom;
}
vector<SIMB> ModelBase::GetAllOutPutVectorByCategory(int category) {
	vector<SIMB> out, outfinal;;// , outsort, 

		out = m_X_Array;
		for (size_t i = 0; i<m_T_Array.size(); i++)
			out.push_back(m_T_Array[i]);
		for (size_t i = 0;i< m_G_Array.size(); i++)
			out.push_back(m_G_Array[i]);
		for (size_t i = 0; i<m_D_Array.size(); i++)
			out.push_back(m_D_Array[i]);

#ifndef NO_MS_GUI
		out = FUtil::SortSimB_ByName(out);
		out = FUtil::SortSimB_ByGroup(out);
		out = FUtil::SortSimB_ByProcess(out);
#endif


		
		vector<SIMB> outcat;
		for (size_t i = 0; i < out.size(); i++) {
			if (category==0||p_ModelInfo->GetGroupCategory(out[i].pBase->GetGroup()) == category)
				outcat.push_back(out[i]);
		}
		for (size_t i = 0; i < outcat.size(); i++) {
				int selected_Single;
				bool selected_Multi;
				auto pSw = outcat[i].pBase;
				auto ii = outcat[i].TabIndex;
				OutSingle *pSingle = nullptr;
				OutVector *pVector = nullptr;
				pVector = dynamic_cast<OutVector*>(pSw);
				size_t num_flags = 1;
				if (pVector != nullptr) {
					num_flags = pVector->GetNumberOfSelectedFlags();
					pSingle = nullptr;
				}
				else {
					num_flags = 1;
					pSingle = dynamic_cast<OutSingle*>(pSw);
				}

				if (pSw->Is_Vector()) {
					selected_Single = pVector->GetStoreFlag(ii);
					selected_Multi = false;
					if (selected_Single == 2) selected_Multi = true;
				}
				else {
					if (pSingle == nullptr) continue;
					selected_Single = pSingle->GetStoreFlag();
					selected_Multi = false;
				}
				if (selected_Single || selected_Multi) {
					outfinal.push_back(outcat[i]);
				}
			}		
		return outfinal;
}
SimB* ModelBase::GetPtrFromIntType(size_t itype, string group, string name) {
	string type;
	switch(itype) {
	case 1: case 8:
			type="State Variables";
			break;
	case 2: case 9:
		 	type="Flow Variables";
		 	break;
	case 3: case 10:
			type="Auxiliary Variables";
	 		break;
	case 4: case 11:
		 	type="Driving Variables";
	 		break;
	}
	if(type.size()==0) return nullptr;
	return GetPtr(type, group, name);
}
SimB* ModelBase::GetPtr(string type, string group, string name)
{
	size_t index=p_ModelInfo->GetGroupIndex(group);
	if(index<0 ) return nullptr;
	//switch () {

		if(type=="Switches") {
			auto it=MapSw[index].find(name);
			if(it!=MapSw[index].end())
				return it->second;
			else
				return nullptr;
		}
		else if(type=="Switches") {
			auto it=MapSw[index].find(name);
			if(it!=MapSw[index].end())
				return it->second;
			else
				return nullptr;
		}
		else if(type=="Tables") {
			auto it=MapTab[index].find(name);
			if(it!=MapTab[index].end())
				return it->second;
			else
				return nullptr;
		}
		else if(type=="Files") {
			auto it=MapF[index].find(name);
			if(it!=MapF[index].end())
				return it->second;
			else
				return nullptr;
		}
		else if(type=="Auxiliary Variables") {
			auto it=MapG[index].find(name);
			if(it!=MapG[index].end())
				return it->second;
			else {
			
				auto it=MapGs[index].find(name);
				if(it!=MapGs[index].end())
					return it->second;
				return nullptr;
			}
		}
		else if(type=="State Variables") {
			auto it=MapX[index].find(name);
			if(it!=MapX[index].end())
				return it->second;
			else {
				auto it=MapXs[index].find(name);
				if(it!=MapXs[index].end())
					return it->second;						
				return nullptr;
			}
		}
		else if(type=="Flow Variables") {
			auto it=MapT[index].find(name);
			if(it!=MapT[index].end())
				return it->second;
			else {
				auto it=MapTs[index].find(name);
				if(it!=MapTs[index].end())
					return it->second;								
				return nullptr;
			}
		}
		else if(type=="Driving Variables") {
			auto it=MapD[index].find(name);
			if(it!=MapD[index].end())
				return it->second;
			else {
				auto it=MapDs[index].find(name);
				if(it!=MapDs[index].end())
					return it->second;								
				return nullptr;			
			}
		}
		else if(type=="Parameters") {
			auto it=MapP[index].find(name);
			if(it!=MapP[index].end())
				return it->second;
			else
				return nullptr;
		}
		else if(type=="Table Parameters") {
			auto it=MapPt[index].find(name);
			if(it!=MapPt[index].end())
				return it->second;
			else
				return nullptr;
		}
		/*case 1:
			it=MapP[index].find(name);
			if(it!=MapP[index].end())
				return it->second;
			else
				return nullptr;
		case 2:
			it=MapTab[index].find(name);
			if(it!=MapTab[index].end())
				return it->second;
			else
				return nullptr;
		case 3:
			if(MapPt[index].empty()) return nullptr;
			it=MapPt[index].find(name);
			if(it!=MapPt[index].end())
				return it->second;
			else
				return nullptr;
		case 4:
			it=MapX[index].find(name);
			if(it!=MapX[index].end())
				return it->second;
			else
				return nullptr;
		case 5:
			it=MapT[index].find(name);
			if(it!=MapT[index].end())
				return it->second;
			else
				return nullptr;
		case 6:
			it=MapG[index].find(name);
			if(it!=MapG[index].end())
				return it->second;
			else
				return nullptr;
		case 7:
			it=MapD[index].find(name);
			if(it!=MapD[index].end())
				return it->second;
			else
				return nullptr;
		case 8:
			it=MapF[index].find(name);
			if(it!=MapF[index].end())
				return it->second;
			else
				return nullptr;*/
		//default:
		//	return nullptr;
	return nullptr;

}
SimB* ModelBase::GetPtr(enum simtype typ, string name, string group) {
	SimB* pSimB;
	auto in = GlobalMap.count(name);
	if (GlobalMap.count(name) > 0) {
		for (auto it = GlobalMap.equal_range(name).first; it != GlobalMap.equal_range(name).second; ++it) {
			pSimB = (*it).second;
			if (size_t(typ) == pSimB->GetType()) {
				if (group == "")
					return pSimB;
				else if (pSimB->GetGroup() == group)
					return pSimB;
			}
		}
	}

	return nullptr;
}

SimB* ModelBase::GetPtr(simtype typ1, simtype typ2, string name) {
	SimB* pSimB;
	auto in = GlobalMap.count(name);
	if (GlobalMap.count(name) > 0) {
			for (auto it = GlobalMap.equal_range(name).first; it != GlobalMap.equal_range(name).second; ++it) {
				pSimB = (*it).second;
				if (pSimB->GetType()>= size_t(typ1) && pSimB->GetType() <= size_t(typ2)) {
						return pSimB;
				}
			}
	}
	return nullptr;
}

SimB* ModelBase::GetPtrByName(string name, string group) {
		SimB *pSimB;
		auto in=GlobalMap.count(name);
		if(GlobalMap.count(name)>0) {
			for (auto it=GlobalMap.equal_range(name).first; it!=GlobalMap.equal_range(name).second; ++it) {
				 pSimB=(*it).second;
				 if(group=="")
					 return pSimB;
				 else if(pSimB->GetGroup()==group)
						return pSimB;
				 
			}
		}
		
		return nullptr;
}
vector<SimB*> ModelBase::GetPtrVector(simtype typ,  bool All)
{
	SimB *pSimB;
	vector<SimB*> vptr;

	for (auto it=GlobalMap.begin(); it!=GlobalMap.end(); ++it) {
				pSimB=(*it).second;
				if(typ==pSimB->GetType()&&(pSimB->IsEnabled()||All))
					vptr.push_back(pSimB);
	}

	return vptr;
}
vector<SimB*> ModelBase::GetPtrVector(simtype typ,size_t GroupNo,  bool All)
{
	SimB *pSimB;

	vector<SimB*> vptr;
	for (auto it=GlobalMap.begin(); it!=GlobalMap.end(); ++it) {
			 pSimB=(*it).second;
			 if(GroupNo<0) {
				 if(typ==pSimB->GetType()&&(pSimB->IsEnabled()||All))
				 vptr.push_back(pSimB);
			 }
			 else if((typ==pSimB->GetType()&&pSimB->GetGroupNo()==GroupNo)&&(pSimB->IsEnabled()||All))
				 vptr.push_back(pSimB);
	}
	return vptr;
}
vector<SimB*> ModelBase::GetAllPtr(simtype typ) {
	vector<SimB*> vptr;
	SimB* pSimB;
	for (auto it = GlobalMap.begin(); it != GlobalMap.end(); ++it) {
		pSimB = (*it).second;
		if(typ == pSimB->GetType())
			vptr.push_back(pSimB);
	}
	return vptr;
}
vector<SimB*> ModelBase::GetAllPtr(simtype typ1, simtype typ2, simtype typ3, simtype typ4) {
	vector<SimB*> vptr;
	SimB* pSimB;
	for (auto it = GlobalMap.begin(); it != GlobalMap.end(); ++it) {
		pSimB = (*it).second;
		if (typ1 == pSimB->GetType() || typ2 == pSimB->GetType() || typ3 == pSimB->GetType() || typ4 == pSimB->GetType())
			vptr.push_back(pSimB);
	}
	return vptr;
}

vector<SimB*> ModelBase::GetPtrVector(string type, string group, bool All)
{
	vector<SimB*> vptr;
		if(type=="Switches") {
				for(auto it=GlobalMap.begin(); it!=GlobalMap.end();it++) {
					if((All||(*it).second->IsEnabled())&&(*it).second->GetType()==SWITCH&&((*it).second->GetGroup()==group||group.size()==0))
						vptr.push_back((*it).second);
				}
		}
		else if(type=="Parameters") {
				for(auto it=GlobalMap.begin(); it!=GlobalMap.end();it++) {
					if((All||(*it).second->IsEnabled())&&(*it).second->GetType()==PAR_SINGLE&&((*it).second->GetGroup()==group||group.size()==0))
						vptr.push_back((*it).second);
				}
		}
		else if(type=="Tables"){
				for(auto it=GlobalMap.begin(); it!=GlobalMap.end();it++) {
					if((All||(*it).second->IsEnabled())&&((*it).second->GetType()==TABLE)&&((*it).second->GetGroup()==group||group.size()==0))
						if(((Tab*)(*it).second)->CheckEnabled()&&(*it).second->IsEnabled()||All)
							vptr.push_back((*it).second);
				}
		}
		else if(type=="Table Parameters"){
				for(auto it=GlobalMap.begin(); it!=GlobalMap.end();it++) {
					if((All||(*it).second->IsEnabled())&&((*it).second->GetType()==PAR_TABLE)&&((*it).second->GetGroup()==group||group.size()==0))
						vptr.push_back((*it).second);
				}
		}
		else if(type=="State Variables"){
				for(auto it=GlobalMap.begin(); it!=GlobalMap.end();it++) {
					if((*it).second->IsEnabled()&&((*it).second->GetType()==STATE||(*it).second->GetType()==STATE_SINGLE))
						vptr.push_back((*it).second);
				}
		}
		else if(type=="Flow Variables"){
				for(auto it=GlobalMap.begin(); it!=GlobalMap.end();it++) {
					if((*it).second->IsEnabled()&&((*it).second->GetType()==FLOW||(*it).second->GetType()==FLOW_SINGLE))
						vptr.push_back((*it).second);
				}
		}
		else if(type=="Auxiliary Variables"){
				for(auto it=GlobalMap.begin(); it!=GlobalMap.end();it++) {
					if(((*it).second->IsEnabled()&&((*it).second->GetType()==AUX||(*it).second->GetType()==AUX_SINGLE)))
						vptr.push_back((*it).second);
				}
		}
		else if(type=="Driving Variables"){
				for(auto it=GlobalMap.begin(); it!=GlobalMap.end();it++) {
					if((*it).second->IsEnabled()&&((*it).second->GetType()==DRIVE||(*it).second->GetType()==DRIVE_SINGLE))
						vptr.push_back((*it).second);
				}
		}
		else if(type=="Files"){
				for(auto it=GlobalMap.begin(); it!=GlobalMap.end();it++) {
					if((All||(*it).second->IsEnabled())&&(*it).second->GetType()==PGFILE&&((*it).second->GetGroup()==group||group.size()==0))
						vptr.push_back((*it).second);
				}
		}
		else if (type == "CSV Files") {
			for (auto it = GlobalMap.begin(); it != GlobalMap.end(); it++) {
				if ((All || (*it).second->IsEnabled()) && (*it).second->GetType() == CSVFILE && ((*it).second->GetGroup() == group || group.size() == 0))
					vptr.push_back((*it).second);
			}
		}
    vptr=p_ModelInfo->SortSimB_ByGroup(vptr);
	return vptr;

}
vector<SimB*> ModelBase::GetPtrVector(string type, size_t igroup)
{
	vector<SimB*> vptr, vp;
	size_t index_s, index_e;
	enum simtype _typ;
	_typ =  simtype(0);
	//vector<void*> vptr;
	//map<string, SimB*>::iterator it;
	
	if(igroup==-1) {
		index_s=0;
		index_e=36;
	} 
	else {
		index_s=igroup;
	 	index_e=index_s+1;
	}
	if(type=="Switches")
		_typ=SWITCH;
	else if(type=="Parameters")
		_typ=PAR_SINGLE;
	else if(type=="Tables")
		_typ=TABLE;
	else if(type=="Table Parameters")
		_typ=PAR_TABLE;
	else if(type=="State Variables")
		_typ=STATE;
	else if(type=="Flow Variables")
		_typ=FLOW;
	else if(type=="Auxiliary Variables")
		_typ=AUX;
	else if(type=="Driving Variables")
		_typ=DRIVE;
	else if(type=="Files")
		_typ=PGFILE;


		for(size_t index=index_s; index<index_e;index++) {
			vp=GetPtrVector(_typ, index, true);
			for(size_t i=0; i<vp.size(); i++)
				vptr.push_back(vp[i]);					
		}

   vptr=p_ModelInfo->SortSimB_ByGroup(vptr);
	return vptr;

}
vector<SimB*> ModelBase::GetPtrVector(size_t itype, size_t igroup)
{

	vector<SimB*> vptr, vp;
	size_t index_s, index_e;
	enum simtype _typ;
	//vector<void*> vptr;
	//map<string, SimB*>::iterator it;
	
	if(igroup==-1) {
		index_s=0;
		index_e=38;
	} 
	else {
		index_s=igroup;
	 	index_e=index_s+1;
	}
	if(itype==0)
		_typ=SWITCH;
	else if(itype==1)
		_typ=PAR_SINGLE;
	else if(itype==2)
		_typ=TABLE;
	else if(itype==3)
		_typ=PAR_TABLE;
	else if(itype==4)
		_typ=STATE;
	else if(itype==5)
		_typ=FLOW;
	else if(itype==6)
		_typ=AUX;
	else if(itype==7)
		_typ=DRIVE;
	else if (itype == 8)
		_typ = PGFILE;
	else
		return vptr;

	if(igroup==-1) {
			vptr=GetPtrVector(_typ, true);
	}
	else {
			for(size_t index=index_s; index<index_e;index++) {
				vp=GetPtrVector(_typ, index, true);
				for(size_t i=0; i<vp.size(); i++)
					vptr.push_back(vp[i]);					
			}
	}
	
	vptr=p_ModelInfo->SortSimB_ByGroup(vptr);
	return vptr;
}
vector<size_t> ModelBase::GetEnabledGroupNumbers(bool all) {
	SimB* pSimB;
	vector<size_t> vptr;
	map<size_t, size_t> groups;

	for (auto it = GlobalMap.begin(); it != GlobalMap.end(); ++it) {
		pSimB = (*it).second;
		if(pSimB->IsEnabled()) groups.insert(pair<size_t, size_t>(pSimB->GetGroupNo(), 10));
	}
	for (auto it = groups.begin(); it != groups.end(); ++it) {
		 vptr.push_back((*it).first);
	}
	if(m_EnabledCategories.size()==0||all)
		return vptr;
	else {
		vector<size_t> final_selection;
		for (size_t i = 0; i < m_EnabledCategories.size(); i++) {
			for (size_t ii = 0; ii < vptr.size(); ii++) {
				CommonModelInfo c;
				if (c.GetGroupCategoryNo(vptr[ii]) == m_EnabledCategories[i])
					final_selection.push_back(vptr[ii]);
			}
		}
		return final_selection;
	}
}
void ModelBase::UpdateEnabledElementvector() {
	SimB* pSimB;

	map<elements, size_t> elements;
	map<fysprocess, size_t> fysproc;
	map<bioprocess, size_t> bioproc;
	for (size_t groupno : m_UpdatedGroupNoVector) {
		for (auto it = GlobalMap.begin(); it != GlobalMap.end(); ++it) {
			pSimB = (*it).second;
			if (pSimB->IsEnabled() && pSimB->GetGroupNo() == groupno) {
				elements.insert(pair<enum::elements, size_t>(pSimB->GetElement(), 10));
				if(fysprocess::NOFPROC!=  fysprocess(pSimB->GetFysProcess())) fysproc.insert(pair<fysprocess, size_t>( fysprocess(pSimB->GetFysProcess()), 10));
				if(bioprocess::NOBPROC !=  bioprocess(pSimB->GetBioProcess())) bioproc.insert(pair<bioprocess, size_t>( bioprocess(pSimB->GetBioProcess()), 10));
			}
		}
	}
	m_EnabledElementsVector.clear();
	for (auto it = elements.begin(); it != elements.end(); it++) {
		m_EnabledElementsVector.push_back((*it).first);
	}
	m_EnabledFysProcVector.clear();
	for (auto it = fysproc.begin(); it != fysproc.end(); it++) {
		m_EnabledFysProcVector.push_back((*it).first);
	}
	m_EnabledBioProcVector.clear();
	for (auto it = bioproc.begin(); it != bioproc.end(); it++) {
		m_EnabledBioProcVector.push_back((*it).first);
	}
}
void ModelBase::UpdateEnabledProcvectors() {
	SimB* pSimB;

	map<fysprocess, size_t> fysproc;
	map<bioprocess, size_t> bioproc;
	for (size_t groupno : m_UpdatedGroupNoVector) {
		for (auto it = GlobalMap.begin(); it != GlobalMap.end(); ++it) {
			pSimB = (*it).second;
			if (pSimB->IsEnabled() && pSimB->GetGroupNo() == groupno) {
				for (elements e : m_UpdatedElementsVector) {
					if (e == pSimB->GetElement()) {
						if (fysprocess::NOFPROC !=  fysprocess(pSimB->GetFysProcess())) fysproc.insert(pair<fysprocess, size_t>( fysprocess(pSimB->GetFysProcess()), 10));
						if (bioprocess::NOBPROC !=  bioprocess(pSimB->GetBioProcess())) bioproc.insert(pair<bioprocess, size_t>( bioprocess(pSimB->GetBioProcess()), 10));
						continue;
					}
				}

			}
		}
	}
	m_EnabledFysProcVector.clear();
	for (auto it = fysproc.begin(); it != fysproc.end(); it++) {
		m_EnabledFysProcVector.push_back((*it).first);
	}
	m_EnabledBioProcVector.clear();
	for (auto it = bioproc.begin(); it != bioproc.end(); it++) {
		m_EnabledBioProcVector.push_back((*it).first);
	}


}

string ModelBase::DB_GetChoice(DB_types index)
{
	if(index<10) 
		return m_DB_choices[index];
	else
		return "";
}
void ModelBase::DB_SetChoice(DB_types index, string value)
{
   m_DB_choices[index]=value;
   CDB *pDB=(CDB*)m_DB_Array[index];
   if(pDB!=nullptr)  pDB->SetValue(value);
}
CDB* ModelBase::DB_GetPointer(DB_types index)
{
	if(m_DB_Array.size()>size_t(DB_types(index)))
		return (CDB*)m_DB_Array[index];
	else
		return nullptr;
}
CDB* ModelBase::DB_GetPointer(string str)
{
	if(str=="Plant")
		return m_DB_Array[0];
	else if(str=="Soil Properties")
		return m_DB_Array[1];
	else if(str=="Soil evaporation")
		return m_DB_Array[2];
	else if(str=="Snow")
		return m_DB_Array[3];
	else if(str=="Soil frost")
		return m_DB_Array[4];
	else if	(str=="Soil water flows")
		return m_DB_Array[5];
	else if (str=="Plant Growth"||str=="Plant growth")
		return m_DB_Array[6];
	else if(str=="Site Management"||str=="Site management")
		return m_DB_Array[7];
	else if(str=="Soil organic processes")
		return m_DB_Array[8];
	else if(str=="Soil mineral N processes")
		return m_DB_Array[9];
	else
		return m_DB_Array[9];
}
void ModelBase::SetFinishedSimulation()
{
	m_DocFile.m_FinishedSimulation=true;
}
F* ModelBase::OutputFilePointer()
{	
	F* pF;	
	string name="Output File";
	string grp="Hidden";
	string typ="Files";

	pF=(F*)GetPtr(PGFILE, name);
	return pF;

}


string ModelBase::GetMBinFileName()
{
	return m_DocFile2.m_Multi_MBin_File;
}
string ModelBase::GetXBinFileName(size_t ValidIndex, size_t AddFile) 
{
	string filename;
	string stdbinfilename;
	string dir=GetMultiSubDir();

	//dir=::GetProfileString("UserDirectory", m_UserDirectory);
	
	string str;
	long actRunNo;
	size_t iposext;
		   if(IsMultiSimulation()) {
				string NewFolder=GetCurrentSimFile();
				size_t ipp=NewFolder.find("_Multi");
				if(ipp<0) ipp=NewFolder.find(" Multi_");
 
				dir=NewFolder.substr(0,ipp)+"\\";
				actRunNo=MR_Get_ActualRunNumber();
				
				iposext=m_DocFile2.m_Multi_MBin_File.find(".MBin");
				auto iposnum=m_DocFile2.m_Multi_MBin_File.rfind("_");
				auto numl=iposext-iposnum-1;
				if(numl>0)
					str=m_DocFile2.m_Multi_MBin_File.substr(iposext-numl,numl);
				else if(iposext>4)
					str=m_DocFile2.m_Multi_MBin_File.substr(iposext-5,5);
				else
					str=FUtil::ItoNumAscii(m_DocFile.m_SimulationRunNo-actRunNo+1);
			}
		   else {
				str=FUtil::STD_ItoAscii(m_DocFile.m_SimulationRunNo);
		   }
		   if(AddFile==0) {
				stdbinfilename="Residuals_"+str+".XBin";

				if(ValidIndex>0) {
					filename=dir;
					filename+="V";
					filename+=FUtil::STD_ItoAscii(ValidIndex);
					filename+="_";
					filename+=stdbinfilename;								
				}
		   }
		   else {
				string strbase=m_MStorage.GetNameAddedFiles(AddFile-1);
				size_t ipos=strbase.find("_Multi_");
				auto iend=strbase.rfind(".MBin");
				auto ilen=iend-ipos-7;
				string numbase=strbase.substr(ipos+7,ilen);

				stdbinfilename="Residuals_"+numbase+".XBin";

				if(ValidIndex>0) {
					filename=dir;
					filename+="V";
					filename+=FUtil::STD_ItoAscii(ValidIndex);
					filename+="_";
					filename+=stdbinfilename;								
				}


		   }

			return filename;

}
string ModelBase::DB_GetNames(size_t index, bool AllIndex)
{
	if (!AllIndex&&index>0)
		return DB_Names[index+1];
	else
		return DB_Names[index];
}
time_t ModelBase::DB_GetImportTimeT(size_t index)
{
	if(m_DB_ImportTimeT.size()>index)
		return m_DB_ImportTimeT[index];
	else
		return 0;

}
void ModelBase::DB_SetImportTimeT(size_t index, time_t time)
{
	m_DB_ImportTimeT[index]=time;
}
void ModelBase::SetMultiStorageBinFileMissing()
{
	m_MultiStorageBinFileExist=false;
}
bool ModelBase::IsMultiStorageBinFileValid()
{
	return m_MultiStorageBinFileExist;
}
vector<SimB*> ModelBase::ReturnIfSelectionMade(vector<SimB*> vp) {
	OutVector* pVector;
	OutSingle* pSingle;
	vector<SimB*> vect;
	SIMB out;
	for (size_t j = 0; j < vp.size(); j++) {
		pVector = dynamic_cast<OutVector*>(vp[j]);
		if (pVector != nullptr) {
				if(pVector->GetNumberOfSelectedFlags(false)>0) vect.push_back(pVector);
		}
		else {
			pSingle = dynamic_cast<OutSingle*>(vp[j]);
			if (pSingle != nullptr) {
				if (pSingle->GetStoreFlag() > 0) vect.push_back(pSingle);
			}
		}
	}
	return vect;
}
vector<SIMB> ModelBase::SelectedOutVector(vector<SimB*> vp) {
	OutVector *pVector;
	OutSingle *pSingle;
	vector<SIMB> vect; 
	SIMB out;
	for( size_t j=0;j<vp.size();j++) {
			    pVector=dynamic_cast<OutVector*>(vp[j]);	
				if(pVector!=nullptr) {
						out.pBase=pVector;
						for(size_t ind=0; ind<pVector->GetNumberOfSelectedFlags(false); ind++) {
							out.TabIndex=pVector->GetSelectedLocalIndex(ind);			
							vect.push_back(out);
						}

				}
				else {
					pSingle=dynamic_cast<OutSingle*>(vp[j]);
					if(pSingle!=nullptr) {
						out.pBase=pSingle;
						out.TabIndex=-1;
						if(pSingle->GetStoreFlag()>0)
							vect.push_back(out);
					}
				}
	}
	return vect;
}
vector<SIMB> ModelBase::CompleteOutVector(vector<SimB*> vp) {
	OutVector *pVector;
	OutSingle *pSingle;
	vector<SIMB> vect; 
	SIMB out;
	for(size_t j=0;j<vp.size();j++) {
			    pVector=dynamic_cast<OutVector*>(vp[j]);	
				if(pVector!=nullptr) {
						out.pBase=pVector;
						for(size_t ind=0; ind<pVector->GetNumberOfFlags(); ind++) {
							out.TabIndex=ind;			
							vect.push_back(out);
						}

				}
				else {
					pSingle=dynamic_cast<OutSingle*>(vp[j]);
					if(pSingle!=nullptr) {
						out.pBase=pSingle;
						out.TabIndex=-1;
						vect.push_back(out);
					}
				}
	}
	return vect;
}

vector<SimB*> ModelBase::GetAllPtrOutputs(size_t choice) {
	vector<SimB*> out, allout;
	vector<SimB*> vp;

	vp.clear();
	if(choice==string::npos||choice==0) {
		vp=GetPtrVector(STATE, false);
		for(size_t i=0; i<vp.size(); i++)
			allout.push_back(vp[i]);

		vp=GetPtrVector(STATE_SINGLE,false);
		for(size_t i=0; i<vp.size(); i++)
			allout.push_back(vp[i]);

	}
	
	vp.clear();

	if(choice==string::npos||choice==1) {
		vp=GetPtrVector(FLOW,false);
		for(size_t i=0; i<vp.size(); i++)
			allout.push_back(vp[i]);
		vp.clear();
		vp=GetPtrVector(FLOW_SINGLE,false);
		for(size_t i=0; i<vp.size(); i++)
			allout.push_back(vp[i]);

	}

	vp.clear();
	if(choice==string::npos||choice==2) {

		vp=GetPtrVector(AUX,false);
		for(size_t i=0; i<vp.size(); i++)
			allout.push_back(vp[i]);
		vp.clear();
		vp=GetPtrVector(AUX_SINGLE,false);
		for(size_t i=0; i<vp.size(); i++)
			allout.push_back(vp[i]);

	}
	vp.clear();
	if(choice==string::npos||choice==3) {
		vp=GetPtrVector(DRIVE,false);
		for(size_t i=0; i<vp.size(); i++)
			allout.push_back(vp[i]);
		vp.clear();
		vp=GetPtrVector(DRIVE_SINGLE,false);
		for(size_t i=0; i<vp.size(); i++)
			allout.push_back(vp[i]);

	}

	return allout;
}
vector<SIMB> ModelBase::GetAllOutputs(size_t choice, bool all_out) {
	vector<SIMB> out, allout;
	vector<SimB*> vp;

	if(choice==string::npos||choice==0) {
		vp=GetPtrVector(STATE, all_out);
		out=CompleteOutVector(vp);
		for(size_t i=0; i<out.size();i++) 
				allout.push_back(out[i]);

		vp=GetPtrVector(STATE_SINGLE,all_out);
		out=CompleteOutVector(vp);
		for(size_t i=0; i<out.size();i++)
			allout.push_back(out[i]);

	}
	


	if(choice==string::npos||choice==1) {
		vp=GetPtrVector(FLOW,all_out);
		out=CompleteOutVector(vp);
		for(size_t i=0; i<out.size();i++)
			allout.push_back(out[i]);

		vp=GetPtrVector(FLOW_SINGLE,all_out);
		out=CompleteOutVector(vp);
		for(size_t i=0; i<out.size();i++)
			allout.push_back(out[i]);

	}


	if(choice==string::npos||choice==0) {

		vp=GetPtrVector(AUX,all_out);
		out=CompleteOutVector(vp);
		for(size_t i=0; i<out.size();i++)
			allout.push_back(out[i]);

		vp=GetPtrVector(AUX_SINGLE,all_out);
		out=CompleteOutVector(vp);
		for(size_t i=0; i<out.size();i++)
			allout.push_back(out[i]);

	}

	if(choice==string::npos||choice==3) {
		vp=GetPtrVector(DRIVE,all_out);
		out=CompleteOutVector(vp);
		for(size_t i=0; i<out.size();i++)
			allout.push_back(out[i]);
	
		vp=GetPtrVector(DRIVE_SINGLE,all_out);
		out=CompleteOutVector(vp);
		for(size_t i=0; i<out.size();i++)
			allout.push_back(out[i]);

	}

	return allout;
}
vector<SIMB> ModelBase::GetAllSelectedOutputs(bool MultiFlag) {
	vector<SIMB> out, allout;
	vector<SimB*> vp;
	OutSingle *pSingle;
	OutVector *pVector;

	vp=GetPtrVector(STATE, false);
	out=SelectedOutVector(vp);
	for(size_t i=0; i<out.size();i++) 
			allout.push_back(out[i]);

	vp=GetPtrVector(STATE_SINGLE, false);
	out=SelectedOutVector(vp);
	for(size_t i=0; i<out.size();i++)
		allout.push_back(out[i]);

	vp=GetPtrVector(FLOW, false);
	out=SelectedOutVector(vp);
	for(size_t i=0; i<out.size();i++)
		allout.push_back(out[i]);

	vp=GetPtrVector(FLOW_SINGLE, false);
	out=SelectedOutVector(vp);
	for(size_t i=0; i<out.size();i++)
		allout.push_back(out[i]);

	vp=GetPtrVector(AUX, false);
	out=SelectedOutVector(vp);
	for(size_t i=0; i<out.size();i++)
		allout.push_back(out[i]);

	vp=GetPtrVector(AUX_SINGLE, false);
	out=SelectedOutVector(vp);
	for(size_t i=0; i<out.size();i++)
		allout.push_back(out[i]);

	vp=GetPtrVector(DRIVE, false);
	out=SelectedOutVector(vp);
	for(size_t i=0; i<out.size();i++)
		allout.push_back(out[i]);
	
	vp=GetPtrVector(DRIVE_SINGLE, false);
	out=SelectedOutVector(vp);
	for(size_t i=0; i<out.size();i++)
		allout.push_back(out[i]);
	

	if(MultiFlag) { 
		// Select those that have StoreFlag==2
		out.clear();
		for(size_t i=0; i<allout.size();i++) {
			pSingle=dynamic_cast<OutSingle*>(allout[i].pBase);
			if(pSingle!=nullptr){
				if(pSingle->GetStoreFlag()==2) out.push_back(allout[i]);
			}
			else  {
				pVector=dynamic_cast<OutVector*>(allout[i].pBase);
				if(pVector!=nullptr) {
					if(pVector->GetStoreFlag(allout[i].TabIndex)==2)
						out.push_back(allout[i]);

				}



			}

		}
		return out;
	}
	else 
		return allout;
}

void ModelBase::RemoveAllSelectedOutputs() {

	vector<SIMB> pvv;
	pvv = GetAllSelectedOutputs();
	size_t ivv = pvv.size();

	OutVector *pVector;
	OutSingle *pSingle;
	size_t storeflag;
	for (size_t i = 0; i < pvv.size(); i++) {
		pVector = dynamic_cast<OutVector*>(pvv[i].pBase);
		if (pVector != nullptr) {
			auto lin = pvv[i].TabIndex;
			storeflag = pVector->GetStoreFlag(lin);
			pVector->SetStoreFlag(lin, 0);
		}
		else {
			pSingle = dynamic_cast<OutSingle*>(pvv[i].pBase);
			if (pSingle != nullptr) {
				storeflag = pSingle->GetStoreFlag();
				pSingle->SetStoreFlag(0);
			}
		}

	}
	Clear_XTGD();
}
size_t ModelBase::SelectAllOutputs() {
	OutVector *pVector;
	OutSingle *pSingle;
	vector<SIMB> pvv;
	pvv = GetAllOutputs();
	size_t count=0;
	for (size_t i = 0; i < pvv.size(); i++) {
		pVector = dynamic_cast<OutVector*>(pvv[i].pBase);
		if (pVector != nullptr) {
			auto lin = pvv[i].TabIndex;
			pVector->SetStoreFlag(lin, 1);
			count++;
		}
		else {
			pSingle = dynamic_cast<OutSingle*>(pvv[i].pBase);
			if (pSingle != nullptr) {
				pSingle->SetStoreFlag(1);
				count++;
			}
		}
	}
	return count;
}
vector<SIMB> ModelBase::GetAllSelectedOutputsSortedByName(bool MultiFlag) {
	vector<SIMB> out;
	out = GetAllSelectedOutputs(MultiFlag);

#ifndef NO_MS_GUI
	out = FUtil::SortSimB_ByName(out);
#endif
	return out;
}
vector<string> ModelBase::GetAllSelectedNameList(bool MultiFlag) {
	vector<string> NameList;
	vector<SIMB> SIMBList=GetAllSelectedOutputsSortedByName(MultiFlag);
	string str;
	for (size_t i = 0; i < SIMBList.size(); i++) {
		str = SIMBList[i].pBase->GetName();
		if (SIMBList[i].TabIndex != string::npos) {
			str += "(";
			str += FUtil::STD_ItoAscii(SIMBList[i].TabIndex + 1);
			str += ")";
		}
		NameList.push_back(str);
	}
	return NameList;
}


vector<size_t> ModelBase::GetPressureHeadOutputIndex(vector<SIMB> allout) {
	vector<size_t> out;
	SimB* pPressureHead;
	pPressureHead=GetPtr(AUX,"PressureHead");
	for(size_t i=0; i<allout.size(); i++) 
		if(allout[i].pBase==pPressureHead) out.push_back(i);
	return out;

}
void ModelBase::CleanValFlags() {
	vector<SIMB> vvp;
	SimB *pSimB;
	vvp=GetAllOutputs();

	for(size_t i=0; i<vvp.size(); i++)	{
			pSimB=vvp[i].pBase;
			if(pSimB->Is_Vector())
				((OutVector*)pSimB)->SetValidFlagIndex(vvp[i].TabIndex, -1);
			else 
				((OutSingle*)pSimB)->SetValidFlagIndex(-1);
	}
}
size_t ModelBase::GetNumAll_TSV_ValVariables()
{

	return	m_Val_Array.size();

}

void ModelBase::SetRunReportArray(string st)
{
	m_Report_Array.push_back(st);
}
vector<SIMB> ModelBase::GetWShedOutputs() {
	vector<SIMB> out, allout;

	vector<SimB*> vp;
	SimB* pSimB;
	SIMB simb;

	OutVector *pVector;

	pSimB=GetPtr(FLOW,"WaterDrainflow");

	pVector=dynamic_cast<OutVector*>(pSimB);
	if(pVector!=nullptr) {
		simb.pBase=pSimB;
		for(size_t i=0;i<pVector->GetNumVector();i++) {
			simb.TabIndex=i;
			allout.push_back(simb);
		}
	}

	pSimB=GetPtr(FLOW,"SaltDrainFlow");
	if(pSimB->IsEnabled()) {
		pVector=dynamic_cast<OutVector*>(pSimB);
		if(pVector!=nullptr) {
			simb.pBase=pSimB;
			for(size_t i=0;i<pVector->GetNumVector();i++) {
				simb.TabIndex=i;
				allout.push_back(simb);
			}
		}
	}


	simb.pBase=GetPtr(FLOW_SINGLE,"SpoolRunoff");
	simb.TabIndex=string::npos;
	allout.push_back(simb);
	simb.pBase=GetPtr(FLOW_SINGLE,"DeepPerc");
	allout.push_back(simb);

	return allout;

}
float ModelBase::GetActualObsValueFromValSumFlex(string IDFileName, string  IDVarName, size_t runno) {
	/*float fvalue;
	VALSUMLINK valsumlink;
	size_t index_var, index_run, index_file, index_key_file;
	index_file = string::npos;
	index_run = string::npos;
	for (size_t i = 0; i<m_ValsumFlexFiles.size(); i++) {
		if (IDFileName == m_ValsumFlexFiles[i].FileName) {
			index_file = i;
			continue;
		}
		else if (IDFileName == "")
			index_file = 0;
	}
	index_var = 0;
	auto it = MapFlexId[index_file].find(std::string(IDVarName));
	if (it != MapFlexId[index_file].end()) {
		index_var = (*it).second;
	};
	index_key_file = 0;
	auto it2 = MapFlexId[index_file].find("ID_PLOT");
	if (it2 != MapFlexId[index_file].end()) {
		index_key_file = (*it2).second;
	};
	float runcode = m_MStorage.GetMultiP(runno, 0);
	size_t icode = size_t(runcode + 0.01);

	auto it3 = MapFlexVal[index_file].find(icode);
	if (it3 != MapFlexVal[index_file].end()) {
		index_run = (*it3).second;
		fvalue = m_ValFlexValue[index_file][index_run][index_var];
	}
	else*/
	float	fvalue = MISSING;

	return fvalue;



}

void ModelBase::CheckFileNameLinkedto_SV_Variables()
{
	VALSUMv vst;

	for (size_t i = 0; i < m_ValSum_Array.size(); i++) {
		vst = m_ValSum_Array[i];
		if (!FUtil::IsFileExisting(vst.IDFileName)) {
			//Suggest new file name
			string shortfilename = FUtil::FileNameOnly(vst.IDFileName);
			string dir = GetCurrentWorkDirectory();
			vst.IDFileName = dir + shortfilename;




			if (FUtil::IsFileExisting(vst.IDFileName))
					m_ValSum_Array[i] = vst;
			else {
				vst.IDFileName = m_CurrentDirectory + shortfilename;
			}
			if (FUtil::IsFileExisting(vst.IDFileName))
				m_ValSum_Array[i] = vst;
			else {

				size_t ix = 0;
				while (m_ValsumFlexFiles.size()>ix&&FUtil::FileNameOnly(m_ValsumFlexFiles[ix].FileName) != shortfilename) {
					ix++;

				}
				if (ix < m_ValsumFlexFiles.size()) {
					m_ValSum_Array[i].IDFileName = m_CurrentDirectory+shortfilename;

				}




			}
		}

	}


}

string ModelBase::GetExistingDocFileName(string FileName)
{
	size_t count = 0;
	while (!FUtil::IsFileExisting(FileName) && count<3) {
		string fileonly = FUtil::FileNameOnly(FileName);
		count++;
		switch (count) {
		case 1:
			FileName = m_CurrentDirectory + fileonly;
			break;
		case 2:
			FileName = GetCurrentWorkDirectory() + fileonly;
			break;
		}

	}
	return FileName;
}
string ModelBase::GetTimeCreated()
{
	if (m_DocFile.m_TimeCreated > 0) {
		struct tm timeinfo;
#ifdef MS_CODE
		localtime_s(&timeinfo, &m_DocFile.m_TimeCreated);
#else
		localtime_r( &m_DocFile.m_TimeCreated, &timeinfo);
#endif
		char buffer[24];
		strftime(buffer, 24, "%Y-%m-%d %H:%M:%S", &timeinfo);
		return string(buffer);
	}
	else
		return "";

}
string ModelBase::GetTimeModified()
{
	if(m_DocFile.m_TimeModified>0) {
		struct tm timeinfo;
#ifdef MS_CODE
		localtime_s(&timeinfo, &m_DocFile.m_TimeModified);
#else		
		localtime_r(&m_DocFile.m_TimeModified, &timeinfo);
#endif
		char buffer[24];
		strftime(buffer,24, "%Y-%m-%d %H:%M:%S", &timeinfo);
		return string(buffer);
		return "";
	}
	else
	return "";

}


