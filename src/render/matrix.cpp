/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "render/matrix.h"

#include <math.h>

/************************************************************************/
/*                                                                      */
/* DEFINES AND CONSTANTS                                                */
/*                                                                      */
/************************************************************************/
STATIC mat44 const mat44::IDENTITY = mat44();

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
//------------------------------------------------------------------------
vec4 mat44::get_column( uint idx ) 
{
   return vec4( data[0 + idx], data[4 + idx], data[8 + idx], data[12 + idx] );
}

//------------------------------------------------------------------------
vec3 mat44::transform( vec3 const &v ) 
{
   vec3 r;
   r.x = v.dot( get_column(0).xyz() );
   r.y = v.dot( get_column(1).xyz() );
   r.z = v.dot( get_column(2).xyz() );

   return r;
}

//------------------------------------------------------------------------
void mat44::transpose()
{
   Swap( data[1], data[4] );
   Swap( data[2], data[8] );
   Swap( data[3], data[12] );
   Swap( data[6], data[9] );
   Swap( data[7], data[13] );
   Swap( data[11], data[14] );
}

//------------------------------------------------------------------------
vec3 mat44::get_translation() const 
{
   return vec3( t.x, t.y, t.z );
}

//------------------------------------------------------------------------
void mat44::set_translation( vec3 const &pos )
{
   t = vec4( pos, 1.0f );
}

//------------------------------------------------------------------------
void mat44::invert_orthonormal()
{
   vec3 translation = get_translation();
   set_translation( vec3( 0.0f ) );

   // invert the rotation part
   transpose();

   translation = transform( translation );
   set_translation( -translation );
}

//------------------------------------------------------------------------
mat44 mat44::get_transpose() const 
{
   mat44 ret = *this;
   ret.transpose();
   return ret;
}

//------------------------------------------------------------------------
mat44 mat44::get_inverse_orthonormal() const
{
   mat44 ret = *this;
   ret.invert_orthonormal();
   return ret;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
mat44 MatrixMakeOrthoProjection( float nx, float fx, 
   float ny, float fy,
   float nz /*= 0.0f*/, float fz /*= 1.0f*/ )
{
   mat44 mat;

   float sx = 1.0f / (fx - nx);
   float sy = 1.0f / (fy - ny);
   float sz = 1.0f / (fz - nz);

   mat.i = vec4( 2.0f * sx, 0.0f, 0.0f, 0.0f );
   mat.j = vec4( 0.0f, 2.0f * sy, 0.0f, 0.0f );
   mat.k = vec4( 0.0f, 0.0f, sz, 0.0f );
   mat.t = vec4( -(fx + nx) * sx, -(fy + ny) * sy, -nz * sz, 1.0f );

   return mat;
}

//------------------------------------------------------------------------
mat44 MatrixMakePerspectiveProjection( float const fov_radians, 
   float const aspect_ratio, 
   float const nz,
   float const fz )
{
   float size = 1.0f / tanf(fov_radians / 2.0f);

   // scale X or Y depending which dimension is bigger
   float w = size;
   float h = size;
   if (aspect_ratio > 1.0f) {
      w /= aspect_ratio;
   } else {
      h *= aspect_ratio;
   }

   float q = 1.0f / (fz - nz);

   mat44 mat;
   mat.i = vec4( w,     0.0f,    0.0f,          0.0f );
   mat.j = vec4( 0.0f,  h,       0.0f,          0.0f );
   mat.k = vec4( 0.0f,  0.0f,    fz * q,        1.0f );
   mat.t = vec4( 0.0f,  0.0f,    -nz * fz * q,  0.0f );
   
   return mat;
}

//------------------------------------------------------------------------
mat44 MatrixMakeLookAt( vec3 const &position, 
   vec3 const &target, 
   vec3 const &world_up /*= vec3( 0.0f, 1.0f, 0.0f )*/ )
{
   vec3 dir = target - position;
   vec3 forward = dir.get_normalized();
   vec3 right = world_up.cross( forward );
   vec3 up = forward.cross(right);

   mat44 mat;
   mat.i = vec4( right, 0.0f );
   mat.j = vec4( up, 0.0f );
   mat.k = vec4( forward, 0.0f );
   mat.t = vec4( position, 1.0f );

   return mat;
}


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
