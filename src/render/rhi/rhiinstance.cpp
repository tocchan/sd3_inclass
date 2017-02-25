/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "rhiinstance.h"

#include <dxgidebug.h>

#include "core/window.h"

#include "render/rhi/rhidevicecontext.h"
#include "render/rhi/rhidevice.h"
#include "render/rhi/rhioutput.h"

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DXGI.lib" )

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
static RHIInstance *gRenderInstance = nullptr;

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

/************************************************************************/
/*                                                                      */
/* EXTERNAL FUNCTIONS                                                   */
/*                                                                      */
/************************************************************************/
//------------------------------------------------------------------------
// RENDER INSTANCE
//------------------------------------------------------------------------

//------------------------------------------------------------------------
STATIC RHIInstance* RHIInstance::GetInstance()
{
   if (gRenderInstance == nullptr) {
      // will create the me variable.
      new RHIInstance();
   }

   return gRenderInstance;
}

//------------------------------------------------------------------------
RHIInstance::RHIInstance()
{
   // Create the lowest level object - this is what everthing else comes from
   // Debug Setup
   // INIT - BEFORE CREATION OF ANY D3D OBJECTS
   debug = nullptr; // IDXGIDebug
   debug_module = nullptr; // HMODULE
   #if defined(_DEBUG)
      #if defined(_X64)
         debug_module = ::LoadLibrary( "dxgidebug.dll" );
      #else 
         debug_module = LoadLibraryEx( "dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32 );
      #endif

      typedef HRESULT (WINAPI *GetDebugModuleCB)( REFIID, void** );
      GetDebugModuleCB cb = (GetDebugModuleCB) ::GetProcAddress( debug_module, "DXGIGetDebugInterface" );
      cb( __uuidof(IDXGIDebug), (void**)&debug );

      debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
   #endif

   gRenderInstance = this;
}

//------------------------------------------------------------------------
RHIInstance::~RHIInstance()
{
   gRenderInstance = nullptr;

   // Debug Shutdown
   if (debug != nullptr) {
      debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);

      DX_SAFE_RELEASE(debug);
      if (nullptr != debug_module) {
         FreeLibrary(debug_module);
         debug_module = nullptr; 
      }
   }
}

//------------------------------------------------------------------------
STATIC void RHIInstance::ReportLiveObjects()
{
   if ((gRenderInstance != nullptr) && (gRenderInstance->debug != nullptr)) {
      gRenderInstance->debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
   }
}
//------------------------------------------------------------------------
STATIC void RHIInstance::SetDebugName( ID3D11DeviceChild *d3d_obj, char const *name ) 
{
   #if defined(_DEBUG)
      if ((d3d_obj != nullptr) && (name != nullptr)) {
         d3d_obj->SetPrivateData( WKPDID_D3DDebugObjectName, strlen(name) + 1, name ); 
      }
   #endif
}

//------------------------------------------------------------------------
bool RHIInstance::create_output( RHIDevice **out_device, 
   RHIDeviceContext **out_context, 
   RHIOutput **out_output, 
   uint const px_width, 
   uint const px_height )
{
   // First, create the window
   Window *window = new Window();
   window->set_client_position( 200, 200, px_width, px_height );
   if (!window->open()) {
      delete window;
      return false;
   }
   window->show();

   // Creation Flags
   // For options, see;
   // https://www.google.com/webhp?sourceid=chrome-instant&ion=1&espv=2&ie=UTF-8#safe=off&q=device_flags+%7C%3D+D3D11_CREATE_DEVICE_DEBUG%3B
   uint device_flags = 0U;
   #if defined(_DEBUG)
      device_flags |= D3D11_CREATE_DEVICE_DEBUG;

      // This flag fails unless we' do 11.1 (which we're not), and we query that
      // the adapter support its (which we're not).  Just here to let you know it exists.
      // device_flags |= D3D11_CREATE_DEVICE_DEBUGGABLE; 
   #endif

   // Setup our Swap Chain
   // For options, see;
   // https://www.google.com/webhp?sourceid=chrome-instant&ion=1&espv=2&ie=UTF-8#safe=off&q=DXGI_SWAP_CHAIN_DESC
    
   DXGI_SWAP_CHAIN_DESC swap_desc;
   memset( &swap_desc, 0, sizeof(swap_desc) );
   
   // fill the swap chain description struct
   swap_desc.BufferCount = 2;                                    // two buffers (one front, one back?)
  
   swap_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT       // how swap chain is to be used
      | DXGI_USAGE_BACK_BUFFER;                                  
   swap_desc.OutputWindow = window->hwnd;                                // the window to be copied to on present
   swap_desc.SampleDesc.Count = 1;                               // how many multisamples (1 means no multi sampling)

   // Default options.
   swap_desc.Windowed = TRUE;                                    // windowed/full-screen mode
   swap_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
   swap_desc.BufferDesc.Width = window->get_width();
   swap_desc.BufferDesc.Height = window->get_height();

   ID3D11Device *dx_device;
   ID3D11DeviceContext *dx_context;
   IDXGISwapChain *dx_swapchain;

   // Actually Create
   HRESULT hr = ::D3D11CreateDeviceAndSwapChain( nullptr, // Adapter, if nullptr, will use adapter window is primarily on.
      D3D_DRIVER_TYPE_HARDWARE,  // Driver Type - We want to use the GPU (HARDWARE)
      nullptr,                   // Software Module - DLL that implements software mode (we do not use)
      device_flags,              // device creation options
      nullptr,                   // feature level (use default)
      0U,                        // number of feature levels to attempt
      D3D11_SDK_VERSION,         // SDK Version to use
      &swap_desc,                // Description of our swap chain
      &dx_swapchain,             // Swap Chain we're creating
      &dx_device,                // [out] The device created
      nullptr,                   // [out] Feature Level Acquired
      &dx_context );             // Context that can issue commands on this pipe.

   if (SUCCEEDED(hr)) {
      RHIDevice *device = new RHIDevice( this, dx_device );
      RHIDeviceContext *context = new RHIDeviceContext( device, dx_context );
      RHIOutput *output = new RHIOutput( device, dx_swapchain, window );

      device->immediate_context = context;
      
      *out_device = device;
      *out_context = context;
      *out_output = output;

      return true;

   } else {
      delete window;
      return false;
   }
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
