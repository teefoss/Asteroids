
#include "draw.h"
#include "defines.h"
#include "entity.h"

#include "sprite.h"
#include "mylib.h"
#include <stdlib.h>

SDL_Window * window;
SDL_Renderer * renderer;
Palette palette;
Sprite sprites[NUM_ENTITY_TYPES];
bool fullscreen;

static void FreeSprites() {
    for ( int i = 0; i < NUM_ENTITY_TYPES; i++ ) {
        SDL_DestroyTexture( sprites[i].texture );
    }
}


void InitWindow() {
    SDL_Rect r = {
        .x = 0,
        .y = 0,
        .w = GAME_WIDTH * SCALE,
        .h = GAME_HEIGHT * SCALE
    };
    
    window = SDL_CreateWindow( "Asteroids", r.x, r.y, r.w, r.h, 0 );
    if ( window == NULL ) {
        fprintf(stderr, "SDL error: %s\n", SDL_GetError());
    }
}


void InitRenderer() {
    renderer = SDL_CreateRenderer( window, -1, 0 );
    if ( renderer == NULL )
        fprintf(stderr, "SDL error: %s\n", SDL_GetError());
    
    SDL_RenderSetLogicalSize( renderer, GAME_WIDTH, GAME_HEIGHT );
    palette = LoadPalette( "default.pal" );
    
    for ( int i = 0; i < NUM_ENTITY_TYPES; i++ ) {
        sprites[i] = LoadSprite( renderer, entity_defs[i].sprite_name, palette );
    }
    
    atexit( FreeSprites );
}


void ToggleFullscreen() {
    u32 flags = fullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP;
    SDL_SetWindowFullscreen( window, flags);
}


void DrawSprite( int type, int x, int y, double angle, float scale ) {
    Sprite * s = &sprites[type];
    
    SDL_Rect src = { 0, 0, s->data.width, s->data.height };
    SDL_Rect dst = {
        .x = x,
        .y = y,
        .w = (int)((float)s->data.width * scale),
        .h = (int)((float)s->data.height * scale)
    };
    
    SDL_RenderCopyEx(renderer,
                     sprites[type].texture,
                     &src,
                     &dst,
                     angle,
                     NULL,
                     SDL_FLIP_NONE);
    
    // draw circle hitbox
#if 0
    int radius = src.w / 2;
    SDL_SetRenderDrawColor( renderer, 255, 0, 0, 255 );
    vec2_t center = { (float)(x + src.w / 2), (float)(y + src.h / 2) };
    
    for ( int y1 = y; y1 <= y + src.h; y1++ ) {
        for ( int x1 = x; x1 <= x + src.w; x1++ ) {
            vec2_t v = { (float)x1, (float)y1 };
            vec2_t between = v - center;
            float dist = between.length();
            
            if ( (int)dist == radius ) {
                SDL_RenderDrawPoint(renderer, x1, y1);
            }
        }
    }
#endif
}


void DrawPoint( int x, int y, paletteColor_t color ) {
    SDL_Color * c = &palette.colors[color];
    SDL_SetRenderDrawColor( renderer, c->r, c->g, c->b, 255 );
    SDL_RenderDrawPoint( renderer, x, y );
}
