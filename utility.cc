#include "utility.h"


SDL_Point Vec2ToSDL( vec2_t v ) {
    return (SDL_Point){ (int)v.x, (int)v.y };
}
