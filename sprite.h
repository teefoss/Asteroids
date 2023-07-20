#ifndef SPRITE_H
#define SPRITE_H

#include <SDL2/SDL.h>
#include "mylib.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SPRITE_MAX_COLORS 32
#define SPRITE_MAX_SIZE (128 * 128)
#define SPRITE_MAX_FRAMES 4
#define SPRITE_TRANSPARENT 0xFF

typedef struct {
    u8 num_colors;
    SDL_Color colors[SPRITE_MAX_COLORS];
} Palette;

typedef struct {
    struct {
        u8 width;
        u8 height;
        u8 num_frames;
        u8 pixels[SPRITE_MAX_FRAMES][SPRITE_MAX_SIZE];
    } data;
    SDL_Texture * texture;
} Sprite;

typedef struct {
    SDL_Rect dst;
    int frame;
    bool h_flip;
    bool v_flip;
    double angle;
} SpriteDrawInfo;

Palette LoadPalette(const char * file_name);
Sprite LoadSprite(SDL_Renderer * renderer, const char * file_name, Palette palette);
Sprite NewSprite(u8 width, u8 height);
void UpdateSpriteTexture(SDL_Renderer * renderer, Sprite * sprite, Palette palette);
void DrawSprite(SDL_Renderer * renderer, Sprite sprite, SpriteDrawInfo info);
void SetColor(SDL_Renderer * renderer, SDL_Color color);

#ifdef SPRITE_IMPLEMENTATION

Palette LoadPalette(const char * file_name) {
    Palette palette;
    
    FILE * file = OpenFile(file_name, "r");
    
    int num_colors;
    if ( fscanf(file, "numcolors: %d\n", &num_colors) != 1 ) {
        printf("error: could not read number of palette colors\n");
        exit(EXIT_FAILURE);
    }
    palette.num_colors = (u8)num_colors;
    
    if ( palette.num_colors > SPRITE_MAX_COLORS ) {
        printf("error: palette has more than 32 colors\n");
        exit(EXIT_FAILURE);
    }
    
    for ( int i = 0; i < palette.num_colors; i++ ) {
        u32 color32;
        if ( fscanf(file, "%x\n", &color32) != 1 ) {
            printf("error: could not read color #(%d)", i);
            exit(EXIT_FAILURE);
        }
        
        palette.colors[i].r = (color32 & 0xFF0000) >> 16;
        palette.colors[i].g = (color32 & 0x00FF00) >> 8;
        palette.colors[i].b = (color32 & 0x0000FF);
        palette.colors[i].a = 0xFF;
    }
    
    fclose(file);
    printf("loaded palette: %s\n", file_name);
    
    return palette;
}

static SDL_Surface * CreateSurfaceFromSprite(Sprite sprite, Palette palette) {
    SDL_Surface * surface = SDL_CreateRGBSurface
        (0,
         sprite.data.width * sprite.data.num_frames,
         sprite.data.height,
         8, 0, 0, 0, 0);
    
    SDL_SetColorKey(surface, SDL_TRUE, SPRITE_TRANSPARENT);
    SDL_SetPaletteColors(surface->format->palette, palette.colors, 0, palette.num_colors);
    
    SDL_LockSurface(surface);
    for ( int frame = 0; frame < sprite.data.num_frames; frame++ ) {
        for ( int y = 0; y < sprite.data.height; y++ ) {
            for ( int x = 0; x < sprite.data.width; x++ ) {
                int surface_position = y * surface->pitch + x + frame * sprite.data.width;
                u8 * pixel = (u8 *)surface->pixels + surface_position;
                
                int sprite_position = y * sprite.data.width + x;
                *pixel = sprite.data.pixels[frame][sprite_position];
            }
        }
    }
    SDL_UnlockSurface(surface);
    
    return surface;
}

// a sprite's data has changed, update its texture
void UpdateSpriteTexture(SDL_Renderer * renderer,
                         Sprite * sprite,
                         Palette palette) {
    if ( sprite->texture ) {
        SDL_DestroyTexture(sprite->texture);
        sprite->texture = NULL;
    }
    
    SDL_Surface * surface = CreateSurfaceFromSprite(*sprite, palette);
    sprite->texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

Sprite LoadSprite(SDL_Renderer * renderer,
                  const char * file_name,
                  Palette palette) {
    FILE * file = fopen(file_name, "r");
    if ( file == NULL ) {
        printf("error: could not open %s!\n", file_name);
        exit(EXIT_FAILURE);
    }
    
    Sprite sprite;
    memset(&sprite, 0, sizeof(sprite));
    
    if ( fread(&sprite.data, sizeof(sprite.data), 1, file) != 1 ) {
        printf("load sprite error: could not read file %s\n", file_name);
        exit(EXIT_FAILURE);
    }
    
    fclose(file);

    UpdateSpriteTexture(renderer, &sprite, palette);
        
    return sprite;
}

Sprite NewSprite(u8 width, u8 height)
{
    Sprite sprite = {
        .data = {
            .width = width,
            .height = height,
            .num_frames = SPRITE_MAX_FRAMES,
        }
    };
        
    for ( int frame = 0; frame < SPRITE_MAX_FRAMES; frame++ ) {
        for ( int pixel = 0; pixel < SPRITE_MAX_SIZE; pixel++ ) {
            sprite.data.pixels[frame][pixel] = SPRITE_TRANSPARENT;
        }
    }
                    
    return sprite;
}

void DrawSprite(SDL_Renderer * renderer, Sprite sprite, SpriteDrawInfo info)
{
    SDL_Rect src = {
        .x = info.frame * sprite.data.width,
        .y = 0,
        .w = sprite.data.width,
        .h = sprite.data.height
    };
    
    if ( info.h_flip || info.v_flip || info.angle != 0.0 ) {
        int flip = SDL_FLIP_NONE;
        
        if ( info.h_flip )
            flip |= (int)SDL_FLIP_HORIZONTAL;
        
        if ( info.v_flip )
            flip |= (int)SDL_FLIP_VERTICAL;
        
        SDL_RenderCopyEx(renderer,
                         sprite.texture,
                         &src,
                         &info.dst,
                         0,
                         NULL,
                         (SDL_RendererFlip)flip);
    } else {
        SDL_RenderCopy(renderer, sprite.texture, &src, &info.dst);
    }
}

#endif /* SPRITE_IMPLEMENTATION */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SPRITE_H */

