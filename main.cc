/* -----------------------------------------------------------------------------
 *
 *  Clone of Asteroids
 *  by Thomas Foster, 2022
 *
 * -------------------------------------------------------------------------- */

#define MYLIB_IMPLEMENTATION
#include "mylib.h"

#define SPRITE_IMPLEMENTATION
#include "sprite.h"

#include "defines.h"
#include "draw.h"
#include "game.h"
#include "utility.h"

#include <stdlib.h>
#include <sys/time.h>

/*  TODO: LIST
    ----------
    - blow up player
    - extended entity info: add to initializer and definitions...
    - switch to Array
    - particles are entities?
    - entity draw function ptr

    - ParticleSystem struct -> world
 
    - World:
        * stars
        * entities
        * particle system 
*/

static game_t * game;

/*
 * The time since program start in seconds
 */
float FloatTime() {
    struct timeval timeval;
    static int seconds = 0;

    gettimeofday(&timeval, NULL);
    if ( seconds == 0 ) {
        seconds = timeval.tv_sec;
        return timeval.tv_usec / 1000000.0f;
    }

    return (timeval.tv_sec - seconds) + timeval.tv_usec / 1000000.0f;
}


void CleanUp() {
    DestroyGame( game );
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    SDL_Quit();
}


int main() {
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        fprintf(stderr, "SDL error: %s\n", SDL_GetError());
        exit( EXIT_FAILURE );
    }

    InitWindow();
    InitRenderer();
    
    game = InitGame();
    StartLevel( game, 1 );

    atexit(CleanUp);
    
    float old_time = FloatTime();
    while ( true ) {
        float new_time = FloatTime();
        float dt = new_time - old_time;
        
        if ( dt < 1.0f / (float)FPS ) {
            SDL_Delay( 1 );
            continue;
        }
                
        DoFrame( game, dt );
        
        old_time = new_time;
    }
}
