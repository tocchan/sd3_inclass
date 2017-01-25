/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "game.h"

// A01
#include "core/types.h"
#include "core/image.h"
#include "game/game_config.h"

#include "render/rhi/rhiinstance.h"
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
Game* Game::sInstance = nullptr;

/************************************************************************/
/*                                                                      */
/* LOCAL FUNCTIONS                                                      */
/*                                                                      */
/************************************************************************/

//------------------------------------------------------------------------
static bool GameMessageHandler( Window *wnd, UINT msg, WPARAM wparam, LPARAM lparam ) 
{
   UNREFERENCED(lparam);

   switch (msg) {
      case WM_KEYUP: 
         if (wparam == VK_ESCAPE) {
            wnd->close();
         }

         return false;

      default:
         return false;
   }
}

/************************************************************************/
/*                                                                      */
/* EXTERNAL FUNCTIONS                                                   */
/*                                                                      */
/************************************************************************/

//------------------------------------------------------------------------
Game::Game() 
   : quit_reason(QUITREASON_NONE)
{
   current_state = nullptr;

   sInstance = this;
}

//------------------------------------------------------------------------
Game::~Game()
{
   sInstance = nullptr;
}

//------------------------------------------------------------------------
void Game::run()
{
   start();
   while (is_running()) {
      run_frame();
   }
   end();
}

//------------------------------------------------------------------------
void Game::start()
{
   quit_reason = QUITREASON_NONE;
   init_rendering();

   renderer.rhi_output->window->set_custom_message_handler( GameMessageHandler );

   // Create Resources
   tex_sample = new Texture2D( renderer.rhi_device, "image/xenoblade.jpg" );
   point_sampler = new Sampler( renderer.rhi_device, FILTER_POINT, FILTER_POINT );
};

//------------------------------------------------------------------------
void Game::run_frame()
{
   // Process Window Messages
   renderer.process_messages();

   // update sim
   update_sim();

   // render
   render();
}

//------------------------------------------------------------------------
void Game::end()
{
   cleanup_rendering();
}

//------------------------------------------------------------------------
void Game::quit( eQuitReason reason /*= QUITREASON_USER*/ )
{
   if (!is_quitting()) {
      quit_reason = reason;
   }
}

//------------------------------------------------------------------------
void Game::update_sim()
{
   if (renderer.is_closed()) {
      quit();
      return;
   }
}

//------------------------------------------------------------------------
void Game::render()
{
   renderer.set_render_target( nullptr );
   renderer.clear_color( 0x9288ffff );

   renderer.set_viewport( 0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT );

   renderer.set_shader( my_shader );
   renderer.set_texture2d( tex_sample );
   renderer.set_sampler( point_sampler );

   renderer.draw( PRIMITIVE_TRIANGLES, quad_vbo, 6 );

   renderer.present();
}

//------------------------------------------------------------------------
// Assignment 01
void Game::init_rendering()
{
   uint width = DEFAULT_WINDOW_WIDTH;
   uint height = DEFAULT_WINDOW_HEIGHT;

   renderer.setup( width, height );

   // my_shader = new ShaderProgram( renderer.rhi_device, "hlsl/imageeffect/nop.hlsl" );
   my_shader = renderer.rhi_device->create_shader_from_hlsl_file( "hlsl/nop_grayscale.hlsl" );

   // Create vertices
   vertex_t vertices[] = {
      vertex_t( vec3( -1.0f, -1.0f, 0.0f ), vec2(0.0f, 1.0f) ), 
      vertex_t( vec3(  1.0f,  1.0f, 0.0f ), vec2(1.0f, 0.0f) ),
      vertex_t( vec3( -1.0f,  1.0f, 0.0f ), vec2(0.0f, 0.0f) ),
      vertex_t( vec3( -1.0f, -1.0f, 0.0f ), vec2(0.0f, 1.0f) ),
      vertex_t( vec3(  1.0f, -1.0f, 0.0f ), vec2(1.0f, 1.0f) ),
      vertex_t( vec3(  1.0f,  1.0f, 0.0f ), vec2(1.0f, 0.0f) ),
   };
   
   quad_vbo = renderer.rhi_device->create_vertex_buffer( vertices, 6 );
}

//------------------------------------------------------------------------
void Game::cleanup_rendering()
{
   // delete vb
   delete quad_vbo;
   delete tex_sample;
   delete point_sampler;
   quad_vbo = nullptr;
   tex_sample = nullptr;
   point_sampler = nullptr;

   // delete shader
   delete my_shader;
   my_shader = nullptr;

   // cleanup renderer
   renderer.destroy();
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
