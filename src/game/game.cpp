/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "game.h"

// A01
#include "core/types.h"
#include "core/image.h"
#include "core/time.h"
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

   prev_frame_time = (float)TimeGetSeconds();
};

//------------------------------------------------------------------------
void Game::run_frame()
{
   // Process Window Messages
   renderer.update(delta_time);

   // Update my clock
   float time = (float)TimeGetSeconds();
   delta_time = time - prev_frame_time;
   prev_frame_time = time;

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
   renderer.clear_depth();
   renderer.enable_depth_test( true );

   renderer.set_viewport( 0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT );

   renderer.set_shader( renderer.unlit_shader );

   /*
   renderer.set_ortho_projection( vec2(0.0f, 0.0f), vec2( 100.0f, 100.0f ) );
   renderer.draw( PRIMITIVE_TRIANGLES, quad_vbo, 6 );

   renderer.set_texture2d( tex_particle );
   renderer.enable_blend( BLEND_SRC_ALPHA, BLEND_INV_SRC_ALPHA );
   renderer.draw_quad2d( vec2( 50.0f, 50.0f), vec2( 100.0f, 100.0f ) );
   renderer.disable_blend();
   */

   float aspect_ratio = 1280.0f / 720.0f;
   aspect_ratio; 

   mat44 camera;

   // Get are camera transform
   float t = (float)TimeGetSeconds();
   float radius = 5.0f;
   vec3 cam_pos = vec3( radius * cosf(t), 2.0f, -radius * sinf(t) );
   camera = MatrixMakeLookAt( cam_pos, vec3( 0.0f, 0.0f, 0.0f ) );

   // renderer.set_projection_matrix( mat44::IDENTITY );
   // renderer.set_ortho_projection( vec2(aspect_ratio * -5.0f, -5.0f), vec2(aspect_ratio * 5.0f, 5.0f) );
   renderer.set_view_matrix( camera.get_inverse_orthonormal() );
   renderer.set_perspective_projection( D2R(60.0f), aspect_ratio, 0.1f, 100.0f );

   renderer.set_shader( renderer.color_shader );
   renderer.draw_line( vec3( 0.0f, 0.0f, 0.0f ), vec3( 1.0f, 0.0f, 0.0f ), rgba_fl::RED );
   renderer.draw_line( vec3( 0.0f, 0.0f, 0.0f ), vec3( 0.0f, 1.0f, 0.0f ), rgba_fl::GREEN );
   renderer.draw_line( vec3( 0.0f, 0.0f, 0.0f ), vec3( 0.0f, 0.0f, 1.0f ), rgba_fl::BLUE );

   renderer.set_shader( renderer.unlit_shader );
   renderer.set_texture2d( diffuse_texture );
   renderer.draw_quad3d( vec3(0.0f, 0.0f, 3.0f), 
      vec3(1.0f, 0.0f, 0.0f), -1.0f, 1.0f,
      vec3(0.0f, 1.0f, 0.0f), -1.0f, 1.0f );

   renderer.draw_quad3d( vec3(2.0f, 0.0f, 5.0f), 
      vec3(1.0f, 0.0f, 0.0f), -1.0f, 1.0f,
      vec3(0.0f, 1.0f, 0.0f), -1.0f, 1.0f );

   renderer.present();
}

//------------------------------------------------------------------------
// Assignment 01
void Game::init_rendering()
{
   uint width = DEFAULT_WINDOW_WIDTH;
   uint height = DEFAULT_WINDOW_HEIGHT;

   renderer.setup( width, height );

   // Create Resources
   diffuse_texture = new Texture2D( renderer.rhi_device, "image/xenoblade.jpg" );
}

//------------------------------------------------------------------------
void Game::cleanup_rendering()
{
   // delete vb
   SAFE_DELETE(diffuse_texture);

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
