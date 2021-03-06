#pragma once
#if !defined( __RENDER_INSTANCE__ )
#define __RENDER_INSTANCE__

/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "render/rhi/dx11.h"


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
class RHIOutput;

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
class RHIInstance
{
   public:
      HMODULE debug_module;
      IDXGIDebug *debug;

   public:
      ~RHIInstance();

      // HELPERS
      // RHIDeviceContext* create_display_context( render_context_options_t const &options ) { return nullptr; }
      bool create_output( RHIDevice **out_device, 
         RHIDeviceContext **out_context, 
         RHIOutput **out_output, 
         uint const px_width, 
         uint const px_height );

   private:
      RHIInstance();

   public:
      // STATICS
      static RHIInstance* GetInstance();
      static void ReportLiveObjects();
      static void SetDebugName( ID3D11DeviceChild *d3d_obj, char const *name );
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
