#pragma once
#if !defined( __RENDER_TEXTURE2D__ )
#define __RENDER_TEXTURE2D__

/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "core/types.h"

#include "render/rhi/dx11.h"

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
class RHIDevice;
class RHIDeviceContext;
class RHIOutput;


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
class Texture2D
{
   public:
      Texture2D( RHIDevice *device );
      Texture2D( RHIDevice *device, RHIOutput *output );
      // Texture2D( RHIDevice *device, char const *filename );
      // Texture2D( RHIDevice *device, Image const *image );

      ~Texture2D();

      // bool load_from_file( char const *filename );
      // bool load_from_image( Image const *image );
      void destroy();

      inline uint get_width() const { return width; }
      inline uint get_height() const { return height; }

      inline bool is_valid() const { return (dx_resource != nullptr); } 
      inline bool is_render_target() const { return (dx_rtv != nullptr); }

   public:
      void create_views();

      RHIDevice *device;

      ID3D11Texture2D *dx_resource;
      ID3D11RenderTargetView *dx_rtv;
      // ID3D11ShaderResourceView *dx_srv;

      uint width;
      uint height;
      // uint dx_bind_flags; 
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
