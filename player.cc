#include "player.h"
#include "defines.h"
#include "world.h"
#include "entity.h"
#include "game.h"

#define PLAYER_THRUST 100.0f
#define PLAYER_ROTATION DEG2RAD(180)
#define PLAYER_SHOT_TIME (FPS / 2)
#define BULLET_VELOCITY 100.0f

void ResetPlayer( entity_t * player ) {
    player->info.player.shot_timer = 0;
    player->position.x = GAME_WIDTH / 2;
    player->position.y = GAME_HEIGHT / 2;
    player->velocity.zero();
    player->rotation = 270.0f;
}


void PlayerContact( entity_t * player, entity_t * hit ) {
    switch ( hit->type ) {
        case ENTITY_ASTEROID_LARGE:
        case ENTITY_ASTEROID_MEDIUM:
        case ENTITY_ASTEROID_SMALL:
            ExplodeEntity( player );
            player->state = ES_RESPAWNING;
            ResetPlayer( player );
            player->scale = 0.0f;
            break;
        default:
            break;
    }
}


static void ShootBullet( entity_t * player ) {
    vec2_t forward = EntityForward( player );
    vec2_t pt = forward;
    pt *= EntityRadius( player );
    pt += player->position;

    entity_t * bullet = SpawnEntity( player->world, ENTITY_BULLET, pt, 0.0f );
    bullet->velocity = forward * BULLET_VELOCITY;
    
    player->info.player.shot_timer = PLAYER_SHOT_TIME;
}


void DoPlayerInput( entity_t * player, float dt ) {
    playerInfo_t * info = &player->info.player;
    const u8 * keys = SDL_GetKeyboardState( NULL );
    
    if ( keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT] ) {
        player->rotation -= PLAYER_ROTATION * dt;
    }

    if ( keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT] ) {
        player->rotation += PLAYER_ROTATION * dt;
    }
    
    if ( keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP] ) {
        vec2_t thrust = ( EntityForward( player ) * PLAYER_THRUST ) * dt;
        player->velocity += thrust;
        
        int num_particles = 1;
        Array<particle_t> exhaust( num_particles );
        for ( int i = 0; i < num_particles; i++ ) {
            vec2_t back = -(EntityForward( player )) * EntityRadius( player );
            
            particle_t p;
            p.position = back + player->position;
            float offset = 1.0f;
            p.position.x += RandomFloat( -offset, offset );
            p.position.y += RandomFloat( -offset, offset );
            
            p.velocity = back * RandomFloat( 10.0f, 15.0f );
            int span = 10;
            p.velocity = p.velocity.rotated( Random( -span, span) );
            
            p.lifespan = Random( FPS * 0.5, FPS * 1 );
            p.color = Random( 0, 2) ? COLOR_YELLOW : COLOR_BRIGHT_RED;
            
            exhaust.append( p );
        }
        
        SpawnParticles( player->world, exhaust.buffer, exhaust.count );
    }
    
    if ( keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN] ) {
        player->velocity *= 0.975f;
        if ( player->velocity.length() <= 1.0f ) {
            player->velocity.zero();
        }
    }
    
    if ( keys[SDL_SCANCODE_SPACE] ) {
        if ( info->shot_timer == 0 ) {
            ShootBullet( player );
        }
    }
}


/*
 * check if the spawn point will be blocked at any
 * point in the next n seconds
 */
bool SpawnPointBlocked( entity_t * player, int seconds, float dt ) {
    
    Array<entity_t> entities = *player->world->entities;
    
    // run a simulation to see what's coming
    for ( int frame = 0; frame < FPS * seconds; frame++ ) {
        for ( int i = 0; i < entities.count; i++ ) {
            entity_t * e = &entities.buffer[i];
            
            if ( e->type != ENTITY_PLAYER ) {
                // don't make an changes to the world
                GeneralUpdateEntity( e, dt );
                
                if ( EntitiesAreColliding( e, player ) ) {
                    return true; // blocked
                }
            }
        }
    }
    
    return false;
}


void UpdatePlayer( entity_t * player, float dt ) {
    
    switch ( player->state ) {
        case ES_ACTIVE: {
            DoPlayerInput( player, dt );
            
            if ( player->info.player.shot_timer > 0 ) {
                --player->info.player.shot_timer;
            }
            break;
        }
        case ES_RESPAWNING: {
            if ( !SpawnPointBlocked( player, 3, dt ) ) {
                player->state = ES_APPEARING; // respawn
            }
            break;
        }
        case ES_APPEARING: {
            if ( player->scale < 1.0f ) {
                player->scale += 1.5f * dt;
            } else {
                player->scale = 1.0f;
                player->state = ES_ACTIVE;
            }
            break;
        }
        default:
            break;
    }
}
