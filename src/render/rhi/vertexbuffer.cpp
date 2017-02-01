/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "render/rhi/vertexbuffer.h"

#include "render/rhi/rhidevice.h"
#include "render/rhi/rhidevicecontext.h"

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
VertexBuffer::VertexBuffer( RHIDevice *device, 
   vertex_t const *vertices, 
   uint const vertex_count, 
   eBufferUsage desired_usage /*= BUFFERUSAGE_STATIC*/ )
{
   
   usage = desired_usage;
   dx_buffer = nullptr;
   buffer_size = 0U;

   create( device, vertices, vertex_count );
  

   // Finally create the vertex buffer
}

//------------------------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
   DX_SAFE_RELEASE(dx_buffer);
}

//------------------------------------------------------------------------
bool VertexBuffer::create( RHIDevice *device, vertex_t const *data, uint const count ) 
{
   // make sure I free the old one
   DX_SAFE_RELEASE( dx_buffer );

   buffer_size = count * sizeof(vertex_t);

   D3D11_BUFFER_DESC vb_desc;
   MemZero( &vb_desc );

   vb_desc.ByteWidth = buffer_size;  // How much data are we putting into this buffer
   vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;         // What can we bind this data as (in this case, only vertex data)
   vb_desc.StructureByteStride = sizeof(vertex_t);       // How large is each element in this buffer

   if (usage == BUFFERUSAGE_DYNAMIC) {
      vb_desc.Usage = D3D11_USAGE_DYNAMIC;
      vb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
   } else {
      vb_desc.Usage = D3D11_USAGE_IMMUTABLE;
   }

   // Next, setup the initial data (required since this is an immutable buffer - so it must be instantiated at creation time)
   dx_buffer = nullptr;
   if (nullptr != data) {
      D3D11_SUBRESOURCE_DATA initial_data;
      memset( &initial_data, 0, sizeof(initial_data) );
      initial_data.pSysMem = data;

      device->dx_device->CreateBuffer( &vb_desc, &initial_data, &dx_buffer );
   } else {
      device->dx_device->CreateBuffer( &vb_desc, nullptr, &dx_buffer );
   }

   if (dx_buffer == nullptr) {
      buffer_size = 0U;
      return false;
   } else {
      return true;
   }
}

//------------------------------------------------------------------------
void VertexBuffer::update( RHIDeviceContext *context, vertex_t const* data, uint const count ) 
{
   size_t size = sizeof(vertex_t) * count;
   if (size > buffer_size) {
      // too large - can't map it.  Recreate a larger one [note, will require a rebind
      // to the context before it takes affect]
      create( context->device, data, count );
   } else {
      // Smaller than the block of memory allocated - so just copy into it.
      D3D11_MAPPED_SUBRESOURCE resource;
      ID3D11DeviceContext *dx_context = context->dx_context;
      if (SUCCEEDED(dx_context->Map( dx_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0U, &resource ))) {
         memcpy( resource.pData, data, size );
         dx_context->Unmap( dx_buffer, 0 );
      }
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
