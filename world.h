#ifndef world_h
#define world_h

#include "defines.h"
#include "draw.h"
#include "array.h"
#include "vec2.h"
#include "entity.h"

typedef struct
{
    int             x;
    int             y;
    paletteColor_t    color;
} star_t;


typedef struct
{
    vec2_t          position;
    vec2_t          velocity;
    int             lifespan;
    paletteColor_t    color;
} particle_t;


typedef struct entity entity_t;
typedef struct game game_t;

typedef struct world
{
    game_t *            game;
    
    Array<star_t> *     stars;
    Array<particle_t> * particles;
    Array<entity_t> *   entities;
} world_t;


world_t *   InitWorld( game_t * game );
void        DestroyWorld( world_t * world );

void        SpawnParticles( world_t * world, particle_t * array, int num );
void        DrawWorld( world_t * world );
void        UpdateWorld( world_t * world, float dt );

entity_t * SpawnEntity
 (  world_t * world,
    entityType_t type,
    vec2_t position,
    float rotation );

#endif /* world_h */
