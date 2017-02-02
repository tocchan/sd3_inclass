/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "blendstate.h"

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
static D3D11_BLEND ToDXFactor( eBlendFactor const factor )
{
   switch (factor) {
      case BLEND_ONE:
         return D3D11_BLEND_ONE;

      case BLEND_ZERO:
         return D3D11_BLEND_ZERO;
         
      case BLEND_SRC_ALPHA:
         return D3D11_BLEND_SRC_ALPHA;

      case BLEND_INV_SRC_ALPHA:
         return D3D11_BLEND_INV_SRC_ALPHA;

      default:
         // ASSERT_OR_DIE(false, "Undefined type.");
         return D3D11_BLEND_ONE;
   };
}

//------------------------------------------------------------------------
BlendState::BlendState( RHIDevice *owner, 
   bool enabled, 
   eBlendFactor const src_factor, 
   eBlendFactor const dest_factor )
   : device(owner)
   , dx_state(nullptr)
{
   D3D11_BLEND_DESC desc;
   MemZero( &desc );

   desc.AlphaToCoverageEnable = false;  // multisampling - not used in class
   desc.IndependentBlendEnable = false; // multiple render targest - do we have different blend state per?

   desc.RenderTarget[0].BlendEnable = enabled;

   // Color Blending
   desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
   desc.RenderTarget[0].SrcBlend = ToDXFactor( src_factor ); // BLEND_ONE
   desc.RenderTarget[0].DestBlend = ToDXFactor( dest_factor ); // BLEND_ZERO

   // Alpha Blending
   desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
   desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
   desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

   desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


   // Create it
   ID3D11Device *dx_device = device->dx_device;
   HRESULT hr = dx_device->CreateBlendState( &desc, &dx_state );

   if (FAILED(hr)) {
      // ASSERT_OR_DIE
      dx_state = nullptr;
   }
}

//------------------------------------------------------------------------
BlendState::~BlendState()
{
   DX_SAFE_RELEASE(dx_state);
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
