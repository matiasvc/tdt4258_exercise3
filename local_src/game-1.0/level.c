#include "level.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "fixedptc.h"
#include "matrix.h"
#include "graphics.h"
#include "input.h"

#define MAX_GAMEOBJECTS 32
#define PLAYER_MOVEMENT_DAMPENING fixedpt_rconst(0.95)
#define PLAYER_ROTATION_DAMPENING fixedpt_rconst(0.9)

#define LARGE_METEOR_RADIUS 35
#define MEDIUM_METEOR_RADIUS 18
#define SMALL_METEOR_RADIUS 8

#define checkbit(X, i) ((X & (1 << i)) != 0)

GameObject *levelObjects[MAX_GAMEOBJECTS] = {};
uint32_t activeGameObjects = 0;

uint32_t spawnTimerLarge = 0;
uint32_t spawnTimerMedium = 0;
uint32_t spawnTimerSmall = 0;

#define LARGE_METEOR_SPAWN_TIME 1600
#define MEDIUM_METEOR_SPAWN_TIME 1200
#define SMALL_METEOR_SPAWN_TIME 800

#define SCREEN_CENTER new_vec(160, 120)

// Method prototypes

void create_new_game_object(GameObjectType goType, Vec2 pos, Vec2 vel, fixedpt rot, fixedpt rotVel);
inline fixedpt randomVal(void);
void spawn_meteor(GameObjectType goType);

// Method implementations

void update_game(fixedpt dt) {
	for (uint8_t i = 0; i < MAX_GAMEOBJECTS; ++i)
	{
		if (checkbit(activeGameObjects, i))
		{
			GameObject *go = levelObjects[i];
			draw_object(go->ro, go->pos, go->rot);

			go->pos = vec_vec_add(go->pos, go->vel);
			go->rot += go->rotVel;

			switch (go->type)
			{
				case GO_PLAYER:
				{

					if (go->pos.x < fixedpt_fromint(-5)) { go->pos.x = fixedpt_fromint(325); }
					else if (go->pos.x > fixedpt_fromint(350)) { go->pos.x = fixedpt_fromint(-5); }
					
					if (go->pos.y < fixedpt_fromint(-5)) { go->pos.y = fixedpt_fromint(245); }
					else if (go->pos.y > fixedpt_fromint(245)) { go->pos.y = fixedpt_fromint(-5); }

					if (get_button(BUTTON_UP))
					{
						go->vel = vec_vec_add(go->vel, vec_scalar_mul(get_rot_vec(go->rot + FIXEDPT_HALF_PI), fixedpt_rconst(0.25)));
						go->ro->vertecies[5] = new_vec( 0, -5);
					}
					else
					{
						go->ro->vertecies[5] = new_vec( 0, 0);
					}

					if (get_button(BUTTON_RIGHT))
					{
						go->rotVel += fixedpt_rconst(0.03);
					}

					if (get_button(BUTTON_LEFT))
					{
						go->rotVel -= fixedpt_rconst(0.03);
					}

					if (get_button_down(BUTTON_FIRE))
					{
						Vec2 rotVec = get_rot_vec(go->rot + FIXEDPT_HALF_PI);

						Vec2 projectilePos = vec_vec_add(go->pos, vec_scalar_mul(rotVec, fixedpt_fromint(10)));
						Vec2 projectileVel = vec_scalar_mul(rotVec, fixedpt_fromint(6));

						create_new_game_object(GO_PROJECTILE, projectilePos, projectileVel, go->rot, 0);
					}

					go->vel = vec_scalar_mul(go->vel, PLAYER_MOVEMENT_DAMPENING);
					go->rotVel = fixedpt_mul(go->rotVel, PLAYER_ROTATION_DAMPENING);
				} break;
				case GO_PROJECTILE:
				{
					if (go->pos.x < fixedpt_fromint(-5) ||
						go->pos.x > fixedpt_fromint(325) ||
						go->pos.y < fixedpt_fromint(-5) ||
						go->pos.y > fixedpt_fromint(245))
					{
						destroy_game_object(go);
					}
				} break;
				case GO_LARGE_METEOR:
				case GO_MEDIUM_METEOR:
				case GO_SMALL_METEOR:
				{
					if (go->pos.x < fixedpt_fromint(-100) ||
						go->pos.x > fixedpt_fromint(420) ||
						go->pos.y < fixedpt_fromint(-100) ||
						go->pos.y > fixedpt_fromint(340))
					{
						destroy_game_object(go);
					}
				} break;
			}

			// Spawn meteorites
			spawnTimerLarge++;
			spawnTimerMedium++;
			spawnTimerSmall++;
			
			if (spawnTimerSmall >= SMALL_METEOR_SPAWN_TIME)
			{
				spawn_meteor(GO_SMALL_METEOR);
				spawnTimerSmall = 0;
			}

			if (spawnTimerMedium >= MEDIUM_METEOR_SPAWN_TIME)
			{
				spawn_meteor(GO_MEDIUM_METEOR);
				spawnTimerMedium = 0;
			}

			if (spawnTimerLarge >= LARGE_METEOR_SPAWN_TIME)
			{
				spawn_meteor(GO_LARGE_METEOR);
				spawnTimerLarge = 0;
			}
		}
	}
}

