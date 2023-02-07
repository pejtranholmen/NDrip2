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
//! Calculates the covariance of a given set of data

#ifndef STATS_MOMENTS_COVARIANCE_H
#define STATS_MOMENTS_COVARIANCE_H

#include "mean.h"

namespace Stats
{

namespace Moments
{

//! Calculates the covariance of a given set of data

template <class T>double covariance(int n, T *data, T *data1)
{
  double
  med=Stats::Moments::mean<T>(n,data),
  med1=Stats::Moments::mean<T>(n,data1),
  m=0;

  int i=n/8;
  while (i>0)
  {
    m += data[0]*data1[0] - med*med1;
    m += data[1]*data1[1] - med*med1;
    m += data[2]*data1[2] - med*med1;
    m += data[3]*data1[3] - med*med1;
    m += data[4]*data1[4] - med*med1;
    m += data[5]*data1[5] - med*med1;
    m += data[6]*data1[6] - med*med1;
    m += data[7]*data1[7] - med*med1;
    data += 8;
    data1+= 8;
    i--;
  }

  switch (n%8)
  {
    case 7: m += data[6]*data1[6] - med*med1;
    case 6: m += data[5]*data1[5] - med*med1;
    case 5: m += data[4]*data1[4] - med*med1;
    case 4: m += data[3]*data1[3] - med*med1;
    case 3: m += data[2]*data1[2] - med*med1;
    case 2: m += data[1]*data1[1] - med*med1;
    case 1: m += data[0]*data1[0] - med*med1;
  }
  return m/n;
}

}

}

#endif

