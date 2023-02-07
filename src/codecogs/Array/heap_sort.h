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
//! Establish the heap property for a given root.

#ifndef ARRAY_SORT_HEAP_SORT_H
#define ARRAY_SORT_HEAP_SORT_H

#include <assert.h>

namespace Array
{

namespace Sort
{

//! Establish the heap property for a given root.

template <class T>void maxHeapify(T vals[], int n, int root)
{
  assert(n>=1);
  assert(root>=1);
  assert(vals!=nullptr);

  int done=0x00;
  int child=root*2;

  while ((child<=n)&&(!done)) {
    if ((child==n)||
        (vals[child-1]>vals[child])) {
      ;
    } else {
      child++;
    }

    if (vals[root-1]<vals[child-1]) {
      T tmp=vals[root-1];
      vals[root-1]=vals[child-1];
      vals[child-1]=tmp;
      root=child;
      child*=2;
    } else 
      done=0x01;
  }
  return;
}

//! Heap sort an array of values into ascending numerical order.

template <class T>void heapSort(T vals[], int n)
{
  assert(n>=1);
  assert(vals!=nullptr);

  for (int a=n/2; a>0; a--) maxHeapify<T>(vals, n, a);

  for (int a=n-1; a>0; a--) {
    T tmp=vals[0];
    vals[0]=vals[a];
    vals[a]=tmp;
    maxHeapify<T>(vals, a, 1);
  }

  return;
}

}

}

#endif

