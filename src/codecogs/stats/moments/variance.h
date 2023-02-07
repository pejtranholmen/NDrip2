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
//! Calculates the variance of a given set of data.

#ifndef STATS_MOMENTS_VARIANCE_H
#define STATS_MOMENTS_VARIANCE_H

#include "./mean.h"

namespace Stats
{

namespace Moments
{

//! Calculates the variance of a given set of data.

template <class T>double variance(int n, T *data, bool total = false)
{
  double m=0, med=mean<T>(n,data);
  int i=n/8;
  while (i>0)
  {
    m += (data[0] - med)*(data[0] - med);
    m += (data[1] - med)*(data[1] - med);
    m += (data[2] - med)*(data[2] - med);
    m += (data[3] - med)*(data[3] - med);
    m += (data[4] - med)*(data[4] - med);
    m += (data[5] - med)*(data[5] - med);
    m += (data[6] - med)*(data[6] - med);
    m += (data[7] - med)*(data[7] - med);
    data += 8;
    i--;
  }

  switch (n%8)
  {
    case 7: m += (data[6] - med)*(data[6] - med);
    case 6: m += (data[5] - med)*(data[5] - med);
    case 5: m += (data[4] - med)*(data[4] - med);
    case 4: m += (data[3] - med)*(data[3] - med);
    case 3: m += (data[2] - med)*(data[2] - med);
    case 2: m += (data[1] - med)*(data[1] - med);
    case 1: m += (data[0] - med)*(data[0] - med);
  }
  if (total) n++;
  return m/(n-1);
}

}

}

#endif

