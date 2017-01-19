/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "game.h"

// A01
#include "core/types.h"
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

   renderer.draw( PRIMITIVE_TRIANGLES, tri_vbo, 3 );

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
   my_shader = renderer.rhi_device->create_shader_from_hlsl_file( "hlsl/nop_color.hlsl" );

   // Create vertices
   vertex_t vertices[3] = {
      vertex_t( vec3( -0.5f, -0.5f, 0.0f ) ), 
      vertex_t( vec3(  0.0f,  0.5f, 0.0f ) ), 
      vertex_t( vec3(  0.5f, -0.5f, 0.0f ) ),
   };
   
   tri_vbo = renderer.rhi_device->create_vertex_buffer( vertices, 3 );
}

//------------------------------------------------------------------------
void Game::cleanup_rendering()
{
   // delete vb
   delete tri_vbo;

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
