#pragma once
#if !defined( __CORE_COMMAND__ )
#define __CORE_COMMAND__

/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "core/types.h"

// If using commands - I disable this warning - since arguments may or may not be used.
#pragma warning( disable : 4100 )   // Disable "unreferenced formal parameter"


/************************************************************************/
/*                                                                      */
/* DEFINES AND CONSTANTS                                                */
/*                                                                      */
/************************************************************************/
/** USE CASE **
COMMAND( help, "Displays all commands." )
{
   // impl
}
** END USE CASE **/

// 1. Predefine (locally) the method that embodies the commands work
// 2. Create a self register method that takes a pointer to the above function
// Start the definition for the function, as the next thing the user types is the function body
#define COMMAND( name, help_text )                        \
   static void AutoCommand_##name##( char const* );       \
   static CommandSelfRegister gCommandRegister_##name##( #name, AutoCommand_##name##, help_text ); \
   static void AutoCommand_##name##( char const *arguments )


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

// I actually use a helper object as the arguments variable
// so as to make parsing it easier in my actual engine - but for brevity
// we'll just pass the string past the command name
typedef void (*command_cb)( char const *arguments );

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
class CommandSelfRegister
{
   public:
      CommandSelfRegister( char const *id, command_cb cb, char const *help_text = nullptr );
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

bool CommandRegister( char const *id, command_cb cb, char const *help_text = nullptr );
bool CommandRun( char const *str );

#endif 
