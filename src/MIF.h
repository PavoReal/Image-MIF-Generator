#ifndef MIF_H
#define MIF_H

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

struct Config
{
    u32 width;
    u32 depth;

    //
    // From the stb docs:
    //
    // 1 grey
    // 2 grey, alpha
    // 3 red, green, blue
    // 4 red, green, blue, alpha
    u32 inputPixelFormat;
};

#endif

