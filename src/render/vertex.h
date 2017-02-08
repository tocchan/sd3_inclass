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

struct mat44
{
   mat44() 
      : i(1.0f, 0.0f, 0.0f, 0.0f) 
      , j(0.0f, 1.0f, 0.0f, 0.0f)
      , k(0.0f, 0.0f, 1.0f, 0.0f) 
      , t(0.0f, 0.0f, 0.0f, 1.0f)
   {}

   union {
      float data[16];
      vec4 rows[4];
      struct {
         vec4 i;
         vec4 j;
         vec4 k;
         vec4 t; 
      };
   };

   void transpose()
   {
      Swap( data[1], data[4] );
      Swap( data[2], data[8] );
      Swap( data[3], data[12] );
      Swap( data[6], data[9] );
      Swap( data[7], data[13] );
      Swap( data[11], data[14] );
   }

   mat44 get_transpose() const 
   {
      mat44 ret = *this;
      ret.transpose();
      return ret;
   }

   static mat44 const IDENTITY;
};

/************************************************************************/
/*                                                                      */
/* STRUCTS                                                              */
/*                                                                      */
/************************************************************************/
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
mat44 MatrixMakeOrthoProjection( float nx, float fx, 
   float ny, float fy,
   float nz = 0.0f, float fz = 1.0f );

mat44 MatrixMakePerspectiveProjection( float const fov_radians, 
   float const aspect_ratio, 
   float const nz,
   float const fz );

#endif 
