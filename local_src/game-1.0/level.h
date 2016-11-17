#ifndef __INCLUDE_LEVEL_H__
#define __INCLUDE_LEVEL_H__

#include "graphics.h"
#include "matrix.h"
#include "fixedptc.h"
#include "level.h"

typedef enum GameObjectType_t {
	GO_PLAYER = 0,
	GO_LARGE_METEOR = 1,
	GO_MEDIUM_METEOR = 2,
	GO_SMALL_METEOR = 3,
	GO_PROJECTILE = 4
} GameObjectType;

typedef struct
{
	Vec2 pos;
	Vec2 vel;
	fixedpt rot; // Rotation in radians
	fixedpt rotVel; // Rotation velocity in rad/s
	fixedpt radius; // Colission radius og the object squared

	GameObjectType type;

	RenderObject *ro;
} GameObject;

/*
	Resets the game
*/
void reset_game(void);

/*
	Updates the game. Should be called at 30Hz.
	@param game_over	a pointer to a uint8_t that will be set to 1 when the game is over
*/
void update_game(uint8_t *game_over);

#endif // #ifndef __INCLUDE_LEVEL_H__
