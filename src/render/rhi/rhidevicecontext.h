#pragma once
#if !defined( __RENDER_CONTEXT__ )
#define __RENDER_CONTEXT__

/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "core/rgba.h"
#include "render/rhi/dx11.h"
#include "render/rhi/rhidevice.h"

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
class RHIInstance;  // System level singleton
class RHIDevice;     // physical GPU
class RHIDeviceContext;    // Potential Display/Background worker
class RHIOutput;
class Sampler;
class ShaderProgram;
class Texture2D;
class RasterState;
class BlendState;

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

//------------------------------------------------------------------------
//------------------------------------------------------------------------
// Specific State for Rendering
class RHIDeviceContext 
{
   public:
      RHIDeviceContext( RHIDevice *owner, ID3D11DeviceContext *ctx );
      ~RHIDeviceContext();

      void clear_state();
      void flush();

      void clear_color_target( Texture2D *output, rgba_fl const &color );

      void set_color_target( Texture2D *target );
      void set_viewport( uint x, uint y, uint w, uint h );

      void set_raster_state( RasterState *rs );
      void set_blend_state( BlendState *bs );

      void set_shader( ShaderProgram *shader );

      void set_texture2d( uint const idx, Texture2D *tex );
      inline void set_texture2d( Texture2D *tex ) { set_texture2d( 0, tex ); }

      void set_sampler( uint const idx, Sampler *sampler );
      inline void set_sampler( Sampler *sampler ) { set_sampler( 0, sampler ); }

      void set_vertex_buffer( uint idx, VertexBuffer *vbo );
      void set_topology( ePrimitiveType const top );

      void draw( uint const vcount, uint offset = 0U ); 

   public:
      ID3D11DeviceContext *dx_context;
      RHIDevice *device;
};




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
