#include "../std.h"
#include "FUtil.h"

struct rgb_color
{
	int red;
	int green;
	int blue;
};



struct DistStat
{
	float Mean;
	float Std;
	float Min;
	float Max;
	float P5;
	float P50;
	float P95;
	size_t NumData;
};
struct TestAlfaFunctor
{
	bool stringIsCorrect;
	TestAlfaFunctor()
		:stringIsCorrect(true)
	{}

	void operator() (char ch)
	{
		if (stringIsCorrect && !((ch <= 'z' && ch >= 'a') || (ch <= 'Z' && ch >= 'A')))
			stringIsCorrect = false;
	}
};

namespace FUtil
{


	int GetProfileIntNo(string item, int value) {

		return value;
	}

}