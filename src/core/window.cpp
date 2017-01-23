
// WS_EX_TOOLWINDOW - prevents window from showing up in taskbar (useful for secondary displays)

// Setting an Icon
// SendMessage( yourwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon );

// Setting a cursor
// ???

// SetWindowsHookEx - can be used to peek at a windows messages.  System seperation...


/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "window.h"

// For DPI Awareness
#include <ShellScalingApi.h>
#pragma comment(lib, "Shcore.lib")

/************************************************************************/
/*                                                                      */
/* DEFINES AND CONSTANTS                                                */
/*                                                                      */
/************************************************************************/
#define DPI_AWARE

static char const*const WNDCLASS_GAME_NAME = "ITWGameWindow";


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
static uint gWindowSystemCount = 0U;

/************************************************************************/
/*                                                                      */
/* GLOBAL VARIABLES                                                     */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* LOCAL FUNCTIONS                                                      */
/*                                                                      */
/************************************************************************/

//------------------------------------------------------------------------
static DWORD InternalGetStyle( bool visible = true )
{
   DWORD ret = 0;
   ret |= (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER);
   
   // if visible
   if (visible) {
      ret |= WS_VISIBLE;
   }

   return ret;
}

//------------------------------------------------------------------------
// Get the rectangle represented by x, y (relative to screen), and width &
// height given a certain style
static RECT InternalGetRect( int x, int y, int width, int height, uint style ) 
{
   RECT out_rect;
   out_rect.left = 0;
   out_rect.top = 0;
   out_rect.right = width;
   out_rect.bottom = height;
   ::AdjustWindowRect( &out_rect, style, FALSE );

   out_rect.left += x;
   out_rect.top += y;
   out_rect.right += x;
   out_rect.bottom += y;

   return out_rect;
}

//------------------------------------------------------------------------
static void SetWindowActive( Window *wnd, bool active ) 
{
   if (nullptr != wnd) {
      if (wnd->has_focus() != active) {
         if (active) {
            SET_BIT_FLAG( wnd->flags, WNDFLAG_FOCUSED );
            // wnd->on_focus.trigger( wnd, true );

         } else {
            UNSET_BIT_FLAG( wnd->flags, WNDFLAG_FOCUSED );
            // wnd->on_blur.trigger( wnd, false );
         }
      }
   }
}


//------------------------------------------------------------------------
static HINSTANCE GetCurrentHINSTANCE()
{
    return GetModuleHandle(NULL);
}

//------------------------------------------------------------------------
static LRESULT CALLBACK GameWndProc( HWND hwnd, 
   UINT msg, 
   WPARAM wparam, 
   LPARAM lparam )
{
   Window *wnd = nullptr;
   wnd = (Window*) GetWindowLongPtr( hwnd, GWLP_USERDATA );

   if ((wnd != nullptr) && (wnd->custom_msgproc != nullptr)) {
      bool supress = wnd->custom_msgproc( wnd, msg, wparam, lparam );
      if (supress) {
         return 1;
      }
   }

   switch (msg) {
      case WM_PAINT: {
            // Ignore WM_PAINT messages
            // [c4] Must handle them otherwise they just keep coming
            PAINTSTRUCT ps;
            BeginPaint( hwnd, &ps );
            EndPaint( hwnd, &ps );
         }   
         return 1;
      
      case WM_ACTIVATE:   {
         wnd = (Window*) GetWindowLongPtr( hwnd, GWLP_USERDATA );
         WORD low = LOWORD( wparam );
         SetWindowActive( wnd, (WA_INACTIVE != low) );
      }
      break;
      
      // Sent before 
      case WM_NCCREATE: {
         #if defined(_WIN32_WINNT_WIN10)
            #if (WINVER >= _WIN32_WINNT_WIN10) 
               // ::EnableNonClientDpiScaling(hwnd);
            #endif
         #endif
      } break;

      case WM_DPICHANGED: {
         // currently not used - but my giant 
         // menu bar is annoying, so keeping this around
         // uint dpi = (uint)LOWORD(wparam);
      } break;

      case WM_CREATE: {
         CREATESTRUCT *cp = (CREATESTRUCT*) lparam;
         wnd = (Window*) cp->lpCreateParams;

         wnd->hwnd = hwnd;
         SetWindowLongPtr( hwnd, GWLP_USERDATA, (LONG_PTR) wnd );
         if (::GetActiveWindow() == hwnd) {
            SetWindowActive(wnd, true);
         }

      } break;

      case WM_SHOWWINDOW: {
         wnd = (Window*) GetWindowLongPtr( hwnd, GWLP_USERDATA );
         bool visible = (wparam == TRUE);
         if (wnd->is_visible() != visible) {
            if (wparam == TRUE) {
               SET_BIT_FLAG( wnd->flags, WNDFLAG_VISIBLE );
               // wnd->on_visible(wnd);
            } else {
               UNSET_BIT_FLAG( wnd->flags, WNDFLAG_VISIBLE );
               // wnd->on_hidden(wnd);
            }
         }
      } break;

      case WM_DESTROY: {
         wnd = (Window*) GetWindowLongPtr( hwnd, GWLP_USERDATA );
         if (nullptr != wnd) {
            wnd->hwnd = NULL;
         }
      } break;

      case WM_INPUT: {
         return 0;
      } break;

      case WM_KEYDOWN: {
      } break;

      case WM_CHAR: {
      } break;

      case WM_STYLECHANGED: {
         // when a style changes it does not update the client size (total size of the window
         // stays constant).  So during this time
         // I prevent updates being fed to the renderer (locked)
      } break;

      // Never do the default action for WM_CLOSE
      case WM_CLOSE: {
         wnd = (Window*) GetWindowLongPtr( hwnd, GWLP_USERDATA );
         wnd->close();
         return 0;
      } 

      case WM_INITMENUPOPUP: 
      // case WM_SYSCOMMAND:
      case WM_ENTERIDLE:
      {
         // Consumes the event
         return 0;
      }

      case WM_MOUSEMOVE:
         break;

      default:
         // Trace( "wnd", "Unhandled Message: 0x%04X [0x%04X, 0x%04X]", msg, wparam, lparam );
         break;

   }

   // Default handling of messages
   return DefWindowProc( hwnd, msg, wparam, lparam );
}


