#include "game.h"
#include "draw.h"
#include "world.h"

#include <stdlib.h>

game_t * InitGame() {
    game_t * game = (game_t *)calloc( 1, sizeof *game );
    
    if ( game == NULL ) {
        fprintf( stderr, "%s: out of memory\n", __func__ );
    }
    
    game->level = 1;
    game->world = InitWorld(game);
        
    return game;
}


void DestroyGame( game_t * game ) {
    DestroyWorld( game->world );
    free(game);
}


void StartLevel( game_t * game, int number ) {
    game->level = number;
    
    // spawn asteroids
    
    int num_asteroids = 20; // TODO: temp
    for ( int i = 0; i < num_asteroids; i++ ) {
        
        vec2_t pt;
        if ( Random( 0, 2 ) == 0 ) {
            // spawn it anywhere along the sides
            pt.x = Random( 0, 2 ) == 0 ? 1.0f : (float)( GAME_WIDTH - 1 );
            pt.y = Random( 0, GAME_HEIGHT );
        } else {
            // spawn it anywhere along the top and bottom
            pt.x = Random( 0, GAME_WIDTH );
            pt.y = Random( 0, 2 ) == 0 ? 1.0f : (float)( GAME_HEIGHT - 1 );
        }
        
        entity_t * asteroid = SpawnEntity(game->world,
                                          ENTITY_ASTEROID_LARGE,
                                          pt,
                                          0.0f);
        
        // random angle
        asteroid->rotation = RandomFloat( 0, MAX_ANGLE );
        
        // start it moving
        asteroid->velocity = EntityForward(asteroid) * RandomFloat(7.0f, 13.0f);
        
        // random rotational velocity
        float spread = DEG2RAD(60);
        asteroid->angular_speed = RandomFloat( -spread, spread );
    }
}


void DrawGame( game_t * game ) {
    SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
    SDL_RenderClear( renderer );
    
    DrawWorld( game->world );
    
    SDL_RenderPresent( renderer );
}


void DoFrame( game_t * game, float dt ) {
    SDL_Event event;
    
    while ( SDL_PollEvent( &event ) ) {
        switch ( event.type ) {
            case SDL_QUIT:
                exit(EXIT_SUCCESS);
                break;
            case SDL_KEYDOWN:
                switch ( event.key.keysym.sym ) {
                    case SDLK_BACKSLASH:
                        ToggleFullscreen();
                        break;
                    default:
                        break;
                }
            default:
                break;
        }
    }
    
    UpdateWorld( game->world, dt );
    DrawGame( game );
    
    ++game->frame;
}
