#pragma once
#if !defined( __UTIL_BIT_UTIL__ )
#define __UTIL_BIT_UTIL__


// *Library with some bit-math macros

#define BITS_PER_BYTE         (8)
#define BIT_SIZE(t)           (sizeof(t) * BITS_PER_BYTE)

// Count the number of bits turned on in uint32
#define BIT_COUNT_MASK_1      (0xaaaaaaaa)      // 101010...
#define BIT_COUNT_MASK_2      (0xcccccccc)      // 11001100...
#define BIT_COUNT_MASK_4      (0xf0f0f0f0)      // 11110000...
#define BIT_COUNT_MASK_8      (0xff00ff00)      // 1111111100000000...
#define BIT_COUNT_MASK_16      (0xffff0000)      // ... etc...

#define BIT_COUNT_1(x)         ((((x) & BIT_COUNT_MASK_1) >> 1) + ((x) & ~BIT_COUNT_MASK_1))
#define BIT_COUNT_2(x)         (((BIT_COUNT_1(x) & BIT_COUNT_MASK_2) >> 2) + (BIT_COUNT_1(x) & ~BIT_COUNT_MASK_2))
#define BIT_COUNT_4(x)         (((BIT_COUNT_2(x) & BIT_COUNT_MASK_4) >> 4) + (BIT_COUNT_2(x) & ~BIT_COUNT_MASK_4))
#define BIT_COUNT_8(x)         (((BIT_COUNT_4(x) & BIT_COUNT_MASK_8) >> 8) + (BIT_COUNT_4(x) & ~BIT_COUNT_MASK_8))

#define BIT_COUNT32(x)           (((BIT_COUNT_8(x) & BIT_COUNT_MASK_16) >> 16) + (BIT_COUNT_8(x) & ~BIT_COUNT_MASK_16))


// Just turns on a certain bit
#define BIT_FLAG(x)                    (1U << (x))
#define IS_BIT_FLAG_NOT_SET(x, f)      (((x) & (f)) != f)

#define SET_BIT_FLAG(v, f)             (v) = (v) | (f)
#define SET_BIT(v, x)                  SET_BIT_FLAG(v, BIT_FLAG(x))
#define UNSET_BIT_FLAG(v, f)           (v) = (v) & ~(f)
#define UNSET_BIT(v, x)                UNSET_BIT_FLAG(v, BIT_FLAG(x))

#define IS_BIT_FLAG_SET(x, f)          (((x) & (f)) == f)
#define IS_BIT_SET(x, b)               IS_BIT_FLAG_SET( x, BIT_FLAG(b) )
#define IS_BIT_NOT_SET( x, b )         IS_BIT_FLAG_NOT_SET( x, BIT_FLAG(b) )

// Will return true if the flag was on, and unset the flag in the process
template <typename T, typename F>
bool BitFlagConsume( T &flags, F const flag ) 
{
   T const f = (T)flag;
   if (IS_BIT_FLAG_SET(flags, f)) {
      UNSET_BIT_FLAG(flags, f);
      return true;
   }

   return false;
}


// Will return a mask of X bits
#define BIT_MASK(x)            ((1 << (x)) - 1)

// Is this number a power of two
#define IS_POWER_OF_TWO(x)   (((x) & ((x) - 1)) == 0)

// Align x to some power of two [make sure align is a power of two!] [rounds up]
#define ALIGN_TO_POW2(x, align)        ((align) == 0) ? (x) : (((x) + ((align) - 1)) & ~((align) - 1))
#define IS_ALIGNED_TO_POW2(x, align)   (((x) & ((align) - 1)) == 0)


//------------------------------------------------------------------------
inline uint32_t MakePowerOfTwo( uint32_t v ) 
{
   v--;
   v |= v >> 1;
   v |= v >> 2;
   v |= v >> 4;
   v |= v >> 8;
   v |= v >> 16;
   
   return v + 1;
}

//------------------------------------------------------------------------
inline uint64_t MakePowerOfTwo( uint64_t v ) 
{
   v--;
   v |= v >> 1;
   v |= v >> 2;
   v |= v >> 4;
   v |= v >> 8;
   v |= v >> 16;
   v |= v >> 32;
   
   return v + 1;
}

//------------------------------------------------------------------------
inline uint BitCount( uint const bits ) 
{
   uint mask = 1;
   uint count = 0U;
   while (mask != 0U) {
      count += ((bits & mask) ? 1U : 0U);
      mask = mask << 1;
   }

   return count;
}




#endif
