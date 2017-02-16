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
   , current_color_target(nullptr) 
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

   rhi_output->window->set_title( "GUILDHALL : SD3 ASSIGNMENT 4" );

   default_raster_state = new RasterState( rhi_device );
   rhi_context->set_raster_state( default_raster_state );

   matrix_data.eye_position = vec3( 0.0f );
   MemZero( &time_data );

   matrix_cb = new ConstantBuffer( rhi_device, &matrix_data, sizeof(matrix_data) );
   time_cb = new ConstantBuffer( rhi_device, &time_data, sizeof(time_data) );
   light_cb = new ConstantBuffer( rhi_device, &light_data, sizeof(light_data) );

   // BIND constant buffers
   set_constant_buffer( MATRIX_BUFFER_INDEX, matrix_cb );
   set_constant_buffer( TIME_BUFFER_INDEX, time_cb );
   set_constant_buffer( LIGHT_BUFFER_INDEX, light_cb );

   temp_vbo = new VertexBuffer( rhi_device, nullptr, 32, BUFFERUSAGE_DYNAMIC );

   // Shaders
   color_shader = new ShaderProgram( rhi_device, "hlsl/color.hlsl" );
   unlit_shader = new ShaderProgram( rhi_device, "hlsl/unlit.hlsl" );
   light_shader = new ShaderProgram( rhi_device, "hlsl/light.hlsl" );
   
   // white texture
   Image image;
   image.create_clear( 1, 1, rgba_fl::WHITE );
   white_texture = new Texture2D( rhi_device, image );

   // create a default normal texture
   image.create_clear( 1, 1, rgba_fl(.5f, .5f, 1.0f, 1.0f) );
   flat_normal_texture = new Texture2D( rhi_device, image );

   // point sampler
   point_sampler = new Sampler( rhi_device, FILTER_POINT, FILTER_POINT );
   linear_sampler = new Sampler( rhi_device, FILTER_LINEAR, FILTER_LINEAR );

   // set initial color target
   default_color_target = rhi_output->get_render_target();

   // Just create a default depth stencil of your back buffers width & height
   default_depth_stencil = new Texture2D( rhi_device, width, height, IMAGEFORMAT_D24S8 );
   current_depth_stencil = nullptr;

   // Set initial depth stencil
   depth_stencil_desc.depth_test_enabled = false;
   depth_stencil_desc.depth_writing_enabled = true;
   depth_stencil_state = new DepthStencilState( rhi_device, depth_stencil_desc );
   rhi_context->set_depth_stencil_state( depth_stencil_state );

   set_render_target(nullptr, nullptr);
   set_sampler(nullptr);
   set_texture2d(nullptr);
}

//------------------------------------------------------------------------
void SimpleRenderer::destroy() 
{
   rhi_context->clear_state();

   SAFE_DELETE(light_shader);
   SAFE_DELETE(unlit_shader);
   SAFE_DELETE(color_shader);

   SAFE_DELETE(point_sampler);
   SAFE_DELETE(linear_sampler);

   SAFE_DELETE(white_texture);
   SAFE_DELETE(flat_normal_texture);

   SAFE_DELETE(current_blend_state);
   SAFE_DELETE(default_raster_state);
   SAFE_DELETE(depth_stencil_state);

   SAFE_DELETE(light_cb);
   SAFE_DELETE(time_cb);
   SAFE_DELETE(matrix_cb);

   SAFE_DELETE(temp_vbo);

   SAFE_DELETE(default_depth_stencil);

   delete rhi_output;
   delete rhi_context;
   delete rhi_device;

   RHIInstance::ReportLiveObjects();
}

//------------------------------------------------------------------------
void SimpleRenderer::set_title( char const *new_title ) 
{
   rhi_output->window->set_title( new_title );
}



