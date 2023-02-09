#ifndef COUPSTD
//#include "../stdafx.h"
#endif
#include "../Util/FUtil.hpp"
#include "./CH.h"
//#include "../External.h"
// New Lapversion
// Further change on Desktop version
#include "./DB/DB_Plant.h"
#include "../ModelTypes/NewModelType.h"

static bool MultiRunChange;
extern int StartMode;

CH::CH()
{
		m_CH_SimCount=0;	
		m_NextDateToSetChangeAt=RunOpt.longMinStart;
}
bool CH::Make_ChaParCheck(unsigned int Act_Min) {

	if(Act_Min<m_ChaParMinute) return false;

	 if(CH_Checker_ChangeValue(m_CH_SimCount)) {
		 UpdateSimCount();
		 m_ChaParMinute=GetHit(m_CH_SimCount);
		 return true;
	 }
	 return false;
}
size_t CH::CH_Checker_build()
{
	// This function creates and array of all time points when some parameters may change value during a simulation
	int   imin_index;
	long next, actmin;
	//CUIntArray Counter;
	vector<unsigned int> Counter;
	int Type;
	bool Continue=false;
	Ps* pPar;
	P* pP;
	CDB* pDB;
	CHRunv runv;
	
	m_CH_Run_Array.clear();

	if(m_P_CH_Array.size()==0) return 0;
	pPar=nullptr;
	pP=nullptr;
	size_t TabIndex;
	for(int i=m_P_CH_Array.size()-1;i>=0;i--) {
		Counter.push_back(0);
		Continue=true;
		
		pPar=(Ps*)m_P_CH_Array[i].pBase;
		TabIndex=m_P_CH_Array[i].TabIndex;
		Type=m_P_CH_Array[i].Type;
		string str;
		double value;
		if (m_P_CH_Array[i].pBase != nullptr) {
			if (Type == 0) {
				pPar = (Ps*)m_P_CH_Array[i].pBase;
				value = pPar->GetValue();
				m_P_CH_Array[i].StartValue = value;
			}
			else if (Type == 2) {
				pDB = (CDB*)m_P_CH_Array[i].pBase;
				SaveTo_DB_Object(pDB->GetName(), "xx23Tmp");
			}
			else {
				pP = (P*)m_P_CH_Array[i].pBase;
				value = pP->GetValue(TabIndex);
				m_P_CH_Array[i].StartValue = value;
			}
		}
		value=4.;
	}

	while(Continue){
		Continue=false;
		next=RunOpt.longMinEnd+100;
		for(unsigned int i=0;i<m_P_CH_Array.size();i++){
			TabIndex=m_P_CH_Array[i].TabIndex;
			Type=m_P_CH_Array[i].Type;
			int testindex=Counter[i];
			int actindex;
			if(Type==2){ 
				pDB=(CDB*)m_P_CH_Array[i].pBase;
				actindex=pDB->GetNumChaDates();
			}
			else if(Type==1) {
				pP=(P*)m_P_CH_Array[i].pBase;
				actindex=pP->GetNumChaDates(TabIndex);				
			}
			else {
				pPar=(Ps*)m_P_CH_Array[i].pBase;
				actindex=pPar->GetNumChaDates();
			}
			if(actindex>testindex){
				if(Type==2) 
				   actmin=((CDB*)pPar)->GetChaDate(testindex);
				else if (Type==1)
				   actmin=pP->GetChaDate(TabIndex, testindex);
				else
					actmin=pPar->GetChaDate(testindex);
				
				if(actmin<next){
					if(Type==2) 
						next=((CDB*)pPar)->GetChaDate(testindex);
					else if(Type==1)
						next=pP->GetChaDate(TabIndex, testindex);
					else
						next=pPar->GetChaDate(testindex);

					imin_index=i;
					Continue=true;
				}
			}
		}
		if(Continue){
			runv.Ind_pP=imin_index;
			runv.Ind_Count=Counter[imin_index];

		//	m_CH_Index_pP.Add(imin_index);   				
		//	m_CH_Index_Count.Add(Counter.GetAt(imin_index)); 
			m_CH_Run_Array.push_back(runv);
			int newcounter=Counter[imin_index]+1;
			Counter[imin_index]=newcounter;
		}
	}
	return m_CH_Run_Array.size();
}
unsigned int CH::GetFirstHit() {
	m_CH_SimCount=0;

	m_ChaParMinute=GetHit(m_CH_SimCount);
	return m_ChaParMinute;
}
unsigned int CH::GetHit(size_t index) {
	size_t ip, icount, TabIndex;
	int Type;
	CDB *pDB;
	P *pP;
	Ps *pPar;

	if(index<m_CH_Run_Array.size()) {
		ip=m_CH_Run_Array[index].Ind_pP;//  m_CHm_CH_Index_pP.GetAt(m_CH_SimCount);
		icount=m_CH_Run_Array[index].Ind_Count;
		Type=m_P_CH_Array[ip].Type;
		TabIndex=m_P_CH_Array[ip].TabIndex;
		if(Type==2) {
			pDB=(CDB*)m_P_CH_Array[ip].pBase;
			return pDB->GetChaDate(icount);

		}
		else if(Type==1) {
			pP=(P*)m_P_CH_Array[ip].pBase;
			return pP->GetChaDate(TabIndex, icount);
		}
		else {
			pPar=(Ps*)m_P_CH_Array[ip].pBase;
			return pPar->GetChaDate(icount);
		}
		//Counter.clear();
	}
	else
		return RunOpt.longMinStart+(RunOpt.longMinEnd-RunOpt.longMinStart)*m_DocFile.m_TimePeriodScaleFactor+10;

	
}
void CH::CH_Checker_Reset()
{
	double newvalue;
	Ps* pPar;
	P* pP;
	CDB* pDB;
	size_t TabIndex;
	int Type;

	for(int i=m_P_CH_Array.size()-1;i>=0;i--){

		Type=m_P_CH_Array[i].Type;
		newvalue=m_P_CH_Array[i].StartValue;
		if(Type==1){
			TabIndex=m_P_CH_Array[i].TabIndex;
			pP=(P*)m_P_CH_Array[i].pBase;
			pP->SetValue(TabIndex, newvalue);
			//pP->Apply(TabIndex+1);
		}
		else if(Type==0){
			pPar=(Ps*)m_P_CH_Array[i].pBase;
			pPar->SetValue(newvalue);
			//pPar->Apply();
		}
		else {
			pDB=(CDB*)m_P_CH_Array[i].pBase;
			string name=pDB->GetName();
			ReadFrom_DB_Object(pDB->GetName(),"xx23Tmp",true);
			//PP_EDIT retrieve(name,,this, (CDB*)pPar);
			//retrieve.DeleteKeyFile();
		}
	}  
}
bool CH::CH_Checker_ChangeValue(size_t index )
{
	int ip,  TabIndex;
	unsigned int icount;
	double newvalue;
	string newstr, strkey, strname;
	Ps* pPar;
	P* pP;
	CDB* pDB;
	bool Valid;
	int Type;

	//_CrtMemState s1, s2 , s3, s4, s5, s6, s7, s8, s9, s10,s11,s12;
	//_CrtMemCheckpoint( &s1 );
	pPar=nullptr;
	pP=nullptr;
	
	if(m_CH_Run_Array.size()<=index) return false;
	Valid=true;
	ip=m_CH_Run_Array[index].Ind_pP; //m_CH_Index_pP.GetAt(index);
	icount=m_CH_Run_Array[index].Ind_Count; // m_CH_Index_Count.GetAt(m_CH_SimCount);

	Type=m_P_CH_Array[ip].Type;

	TabIndex=m_P_CH_Array[ip].TabIndex;


	if(Type==0) {
		pPar=(Ps*)m_P_CH_Array[ip].pBase;
		TabIndex=-1;
		newvalue=pPar->GetChaParValue(icount);
	}
	else if(Type==2){
		pDB=(CDB*)m_P_CH_Array[ip].pBase;
		strname=pDB->GetName();
		strkey=pDB->GetChaParKey(icount);
		TabIndex=-1;
	}
	else {
		pP=(P*)m_P_CH_Array[ip].pBase;
		newvalue=pP->GetChaParValue(TabIndex, icount);
	}

	
	if(Type==2){
		if(!ReadFrom_DB_Object(strname,strkey, false))
			Valid=false;
	}
	else if(Type==0) {
		pPar->SetValue(newvalue);
		//pPar->Apply();
	}
	else {
		pP->SetValue(TabIndex, newvalue);
		//pP->Apply(TabIndex+1);
	}

	return Valid;
}


