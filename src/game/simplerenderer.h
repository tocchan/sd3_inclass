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
   float game_frame_time;
   float system_frame_time;
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

      void process_messages(); 
      bool is_closed() const; 

      // RENDER TARGETS
      // [A02]
      // Setting nullptr should default to the output. 
      void set_render_target( Texture2D *color_target );

      // [A02]
      void set_viewport( uint x, uint y, uint width, uint height );

      // [A02: Optional]
      // Set the viewport using percentages of the size of the current render target.
      void set_viewport_as_percent( float x, float y, float w, float h ) 
      {
         UNREFERENCED_PARAMETER(x);
         UNREFERENCED_PARAMETER(y); 
         UNREFERENCED_PARAMETER(w);
         UNREFERENCED_PARAMETER(h);
      }

      void set_projection_matrix( mat44 const &proj );
      void set_ortho_projection( vec2 const &bottom_left, vec2 const &top_right );
      void set_perspective_projection( float const fov_radians, float aspect_ratio, float const nz, float const fz );

      void enable_blend( eBlendFactor src, eBlendFactor dest );
      void disable_blend();

      // [A02] CLEARING 
      // Clears currently bound target
      void clear_color( rgba_fl const &color ); 

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

      void draw_indexed( ePrimitiveType topology, VertexBuffer *vbo, IndexBuffer *ibo, uint const vertex_count ) 
      {
         UNREFERENCED_PARAMETER(topology);
         UNREFERENCED_PARAMETER(ibo);
         UNREFERENCED_PARAMETER(vbo);
         UNREFERENCED_PARAMETER(vertex_count);
      }

   public:
      RHIDevice *rhi_device;
      RHIDeviceContext *rhi_context;
      RHIOutput *rhi_output;

      Texture2D *current_target;

      RasterState *default_raster_state;

      blend_state_t blend_state;
      BlendState *current_blend_state;

      matrix_buffer_t matrix_data;
      ConstantBuffer *matrix_cb;

      time_buffer_t time_data;
      ConstantBuffer *time_cb;

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
