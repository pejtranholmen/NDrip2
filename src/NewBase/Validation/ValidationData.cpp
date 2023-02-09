
#include "../../Util/FUtil.hpp"
#include "../NewMap.h"
#include "./ValidationData.h"
#include "../Sim.h"

#include <limits>

using namespace std;

ValidationData::ValidationData()
{
   m_pSimDoc=nullptr;
   m_look=false;
   m_TotalNumberSelected=0;
   m_IsSumValSet=false;
}
ValidationData::~ValidationData()
{
	while(m_PGCreatedPGFiles.size()>0) {
		CPG *pPG=m_PGCreatedPGFiles[0];
		if(pPG!=nullptr) delete pPG;
		m_PGCreatedPGFiles.erase(m_PGCreatedPGFiles.begin());
	}
}
bool ValidationData::Init(NewMap *pDoc, int itype)

{

	m_pSimDoc=pDoc;

	return true;
}
bool ValidationData::AddNewValList(size_t index,int FileNo, int valFileIndex, int outtype, string group, string name, int localindex, float perr, float aerr, bool ATest, bool LogTrans, int Dur)
{	string Type;
	SimB* Sw;

	if(FileNo<0) return false;

	 m_NumberSelectedValidationVar[FileNo-1]++;
	 F* pF = m_pSimDoc->ValidationFilePointer(size_t(FileNo));
	 pF->SetNumberOfSelectedVal(m_NumberSelectedValidationVar[FileNo-1]);
	//if(!ValidationFilePointer()->m_Exist) return false;
	if(index<pF->GetNumberOfSelectedVal())
		pF->SetValNumber(valFileIndex, index+1);
	else {
		pF->SetNumberOfSelectedVal(valFileIndex+1);
		pF->SetValNumber(valFileIndex, index+1);
	}

	 Sw=m_pSimDoc->GetPtr(Type, group, name);	
	 if(Sw!=nullptr){
		 if (Sw->Is_Vector())
			((OutVector*)Sw)->SetValidFlagIndex(localindex, index+1);
		 else 
			((OutSingle*)Sw)->SetValidFlagIndex(index+1);
	 }
	VALv Node;
	if(index<m_pSimDoc->m_Val_Array.size()) {
		Node=m_pSimDoc->m_Val_Array[index];
		Node.ValidationFileIndex=valFileIndex;
		Node.OutputType=outtype;
		Node.Group=group;
		Node.Name=name;
		Node.LocalIndex=localindex;
		Node.P_Error=perr;
		Node.A_Error=aerr;
		Node.AccTest=ATest;
		Node.ValidationFileNumber=FileNo;
		Node.LogTrans=LogTrans;
		m_pSimDoc->m_Val_Array[index]=Node;
		return true;
	}

 	 Node.ValidationFileIndex=valFileIndex;
	 Node.OutputType=outtype;
 	 Node.Group=group;
	 Node.Name=name;
	 Node.LocalIndex=localindex;
	 Node.nNumber=-1;
	 Node.R2     =float(MISSING);
	 Node.A0     =float(MISSING);
	 Node.A1     =float(MISSING);
	 Node.ME     =float(MISSING);
	 Node.RMSE   =float(MISSING);
	 Node.MeanSim=float(MISSING);
	 Node.MeanVal=float(MISSING);
	 Node.P_Error=perr;
	 Node.A_Error=aerr;
	 Node.AccTest=ATest;
	 Node.LogLi=float(MISSING);
	 Node.ValidationFileNumber=FileNo;
	 Node.LogTrans=LogTrans;
	 Node.Duration=Dur;
	 Node.NSE=float(MISSING);
	 m_pSimDoc->m_Val_Array.push_back(Node);
	 m_NumberSelectedValidationVar[FileNo-1]++;
	 m_pSimDoc->ValidationFilePointer(size_t(FileNo))->SetNumberOfSelectedVal(m_NumberSelectedValidationVar[FileNo-1]);
	 return false;
}
bool ValidationData::AddValList(size_t index,int valFileIndex, int outtype, string group, string name, int localindex, float perr, float aerr, bool ATest)
{	string Type;
	SimB* Sw;

	VALv Node;

	//if(!ValidationFilePointer()->m_Exist) return false;
	m_pSimDoc->ValidationFilePointer()->SetValNumber(valFileIndex, index+1);
	switch (outtype) {
		case 1:
			Type="State Variables";
			break;
		case 2:
		 	Type="Flow Variables";
		 	break;
	    case 3:
			Type="Auxiliary Variables";
	 		break;
		 case 4:
		 	Type="Driving Variables";
	 		break;
		 }
	 Sw=(SimB*)m_pSimDoc->GetPtr(Type, group, name);	
	 if(Sw!=nullptr){
		 if (outtype>=3)
			((G*)Sw)->SetValidFlagIndex(localindex, index+1);
		 else 
			((X*)Sw)->SetValidFlagIndex(localindex, index+1);
	 }
	if(index<m_pSimDoc->m_Val_Array.size()) {
		Node=m_pSimDoc->m_Val_Array[index];
		Node.ValidationFileIndex=valFileIndex;
		Node.OutputType=outtype;
		Node.Group=group;
		Node.Name=name;
		Node.LocalIndex=localindex;
		Node.P_Error=perr;
		Node.A_Error=aerr;
		Node.AccTest=ATest;
		m_pSimDoc->m_Val_Array[index]=Node;
		return true;
	}

 	 Node.ValidationFileIndex=valFileIndex;
	 Node.OutputType=outtype;
 	 Node.Group=group;
	 Node.Name=name;
	 Node.LocalIndex=localindex;
	 Node.nNumber=-1;
	 Node.R2     =float(MISSING);
	 Node.A0     =float(MISSING);
	 Node.A1     =float(MISSING);
	 Node.ME     =float(MISSING);
	 Node.RMSE   =float(MISSING);
	 Node.MeanSim=float(MISSING);
	 Node.MeanVal=float(MISSING);
	 Node.P_Error=perr;
	 Node.A_Error=aerr;
	 Node.AccTest=ATest;
	 Node.LogLi=float(MISSING);
	 m_pSimDoc->m_Val_Array.push_back(Node);
	 return false;
}
bool ValidationData::AddValList(size_t index,int valFileIndex, int outtype, string group, string name, int localindex)
{	string Type;
	SimB* Sw;


	if(!m_pSimDoc->ValidationFilePointer()->m_Exist) {
		int i=0;
		m_pSimDoc->ValidationFilePointer()->CheckFileNameAndAssignNameToPGClass();
	}
	m_pSimDoc->ValidationFilePointer()->SetValNumber(valFileIndex, index+1);
	switch (outtype) {
		case 1:
			Type="State Variables";
			break;
		case 2:
		 	Type="Flow Variables";
		 	break;
	    case 3:
			Type="Auxiliary Variables";
	 		break;
		 case 4:
		 	Type="Driving Variables";
	 		break;
		 }
	Sw=(SimB*)m_pSimDoc->GetPtr(Type, group, name);	
	 if(Sw!=nullptr){
		 if (outtype>=3)
			((G*)Sw)->SetValidFlagIndex(localindex, index+1);
		 else 
			((X*)Sw)->SetValidFlagIndex(localindex, index+1);
	 }
	VALv Node;
	if(index<m_pSimDoc->m_Val_Array.size()) {
		Node=m_pSimDoc->m_Val_Array[index];
		Node.ValidationFileIndex=valFileIndex;
		Node.OutputType=outtype;
		Node.Group=group;
		Node.Name=name;
		Node.LocalIndex=localindex;
				m_pSimDoc->m_Val_Array[index]=Node;

		return true;
	}

 	 Node.ValidationFileIndex=valFileIndex;
	 Node.OutputType=outtype;
 	 Node.Group=group;
	 Node.Name=name;
	 Node.LocalIndex=localindex;
	 Node.nNumber=-1;
	 Node.ValidationFileNumber=1;
	 Node.LogTrans=false;
	 Node.R2     =float(MISSING);
	 Node.A0     =float(MISSING);
	 Node.A1     =float(MISSING);
	 Node.ME     =float(MISSING);
	 Node.RMSE   =float(MISSING);
	 Node.MeanSim=float(MISSING);
	 Node.MeanVal=float(MISSING);
	 Node.P_Error=float(MISSING);
	 Node.A_Error=float(MISSING);
	 Node.LogLi=float(MISSING);
 	 m_pSimDoc->m_Val_Array.push_back(Node);

	 return false;
}
bool ValidationData::AddValSumVarList(size_t index, SIMSUMSOURCE source_type, size_t res_index, int sumvartype, SIMB Selected, string linkvarname, string linkfilename, float perror, float aerror)
{
	string Type;


	if (index<m_pSimDoc->m_ValSum_Array.size()) {
		VALSUMv Node = m_pSimDoc->m_ValSum_Array[index];
		Node.SumVarType = sumvartype;
		if (source_type == SIMSUMSOURCE::MULTIFILESTAT) {
			Node.OutputType = Selected.pBase->GetType();
			Node.Group = Selected.pBase->GetGroup();
			Node.Name = Selected.pBase->GetName();
		}
		else if (source_type == SIMSUMSOURCE::CSVFLEXFILE) {
			Node.Name = m_pSimDoc->m_ValSum_Array[res_index].Name;
			Node.Name += " - ";
			if (m_pSimDoc->m_ValSum_Array[res_index].FlexSumVar) {
				Node.Name += m_pSimDoc->m_ValSum_Array[res_index].IDVarName;
			}
		}
		else if (source_type == SIMSUMSOURCE::MULTI_OUTPGFILE) {
			Node.Name = m_pSimDoc->m_PG_MultiOutputFile.GetVarName(res_index+1);
			Node.Name += m_pSimDoc->m_PG_MultiOutputFile.GetVarId(res_index+1);
		}
		Node.LocalIndex = Selected.TabIndex;
		Node.ObsValue = MISSING;
		Node.P_Error = perror;
		Node.A_Error = aerror;
		Node.IDVarName = linkvarname;
		Node.IDFileName = linkfilename;
		Node.ObsScaling = 0;
		Node.ObsScalingFactor = 1.;
		Node.SimValueSource = source_type;
		Node.Sim_Value_Residual_Index = res_index;
		m_pSimDoc->m_ValSum_Array[index] = Node;
		return true;
	}
	VALSUMv Node;
	Node.SumVarType = sumvartype;
	if (source_type == SIMSUMSOURCE::MULTIFILESTAT) {
		Node.OutputType = Selected.pBase->GetType();
		Node.Group = Selected.pBase->GetGroup();
		Node.Name = Selected.pBase->GetName();
	}
	else if (source_type == SIMSUMSOURCE::CSVFLEXFILE) {
		Node.Name= m_pSimDoc->m_ValSum_Array[res_index].Name;
		Node.Name += " - ";
		if (m_pSimDoc->m_ValSum_Array[res_index].FlexSumVar) {
			Node.Name += m_pSimDoc->m_ValSum_Array[res_index].IDVarName;
		}
	}
	else if (source_type == SIMSUMSOURCE::MULTI_OUTPGFILE) {
		Node.Name = m_pSimDoc->m_PG_MultiOutputFile.GetVarName(res_index+1);
		Node.Name += m_pSimDoc->m_PG_MultiOutputFile.GetVarId(res_index+1);

	}
	Node.LocalIndex = Selected.TabIndex;
	Node.ME.push_back(MISSING);
	Node.RMSE.push_back(MISSING);
	Node.SimValue = float(MISSING);
	Node.IDVarName = linkvarname;
	Node.IDFileName = linkfilename;
	Node.ObsValue = MISSING;
	Node.P_Error = perror;
	Node.A_Error = aerror;
	Node.LogLi.push_back(double(MISSING));
	Node.Monitoring = 1;
	Node.ObsScaling = 0;
	Node.ObsScalingFactor = 1.;
	Node.FlexSumVar = true;
	Node.SimValueSource = source_type;
	Node.Sim_Value_Residual_Index = res_index;
	m_pSimDoc->m_ValSum_Array.push_back(Node);

	return false;
}
bool ValidationData::AddValSumVarList(size_t index, SIMSUMSOURCE source_type, size_t res_index, int sumvartype, SIMB Selected, float obsvalue, float perror, float aerror)
{
	string Type;






	if (index<m_pSimDoc->m_ValSum_Array.size()) {
		VALSUMv Node = m_pSimDoc->m_ValSum_Array[index];
		Node.SumVarType = sumvartype;
		if (source_type == SIMSUMSOURCE(0)) {
			Node.OutputType = Selected.pBase->GetType();
			Node.Group = Selected.pBase->GetGroup();
			Node.Name = Selected.pBase->GetName();
		}
		else if (source_type == SIMSUMSOURCE(1) || source_type == SIMSUMSOURCE(2)) {
			Node.Name = m_pSimDoc->m_ValSum_Array[res_index].Name;
			Node.Name += " - ";
			if (m_pSimDoc->m_ValSum_Array[res_index].FlexSumVar) {
				Node.Name += m_pSimDoc->m_ValSum_Array[res_index].IDVarName;
			}
		}
		Node.LocalIndex = Selected.TabIndex;
		Node.ObsValue = obsvalue;
		Node.P_Error = perror;
		Node.A_Error = aerror;
		Node.SimValueSource = source_type;
		Node.Sim_Value_Residual_Index = res_index;
		m_pSimDoc->m_ValSum_Array[index] = Node;
		return true;
	}
	VALSUMv Node;
	Node.SumVarType = sumvartype;
	if (source_type == SIMSUMSOURCE(0)) {
		Node.OutputType = Selected.pBase->GetType();
		Node.Group = Selected.pBase->GetGroup();
		Node.Name = Selected.pBase->GetName();
	}
	else if (source_type == SIMSUMSOURCE(1) || source_type == SIMSUMSOURCE(2)) {
		Node.Name = m_pSimDoc->m_ValSum_Array[res_index].Name;
		Node.Name += " - ";
		if (m_pSimDoc->m_ValSum_Array[res_index].FlexSumVar) {
			Node.Name += m_pSimDoc->m_ValSum_Array[res_index].IDVarName;
		}
	}
	Node.LocalIndex = Selected.TabIndex;
	Node.ME.push_back(MISSING);
	Node.RMSE.push_back(MISSING);

	Node.SimValue = float(MISSING);
	Node.ObsValue = obsvalue;
	Node.P_Error = perror;
	Node.A_Error = aerror;
	Node.LogLi.push_back(double(MISSING));
	Node.Monitoring = 1;
	Node.ObsScaling = 0;
	Node.ObsScalingFactor = 1.;
	Node.FlexSumVar = false;
	Node.SimValueSource = source_type;
	Node.Sim_Value_Residual_Index = res_index;
	m_pSimDoc->m_ValSum_Array.push_back(Node);

	return false;
}
bool ValidationData::AddValSumVarList(size_t index,int sumvartype, int outtype, string group, string name, int localindex, float obsvalue, float perror, float aerror )
{	string Type;
	SimB* Sw;




	switch (outtype) {
		case 1:
			Type="State Variables";
			break;
		case 2:
		 	Type="Flow Variables";
		 	break;
	    case 3:
			Type="Auxiliary Variables";
	 		break;
		 case 4:
		 	Type="Driving Variables";
	 		break;
		 }
		 Sw=(SimB*)m_pSimDoc->GetPtr(Type, group, name);	
		 if(Sw!=nullptr){
		 if (outtype>=3)
			((G*)Sw)->SetValidFlagIndex(localindex, index+1);
		 else 
			((X*)Sw)->SetValidFlagIndex(localindex, index+1);
		}
		if(index<m_pSimDoc->m_ValSum_Array.size()) {
		VALSUMv Node=m_pSimDoc->m_ValSum_Array[index];
		Node.SumVarType=sumvartype;
		Node.OutputType=outtype;
		Node.Group=group;
		Node.Name=name;
		Node.LocalIndex=localindex;
		Node.ObsValue=obsvalue;
		Node.P_Error=perror;
		Node.A_Error=aerror;
		m_pSimDoc->m_ValSum_Array[index]=Node;
		return true;
		}
	 VALSUMv Node;
 	 Node.SumVarType=sumvartype;
	 Node.OutputType=outtype;
 	 Node.Group=group;
	 Node.Name=name;
	 Node.LocalIndex=localindex;
	 Node.ME.push_back(MISSING);
	 Node.RMSE.push_back(MISSING);

	 Node.SimValue=float(MISSING);
	 Node.ObsValue=obsvalue;
	 Node.P_Error=perror;
	 Node.A_Error=aerror;
	 Node.LogLi.push_back(double(MISSING));

	 Node.Monitoring=1;
	 Node.ObsScaling = 0;
	 Node.ObsScalingFactor = 1.;
	 m_pSimDoc->m_ValSum_Array.push_back(Node);

	 return false;
}

