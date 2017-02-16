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
#define LIGHT_BUFFER_INDEX (2)
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
   matrix_buffer_t() 
      : eye_position(0.0f)
   {}

   mat44 model;
   mat44 view;
   mat44 projection;

   vec4 eye_position;
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
   light_buffer_t()
      : ambient(1, 1, 1, 1)
      , light_color(1, 1, 1, 0)
      , light_position(0, 0, 0, 1)
      , attenuation(1, 0, 0, 0)     // will effectively dampen the light - at intensity 1, this will make the light constant
      , spec_attenuation(1, 0, 0, 0)
      , spec_factor(0.0f)
      , spec_power(1.0f)
   {}

   rgba_fl ambient;  // <r, g, b, intensity>

   // POINT LIGHT
   rgba_fl light_color;   // <r, g, b, intensity>
   vec4 light_position;   // <x, y, z, padding>  // variables can not cross a 16-byte boundary, so we pad out 
   vec4 attenuation;      // <constant, linear, quadratic, unused>
   vec4 spec_attenuation; // <constant, linear, quadratic, unused>

   // SURFACE INFORMATION
   float spec_factor;
   float spec_power;
   vec2 unused;
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

      // Setting a camera will set the view and eye position
      void set_camera_matrix( mat44 const &camera ); 
      void set_eye_position( vec3 const &eye_position );

      void set_ortho_projection( vec2 const &bottom_left, vec2 const &top_right );
      void set_perspective_projection( float const fov_radians, float aspect_ratio, float const nz, float const fz );

      void enable_blend( eBlendFactor src, eBlendFactor dest );
      void disable_blend();

      void enable_depth_test( bool enable );
      void enable_depth_write( bool enable );
      void enable_depth( bool test, bool write );

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

      // other aliases
      inline void set_diffuse( Texture2D *texture ) { set_texture2d( 0, (texture == nullptr) ? white_texture : texture ); }
      inline void set_normal( Texture2D *texture ) { set_texture2d( 1, (texture == nullptr) ? flat_normal_texture : texture ); }
      inline void set_spec( Texture2D *texture ) { set_texture2d( 2, (texture == nullptr) ? white_texture : texture ); }


      // [A02]
      void set_sampler( uint sampler_index, Sampler *sampler );
      inline void set_sampler( Sampler *sampler ) { set_sampler( 0, sampler ); }

      void set_constant_buffer( uint idx, ConstantBuffer *cb );

      // LIGHTING
      void set_ambient_light( float intensity, rgba_fl const &color = rgba_fl::WHITE );
      void set_specular_constants( float const spec_power, float const spec_factor = 1.0f );
      
      // Goal of assignment is to support multiple of these
      // cone lights, directional lights, and point lights
      // spec maps, and TBN
      void enable_point_light( vec3 const &pos
         , rgba_fl const &color
         , float intensity = 1.0f
         , vec3 const &attenuation = vec3( 0, 0, 1 )
         , vec3 const &spec_attenuation = vec3( 0, 0, 1 ) );

      // disabling a light is the same as setting it to have no intensity (it won't contribute any light)
      inline void disable_point_light() { enable_point_light( vec3(0.0f), rgba_fl::WHITE, 0.0f, vec3(1, 0, 0) ); }


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
      void draw_point( vec3 const &p, rgba_fl const &color = rgba_fl::WHITE );


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
      Texture2D *flat_normal_texture;

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

      light_buffer_t light_data;
      ConstantBuffer *light_cb;

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
