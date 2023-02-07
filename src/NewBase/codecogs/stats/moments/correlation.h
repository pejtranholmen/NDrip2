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
//! Calculates the correlation of a given set of data.

#ifndef STATS_MOMENTS_CORRELATION_H
#define STATS_MOMENTS_CORRELATION_H

#include<math.h>
#include"./variance.h"
#include"./covariance.h"

namespace Stats
{

namespace Moments
{

//! Calculates the correlation of a given set of data.

template <class T>inline double correlation(int n ,T *data ,T *data1)
{
  double var=variance<T>(n,data);
  double var1=variance<T>(n,data1);
  double covar=covariance<T>(n,data,data1);
  return covar/sqrt(var*var1);
}

}

}

#endif

