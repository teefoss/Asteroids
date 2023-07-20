#ifndef mylib_h
#define mylib_h

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

#define U8_MAX 0xFF
#define U16_MAX 0xFFFF
#define U32_MAX 0xFFFFFFFF

#define CENTERED SDL_WINDOWPOS_CENTERED
#define LOG_SDL_ERROR fprintf(stderr, "%s: %s\n", __func__, SDL_GetError())
#define LOG_BAD_MALLOC fprintf(stderr, "%s: malloc failed\n", __func__);

#define BIT(n) (1 << n)
#define array_size(arr) (sizeof(arr) / sizeof(arr[0]))
#define foreach(index, arr) for (size_t _i_ = 0; _i_ < array_size(arr); _i_++)
#define sign(a) (a < 0 ? -1 : a > 0 ? 1 : 0)
#define MAX(a, b) ((a > b) ? (a) : (b))
#define MIN(a, b) ((a < b) ? (a) : (b))
#define RANDOM_INDEX(array) Random(0, array_size(array))
#define RANDOM_ELEMENT(array) array[RANDOM_INDEX(array)]
#define RANDOM_ANGLE arc4random_uniform(360)

// declare pt (something with a .x and .y) prior to use
#define LOOP_2D(pt, w, h)   for ( pt.y = 0; pt.y < h; pt.y++ ) \
                            for ( pt.x = 0; pt.x < w; pt.x++ )

#define CLAMP(a, min, max) \
     if ( a < min ) { \
          a = min; \
     } else if ( a > max ) { \
          a = max; \
     }

#define print(var) _Generic((var),  \
        unsigned char: print_uchar, \
        int: print_int,             \
        double: print_double,       \
        float: print_float,         \
        unsigned long: print_ulong, \
        SDL_Point: print_sdl_point, \
        SDL_Rect: print_sdl_rect    \
    )(#var, var)

#define STORAGE(T, name, size)  \
    struct name {               \
        T buffer[size];         \
        int count;              \
    }

#define COUNT(arr)              arr.count
#define CAPACITY(arr)           (sizeof(arr.buffer) / sizeof(arr.buffer[0]))
#define CLEAR(arr)              arr.count = 0
#define REMOVE(arr, i)          arr.buffer[i] = arr.buffer[--arr.count]
#define INSERT(arr, i, value)   arr.buffer[arr.count++] = arr.buffer[i]; \
                                arr.buffer[i] = value
#define APPEND(arr, value)      arr.buffer[arr.count++] = value
#define LAST(arr)               arr.buffer[arr.count - 1]
#define TYPE(arr)               typeof(arr.buffer[0])
#define ELEMENT(arr, i)         arr.buffer[i]
#define PUSH(stack, value)      stack.buffer[s.count++] = value
#define POP(stack)              stack.buffer[--s.count]
#define FOR_EACH(it, arr)       for ( TYPE(arr) * it = arr.buffer; \
                                it < &LAST(arr); \
                                it++ )

typedef int8_t      s8;
typedef int16_t     s16;
typedef int32_t     s32;
typedef int64_t     s64;
typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

#define PRINT_DECL(func, T) void func(const char * name, T value)
PRINT_DECL(print_uchar, unsigned char);
PRINT_DECL(print_int, int);
PRINT_DECL(print_float, float);
PRINT_DECL(print_double, double);
PRINT_DECL(print_ulong, unsigned long);
PRINT_DECL(print_sdl_point, SDL_Point);
PRINT_DECL(print_sdl_rect, SDL_Rect);

FILE * OpenFile( const char * file_name, const char * mode );

s32 Random( s32 min, s32 max ); // from min to max - 1
float RandomFloat( float min, float max );

int MapInt( int x, int in_min, int in_max, int out_min, int out_max );
void SetColor( SDL_Renderer * renderer, SDL_Color color );

float Lerp( float a, float b, float w );

void Error( char * message, ... );

#pragma mark -
#ifdef MYLIB_IMPLEMENTATION

#define PRINT_DEF(func, T, format) \
    PRINT_DECL(func, T) { printf("%s: " format "\n", name, value); }
PRINT_DEF(print_uchar, unsigned char, "%d")
PRINT_DEF(print_int, int, "%d")
PRINT_DEF(print_float, float, "%f")
PRINT_DEF(print_double, double, "%f")
PRINT_DEF(print_ulong, unsigned long, "%lu")

void print_sdl_point(const char * name, SDL_Point point) {
    printf("%s: (%d, %d)\n", name, point.x, point.y);
}

void print_sdl_rect(const char * name, SDL_Rect r) {
    printf("%s: x: %d, y: %d, w: %d, h: %d\n", name, r.x, r.y, r.w, r.h);
}


FILE * OpenFile( const char * file_name, const char * mode ) {
    FILE * file = fopen( file_name, mode );
    if ( file == NULL ) {
        fprintf(stderr, "error: could not open %s\n", file_name);
        exit(EXIT_FAILURE);
    }
    
    return file;
}


s32 Random( s32 min, s32 max ) {
    return arc4random_uniform( max - min ) + min;
}


float RandomFloat( float min, float max ) {
    float random = (float)arc4random() / (float)U32_MAX;
    return random * (max - min) + min;
}


int MapInt( int x, int in_min, int in_max, int out_min, int out_max ) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void SetColor( SDL_Renderer * renderer, SDL_Color color ) {
    SDL_SetRenderDrawColor( renderer, color.r, color.g, color.b, color.a );
}


float Lerp( float a, float b, float w ) {
    if ( fabsf( a - b ) < 1.0f ) {
        return b;
    } else {
        return (1.0f - w) * a + w * b;
    }
}


void Error( char * message, ... ) {
    va_list argptr;

    va_start( argptr, message );
    vfprintf( stderr, message, argptr );
    va_end( argptr );
    printf( "\n" );

    exit( EXIT_FAILURE );
}


#endif /* MYLIB_IMPLEMENTATION */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* mylib_h */
