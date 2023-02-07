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
//! Calculates the linear regression parameters and evaluates the regression line at arbitrary abscissas

#pragma once
//#ifndef MATHS_REGRESSION_LINEAR_H
//#define MATHS_REGRESSION_LINEAR_H

#include <assert.h>
#include <math.h>

namespace Maths
{

namespace Regression
{

//! Given a set of points, this class calculates the linear regression parameters and evaluates the regression line at arbitrary abscissas.

class Linear
{
public:

//! Class constructor

Linear(int n, double *x, double *y)
{

            // calculate the averages of arrays x and y
            double xa = 0, ya = 0;
            for (int i = 0; i < n; i++) {
                xa += x[i];
                ya += y[i];
            }
            xa /= n;
            ya /= n;

            // calculate auxiliary sums
            double xx = 0, yy = 0, xy = 0;
            for (int i = 0; i < n; i++) {
                double tmpx = x[i] - xa, tmpy = y[i] - ya;
                xx += tmpx * tmpx;
                yy += tmpy * tmpy;
                xy += tmpx * tmpy;
            }

            // calculate regression line parameters

            // make sure slope is not infinite
            if(xx!= 0) {

                m_b = xy / xx;
                m_a = ya - m_b * xa;
				m_coeff = (fabs(yy) == 0) ? 1 : xy / sqrt(xx * yy);
			}
			else {
				m_b=0.;
				m_a=0.;
				m_coeff=0.;
			}

        }

//! Evaluates the linear regression function at the given abscissa.

double getValue(double x)
{
    return m_a + m_b * x;
}

//! Returns the slope of the regression line
double getSlope()
{
   return m_b;
}

//! Returns the intercept on the Y axis of the regression line
double getIntercept()
{
  return m_a;
}

//! Returns the linear regression coefficient

double getCoefficient()
{
  return m_coeff;
}

private:

double m_a, m_b, m_coeff;
};


//! A static function implementing the Linear Class for one off calculations
/*
double Linear_once(int n, double *x, double *y, double a )
{
  // This function is created to enable an Instant Calculator on CodeCogs. 
  // You probably shouldn't be using this function otherwise. 

   Maths::Regression::Linear A(n, x, y);
   return A.getValue(a);
}
*/
}

}

//#endif

