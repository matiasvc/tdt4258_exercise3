#ifndef __INCLUDE_GRAPHICS_H__
#define __INCLUDE_GRAPHICS_H__

#include <stdint.h>

#include "matrix.h"

#define COLOR_BLACK   0x0000
#define COLOR_WHITE   0xFFFF
#define COLOR_RED     0xF800

#define RENDER_OBJECT_CTRL_CLEAR	0
#define RENDER_OBJECT_CTRL_DRAW		1

typedef struct
{
	uint16_t x;
	uint8_t y;
} ScreenPoint;

typedef struct
{
	uint16_t xLow;
	uint16_t xHigh;
	uint16_t yLow;
	uint16_t yHigh;
} UpdateRect;

typedef struct
{
	uint8_t ctrlFlags;
	uint16_t color;

	Vec2 *vertecies;
	uint8_t nVertecies;
	uint8_t *linePoints;
	uint8_t nLinePoints;
	
	ScreenPoint *clearBuffer;
	uint16_t clearBufferIndex;
	uint16_t clearBufferSize;

	UpdateRect updateArea;
} RenderObject;

/*
	Initializes the graphics.
*/
int init_graphics(void);

/*
	Deinitalized the graphics.
*/
void deinit_graphics(void);

/*
	Draws a RenderObject.
	@param ro	pointer to the RenderObject to draw
	@param pos	the position to draw the object at
	@param rot	the rotation to draw the object at
*/
void draw_object(RenderObject *ro, Vec2 pos, fixedpt rot);

/*
	Draws the game over screen.
*/
void draw_game_over(void);

/*
	Clears the screen to black.
*/
void clear_screen(void);


#endif // #ifndef __INCLUDE_GRAPHICS_H__
