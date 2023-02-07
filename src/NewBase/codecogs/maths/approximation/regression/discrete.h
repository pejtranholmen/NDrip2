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
//! Approximates a discrete function using least squares polynomial fitting.

#ifndef MATHS_REGRESSION_DISCRETE_H
#define MATHS_REGRESSION_DISCRETE_H

#include <math.h>
#include <string.h>
#include "./codecogs/computing/array/matrix.h"

namespace Maths
{

namespace Regression
{

//! Approximates a discrete function using least squares polynomial fitting.

class Discrete
{
public:

//! Class constructor

Discrete(int n, double *x, double *y, int degree)
{
    m_deg = degree;
    int m  = degree;
    int m1 = degree +1;
    
    // ---- Calc Powers of x and the RHS of [C][a]=[b] ----
    
  // Calculation \sum_{i=1}^n y_i * x_i^k
    double *b = new double [m1];
    // zero term
    double sum=0;
    for (int i = 0; i < n; i++)
      sum += y[i];
    b[0]=sum;
    
    // Computes \sum_{i=1}^n x_i^k which form the key elements of square matrix
    double *xc = new double [2*m1];
    xc[0] = n;  // \sum_o^n x^0 == n
    
    // compute xc and b for k=1 to m
    for (int k=1; k <= m; k++)
    {
      xc[k]=0;
      double sumx=0, sumy=0;
      for (int i = 0; i < n; i++) 
      {  
        double xpower = IntPower(x[i],k);
        sumx += xpower;
        sumy += y[i] * xpower;
      }
      xc[k]=sumx;
      b[k]= sumy;
    }
    
    // Compute rest of xc only.
    for (int k=m+1; k <= 2*m; k++)
    {
      double sumx=0;
      for (int i = 0; i < n; i++) 
        sumx += IntPower(x[i], k);
      xc[k]=sumx;
    }
    
    // ---- Create main matrix, [C] with x powers ----
    double **C = Array::matrix<double>(m1, m1);    
    for (int i = 0; i <= m; i++)
    {
      C[i][i] = xc[2*i];
      for (int j = i+1; j <= m; j++)
        C[i][j] = C[j][i] = xc[i+j];   // symetric matrix. 
    }
    
    // ---- Now compute inverse ----
    for (int k=0; k<m; k++)
      for (int i=k+1; i<=m; i++) 
      {
        b[i] -= C[i][k] / C[k][k] * b[k];
        for (int j=k+1; j<=m; j++)
          C[i][j] -= C[i][k] / C[k][k] * C[k][j];
      }
        
    // ---- Compute coefficients using inverse of c ----
    double *A = m_a =  new double [m1];  // user local refer here
    
    A[m] = b[m] / C[m][m];
    for (int i=m-1; i>=0; i--)  
    {
      double s = 0;
      for (int k=i+1; k<=m; k++)
        s += C[i][k] * A[k];
      A[i] = (b[i] - s) / C[i][i];
    }
    
    delete [] b;
    delete [] xc;
    Array::free_matrix(C);
}

//! Class destructor

~Discrete()
{
  delete [] m_a;
}

//! Returns an approximated value.

double getValue(double x)
{
  double p = 0;
  for (int k = 0; k <= m_deg; k++)
    p = p * x + m_a[m_deg - k];
  return p;
}

//! Returns the coefficients for the polynomial

double getCoefficent(int i)
{
  if(i<=m_deg) return m_a[i]; 
  return 0;
}

private:


// Calculates integer powers of x
double IntPower(double x,int k)
{
  if (x==0)  return 0;
  else return (exp(k*log(x)));
}

// degree of the fitting polynomial
int m_deg;

// coefficients of the polynomial
double *m_a;
};


//! A static function implementing the Discrete Class for one off calculations

double Discrete_once(int N, double *x, double *y, int degree, double a )
{
  // This function is created to enable an Instant Calculator on CodeCogs. 
  // You probably shouldn't be using this function otherwise. 

   Maths::Regression::Discrete A(N, x, y, degree);
   return A.getValue(a);
}

}

}

#endif

