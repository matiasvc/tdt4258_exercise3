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

#define FB_DRAW 0x4680

int fbfd;
uint16_t* fbp;

struct fb_var_screeninfo vinfo;
struct fb_copyarea screen;

int fb_nPixels;
int fb_nBytes;

// method implementations
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

	screen.dx = 0;
	screen.dy = 0;
	screen.width = vinfo.xres;
	screen.height = vinfo.yres;

	fb_nPixels = vinfo.xres * vinfo.yres;
	fb_nBytes = fb_nPixels * vinfo.bits_per_pixel / 8;

	fbp = (uint16_t*)mmap(NULL, fb_nBytes, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if (fbp == MAP_FAILED) {
	    printf("ERROR: Unable to memorymap framebuffer.\n");
	    return EXIT_FAILURE;
	}

	// Write black to the entire screen
	memset(fbp, 0x00, fb_nBytes);
	refresh_fb();

	return EXIT_SUCCESS;
}


inline void draw_pixel(int16_t x, int16_t y, uint16_t color, RenderObject *ro)
{
	if (x < 0 || x > (int16_t)screen.width || y < 0 || y > (int16_t)screen.height) { return; }

	if (ro->clearBufferIndex >= ro->clearBufferSize)
	{
		printf("ERROR: Clear buffer is full. Unable to draw pixel\n");
		return;
	}

	fbp[x + (y * screen.width)] = color;

	ro->clearBuffer[ro->clearBufferIndex].x = x;
	ro->clearBuffer[ro->clearBufferIndex].y = (uint8_t) y;
	ro->clearBufferIndex++;
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
	if (ro->ctrlVector & (1 << RENDER_OBJECT_CTRL_CLEAR)) // Should we clear
	{
		for (uint16_t i = 0; i < ro->clearBufferIndex; ++i)
		{
			uint16_t x = ro->clearBuffer[i].x;
			uint16_t y = ro->clearBuffer[i].y;
			fbp[x + (y * screen.width)] = COLOR_BLACK;
		}
		ro->clearBufferIndex = 0;
		ro->ctrlVector |= ~(1 << RENDER_OBJECT_CTRL_CLEAR);
	}

	if (ro->ctrlVector & (1 << RENDER_OBJECT_CTRL_DRAW)) // Should we render
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
		ro->ctrlVector |= ~(1 << RENDER_OBJECT_CTRL_DRAW);
	}

}

void deinit_graphics(void)
{
    munmap(fbp, fb_nBytes);
    close(fbfd);
}

void refresh_fb(void)
{
    ioctl(fbfd, FB_DRAW, &screen);
}