void spawn_meteor(GameObjectType goType)
{
	fixedpt angle = fixedpt_mul(randomVal(), FIXEDPT_PI);
	Vec2 pos =  vec_vec_add(vec_scalar_mul(get_rot_vec(angle), fixedpt_fromint(220)), SCREEN_CENTER);
	Vec2 vel = vec_scalar_mul(get_rot_vec(angle + fixedpt_mul(randomVal(), fixedpt_rconst(0.7))), fixedpt_rconst(-3.0));

	printf("Spawning meteor at %.3f %.3f\n", fixedpt_tofloat(pos.x), fixedpt_tofloat(pos.y));

	switch (goType)
	{
		case GO_LARGE_METEOR:
		{
			Vec2 vel = vec_scalar_mul(get_rot_vec(angle + fixedpt_mul(randomVal(), fixedpt_rconst(0.7))), fixedpt_rconst(-0.8));
			create_new_game_object(GO_LARGE_METEOR, pos, vel, 0, fixedpt_mul(randomVal(), fixedpt_rconst(0.02)));
		} break;
		case GO_MEDIUM_METEOR:
		{
			Vec2 vel = vec_scalar_mul(get_rot_vec(angle + fixedpt_mul(randomVal(), fixedpt_rconst(0.7))), fixedpt_rconst(-1.5));
			create_new_game_object(GO_MEDIUM_METEOR, pos, vel, 0, fixedpt_mul(randomVal(), fixedpt_rconst(0.05)));
		} break;
		case GO_SMALL_METEOR:
		{
			Vec2 vel = vec_scalar_mul(get_rot_vec(angle + fixedpt_mul(randomVal(), fixedpt_rconst(0.7))), fixedpt_rconst(-3.0));
			create_new_game_object(GO_SMALL_METEOR, pos, vel, 0, fixedpt_mul(randomVal(), fixedpt_rconst(0.08)));
		} break;
		default:
		{
			printf("WARNING: Invalid meteor type\n");
		} break;
	}
}

