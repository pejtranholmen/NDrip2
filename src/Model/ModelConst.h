#pragma once
#include <stddef.h>
struct OUTPUT_SUMMARY_RESULTS
{
	double Initial;
	double Min;
	double Max;
	double Acc;
	double AccTotal;
	bool YBin;
	bool NC_Type;
};
struct VALID_TEST_RESULTS
{	size_t count;
	size_t duration;
	size_t nc_shift;
	double acc_sim;
	size_t acc_sim_count;
	double acc_sim_prior;
	size_t acc_sim_prior_count;
	bool hitdone;
	bool delayed_hit;
	bool postmean;
	bool lastrecord;
	bool lasthit;
	bool firsthit;
	size_t nextrecord;
	unsigned int minute_post;
	unsigned int minute_post2;
	unsigned int minute_prior;
	double value_post;
	double value_prior;
};


class ModelConst {
	public:
	ModelConst();
	~ModelConst();
	double SecPerDay;
	double JDayNum, DayLength;
	int JYearNum;

	protected:

	

};
