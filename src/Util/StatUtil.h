#pragma once
#include <string>
#include "FUtil.hpp"
#include <cmath>

struct STAT_PERF
{
	size_t n;
	float intercept;
	float slope;
	float r2;
	float meansim;
	float meanobs;
	float me;
	float rmse;
	float sse;
	float nse;
	float logli;
};

struct SimpleDistStat
{	float Mean;
	float Std;
	float Min;
	float Max;
	size_t NumData;
};


template <class T>void heapMean(T vals[], size_t n, double *sum, double* Mean, double* Std)
{

*sum=0;
 int ncount=0;


  for (size_t i=0; i<n; i++) {
	  if(abs(vals[i])<1.E37) {
		*sum+=vals[i];
		ncount++;
	  }
  }
  if(ncount<4) {
	*Mean=-1.1E38f;	  	
	return ;
  }
  
  *Mean=*sum/ncount;
  *sum=0;
  for (size_t i=0; i<n; i++) {
	  if(abs(vals[i])<1.E37) {
		*sum+=(vals[i]-*Mean)*(vals[i]-*Mean);
	  }
  }   
  *Std=sqrt(*sum/ (double(ncount) - 1.));

  
  return;
};
template <class T>void MeanStat(T vals[], size_t n, size_t  *ncheck, DistStat *Stat)
{
	float sum=0;
	int ncount=0;
	float weight;


  for (size_t i=0; i<n; i++) {
	  if(abs(vals[i])<1.E37) {
		sum+=vals[i];
		ncount++;
	  }
  }
  *ncheck=ncount;
  if(ncount>0){
	Stat->Mean=sum/ncount;
	Stat->Max=vals[n-1];
	Stat->Min=vals[n-ncount];
  }
  else {
	  Stat->Max=Stat->Min=Stat->Mean=Stat->Std=Stat->P5=Stat->P50=Stat->P95=float(-1.1E38f);
	  return;
  }

  if(ncount<4) { 	
	Stat->Std=float(-1.1E38f);
	Stat->P5=float(-1.1E38f);
	Stat->P50=float(-1.1E38f);
	Stat->P95=float(-1.1E38f);
	return;
  }
  
 
  sum=0;
  for (size_t i=0; i<n; i++) {
	  if(abs(vals[i])<1.E37) {
		sum+=(vals[i]-Stat->Mean)*(vals[i]-Stat->Mean);
	  }
  }   
  Stat->Std=sqrt(sum/(ncount-1));
 
   int low, high;
 
	if(ncount%2==1) {
		low=ncount/2;
		Stat->P50=float(vals[low]);
	}
	else {
		low=ncount/2-1;
		Stat->P50=float((vals[low]+vals[low+1])/2.);
	}

	if(ncount%21==0) {
		low=ncount/21;
		high=ncount-low-1;
		Stat->P5=float(vals[low]);
		Stat->P95=float(vals[high]);
	}
	else {
		low=ncount/21;
		weight=ncount%21/21.f;
		high=ncount-low-2;
		Stat->P5=float((1.-weight)*vals[low]+weight*vals[low+1]);
		Stat->P95=float(weight*vals[high]+(1.-weight)*vals[high+1]);
	}
	Stat->NumData=ncount;
	return;
};
template <class T>void SStat(T vals[], unsigned long n, size_t  *ncheck, SimpleDistStat *Stat)
{
	T sum=0;
	size_t ncount=0;
	T min1=1.1E38, max1=-1.1E38;

  for (size_t i=0; i<n; i++) {
	  if(abs(vals[i])<1.E37) {
		sum+=vals[i];
		ncount++;
		if(vals[i]>max1) max1=vals[i];
		if(vals[i]<min1) min1=vals[i];
	  }
  }
  *ncheck=ncount;
  if(ncount>0){
	Stat->Mean=float(sum/ncount);
	Stat->Max=float(max1);
	Stat->Min=float(min1);
  }
  else {
	  Stat->Max=Stat->Min=Stat->Mean=Stat->Std=float(-1.1E38f);
	  return;
  }
  sum=0;
  for (size_t i=0; i<n; i++) {
	  if(abs(vals[i])<1.E37) {
		sum+=(vals[i]-Stat->Mean)*(vals[i]-Stat->Mean);
	  }
  }   
  Stat->Std=float(sqrt(sum/(ncount-1)));
 
  Stat->NumData=ncount;
return;
};

namespace StatUtil
{
	STAT_PERF Get_StatComp(vector<double> *sim, vector<double> *obs);
	STAT_PERF Get_StatComp_ObsIndep(vector<float> *sim, vector<float> *obs);
	double AIC_Func(double SSE, double n_point, double p_point);
	double BIC_Func(double SSE, double n_point, double p_point);
	double sum(vector<double>);
	double sum(vector<double>*);
	double mean(vector<double>);
	float mean(vector<float>);
	double minimum(vector<double>);
	double maximum(vector<double>);

	float Normal_Inv(double p, double mean, double sigma);
	double Normal_InvD(double p, double mean, double sigma);
	void	SortArray(float *p, size_t n);
	void	SortDArray(double *p, size_t n);

	double DistanceFunc( double, double,double, double);

	void rand_init(size_t );
	double frand();

	string	STD_DtoAscii(double v);
	string	STD_FtoAscii(float v) ;
	string	STD_ItoAscii(int v) ;
	int AtoInt(string str);

};
using namespace StatUtil;


