#pragma once
#if !defined( __RHI_VERTEX_BUFFER__ )
#define __RHI_VERTEX_BUFFER__

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
class VertexBuffer 
{
   public:
      VertexBuffer( RHIDevice *owner, 
         vertex_t const *vertices, 
         uint const vertex_count );
      ~VertexBuffer(); 

      inline bool is_valid() const { return (dx_buffer != nullptr); }

   public:
      ID3D11Buffer *dx_buffer;
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
