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

/************************************************************************/
/*                                                                      */
/* STRUCTS                                                              */
/*                                                                      */
/************************************************************************/
struct vertex_t
{
   public:
      vec3 position;
      
   public:
      vertex_t() {}
      vertex_t( vec3 const &pos )
         : position(pos)
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
