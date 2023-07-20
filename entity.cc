#include "entity.h"
#include "defines.h"
#include "player.h"
#include "world.h"
#include "array.h"
#include "draw.h"
#include "utility.h"

#include <math.h>

#define ASSET_DIR "assets"

static const spriteColors_t asteroid_colors = {
    .count = 3,
    .array = {
        COLOR_GRAY,
        COLOR_WHITE,
        COLOR_BRIGHT_WHITE
    }
};


void BulletContact(entity_t * bullet, entity_t * hit);


const entity_t entity_defs[] = {
    [ENTITY_PLAYER] = {
        .type = ENTITY_PLAYER,
        .radius = 4.0f,
        .sprite_name = ASSET_DIR "/ship.px",
        .colors = {
            .count = 7,
            .array = {
                COLOR_BRIGHT_CYAN,
                COLOR_BRIGHT_BLUE,
                COLOR_GRAY,
                COLOR_WHITE,
                COLOR_BRIGHT_WHITE,
                COLOR_RED,
                COLOR_BRIGHT_RED
            }
        },
            .update = UpdatePlayer,
            .contact = PlayerContact,
    },
    [ENTITY_ASTEROID_LARGE] = {
        .type = ENTITY_ASTEROID_LARGE,
        .radius = 16.0f,
        .sprite_name = ASSET_DIR "/asteroid-large.px",
        .colors = asteroid_colors,
    },
    [ENTITY_ASTEROID_MEDIUM] = {
        .type = ENTITY_ASTEROID_MEDIUM,
        .radius = 8.0f,
        .sprite_name = ASSET_DIR "/asteroid-medium.px",
        .colors = asteroid_colors,
    },
    [ENTITY_ASTEROID_SMALL] = {
        .type = ENTITY_ASTEROID_SMALL,
        .radius = 4.0f,
        .sprite_name = ASSET_DIR "/asteroid-small.px",
        .colors = asteroid_colors,
    },
    [ENTITY_BULLET] = {
        .type = ENTITY_BULLET,
        .radius = 1.5f,
        .sprite_name = ASSET_DIR "/bullet.px",
        .flags = FL_NO_WRAP,
        .contact = BulletContact,
        .colors = {
            .count = 2,
            .array = {
                COLOR_GREEN,
                COLOR_BRIGHT_GREEN
            }
        },
    },
};


vec2_t EntityForward( entity_t * entity ) {
    return (vec2_t){ cosf(entity->rotation), sinf(entity->rotation) };
}


float EntityRadius( entity_t * e ) {
    return e->radius * e->scale;
}


bool EntityIsVisible( entity_t * entity ) {
    SDL_Point entity_point = Vec2ToSDL( entity->position );
    
    int diameter = EntityRadius( entity ) * 2.0f;
    
    SDL_Rect visible_area = {
        .x = -diameter,
        .y = -diameter,
        .w = GAME_WIDTH + diameter * 2,
        .h = GAME_HEIGHT + diameter * 2
    };
    
    return SDL_PointInRect( &entity_point, &visible_area );
}


bool EntitiesAreColliding( entity_t * a, entity_t * b ) {
    if ( a->state != ES_ACTIVE ) {
        return false;
    }
    
    vec2_t between = a->position - b->position;
    float ar = EntityRadius( a );
    float br = EntityRadius( b );
    float radii_sqruared = (ar + br) * (ar + br);

    return between.lengthSquared() < radii_sqruared;
}


void GeneralUpdateEntity( entity_t * entity, float dt ) {

    if ( entity->state != ES_ACTIVE ) {
        return;
    }
    
    vec2_t velocity = entity->velocity * dt;
    entity->position += velocity;
    
    entity->rotation += entity->angular_speed * dt;
    
    if ( !(entity->flags & FL_NO_WRAP) ) {
        entity->position.x = fmodf(entity->position.x + GAME_WIDTH, GAME_WIDTH);
        entity->position.y = fmodf(entity->position.y + GAME_HEIGHT, GAME_HEIGHT);
    }
    
    // mark removed if entity too far off screen
    
    if ( !EntityIsVisible(entity) ) {
        entity->state = ES_REMOVE;
        return;
    }
}


void UpdateEntity( entity_t * entity, float dt ) {
    GeneralUpdateEntity( entity, dt );
    
    if ( entity->update ) {
        entity->update(entity, dt);
    }
}


