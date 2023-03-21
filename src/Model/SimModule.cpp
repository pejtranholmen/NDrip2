
#include "SimModule.h"
#include "../NewBase/ModelMap.h"
#include "../ModelTypes/NewModelType.h"
#include "./PLANT/WaterUptake.h"
#include "../PG/PGUtil.h"
#ifndef STD
#include "../stdafx.h"
#include "../MainFrm.h"
#endif
#include "./FunctionProperties.h"

FunctionProperties fproperties;
FunctionProperties* pFunc = &fproperties;

SimModule::SimModule()
{
	m_pModelInfo=nullptr;
	pSub=nullptr;
	m_pModelMap=nullptr;
	Ratio_ToGlobalTimeStep=1;
	Count_Call=0;
	m_NumErrors = 0;
	m_pModelStructure = nullptr;
	m_pg_util = new PGUtil();

	//pT_Step = &pModel->T_Step; 


}
SimModule::~SimModule()
{		

}
double SimModule::GetEcoDat(PGPOS *pos, ININFO f, size_t AdjustPosIvar) {	
	double var1, var2;
	if(AdjustPosIvar>0) f.ivar+=AdjustPosIvar;
	if (!f.pgfile->IsGood()) {
		int i = 0;

	}
	if(!f.discrete||f.shift==0)
		*pos=f.pgfile->GetPos(m_pModelInfo->m_ActMinRead+f.shift, f.ivar);
	else {
		*pos=f.pgfile->GetPos(m_pModelInfo->m_ActMinRead+f.shift, f.ivar);
		auto actpos=f.pgfile->GetHitPos(f.ivar, m_pModelInfo->m_ActMinRead,m_pModelInfo->m_ActMinRead+2*f.shift);
		float  fvalue=MISSING;
		if(actpos>0)
			fvalue= f.pgfile->GetVarValue(f.ivar,actpos);
		else
			return 0.;

		if (fvalue != MISSING&&fvalue > 0)
			return double(fvalue);
		else
			return 0.;

		/*if(pos->first==2||pos->first==3) {
			var2=0.;

		}
		if(pos->second>0&&pos->weight<0.99999)
			var2=f.pgfile->GetVarValue(f.ivar,pos->first);
		else
			return 0.;
		if(var2<=(MISSING*0.9))
			return 0.;
		else
			return var2;*/
	}

	var1=f.pgfile->GetVarValue(f.ivar,pos->first);
	if(pos->shift&&pos->weight>0&&!f.discrete&&var1>0.9*MISSING) {
		    var2=f.pgfile->GetVarValue(f.ivar,pos->second);
			if(var2>0.9*MISSING)
				return var1*(1-pos->weight)+pos->weight*var2;
			else
				return var1;
	}
	else {
		if (var1 >= .9 * MISSING && pos->weight > 0)
			return var1;
		else if (pos->weight < -720.)
			return 0.;
		else {
			if(pos->second==0)
				var2 = f.pgfile->GetVarValue(f.ivar, 1);
			else
				var2=f.pgfile->GetVarValue(f.ivar,pos->second);
			if(var2>0.9*MISSING)
				return var2;
			else
				return f.pgfile->GetLastValid(f.ivar);
		}
	}
		
}
vector<double> SimModule::GetEcoDat(PGPOS *pos, size_t NVar, ININFO f) {	
	if(!f.discrete)
		*pos=f.pgfile->GetPos(m_pModelInfo->m_ActMinRead+f.shift, f.ivar);
	else
		*pos=f.pgfile->GetPos(m_pModelInfo->m_ActMinRead+f.shift);

	double var1, var2;
	vector<double> out;
	for(size_t i=0; i<NVar; i++ ) {
		var1=f.pgfile->GetVarValue(f.ivar+i,pos->first);
		if(pos->shift&&pos->weight>0) {
			var2=f.pgfile->GetVarValue(f.ivar+i,pos->second);
			var1=var1*(1-pos->weight)+pos->weight*var2;
		}
		out.push_back(var1);
	}
	return out;
}
void SimModule::Message(string str) {

	size_t v=str.size();
#ifndef COUPSTD
	if(m_pModelMap!=nullptr) v=((Sim*)(m_pModelMap))->AddToRunReport(str);
#endif

}
int SimModule::Error()
{
	m_NumErrors++;
#ifdef COUPSTD
	if (m_NumErrors < 6) 
		cout << "Simulation Error "+FUtil::STD_ItoAscii(m_NumErrors);
	
#else
	MFC_Util m;
	m.SetPaneMessage("Error");		
	if (m_NumErrors < 6) {

		MFC_Util::MessageBoxReturn("Numerical Error during update of state variable\nThe flux value was not considered\nCheck your results carefully\nOnly 5 messages will be displayed but more errors may have been", "Numerical", MB_OK);

	}
#endif
	return m_NumErrors;

}
void SimModule::Define(F* ptr, string name, string filename, string conditions)
{
   string AllConditions;
   AllConditions=ModuleConditions;
   if(conditions.size()>0) {
	   AllConditions.append("|");
	   AllConditions.append(conditions);
   }
   m_pModelMap->GlobalMap.insert(pair<string,F*>(name,ptr));
   m_pModelMap->MapF[ModuleNo].insert(pair<string,F*>(name,ptr));
   ptr->Def(ModuleNo, name, filename, AllConditions);



}
void SimModule::Define(FCSV* ptr, string name, string filename, string conditions, size_t NumHeaders)
{
	string AllConditions;
	AllConditions = ModuleConditions;
	if (conditions.size()>0) {
		AllConditions.append("|");
		AllConditions.append(conditions);
	}
	m_pModelMap->GlobalMap.insert(pair<string, FCSV*>(name, ptr));
	m_pModelMap->MapCSV.insert(pair<string, FCSV*>(name, ptr));
	ptr->Def(ModuleNo, name, filename, AllConditions, NumHeaders);



}
void SimModule::Define(Sw* ptr, string name, int value,string options, string conditions, int reCalc)
{
   string AllConditions;
   AllConditions=ModuleConditions;
   if(AllConditions.size()>0) {
	   if (conditions.size() > 0) {
		   AllConditions.append("|");
		   AllConditions.append(conditions);
	   }
   }
   else {
	   AllConditions = conditions;
   }
   m_pModelMap->GlobalMap.insert(pair<string,Sw*>(name,ptr));
   m_pModelMap->MapSw[ModuleNo].insert(pair<string,Sw*>(name,ptr));

   ptr->Def(ModuleNo,name, value,options, AllConditions, reCalc);
}
void SimModule::Define(Xs* ptr, string name, double value, UNIT_TYPES UNIT, string conditions)
{
	ptr->SetUnitType(UNIT);
	Define(ptr, name, value, p_ModelInfo->GetUnitString(UNIT), conditions);
}
void SimModule::Define(Xs* ptr,  string name, double value, string unit, string conditions)
{
	if (ptr->GetUnitType() == UNIT_TYPES::NO_UNIT)
		ptr->SetUnitType(m_pg_util->GetUnitType(unit));
   string AllConditions;
   AllConditions=ModuleConditions;
   if(conditions.size()>0) {
	   AllConditions.append("|");
	   AllConditions.append(conditions);
   }
   m_pModelMap->GlobalMap.insert(pair<string,Xs*>(name,ptr));
   m_pModelMap->MapXs[ModuleNo].insert(pair<string,Xs*>(name,ptr));
   ptr->Def(ModuleNo, name, value, unit,AllConditions);
}
void SimModule::Define(Ts* ptr, string name, double value, UNIT_TYPES UNIT, string conditions)
{
	ptr->SetUnitType(UNIT);
	Define(ptr, name, value, p_ModelInfo->GetUnitString(UNIT), conditions);
}
void SimModule::Define(Ts* ptr,  string name, double value, string unit, string conditions)
{
	if (ptr->GetUnitType() == UNIT_TYPES::NO_UNIT)
		ptr->SetUnitType(m_pg_util->GetUnitType(unit));
   string AllConditions;
   AllConditions=ModuleConditions;
   if(conditions.size()>0) {
	   AllConditions.append("|");
	   AllConditions.append(conditions);
   }
   m_pModelMap->GlobalMap.insert(pair<string,Ts*>(name,ptr));
   m_pModelMap->MapTs[ModuleNo].insert(pair<string,Ts*>(name,ptr));
   ptr->Def(ModuleNo, name, value, unit,AllConditions);
}
void SimModule::Define(Gs* ptr, string name, double value, UNIT_TYPES UNIT, string conditions)
{
	ptr->SetUnitType(UNIT);
	Define(ptr, name, value, p_ModelInfo->GetUnitString(UNIT), conditions);
}
void SimModule::Define(Gs* ptr,  string name, double value, string unit, string conditions)
{
	if (ptr->GetUnitType() == UNIT_TYPES::NO_UNIT)
		ptr->SetUnitType(m_pg_util->GetUnitType(unit));
   string AllConditions;
   AllConditions=ModuleConditions;
   if(conditions.size()>0) {
	   AllConditions.append("|");
	   AllConditions.append(conditions);
   }
   m_pModelMap->GlobalMap.insert(pair<string,Gs*>(name,ptr));
   m_pModelMap->MapGs[ModuleNo].insert(pair<string,Gs*>(name,ptr));
   ptr->Def(ModuleNo, name, value, unit,AllConditions);
}
void SimModule::Define(Ds* ptr, string name, double value, UNIT_TYPES UNIT, string conditions)
{
	ptr->SetUnitType(UNIT);
	Define(ptr, name, value, p_ModelInfo->GetUnitString(UNIT), conditions);
}
void SimModule::Define(Ds* ptr,  string name, double value, string unit, string conditions)
{
	if (ptr->GetUnitType() == UNIT_TYPES::NO_UNIT)
		ptr->SetUnitType(m_pg_util->GetUnitType(unit));
   string AllConditions;
   AllConditions=ModuleConditions;
   if(conditions.size()>0) {
	   AllConditions.append("|");
	   AllConditions.append(conditions);
   }
   m_pModelMap->GlobalMap.insert(pair<string,Ds*>(name,ptr));
   m_pModelMap->MapDs[ModuleNo].insert(pair<string,Ds*>(name,ptr));
   ptr->Def(ModuleNo, name, value, unit,AllConditions);
} 
void SimModule::Define(Ps* ptr, string name, double value, UNIT_TYPES UNIT, string conditions, double emin, double emax, double wmin, double wmax, int recalc, int UserLevel, int view)
{
	ptr->SetUnitType(UNIT);
	Define(ptr, name, value, p_ModelInfo->GetUnitString(UNIT), conditions, emin, emax, wmin, wmax, recalc, UserLevel, view);
	
}
void SimModule::Define(Ps* ptr, string name, double value, string unit, string conditions, double emin, double emax, double wmin, double wmax,int recalc, int UserLevel, int view)
{
	if (ptr->GetUnitType() == UNIT_TYPES::NO_UNIT)
		ptr->SetUnitType(m_pg_util->GetUnitType(unit));
   string AllConditions;
   AllConditions=ModuleConditions;
   if(conditions.size()>0) {
	   AllConditions.append("|");
	   AllConditions.append(conditions);
   }
  m_pModelMap->GlobalMap.insert(pair<string,Ps*>(name,ptr));
  m_pModelMap->MapP[ModuleNo].insert(pair<string,Ps*>(name,ptr));
 
   if(emax<=emin) {
		emax=emin+abs(emin/10.);
	}
   if(wmin==wmax) {
		wmin=emin;
		wmax=emax;
	}
   ptr->Def(ModuleNo, name, value, unit,AllConditions, emin, emax,  wmin, wmax, recalc, UserLevel);
}
void SimModule::Define(NE* ne, string name)
{
   m_pModelMap->NE_Map.insert(pair<string,NE*>(name,ne));
 //  m_pModelMap->GlobalMap.insert(pair<string,NE*>(name,ne));
   ne->Def(name);
}