bool ValidationData::AddValList(size_t index,int valFileIndex, int outtype, string group, string name, int localindex, float perr, float aerr)
{	string Type;
	SimB* Sw;

	VALv Node;


//	if(!ValidationFilePointer()->m_Exist) return false;
	m_pSimDoc->ValidationFilePointer()->SetValNumber(valFileIndex, index+1);
	switch (outtype) {
		case 1:
			Type="State Variables";
			break;
		case 2:
		 	Type="Flow Variables";
		 	break;
	    case 3:
			Type="Auxiliary Variables";
	 		break;
		 case 4:
		 	Type="Driving Variables";
	 		break;
		 }
	 Sw=(SimB*)m_pSimDoc->GetPtr(Type, group, name);	
	 if(Sw!=nullptr){
		 if (outtype>=3)
			((G*)Sw)->SetValidFlagIndex(localindex, index+1);
		 else 
			((X*)Sw)->SetValidFlagIndex(localindex, index+1);
	 }
	if(index<m_pSimDoc->m_Val_Array.size()) {
		Node=m_pSimDoc->m_Val_Array[index];
		Node.ValidationFileIndex=valFileIndex;
		Node.OutputType=outtype;
		Node.Group=group;
		Node.Name=name;
		Node.LocalIndex=localindex;
		Node.P_Error=perr;
		Node.A_Error=aerr;
		m_pSimDoc->m_Val_Array[index]=Node;
		return true;
	}

 	 Node.ValidationFileIndex=valFileIndex;
	 Node.OutputType=outtype;
 	 Node.Group=group;
	 Node.Name=name;
	 Node.LocalIndex=localindex;
	 Node.nNumber=-1;
	 Node.R2     =float(MISSING);
	 Node.A0     =float(MISSING);
	 Node.A1     =float(MISSING);
	 Node.ME     =float(MISSING);
	 Node.RMSE   =float(MISSING);
	 Node.MeanSim=float(MISSING);
	 Node.MeanVal=float(MISSING);
	 Node.P_Error=perr;
	 Node.A_Error=aerr;
	 Node.LogLi=float(MISSING);
	 Node.Monitoring=1;
 	 m_pSimDoc->m_Val_Array.push_back(Node);

	 return false;
}
bool ValidationData::SetValMonitoring(size_t index, int value)
{	
	VALv Node;
	if(index<m_pSimDoc->m_Val_Array.size()) {
		m_pSimDoc->m_Val_Array[index].Monitoring=value;
		m_ResetValMonitoring=true;
		return true;
	}
	
	 return false;
}
bool ValidationData::SetValErrors(size_t index, float perr, float aerr)
{	

	VALv Node;
	if(index<m_pSimDoc->m_Val_Array.size()) {
		m_pSimDoc->m_Val_Array[index].P_Error=perr;
		m_pSimDoc->m_Val_Array[index].A_Error=aerr;
		return true;
	}
	
	 return false;
}
bool ValidationData::SetValErrors(size_t index, float perr, float aerr, bool ATest, bool LTrans)
{	

	VALv Node;
	if(index<m_pSimDoc->m_Val_Array.size()) {
		m_pSimDoc->m_Val_Array[index].P_Error=perr;
		m_pSimDoc->m_Val_Array[index].A_Error=aerr;
		m_pSimDoc->m_Val_Array[index].AccTest=ATest;
		m_pSimDoc->m_Val_Array[index].LogTrans=LTrans;
		return true;
	}
	
	 return false;
}
bool ValidationData::SetValSumVarErrors(size_t index, int ityp, float obs, float perr, float aerr)
{	


	if(index<m_pSimDoc->m_ValSum_Array.size()) {

 		m_pSimDoc->m_ValSum_Array[index].SumVarType=ityp;
		m_pSimDoc->m_ValSum_Array[index].ObsValue=obs;
		m_pSimDoc->m_ValSum_Array[index].P_Error=perr;
		m_pSimDoc->m_ValSum_Array[index].A_Error=aerr;
		return true;
	}
	
	 return false;
}
bool ValidationData::SetValCoef(size_t index, int n, float r2, float a0, float a1, float me, float rmse, float msim, float mobs)
{

	VALv Node;
	if(index<m_pSimDoc->m_Val_Array.size()) {
		Node=m_pSimDoc->m_Val_Array[index];
		Node.nNumber=n;
		Node.R2     =r2;
		Node.A0     =a0;
		Node.A1     =a1;
		Node.ME     =me;
		Node.RMSE   =rmse;
		Node.MeanSim=msim;
		Node.MeanVal=mobs;
		Node.LogLi=float(MISSING);
		Node.P_Error=float(MISSING);
		Node.A_Error=float(MISSING);
		Node.NSE=float(MISSING);
		m_pSimDoc->m_Val_Array[index]=Node;
		return true;
	}
	return false;
}
bool ValidationData::SetValCoef(size_t index, int n, float r2, float a0, float a1, float me, float rmse, float msim, float mobs, float perr, float aerr, double logli,  bool A_Test)
{
	if(!m_pSimDoc->ValidationFilePointer()->m_Exist) return false;

	VALv Node;
	if(index<m_pSimDoc->m_Val_Array.size()) {
		Node=m_pSimDoc->m_Val_Array[index];
		Node.nNumber=n;
		Node.R2     =r2;
		Node.A0     =a0;
		Node.A1     =a1;
		Node.ME     =me;
		Node.RMSE   =rmse;
		Node.MeanSim=msim;
		Node.MeanVal=mobs;
		Node.P_Error=perr;
		Node.A_Error=aerr;
		Node.LogLi=logli;
		Node.AccTest=A_Test;
		m_pSimDoc->m_Val_Array[index]=Node;
		return true;
	}
	return false;
}
bool ValidationData::SetValCoef(size_t index, int n, float r2, float a0, float a1, float me, float rmse, float msim, float mobs, float perr, float aerr, double logli,  bool A_Test, float nse)
{
	if(!m_pSimDoc->ValidationFilePointer()->m_Exist) return false;

	VALv Node;
	if(index<m_pSimDoc->m_Val_Array.size()) {
		if(n==0) {
			a1=0;
			a0=float(MISSING);
			me=float(MISSING);
			rmse=float(MISSING);
			msim=float(MISSING);
			logli=float(MISSING);
			nse=float(MISSING);
		}
		Node=m_pSimDoc->m_Val_Array[index];
		Node.nNumber=n;
		Node.R2     =r2;
		Node.A0     =a0;
		Node.A1     =a1;
		Node.ME     =me;
		Node.RMSE   =rmse;
		Node.MeanSim=msim;
		Node.MeanVal=mobs;
		Node.P_Error=perr;
		Node.A_Error=aerr;
		Node.LogLi=logli;
		Node.AccTest=A_Test;
		Node.NSE=nse;
		m_pSimDoc->m_Val_Array[index]=Node;

		return true;
	}
	return false;
}
bool ValidationData::SetValCoef(size_t index, int n, float r2, float a0, float a1, float me, float rmse, float msim, float mobs, float perr, float aerr, double logli)
{
	if(!m_pSimDoc->ValidationFilePointer()->m_Exist) return false;

	VALv Node;
	if(index<m_pSimDoc->m_Val_Array.size()) {
		Node=m_pSimDoc->m_Val_Array[index];
		Node.nNumber=n;
		Node.R2     =r2;
		Node.A0     =a0;
		Node.A1     =a1;
		Node.ME     =me;
		Node.RMSE   =rmse;
		Node.MeanSim=msim;
		Node.MeanVal=mobs;
		Node.P_Error=perr;
		Node.A_Error=aerr;
		Node.LogLi=logli;
		Node.NSE=float(MISSING);
		m_pSimDoc->m_Val_Array[index]=Node;
		return true;
	}
	return false;
}
void ValidationData::SetValP_ErrorNew(size_t index,  float v)
{
	if(!m_pSimDoc->ValidationFilePointer()->m_Exist) return ;

	if(index<m_pSimDoc->m_Val_Array.size()) {
		m_pSimDoc->m_Val_Array[index].P_NewError =v;
	}
}
void ValidationData::SetValA_ErrorNew(size_t index,  float v)
{
	if(!m_pSimDoc->ValidationFilePointer()->m_Exist) return ;

	if(index<m_pSimDoc->m_Val_Array.size()) {
		m_pSimDoc->m_Val_Array[index].A_NewError =v;

	}
}
void ValidationData::SetValBoxCox_PowerNew(size_t index,  float v)
{
	if(!m_pSimDoc->ValidationFilePointer()->m_Exist) return ;

	if(index<m_pSimDoc->m_Val_Array.size()) {
		m_pSimDoc->m_Val_Array[index].NewBoxCox_Power =v;
	}
}
void ValidationData::SetValBoxCox_OffsetNew(size_t index,  float v)
{
	if(!m_pSimDoc->ValidationFilePointer()->m_Exist) return ;

	if(index<m_pSimDoc->m_Val_Array.size()) {
		m_pSimDoc->m_Val_Array[index].NewBoxCox_Offset =v;
	}
}
bool ValidationData::SetValNewLogLi(size_t index,  float per, float aer, float box1, float box2)
{
	if(!m_pSimDoc->ValidationFilePointer()->m_Exist) return false;

	if(index<m_pSimDoc->m_Val_Array.size()) {
		m_pSimDoc->m_Val_Array[index].P_NewError =per;
		m_pSimDoc->m_Val_Array[index].A_NewError =aer;
		m_pSimDoc->m_Val_Array[index].NewBoxCox_Power =box1;
		m_pSimDoc->m_Val_Array[index].NewBoxCox_Offset =box2;
		return true;
	}
	return false;
}
bool ValidationData::SetValCoef(size_t index, int n, float r2, float a0, float a1, float me, float rmse, float msim, float mobs, float perr, float aerr, double logli, float nse)
{
	if(!m_pSimDoc->ValidationFilePointer()->m_Exist) return false;

	if(index<m_pSimDoc->m_Val_Array.size()) {
		m_pSimDoc->m_Val_Array[index].nNumber=n;
		m_pSimDoc->m_Val_Array[index].R2     =r2;
		m_pSimDoc->m_Val_Array[index].A0     =a0;
		m_pSimDoc->m_Val_Array[index].A1     =a1;
		m_pSimDoc->m_Val_Array[index].ME     =me;
		m_pSimDoc->m_Val_Array[index].RMSE   =rmse;
		m_pSimDoc->m_Val_Array[index].MeanSim=msim;
		m_pSimDoc->m_Val_Array[index].MeanVal=mobs;
		m_pSimDoc->m_Val_Array[index].P_Error=perr;
		m_pSimDoc->m_Val_Array[index].A_Error=aerr;
		m_pSimDoc->m_Val_Array[index].LogLi=logli;
		m_pSimDoc->m_Val_Array[index].NSE=nse;
		return true;
	}
	return false;
}
bool ValidationData::SetValSumVarCoef(size_t index, float me, float rmse, float msim, float mobs, float perr, float aerr, double logli)
{

	if(index<m_pSimDoc->m_ValSum_Array.size()) {
		VALSUMv Node=m_pSimDoc->m_ValSum_Array[index];
		Node.ME.push_back(me);
		Node.RMSE.push_back(rmse);
		Node.SimValue=msim;
		Node.ObsValue=mobs;
		Node.P_Error=perr;
		Node.A_Error=aerr;
		Node.LogLi.push_back(logli);

		m_pSimDoc->m_ValSum_Array[index]=Node;

		return true;
	}
	return false;
}
bool ValidationData::SetValSumVarCoef(size_t index, float me, float rmse, double logli)
{

	if(index<m_pSimDoc->m_ValSum_Array.size()) {
		VALSUMv Node=m_pSimDoc->m_ValSum_Array[index];

		m_pSimDoc->m_ValSum_Array[index]=Node;
		return true;
	}
	return false;
}
bool ValidationData::RemoveValFile(size_t index_remove)
{
	F *pCF;
	int icount;
	icount=0;


	VALv vst;
	auto ikoll=m_pSimDoc->m_Val_Array.size();
	for(size_t i=0;i<m_pSimDoc->m_Val_Array.size();i++) {
		vst=m_pSimDoc->m_Val_Array[i];
		if(vst.ValidationFileNumber-1==index_remove) {	
			m_pSimDoc->m_Val_Array.erase(m_pSimDoc->m_Val_Array.begin()+i);
			i--;
		}
		else if(vst.ValidationFileNumber>index_remove) {
			vst.ValidationFileNumber--;
			m_pSimDoc->m_Val_Array[i]=vst;
		}
	};


	if(!m_pSimDoc->ValidationFilePointer()->m_Exist) return false;
	for(size_t ii=0;ii<MAXSIMVAL;ii++) {
			if(ii==index_remove) {
				//for(size_t i=0;i<ValidationFilePointer(ii)->GetNumberOfSelectedVal();i++){
				//	RemoveValNode(icount);			
				//}	

				m_pSimDoc->ValidationFilePointer(ii+1)->ResetValNumbers();
				pCF= m_pSimDoc->ValidationFilePointer(ii+1);

				pCF->SetEnable(false);
				string str="";
				pCF->SetValue(str);
				string grp="Validation";
				string name="ValidationMode";
				string type="Switches";

				Sw* pSw;
	
				pSw=(Sw*)m_pSimDoc->GetPtr(type, grp, name);
				auto inumvalid=pSw->GetIntValue();
				pSw->SetIntValue(inumvalid-1);
				//Sw->Apply();				
			}



			if(ii>=index_remove&&ii<MAXSIMVAL-1) {
				string FileName= m_pSimDoc->ValidationFilePointer(ii+1)->GetStrValue();
				m_pSimDoc->ValidationFilePointer(ii+1)->SetValue(FileName);
				int nval= m_pSimDoc->ValidationFilePointer(ii+1)->GetNumberOfSelectedVal();
				m_pSimDoc->ValidationFilePointer(ii+1)->SetNumberOfSelectedVal(nval);
				m_pSimDoc->ValidationFilePointer(ii+1)->SetNumberOfSelectedVal(0);
				
				m_pSimDoc->ValidationFilePointer(ii+1)->Apply();
				m_pSimDoc->ValidationFilePointer(ii+1)->Apply();
			}
	}	

	
	return true;
}
bool ValidationData::RemoveValNode(size_t index) // Index är från Alla valideringsvariabler
{
	F *pCF;
	if(!m_pSimDoc->ValidationFilePointer()->m_Exist) return false;

	VALv Node;
	if(index<m_pSimDoc->m_Val_Array.size()) {
		string Type;
		SimB* Sw;


		int localindex;
		int outtype;
		outtype=GetValNameType(index);
		string name,  group;
		name=GetValName(index);
		group=GetValGroup(index);
		localindex=GetValLocalIndex(index);


		switch (outtype) {
			case 1:
				Type="State Variables";
				break;
			case 2:
		 		Type="Flow Variables";
		 		break;
			case 3:
				Type="Auxiliary Variables";
	 			break;
			case 4:
		 		Type="Driving Variables";
	 			break;
		}
		Sw=(SimB*)m_pSimDoc->GetPtr(Type,group, name);
		if(Sw!=nullptr){
			 if (outtype>2)
				((G*)Sw)->SetValidFlagIndex(localindex, -1);
			else 
				((X*)Sw)->SetValidFlagIndex(localindex, -1);
		}
		Node=m_pSimDoc->m_Val_Array[index];
		pCF= m_pSimDoc->ValidationFilePointer(Node.ValidationFileNumber);
		m_NumberSelectedValidationVar[Node.ValidationFileNumber-1]--;
		pCF->SetNumberOfSelectedVal(pCF->GetNumberOfSelectedVal()-1);
		int valFileIndex;
		valFileIndex=GetValFileIndex(index);
		pCF->SetValNumber(valFileIndex,-1);
		auto ii=m_pSimDoc->m_Val_Array.begin();
		ii+=index,
		m_pSimDoc->m_Val_Array.erase(ii);
		return true;
	}
	return false;
}
bool ValidationData::RemoveValSumVarNode(size_t index)
{

	if(index<m_pSimDoc->m_ValSum_Array.size()) {
		m_pSimDoc->m_ValSum_Array.erase(m_pSimDoc->m_ValSum_Array.begin()+index);
		return true;
	}
	return false;
}
int ValidationData::GetValFileIndex(size_t index, size_t index_Dim1)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].ValidationFileIndex;
	}
	return -1;
}
int ValidationData::GetValFileResultIndex(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].ValidationFileResultIndex;
	}
	return -1;
}
bool ValidationData::SetValFileIndex(size_t index, int localindex)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		m_pSimDoc->m_Val_Array[index].ValidationFileIndex=localindex;
		return true;
	}
	return false;
}
int ValidationData::GetValFileNumber(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].ValidationFileNumber;
	}
	return -1;
}
bool ValidationData::SetValFileLogTrans(size_t index, bool value)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		m_pSimDoc->m_Val_Array[index].LogTrans=value;
		return true;
	}
	return false;
}
bool ValidationData::GetValFileLogTrans(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		m_pSimDoc->m_Val_Array[index].LogTrans;
	}
	return false;
}
bool ValidationData::SetValFileNumber(size_t index, int localindex)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		m_pSimDoc->m_Val_Array[index].ValidationFileNumber=localindex;
		return true;
	}
	return false;
}
bool ValidationData::SetValDuration(size_t index, int dur)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		m_pSimDoc->m_Val_Array[index].Duration=dur;
		return true;
	}
	return false;
}
float ValidationData::GetValSumVarP_Error(size_t index)
{

		if(index<m_pSimDoc->m_ValSum_Array.size()) {
		return m_pSimDoc->m_ValSum_Array[index].P_Error;
	}
	return MISSING;

}
float ValidationData::GetValSumVarA_Error(size_t index)
{

	
	if(index<m_pSimDoc->m_ValSum_Array.size()) {
		return m_pSimDoc->m_ValSum_Array[index].A_Error;
	}
	return MISSING;

}
float ValidationData::GetValSumVarMVAL(size_t index)
{

		if(index<m_pSimDoc->m_ValSum_Array.size()) {
		return m_pSimDoc->m_ValSum_Array[index].ObsValue;
	}
	return MISSING;

}
string ValidationData::GetValSumVarGroup(size_t index)
{

	if(index<m_pSimDoc->m_ValSum_Array.size()) {
		return m_pSimDoc->m_ValSum_Array[index].Group;
	}
	return "-";
}
string ValidationData::GetValSumVarName(size_t index)
{

		if(index<m_pSimDoc->m_ValSum_Array.size()) {
		return m_pSimDoc->m_ValSum_Array[index].Name;
	}
	return "-";
}
int ValidationData::GetValSumVarNameType(size_t index)
{

		if(index<m_pSimDoc->m_ValSum_Array.size()) {
		return m_pSimDoc->m_ValSum_Array[index].OutputType;
	}
	return -1;
}
int ValidationData::GetValSumVarOutputType(size_t index)
{

		if(index<m_pSimDoc->m_ValSum_Array.size()) {
		return m_pSimDoc->m_ValSum_Array[index].SumVarType;
	}
	return -1;
}
string ValidationData::GetStringValSumVarOutputType(size_t index)
{
	int ityp;
	string test;

	if(index<m_pSimDoc->m_ValSum_Array.size()) {
		ityp=m_pSimDoc->m_ValSum_Array[index].SumVarType;
	}
	else 
		ityp=-1;
	switch (ityp) {
	case -1: test="-"; break;
	case 0: test="Initial"; break;
	case 1: test="Final"; break;
	case 2: test="Minimum"; break;
	case 3: test="Maximum"; break;
	case 4: test="Mean"; break;
	case 5: test="Accumulated"; break;
	}
	return test;
}
int ValidationData::GetValSumVarLocalIndex(size_t index)
{

		if(index<m_pSimDoc->m_ValSum_Array.size()) {
		return m_pSimDoc->m_ValSum_Array[index].LocalIndex;
	}
	return -1;
}
double ValidationData::GetValSumVarLogLi(size_t index, size_t irep)
{

		if(index<m_pSimDoc->m_ValSum_Array.size() && m_pSimDoc->m_ValSum_Array[index].LogLi.size()>irep) {
		return m_pSimDoc->m_ValSum_Array[index].LogLi[irep];
	}
	return MISSING;
}
float ValidationData::GetValSumVarME(size_t index, size_t irep)
{

	if(index<m_pSimDoc->m_ValSum_Array.size()&& m_pSimDoc->m_ValSum_Array[index].ME.size()>irep) {
		return m_pSimDoc->m_ValSum_Array[index].ME[irep];
	}
	return -1.1E38f;
}
float ValidationData::GetValSumVarRMSE(size_t index, size_t irep)
{

		if(index<m_pSimDoc->m_ValSum_Array.size() && m_pSimDoc->m_ValSum_Array[index].RMSE.size()>irep) {
		return m_pSimDoc->m_ValSum_Array[index].RMSE[irep];
	}
	return -1.1E38f;
}

