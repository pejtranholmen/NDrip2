
#include "ModelInfo.h"

ModelInfo::ModelInfo() 
{
	 StartStateValue=false;
	 FIRST=false;
	 Run_Res_Id=true;
	 ActualNoMulRun=0;
}
ModelInfo::~ModelInfo() 
{



}
void ModelInfo::AddModel(string name, void* ptr)
{
	ptrModule.insert(pair<string, void*>(name, ptr));
}
void* ModelInfo::GetModelPtr(string name) 
{

	map<string, void*>::iterator it;
	it=ptrModule.find(name);
	if(it!=ptrModule.end())
		return it->second;
	else
		return nullptr;

}
void ModelInfo::SetAbioticTimeSteps(size_t TimeResolutionOption)
{
	if (TimeResolutionOption == 0) {
		T_Step_NC = 1. / 144.;
		T_StepSnow = T_Step_NC;
		T_Shift = 0.;
	}
	else if (TimeResolutionOption == 1) {
		T_Step_NC = 1.0;
		T_StepSnow = T_Step_NC;
		T_Shift = 0.5;
	}
	else if (TimeResolutionOption == 2) {
		T_Step_NC = 1. / 24.;
		T_StepSnow = T_Step_NC;
		T_Shift = 1. / 48;
	}
	else if (TimeResolutionOption == 3) {
		T_Step_NC = 1. / 144.;
		T_StepSnow = T_Step_NC;
		T_Shift = 1. / 288;
	}
}

double ModelInfo::GetRemainingTime(bool Multi)
{
	if(Multi) 
		return double(m_UsedTime)/CLOCKS_PER_SEC/(RelRunTimeMulti+RelRunTimeSingle/TotalNoMulRun)*(1-RelRunTimeMulti-RelRunTimeSingle/TotalNoMulRun);
	else
		return double(m_UsedTime)/CLOCKS_PER_SEC/RelRunTimeSingle*(1-RelRunTimeSingle);
}
string ModelInfo::GetRemainingTimeString(bool Multi)
{
	if(RelRunTimeSingle<=0.0001) return "";
	double rem=GetRemainingTime(Multi);
	int iday, ihour, imin, isec, imsec;

	iday=int(rem/86400.);
	ihour=int((rem-double(iday)*86400.)/3600.);
	imin=int((rem-double(iday)*86400.-double(ihour)*3600.)/60.);
	isec=int(rem-double(iday)*86400.-double(ihour)*3600.-double(imin)*60.);
	imsec=int((rem-double(isec))*1000.);
	char buffer[180];
	size_t n;
	if(rem>86400.) 
		n=snprintf(buffer,60, "%d days %d hours %d minutes and %d sec left", iday, ihour, imin, isec);
	else if(rem>3600.) 
		n=snprintf(buffer,54, "%d hours %d minutes and %d sec left", ihour, imin, isec);
	else 
		n=snprintf(buffer,50, "%d minutes and %d sec left", imin, isec);

	string str;
	str.assign(&buffer[0], n );
	return str;

}
double ModelInfo::GetTimeUsed()
{
		return double(m_UsedTime)/CLOCKS_PER_SEC;
}
string ModelInfo::GetTimeUsedString()
{
	double rem=GetTimeUsed();
	int iday, ihour, imin, isec, imsec;

	iday=int(rem/86400.);
	ihour = int((rem - double(iday)*86400.) / 3600.);
	imin = int((rem - double(iday)*86400. - double(ihour)*3600.) / 60.);
	isec = int(rem - double(iday)*86400. - double(ihour)*3600. - double(imin)*60.);
	imsec = int((rem - double(isec))*1000.);
	char buffer[180];
	size_t n;
	if(rem>86400.) 
		n=snprintf(buffer,80, "Time used %d days %d hours %d minutes %d sec and %d millisec", iday, ihour, imin, isec, imsec);
	else if(rem>3600.) 
		n=snprintf(buffer,74, "Time used %d hours %d minutes %d sec and %d millisec", ihour, imin, isec, imsec);
	else if(rem>60.)
		n=snprintf(buffer,60, "Time used  %d minutes %d sec and %d millisec",imin, isec, imsec);
	else 
		n=snprintf(buffer,50, "Time used %d sec and %d millisec", isec, imsec);
	string str;
	str.assign(&buffer[0], n );
	return str;
}