void SimModule::Define(P* ptr, NE* ptrNE, string name, vector<double> vvalue, UNIT_TYPES UNIT, string conditions, double emin, double emax, double wmin, double wmax, int recalc, int views, int view)
{
	ptr->SetUnitType(UNIT);
	Define(ptr,  ptrNE, name,  vvalue,p_ModelInfo->GetUnitString(UNIT) , conditions,  emin,  emax,  wmin, wmax,  recalc, views,  view);
}

void SimModule::Define(P* ptr,NE* ptrNE,string name, vector<double> vvalue, string unit, string conditions, double emin, double emax, double wmin, double wmax, int recalc, int views, int view)
{
	if (ptr->GetUnitType() == UNIT_TYPES::NO_UNIT)
		ptr->SetUnitType(m_pg_util->GetUnitType(unit));

   string AllConditions;
   AllConditions=ModuleConditions;
   if(conditions.size()>0) {
	   AllConditions.append("|");
	   AllConditions.append(conditions);
   }
   m_pModelMap->GlobalMap.insert(pair<string,P*>(name,ptr));
   if(emax<=emin) {
		emax=emin+abs(emin/10.);
	}
   if(wmin==wmax) {
		wmin=emin;
		wmax=emax;
	}
   ptr->SetIsOriginalValue();
   ptr->Def(ModuleNo, name, vvalue, unit,AllConditions, emin, emax,  wmin, wmax, recalc,views);
   m_pModelMap->MapPt[ModuleNo].insert(pair<string,P*>(name,ptr));
   ptrNE->LinkTo(ptr);
}

