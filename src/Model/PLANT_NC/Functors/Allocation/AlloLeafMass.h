#pragma once
#include "../../../../std.h"

class CAlloLeaf_Fixed {
public: CAlloLeaf_Fixed(vector<double> AlloLeaf1):
	a(AlloLeaf1){}
		double operator() (size_t index, double Mass=0) {
			return a[index];
}
private:
	vector<double> a;
};
class CAlloLeaf_Linear {
public: CAlloLeaf_Linear(vector<double> AlloLeaf1, vector<double> AlloLeaf2) :
	a(AlloLeaf1), b(AlloLeaf2) {}
		double operator() (size_t index, double Mass = 0) {
			return a[index]+Mass*b[index];
		}
private:
	vector<double> a, b;

};
class CAlloLeaf_Exp {
public: CAlloLeaf_Exp(vector<double> AlloLeaf1, vector<double> AlloLeaf2, vector<double> AlloLeaf3) :
	a(AlloLeaf1), b(AlloLeaf2), c(AlloLeaf3) {}
	double operator() (size_t index, double Mass = 0) {
		return a[index]*exp( Mass*c[index]);
	}
private:
	vector<double> a, b, c;

};
