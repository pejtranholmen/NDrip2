
#include "../mcmc/lot.h"
#include "./BayesStatistics.h"
BayesStatistics::BayesStatistics() {

	m_plot= new lot(lot::RAN_MT, lot::OPEN);
	Num_Burn=100;
}
BayesStatistics::~BayesStatistics() {
	delete m_plot;
}


double BayesStatistics::GetBayes_PValue(size_t index) {
	if(index<PValues.size())
		return PValues[index];
	else
		return -1.1E38;

}
double BayesStatistics::GetBayes_PCandidateValue(size_t index) {
	if(index<Candidate_PValues.size())
		return Candidate_PValues[index];
	else
		return -1.1E38;

}
void BayesStatistics::SetStart_PValues(size_t index, double value) {
		if(index<PValues.size())
		 PValues[index]=value;
}

void BayesStatistics::UpdateNewPValues() {

	for(size_t i=0; i<Candidate_PValues.size(); i++) {
		Candidate_PValues[i]=PValues[i]+Random_NormDist()*mc_step;
		if(Candidate_PValues[i]<0.) Candidate_PValues[i]=0.+abs(Candidate_PValues[i]);
		if(Candidate_PValues[i]>1.) Candidate_PValues[i]=1.-abs(Candidate_PValues[i]-1.);
	}
}
void BayesStatistics::SetNewPValues() {

	for(size_t i=0; i<Candidate_PValues.size(); i++) {
		PValues[i]=Candidate_PValues[i];
	}
}
double BayesStatistics::Random_NormDist()
{


	return m_plot->snorm();
}
void BayesStatistics::SetNum_BayesPar(size_t num) {
	Candidate_PValues.assign(num, 0.5);
	PValues.assign(num, 0.5);
	Num_BayesPar=num;
}

void BayesStatistics::Bayes_Check() {
	if(CalibrationMode) {

		AcceptedRun=BayesAccept();




	}
	else {






	}




}
void BayesStatistics::SetAccepted(bool accept) {
	AcceptedRun = accept;
}
bool BayesStatistics::BayesAccept() 
{
	m_NumRuns++;
	double randomv=(double)rand()/(double)RAND_MAX;
	double logalpha=m_totlogli-m_totlogprevgood;

	if(randomv<logalpha) {
		m_NumImproved++;
		SetNewPValues();
		m_totlogprevgood=m_totlogli;
		if(m_totlogli>m_totlogMap) {
			m_totlogMap=m_totlogli;
			m_BestRun=m_NumRuns;
		}
		return true;
	}
	else 
		return false;


}

