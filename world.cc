#include "world.h"
#include "mylib.h"
#include "game.h"
#include <stdio.h>

static void InitStars( world_t * world ) {
    const paletteColor_t star_colors[2] = {
        COLOR_GRAY,
        COLOR_BLUE,
    };
    
    world->stars = new Array<star_t>( (GAME_WIDTH * GAME_HEIGHT) / 60 );
    
    for ( int i = 0; i < world->stars->capacity; i++ ) {
        
        star_t star = {
            .x = Random( 0, GAME_WIDTH ),
            .y = Random( 0, GAME_HEIGHT ),
            .color = RANDOM_ELEMENT( star_colors )
        };
        
        world->stars->append( star );
    }
}


world_t * InitWorld( game_t * game ) {
    world_t * world = (world_t *)calloc( 1, sizeof *world );
    if ( world == NULL ) {
        fprintf( stderr, "%s: malloc failed\n", __func__ );
    }
    
    world->game = game;
    InitStars( world );
    world->particles = new Array<particle_t>( 1024 );
    world->entities = new Array<entity_t>( 64 );
    
    vec2_t player_start = { GAME_WIDTH / 2.0f, GAME_HEIGHT / 2.0f };
    entity_t * player =
    SpawnEntity( world, ENTITY_PLAYER, player_start, DEG2RAD( 270 ) );
    player->scale = 0.0f;
    player->state = ES_APPEARING;
    
    return world;
}


void DestroyWorld( world_t * world ) {    
    delete world->stars;
    delete world->entities;
    delete world->particles;
}


void SpawnParticles( world_t * world, particle_t * array, int num ) {
    for ( int i = 0; i < num; i++ ) {
        world->particles->append( array[i] );
    }
}


static void UpdateParticles( world_t * world, float dt ) {
    for ( int i = 0; i < world->particles->count; i++ ) {
        particle_t * p = &world->particles->buffer[i];
        
        if ( p->lifespan > 0 ) {
            --p->lifespan;
            
            vec2_t velocity = p->velocity * dt;
            p->position += velocity;
        }
    }

    // remove any whose lifespan is 0
    
    for ( int i = 0; i < world->particles->count; i++) {
        particle_t * p = &world->particles->buffer[i];
        
        if ( p->lifespan <= 0 ) {
            world->particles->remove( i );
        }
    }
}


entity_t * SpawnEntity
 (  world_t * world,
    entityType_t type,
    vec2_t position,
    float rotation )
{
    entity_t entity = entity_defs[type];
    
    entity.state        = ES_ACTIVE;
    entity.position     = position;
    entity.rotation     = rotation;
    entity.scale        = 1.0f;
    entity.world        = world;
        
    return world->entities->append( entity );
}


void DrawWorld( world_t * world ) {
    for ( int i = 0; i < world->stars->count; i++ ) {
        star_t * s = &world->stars->buffer[i];
        DrawPoint( s->x, s->y, s->color );
    }
    
    for ( int i = 0; i < world->entities->count; i++ ) {
        entity_t * e = &world->entities->buffer[i];
        DrawEntity( e );
    }
    
    for ( int i = 0; i < world->particles->count; i++ ) {
        particle_t * p = &world->particles->buffer[i];
        DrawPoint( p->position.x, p->position.y, p->color );
    }
}


void UpdateWorld( world_t * world, float dt ) {
    
    // update all entities
    
    for ( int i = 0; i < world->entities->count; i++ ) {
        entity_t * e = &world->entities->buffer[i];
        UpdateEntity( e, dt );
    }
    
    // do entity collisions
    
    for ( int i = 0; i < world->entities->count; i++ ) {
        for ( int j = i + 1; j < world->entities->count; j++ ) {
            entity_t * a = &world->entities->buffer[i];
            entity_t * b = &world->entities->buffer[j];
            
            if ( EntitiesAreColliding( a, b ) ) {
                if ( a->contact ) {
                    a->contact(a, b);
                }
                if ( b->contact ) {
                    b->contact(b, a);
                }
            }
        }
    }
    
    // remove any dead entities
    
    for ( int i = world->entities->count - 1; i >= 0; i-- ) {
        entity_t * e = &world->entities->buffer[i];
        
        if ( e->state == ES_REMOVE ) {
            world->entities->remove( i );
        }
    }
    
    UpdateParticles( world, dt );
}
