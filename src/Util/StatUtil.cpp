#include "../std.h"
#include "StatUtil.h"
#include <sys/stat.h>

#include "../codecogs/stats/dists/continuous/StdNormal/cdf_inv.h"
#include "../codecogs/stats/dists/continuous/Normal/cdf_inv.h"
#include "../codecogs/Array/heap_sort.h"
#include "../codecogs/stats/moments/correlation.h"
#include "../codecogs/stats/moments/covariance.h"
#include "../codecogs/computing/array/matrix.h"
#include "../codecogs/maths/approximation/regression/discrete.h"
#include "../codecogs/maths/approximation/regression/linear.h"

double StatUtil::AIC_Func(double SSE, double n_point, double p_point) {
	return n_point * log(SSE / n_point) + 2 * p_point + 2 * p_point * (p_point + 1) / (n_point - p_point - 1);
}
double StatUtil::BIC_Func(double SSE, double n_point, double p_point) {
	return n_point * log(SSE / n_point) +  p_point*log(n_point);
}



STAT_PERF StatUtil::Get_StatComp(vector<double> *psim, vector<double> *pobs) 
{
	STAT_PERF out;

	Maths::Regression::Linear lin(psim->size(), psim->data(), pobs->data());
	if(psim->size()>0) {						
		out.intercept=float(lin.getIntercept());
		out.slope=float(lin.getSlope());
		out.r2=float(pow(lin.getCoefficient(),2));
	
		double me_acc=0, me2_acc=0, sim_acc=0, obs_acc=0;
		double sim2_acc=0, mobs2_acc=0, mean_obs;
		for(size_t i=0; i<psim->size(); i++) {
			me_acc+=psim->at(i)-pobs->at(i);
			sim_acc+=psim->at(i);
			obs_acc+=pobs->at(i);
			me2_acc+=(psim->at(i)-pobs->at(i))*(psim->at(i)-pobs->at(i));
		}
		mean_obs=obs_acc/psim->size();
		for(size_t i=0; i<psim->size(); i++)
			mobs2_acc+=(pobs->at(i)-mean_obs)*(pobs->at(i)-mean_obs);
	
		out.nse=float(1-me2_acc/mobs2_acc);
		out.me=float(me_acc/psim->size());
		out.rmse=float(pow(me2_acc/psim->size(),0.5));
		out.n=psim->size();
		out.meanobs=float(mean_obs);
		out.meansim=float(sim_acc/psim->size());
		out.sse = me2_acc;
	}
	else {
		out.n=0;


	}
	return out;
}
STAT_PERF StatUtil::Get_StatComp_ObsIndep(vector<float> *psim, vector<float> *pobs)
{
	STAT_PERF out;

	vector<double> sim, obs;
	for (size_t i = 0; i < psim->size(); i++) {
		if (pobs->at(i) > 0.9*MISSING) {
			sim.push_back(double(psim->at(i)));
			obs.push_back(double(pobs->at(i)));
		}
	}

	Maths::Regression::Linear lin(sim.size(), obs.data(),sim.data());
	if (sim.size()>0) {
		out.intercept = float(lin.getIntercept());
		out.slope = float(lin.getSlope());
		out.r2 = float(pow(lin.getCoefficient(), 2));

		double me_acc = 0, me2_acc = 0, sim_acc = 0, obs_acc = 0;
		double sim2_acc = 0, mobs2_acc = 0, mean_obs;
		for (size_t i = 0; i<sim.size(); i++) {
			me_acc += sim.at(i) - obs.at(i);
			sim_acc += sim.at(i);
			obs_acc += obs.at(i);
			me2_acc += (sim.at(i) - obs.at(i))*(sim.at(i) - obs.at(i));
		}
		mean_obs = obs_acc / sim.size();
		for (size_t i = 0; i<sim.size(); i++)
			mobs2_acc += (obs.at(i) - mean_obs)*(obs.at(i) - mean_obs);

		out.nse = float(1 - me2_acc / mobs2_acc);
		out.me = float(me_acc / sim.size());
		out.rmse = float(pow(me2_acc / sim.size(), 0.5));
		out.n = sim.size();
		out.meanobs = float(mean_obs);
		out.meansim = float(sim_acc / sim.size());
	}
	else {
		out.n = 0;


	}
	return out;
}

