#pragma once
#if !defined( __CORE_UTIL__ )
#define __CORE_UTIL__

/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "core/types.h"

/************************************************************************/
/*                                                                      */
/* MACROS                                                               */
/*                                                                      */
/************************************************************************/
//------------------------------------------------------------------------
#define MEMBER_OFFSET(c,m)      (uintptr_t)(&(((c*)(nullptr))->m))
#define ARRAY_LENGTH(a)         (sizeof(a) / sizeof(a[0]))

/************************************************************************/
/*                                                                      */
/* FUNCTIONS                                                            */
/*                                                                      */
/************************************************************************/
//------------------------------------------------------------------------
// Swaps A and B
template <typename _T>
inline void Swap( _T &a, _T &b )
{
   _T temp = a;
   a = b;
   b = temp;

   /*
   a = a ^ b;
   b = b ^ a;
   a = a ^ b;
   */
}

//------------------------------------------------------------------------
inline void* GetElement( void *data, size_t stride, int idx )
{
   uint8_t *buf = (uint8_t*) data;
   return buf + (idx * stride);
}

//------------------------------------------------------------------------
void SwapElement( void *data, size_t stride, int idx0, int idx1 );


//------------------------------------------------------------------------
template <typename Tx> 
size_t ArrayFind( Tx const*const array, size_t const count, Tx const &element ) 
{
   for (size_t i = 0; i < count; ++i) {
      if (array[i] == element) {
         return i;
      }
   }

   return count;
}

#endif
