#pragma once
#include "../CH.h"
class ChangeFromFile :public CH
{
public:
	ChangeFromFile();
	~ChangeFromFile();

	bool LinkedChangeToParameters(Par *pPar, size_t option, size_t index=string::npos);

	void Get_IGBP_Soil();
	void Get_Mascur_Soil();
	bool ReCalculateNewSoilProfile();
	size_t SaveSelectedParamtersToCSVFile(size_t options);
	bool ReadDataFromCSVFileandSetDataCorrespondingtoIndexValue(FCSV *pCSVFile, size_t keyindex);


};

