#include "../../../std.h"
#include "../../../NewBase/PhysFunc.h"
class SurfaceRunOff {
public: SurfaceRunOff(double SurfaceCoef, double SurfacePoolMax): Threshold(SurfacePoolMax), RateCoef(SurfaceCoef) {}

		double operator() (double SurfacePool) {
			if (SurfacePool - Threshold > 0.001)
				return (SurfacePool - Threshold)*RateCoef;
			else
				return 0.;
}
	private:
	double Threshold, RateCoef;
};
class SurfacePoolCover {
public: SurfacePoolCover(double SPCovMax, double SPCovPot, double SPCovTot): a(SPCovMax), b(SPCovPot), c(SPCovTot) {}
	double operator() (double SurfacePool) {
		return a*max(0., pow(min(SurfacePool / b, 1.), c));
	}
private:
	double a, b, c;
};
