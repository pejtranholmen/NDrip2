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
//! Evaluates the standard normal cumulative density function.

#ifndef STATS_DISTS_CONTINUOUS_STDNORMAL_CDF_H
#define STATS_DISTS_CONTINUOUS_STDNORMAL_CDF_H

#include <math.h>
#include <codecogs/maths/special/errorfn.h>

#define SQRT2 1.4142135623730950488016887242097

namespace Stats
{

namespace Dists
{

namespace Continuous
{

namespace StdNormal
{

//! Evaluates the standard normal cumulative density function.

inline double CDF(double x)
{
  return 0.5 + 0.5 * Maths::Special::errorFn(x / SQRT2);
}

}

}

}

}

#undef SQRT2

#endif

