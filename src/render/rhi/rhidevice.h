#pragma once
#if !defined( __RENDER_DEVICE__ )
#define __RENDER_DEVICE__

/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "render/rhi/dx11.h"

#include "render/rhi/texture2d.h"
#include "render/rhi/types.h"


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
class RHIDeviceContext;    // Potential Display/Background worker
class RHIDevice;     // physical GPU
class RHIInstance;  // System level singleton
class RHIOutput;
class Texture2D;

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
// A single GPU
// All contexts derived from this
// device may use this devices resources
class RHIDevice
{

   public:
      RHIDevice( RHIInstance *owner, ID3D11Device *dxd );
      ~RHIDevice();

      RHIDeviceContext* get_immediate_context() { return immediate_context; }
      
      
   public:
      ID3D11Device *dx_device;

      RHIInstance *instance;         // hold a reference so the system doesn't shutdown without me.
      RHIDeviceContext *immediate_context; // reference to the immediate context

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
