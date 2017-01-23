/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "sampler.h"

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
//------------------------------------------------------------------------
Sampler::Sampler( RHIDevice *owner )
   : device(owner)
   , dx_sampler(nullptr)
{}

//------------------------------------------------------------------------
Sampler::Sampler( RHIDevice *owner, eFilterMode min, eFilterMode mag )
   : Sampler(owner)
{
   create_sampler( min, mag );
}

//------------------------------------------------------------------------
Sampler::~Sampler()
{
   DX_SAFE_RELEASE(dx_sampler);
}

//------------------------------------------------------------------------
static D3D11_FILTER DXGetFilterMode( eFilterMode min, eFilterMode mag )
{
   if (min == FILTER_POINT) {
      if (mag == FILTER_POINT) {
         return D3D11_FILTER_MIN_MAG_MIP_POINT;
      } else { // LINEAER
         return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
      }
   } else { // assume linear
      if (mag == FILTER_POINT) {
         return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT; 
      } else { // assume linear
         return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
      }
   }

   // UNSUPPORTED - Assert?
   // ASSERT_RETURN_VALUE( 0, D3D11_FILTER_MIN_MAG_MIP_POINT );
   // return D3D11_FILTER_MIN_MAG_MIP_POINT;
}

//------------------------------------------------------------------------
bool Sampler::create_sampler( eFilterMode min, eFilterMode mag ) 
{
   D3D11_SAMPLER_DESC desc;
   MemZero( &desc );

   desc.Filter = DXGetFilterMode( min, mag );
   desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
   desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
   desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
   // desc.BorderColor = BLACK TRANSPARENT if 0.
   desc.MinLOD = 0; 
   desc.MaxLOD = 0;

   ID3D11Device *dx_device = device->dx_device;
   HRESULT hr = dx_device->CreateSamplerState( &desc, &dx_sampler );
   
   return SUCCEEDED(hr);
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
