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
// Determines how a resource is meant to be used.
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
/* GLOBAL VARIABLES                                                     */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* FUNCTION PROTOTYPES                                                  */
/*                                                                      */
/************************************************************************/
#endif 