void StatUtil::rand_init(size_t seed) 
{
	srand(seed);
}
double StatUtil::frand()
{
	return (double)rand()/(double)RAND_MAX;
}
double StatUtil::sum(vector<double> v) 
{
	double sum=0;
	for(auto it=v.begin();it!=v.end();it++)
		sum+=*it;
	return sum;
}
double StatUtil::sum(vector<double> *v)
{
	double sum = 0;
	for (auto it = v->begin(); it != v->end(); it++)
		sum += *it;
	return sum;
}
double StatUtil::maximum(vector<double> v) 
{
	double maxv=-1.1E38;
	for(auto it=v.begin();it!=v.end();it++)
		if(maxv<*it) maxv=*it;
	return maxv;
}
double StatUtil::minimum(vector<double> v) 
{
	double minv=1.1E38;
	for(auto it=v.begin();it!=v.end();it++)
		if(minv>*it) minv=*it;
	return minv;
}
double StatUtil::mean(vector<double> v) 
{
	double sum=0;
	for(auto it=v.begin();it!=v.end();it++)
		sum+=*it;
	if(v.size()>0)
		return sum/v.size();
	else
		return 0.;
}
float StatUtil::mean(vector<float> v) 
{
	double sum=0;
	for(auto it=v.begin();it!=v.end();it++)
		sum+=*it;
	if(v.size()>0)
		return float(sum)/v.size();
	else
		return 0.;
}

float StatUtil::Normal_Inv(double p, double mean, double sigma)
{
	double v=Stats::Dists::Continuous::Normal::CDF_inv(p,mean,sigma);
	v=Stats::Moments::mean(5, &p);
	return float(v);
}
double StatUtil::Normal_InvD(double p, double mean, double sigma)
{
	return Stats::Dists::Continuous::Normal::CDF_inv(p,mean,sigma);
}
void StatUtil::SortArray(float *p, size_t n)
{
  Array::Sort::heapSort(p, n);
}
void StatUtil::SortDArray(double *p, size_t n)
{
   Array::Sort::heapSort(p, n);
 }
double StatUtil::DistanceFunc(double lat1,double lat2, double long1, double long2) {
        double PI = 4.0*atan(1.0);
        
        //main code inside the class
        double dlat1=lat1*(PI/180);

        double dlong1=long1*(PI/180);
        double dlat2=lat2*(PI/180);
        double dlong2=long2*(PI/180);

        double dLong=dlong1-dlong2;
        double dLat=dlat1-dlat2;

        double aHarv= pow(sin(dLat/2.0),2.0)+cos(dlat1)*cos(dlat2)*pow(sin(dLong/2),2);
        double cHarv=2*atan2(sqrt(aHarv),sqrt(1.0-aHarv));
        //earth"s radius from wikipedia varies between 6,356.750 km — 6,378.135 km (˜3,949.901 — 3,963.189 miles)
        //The IUGG value for the equatorial radius of the Earth is 6378.137 km (3963.19 mile)
        const double earth=6378.137;  //in kilometers to get distances in km
        double distance=earth*cHarv;
		return distance;

}
string StatUtil::STD_FtoAscii(float v)
{

	string str;
	char buf[20];
	snprintf(buf,20, "%05G",v);
	str = buf;
	return str;
}
string StatUtil::STD_DtoAscii(double v)
{

	string str;
	char buf[20];
	snprintf(buf,20, "%05G", v);
	str = buf;
	return str;
}
string StatUtil::STD_ItoAscii(int v)  {
	string str;
	char buf[20];
	snprintf(buf,20, "%d",v);
	str = buf;
	return str;
}
int	StatUtil::AtoInt(string str) {
	int i;
#ifdef MS_CODE
	sscanf_s(str.data(), "%d", &i);
#else
	sscanf(str.data(),"%d",&i);
#endif

	return i;

}
