#ifndef __INCLUDE_GRAPHICS_H__
#define __INCLUDE_GRAPHICS_H__

#include <stdint.h>

#include "matrix.h"

#define COLOR_BLACK           0x0000
#define COLOR_WHITE           0xFFFF
#define COLOR_DARKGREY        0x7BEF
#define COLOR_LIGHTGREY       0xC618
#define COLOR_YELLOW          0xFFE0
#define COLOR_ORANGE          0xFD20
#define COLOR_RED             0xF800
#define COLOR_MAROON          0x7800
#define COLOR_PINK            0xF81F
#define COLOR_GREENYELLOW     0xAFE5
#define COLOR_GREEN           0x07E0
#define COLOR_DARKGREEN       0x03E0
#define COLOR_DARKCYAN        0x03EF
#define COLOR_CYAN            0x07FF
#define COLOR_BLUE            0x001F
#define COLOR_NAVY            0x000F
#define COLOR_PURPLE          0x780F
#define COLOR_OLIVE           0x7BE0

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

int init_graphics(void);
void deinit_graphics(void);
void draw_object(RenderObject *ro, Vec2 pos, fixedpt rot);


#endif // #ifndef __INCLUDE_GRAPHICS_H__
