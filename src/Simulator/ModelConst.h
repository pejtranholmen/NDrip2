#pragma once
#include "../std.h"

struct OUT_SUM
{
	double Initial;
	double Min;
	double Max;
	double Acc;
	double AccTotal;
	bool YBin;
	bool NC_Type;
};
struct VALID_TEST
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
	long minute_post;
	long minute_post2;
	long minute_prior;
	double value_post;
	double value_prior;
};


class ModelConst {
	public:
	ModelConst();
	~ModelConst();
	double SecPerDay, PI, RAD;
	double JDayNum, DayLength;
	size_t JYearNum;

	protected:

	

};