string ValidationData::GetValGroup(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].Group;
	}
	return "-";
}
string ValidationData::GetValName(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].Name;
	}
	return "-";
}
int ValidationData::GetValNameType(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].OutputType;
	}
	return -1;
}
int ValidationData::GetValLocalIndex(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].LocalIndex;
	}
	return -1;
}

int ValidationData::GetValn(size_t index)
{
	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].nNumber;
	}
	return -1;

}
float ValidationData::GetValR2(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].R2;
	}
	return MISSING;

}
float ValidationData::GetValNSE(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].NSE;
	}
	return MISSING;

}
float ValidationData::GetValRMSE_O(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		VALv Node;
		float v= m_pSimDoc->m_Val_Array[index].RMSE;
		Node=m_pSimDoc->m_Val_Array[index];
		if(v!=float(MISSING))
			return (v*v*Node.nNumber)/(1-(Node.NSE));
	}
	return MISSING;

}
float ValidationData::GetValA0(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].A0;
	}
	return MISSING;

}
float ValidationData::GetValA1(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].A1;
	}
	return MISSING;

}
float ValidationData::GetValME(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].ME;
	}
	return MISSING;

}
float ValidationData::GetValRMSE(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].RMSE;
	}
	return float(MISSING);

}
float ValidationData::GetValMSIM(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].MeanSim;
	}
	return MISSING;

}
float ValidationData::GetValMVAL(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].MeanVal;
	}
	return MISSING;

}
float ValidationData::GetValP_Error(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].P_Error;
	}
	return MISSING;

}
float ValidationData::GetValA_Error(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].A_Error;
	}
	return MISSING;

}
float ValidationData::GetValP_ErrorNew(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].P_NewError;
	}
	return MISSING;

}
float ValidationData::GetValA_ErrorNew(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].A_NewError;
	}
	return MISSING;

}
float ValidationData::GetValBoxCox_PowerNew(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].NewBoxCox_Power;
	}
	return MISSING;

}
float ValidationData::GetValBoxCox_OffsetNew(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].NewBoxCox_Offset;
	}
	return MISSING;

}