void SimModule::Define(Tab* ptr,string name, size_t min_P2EnableTab)
{

   m_pModelMap->GlobalMap.insert(pair<string,Tab*>(name,ptr));
   ptr->Def(ModuleNo, name,min_P2EnableTab);
   m_pModelMap->MapTab[ModuleNo].insert(pair<string,Tab*>(name,ptr));
}


void SimModule::Define(Func* ptr, FunctorProp myf, SimModule *mClass, string name)
{
	m_pModelMap->GlobalMap.insert(pair<string, Func*>(name, ptr));
	ptr->Def(myf, mClass, name, pFunc, m_pModelMap);
	ptr->SetGroupNo(ModuleNo);
	auto ret = m_pModelMap->MapFunc[ModuleNo].insert(pair<string, Func*>(name, ptr));
	if (ret.second == false) {
		m_pModelMap->MapErrors.insert(pair<string, int>(name, int(ModuleNo)));
	}
}
void SimModule::Define(Func* ptr, FunctorProp myf,  SimModule *mClass, string name, PLOT_PARAM plotpar)
{
	m_pModelMap->GlobalMap.insert(pair<string, Func*>(name, ptr));

	ptr->Def(myf, mClass, name, pFunc, m_pModelMap);
	ptr->SetGroupNo(ModuleNo);
	auto ret = m_pModelMap->MapFunc[ModuleNo].insert(pair<string, Func*>(name, ptr));
	auto ret2 = m_pModelMap->MapMyFunc.insert(pair<MyFunc, Func*>(myf.FuncName, ptr));
	if (ret.second == false) {
		m_pModelMap->MapErrors.insert(pair<string, int>(name, int(ModuleNo)));
	}
}