//------------------------------------------------------------------------
static bool RegisterGameWindowClass()
{
   WNDCLASSEX wc;
   memset( &wc, 0, sizeof(wc) );

   // Setup the definition for this window class
   wc.cbSize = sizeof(WNDCLASSEX);

   // This sets that it will redraw for vertical or horizontal changes
   // and it also owns its own device context handle (more effecient if we're
   // drawing to this window a lot).
   wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

   // The Windows Messaeg Pump - handles messages for this window type.
   wc.lpfnWndProc = GameWndProc;
   wc.cbClsExtra = 0;
   wc.cbWndExtra = 0;
   wc.hInstance = GetCurrentHINSTANCE();

   // Default icon for this window (can be set later)
   wc.hIcon = NULL;
   wc.hIconSm = NULL;

   // Cursor to use (can be set later if needed)
   wc.hCursor = ::LoadCursorA( NULL, IDC_ARROW );
   wc.hbrBackground = (HBRUSH) 0;
   wc.lpszMenuName = NULL;

   // Name to use when creating windows so it knows to use this class
   wc.lpszClassName = WNDCLASS_GAME_NAME;

   ATOM result = RegisterClassEx( &wc );
   if (NULL == result) {
      return false;
   }

   return true;
}

//------------------------------------------------------------------------
static void UnregisterWindowClass( char const *class_name )
{
   UnregisterClass( class_name, GetCurrentHINSTANCE() );
}


//------------------------------------------------------------------------
static void WindowSystemAcquire()
{
   uint count = ++gWindowSystemCount;
   if (count == 1U) {
      #if defined(DPI_AWARE)
         ::SetProcessDpiAwareness( PROCESS_PER_MONITOR_DPI_AWARE );
      #endif
      RegisterGameWindowClass();
   }
}

//------------------------------------------------------------------------
static void WindowSystemRelease()
{
   uint count = --gWindowSystemCount;
   if (0U == count) {
      UnregisterWindowClass( WNDCLASS_GAME_NAME );
   }
}

//------------------------------------------------------------------------
static RECT WindowGetRect( Window *wnd )
{
   DWORD style = InternalGetStyle();

   RECT win_rect;
  
   win_rect = InternalGetRect( wnd->px_x, wnd->px_y, 
      wnd->px_windowed_width, wnd->px_windowed_height, 
      style );

   wnd->px_actual_width = wnd->px_windowed_width;
   wnd->px_actual_height = wnd->px_windowed_height;

   return win_rect;
}

