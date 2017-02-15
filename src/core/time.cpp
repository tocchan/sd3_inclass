/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "time.h"

#include "core/window.h"
#include <MMSystem.h>
#pragma comment(lib, "Winmm.lib")

// ASSERT_COMPILE_TIME( sizeof(LARGE_INTEGER) == sizeof(uint64_t) );

/************************************************************************/
/*                                                                      */
/* DEFINES AND CONSTANTS                                                */
/*                                                                      */
/************************************************************************/
#define TIME_MAX_INCREMENT_MS          (1024)
#define SCALE_MAX_PRECISION            (1024)

/************************************************************************/
/*                                                                      */
/* CLASSES                                                              */
/*                                                                      */
/************************************************************************/
class InternalTimeSystem 
{
   public:
      InternalTimeSystem() 
      {
         ::QueryPerformanceFrequency((LARGE_INTEGER*)&ops_per_second);
         seconds_per_op = 1.0 / (double)ops_per_second;

         ::QueryPerformanceCounter( (LARGE_INTEGER*)&start_ops );
      }

      uint64_t start_ops;
      uint64_t ops_per_second;

      double seconds_per_op;
};


/************************************************************************/
/*                                                                      */
/* LOCAL VARIABLES                                                      */
/*                                                                      */
/************************************************************************/
static InternalTimeSystem gTime;

/************************************************************************/
/*                                                                      */
/* EXTERNAL FUNCTIONS                                                   */
/*                                                                      */
/************************************************************************/

//------------------------------------------------------------------------
//
uint64_t __fastcall TimeGetOpCount()
{
   uint64_t i;
   QueryPerformanceCounter((LARGE_INTEGER*)&i);
   return i;
}

//------------------------------------------------------------------------
uint __fastcall TimeGet_ms()
{
   uint64_t i = TimeGetOpCount() - gTime.ops_per_second;
   i = (i * 1000U) / gTime.ops_per_second;
   return (uint)i;
}

//------------------------------------------------------------------------
uint __fastcall TimeGet_us()
{
   uint64_t i = TimeGetOpCount() - gTime.ops_per_second;;
   i = (i * 1000U * 1000U) / gTime.ops_per_second;   // f is operations per second, so I want to return micro seconds, so times the top by 1000000 us / 1 s
   return (uint)i;
}

//------------------------------------------------------------------------
double __fastcall TimeGetSeconds()
{
   uint64_t op = TimeGetOpCount() - gTime.ops_per_second;
   return (double)op * gTime.seconds_per_op;
}

//------------------------------------------------------------------------
uint64_t TimeOpCountTo_us( uint64_t op_count ) 
{
   op_count *= (1000U * 1000U);
   uint64_t const us = (uint64_t)(op_count * gTime.seconds_per_op);
   return us;
}

//------------------------------------------------------------------------
double TimeOpCountTo_ms( uint64_t op_count ) 
{
   double seconds = op_count * gTime.seconds_per_op;
   return seconds * 1000.0;
}

//------------------------------------------------------------------------
uint64_t TimeOpCountFrom_ms( double ms )
{
   double s = ms / 1000.0;
   uint64_t const ops = (uint64_t)(s * gTime.ops_per_second);
   return ops;
}