int ValidationData::GetValDuration(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].Duration;
	}
	return 0;

}
bool ValidationData::GetVal_AccTestValue(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].AccTest;
	}
	return false;

}
double ValidationData::GetValLogLi(size_t index)
{

	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].LogLi;
	}
	return MISSING;

}
void ValidationData::ClearSumVarList(){


	m_pSimDoc->m_ValSum_Array.clear();

}
bool ValidationData::CleanValList()
{

	m_pSimDoc->m_Val_Array.clear();
	m_pSimDoc->CleanValFlags();
	return true;
}
/*extern "C" void   VALIDATION_mp_GETVALINDEX();
extern "C" void  VALIDATION_mp_GETVALERRORS();
extern "C" long  VALIDATION_mp_IOBS_INDEX;
extern "C" long  VALIDATION_mp_IVAL_INDEX;
extern "C" long  VALIDATION_mp_NREG;
extern "C" float VALIDATION_mp_RELERROR;
extern "C" float VALIDATION_mp_ABSERROR;
extern "C" bool VALIDATION_mp_ACCTESTVALUE;
extern "C" long VALIDATION_mp_INDEXVAL;
extern "C" long VALIDATION_mp_INDEXSUM;
extern "C" int VALIDATION_mp_NPSUM;
extern "C" float VALIDATION_mp_A_VALUE_SIM;
extern "C" float VALIDATION_mp_A_VALUE_OBS;
extern "C" double VALIDATION_mp_EXTRA_LIKELIHOOD;
extern "C" void VALIDATION_mp_GETSUMVALUES();

extern "C" char	PARAMETERS_mp_GROUPNAME[50];
extern "C" char	PARAMETERS_mp_VARNAME[50];
extern "C" int	PARAMETERS_mp_NUMC_GROUPNAME;
extern "C" int	PARAMETERS_mp_NUMC_VARNAME;
extern "C" int	PARAMETERS_mp_VALID_VARINDEX;
extern "C" int	PARAMETERS_mp_VALID_VARTYPE;
extern "C" int	PARAMETERS_mp_VALID_ACCTYPE;
extern "C" int	PARAMETERS_mp_VALID_FILENUMBER;
extern "C" int	PARAMETERS_mp_VALID_LOGTRANS;
extern "C" int	PARAMETERS_mp_VALID_DURATION;
extern "C" int	PARAMETERS_mp_VALID_INDEX_VALFILE;
extern "C" float PARAMETERS_mp_VALID_REL_ERROR;
extern "C" float PARAMETERS_mp_VALID_ABS_ERROR;

extern "C" int	PARAMETERS_mp_NUMVALIDVAR;
extern "C" void	STAGE_mp_VALIDATION_INIT();*/



