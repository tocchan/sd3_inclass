/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "game/simplerenderer.h"

// A01
#include "game/game_config.h"

#include "render/rhi/rhiinstance.h"
#include "render/rhi/rhidevice.h"
#include "render/rhi/rhidevicecontext.h"
#include "render/rhi/rhioutput.h"

#include "render/rhi/sampler.h"
#include "render/rhi/shaderprogram.h"
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
SimpleRenderer::SimpleRenderer() 
   : rhi_device(nullptr)
   , rhi_context(nullptr)
   , rhi_output(nullptr)
   , current_target(nullptr) 
{
}

//------------------------------------------------------------------------
void SimpleRenderer::setup( uint width, uint height ) 
{
   RHIInstance *ri = RHIInstance::GetInstance();
   ri->create_output( &rhi_device, &rhi_context, &rhi_output, width, height );

   rhi_output->window->set_title( "GUILDHALL : SD3 ASSIGNMENT 1" );

   set_render_target(nullptr);
}

//------------------------------------------------------------------------
void SimpleRenderer::set_title( char const *new_title ) 
{
   rhi_output->window->set_title( new_title );
}

//------------------------------------------------------------------------
void SimpleRenderer::destroy() 
{
   delete rhi_output;
   delete rhi_context;
   delete rhi_device;
}

//------------------------------------------------------------------------
void SimpleRenderer::set_render_target( Texture2D *color_target )
{
   if (color_target != nullptr) {
      current_target = color_target; 
   } else {
      current_target = rhi_output->get_render_target();
   }

   rhi_context->dx_context->OMSetRenderTargets(1,
      &current_target->dx_rtv,
      nullptr);
}

//------------------------------------------------------------------------
void SimpleRenderer::set_viewport( uint x, uint y, uint w, uint h ) 
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

   rhi_context->dx_context->RSSetViewports(1, &viewport);
}

//------------------------------------------------------------------------
void SimpleRenderer::process_messages()
{
   rhi_output->window->process_messages();
}

//------------------------------------------------------------------------
bool SimpleRenderer::is_closed() const
{
   return (rhi_output == nullptr) || rhi_output->window->is_closed();
}

//------------------------------------------------------------------------
void SimpleRenderer::clear_color( rgba_fl const &color ) 
{
   rhi_context->clear_color_target( current_target, color );
}

//------------------------------------------------------------------------
void SimpleRenderer::clear_target_color( Texture2D *target, rgba_fl const &color )
{
   rhi_context->clear_color_target( target, color );
}

//------------------------------------------------------------------------
void SimpleRenderer::present()
{
   rhi_output->present();
}

//------------------------------------------------------------------------
void SimpleRenderer::set_shader( ShaderProgram *program ) 
{
   rhi_context->dx_context->VSSetShader( program->dx_vertex_shader, nullptr, 0U );
   rhi_context->dx_context->PSSetShader( program->dx_fragment_shader, nullptr, 0U );
   rhi_context->dx_context->IASetInputLayout( program->dx_input_layout );
}

//------------------------------------------------------------------------
void SimpleRenderer::set_texture2d( uint tex_index, Texture2D *tex ) 
{
   rhi_context->dx_context->VSSetShaderResources( tex_index, 1, &tex->dx_srv );
   rhi_context->dx_context->PSSetShaderResources( tex_index, 1, &tex->dx_srv );
}

//------------------------------------------------------------------------
void SimpleRenderer::set_sampler( uint samp_index, Sampler *samp ) 
{
   rhi_context->dx_context->VSSetSamplers( samp_index, 1, &samp->dx_sampler );
   rhi_context->dx_context->PSSetSamplers( samp_index, 1, &samp->dx_sampler );
}

//------------------------------------------------------------------------
void SimpleRenderer::draw( ePrimitiveType topology, 
   VertexBuffer *vbo, 
   uint const vertex_count ) 
{
   D3D11_PRIMITIVE_TOPOLOGY d3d_prim;
   switch (topology) {
      case PRIMITIVE_TRIANGLES: 
      default:
         d3d_prim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
   }

   rhi_context->dx_context->IASetPrimitiveTopology( d3d_prim );
   uint stride = sizeof(vertex_t);
   uint offsets = 0;
   rhi_context->dx_context->IASetVertexBuffers( 0, 1, &vbo->dx_buffer, &stride, &offsets );

   rhi_context->dx_context->Draw( vertex_count, 0 );
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
