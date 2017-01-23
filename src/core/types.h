#pragma once
#if !defined( __CORE_TYPES__ )
#define __CORE_TYPES__

/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include <stdint.h>

// Some common warnings to ignore
#pragma warning( disable : 4201 )  // nonstandard extension used: nameless struct/union
#pragma warning( disable : 4800 )  // cast warnings
// #pragma warning( disable : 4244 )  // possible loss of data
#pragma warning( disable : 4577 )  // <exception> causes a warning now if you compile without exceptions, dammit windows
#pragma warning( disable : 4530 )  // <exception> causes a warning now if you compile without exceptions, dammit windows

/************************************************************************/
/*                                                                      */
/* DEFINES AND CONSTANTS                                                */
/*                                                                      */
/************************************************************************/
#if !defined(NULL)
   #define NULL 0
#endif

// Used for clarity in source files.
#define VIRTUAL
#define STATIC
#define OVERRIDE

#define INVALID_HANDLE      (NULL)


#if defined(_WIN64) && !defined(_X64)
   static_assert( false, "Define _X64 in build settings." );
#endif

// Number of times to attempt an operation before yielding the thread.
#define MEM_DEFAULT_ALIGNMENT sizeof(void*)
#define MEM_LONGJUMP_ALIGNMENT (16U)

#define ASYNC_DEFAULT_TRY_COUNT (32)
   
#if !defined UNREFERENCED
   #define UNREFERENCED(v) v;
#endif

// Used for combining unique names
#define COMBINE1(X,Y) X##Y  // helper macro
#define COMBINE(X,Y) COMBINE1(X,Y)
   
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)

#define XOR(a, b)  (!(a) != !(b))


/************************************************************************/
/*                                                                      */
/* MACROS                                                               */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* TYPES                                                                */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* DEFINES                                                              */
/*                                                                      */
/************************************************************************/

// 
// TYPES
//
   /*
typedef __int8               int8_t;
typedef __int16            int16_t;
typedef __int32            int32_t;
typedef __int64            int64_t;

typedef unsigned __int8      uint8_t;
typedef unsigned __int16   uint16_t;
typedef unsigned __int32   uint32_t;
typedef unsigned __int64   uint64_t;
   */

typedef unsigned int uint;
typedef uint8_t byte_t;


// Some common types
typedef void* thread_id_t;
typedef void* thread_handle_t;

/************************************************************************/
/*                                                                      */
/* CONSTANTS                                                            */
/*                                                                      */
/************************************************************************/
uint32_t constexpr UINFINITY32   = (uint32_t)(-1);
uint64_t constexpr UINFINITY64   = (uint64_t)(-1);
uint     constexpr UINFINITY     = (uint)(-1);

uint     constexpr INVALID_INDEX = (uint)(-1);
size_t   constexpr MAX_SIZE      = (size_t)(-1);
uint     constexpr MAX_COUNT     = (uint)(-1);

float    constexpr MAX_FLOAT     = ( 3.402823466e+38f ); // stolen from D3D11

/************************************************************************/
/*                                                                      */
/* STRUCTS                                                              */
/*                                                                      */
/************************************************************************/
#pragma warning( push )
   // disable wrap warning since I expect it to wrap to get min
   #pragma warning( disable : 4307 )
   template <typename _Tx>
   struct type_data
   {
      public:
         static _Tx  constexpr      ZERO           = (_Tx)0;
         static _Tx  constexpr      ONE            = (_Tx)1;
         static _Tx  constexpr      MASK           = static_cast<_Tx>(-1);
         static bool constexpr      IS_SIGNED      = MASK < ZERO;
         static _Tx  constexpr      SIGNBIT        = IS_SIGNED ? (_Tx)(ONE << (_Tx)((sizeof(_Tx) * 8 - 1))) : ZERO;
         static _Tx  constexpr      MAX            = IS_SIGNED ? (MASK & ~SIGNBIT) : MASK;
         static _Tx  constexpr      MIN            = SIGNBIT;
         static _Tx  constexpr      HALFMAX        = (MAX / 2) + 1;
   };
#pragma warning( pop )

/************************************************************************/
/*                                                                      */
/* STRUCTS                                                              */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* CLASSES                                                              */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* GLOBAL VARIABLES                                                     */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* FUNCTION PROTOTYPES                                                  */
/*                                                                      */
/************************************************************************/
   
//------------------------------------------------------------------------
// Used by many methods - may be overloaded by type
template <typename T>
inline int Compare( T const &a, T const &b ) 
{
   return a - b;
}

//------------------------------------------------------------------------
template <typename T>
inline void MemZero( T *ptr ) 
{
   memset( ptr, 0, sizeof(T) );
}


#endif 
