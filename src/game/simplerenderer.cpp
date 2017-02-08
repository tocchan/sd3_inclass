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
   , matrix_cb(nullptr)
   , time_cb(nullptr)
   , temp_vbo(nullptr)
   , current_blend_state(nullptr)
{

}

//------------------------------------------------------------------------
void SimpleRenderer::setup( uint width, uint height ) 
{
   RHIInstance *ri = RHIInstance::GetInstance();
   ri->create_output( &rhi_device, &rhi_context, &rhi_output, width, height );

   rhi_output->window->set_title( "GUILDHALL : SD3 ASSIGNMENT 1" );

   set_render_target(nullptr);

   default_raster_state = new RasterState( rhi_device );
   rhi_context->set_raster_state( default_raster_state );

   MemZero( &time_data );

   matrix_cb = new ConstantBuffer( rhi_device, &matrix_data, sizeof(matrix_data) );
   time_cb = new ConstantBuffer( rhi_device, &time_data, sizeof(time_data) );

   // BIND constant buffers
   set_constant_buffer( MATRIX_BUFFER_INDEX, matrix_cb );
   set_constant_buffer( TIME_BUFFER_INDEX, time_cb );

   temp_vbo = new VertexBuffer( rhi_device, nullptr, 32, BUFFERUSAGE_DYNAMIC );
}

//------------------------------------------------------------------------
void SimpleRenderer::destroy() 
{
   SAFE_DELETE(current_blend_state);
   SAFE_DELETE(default_raster_state);
   SAFE_DELETE(matrix_cb);
   SAFE_DELETE(time_cb);
   SAFE_DELETE(temp_vbo);

   delete rhi_output;
   delete rhi_context;
   delete rhi_device;
}

//------------------------------------------------------------------------
void SimpleRenderer::set_title( char const *new_title ) 
{
   rhi_output->window->set_title( new_title );
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
void SimpleRenderer::set_projection_matrix( mat44 const &proj )
{
   matrix_data.projection = proj.get_transpose();
   matrix_cb->update( rhi_context, &matrix_data );
}

//------------------------------------------------------------------------
void SimpleRenderer::set_ortho_projection( vec2 const &bottom_left, vec2 const &top_right ) 
{
   mat44 proj = MatrixMakeOrthoProjection( bottom_left.x, 
      top_right.x, 
      bottom_left.y, 
      top_right.y, 
      0.0f, 1.0f );
   set_projection_matrix( proj );
}

//------------------------------------------------------------------------
void SimpleRenderer::set_perspective_projection( float const fov_radians, 
   float aspect_ratio, 
   float const nz, 
   float const fz )
{
   mat44 proj = MatrixMakePerspectiveProjection( fov_radians, 
      aspect_ratio, 
      nz, 
      fz );
   set_projection_matrix( proj );
}


//------------------------------------------------------------------------
void SimpleRenderer::enable_blend( eBlendFactor src, eBlendFactor dest )
{
   if (current_blend_state != nullptr) {
      if ( (blend_state.enabled == true)
         && (blend_state.src_factor == src)
         && (blend_state.dst_factor == dest) ) 
      {
         return;
      }
   }

   SAFE_DELETE( current_blend_state );

   BlendState *bs = new BlendState( rhi_device, true, src, dest );
   rhi_context->set_blend_state( bs );

   blend_state.enabled = true;
   blend_state.src_factor = src;
   blend_state.dst_factor = dest;

   current_blend_state = bs;
}

//------------------------------------------------------------------------
void SimpleRenderer::disable_blend()
{
   if (!blend_state.enabled) {
      return;
   }

   SAFE_DELETE( current_blend_state );
   BlendState *bs = new BlendState( rhi_device, false );
   rhi_context->set_blend_state( bs );

   blend_state.enabled = false;

   current_blend_state = bs;
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
void SimpleRenderer::set_constant_buffer( uint idx, ConstantBuffer *cb )
{
   // rhi_context->set_constant_buffer( ... );
   rhi_context->dx_context->VSSetConstantBuffers( idx, 1, &cb->dx_buffer );
   rhi_context->dx_context->PSSetConstantBuffers( idx, 1, &cb->dx_buffer );
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

//------------------------------------------------------------------------
void SimpleRenderer::draw_vertex_array( ePrimitiveType topology, 
   vertex_t const *vertices,
   uint const count )
{
   temp_vbo->update( rhi_context, vertices, count );
   draw( topology, temp_vbo, count );
}

//------------------------------------------------------------------------
void SimpleRenderer::draw_quad2d( vec2 const &bl, vec2 const &tr, rgba_fl const &color ) 
{
   vertex_t vertices[] = {
      vertex_t( vec3( bl.x, bl.y, 0.0f ), vec2( 0.0f, 1.0f ), color ), 
      vertex_t( vec3( tr.x, tr.y, 0.0f ), vec2( 1.0f, 0.0f ), color ), 
      vertex_t( vec3( bl.x, tr.y, 0.0f ), vec2( 0.0f, 0.0f ), color ), 
      vertex_t( vec3( bl.x, bl.y, 0.0f ), vec2( 0.0f, 1.0f ), color ), 
      vertex_t( vec3( tr.x, bl.y, 0.0f ), vec2( 1.0f, 1.0f ), color ), 
      vertex_t( vec3( tr.x, tr.y, 0.0f ), vec2( 1.0f, 0.0f ), color ),
   };

   draw_vertex_array( PRIMITIVE_TRIANGLES, vertices, 6 );
}

//------------------------------------------------------------------------
void SimpleRenderer::draw_quad3d( vec3 const &origin, 
   vec3 const &right, float neg_x_extents, float pos_x_extents, 
   vec3 const &up, float neg_y_extents, float pos_y_extents, 
   rgba_fl const &color ) 
{
   vec3 bl = origin + neg_x_extents * right + neg_y_extents * up;
   vec3 tl = origin + neg_x_extents * right + pos_y_extents * up;
   vec3 br = origin + pos_x_extents * right + neg_y_extents * up;
   vec3 tr = origin + pos_x_extents * right + pos_y_extents * up;

   vertex_t vertices[] = {
      vertex_t( bl, vec2( 0.0f, 1.0f ), color ), 
      vertex_t( tr, vec2( 1.0f, 0.0f ), color ), 
      vertex_t( tl, vec2( 0.0f, 0.0f ), color ), 
      vertex_t( bl, vec2( 0.0f, 1.0f ), color ), 
      vertex_t( br, vec2( 1.0f, 1.0f ), color ), 
      vertex_t( tr, vec2( 1.0f, 0.0f ), color ),
   };

   draw_vertex_array( PRIMITIVE_TRIANGLES, vertices, 6 );
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
