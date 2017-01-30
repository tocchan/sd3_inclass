#pragma once
#if !defined( __RHI_CONSTANT_BUFFER__ )
#define __RHI_CONSTANT_BUFFER__

/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "render/rhi/dx11.h"

#include "render/vertex.h"


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
class ConstantBuffer 
{
   public:
      ConstantBuffer( RHIDevice *owner, 
         void const *buffer, 
         size_t const buffer_size );
      ~ConstantBuffer();

      bool update( RHIDeviceContext *context, void const *buffer );

      inline bool is_valid() const { return (dx_buffer != nullptr); }

   public:
      ID3D11Buffer *dx_buffer;
      size_t buffer_size;
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