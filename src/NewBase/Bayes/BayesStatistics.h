#pragma once
#include "../../std.h"
using namespace std;
class SimB;
class lot;
class BayesStatistics
{
public:
	BayesStatistics();
	~BayesStatistics();
	
protected:
	double GetBayes_PValue(size_t index);
	double GetBayes_PCandidateValue(size_t index);
	void SetStart_PValues(size_t index, double value);
	void SetNum_BayesPar(size_t num);
	void SetNum_BayesChain(size_t num) {Num_Chain=num;};
	void Set_BayesInitCalib(bool value) {CalibrationMode=value;m_totlogprevgood=m_totlogMap=-1.E38; m_NumImproved=m_BestRun=m_NumRuns=0;};
	void Set_TotLogLi(double value) {m_totlogli=value;};
	double Random_NormDist();
	void UpdateNewPValues();
	float GetBestRun() {return float(m_BestRun);};
	void SetNewPValues();
	bool IsRunAccepted() {return AcceptedRun;};

	void Bayes_Check();
	void SetAccepted(bool value);
	vector<double> Candidate_PValues, PValues;
	bool First,  CalibrationMode;
	double mc_step;
	size_t Num_BayesPar;
	size_t Num_Chain;
	size_t Num_Burn;


private:
	bool BayesAccept();
	double m_totlogli;
	double m_totlogprevgood;
	double m_totlogMap;
	size_t m_NumImproved;
	size_t m_NumRuns, m_BestRun;
	bool AcceptedRun;

	lot *m_plot;

};