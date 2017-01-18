#pragma once
#if !defined( __RENDER_TYPES__ )
#define __RENDER_TYPES__

/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "core/types.h"
#include "core/bit.h"

/************************************************************************/
/*                                                                      */
/* DEFINES AND CONSTANTS                                                */
/*                                                                      */
/************************************************************************/

// See: https://msdn.microsoft.com/en-us/library/windows/desktop/ff819065(v=vs.85).aspx

// +1 for immediate/$Global constant buffer, so actually 16
#define MAX_CONSTANT_BUFFERS (15)   

// 4096 16-byte elements.
#define MAX_CONSTANT_BUFFER_SIZE    (16 * 4096)    

#define MAX_BOUND_COLOR_TARGETS (8)




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
//------------------------------------------------------------------------
enum eRenderOutputMode : uint
{
   RENDEROUTPUT_WINDOWED,
   RENDEROUTPUT_BORDERLESS,
   RENDEROUTPUT_FULLSCREEN_WINDOW,
   RENDEROUTPUT_FULLSCREEN_DEDICATED, 
};

//------------------------------------------------------------------------
enum eBindFlag : uint
{
   // Vertex Data
   BIND_VERTEX_BUFFER = BIT_FLAG(0),
   BIND_INDEX_BUFFER = BIT_FLAG(1),

   // Constant Buffers
   BIND_CONSTANT_BUFFER = BIT_FLAG(2),

   // Textures/Buffers
   BIND_SHADER_RESOURCE = BIT_FLAG(3),
   BIND_RENDER_TARGET = BIT_FLAG(4),
   BIND_DEPTH_STENCIL = BIT_FLAG(5),
};


//------------------------------------------------------------------------
enum eBufferUsage : uint
{
   BUFFERUSAGE_GPU,     // GPU Read/Write, CPU Full Writes        (RenderTargets?)
   BUFFERUSAGE_STATIC,  // GPU Read, Written only at creation     (TEXTURES, STATIC MESHES)
   BUFFERUSAGE_DYNAMIC, // GPU Read, CPU Write (Partial or Full)  (DYNAMIC MESHES/DYNAMIC TEXTURES)
   BUFFERUSAGE_STAGING, // GPU Read/Write, CPU Read/Write         Slowest - image composition/manipulation
};

//------------------------------------------------------------------------
enum ePrimitiveType : uint
{
   PRIMITIVE_NONE,
   PRIMITIVE_POINTS,
   PRIMITIVE_LINES,
   PRIMITIVE_TRIANGLES,
};

//------------------------------------------------------------------------
enum eFillMode : uint
{
   FILLMODE_SOLID,
   FILLMODE_WIREFRAME,
};

//------------------------------------------------------------------------
enum eCullMode : uint
{
   CULL_NONE,
   CULL_FRONT,
   CULL_BACK,
};

//------------------------------------------------------------------------
enum eBlendFactor : uint
{
   BLEND_ZERO,             
   BLEND_ONE,              
   BLEND_SRC_COLOR,        
   BLEND_INV_SRC_COLOR,
   BLEND_SRC_ALPHA,
   BLEND_INV_SRC_ALPHA,
   BLEND_DEST_COLOR,
   BLEND_INV_DEST_COLOR,
   BLEND_DEST_ALPHA,
   BLEND_INV_DEST_ALPHA,
   BLEND_CONSTANT,         // supplied when binding a blend state
   BLEND_INV_CONSTANT,         // supplied when binding a blend state
};

//------------------------------------------------------------------------
enum eBlendOp : uint
{
   BLENDOP_ADD,
   BLENDOP_SUBTRACT,
   BLENDOP_REV_SUBTRACT,
   BLENDOP_MIN,
   BLENDOP_MAX,
};

enum eCompareFunc : uint8_t
{
   COMPARE_NEVER,          // Never passes
   COMPARE_LESS,           // Passes if we're less
   COMPARE_EQUAL,          // Passes if we're equal
   COMPARE_LESS_EQUAL,     // Passes if we're <=
   COMPARE_GREATER,        // Pass if src > dest
   COMPARE_NOT_EQUAL,      // Pass if we're not equal
   COMPARE_GREATER_EQUAL,  // Pass if we're greater or equal
   COMPARE_ALWAYS,         // Always passes
};

enum eStencilOp : uint8_t 
{
   STENCIL_KEEP,
   STENCIL_ZERO,
   STENCIL_REPLACE,  // Set's to the constant value set when setting state

   STENCIL_INCREMENT,      // increments, can wrap
   STENCIL_INCREMENT_SAT,  // increments, can't wrap 

   STENCIL_DECREMENT,      // decrements, can wrap
   STENCIL_DECREMENT_SAT,  // decrements, no wrap

   STENCIL_INVERT,         // value = MAX_STENCIL - value;
};

/************************************************************************/
/*                                                                      */
/* STRUCTS                                                              */
/*                                                                      */
/************************************************************************/
struct gpu_buffer_data_t
{
   void const *cpu_data;
   size_t cpu_data_byte_size;
   
   // used for data
   size_t pitch;
   size_t slice;
};

/************************************************************************/
/*                                                                      */
/* CLASSES                                                              */
/*                                                                      */
/************************************************************************/

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
