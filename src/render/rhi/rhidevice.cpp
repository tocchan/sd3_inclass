/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "rhidevice.h"

#include "core/file.h"
#include "core/log.h"

#include "render/vertex.h"
#include "render/rhi/rhidevicecontext.h"
#include "render/rhi/rhiinstance.h"
#include "render/rhi/rhioutput.h"

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
// RENDER DEVICE
//------------------------------------------------------------------------

//------------------------------------------------------------------------
RHIDevice::RHIDevice( RHIInstance *owner, ID3D11Device *dxd )
{
   instance = owner;
   dx_device = dxd;
   immediate_context = nullptr;
}

//------------------------------------------------------------------------
RHIDevice::~RHIDevice()
{
   DX_SAFE_RELEASE(dx_device);
}



//------------------------------------------------------------------------
static ID3DBlob* CompileHLSLToShaderBlob( char const *opt_filename,  // optional: used for error messages
   void const *source_code,                                          // buffer containing source code.
   size_t const source_code_size,                                    // size of the above buffer.
   char const *entrypoint,                                           // Name of the Function we treat as the entry point for this stage
   char const* target  )                                             // What stage we're compiling for (Vertex/Fragment/Hull/Compute... etc...)
{
   /* DEFINE MACROS - CONTROLS SHADER
   // You can optionally define macros to control compilation (for instance, DEBUG builds, disabling lighting, etc...)
   D3D_SHADER_MACRO defines[1];
   defines[0].Name = "TEST_MACRO";
   defines[0].Definition = nullptr;
   */

   DWORD compile_flags = 0U;
   #if defined(DEBUG_SHADERS)
      compile_flags |= D3DCOMPILE_DEBUG;
      compile_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
      compile_flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;   // cause, FIX YOUR WARNINGS
   #else 
      // compile_flags |= D3DCOMPILE_SKIP_VALIDATION;       // Only do this if you know for a fact this shader works with this device (so second run through of a game)
      compile_flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;   // Yay, fastness (default is level 1)
   #endif

   ID3DBlob *code = nullptr;
   ID3DBlob *errors = nullptr;

   HRESULT hr = ::D3DCompile( source_code, 
      source_code_size,                   // plain text source code
      opt_filename,                       // optional, used for error messages (If you HLSL has includes - it will not use the includes names, it will use this name)
      nullptr,                            // pre-compiler defines - used more for compiling multiple versions of a single shader (different quality specs, or shaders that are mostly the same outside some constants)
      D3D_COMPILE_STANDARD_FILE_INCLUDE,  // include rules - this allows #includes in the shader to work relative to the src_file path or my current working directly
      entrypoint,                         // Entry Point for this shader
      target,                             // Compile Target (MSDN - "Specifying Compiler Targets")
      compile_flags,                      // Flags that control compilation
      0,                                  // Effect Flags (we will not be doing Effect Files)
      &code,                              // [OUT] ID3DBlob (buffer) that will store the byte code.
      &errors );                          // [OUT] ID3DBlob (buffer) that will store error information

   if (FAILED(hr) || (errors != nullptr)) {
      if (errors != nullptr) {
         char *error_string = (char*) errors->GetBufferPointer();
         Logf( "Failed to compile [%s].  Compiler gave the following output;\n%s", 
            opt_filename, 
            error_string );
         DX_SAFE_RELEASE(errors);
      }
   } 

   // will be nullptr if it failed to compile
   return code;
}
//------------------------------------------------------------------------
ShaderProgram* RHIDevice::create_shader_from_hlsl_file( char const *filename )
{
   size_t src_size = 0U;
   void *src_code = FileReadToBuffer( filename, &src_size );
   if (src_code == nullptr) {
      // ASSERT_OR_DIE( 0, "File not found. %s", filename );
      return nullptr; 
   }

   ID3DBlob *vs_byte_code = nullptr;
   ID3DBlob *fs_byte_code = nullptr;

   vs_byte_code = CompileHLSLToShaderBlob( filename, src_code, src_size, "VertexFunction", "vs_5_0" );
   fs_byte_code = CompileHLSLToShaderBlob( filename, src_code, src_size, "FragmentFunction", "ps_5_0" );
   delete src_code;

   if ((vs_byte_code != nullptr) 
      && (fs_byte_code != nullptr)) {

      ID3D11VertexShader *vs = nullptr;
      ID3D11PixelShader *fs = nullptr;

      dx_device->CreateVertexShader( vs_byte_code->GetBufferPointer(), 
         vs_byte_code->GetBufferSize(), 
         nullptr,  
         &vs );

      dx_device->CreatePixelShader( fs_byte_code->GetBufferPointer(),
         fs_byte_code->GetBufferSize(), 
         nullptr, 
         &fs );

      ShaderProgram *program = new ShaderProgram( this, vs, fs, vs_byte_code, fs_byte_code );
      return program;
   }

   return nullptr;
}

//------------------------------------------------------------------------
VertexBuffer* RHIDevice::create_vertex_buffer( vertex_t *vertices, uint vertex_count )
{
   VertexBuffer *buffer = new VertexBuffer( this, vertices, vertex_count );
   return buffer;
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
