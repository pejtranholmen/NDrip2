#pragma once
#include <string>
#include <map>

class GlobalInfo {
	public:
	GlobalInfo();
	~GlobalInfo();
	void AddModule(std::string, void*);
	void* GetModulePtr(std::string);
	

	 bool StartStateValue;
	 std::map<std::string, void*> ptrModule;







};
