#pragma once
#include "../../../../std.h"
class PhoPLeafResponse {
public: PhoPLeafResponse(vector<double> CPLeafOpt, vector<double> CPLeafThres) : CPLeafThres(CPLeafThres), CPLeafOpt(CPLeafOpt) {}
		double operator() (size_t index, double CP_RatioLeaf) {
		return max(0.001, min(1., (CP_RatioLeaf - CPLeafThres[index]) / (CPLeafOpt[index] - CPLeafThres[index])));
}

private:
	vector<double> CPLeafThres, CPLeafOpt;
};

