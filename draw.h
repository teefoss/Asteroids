#ifndef draw_h
#define draw_h

#include <SDL2/SDL.h>

typedef enum
{
    COLOR_BLACK,
    COLOR_BLUE,
    COLOR_GREEN,
    COLOR_CYAN,
    COLOR_RED,
    COLOR_MAGENTA,
    COLOR_BROWN,
    COLOR_WHITE,
    COLOR_GRAY,
    COLOR_BRIGHT_BLUE,
    COLOR_BRIGHT_GREEN,
    COLOR_BRIGHT_CYAN,
    COLOR_BRIGHT_RED,
    COLOR_BRIGHT_MAGENTA,
    COLOR_YELLOW,
    COLOR_BRIGHT_WHITE,
    NUM_COLORS
} paletteColor_t;

typedef struct {
    int count;
    paletteColor_t array[16];
} spriteColors_t;

void InitWindow( void );
void InitRenderer( void );
void ToggleFullscreen( void );
void DrawSprite( int type, int x, int y, double angle, float scale );
void DrawPoint( int x, int y, paletteColor_t color );

extern SDL_Window *     window;
extern SDL_Renderer *   renderer;

#endif /* draw_h */
