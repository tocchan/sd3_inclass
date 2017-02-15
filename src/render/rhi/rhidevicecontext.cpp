/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "rhidevicecontext.h"

#include "render/rhi/rhidevice.h"
#include "render/rhi/rhiinstance.h"
#include "render/rhi/rhioutput.h"

#include "render/rhi/blendstate.h"
#include "render/rhi/rasterstate.h"
#include "render/rhi/depthstencilstate.h"
#include "render/rhi/sampler.h"
#include "render/rhi/shaderprogram.h"
#include "render/rhi/texture2d.h"
#include "render/rhi/vertexbuffer.h"

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
// RENDER CONTEXT
//------------------------------------------------------------------------

//------------------------------------------------------------------------
RHIDeviceContext::RHIDeviceContext( RHIDevice *owner, ID3D11DeviceContext *ctx ) 
   : dx_context(ctx)
{
   device = owner;
}

//------------------------------------------------------------------------
RHIDeviceContext::~RHIDeviceContext()
{
   DX_SAFE_RELEASE(dx_context);
}


//------------------------------------------------------------------------
void RHIDeviceContext::clear_state()
{
   dx_context->ClearState();
}

//------------------------------------------------------------------------
void RHIDeviceContext::flush()
{
   dx_context->Flush();
}

//------------------------------------------------------------------------
void RHIDeviceContext::clear_color_target( Texture2D *output, rgba_fl const &color ) 
{
   dx_context->ClearRenderTargetView( output->dx_rtv, color );
}

//------------------------------------------------------------------------
void RHIDeviceContext::clear_depth_target( Texture2D *output
   , float depth /*= 1.0f*/
   , uint8_t stencil /*= 0*/ )
{
   if (output != nullptr) {
      dx_context->ClearDepthStencilView( output->dx_dsv, 
         D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 
         depth, stencil );
   }
}

//------------------------------------------------------------------------
void RHIDeviceContext::set_color_target( Texture2D *color, Texture2D *depth_stencil_target ) 
{
   // ASSERT_OR_DIE( color->is_render_target() );
   // ASSERT_OR_DIE( depth_stencil_target->is_depth_stencil_target() );
   dx_context->OMSetRenderTargets(1,
      &color->dx_rtv,
      depth_stencil_target != nullptr ? depth_stencil_target->dx_dsv : nullptr );
}

//------------------------------------------------------------------------
void RHIDeviceContext::set_viewport( uint x, uint y, uint w, uint h )
{
   // Also, set which region of the screen we're rendering to, in this case, all of it 
   D3D11_VIEWPORT viewport;
   memset( &viewport, 0, sizeof(viewport) );
   viewport.TopLeftX = (FLOAT)x;
   viewport.TopLeftY = (FLOAT)y;
   viewport.Width = (FLOAT)w;
   viewport.Height = (FLOAT)h;
   viewport.MinDepth = 0.0f;        // must be between 0 and 1 (defualt is 0);
   viewport.MaxDepth = 1.0f;        // must be between 0 and 1 (default is 1)

   dx_context->RSSetViewports(1, &viewport);
}

//------------------------------------------------------------------------
void RHIDeviceContext::set_raster_state( RasterState *rs ) 
{
   dx_context->RSSetState( rs->dx_state );
}

//------------------------------------------------------------------------
void RHIDeviceContext::set_blend_state( BlendState *bs ) 
{
   float constant[] = { 1, 1, 1, 1 };
   dx_context->OMSetBlendState( bs->dx_state, 
      constant, 
      0xffffffff );
}

//------------------------------------------------------------------------
void RHIDeviceContext::set_depth_stencil_state( DepthStencilState *dss ) 
{
   dx_context->OMSetDepthStencilState( dss->dx_state, 0U );
}

//------------------------------------------------------------------------
void RHIDeviceContext::set_shader( ShaderProgram *program ) 
{
   if (program->is_valid()) {
      dx_context->VSSetShader( program->dx_vertex_shader, nullptr, 0U );
      dx_context->PSSetShader( program->dx_fragment_shader, nullptr, 0U );
      dx_context->IASetInputLayout( program->dx_input_layout );
   }
}

//------------------------------------------------------------------------
void RHIDeviceContext::set_texture2d( uint tex_index, Texture2D *tex ) 
{
   dx_context->VSSetShaderResources( tex_index, 1, &tex->dx_srv );
   dx_context->PSSetShaderResources( tex_index, 1, &tex->dx_srv );
}

//------------------------------------------------------------------------
void RHIDeviceContext::set_sampler( uint samp_index, Sampler *samp ) 
{
   dx_context->VSSetSamplers( samp_index, 1, &samp->dx_sampler );
   dx_context->PSSetSamplers( samp_index, 1, &samp->dx_sampler );
}

//------------------------------------------------------------------------
void RHIDeviceContext::set_vertex_buffer( uint idx, VertexBuffer *vbo )
{
   // TODO: if supporting multipole vertex types - vbo should give me the stride
   uint stride = sizeof(vertex_t);
   uint offsets = 0;
   dx_context->IASetVertexBuffers( idx, 1, &vbo->dx_buffer, &stride, &offsets );
}

//------------------------------------------------------------------------
void RHIDeviceContext::set_topology( ePrimitiveType const topology )
{
   D3D11_PRIMITIVE_TOPOLOGY d3d_prim;
   switch (topology) {
      case PRIMITIVE_LINES:
         d3d_prim = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
         break;

      case PRIMITIVE_TRIANGLES: 
      default:
         d3d_prim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
   }

   dx_context->IASetPrimitiveTopology( d3d_prim );
}

//------------------------------------------------------------------------
void RHIDeviceContext::draw( uint const vcount, uint offset /*= 0U*/ )
{
   dx_context->Draw( vcount, offset );
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
