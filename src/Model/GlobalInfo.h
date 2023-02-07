#pragma once

#include "../std.h"


class GlobalInfo {
	public:
	GlobalInfo();
	~GlobalInfo();
	void AddModule(std::string, void*);
	void* GetModulePtr(std::string);
	

	 bool StartStateValue;
	 std::map<std::string, void*> ptrModule;







};