void SimModule::Define(Func* ptr, FunctorProp myf, string name,  vector<Ps*> Ps_Vec)
{
	m_pModelMap->GlobalMap.insert(pair<string, Func*>(name, ptr));

	ptr->Def(myf, name,Ps_Vec, pFunc, m_pModelMap);
	ptr->SetGroupNo(ModuleNo);


	auto ret = m_pModelMap->MapFunc[ModuleNo].insert(pair<string, Func*>(name, ptr));
    if (myf.FuncName == MyFunc::NO_PARFUNC) m_pModelMap->MapPhysFunc.insert(pair<int, Func*>(int(myf.PhysFuncName), ptr));
	else m_pModelMap->MapMyFunc.insert(pair<enum  MyFunc, Func*>(myf.FuncName, ptr));
	if (ret.second == false) {
		m_pModelMap->MapErrors.insert(pair<string, int>(name, int(ModuleNo)));
	}
}
void SimModule::Define(Func* ptr, FunctorProp myf, string name, vector<Ps*> Ps_Vec, PLOT_PARAM plotpar)
{
	m_pModelMap->GlobalMap.insert(pair<string, Func*>(name, ptr));

	ptr->Def(myf, name, Ps_Vec, pFunc, m_pModelMap);
	ptr->SetPlotParam(plotpar);
	ptr->SetGroupNo(ModuleNo);


	auto ret = m_pModelMap->MapFunc[ModuleNo].insert(pair<string, Func*>(name, ptr));
	if (myf.FuncName == MyFunc::NO_PARFUNC) m_pModelMap->MapPhysFunc.insert(pair<int, Func*>(int(myf.PhysFuncName), ptr));
	else m_pModelMap->MapMyFunc.insert(pair<enum  MyFunc, Func*>(myf.FuncName, ptr));
	if (ret.second == false) {
		m_pModelMap->MapErrors.insert(pair<string, int>(name, int(ModuleNo)));
	}
}
void SimModule::Define(Func* ptr, FunctorProp myf, string name, vector<Ps*> Ps_Vec, vector<P*> P_Vec, PLOT_PARAM plotpar)
{
	m_pModelMap->GlobalMap.insert(pair<string, Func*>(name, ptr));


	ptr->Def(myf, name, Ps_Vec,P_Vec, pFunc, m_pModelMap);
	ptr->SetPlotParam(plotpar);
	ptr->SetGroupNo(ModuleNo);

	


	auto ret = m_pModelMap->MapFunc[ModuleNo].insert(pair<string, Func*>(name, ptr));
	if (myf.FuncName == MyFunc::NO_PARFUNC) m_pModelMap->MapPhysFunc.insert(pair<int, Func*>(int(myf.PhysFuncName), ptr));
	else m_pModelMap->MapMyFunc.insert(pair<enum  MyFunc, Func*>(myf.FuncName, ptr));
	if (ret.second == false) {
		m_pModelMap->MapErrors.insert(pair<string, int>(name, int(ModuleNo)));
	}
}

