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
};

//------------------------------------------------------------------------
void Game::run_frame()
{
   // Process Window Messages
   renderer.process_messages();

   float dt = 0.01f;
   time.time += dt;
   time_constants->update( renderer.rhi_context, &time );

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

   renderer.set_ortho_projection( vec2(0.0f, 0.0f), vec2( 1280.0f, 720.0f ) );
   renderer.draw( PRIMITIVE_TRIANGLES, quad_vbo, 6 );

   renderer.enable_blend( BLEND_ONE, BLEND_ONE );
   renderer.draw_quad2d( vec2( 50.0f, 50.0f), vec2( 740.0f, 460.0f ) );
   renderer.disable_blend();

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
   my_shader = renderer.rhi_device->create_shader_from_hlsl_file( "hlsl/ortho_textured.hlsl" );

   // Create vertices
   vertex_t vertices[] = {
      vertex_t( vec3(  0.0f,    0.0f, 0.0f ),   vec2(0.0f, 1.0f) ), 
      vertex_t( vec3(  100.0f,  100.0f, 0.0f ), vec2(1.0f, 0.0f) ),
      vertex_t( vec3(  0.0f,    100.0f, 0.0f ), vec2(0.0f, 0.0f) ),
      vertex_t( vec3(  0.0f,    0.0f, 0.0f ),   vec2(0.0f, 1.0f) ),
      vertex_t( vec3(  100.0f,  0.0f, 0.0f ),   vec2(1.0f, 1.0f) ),
      vertex_t( vec3(  100.0f,  100.0f, 0.0f ), vec2(1.0f, 0.0f) ),
   };
   
   quad_vbo = renderer.rhi_device->create_vertex_buffer( vertices, 6 );

   // Create Resources
   point_sampler = new Sampler( renderer.rhi_device, FILTER_POINT, FILTER_POINT );
   tex_sample = new Texture2D( renderer.rhi_device, "image/xenoblade.jpg" );

   
   time.time = 0.0f;
   time_constants = new ConstantBuffer( renderer.rhi_device, &time, sizeof(time) );
}

//------------------------------------------------------------------------
void Game::cleanup_rendering()
{
   // delete vb
   SAFE_DELETE(quad_vbo);

   SAFE_DELETE(point_sampler);
   SAFE_DELETE(tex_sample);
   SAFE_DELETE(my_shader);
   SAFE_DELETE(time_constants);


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
