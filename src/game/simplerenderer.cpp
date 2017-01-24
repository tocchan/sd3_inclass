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
      if (color_target->is_render_target()) {
         current_target = color_target; 
      } // else, WTH?
   } else {
      current_target = rhi_output->get_render_target();
   }

   rhi_context->set_color_target( current_target );
}

//------------------------------------------------------------------------
void SimpleRenderer::set_viewport( uint x, uint y, uint w, uint h ) 
{
   rhi_context->set_viewport( x, y, w, h );
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
   rhi_context->set_shader( program );
}

//------------------------------------------------------------------------
void SimpleRenderer::set_texture2d( uint tex_index, Texture2D *tex ) 
{
   rhi_context->set_texture2d( tex_index, tex );
}

//------------------------------------------------------------------------
void SimpleRenderer::set_sampler( uint samp_index, Sampler *samp ) 
{
   rhi_context->set_sampler( samp_index, samp );
}

//------------------------------------------------------------------------
void SimpleRenderer::draw( ePrimitiveType topology, 
   VertexBuffer *vbo, 
   uint const vertex_count ) 
{
   rhi_context->set_topology( topology );
   rhi_context->set_vertex_buffer( 0, vbo );
   rhi_context->draw( vertex_count );
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
