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

      // [A02]
      // Indexed is optional - though highly encouraged to do 
      // during this assignment as it is the easiest assignment to 
      // implement them with
      void draw( ePrimitiveType topology, VertexBuffer *vbo, uint const vertex_count );

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
