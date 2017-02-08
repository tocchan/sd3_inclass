#pragma once
#if !defined( __RENDER_VERTEX__ )
#define __RENDER_VERTEX__

/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "core/types.h"
#include "core/rgba.h"
#include "core/util.h"

/************************************************************************/
/*                                                                      */
/* DEFINES AND CONSTANTS                                                */
/*                                                                      */
/************************************************************************/
#define VERTEX_MAX_UVS (4)
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
};

inline vec3 operator+( vec3 const &a, vec3 const &b ) 
{
   return vec3( a.x + b.x, a.y + b.y, a.z + b.z );
}

inline vec3 operator*( float const c, vec3 const &v )
{
   return vec3( c * v.x, c * v.y, c * v.z );
}

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
};


struct vertex_t
{
   public:
      vec3 position;
      vec2 uv; 
      rgba_fl tint;
      
   public:
      vertex_t() {}
      vertex_t( vec3 const &pos, vec2 const &tex = vec2(0.0f), rgba_fl const &tint = rgba_fl::WHITE )
         : position(pos)
         , uv(tex)
         , tint(tint)
      {}
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
