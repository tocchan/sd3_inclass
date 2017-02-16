/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "game.h"

#include "core/types.h"
#include "core/command.h"
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
   CommandRun( "help one two three" );

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
   // Update my clock
   float time = (float)TimeGetSeconds();
   delta_time = time - prev_frame_time;
   prev_frame_time = time;
   
   // Process Window Messages
   renderer.update(delta_time);

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
   renderer.clear_color( 0x102040ffU );
   renderer.clear_depth();

   renderer.set_viewport( 0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT );
   renderer.set_shader( renderer.unlit_shader );

   mat44 camera;

   // Get are camera transform
   float t = (float)TimeGetSeconds();
   float radius = 2.0f;

   // uncomment to make camera move in a circle.  Currently it just goes back and forth
   vec3 cam_pos = vec3( radius * cosf(.2f * t), 1.0f, -1.0f /*-radius * sinf(.2f * t)*/ );
   camera = MatrixMakeLookAt( cam_pos, vec3( 0.0f, 0.0f, 3.0f ) );

   float light_radius = 4.0f;
   vec3 light_pos = vec3( light_radius * cosf(t), 0.0f, 0.0f /*-light_radius * sinf(t)*/ );

   // renderer.set_projection_matrix( mat44::IDENTITY );
   // renderer.set_ortho_projection( vec2(aspect_ratio * -5.0f, -5.0f), vec2(aspect_ratio * 5.0f, 5.0f) );
   float aspect_ratio = 1280.0f / 720.0f;
   renderer.set_camera_matrix( camera );
   renderer.set_perspective_projection( D2R(60.0f), aspect_ratio, 0.1f, 100.0f );

   renderer.enable_depth( true, true );

   // Draw my basis
   renderer.set_shader( renderer.color_shader );
   renderer.draw_line( vec3( 0.0f, 0.0f, 0.0f ), vec3( 1.0f, 0.0f, 0.0f ), rgba_fl::RED );
   renderer.draw_line( vec3( 0.0f, 0.0f, 0.0f ), vec3( 0.0f, 1.0f, 0.0f ), rgba_fl::GREEN );
   renderer.draw_line( vec3( 0.0f, 0.0f, 0.0f ), vec3( 0.0f, 0.0f, 1.0f ), rgba_fl::BLUE );

   // draw a visualization of the point
   renderer.draw_point( light_pos, rgba_fl::YELLOW );

   // Draw two quads
   renderer.set_ambient_light( .2f );
   renderer.enable_point_light( light_pos, rgba_fl::WHITE, 4.0f );
   renderer.set_shader( renderer.light_shader );

   // set textures
   renderer.set_diffuse( diffuse_texture );
   renderer.set_normal( normal_texture );
   renderer.set_specular_constants( 32.0f, 1.0f ); 


   // Absolutely horrible way to draw a cube, but doing it for brevity
   // Front
   renderer.draw_quad3d( vec3(0.0f, 0.0f, 3.0f), 
      vec3(1.0f, 0.0f, 0.0f), -1.0f, 1.0f,
      vec3(0.0f, 1.0f, 0.0f), -1.0f, 1.0f );

   // Back
   renderer.draw_quad3d( vec3(0.0f, 0.0f, 5.0f), 
      vec3(-1.0f, 0.0f, 0.0f), -1.0f, 1.0f,
      vec3(0.0f, 1.0f, 0.0f), -1.0f, 1.0f );

   // Left
   renderer.draw_quad3d( vec3(-1.0f, 0.0f, 4.0f), 
      vec3(0.0f, 0.0f,-1.0f), -1.0f, 1.0f,
      vec3(0.0f, 1.0f, 0.0f), -1.0f, 1.0f );

   // Right
   renderer.draw_quad3d( vec3( 1.0f, 0.0f, 4.0f), 
      vec3(0.0f, 0.0f, 1.0f), -1.0f, 1.0f,
      vec3(0.0f, 1.0f, 0.0f), -1.0f, 1.0f );

   // Top
   renderer.draw_quad3d( vec3(0.0f, 1.0f, 4.0f), 
      vec3(1.0f, 0.0f, 0.0f), -1.0f, 1.0f,
      vec3(0.0f, 0.0f, 1.0f), -1.0f, 1.0f );

   // Bottom
   renderer.draw_quad3d( vec3(0.0f, -1.0f, 3.0f), 
      vec3(1.0f, 0.0f, 0.0f), -1.0f, 1.0f,
      vec3(0.0f, 0.0f,-1.0f), -1.0f, 1.0f );


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
   diffuse_texture = new Texture2D( renderer.rhi_device, "image/stone_diffuse.png" );
   normal_texture = new Texture2D( renderer.rhi_device, "image/stone_normal.png" );
}

//------------------------------------------------------------------------
void Game::cleanup_rendering()
{
   // delete vb
   SAFE_DELETE(diffuse_texture);
   SAFE_DELETE(normal_texture);

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
