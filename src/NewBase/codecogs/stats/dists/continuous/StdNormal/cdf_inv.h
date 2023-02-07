// CodeCogs Commercial License Agreement
// Copyright (C) 2004-2010 CodeCogs, Zyba Ltd, Broadwood, Holford, TA5 1DU, England.
//
// This software is licensed to Per-Erik Jansson 
// for commercial usage by version 1.2.1 of the CodeCogs Commercial Licence. You must 
// read this License (available at www.codecogs.com) before using this software.
//
// If you distribute this file it is YOUR responsibility to ensure that all 
// recipients have a valid number of commercial licenses. You must retain a
// copy of this licence in all copies you make. 
//
// This program is distributed WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// See the CodeCogs Commercial Licence for more details.
//---------------------------------------------------------------------------------
//! Evaluates the inverse of the standard normal cumulative density function.

#ifndef STATS_DISTS_CONTINUOUS_STDNORMAL_CDF_INV_H
#define STATS_DISTS_CONTINUOUS_STDNORMAL_CDF_INV_H

#include <math.h>

#include <assert.h>
#include "./codecogs/stats/dists/continuous/stdnormal/cdf.h"

#define SQRT2PI 2.5066282746310005024157652848108
#define MIN(a, b) ((a < b) ? (a) : (b))

namespace Stats
{

namespace Dists
{

namespace Continuous
{

namespace StdNormal
{

//! Evaluates the inverse of the standard normal cumulative density function.

double CDF_inv(double p)
{
  assert(p > 0 && p < 1);

  const double a[6] = {
  -3.969683028665376e+01, 2.209460984245205e+02,
  -2.759285104469687e+02, 1.383577518672690e+02,
  -3.066479806614716e+01, 2.506628277459239e+00
  };
  const double b[5] = {
  -5.447609879822406e+01, 1.615858368580409e+02,
  -1.556989798598866e+02, 6.680131188771972e+01,
  -1.328068155288572e+01
  };
  const double c[6] = {
  -7.784894002430293e-03,-3.223964580411365e-01,
  -2.400758277161838e+00,-2.549732539343734e+00,
   4.374664141464968e+00, 2.938163982698783e+00
  };
  const double d[4] = {
   7.784695709041462e-03, 3.224671290700398e-01,
   2.445134137142996e+00, 3.754408661907416e+00
  };

  double q = MIN(p, 1 - p), u, t;
  if (q > 0.02425) {
    u = q - 0.5;
    double t = u * u;
    u *= (((((a[0] * t + a[1]) * t + a[2]) * t + a[3]) * t + a[4]) * t + a[5])
        /(((((b[0] * t + b[1]) * t + b[2]) * t + b[3]) * t + b[4]) * t + 1);
  } else {
    double t = sqrt(-2 * log(q));
    u = (((((c[0] * t + c[1]) * t + c[2]) * t + c[3]) * t + c[4]) * t + c[5])
        /((((d[0] * t + d[1]) * t + d[2]) * t + d[3]) * t + 1);
  }
  t = Stats::Dists::Continuous::StdNormal::CDF(u) - q;
  t *= SQRT2PI * exp(u * u / 2);
  u -= t / (1 + u * t / 2);

  return p > 0.5 ? -u : u;
}

}

}

}

}

#undef SQRT2PI
#undef MIN

#endif

