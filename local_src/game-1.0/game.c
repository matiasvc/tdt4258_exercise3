#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include "graphics.h"
#include "matrix.h"
#include "input.h"
#include "level.h"

#include "fixedptc.h"

#define FRAME_TIME 33333

uint8_t gameOver = 0;

int main(int argc, char *argv[])
{
	init_graphics();
	init_input();

	reset_game();

	struct timeval tv;
	while (1)
	{
		gettimeofday(&tv, NULL);
		unsigned long timeBeforeRender = (unsigned long)(tv.tv_sec) * 1000000 + (unsigned long)(tv.tv_usec);

		update_game(&gameOver);

		gettimeofday(&tv, NULL);
		unsigned long timeAfterRender = (unsigned long)(tv.tv_sec) * 1000000 + (unsigned long)(tv.tv_usec);
		unsigned long diff = timeAfterRender - timeBeforeRender;

		if (diff < FRAME_TIME)
		{
			usleep(FRAME_TIME - diff);	
		}

		if (gameOver)
		{
			for (uint16_t i = 0; i < 90; ++i)
			{
				usleep(FRAME_TIME);
			}
			reset_game();
			gameOver = 0;
		}
	}

	exit(EXIT_SUCCESS);
}
