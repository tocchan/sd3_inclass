#pragma once
#if !defined( __ITW_IMAGE_RGBA__ )
#define __ITW_IMAGE_RGBA__

/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "core/types.h"
#include "core/endian.h"

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

//------------------------------------------------------------------------
static float Clamp01( float v ) 
{
   if (v < 0.0f) {
      return 0.0f;
   } else if (v >= 1.0f) {
      return 1.0f;
   } else {
      return v;
   }
}

/************************************************************************/
/*                                                                      */
/* TYPES                                                                */
/*                                                                      */
/************************************************************************/
struct rgba_u32;
struct rgba_fl;

//------------------------------------------------------------------------
//------------------------------------------------------------------------
// vec4 color
struct rgba_fl
{
   union {
      struct {
         float x;
         float y; 
         float z;
         float w;
      };
      struct {
         float r;
         float g; 
         float b;
         float a;
      };
      float data[4];
   };

   rgba_fl() {} 
   rgba_fl( float const v ) 
      : r(v)
      , g(v)
      , b(v)
      , a(v) {}

   rgba_fl( uint32_t const rgba );
   rgba_fl( float _r, float _g, float _b, float _a ) 
      : r(_r)
      , g(_g)
      , b(_b)
      , a(_a) {}

   rgba_fl( float _r, float _g, float _b ) 
      : rgba_fl( _r, _g, _b, 1.0f ) {} 

   rgba_fl( rgba_u32 const &c );

   inline void set( float pr, float pg, float pb, float pa ) 
   {
      this->r = pr;
      this->g = pg;
      this->b = pb;
      this->a = pa;
   }

   inline void set( float pr, float pg, float pb ) 
   {
      set( pr, pg, pb, 1.0f );
   }

   //------------------------------------------------------------------------
   // helpers
   //------------------------------------------------------------------------
   operator float const*const () const { return data; } // used for D3D methods that use color


   // CONSTANTS //////////////////////////////////////////////////////////
   static rgba_fl const WHITE;
   static rgba_fl const BLACK;
   static rgba_fl const RED;
   static rgba_fl const GREEN;
   static rgba_fl const BLUE;
   static rgba_fl const YELLOW;
};


//------------------------------------------------------------------------
//------------------------------------------------------------------------
// Color as I usually use it... 0xRRGGBBAA
// 
struct rgba_u32
{
   union {
      struct {
         uint32_t r : 8;
         uint32_t g : 8;
         uint32_t b : 8;
         uint32_t a : 8;
      };
      uint32_t data;
   };

   //------------------------------------------------------------------------
   // U8 Ops
   inline void set( uint8_t pr, uint8_t pg, uint8_t pb, uint8_t pa )
   {
      data = ( (uint32_t)(pr) 
         | ((uint32_t)(pg) << 8) 
         | ((uint32_t)(pb) << 16) 
         | ((uint32_t)(pa) << 24) );
   }

   inline void set( uint8_t pr, uint8_t pg, uint8_t pb )
   {
      set( pr, pg, pb, 0xff );
   }

   inline void set_red( uint8_t v )
   {
      data = (data & ~(0xff)) | (v);
   }

   inline void set_blue( uint8_t v )
   {
      data = (data & ~(0xff << 8)) | (v << 8);
   }

   inline void set_green( uint8_t v )
   {
      data = (data & ~(0xff << 16)) | (v << 16);
   }

   inline void set_alpha( uint8_t v )
   {
      data = (data & ~(0xff << 24)) | (v << 24);
   }

   inline void set_rgb( uint8_t pr, uint8_t pg, uint8_t pb )
   {
      uint32_t rgb = ( (uint32_t)(pr) 
         | ((uint32_t)(pg) << 8) 
         | ((uint32_t)(pb) << 16) );

      data = (data & 0xff000000) | rgb;
   }
   
   //------------------------------------------------------------------------
   // Floating Point Ops
   inline void set( float pr, float pg, float pb, float pa )
   {
      data = ( (((uint32_t)(Clamp01(pr) * 255.0f + .5f) & 0xff))
            |  (((uint32_t)(Clamp01(pg) * 255.0f + .5f) & 0xff) << 8) 
            |  (((uint32_t)(Clamp01(pb) * 255.0f + .5f) & 0xff) << 16) 
            |  (((uint32_t)(Clamp01(pa) * 255.0f + .5f) & 0xff) << 24) );
   }

   inline void set( float pr, float pg, float pb )
   {
      set( pr, pg, pb, 1.0f );
   }

   inline void set_red( float v )
   {
      set_red( (uint8_t)(v * 255.0f) );
   }

   inline void set_green( float v )
   {
      set_green( (uint8_t)(v * 255.0f) );
   }
   
   inline void set_blue( float v )
   {
      set_blue( (uint8_t)(v * 255.0f) );
   }

   inline void set_alpha( float v )
   {
      set_alpha( (uint8_t)(v * 255.0f) );
   }

   inline void set_rgb( float pr, float pg, float pb )
   {
      uint32_t rgb = ( (((uint32_t)(pr * 255.0f) & 0xff))
            |  (((uint32_t)(pg * 255.0f) & 0xff) << 8) 
            |  (((uint32_t)(pb * 255.0f) & 0xff) << 16) );

      data = (data & 0xff000000) | rgb;
   }

   inline void get_floats( float *rgba_out ) const
   {
      rgba_out[0] = (float)(data & 0x000000ff) / 255.0f;
      rgba_out[1] = (float)((data & 0x0000ff00) >> 8) / 255.0f;
      rgba_out[2] = (float)((data & 0x00ff0000) >> 16) / 255.0f;
      rgba_out[3] = (float)((data & 0xff000000) >> 24) / 255.0f;
   }

   //////////////////////////////////////////////////////////////////////////
   // Dealing with another rgba_u32
   inline void set_rgb( rgba_u32 const &c )
   {
      data = ((data & 0xff000000) | (c.data & 0x00ffffff));
   }

   //////////////////////////////////////////////////////////////////////////
   // Constructors
   rgba_u32() {}

   rgba_u32( uint32_t v )
   {
      data = ToBigEndian(v);
   }

   rgba_u32( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
   {
      set( r, g, b, a );
   }

   rgba_u32( float r, float g, float b, float a )
   {
      set( r, g, b, a );
   }
   
   rgba_u32( float r, float g, float b )
   {
      set( r, g, b, 1.0f );
   }

   rgba_u32( rgba_fl const &c );
};

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

#endif 