void SimModule::Define(Func* ptr, FunctorProp myf, string name, vector<P*> P_Vec)
{
	m_pModelMap->GlobalMap.insert(pair<string, Func*>(name, ptr));
	ptr->SetGroupNo(ModuleNo);

	ptr->Def(myf, name, P_Vec, pFunc, m_pModelMap);
	auto ret = m_pModelMap->MapFunc[ModuleNo].insert(pair<string, Func*>(name, ptr));
	if (myf.FuncName == MyFunc::NO_PARFUNC) m_pModelMap->MapPhysFunc.insert(pair<int, Func*>(int(myf.PhysFuncName), ptr));
	else m_pModelMap->MapMyFunc.insert(pair<enum  MyFunc, Func*>(myf.FuncName, ptr));
	if (ret.second == false) {
		m_pModelMap->MapErrors.insert(pair<string, int>(name, int(ModuleNo)));
	}
}
void SimModule::Define(Func* ptr, FunctorProp myf, string name, vector<P*> P_Vec,  PLOT_PARAM plotpar)
{
	m_pModelMap->GlobalMap.insert(pair<string, Func*>(name, ptr));

	ptr->Def(myf, name, P_Vec, pFunc, m_pModelMap);
	ptr->SetPlotParam(plotpar);
	ptr->SetGroupNo(ModuleNo);
	auto ret=m_pModelMap->MapFunc[ModuleNo].insert(pair<string, Func*>(name, ptr));
	if (myf.FuncName == MyFunc::NO_PARFUNC) m_pModelMap->MapPhysFunc.insert(pair<int, Func*>(int(myf.PhysFuncName), ptr));
	else m_pModelMap->MapMyFunc.insert(pair<enum  MyFunc, Func*>(myf.FuncName, ptr));
	if (ret.second == false) {
		m_pModelMap->MapErrors.insert(pair<string, int>(name,int(ModuleNo) ));
	}
}


/*void SimModule::Define(Func* ptr,double (WaterUptake_Functions::*fpoint)(double),WaterUptake_Functions *mClass, string name,  string conditions)
{
   string AllConditions;
   AllConditions=ModuleConditions;
   if(conditions.size()>0) {
	   AllConditions.append("|");
	   AllConditions.append(conditions);
   }
   m_pModelMap->GlobalMap.insert(pair<string,Func*>(name,ptr));
   ptr->Def(ModuleNo,fpoint, mClass, name,conditions);
   m_pModelMap->MapFunc[ModuleNo].insert(pair<string,Func*>(name,ptr));
}*/
void SimModule::Define(X* ptr, NE* ptrNE, string name, vector<double> vvalue, UNIT_TYPES UNIT, string conditions)
{
	ptr->SetUnitType(UNIT);
	
	Define(ptr, ptrNE, name, vvalue, p_ModelInfo->GetUnitString(UNIT), conditions);
}

