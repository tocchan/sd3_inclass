#pragma once
#if !defined( __GAME_SIMPLERENDERER__ )
#define __GAME_SIMPLERENDERER__

/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "core/types.h"

#include "render/rhi/rhidevice.h"
#include "render/rhi/rhidevicecontext.h"
#include "render/rhi/rhioutput.h"
#include "render/rhi/blendstate.h"
#include "render/rhi/depthstencilstate.h"
#include "render/rhi/rasterstate.h"
#include "render/rhi/constantbuffer.h"
#include "render/vertex.h"
#include "render/matrix.h"

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
#define MATRIX_BUFFER_INDEX (0)
#define TIME_BUFFER_INDEX (1)
// 2 is going to be LIGHTING
// 3 is going to be ANIMATION [AES]

// 4 and on, User Defined;

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
struct matrix_buffer_t
{
   mat44 model;
   mat44 view;
   mat44 projection;
};

struct time_buffer_t 
{
   float game_time;
   float system_time;
   float game_delta_time;
   float system_delta_time;
};

struct light_buffer_t
{
   rgba_fl ambient;  // <r, g, b, intensity>

   // DIRECTIONAL LIGHT
   rgba_fl dir_light_color;
   vec4 light_direction;

   // POINT LIGHT
   rgba_fl light_color; // <r, g, b, intensity>
   vec4 light_position;
   vec4 attenuation;
};

struct blend_state_t
{
   bool enabled;
   eBlendFactor src_factor;
   eBlendFactor dst_factor;
};

// LIGHTS
// ANIMATION


/************************************************************************/
/*                                                                      */
/* CLASSES                                                              */
/*                                                                      */
/************************************************************************/

class ShaderProgram;
class Sampler;
class VertexBuffer;
class IndexBuffer;

// State Based Forward Renderer
// [A02-A06]
class SimpleRenderer
{
   public:
      SimpleRenderer();
      ~SimpleRenderer() {}

      // [A02] However you want this to initialize, is up to you
      // For example, all of the following are perfectly good ways to expect
      // this to be used;
      // void setup( RHIOutput *output );
      void setup( uint width, uint height );
      void set_title( char const *new_title );

      // void setup( Window *window );
      // void setup( Texture2D *default_render_target );

      // Cleanup - could just be the deconstructor.
      void destroy();

      void update( float delta_time );
      void process_messages(); 
      bool is_closed() const; 

      // RENDER TARGETS
      // [A02]
      // Setting nullptr should default to the output. 
      void set_render_target( Texture2D *color_target, Texture2D *depth_target = nullptr );

      // [A02]
      void set_viewport( uint x, uint y, uint width, uint height );

      void set_model_matrix( mat44 const &model );
      void set_view_matrix( mat44 const &view );
      void set_projection_matrix( mat44 const &proj );

      void set_ortho_projection( vec2 const &bottom_left, vec2 const &top_right );
      void set_perspective_projection( float const fov_radians, float aspect_ratio, float const nz, float const fz );

      void enable_blend( eBlendFactor src, eBlendFactor dest );
      void disable_blend();

      void enable_depth_test( bool enable );
      void enable_depth_write( bool enable );

      // [A02] CLEARING 
      // Clears currently bound target
      void clear_color( rgba_fl const &color ); 
      void clear_depth( float depth = 1.0f, 
         uint8_t stencil = 0 );

      // Clears specified target
      void clear_target_color( Texture2D *target, rgba_fl const &color );

      // [A02] CLEARING 
      // Calls Outputs Present
      void present();
      
      // [A02]
      void set_shader( ShaderProgram *shader );
    
      // [A02]
      void set_texture2d( uint texture_index, Texture2D *texture );
      inline void set_texture2d( Texture2D *texture ) { set_texture2d( 0, texture ); }

      // [A02]
      void set_sampler( uint sampler_index, Sampler *sampler );
      inline void set_sampler( Sampler *sampler ) { set_sampler( 0, sampler ); }

      void set_constant_buffer( uint idx, ConstantBuffer *cb );



      // [A02]
      // Indexed is optional - though highly encouraged to do 
      // during this assignment as it is the easiest assignment to 
      // implement them with
      void draw( ePrimitiveType topology, VertexBuffer *vbo, uint const vertex_count );
      void draw_vertex_array( ePrimitiveType topology, vertex_t const *vbo, uint const vertex_count );

      void draw_quad2d( vec2 const &lower_left, 
         vec2 const &top_right, 
         rgba_fl const &color = rgba_fl::WHITE );

      void SimpleRenderer::draw_quad3d( vec3 const &origin, 
         vec3 const &right, float neg_x_extents, float pos_x_extents, 
         vec3 const &up, float neg_y_extents, float pos_y_extents, 
         rgba_fl const &color = rgba_fl::WHITE );

      void draw_line( vec3 const &p0, 
         vec3 const &p1, 
         rgba_fl const &color = rgba_fl::WHITE );

   public:
      RHIDevice *rhi_device;
      RHIDeviceContext *rhi_context;
      RHIOutput *rhi_output;

      // BASIC SHADERS
      ShaderProgram *color_shader;
      ShaderProgram *unlit_shader;
      ShaderProgram *light_shader;

      // BASIC SAMPLERS
      Sampler *point_sampler; // wrapped point
      Sampler *linear_sampler; // linear sampling

      // DEFAUL TEXTURES
      Texture2D *white_texture;

      // RENDER TARGETS
      Texture2D *default_color_target;
      Texture2D *current_color_target;

      Texture2D *default_depth_stencil;
      Texture2D *current_depth_stencil;

      // STATE MANAGEMENT
      RasterState *default_raster_state;

      blend_state_t blend_state;
      BlendState *current_blend_state;

      depth_stencil_desc_t depth_stencil_desc;
      DepthStencilState *depth_stencil_state;

      // CONSTANT BUFFERS
      matrix_buffer_t matrix_data;
      ConstantBuffer *matrix_cb;

      time_buffer_t time_data;
      ConstantBuffer *time_cb;

      // DATA FOR TEMP MESH CREATION
      VertexBuffer *temp_vbo;
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
