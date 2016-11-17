#include "graphics.h"
#include "fixedptc.h"
#include "matrix.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <string.h>
#include <math.h>

#include <unistd.h>

#define FB_DRAW 0x4680

#define checkbit(X, i) ((X & (1 << i)) != 0)

int fbfd;
uint16_t* fbp;

struct fb_var_screeninfo vinfo;
struct fb_copyarea copyarea;

int fb_nPixels;
int fb_nBytes;

// Method prototypes
void refresh_fb(struct fb_copyarea *copyarea);

// Method implementations
int init_graphics(void)
{
	fbfd = open("/dev/fb0", O_RDWR);

	if (fbfd == -1)
	{
		printf("ERROR: Unable to open /dev/fb0\n");
		return EXIT_FAILURE;
	}

	// Get screen size
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
	    printf("ERROR: Unable to aquire screen info.\n");
	    return EXIT_FAILURE;
	}

	printf("Screeninfo: %d x %d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

	

	fb_nPixels = vinfo.xres * vinfo.yres;
	fb_nBytes = fb_nPixels * vinfo.bits_per_pixel / 8;

	fbp = (uint16_t*)mmap(NULL, fb_nBytes, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if (fbp == MAP_FAILED) {
	    printf("ERROR: Unable to memorymap framebuffer.\n");
	    return EXIT_FAILURE;
	}	

	clear_screen();

	return EXIT_SUCCESS;
}

void clear_screen(void)
{
	copyarea.dx = 0;
	copyarea.dy = 0;
	copyarea.width = vinfo.xres;
	copyarea.height = vinfo.yres;

	memset(fbp, 0x00, fb_nBytes);
	refresh_fb(&copyarea);
}

inline void draw_pixel(int16_t x, int16_t y, uint16_t color, RenderObject *ro)
{
	// Ignore pixels outside the screen
	if (x < 0 || x > (int16_t)vinfo.xres || y < 0 || y > (int16_t)vinfo.yres) { return; }

	if (ro && ro->clearBufferIndex >= ro->clearBufferSize)
	{
		printf("ERROR: Clear buffer is full. Unable to draw pixel\n");
		return;
	}

	fbp[x + (y * vinfo.xres)] = color;

	if (ro)
	{
		if (x < ro->updateArea.xLow)  { ro->updateArea.xLow  = x; }
		if (x > ro->updateArea.xHigh) { ro->updateArea.xHigh = x; }
		if (y < ro->updateArea.yLow)  { ro->updateArea.yLow  = y; }
		if (y > ro->updateArea.yHigh) { ro->updateArea.yHigh = y; }

		ro->clearBuffer[ro->clearBufferIndex].x = x;
		ro->clearBuffer[ro->clearBufferIndex].y = (uint8_t) y;
		ro->clearBufferIndex++;
	}
}

void draw_line(Vec2 v0, Vec2 v1, uint16_t color, RenderObject *ro)
{
	int16_t x0 = fixedpt_toint(v0.x + FIXEDPT_ONE_HALF);
	int16_t y0 = fixedpt_toint(v0.y + FIXEDPT_ONE_HALF);
	int16_t x1 = fixedpt_toint(v1.x + FIXEDPT_ONE_HALF);
	int16_t y1 = fixedpt_toint(v1.y + FIXEDPT_ONE_HALF);

	int16_t dx = abs(x1-x0), sx = x0 < x1 ? 1 : -1;
	int16_t dy = abs(y1-y0), sy = y0 < y1 ? 1 : -1; 
	int16_t err = (dx > dy ? dx : -dy)/2;

	while(1)
	{
		draw_pixel(x0, y0, color, ro);
		if (x0 == x1 && y0 == y1) break;
		int e2 = err;
		if (e2 >-dx) { err -= dy; x0 += sx; }
		if (e2 < dy) { err += dx; y0 += sy; }
	}
}

void draw_object(RenderObject *ro, Vec2 pos, fixedpt rot)
{
	if (checkbit(ro->ctrlFlags, RENDER_OBJECT_CTRL_CLEAR))
	{
		ro->updateArea.xLow = vinfo.xres;
		ro->updateArea.xHigh = 0;
		ro->updateArea.yLow = vinfo.yres;
		ro->updateArea.yHigh = 0;

		for (uint16_t i = 0; i < ro->clearBufferIndex; ++i)
		{
			uint16_t x = ro->clearBuffer[i].x;
			uint16_t y = ro->clearBuffer[i].y;
			fbp[x + (y * vinfo.xres)] = COLOR_BLACK;

			if (x < ro->updateArea.xLow)  { ro->updateArea.xLow  = x; }
			if (x > ro->updateArea.xHigh) { ro->updateArea.xHigh = x; }
			if (y < ro->updateArea.yLow)  { ro->updateArea.yLow  = y; }
			if (y > ro->updateArea.yHigh) { ro->updateArea.yHigh = y; }
		}
		ro->clearBufferIndex = 0;
		ro->ctrlFlags &= ~(1 << RENDER_OBJECT_CTRL_CLEAR);
	}

	if (checkbit(ro->ctrlFlags, RENDER_OBJECT_CTRL_DRAW))
	{
		Mat2x2 rotMat = get_rotation_matrix(rot);

		Vec2 vertecies[ro->nVertecies];

		for (int i = 0; i < ro->nVertecies; ++i)
		{
			vertecies[i] = vec_vec_add(mat_vec_mul(rotMat, ro->vertecies[i]), pos);
		}

		for (int i = 0; i < ro->nLinePoints; i+=2)
		{
			Vec2 from = vertecies[ro->linePoints[i]];
			Vec2 to = vertecies[ro->linePoints[i+1]];

			draw_line(from, to, ro->color, ro);
		}
		ro->ctrlFlags &= ~(1 << RENDER_OBJECT_CTRL_DRAW);
	}


	int16_t draw_dx = ro->updateArea.xLow - 3;
	int16_t draw_dy = ro->updateArea.yLow - 3;
	int16_t draw_width = ro->updateArea.xHigh - ro->updateArea.xLow + 6;
	int16_t draw_height = ro->updateArea.yHigh - ro->updateArea.yLow + 6;

	if (draw_dx < 0) { draw_dx = 0; }
	if (draw_dy < 0) { draw_dy = 0; }
	if (draw_dx + draw_width > vinfo.xres) { draw_width = vinfo.xres - draw_dx; } 
	if (draw_dy + draw_height > vinfo.yres) { draw_height = vinfo.yres - draw_dy; } 

	copyarea.dx = draw_dx;
	copyarea.dy = draw_dy;
	copyarea.width = draw_width; 
	copyarea.height = draw_height;
	
	refresh_fb(&copyarea);
}

void draw_game_over(void)
{

	for (uint16_t x = 90; x < 220; ++x)
	{
		for (uint16_t y = 40; y < 110; ++y)
		{
			fbp[x + (y * vinfo.xres)] = COLOR_BLACK;
		}
	}

	Vec2 points[43];

	points[0] = new_vec(0, 0);
	points[1] = new_vec(2, 0);
	points[2] = new_vec(1, 1);
	points[3] = new_vec(2, 1);
	points[4] = new_vec(0, 2);
	points[5] = new_vec(2, 2);
	points[6] = new_vec(3, 0);
	points[7] = new_vec(5, 0);
	points[8] = new_vec(3, 1);
	points[9] = new_vec(5, 1);
	points[10] = new_vec(3, 2);
	points[11] = new_vec(5, 2);
	points[12] = new_vec(6, 0);
	points[13] = new_vec(7, 0);
	points[14] = new_vec(8, 0);
	points[15] = new_vec(6, 2);
	points[16] = new_vec(7, 2);
	points[17] = new_vec(8, 2);
	points[18] = new_vec(9, 0);
	points[19] = new_vec(11, 0);
	points[20] = new_vec(9, 1);
	points[21] = new_vec(11, 1);
	points[22] = new_vec(9, 2);
	points[23] = new_vec(11, 2);
	points[24] = new_vec(0, 3);
	points[25] = new_vec(2, 3);
	points[26] = new_vec(0, 5);
	points[27] = new_vec(2, 5);
	points[28] = new_vec(3, 3);
	points[29] = new_vec(5, 3);
	points[30] = new_vec(4, 5);
	points[31] = new_vec(6, 3);
	points[32] = new_vec(8, 3);
	points[33] = new_vec(6, 4);
	points[34] = new_vec(8, 4);
	points[35] = new_vec(6, 5);
	points[36] = new_vec(8, 5);
	points[37] = new_vec(9, 3);
	points[38] = new_vec(11, 3);
	points[39] = new_vec(9, 4);
	points[40] = new_vec(11, 4);
	points[41] = new_vec(9, 5);
	points[42] = new_vec(11, 5);

	uint8_t lines[] = {
		1, 0,
		0, 4,
		4, 5,
		5, 3,
		3, 2,
		7, 6,
		9, 8,
		6, 10,
		7, 11,
		14,12,
		12,15,
		13,16,
		14,17,
		18,19,
		20,21,
		22,23,
		18,22,
		25,24,
		24,26,
		26,27,
		27,25,
		28,30,
		30,29,
		32,31,
		34,33,
		36,35,
		31,35,
		38,37,
		40,39,
		37,41,
		39,42,
		38,40
	};

	Vec2 move = new_vec(100, 50);

	for (uint8_t i = 0; i < 64; i+= 2)
	{
		Vec2 from = points[lines[i]];
		Vec2 to = points[lines[i+1]];

		from = vec_vec_add(vec_scalar_mul(from, fixedpt_fromint(10)), move);
		to = vec_vec_add(vec_scalar_mul(to, fixedpt_fromint(10)), move);

		draw_line(from, to, COLOR_RED, NULL);
	}

	copyarea.dx = 90;
	copyarea.dy = 40;
	copyarea.width = 131; 
	copyarea.height = 71;
	refresh_fb(&copyarea);
}

void deinit_graphics(void)
{
    munmap(fbp, fb_nBytes);
    close(fbfd);
}

void refresh_fb(struct fb_copyarea *copyarea)
{
    ioctl(fbfd, FB_DRAW, copyarea);
}

