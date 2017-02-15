#pragma once
#if !defined( __CORE_VECTOR__ )
#define __CORE_VECTOR__

/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "core/types.h"
#include "core/util.h"

#include <math.h>

/************************************************************************/
/*                                                                      */
/* DEFINES AND CONSTANTS                                                */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* MACROS                                                               */
/*                                                                      */
/************************************************************************/
float constexpr PI = (3.1415926535897932384626f);
#define D2R(deg) ((deg) * (PI / 180.0f))

/************************************************************************/
/*                                                                      */
/* TYPES                                                                */
/*                                                                      */
/************************************************************************/



/************************************************************************/
/*                                                                      */
/* STRUCTS                                                              */
/*                                                                      */
/************************************************************************/


//------------------------------------------------------------------------
// vec2
//------------------------------------------------------------------------
struct vec2 
{
   vec2( float v ) 
      : x(v)
      , y(v) 
   {}

   vec2() 
      : vec2(0.0f)
   {}

   vec2( float _x, float _y )
      : x(_x)
      , y(_y) 
   {}
   

   float x;
   float y;
};

//------------------------------------------------------------------------
// vec3
//------------------------------------------------------------------------
struct vec3 
{
   float x;
   float y; 
   float z;

   vec3() 
      : x(0.0f)
      , y(0.0f)
      , z(0.0f) {}

   vec3( float v ) 
      : x(v)
      , y(v)
      , z(v) {}

   vec3( float _x, float _y, float _z ) 
      : x(_x)
      , y(_y)
      , z(_z)  {}

   inline vec3 cross( vec3 const &other ) const
   {
      return vec3( 
          y * other.z   - z * other.y, 
         -x * other.z   + z * other.x, 
          x * other.y   - y * other.x 
      );
   }

   inline float dot( vec3 const &other ) const 
   {
      return x * other.x
         + y * other.y 
         + z * other.z;
   }

   inline float get_length() const 
   {
      return sqrtf(this->dot(*this));
   }

   inline vec3 get_normalized() const 
   {
      float const il = 1.0f / get_length();
      return vec3( x * il, y * il, z * il );
   }

   inline vec3 operator-() const 
   {
      return vec3( -x, -y, -z );
   }
};

inline vec3 operator+( vec3 const &a, vec3 const &b ) 
{
   return vec3( a.x + b.x, a.y + b.y, a.z + b.z );
}

inline vec3 operator-( vec3 const &a, vec3 const &b )
{
   return vec3( a.x - b.x, a.y - b.y, a.z - b.z );
}

inline vec3 operator*( float const c, vec3 const &v )
{
   return vec3( c * v.x, c * v.y, c * v.z );
}

//------------------------------------------------------------------------
// vec4
//------------------------------------------------------------------------
struct vec4
{
   float x;
   float y;
   float z;
   float w;

   vec4( float x, float y, float z, float w = 1.0f )
      : x(x)
      , y(y)
      , z(z)
      , w(w)
   {}

   vec4( vec3 const &v, float w = 1.0f )
      : x(v.x)
      , y(v.y)
      , z(v.z)
      , w(w)
   {}

   inline vec3 xyz() const { return vec3( x, y, z ); }
};

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

#endif 
