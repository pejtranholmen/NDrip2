#include "./SimB.h"
class CRunInfo:public SimB {

public:
	CRunInfo();
	~CRunInfo();
	CRunInfo(int);
	string GetName();
	string GetGroup();
protected:
	string m_Group;
	string m_Name;
	int m_Format; // 0=Int, 1=Date, 2=String
	
};
