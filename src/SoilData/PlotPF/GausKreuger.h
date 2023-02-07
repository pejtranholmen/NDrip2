#include "../../std.h"
#include <limits>
#include <math.h>
#define PI 3.14159265

using namespace std;
struct XY
{
	double x;
	double y;
};

class GaussKreuger {

public:
		GaussKreuger(size_t i) ;
	    double axis; // Semi-major axis of the ellipsoid.
        double flattening; // Flattening of the ellipsoid.
        double central_meridian; // Central meridian for the projection.    
        double scale; // Scale on central meridian.
        double false_northing; // Offset for origo.
        double false_easting; // Offset for origo.
		XY grid_to_geodetic(double latitude, double longitude);
        XY geodetic_to_grid(double latitude, double longitude)
        {
  			XY ut;
            // Prepare ellipsoid-based stuff.
            double e2 = flattening * (2.0 - flattening);
            double n = flattening / (2.0 - flattening);
            double a_roof = axis / (1.0 + n) * (1.0 + n * n / 4.0 + n * n * n * n / 64.0);
            double A = e2;
            double B = (5.0 * e2 * e2 - e2 * e2 * e2) / 6.0;
            double C = (104.0 * e2 * e2 * e2 - 45.0 * e2 * e2 * e2 * e2) / 120.0;
            double D = (1237.0 * e2 * e2 * e2 * e2) / 1260.0;
            double beta1 = n / 2.0 - 2.0 * n * n / 3.0 + 5.0 * n * n * n / 16.0 + 41.0 * n * n * n * n / 180.0;
            double beta2 = 13.0 * n * n / 48.0 - 3.0 * n * n * n / 5.0 + 557.0 * n * n * n * n / 1440.0;
            double beta3 = 61.0 * n * n * n / 240.0 - 103.0 * n * n * n * n / 140.0;
            double beta4 = 49561.0 * n * n * n * n / 161280.0;

            // Convert.
            double deg_to_rad = PI / 180.0;
            double phi = latitude * deg_to_rad;
            double lambda = longitude * deg_to_rad;
            double lambda_zero = central_meridian * deg_to_rad;

            double phi_star = phi - sin(phi) * cos(phi) * (A +
                            B * pow(sin(phi), 2) +
                            C * pow(sin(phi), 4) +
                            D * pow(sin(phi), 6));
            double delta_lambda = lambda - lambda_zero;
            double xi_prim = atan(tan(phi_star) / cos(delta_lambda));
            double eta_prim = math_atanh(cos(phi_star) * sin(delta_lambda));
            double x = scale * a_roof * (xi_prim +
                            beta1 * sin(2.0 * xi_prim) * math_cosh(2.0 * eta_prim) +
                            beta2 * sin(4.0 * xi_prim) * math_cosh(4.0 * eta_prim) +
                            beta3 * sin(6.0 * xi_prim) * math_cosh(6.0 * eta_prim) +
                            beta4 * sin(8.0 * xi_prim) * math_cosh(8.0 * eta_prim)) +
                            false_northing;
            double y = scale * a_roof * (eta_prim +
                            beta1 * cos(2.0 * xi_prim) * math_sinh(2.0 * eta_prim) +
                            beta2 * cos(4.0 * xi_prim) * math_sinh(4.0 * eta_prim) +
                            beta3 * cos(6.0 * xi_prim) * math_sinh(6.0 * eta_prim) +
                            beta4 * cos(8.0 * xi_prim) * math_sinh(8.0 * eta_prim)) +
                            false_easting;
            ut.x = (x * 1000.0) / 1000.0;
            ut.y = (y * 1000.0) / 1000.0;

            return ut;
        };


        // Parameters for RT90 and SWEREF99TM.
        // Note: Parameters for RT90 are choosen to eliminate the 
        // differences between Bessel and GRS80-ellipsoides.
        // Bessel-variants should only be used if lat/long are given as
        // RT90-lat/long based on the Bessel ellipsoide (from old maps).
        // Parameter: projection (string). Must match if-statement.

private:
		void grs80_params() {
            axis = 6378137.0; // GRS 80.
            flattening = 1.0 / 298.257222101; // GRS 80.
            central_meridian = numeric_limits<double>::min();
		};
		void bessel_params() {
            axis = 6377397.155; // Bessel 1841.
            flattening = 1.0 / 299.1528128; // Bessel 1841.
            central_meridian = numeric_limits<double>::min();
            scale = 1.0;
            false_northing = 0.0;
            false_easting = 1500000.0;
		};
		void sweref99_params()
        {
            axis = 6378137.0; // GRS 80.
            flattening = 1.0 / 298.257222101; // GRS 80.
            central_meridian =numeric_limits<double>::min() ;
            scale = 1.0;
            false_northing = 0.0;
            false_easting = 150000.0;
        };


        double math_sinh(double value) {
            return 0.5 * (exp(value) - exp(-value));
        };
        double math_cosh(double value) {
            return 0.5 * (exp(value) + exp(-value));
        };
        double math_atanh(double value) {
            return 0.5 * log((1.0 + value) / (1.0 - value));
        };

};
