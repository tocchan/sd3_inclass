/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "texture2d.h"

#include "core/image.h"

#include "render/rhi/rhidevicecontext.h"
#include "render/rhi/rhidevice.h"
#include "render/rhi/rhioutput.h"

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

/************************************************************************/
/*                                                                      */
/* CLASSES                                                              */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* LOCAL VARIABLES                                                      */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* GLOBAL VARIABLES                                                     */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* LOCAL FUNCTIONS                                                      */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* EXTERNAL FUNCTIONS                                                   */
/*                                                                      */
/************************************************************************/
//------------------------------------------------------------------------
// TEXTURE2D
//------------------------------------------------------------------------

//------------------------------------------------------------------------
Texture2D::Texture2D( RHIDevice *owner ) 
   : device(owner)
   , dx_resource(nullptr)
   , dx_rtv(nullptr)
   // , dx_srv(nullptr)
   , width(0)
   , height(0)
   // , dx_bind_flags(0U
{}

//------------------------------------------------------------------------
// specialized for the output's texture
Texture2D::Texture2D( RHIDevice *owner, RHIOutput *output ) 
   : Texture2D( owner )
{
   if (output != nullptr) {
      ID3D11Texture2D *back_buffer = nullptr;
      output->swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);
      if (nullptr != back_buffer) {
         dx_resource = back_buffer;

         // Get info about it.
         D3D11_TEXTURE2D_DESC desc;
         dx_resource->GetDesc(&desc);

         // Set flags for backbuffer texture
         width = desc.Width;
         height = desc.Height;
         dx_bind_flags = desc.BindFlags;

         create_views();
      }
   }
}

//------------------------------------------------------------------------
Texture2D::Texture2D( RHIDevice *owner, Image const &img ) 
   : Texture2D( owner )
{
   load_from_image( img );
}

//------------------------------------------------------------------------
Texture2D::Texture2D( RHIDevice *owner, char const *filename )
   : Texture2D(owner)
{
   Image image;
   if (!image.load_from_file( filename )) {
      return;
   }

   // load_from_image( image );

}


//------------------------------------------------------------------------
Texture2D::~Texture2D()
{
   destroy();
}

//------------------------------------------------------------------------
bool Texture2D::load_from_image( Image const &img ) 
{
   D3D11_USAGE usage = D3D11_USAGE_IMMUTABLE;

   D3D11_TEXTURE2D_DESC tex_desc;
   memset( &tex_desc, 0, sizeof(tex_desc) );

   tex_desc.Width = img.get_width();
   tex_desc.Height = img.get_height();
   tex_desc.MipLevels = 1;                // no mip mapping
   tex_desc.ArraySize = 1;                // NO ARRAYS!  
   tex_desc.Usage = usage;
   tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
   tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
   tex_desc.CPUAccessFlags = 0U;
   tex_desc.MiscFlags = 0U; // there is one for generating mip maps.

   // multisampling options
   tex_desc.SampleDesc.Count = 1;
   tex_desc.SampleDesc.Quality = 0;

   // Helps us figure out what views to make
   dx_bind_flags = D3D11_BIND_SHADER_RESOURCE;

   // Setup initial data
   D3D11_SUBRESOURCE_DATA data;
   memset( &data, 0, sizeof(data) );
   data.pSysMem = img.buffer;
   data.SysMemPitch = img.bpp * tex_desc.Width;

   ID3D11Device *dx_device = device->dx_device;
   HRESULT hr = dx_device->CreateTexture2D( &tex_desc, &data, &dx_resource );

   if (SUCCEEDED(hr)) {
      create_views();
      return true;
   } else {
      return false;
   }
}

//------------------------------------------------------------------------
void Texture2D::destroy()
{
   if (is_valid()) {
      DX_SAFE_RELEASE(dx_rtv);
      DX_SAFE_RELEASE(dx_resource);
   }
}

//------------------------------------------------------------------------
void Texture2D::create_views()
{
   ID3D11Device *dd = device->dx_device;

   if (dx_bind_flags & D3D11_BIND_RENDER_TARGET) {
      dd->CreateRenderTargetView( dx_resource, nullptr, &dx_rtv );
   } 

   if (dx_bind_flags & D3D11_BIND_SHADER_RESOURCE) {
      dd->CreateShaderResourceView( dx_resource, nullptr, &dx_srv );
   }
}


/************************************************************************/
/*                                                                      */
/* COMMANDS                                                             */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* UNIT TESTS                                                           */
/*                                                                      */
/************************************************************************/
