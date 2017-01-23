#pragma once
#if !defined( __RHI_SHADERPROGRAM__ )
#define __RHI_SHADERPROGRAM__

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
class RHIDevice;

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
enum eShaderStage 
{
   SHADER_VERTEX,    // maps to target "vs_5_0", or Vertex Shader Model 5 (currently latest)
   SHADER_FRAGMENT,  // maps to target "ps_5_0", or Pixel Shader Model 5 (currently latest)
};

// Technically a ShaderProgram is just the program running on the GPU (or multiple
// programs as part of a pipeline).
class ShaderProgram
{
   public:
      ShaderProgram( RHIDevice *device );
      ShaderProgram( RHIDevice *device, char const *filename );
      ~ShaderProgram();

      // feel free to add in more methods to help with the creation process if you want.
      bool load_from_file( char const *filename );

      // Releases all DX resources
      void destroy();

      inline bool is_valid() const { return (dx_vertex_shader != nullptr) && (dx_fragment_shader != nullptr); }

   private:
      void create_input_layout();

   public:
      RHIDevice *device;

      // All the steps to this (could be split out to a ShaderStage)
      ID3D11VertexShader *dx_vertex_shader;
      ID3D11PixelShader *dx_fragment_shader;

      // Input Layout - for now, we're going 
      // to assume every shader makes their own
      // for simplicity, but honestly you should make these
      // based on need
      ID3D11InputLayout *dx_input_layout;

      // [OPTIONAL] ByteCode - only need to keep it around if using Reflection
      ID3DBlob *vs_byte_code;
      ID3DBlob *fs_byte_code;
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
