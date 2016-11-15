#ifndef __INCLUDE_LEVEL_H__
#define __INCLUDE_LEVEL_H__

#include "graphics.h"
#include "matrix.h"
#include "fixedptc.h"

typedef enum GameObjectType_t {
	GO_PLAYER, GO_LARGE_METEOR, GO_MEDIUM_METEOR, GO_SMALL_METEOR, GO_PROJECTILE
} GameObjectType;

typedef struct
{
	Vec2 pos;
	Vec2 vel;
	fixedpt rot; // Rotation in radians
	fixedpt rotVel; // Rotation velocity in rad/s

	GameObjectType type;

	RenderObject *ro;
} GameObject;

#endif // #ifndef __INCLUDE_LEVEL_H__
