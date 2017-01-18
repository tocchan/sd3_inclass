#pragma once
#if !defined( __CORE_PLATFORM_WINDOW__ )
#define __CORE_PLATFORM_WINDOW__

/**
 A window a physical area I can draw too.  Should be encompassed by a 
 display (On windows this isn't a "forced" rule, but it'll handle the 
 cases when it is not - usually at a performance penalty)
**/

/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "core/types.h"
#include "core/bit.h"

#include <string>

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
enum eWindowFlag : uint
{
   WNDFLAG_OPEN       = BIT_FLAG(0),
   WNDFLAG_VISIBLE    = BIT_FLAG(1),
   WNDFLAG_FOCUSED    = BIT_FLAG(2),
   WNDFLAG_LOCKED     = BIT_FLAG(3), // used internally to prevent updates during transitions
};


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
class Window
{
   public:
      Window();
      ~Window();

      void set_client_position( uint x, uint y, uint width, uint height );

      bool open();
      void close();

      void show( bool activate = true );
      void hide();

      void focus();

      void set_title( char const *title );
      char const* get_title() const { return title.c_str(); }

      uint process_messages();

      uint get_width() { return px_actual_width; }
      uint get_height() { return px_actual_height; }

      inline bool is_open() const { return IS_BIT_FLAG_SET(flags, WNDFLAG_OPEN); }
      inline bool is_closed() const { return !is_open(); }

      inline bool is_visible() const { return IS_BIT_FLAG_SET(flags, WNDFLAG_VISIBLE); }
      inline bool is_hidden() const { return !is_visible(); }

      inline bool has_focus() const { return IS_BIT_FLAG_SET(flags, WNDFLAG_FOCUSED); }
      inline bool is_locked() const { return IS_BIT_FLAG_SET(flags, WNDFLAG_LOCKED); }

   public:
      // Windows Specfic
      HWND hwnd;

      int px_x;
      int px_y;
      int px_windowed_width;
      int px_windowed_height;

      // When in fullscreen borderless - this changes.
      int px_actual_width;
      int px_actual_height;

      uint flags;

      std::string title;
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
