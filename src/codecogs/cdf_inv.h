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
//! Evaluates the inverse of the normal cumulative density function.

#ifndef STATS_NORMALCDF_INV_H
#define STATS_NORMALCDF_INV_H

#include <math.h>
#include <codecogs/stats/dists/continuous/stdnormal/cdf_inv.h>

namespace Stats
{

namespace Dists
{

namespace Continuous
{

namespace Normal
{

//! Evaluates the inverse of the normal cumulative density function.

inline double CDF_inv(double x, double m, double s)
{
  return Stats::Dists::Continuous::StdNormal::CDF_inv(x) * s + m;
}

}

}

}

}

#endif

