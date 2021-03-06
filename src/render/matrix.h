#pragma once
#if !defined( __RENDER_MATRIX__ )
#define __RENDER_MATRIX__

/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "core/vector.h"

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

   vec4 get_column( uint idx );

   vec3 get_translation() const;
   void set_translation( vec3 const &pos );

   vec3 transform( vec3 const &v );

   void transpose();
   void invert_orthonormal();

   mat44 get_transpose() const;
   mat44 get_inverse_orthonormal() const;


   static mat44 const IDENTITY;
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

mat44 MatrixMakeLookAt( vec3 const &position, vec3 const &target, vec3 const &up = vec3( 0.0f, 1.0f, 0.0f ) );

#endif 
