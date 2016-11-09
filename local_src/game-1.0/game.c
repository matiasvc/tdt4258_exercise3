#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "graphics.h"
#include "matrix.h"

#include "fixedptc.h"

int main(int argc, char *argv[])
{
	init_graphics();

	Vec2 points[8];

	points[0] = new_vec(-5, -5);
	points[1] = new_vec( 5, -5);
	points[2] = new_vec( 5,  5);
	points[3] = new_vec( 0, 10);
	points[4] = new_vec(-5,  5);
	points[5] = new_vec( 0, -5);
	points[6] = new_vec(-5, -10);
	points[7] = new_vec( 5, -10);


	uint8_t lines[] = {
		0, 1,
		1, 2,
		2, 3,
		3, 4,
		4, 0,
		5, 6,
		6, 7,
		7, 5
	};

	Vec2 trans = new_vec(160, 120);

	fixedpt theta = 0;
	fixedpt dtheta = fixedpt_rconst(0.05);


	RenderObject square = {};
	square.color = COLOR_WHITE;
	square.vertecies = &points;
	square.nVertecies = 8;
	square.linePoints = &lines;
	square.nLinePoints = 16;
	square.clearBuffer = malloc(sizeof(ScreenPoint)*4096);
	square.clearBufferIndex = 0;
	square.clearBufferSize = 4096;

	while (1)
	{
		square.ctrlVector = 0b00000011;
		draw_object(&square, trans, theta);

		trans.x += fixedpt_rconst(3);
		if (trans.x > fixedpt_rconst(200))
		{
			trans.x = fixedpt_rconst(40);
		}

		theta += dtheta;
		refresh_fb();
	}

	free(square.clearBuffer);

	printf("Hello World, I'm a game!\n");
	exit(EXIT_SUCCESS);
}