void SimModule::Define(X* ptr, NE* ptrNE, string name, vector<double> vvalue, string unit, string conditions)
{
	if (ptr->GetUnitType() == UNIT_TYPES::NO_UNIT)
		ptr->SetUnitType(m_pg_util->GetUnitType(unit));
   string AllConditions;
   AllConditions=ModuleConditions;
   if(conditions.size()>0) {
	   AllConditions.append("|");
	   AllConditions.append(conditions);
   }
   m_pModelMap->MapX[ModuleNo].insert(pair<string,X*>(name,ptr));
   m_pModelMap->GlobalMap.insert(pair<string,X*>(name,ptr));
   ptr->Def(ModuleNo, name, vvalue, unit,AllConditions);
   ptrNE->LinkTo(ptr);
}
void SimModule::Define(T* ptr, NE* ptrNE, string name, vector<double> vvalue, UNIT_TYPES UNIT, string conditions)
{
	ptr->SetUnitType(UNIT);
	Define(ptr, ptrNE, name, vvalue, p_ModelInfo->GetUnitString(UNIT), conditions);
}
void SimModule::Define(T* ptr, NE* ptrNE, string name, vector<double> vvalue, string unit, string conditions)
{
	if (ptr->GetUnitType() == UNIT_TYPES::NO_UNIT)
		ptr->SetUnitType(m_pg_util->GetUnitType(unit));

   string AllConditions;
   AllConditions=ModuleConditions;
   if(conditions.size()>0) {
	   AllConditions.append("|");
	   AllConditions.append(conditions);
   }
   m_pModelMap->GlobalMap.insert(pair<string,T*>(name,ptr));
   m_pModelMap->MapT[ModuleNo].insert(pair<string,T*>(name,ptr));
 
   ptr->Def(ModuleNo, name, vvalue, unit,AllConditions);
   ptrNE->LinkTo(ptr);
}
void SimModule::Define(G* ptr, NE* ptrNE, string name, vector<double> vvalue, UNIT_TYPES UNIT, string conditions)
{
	ptr->SetUnitType(UNIT);
	Define(ptr, ptrNE, name, vvalue,p_ModelInfo->GetUnitString(UNIT), conditions);
}
void SimModule::Define(G* ptr, NE* ptrNE,string name, vector<double> vvalue, string unit, string conditions)
{
	if (ptr->GetUnitType() == UNIT_TYPES::NO_UNIT)
		ptr->SetUnitType(m_pg_util->GetUnitType(unit));

   string AllConditions;
   AllConditions=ModuleConditions;
   if(conditions.size()>0) {
	   AllConditions.append("|");
	   AllConditions.append(conditions);
   }
   m_pModelMap->GlobalMap.insert(pair<string,G*>(name,ptr));
   m_pModelMap->MapG[ModuleNo].insert(pair<string,G*>(name,ptr));
   ptr->Def(ModuleNo, name, vvalue, unit,AllConditions);
   ptrNE->LinkTo(ptr);
}
void SimModule::Define(D* ptr, NE* ptrNE, string name, vector<double> vvalue, UNIT_TYPES UNIT, string conditions)
{
	ptr->SetUnitType(UNIT);
	Define(ptr, ptrNE, name, vvalue, p_ModelInfo->GetUnitString(UNIT), conditions);
}
void SimModule::Define(D* ptr, NE* ptrNE, string name, vector<double> vvalue, string unit, string conditions)
{
	if (ptr->GetUnitType() == UNIT_TYPES::NO_UNIT)
		ptr->SetUnitType(m_pg_util->GetUnitType(unit));
   string AllConditions;
   AllConditions=ModuleConditions;
   if(conditions.size()>0) {
	   AllConditions.append("|");
	   AllConditions.append(conditions);
   }
   m_pModelMap->GlobalMap.insert(pair<string,D*>(name,ptr));
   m_pModelMap->MapD[ModuleNo].insert(pair<string,D*>(name,ptr));
   ptr->Def(ModuleNo, name, vvalue, unit,AllConditions);
   ptrNE->LinkTo(ptr);
}
function<double(bool, size_t)> SimModule::Get_boolBlank_TableFunctor(enum  MyFunc choice)
{

	auto it = m_pModelMap->MapMyFunc.find(choice);
	if (it != m_pModelMap->MapMyFunc.end()) {
		return (*it).second->Get_boolBlank_TableFunctor();
	}
	return nullptr;
}
function<double(bool, size_t, double, double, double)> SimModule::Get_boolD3_TableFunctor(enum  MyFunc choice)
{

	auto it = m_pModelMap->MapMyFunc.find(choice);
	if (it != m_pModelMap->MapMyFunc.end()) {
		return (*it).second->Get_boolD3_TableFunctor();
	}
	return nullptr;
}
function<double(bool, size_t, double, double, double, double)> SimModule::Get_boolD4_TableFunctor(enum  MyFunc choice)
{

	auto it = m_pModelMap->MapMyFunc.find(choice);
	if (it != m_pModelMap->MapMyFunc.end()) {
		return (*it).second->Get_boolD4_TableFunctor();
	}
	return nullptr;
}
function<double(size_t, double)> SimModule::Get_D1_TableFunctor(enum  MyFunc choice)
{

	auto it = m_pModelMap->MapMyFunc.find(choice);
	if (it != m_pModelMap->MapMyFunc.end()) {
		return (*it).second->Get_D1_TableFunctor();
	}
	return nullptr;
}
function<double(size_t, double, double)> SimModule::Get_D2_TableFunctor(enum  MyFunc choice)
{

	auto it = m_pModelMap->MapMyFunc.find(choice);
	if (it != m_pModelMap->MapMyFunc.end()) {
		return (*it).second->Get_D2_TableFunctor();
	}
	return nullptr;
}
function<double(size_t, double, double, double)> SimModule::Get_D3_TableFunctor(enum  MyFunc choice)
{

	auto it = m_pModelMap->MapMyFunc.find(choice);
	if (it != m_pModelMap->MapMyFunc.end()) {
		return (*it).second->Get_D3_TableFunctor();
	}
	return nullptr;
}
function<double(size_t, double, double, double, double)> SimModule::Get_D4_TableFunctor(enum  MyFunc choice)
{

	auto it = m_pModelMap->MapMyFunc.find(choice);
	if (it != m_pModelMap->MapMyFunc.end()) {
		return (*it).second->Get_D4_TableFunctor();
	}
	return nullptr;
}
function<double(bool)> SimModule::Get_boolBlank_Functor(enum  MyFunc choice)
{

	auto it = m_pModelMap->MapMyFunc.find(choice);
	if (it != m_pModelMap->MapMyFunc.end()) {
		return (*it).second->Get_boolBlank_Functor();
	}
	return nullptr;
}
function<vector<double>(bool)> SimModule::GetVector_boolBlank_Functor(enum  MyFunc choice)
{

	auto it = m_pModelMap->MapMyFunc.find(choice);
	if (it != m_pModelMap->MapMyFunc.end()) {
		return (*it).second->GetVector_boolBlank_Functor();
	}
	return nullptr;
}
function<vector<double>()> SimModule::GetVector_Blank_Functor(enum  MyFunc choice)
{

	auto it = m_pModelMap->MapMyFunc.find(choice);
	if (it != m_pModelMap->MapMyFunc.end()) {
		return (*it).second->GetVector_Blank_Functor();
	}
	return nullptr;
}
function<double(double)> SimModule::Get_D1_Functor(enum  MyFunc choice)
{

	auto it=m_pModelMap->MapMyFunc.find(choice);
	if (it != m_pModelMap->MapMyFunc.end()) {
		return (*it).second->Get_D1_Functor();
	}
	return nullptr;
}
function<double(double,double)> SimModule::Get_D2_Functor(enum  MyFunc choice)
{
	auto it = m_pModelMap->MapMyFunc.find(choice);
	if (it != m_pModelMap->MapMyFunc.end()) {
		return (*it).second->Get_D2_Functor();
	}
	return nullptr;
}
function<double(double,double,double)> SimModule::Get_D3_Functor(enum  MyFunc choice)
{

	auto it = m_pModelMap->MapMyFunc.find(choice);
	if (it != m_pModelMap->MapMyFunc.end()) {
		return (*it).second->Get_D3_Functor();
	}
	return nullptr;
}
function<double(double,double,double,double)> SimModule::Get_D4_Functor(enum  MyFunc choice)
{

	auto it = m_pModelMap->MapMyFunc.find(choice);
	if (it != m_pModelMap->MapMyFunc.end()) {
		return (*it).second->Get_D4_Functor();
	}
	return nullptr;
}
function<double(double, double, double, double, double)> SimModule::Get_D5_Functor(enum  MyFunc choice)
{

	auto it = m_pModelMap->MapMyFunc.find(choice);
	if (it != m_pModelMap->MapMyFunc.end()) {
		return (*it).second->Get_D5_Functor();
	}
	return nullptr;
}
function<double(double)> SimModule::Get_D1_PhysFunctor(enum  FUNC_CHOICE choice)
{
	auto it = m_pModelMap->MapPhysFunc.find(int(choice));
	if (it != m_pModelMap->MapPhysFunc.end()) {
		return (*it).second->Get_D1_Functor();
	}
	return nullptr;
}
function<double(double, double)> SimModule::Get_D2_PhysFunctor(enum   FUNC_CHOICE choice)
{

	auto it = m_pModelMap->MapPhysFunc.find(int(choice));
	if (it != m_pModelMap->MapPhysFunc.end()) {
		return (*it).second->Get_D2_Functor();
	}
	return nullptr;
}
function<double(double, double, double)> SimModule::Get_D3_PhysFunctor(enum   FUNC_CHOICE choice)
{

	auto it = m_pModelMap->MapPhysFunc.find(int(choice));
	if (it != m_pModelMap->MapPhysFunc.end()) {
		return (*it).second->Get_D3_Functor();
	}
	return nullptr;
}
function<double(double, double, double, double)> SimModule::Get_D4_PhysFunctor(enum   FUNC_CHOICE choice)
{

	auto it = m_pModelMap->MapPhysFunc.find(int(choice));
	if (it != m_pModelMap->MapPhysFunc.end()) {
		return (*it).second->Get_D4_Functor();
	}
	return nullptr;
}
function<double(double, double, double, double, double)> SimModule::Get_D5_PhysFunctor(enum   FUNC_CHOICE choice)
{
	auto it = m_pModelMap->MapPhysFunc.find(int(choice));
	if (it != m_pModelMap->MapPhysFunc.end()) {
		return (*it).second->Get_D5_Functor();
	}
	return nullptr;
}
function<void(vector<double>*)>  SimModule::GetVectorPointer_D1_Functor(enum  MyFunc choice) {
	auto it = m_pModelMap->MapMyFunc.find(choice);
	if (it != m_pModelMap->MapMyFunc.end()) {
		return (*it).second->GetVectorPointer_D1_Functor();
	}
	return nullptr;
}
function<void(vector<double>*, vector<double>*)>  SimModule::GetVectorPointer_D2_Functor(enum  MyFunc choice) {
	auto it = m_pModelMap->MapMyFunc.find(choice);
	if (it != m_pModelMap->MapMyFunc.end()) {
		return (*it).second->GetVectorPointer_D2_Functor();
	}
	return nullptr;
}
function<void(vector<double>*, vector<double>*, vector<double>*)>  SimModule::GetVectorPointer_D3_Functor(enum  MyFunc choice) {
	auto it = m_pModelMap->MapMyFunc.find(choice);
	if (it != m_pModelMap->MapMyFunc.end()) {
		return (*it).second->GetVectorPointer_D3_Functor();
	}
	return nullptr;
}
function<void(vector<double>*, vector<double>*, vector<double>*, vector<double>*)> SimModule::GetVectorPointer_D4_Functor(enum  MyFunc choice) {
	auto it = m_pModelMap->MapMyFunc.find(choice);
	if (it != m_pModelMap->MapMyFunc.end()) {
		return (*it).second->GetVectorPointer_D4_Functor();
	}
	return nullptr;
}
function<void(vector<double>*, vector<double>*, vector<double>*, vector<double>*, vector<double>*)> SimModule::GetVectorPointer_D5_Functor(enum  MyFunc choice) {
	auto it = m_pModelMap->MapMyFunc.find(choice);
	if (it != m_pModelMap->MapMyFunc.end()) {
		return (*it).second->GetVectorPointer_D5_Functor();
	}
	return nullptr;
}
function<OrgTurnOver(double, double, double)> SimModule::Get_OrgTurnOverFunctor(enum  MyFunc choice) {
	auto it = m_pModelMap->MapMyFunc.find(choice);
	if (it != m_pModelMap->MapMyFunc.end()) {
		return (*it).second->Get_OrgTurnOverFunctor();
	}
	return nullptr;
}
function<OrgTurnOver(double, double, double, double, double)> SimModule::Get_D5_OrgTurnOverFunctor(enum  MyFunc choice) {
	auto it = m_pModelMap->MapMyFunc.find(choice);
	if (it != m_pModelMap->MapMyFunc.end()) {
		return (*it).second->Get_D5_OrgTurnOverFunctor();
	}
	return nullptr;
}
