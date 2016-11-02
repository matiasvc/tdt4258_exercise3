#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <linux/fb.h>
#include <fcntl.h>

int fbFile;
uint16_t* fbp;
int screen_pixels;
int screen_bytes;

struct fb_var_screeninfo vinfo;
struct fb_copyarea screen;

int main(int argc, char *argv[])
{
	printf("Hello World, I'm game!\n");

	fbFile = open("/dev/fb0", O_RDWR);

	ioctl(fbFile, FBIOGET_VSCREENINFO, &vinfo);
	printf("Screeninfo: %d x %d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

	screen.dx = 0;
	screen.dy = 0;
	screen.width = vinfo.xres;
	screen.height = vinfo.yres;

	screen_pixels = vinfo.xres * vinfo.yres;
	screen_bytes = screen_pixels * vinfo.bits_per_pixel / 8;

	fbp = (uint16_t*)mmap(NULL, screensize_bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	
	exit(EXIT_SUCCESS);
}
