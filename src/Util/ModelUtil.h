#pragma once
#include "../ModelTypes/NewModelType.h"
class Doc;
struct SIMB;

class ModelUtil

{
public:
	ModelUtil(void);
	~ModelUtil(void);
	static bool SetParTableValue(Doc *pDoc, P *pP, size_t index, double newvalue);
	static bool SetParValue(Doc*, Ps *pPs, double newvalue);
	static bool SetMRSoilPropConnections(Doc *pDlc);
	static bool SetMRSoilPropConnectionsProfNumberOnly(Doc *pDoc);

};
