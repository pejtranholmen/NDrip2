#include "GausKreuger.h"

GaussKreuger::GaussKreuger(size_t i) {
			if(i==0) {
				//projection == "rt90_2.5_gon_v
                grs80_params();
                central_meridian = 15.0 + 48.0 / 60.0 + 22.624306 / 3600.0;
                scale = 1.00000561024;
                false_northing = -667.711;
                false_easting = 1500064.274;
			}
			else if(i==1) {
                sweref99_params();
                central_meridian = 15.00;
                scale = 0.9996;
                false_northing = 0.0;
                false_easting = 500000.0;
			}
			else if(i==2) {
                bessel_params();
                central_meridian = 11.0 + 18.0 / 60.0 + 29.8 / 3600.0;
			}

}
 XY GaussKreuger::grid_to_geodetic(double x, double y)
 {
			XY ut;

            if (central_meridian == numeric_limits<double>::min())
            {
				ut.x=0;ut.y=0;
                return ut;
            }
            // Prepare ellipsoid-based stuff.
            double e2 = flattening * (2.0 - flattening);
            double n = flattening / (2.0 - flattening);
            double a_roof = axis / (1.0 + n) * (1.0 + n * n / 4.0 + n * n * n * n / 64.0);
            double delta1 = n / 2.0 - 2.0 * n * n / 3.0 + 37.0 * n * n * n / 96.0 - n * n * n * n / 360.0;
            double delta2 = n * n / 48.0 + n * n * n / 15.0 - 437.0 * n * n * n * n / 1440.0;
            double delta3 = 17.0 * n * n * n / 480.0 - 37 * n * n * n * n / 840.0;
            double delta4 = 4397.0 * n * n * n * n / 161280.0;

            double Astar = e2 + e2 * e2 + e2 * e2 * e2 + e2 * e2 * e2 * e2;
            double Bstar = -(7.0 * e2 * e2 + 17.0 * e2 * e2 * e2 + 30.0 * e2 * e2 * e2 * e2) / 6.0;
            double Cstar = (224.0 * e2 * e2 * e2 + 889.0 * e2 * e2 * e2 * e2) / 120.0;
            double Dstar = -(4279.0 * e2 * e2 * e2 * e2) / 1260.0;

            // Convert.
            double deg_to_rad = PI / 180;
            double lambda_zero = central_meridian * deg_to_rad;
            double xi = (x - false_northing) / (scale * a_roof);
            double eta = (y - false_easting) / (scale * a_roof);
            double xi_prim = xi -
                            delta1 * sin(2.0 * xi) * math_cosh(2.0 * eta) -
                            delta2 * sin(4.0 * xi) * math_cosh(4.0 * eta) -
                            delta3 * sin(6.0 * xi) * math_cosh(6.0 * eta) -
                            delta4 * sin(8.0 * xi) * math_cosh(8.0 * eta);
            double eta_prim = eta -
                            delta1 * cos(2.0 * xi) * math_sinh(2.0 * eta) -
                            delta2 * cos(4.0 * xi) * math_sinh(4.0 * eta) -
                            delta3 * cos(6.0 * xi) * math_sinh(6.0 * eta) -
                            delta4 * cos(8.0 * xi) * math_sinh(8.0 * eta);
            double phi_star = asin(sin(xi_prim) / math_cosh(eta_prim));
            double delta_lambda = atan(math_sinh(eta_prim) / cos(xi_prim));
            double lon_radian = lambda_zero + delta_lambda;
            double lat_radian = phi_star + sin(phi_star) * cos(phi_star) *
                            (Astar +
                             Bstar * pow(sin(phi_star), 2) +
                             Cstar * pow(sin(phi_star), 4) +
                             Dstar * pow(sin(phi_star), 6));
            ut.x = lat_radian * 180.0 / PI;
            ut.y = lon_radian * 180.0 / PI;
            return ut;
        }