void ValidationData::SetNumberSelectedTSV_Val()
{
	m_TotalNumberSelected=GetNumberSelectedTSV_Val();
	m_look=true;

}
size_t ValidationData::GetNumberSelectedTSV_Val()
{
	if(m_look) 
		return m_TotalNumberSelected;
	size_t count=0;
	if(m_pSimDoc->ValidationFilePointer(1)==nullptr) return 0;
	for(int ii=0;ii<MAXSIMVAL;ii++) {
		count+=m_pSimDoc->ValidationFilePointer(ii+1)->GetNumberOfSelectedVal();
	}
	size_t count2=m_pSimDoc->GetNumAll_TSV_ValVariables();
	if(count<count2)
		return count2;
	else
		return count;
}

/*
float ValidationData::GetValSumVarMSIM(size_t index)
{
			string Group,Name, Type;
			int	iType, localindex, iOutput, SelectedLocalIndex, ii;
			float Value;
			SimB *Sw;


			Value=MISSING;
			
			Group=GetValSumVarGroup(index);
			Name=GetValSumVarName(index);
			localindex=GetValSumVarLocalIndex(index);
			iType=GetValSumVarNameType(index);
			iOutput=GetValSumVarOutputType(index);
	   		switch (iType) {
				case 1:
					Type="State Variables";
					break;
				case 2:
					Type="Flow Variables";
					break;
				case 3:
					Type="Auxiliary Variables";
					break;
				case 4:
					Type="Driving Variables";
					break;
				}
			Sw=(SimB*)m_pSimDoc->GetPtr(Type, Group,Name);
			if(Sw==nullptr) return -1.1e38f;

		  if(iType>2){
			  int num;
			  num=((G*)Sw)->GetNumberOfSelectedFlags();
			  for(ii=0;ii<num;ii++) {

					SelectedLocalIndex=((G*)Sw)->GetSelectedLocalIndex(ii);
					if(localindex==SelectedLocalIndex||localindex==-1) {
						switch(iOutput) {
							case 0:
							Value=((G*)Sw)->GetInitial(ii); break;
							case 1:
							Value=((G*)Sw)->GetFinal(ii); break;
							case 2:
							Value=((G*)Sw)->GetMin(ii); break;
							case 3:
							Value=((G*)Sw)->GetMax(ii); break;
							case 4:
							Value=((G*)Sw)->GetMean(ii); break;
							case 5:
							Value=((G*)Sw)->GetAccumulated(ii); break;
						}
					}
			  };
		  }
		  else {
		 	  int num;
			  num=((X*)Sw)->GetNumberOfSelectedFlags();
			  for(ii=0;ii<num;ii++) {

					SelectedLocalIndex=((X*)Sw)->GetSelectedLocalIndex(ii);
					if(localindex==SelectedLocalIndex||localindex==-1) {
						switch(iOutput) {
							case 0:
							Value=((X*)Sw)->GetInitial(ii); break;
							case 1:
							Value=((X*)Sw)->GetFinal(ii); break;
							case 2:
							Value=((X*)Sw)->GetMin(ii); break;
							case 3:
							Value=((X*)Sw)->GetMax(ii); break;
							case 4:
							Value=((X*)Sw)->GetMean(ii); break;
							case 5:
							Value=((X*)Sw)->GetAccumulated(ii); break;
						}
					}
			  }
		  }
		return Value;
}*/

