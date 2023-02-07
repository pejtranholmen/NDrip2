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
//! Dynamically allocates a zero-based 2D block (matrix) of data.

#ifndef ARRAY_MATRIX_H
#define ARRAY_MATRIX_H

#include <assert.h>

namespace Array
{

//! Dynamically allocates a zero-based 2D block (matrix) of data.

template<class T>inline T** matrix(long Rows, long Columns)
{
  T **m=new T*[Rows];
  assert(m!=nullptr);

  if(m!=nullptr)
  {
    m[0] = new T[Rows * Columns];
    assert(m[0]!=nullptr);
    if(m[0]!=nullptr)
    {
      for(long i=1; i<Rows; i++) m[i]=m[i-1] + Columns;
    }
  }
  return m;
}

//! Frees from memory a 2D block of dynamically allocated data.

template<class T>inline void free_matrix(T** Matrix)
{
  delete[](Matrix[0]);
  delete[](Matrix);
}

}

#endif

