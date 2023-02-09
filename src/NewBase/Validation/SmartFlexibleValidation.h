#pragma once
#include <vector>
#include "./SmartFlexDataAdmin.h"
class FCSV;


class SmartFlexibleValidation: public SmartFlexDataAdmin
{
public:
	SmartFlexibleValidation();
	~SmartFlexibleValidation();
	bool UpdateValidationPerformance_SmartFlexVariables();
	vector<float> GetObsValueFromValSumFlex(string IDFileName, string  IDVarName);

	size_t ImportSelectedParamtersFromCSVFile(FCSV *pCSVFile);

	bool CalculatePerformanceAndSetValues();
	bool CalculateMeanPerformanceAndSetValues();
	

	
	bool SetSimObsRatio();
	bool SetValSum(size_t index, VALSUMv valsum);

	bool m_IsPerformanceSmartFlexibleFile_Read;


private:
	double SumLikelihoodSingleValue(size_t i, double sim, double obsvalue = double(MISSING));


};