//------------------------------------------------------------------------
void SimpleRenderer::set_render_target( Texture2D *color_target,
   Texture2D *depth_target )
{
   if (color_target != nullptr) {
      if (color_target->is_render_target()) {
         current_color_target = color_target; 
      } // else, WTH?
   } else {
      current_color_target = default_color_target;
   }

   if (depth_target == nullptr) {
      depth_target = default_depth_stencil;
   }

   current_depth_stencil = depth_target;
   rhi_context->set_color_target( current_color_target, depth_target );
}

//------------------------------------------------------------------------
void SimpleRenderer::set_viewport( uint x, uint y, uint w, uint h ) 
{
   rhi_context->set_viewport( x, y, w, h );
}

//------------------------------------------------------------------------
void SimpleRenderer::update( float delta_time ) 
{
   process_messages();

   time_data.game_delta_time = delta_time;
   time_data.system_delta_time = delta_time;
   time_data.game_time += delta_time;
   time_data.system_time += delta_time;

   time_cb->update( rhi_context, &time_data );
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
void SimpleRenderer::set_model_matrix( mat44 const &model )
{
   matrix_data.model = model.get_transpose();
   matrix_cb->update( rhi_context, &matrix_data );
}

//------------------------------------------------------------------------
void SimpleRenderer::set_view_matrix( mat44 const &view )
{
   matrix_data.view = view.get_transpose();
   matrix_cb->update( rhi_context, &matrix_data );
}

//------------------------------------------------------------------------
void SimpleRenderer::set_projection_matrix( mat44 const &proj )
{
   matrix_data.projection = proj.get_transpose();
   matrix_cb->update( rhi_context, &matrix_data );
}


//------------------------------------------------------------------------
void SimpleRenderer::set_camera_matrix( mat44 const &camera )
{
   vec3 position = camera.get_translation();
   matrix_data.eye_position = position;

   // this will also update the buffer
   set_view_matrix( camera.get_inverse_orthonormal() );
}

//------------------------------------------------------------------------
void SimpleRenderer::set_eye_position( vec3 const &eye_position )
{
   matrix_data.eye_position = eye_position;
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
void SimpleRenderer::enable_depth_test( bool enable )
{
   // deleting these each change is inefficient - honesty the state should be cached off
   // since it is likely it will need to be set the same way next frame.  
   // Ideally this would be stored with the shader imo.
   if (depth_stencil_desc.depth_test_enabled != enable) {
      depth_stencil_desc.depth_test_enabled = enable;
      SAFE_DELETE( depth_stencil_state );
      depth_stencil_state = new DepthStencilState( rhi_device, depth_stencil_desc );
      rhi_context->set_depth_stencil_state( depth_stencil_state );
   }
}

//------------------------------------------------------------------------
void SimpleRenderer::enable_depth_write( bool enable )
{
   if (depth_stencil_desc.depth_writing_enabled != enable) {
      depth_stencil_desc.depth_writing_enabled = enable;
      SAFE_DELETE( depth_stencil_state );
      depth_stencil_state = new DepthStencilState( rhi_device, depth_stencil_desc );
      rhi_context->set_depth_stencil_state( depth_stencil_state );
   }
}

//------------------------------------------------------------------------
void SimpleRenderer::enable_depth( bool test, bool write )
{
   if ((depth_stencil_desc.depth_writing_enabled != write)
      || (depth_stencil_desc.depth_test_enabled != test)) {

      depth_stencil_desc.depth_writing_enabled = write;
      depth_stencil_desc.depth_test_enabled = test;
      SAFE_DELETE( depth_stencil_state );
      depth_stencil_state = new DepthStencilState( rhi_device, depth_stencil_desc );
      rhi_context->set_depth_stencil_state( depth_stencil_state );
   }
} 

//------------------------------------------------------------------------
void SimpleRenderer::clear_color( rgba_fl const &color ) 
{
   rhi_context->clear_color_target( current_color_target, color );
}

//------------------------------------------------------------------------
void SimpleRenderer::clear_depth( float depth, uint8_t stencil )
{
   rhi_context->clear_depth_target( current_depth_stencil, depth, stencil );
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
   // by default, use a white texture
   if (tex == nullptr) {
      tex = white_texture;
   }

   rhi_context->set_texture2d( tex_index, tex );
}

//------------------------------------------------------------------------
void SimpleRenderer::set_sampler( uint samp_index, Sampler *samp ) 
{
   // by default, use linear sampling
   if (samp == nullptr) {
      samp = linear_sampler; 
   }

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
void SimpleRenderer::set_ambient_light( float intensity, rgba_fl const &color /*= rgba_fl::WHITE*/ )
{
   light_data.ambient = rgba_fl( color.r, color.g, color.b, intensity );
   light_cb->update( rhi_context, &light_data );
}

//------------------------------------------------------------------------
void SimpleRenderer::enable_point_light( vec3 const &pos
   , rgba_fl const &color
   , float intensity /*= 1.0f*/
   , vec3 const &attenuation /*= vec3( 0, 0, 1 )*/
   , vec3 const &spec_attenuation /*= vec3( 0, 0, 1 )*/ )
{
   light_data.light_position = vec4( pos, 1.0f );
   light_data.light_color = rgba_fl( color.r, color.g, color.b, intensity );
   light_data.attenuation = attenuation;
   light_data.spec_attenuation = spec_attenuation;

   light_cb->update( rhi_context, &light_data );
}

//------------------------------------------------------------------------
void SimpleRenderer::set_specular_constants( float const spec_power
   , float const spec_factor /*= 1.0f*/ )
{
   light_data.spec_factor = spec_factor;
   light_data.spec_power = spec_power;

   light_cb->update( rhi_context, &light_data );
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

   vec3 normal = up.cross(right);

   vertex_t vertices[] = {
      vertex_t( bl, normal, vec2( 0.0f, 1.0f ), color ), 
      vertex_t( tr, normal, vec2( 1.0f, 0.0f ), color ), 
      vertex_t( tl, normal, vec2( 0.0f, 0.0f ), color ), 
      vertex_t( bl, normal, vec2( 0.0f, 1.0f ), color ), 
      vertex_t( br, normal, vec2( 1.0f, 1.0f ), color ), 
      vertex_t( tr, normal, vec2( 1.0f, 0.0f ), color ),
   };

   draw_vertex_array( PRIMITIVE_TRIANGLES, vertices, 6 );
}

//------------------------------------------------------------------------
void SimpleRenderer::draw_line( vec3 const &p0, 
   vec3 const &p1, 
   rgba_fl const &color /*= rgba_fl::WHITE*/ )
{
   
   vertex_t vertices[] = {
      vertex_t( p0, vec2( 0.0f, 0.0f ), color ), 
      vertex_t( p1, vec2( 1.0f, 1.0f ), color ), 
   };

   draw_vertex_array( PRIMITIVE_LINES, vertices, 2 );
}

//------------------------------------------------------------------------
void SimpleRenderer::draw_point( vec3 const &p, rgba_fl const &color /*= rgba_fl::WHITE*/ )
{
   vec3 r = vec3( 0.2f, 0.0f, 0.0f );
   vec3 u = vec3( 0.0f, 0.2f, 0.0f );
   vec3 f = vec3( 0.0f, 0.0f, 0.2f );

   vertex_t vertices[] = {
      vertex_t( p - r, vec2( 0.0f, 0.0f ), color ), 
      vertex_t( p + r, vec2( 0.0f, 0.0f ), color ), 
      vertex_t( p - u, vec2( 0.0f, 0.0f ), color ), 
      vertex_t( p + u, vec2( 0.0f, 0.0f ), color ), 
      vertex_t( p - f, vec2( 0.0f, 0.0f ), color ), 
      vertex_t( p + f, vec2( 0.0f, 0.0f ), color ), 
   };

   draw_vertex_array( PRIMITIVE_LINES, vertices, ARRAYSIZE(vertices) );
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
