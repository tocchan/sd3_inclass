#pragma once
#if !defined( __ITW_ENDIANESS__ )
#define __ITW_ENDIANESS__

#include "core/types.h"
#include "core/util.h"

/************************************************************************/
/*                                                                      */
/* TYPES                                                                */
/*                                                                      */
/************************************************************************/
enum eEndianness
{
   LITTLE_ENDIAN = 0,
   BIG_ENDIAN,
};

/************************************************************************/
/*                                                                      */
/* FUNCTION PROTOTYPES                                                  */
/*                                                                      */
/************************************************************************/
//------------------------------------------------------------------------
inline uint16_t ByteFlip2( uint16_t x ) 
{
   return (((x & 0xff00) >> 8) 
      | ((x & 0x00ff) << 8));
}

//------------------------------------------------------------------------
inline uint32_t ByteFlip4( uint32_t x )
{
   return ( ((x & 0xff000000) >> 24)
          | ((x & 0x00ff0000) >>  8)
          | ((x & 0x0000ff00) <<  8)
          | ((x & 0x000000ff) << 24) );
}

//------------------------------------------------------------------------
inline uint64_t ByteFlip8( uint64_t x )
{
   return ( ((x & 0xff00000000000000) >> 56)
          | ((x & 0x00ff000000000000) >> 40)
          | ((x & 0x0000ff0000000000) >> 24)  
          | ((x & 0x000000ff00000000) >>  8)
          | ((x & 0x00000000ff000000) <<  8)
          | ((x & 0x0000000000ff0000) << 24)  
          | ((x & 0x000000000000ff00) << 40)
          | ((x & 0x00000000000000ff) << 56) );
}

//------------------------------------------------------------------------
inline void ByteFlip( void *arg, uint size )
{
   uint count = size >> 1;
   uint8_t *buffer = (uint8_t*) arg;
   for (uint i = 0; i < count; i++) {
      uint j = size - i - 1;
      Swap( buffer[i], buffer[j] );
   }
}

//------------------------------------------------------------------------
inline void ByteFlipTo( void *out, void const *in, size_t size ) 
{
   byte_t *out_buffer = (byte_t*)out;
   byte_t const *in_buffer = (byte_t const*)in;
   for (size_t i = 0; i < size; ++i) {
      out_buffer[i] = in_buffer[size - i - 1];
   }
}

//------------------------------------------------------------------------
template <typename T>
inline void ByteFlip( T &v ) 
{
   ByteFlip( &v, sizeof(T) );
}

//------------------------------------------------------------------------
// Determine the eEndianness of the system
constexpr inline bool IsLittleEndian() 
{
   return (((uint8_t*)(&type_data<uint>::ONE))[0] == 0x01);
}

//------------------------------------------------------------------------
constexpr inline bool IsBigEndian() 
{
   return (((uint8_t*)(&type_data<uint>::ONE))[0] == 0x00);
}

//------------------------------------------------------------------------
constexpr inline eEndianness __fastcall EndiannessOfSystem()
{
   return IsLittleEndian() ? LITTLE_ENDIAN : BIG_ENDIAN;
}


//------------------------------------------------------------------------
// Convert a big enedian variable to the endianness of this system
inline void MakeBigEndian( void *buffer, uint buffer_size ) 
{
   if (IsLittleEndian()) {
      ByteFlip( buffer, buffer_size );
   }
}

//------------------------------------------------------------------------
// Converts a little endian variable to the endianness of the system
template <typename T>
inline void MakeBigEndian( T &arg ) 
{
   MakeBigEndian( &arg, sizeof(T) );
}

//------------------------------------------------------------------------
template <typename T>
inline T ToBigEndian( T const &v ) 
{
   T copy = v;
   MakeBigEndian(copy);
   return copy;
}


//------------------------------------------------------------------------
inline void MakeLittleEndian( void *buffer, uint buffer_size ) 
{
   if (!IsLittleEndian()) {
      ByteFlip( buffer, buffer_size );
   } 
}

//------------------------------------------------------------------------
template <typename T>
inline void MakeLittleEndian( T &arg ) 
{
   MakeLittleEndian( &arg, sizeof(T) );
}

//------------------------------------------------------------------------
template <typename T>
inline T ToLittleEndian( T const &v ) 
{
   T copy = v;
   MakeLittleEndian(copy);
   return copy;
}


#endif