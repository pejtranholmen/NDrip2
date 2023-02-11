#include "./SimB.h"
class CRunInfo:public SimB {

public:
	CRunInfo();
	~CRunInfo();
	CRunInfo(int);
	std::string GetName();
	std::string GetGroup();
protected:
	std::string m_Group;
	std::string m_Name;
	int m_Format; // 0=Int, 1=Date, 2=String
	
};
