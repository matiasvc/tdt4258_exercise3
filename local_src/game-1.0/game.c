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

int main(int argc, char *argv[])
{
	init_graphics();
	init_input();

	create_new_game_object(GO_PLAYER, new_vec(160, 120), new_vec(0, 0), 0, 0);

	struct timeval tv;
	while (1)
	{
		gettimeofday(&tv, NULL);
		unsigned long timeBeforeRender = (unsigned long)(tv.tv_sec) * 1000000 + (unsigned long)(tv.tv_usec);

		update_game(fixedpt_div(FIXEDPT_ONE, fixedpt_fromint(30)));

		gettimeofday(&tv, NULL);
		unsigned long timeAfterRender = (unsigned long)(tv.tv_sec) * 1000000 + (unsigned long)(tv.tv_usec);
		unsigned long diff = timeAfterRender - timeBeforeRender;

		if (diff < FRAME_TIME)
		{
			usleep(FRAME_TIME - diff);	
		}
	}

	exit(EXIT_SUCCESS);
}
