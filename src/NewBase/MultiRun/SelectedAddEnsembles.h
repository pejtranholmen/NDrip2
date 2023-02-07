#pragma once



#include "MultiStorageStatData.h"


class SelectedAddEnsembles : public MultiStorageStatData
{
public:
	SelectedAddEnsembles();
	virtual ~SelectedAddEnsembles();
	bool SetNewPerformanceVector(size_t, vector<float> VarValidVector);
	bool AddNewEnsemble(bool KeepPreviousCriteria=false);
	bool ChangeEnsembleNames(size_t index, string newname);
	bool RemoveEnsemble(size_t index);

protected:



};