bool CH::ApplyOptStartValues()
{
	X* pXT;
	unsigned int i=0;
	int nnn;

	while(i<m_X_Array.size())	{
		pXT=(X*)m_X_Array[i].pBase;
		if(pXT->IsEnabled()){
			if(pXT->Is_Vector()) {
				nnn=((OutVector*)pXT)->GetNumberOfFlags();
				int nlocal=0;
				while(nlocal<nnn) {
						pXT->ApplyInitialValue(nlocal);
						nlocal++;
		   		}
			}
			else {



			}
		}
		i++;
	}

return true;
}
bool CH::Reset_P_CH(SimB* Sw){
	for(size_t i=0;i<m_P_CH_Array.size();i++){
		if(Sw==m_P_CH_Array[i].pBase) {
			m_P_CH_Array.erase(m_P_CH_Array.begin()+i);
			return false;
		}
	}
	return true;
}
bool CH::Set_P_CH(SimB* Sw, int type, int index){

	if(type>100) return false;
	bool newpar;
	for(size_t i=0;i<m_P_CH_Array.size();i++){
		newpar=false;
		if(Sw==m_P_CH_Array[i].pBase&&type==m_P_CH_Array[i].Type) {
			
			if(type==0) {
				m_P_CH_Array[i].NumDates=((Ps*)Sw)->GetNumChaDates();
			}
			else if (type==1) {
				if(index!=m_P_CH_Array[i].TabIndex) 
					newpar=true;
				else
					m_P_CH_Array[i].NumDates=((P*)Sw)->GetNumChaDates(index);
			}
			else if(type==2) {
				m_P_CH_Array[i].NumDates=((CDB*)Sw)->GetNumChaDates();
			}
			
			if(!newpar) return false;
		}
	}
	CHPARv nch;
	nch.pBase=Sw;
	nch.TabIndex=index;
	if(type==0) {
		nch.Type=0;
		nch.NumDates=((Ps*)Sw)->GetNumChaDates();
		nch.TabIndex=-1;

	}
	else if (type==1) {
		nch.Type=1;
		nch.NumDates=((P*)Sw)->GetNumChaDates(nch.TabIndex);
	}
	else if(type==2) {
		nch.Type=2;
		nch.NumDates=((CDB*)Sw)->GetNumChaDates();
	}
	m_P_CH_Array.push_back(nch);
	return true;
}
