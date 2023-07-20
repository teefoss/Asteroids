#ifndef GAME_H
#define GAME_H

#include "sprite.h"
#include "defines.h"

#define MAX_ENTITIES 200

typedef struct world world_t;


typedef struct game
{
    int         frame;
    world_t *   world;
    int         level;
} game_t;


game_t *    InitGame( void );
void        DestroyGame( game_t * );
void        StartLevel( game_t *, int number);
void        DrawGame( game_t *);
void        DoFrame( game_t *, float dt );

#endif /* GAME_H */