//------------------------------------------------------------------------
static bool WindowUpdateStyleAndPosition( Window *wnd )
{
   HWND hwnd = wnd->hwnd;
   if (hwnd != NULL) {
      DWORD style = InternalGetStyle();
      
      // do the window stuff
      SET_BIT_FLAG( wnd->flags, WNDFLAG_LOCKED );
      SetWindowLongPtr( hwnd, GWL_STYLE, style );
      UNSET_BIT_FLAG( wnd->flags, WNDFLAG_LOCKED );
      
      RECT win_rect = WindowGetRect( wnd );
      if (FALSE == ::SetWindowPos( hwnd, NULL, 
         win_rect.left, win_rect.top,
         win_rect.right - win_rect.left,
         win_rect.bottom - win_rect.top, 
         SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED )) {

         return false;
      }
   }

   return true;
}

/************************************************************************/
/*                                                                      */
/* EXTERNAL FUNCTIONS                                                   */
/*                                                                      */
/************************************************************************/
//------------------------------------------------------------------------
Window::Window() 
   : hwnd(INVALID_HANDLE)
   , flags(0) 
{
   WindowSystemAcquire();

   custom_msgproc = nullptr;

   px_x = 200;
   px_y = 200;
   px_windowed_width = 1200;
   px_windowed_height = 720;
   px_actual_width = px_windowed_width;
   px_actual_height = px_windowed_height;
}

//------------------------------------------------------------------------
Window::~Window()
{
   close();
   WindowSystemRelease();
}


//------------------------------------------------------------------------
void Window::set_client_position( uint x, uint y, uint width, uint height ) 
{
   px_windowed_width = (int) width;
   px_windowed_height = (int) height;
   px_x = (int) x;
   px_y = (int) y;
   
   if (is_open()) {
      WindowUpdateStyleAndPosition(this);
   }
}

//------------------------------------------------------------------------
bool Window::open()
{
   if (is_open()) {
      return false;
   }
   
   DWORD extended_style = 0;
   DWORD style = InternalGetStyle( false );

   int res_x, res_y;
   res_x = px_windowed_width;
   res_y = px_windowed_height;

   RECT win_rect = WindowGetRect(this); 

   int x, y;
   int w = win_rect.right - win_rect.left;
   int h = win_rect.bottom - win_rect.top;
   x = win_rect.left;
   y = win_rect.top;
   
   hwnd = CreateWindowExA( extended_style,
      WNDCLASS_GAME_NAME,
      title.c_str(), // title
      style,
      x, y,
      w, h,
      NULL,
      NULL,
      GetCurrentHINSTANCE(),
      this );

   if (NULL == hwnd)   {
      return false;
   }

   SET_BIT_FLAG( flags, WNDFLAG_OPEN );

   return true;
}

//------------------------------------------------------------------------
void Window::close()
{
   if (!is_open()) {
      return;
   }

   // on_closed.trigger(this);
   ::DestroyWindow( hwnd );
   hwnd = INVALID_HANDLE;

   // Removes all the flags
   flags = 0U;
}

//------------------------------------------------------------------------
void Window::show( bool activate /*= true*/ )
{
   if (is_open() && !is_visible()) {
      int const command = activate ? SW_SHOW : SW_SHOWNOACTIVATE;
      ::ShowWindow( hwnd, command );
   } 
}

//------------------------------------------------------------------------
void Window::hide()
{
   if (is_open() && is_visible()) {
      ::ShowWindow( hwnd, SW_HIDE );
   }
}

//------------------------------------------------------------------------
void Window::focus() 
{
   if (is_open() && !has_focus()) {
      ::SetFocus(hwnd);
      ::SetForegroundWindow(hwnd);
   }
}

//------------------------------------------------------------------------
void Window::set_title( char const *new_title ) 
{
   if (title != new_title) {
      title = new_title;
      if (hwnd != NULL) {
         ::SetWindowTextA( hwnd, new_title );
      }
   }
}

//------------------------------------------------------------------------
uint Window::process_messages() 
{
   uint msgs_processed = 0U;
   if (is_open()) {
      MSG msg;
      while (PeekMessage( &msg, hwnd, 0, 0, PM_REMOVE)) {
         ++msgs_processed;
         TranslateMessage( &msg );
         DispatchMessage( &msg );

         if (msg.message == WM_DESTROY) {
            close();
            break;
         } 
      }
   }

   return msgs_processed;
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

