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
//! Evaluates a polynomial of degree N.


#ifndef MATHS_ALGEBRA_POLYNOMIAL_POLY_EVAL_H
#define MATHS_ALGEBRA_POLYNOMIAL_POLY_EVAL_H

namespace Maths
{

namespace Algebra
{

namespace Polynomial
{

//! Evaluates a polynomial of degree N.

double polyEval(double x, const double coef[], int N)
{
  const double *p = coef;
  double ans = *p++;
  int i = N;

  do
    ans = ans * x  +  *p++;
  while( --i );

  return( ans );
}

//! Evaluates a polynomial of degree N, with $C_N=1.0$.

double polyEval1(double x, const double coef[], int N)
{
  const double *p = coef;
  double ans = x + *p++;
  int i = N-1;

  do
    ans = ans * x  + *p++;
  while( --i );

  return( ans );
}

}

}

}

#endif