int ValidationData::GetNumSumVarVariables()
{

	if(m_pSimDoc==nullptr)
		return 0;
	else
		return m_pSimDoc->m_ValSum_Array.size();

}
bool ValidationData::UpdateValOutIndex()
{
	int count;
	count=0;
	int fileno;
	for(int i=0;i<MAXSIMVAL;i++)
		m_NumberSelectedValidationVar[i]=0;

	if(m_pSimDoc==nullptr) return false;
	VALv Node;
	size_t index=0;
	while(index<m_pSimDoc->m_Val_Array.size())
	{
		Node=m_pSimDoc->m_Val_Array[index];
		fileno=Node.ValidationFileNumber;
		m_NumberSelectedValidationVar[fileno-1]++;
		m_pSimDoc->m_Val_Array[index].ValidationFileResultIndex=m_NumberSelectedValidationVar[fileno-1];
		//m_pSimDoc->m_Val_Array[index]
/// What is the use of FileResultIndex ??  consider
		//m_pSimDoc->m_Val_Array[index]=Node;
		index++;
	}

	return true;
}
int ValidationData::GetValMonitoring(size_t index)
{


	if(index<m_pSimDoc->m_Val_Array.size()) {
		return m_pSimDoc->m_Val_Array[index].Monitoring;
	}
	return 0;

}

