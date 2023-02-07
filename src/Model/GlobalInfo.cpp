#include "GlobalInfo.h"

GlobalInfo::GlobalInfo() 
{
	 StartStateValue=true;


}
GlobalInfo::~GlobalInfo() 
{



}
void GlobalInfo::AddModule(std::string name, void* ptr)
{
	ptrModule.insert(std::pair<std::string, void*>(name, ptr));
}
void* GlobalInfo::GetModulePtr(std::string name) 
{

	std::map<std::string, void*>::iterator it;
	it=ptrModule.find(name);
	if(it!=ptrModule.end())
		return it->second;
	else
		return nullptr;

}