void DrawEntity( entity_t * entity ) {
    
    if ( entity->state == ES_RESPAWNING ) {
        return;
    }
    
    float r = EntityRadius( entity );
    int x = entity->position.x - r;
    int y = entity->position.y - r;
    
    double angle = RAD2DEG( entity->rotation ) + 90.0;
    DrawSprite( (int)entity->type, x, y, angle, entity->scale );
        
    if ( entity->flags & FL_NO_WRAP ) {
        return;
    }
    
    int diameter = (int)(r * 2.0f);
    
    // if outside, draw twice horizontally
    SDL_Rect horizontal_inner = {
        .x = (int)r,
        .y = 0,
        .w = GAME_WIDTH - diameter,
        .h = GAME_HEIGHT
    };

    // if outside, draw twice vertically
    SDL_Rect vertical_inner = {
        .x = 0,
        .y = (int)r,
        .w = GAME_WIDTH,
        .h = GAME_HEIGHT - diameter
    };
    
    SDL_Point pt = Vec2ToSDL( entity->position );
    
    if ( !SDL_PointInRect( &pt, &horizontal_inner ) ) {
        int x2 = x;
        
        if ( entity->position.x < r ) {
            x2 += GAME_WIDTH;
        }
        
        if ( entity->position.x > GAME_WIDTH - r ) {
            x2 -= GAME_WIDTH;
        }

        DrawSprite( entity->type, x2, y, angle, entity->scale );
    }
    
    if ( !SDL_PointInRect( &pt, &vertical_inner ) ) {
        int y2 = y;
        
        if ( entity->position.y < r ) {
            y2 += GAME_HEIGHT;
        }
        
        if ( entity->position.y > GAME_HEIGHT - r ) {
            y2 -= GAME_HEIGHT;
        }
        
        DrawSprite( entity->type, x, y2, angle, entity->scale );
    }
}


int EntityArea( entity_t * entity ) {
    float r = EntityRadius( entity );
    return M_PI * r * r;
}


void ExplodeEntity( entity_t * entity ) {
    
    int num_particles = EntityArea( entity ) / 2;
    Array<particle_t> debris( num_particles );
    
    for ( int i = 0; i < num_particles; i++ ) {
        particle_t p;
        
        float r = EntityRadius( entity );
        p.position = (vec2_t){ 0.0f, RandomFloat( -r, 0.0f ) };
        p.position = p.position.rotated( RANDOM_ANGLE );
        p.position += entity->position;
                
        p.velocity = (vec2_t){ 1.0f, 0.0f };
        p.velocity = p.velocity.rotated( RANDOM_ANGLE );
        p.velocity *= RandomFloat( 15.0f, 40.0f );
        p.velocity += entity->velocity;
        
        p.color = entity->colors.array[Random( 0, entity->colors.count )];
        p.lifespan = Random(FPS * 0.25, FPS * 1);
        
        debris.append( p );
    }
    
    SpawnParticles( entity->world, debris.buffer, debris.count );
}
    

void DestroyAsteroid( entity_t * asteroid ) {
    asteroid->state = ES_REMOVE;
    
    if ( asteroid->type == ENTITY_ASTEROID_SMALL ) {
        return;
    }
        
    entityType_t new_type;
    if ( asteroid->type == ENTITY_ASTEROID_LARGE ) {
        new_type = ENTITY_ASTEROID_MEDIUM;
    } else {
        new_type = ENTITY_ASTEROID_SMALL;
    }
    
    for ( int i = 0; i < 2; i++ ) {
        vec2_t pt = { 0, EntityRadius( asteroid ) };
        pt = pt.rotated( RANDOM_ANGLE );
        pt += asteroid->position;
        
        entity_t * a = SpawnEntity( asteroid->world, new_type, pt, RANDOM_ANGLE );
        a->angular_speed = asteroid->angular_speed;
        a->angular_speed *= RandomFloat( 2.0f, 4.0f );
        a->velocity = asteroid->velocity * RandomFloat( 1.5f, 2.0f );
        a->rotation += RandomFloat( M_PI / 4.0, -M_PI / 4.0);
        a->velocity = a->velocity.rotated( RandomFloat( -45.0f, 45.0f ) );
    }
}


void BulletContact( entity_t * bullet, entity_t * hit ) {
    switch ( hit->type ) {
        case ENTITY_ASTEROID_LARGE:
        case ENTITY_ASTEROID_MEDIUM:
        case ENTITY_ASTEROID_SMALL:
            bullet->state = ES_REMOVE;
            DestroyAsteroid( hit );
            ExplodeEntity( hit );
            break;
        default:
            break;
    }
}
