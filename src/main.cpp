/** 
 * File is common entry point and startup code for all projects.  
 * Project main contains the actual entry point once standard startup and tests are run.  
 *
 */


/************************************************************************/
/*                                                                      */
/* INCLUDES                                                             */
/*                                                                      */
/************************************************************************/
#include "core/types.h"

#include "game/game_config.h"
#include "game/game.h"


/************************************************************************/
/*                                                                      */
/* TYPES AND ENUMS                                                      */
/*                                                                      */
/************************************************************************/


/************************************************************************/
/*                                                                      */
/* MACROS                                                               */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* LOCAL GLOBALS                                                        */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* LOCAL FUNCTIONS                                                      */
/*                                                                      */
/************************************************************************/


/************************************************************************/
/*                                                                      */
/* GLOBAL FUNCTIONS                                                     */
/*                                                                      */
/************************************************************************/

//------------------------------------------------------------------------
// Platform Specific Entrance Point
int WINAPI WinMain( HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int show_cmd )
{
   UNREFERENCED_PARAMETER(instance);
   UNREFERENCED_PARAMETER(prev_instance);
   UNREFERENCED_PARAMETER(cmd_line);
   UNREFERENCED_PARAMETER(show_cmd);

   // Start Game
   Game game;
   game.run();
   
   return 0;
}