int ValidationData::GetSumVarIndex(int outtype, SimB *pBase, int TabIndex, int OutputType) {
	int Index=0;
	if(pBase->Is_Vector())
		Index=((OutVector*)pBase)->GetSumVarIndex(TabIndex, size_t(OutputType)+6);
	else 
		Index=((OutSingle*)pBase)->GetSumVarIndex(size_t(OutputType)+6);
	return Index;
}
void ValidationData::SetPointersToOutputValidationFiles() {

	for(size_t i=0;i<MAXSIMVAL;i++) {
		//if(ValidationFilePointer(i+1)->m_Exist) {
			F* pF=m_pSimDoc->ValidationFilePointer(i+1);
			string filename=m_pSimDoc->GetOutputFileName(i+1);
			if(FUtil::IsFileExisting(filename)) {
				CPG *pPG= new CPG;
				pPG->Open(filename, false);
				pF->SetPGResPointer(pPG);
				pPG->CloseFile();
				m_PGCreatedPGFiles.push_back(pPG);
			}
		//}
	}

}
SIMB ValidationData::GetSimPointer_TSV(size_t Index) {
	SIMB out;
	if(m_pSimDoc!=nullptr&&Index<m_pSimDoc->m_Val_Array.size()) {
		string name=m_pSimDoc->m_Val_Array[Index].Name;
		string group=m_pSimDoc->m_Val_Array[Index].Group;
		string typstr="";
		switch(m_pSimDoc->m_Val_Array[Index].OutputType) {
					case 1:	typstr="State Variables"; break;
					case 2:	typstr="Flow Variables"; break;
					case 3:	typstr="Auxiliary Variables"; break;
					case 4:	typstr="Driving Variables"; break;	
		}
		out.pBase=m_pSimDoc->GetPtr(typstr,group, name);
		out.TabIndex=m_pSimDoc->m_Val_Array[Index].LocalIndex;
	}
	else {
		out.pBase=nullptr;
		out.TabIndex=string::npos;
	}
	return out;
}
SIMB ValidationData::GetSimPointer_SV(size_t Index) {
	SIMB out;
	if(m_pSimDoc!=nullptr&&Index<m_pSimDoc->m_ValSum_Array.size()) {
		string name=m_pSimDoc->m_ValSum_Array[Index].Name;
		string group=m_pSimDoc->m_ValSum_Array[Index].Group;
		string typstr="";
		switch(m_pSimDoc->m_ValSum_Array[Index].OutputType) {
		case 1:;case 8:	typstr = "State Variables"; break;
		case 2:; case 9:	typstr = "Flow Variables"; break;
		case 3:; case 10:	typstr = "Auxiliary Variables"; break;
		case 4:; case 11:	typstr = "Driving Variables"; break;
		}
		out.pBase=m_pSimDoc->GetPtr(typstr,group, name);
		out.TabIndex=m_pSimDoc->m_ValSum_Array[Index].LocalIndex;
	}
	else {
		out.pBase=nullptr;
		out.TabIndex=string::npos;
	}
	return out;
}