#ifndef player_h
#define player_h

#include "entity.h"

void UpdatePlayer( entity_t * player, float dt );
void PlayerContact( entity_t * player, entity_t * hit );
void ResetPlayer( entity_t * player );

#endif /* player_h */
