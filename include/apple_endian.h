#ifndef _APPLE_ENDIAN_H_
#define _APPLE_ENDIAN_H_

#ifndef __arm__
#error dude, this is only valid on ARM.
#endif

enum {
    OSUnknownByteOrder,
    OSLittleEndian,
    OSBigEndian
};

/* Host endianess to big endian byte swapping macros for constants. */

#define OSSwapHostToBigConstInt16(x) OSSwapConstInt16(x)
#define OSSwapHostToBigConstInt32(x) OSSwapConstInt32(x)
#define OSSwapHostToBigConstInt64(x) OSSwapConstInt64(x)

/* Generic host endianess to big endian byte swapping functions. */
#define _OSSwapInt64	OSSwapInt64
#define _OSSwapInt32	OSSwapInt32
#define _OSSwapInt16	OSSwapInt16


#define OSSwapHostToBigInt16(x) OSSwapInt16(x)
#define OSSwapHostToBigInt32(x) OSSwapInt32(x)
#define OSSwapHostToBigInt64(x) OSSwapInt64(x)

/* Host endianess to little endian byte swapping macros for constants. */

#define OSSwapHostToLittleConstInt16(x) ((uint16_t)(x))
#define OSSwapHostToLittleConstInt32(x) ((uint32_t)(x))
#define OSSwapHostToLittleConstInt64(x) ((uint64_t)(x)) 

/* Generic host endianess to little endian byte swapping functions. */

#define OSSwapHostToLittleInt16(x) ((uint16_t)(x))
#define OSSwapHostToLittleInt32(x) ((uint32_t)(x))
#define OSSwapHostToLittleInt64(x) ((uint64_t)(x))

/* Big endian to host endianess byte swapping macros for constants. */

#define OSSwapBigToHostConstInt16(x) OSSwapConstInt16(x)
#define OSSwapBigToHostConstInt32(x) OSSwapConstInt32(x)
#define OSSwapBigToHostConstInt64(x) OSSwapConstInt64(x)

/* Generic big endian to host endianess byte swapping functions. */

#define OSSwapBigToHostInt16(x) OSSwapInt16(x)
#define OSSwapBigToHostInt32(x) OSSwapInt32(x)
#define OSSwapBigToHostInt64(x) OSSwapInt64(x)

/* Little endian to host endianess byte swapping macros for constants. */

#define OSSwapLittleToHostConstInt16(x) ((uint16_t)(x))
#define OSSwapLittleToHostConstInt32(x) ((uint32_t)(x))
#define OSSwapLittleToHostConstInt64(x) ((uint64_t)(x))

/* Generic little endian to host endianess byte swapping functions. */

#define OSSwapLittleToHostInt16(x) ((uint16_t)(x))
#define OSSwapLittleToHostInt32(x) ((uint32_t)(x))
#define OSSwapLittleToHostInt64(x) ((uint64_t)(x))

#if !defined(OS_INLINE)
# if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#        define OS_INLINE static inline
# else
#        define OS_INLINE static __inline__
# endif
#endif

OS_INLINE
uint16_t
OSSwapInt16(
    uint16_t        data
)
{
#if defined(__llvm__)
  data = (data << 8 | data >> 8);
#elif defined(_ARM_ARCH_6)
  __asm__ ("rev16 %0, %1\n" : "=l" (data) : "l" (data));
#else
  data = (data << 8 | data >> 8);
#endif

  return data;
}

OS_INLINE
uint32_t
OSSwapInt32(
    uint32_t        data
)
{
#if defined(__llvm__)
  data = __builtin_bswap32(data);
#elif defined(_ARM_ARCH_6)
  __asm__ ("rev %0, %1\n" : "=l" (data) : "l" (data));
#else
  /* This actually generates the best code */
  data = (((data ^ (data >> 16 | (data << 16))) & 0xFF00FFFF) >> 8) ^ (data >> 8 | data << 24);
#endif

  return data;
}

OS_INLINE
uint64_t
OSSwapInt64(
    uint64_t        data
)
{
#if defined(__llvm__)
    return __builtin_bswap64(data);
#else
    union {
        uint64_t ull;
        uint32_t ul[2];
    } u;

    /* This actually generates the best code */
    u.ul[0] = (uint32_t)(data >> 32);
    u.ul[1] = (uint32_t)(data & 0xffffffff);
    u.ul[0] = _OSSwapInt32(u.ul[0]);
    u.ul[1] = _OSSwapInt32(u.ul[1]);
    return u.ull;
#endif
}

#endif