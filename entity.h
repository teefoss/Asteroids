#ifndef entity_h
#define entity_h

#include "vec2.h"
#include "draw.h"
#include "sprite.h"

typedef enum entity_type
{
    ENTITY_PLAYER,
    ENTITY_ASTEROID_LARGE,
    ENTITY_ASTEROID_MEDIUM,
    ENTITY_ASTEROID_SMALL,
//  ENTITY_ALIEN,
    ENTITY_BULLET,
    NUM_ENTITY_TYPES,
} entityType_t;

typedef enum {
    ES_ACTIVE,
    ES_RESPAWNING,
    ES_APPEARING,
    ES_REMOVE
} entityState_t;

#define FL_NO_WRAP  0x01

typedef struct {
    int shot_timer; // shot cooldown
    int respawn_frame; // if ES_RESPAWING, respawn at the this frame
} playerInfo_t;

typedef struct world world_t;
typedef struct entity entity_t;

//typedef struct {
//    int num_ticks;
//    void (* update)(entity_t * self, float dt);
//    void (* contact)(entity_t * self, entity_t * hit);
//} entityState_t;

struct entity {
    entityType_t    type;
//    entityState_t * state;
    entityState_t   state;
        
    vec2_t          position;
    vec2_t          velocity;
    float           rotation; // in radians
    float           angular_speed;
    float           radius; // use EntityRadius() to read
    float           scale;

    int             flags;
    
    const char *    sprite_name;
    spriteColors_t  colors;
    world_t *       world;
    
    union {
        playerInfo_t  player;
    } info;
    
    void (* update)(entity_t * self, float dt);
    void (* contact)(entity_t * self, entity_t * hit);
    void (* draw)(entity * self);
};

extern const entity_t entity_defs[];

float   EntityRadius( entity_t * e );
void    GeneralUpdateEntity( entity_t * entity, float dt );
void    UpdateEntity(entity_t *, float dt);
void    DrawEntity(entity_t *);
vec2_t  EntityForward( entity_t * entity );
void    ExplodeEntity( entity_t * entity );
bool    EntitiesAreColliding( entity_t * a, entity_t * b );

#endif /* entity_h */