void create_new_game_object(GameObjectType goType, Vec2 pos, Vec2 vel, fixedpt rot, fixedpt rotVel)
{
	int8_t goIndex = -1;

	for (uint8_t i = 0; i < MAX_GAMEOBJECTS; ++i)
	{
		if (!checkbit(activeGameObjects, i))
		{
			goIndex = i;
			activeGameObjects |= (1 << i);
			break;
		}
	}

	if (goIndex == -1)
	{
		printf("WARNING: Unable to instantiate another Game Object.\n");
		return;
	}

	RenderObject *ro = malloc(sizeof(RenderObject));
	GameObject *go = malloc(sizeof(GameObject));
	go->ro = ro;

	go->pos = pos;
	go->vel = vel;
	go->rot = rot;
	go->rotVel = rotVel;

	switch (goType)
	{
		case GO_PLAYER:
		{
			go->type = GO_PLAYER;

			uint8_t nVertecies = 7;
			ro->color = COLOR_WHITE;
			ro->vertecies = malloc(sizeof(Vec2) * nVertecies);
			ro->vertecies[0] = new_vec( 0,  0);
			ro->vertecies[1] = new_vec( 5, -2.5);
			ro->vertecies[2] = new_vec( 0,  9);
			ro->vertecies[3] = new_vec(-5, -2.5);
			ro->vertecies[4] = new_vec(-2.5, -1.25);
			ro->vertecies[5] = new_vec( 0, 0);
			ro->vertecies[6] = new_vec( 2.5, -1.25);
			ro->nVertecies = nVertecies;

			uint8_t nLinePoints = 12;
			ro->linePoints = malloc(sizeof(uint8_t) * nLinePoints);
			ro->linePoints[0] = 0;
			ro->linePoints[1] = 1;
			ro->linePoints[2] = 1;
			ro->linePoints[3] = 2;
			ro->linePoints[4] = 2;
			ro->linePoints[5] = 3;
			ro->linePoints[6] = 3;
			ro->linePoints[7] = 0;
			ro->linePoints[8] = 4;
			ro->linePoints[9] = 5;
			ro->linePoints[10] = 5;
			ro->linePoints[11] = 6;
			ro->nLinePoints = nLinePoints;
			
			uint16_t clearBufferSize = 128;
			ro->clearBuffer = malloc(sizeof(ScreenPoint)*clearBufferSize);
			ro->clearBufferIndex = 0;
			ro->clearBufferSize = clearBufferSize;
		} break;
		case GO_PROJECTILE:
		{
			go->type = GO_PROJECTILE;

			uint8_t nVertecies = 2;
			ro->color = COLOR_WHITE;
			ro->vertecies = malloc(sizeof(Vec2) * nVertecies);
			ro->vertecies[0] = new_vec( 0,  2);
			ro->vertecies[1] = new_vec( 0, -1);
			ro->nVertecies = nVertecies;

			uint8_t nLinePoints = 2;
			ro->linePoints = malloc(sizeof(uint8_t) * nLinePoints);
			ro->linePoints[0] = 0;
			ro->linePoints[1] = 1;
			ro->nLinePoints = nLinePoints;
			
			uint16_t clearBufferSize = 32;
			ro->clearBuffer = malloc(sizeof(ScreenPoint)*clearBufferSize);
			ro->clearBufferIndex = 0;
			ro->clearBufferSize = clearBufferSize;
		} break;
		case GO_LARGE_METEOR:
		{
			go->type = GO_LARGE_METEOR;

			uint8_t nVertecies = 20;
			uint8_t nLinePoints = nVertecies*2;
			ro->color = COLOR_WHITE;
			ro->vertecies = malloc(sizeof(Vec2) * nVertecies);
			ro->linePoints = malloc(sizeof(uint8_t) * nLinePoints);

			fixedpt step = fixedpt_div(FIXEDPT_TWO_PI, fixedpt_fromint(nVertecies));
			for (uint8_t i = 0; i < nVertecies; ++i)
			{
				fixedpt radius = fixedpt_fromint(LARGE_METEOR_RADIUS) + fixedpt_mul(randomVal(), fixedpt_rconst(7));
				ro->vertecies[i] = vec_scalar_mul(get_rot_vec(fixedpt_mul(step, fixedpt_fromint(i))), radius);
				ro->linePoints[2*i] = i;
				ro->linePoints[2*i+1] = (i+1) % nVertecies;
			}
			ro->nVertecies = nVertecies;
			ro->nLinePoints = nLinePoints;
			
			
			uint16_t clearBufferSize = 2048;
			ro->clearBuffer = malloc(sizeof(ScreenPoint)*clearBufferSize);
			ro->clearBufferIndex = 0;
			ro->clearBufferSize = clearBufferSize;
		} break;
		case GO_MEDIUM_METEOR:
		{
			go->type = GO_MEDIUM_METEOR;

			uint8_t nVertecies = 15;
			uint8_t nLinePoints = nVertecies*2;
			ro->color = COLOR_WHITE;
			ro->vertecies = malloc(sizeof(Vec2) * nVertecies);
			ro->linePoints = malloc(sizeof(uint8_t) * nLinePoints);

			fixedpt step = fixedpt_div(FIXEDPT_TWO_PI, fixedpt_fromint(nVertecies));
			for (uint8_t i = 0; i < nVertecies; ++i)
			{
				fixedpt radius = fixedpt_fromint(MEDIUM_METEOR_RADIUS) + fixedpt_mul(randomVal(), fixedpt_rconst(4));
				ro->vertecies[i] = vec_scalar_mul(get_rot_vec(fixedpt_mul(step, fixedpt_fromint(i))), radius);
				ro->linePoints[2*i] = i;
				ro->linePoints[2*i+1] = (i+1) % nVertecies;
			}
			ro->nVertecies = nVertecies;
			ro->nLinePoints = nLinePoints;
			
			
			uint16_t clearBufferSize = 2048;
			ro->clearBuffer = malloc(sizeof(ScreenPoint)*clearBufferSize);
			ro->clearBufferIndex = 0;
			ro->clearBufferSize = clearBufferSize;
		} break;
		case GO_SMALL_METEOR:
		{
			go->type = GO_SMALL_METEOR;

			uint8_t nVertecies = 8;
			uint8_t nLinePoints = nVertecies*2;
			ro->color = COLOR_WHITE;
			ro->vertecies = malloc(sizeof(Vec2) * nVertecies);
			ro->linePoints = malloc(sizeof(uint8_t) * nLinePoints);

			fixedpt step = fixedpt_div(FIXEDPT_TWO_PI, fixedpt_fromint(nVertecies));
			for (uint8_t i = 0; i < nVertecies; ++i)
			{
				fixedpt radius = fixedpt_fromint(SMALL_METEOR_RADIUS) + fixedpt_mul(randomVal(), fixedpt_rconst(2));
				ro->vertecies[i] = vec_scalar_mul(get_rot_vec(fixedpt_mul(step, fixedpt_fromint(i))), radius);
				ro->linePoints[2*i] = i;
				ro->linePoints[2*i+1] = (i+1) % nVertecies;
			}
			ro->nVertecies = nVertecies;
			ro->nLinePoints = nLinePoints;
			
			
			uint16_t clearBufferSize = 2048;
			ro->clearBuffer = malloc(sizeof(ScreenPoint)*clearBufferSize);
			ro->clearBufferIndex = 0;
			ro->clearBufferSize = clearBufferSize;
		} break;
		default:
		{
			printf("WARNING: Unknown Game Object type.\n");
		} break;
	}

	levelObjects[goIndex] = go;
}

void destroy_game_object(GameObject *go)
{
	for (uint8_t i = 0; i < 16; ++i)
	{
		if (levelObjects[i] == go)
		{
			activeGameObjects &= ~(1 << i);
			break;
		}
	}
	
	RenderObject *ro = go->ro;

	free(ro->vertecies);
	free(ro->linePoints);
	free(ro->clearBuffer);
	free(ro);
	free(go);
}

inline fixedpt randomVal(void)
{
	fixedpt fpVal = fixedpt_fromint((rand() % RAND_MAX) % 65535);
	fpVal = fixedpt_div(fpVal, fixedpt_rconst(32767.5));
	return fpVal;
}





