#pragma once
#include "../std.h"
class Doc;
class CCoupModelDoc;
struct SimObject { bool Running; size_t DocType; CCoupModelDoc *pDoc; Doc *pSimDoc;};
class SimArchive
{
public:
	SimArchive();
	virtual ~SimArchive();
	std::vector <SimObject> m_SimDocArray;
	size_t GetNumSimDoc() {return m_SimDocArray.size();};
	void AddSimDoc(SimObject fil) {m_SimDocArray.push_back(fil);};
	void EraseSimDoc(size_t i) {m_SimDocArray.erase(m_SimDocArray.begin()+i);};
	
	Doc *GetSimDocPointer(size_t i=0);
	CCoupModelDoc *GetDocPointer(size_t i=0);

};
