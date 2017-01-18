#pragma once
#if !defined( __RENDER_OUTPUT__ )
#define __RENDER_OUTPUT__

/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "core/types.h"
#include "core/window.h"
#include "render/rhi/dx11.h"
#include "render/rhi/types.h"

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
class RHIInstance;  // System level singleton
class RHIDevice;     // physical GPU
class RHIDeviceContext;    // Potential Display/Background worker
class Texture2D;


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

//------------------------------------------------------------------------
//------------------------------------------------------------------------
// A final destination.  Like a render target, but meant for being
// output to a display, and can be buffered.
// In this version - it corresponds to a window.
//
// Has a render target that can be rendered too

class RHIOutput
{
   public:
      RHIOutput( RHIDevice *o, 
         IDXGISwapChain *sc,
         Window *wnd );
      ~RHIOutput();
      
      // Whatever is currently being renderered
      // that is what we're working with.
      void present();

      void close();

      inline bool is_valid() const { return swap_chain != nullptr; }
      inline float get_width() const { return actual_width; }
      inline float get_height() const { return actual_height; }
      inline float get_aspect_ratio() const { return get_width() / get_height(); }

      inline Texture2D* get_render_target() { return render_target; }

   private:
      public:
      RHIDevice *device;

      Window *window;
      IDXGISwapChain *swap_chain;

      // temporary
      Texture2D *render_target;

      float actual_width;
      float actual_height;

      bool create_render_target();
      void destroy_render_target();
      void destroy_swap_chain();
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
