#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <asm/fcntl.h>
#include <unistd.h>

#include "input.h"

#define checkbit(X, i) ((X & (1 << i)) != 0)

FILE* device;
uint8_t lastInput = 0;

uint8_t inputStatus = 0;
uint8_t inputDownStatus = 0;

// Method prototypes
void io_handler(int sigio);


// Method implementations
int init_input(void)
{
	device = fopen("/dev/GPIOGamepad", "rb");

	if (!device)
	{
		printf("ERROR: Unable to open gamepad device driver.\n");
		return EXIT_FAILURE;
	}

	if (signal(SIGIO, &io_handler) == SIG_ERR)
	{
		printf("ERROR: Unable to register io handler.\n");
		return EXIT_FAILURE;
	}

	if (fcntl(fileno(device), F_SETOWN, getpid()) == -1)
	{
		printf("ERROR: Unable to set file owner.\n");
		return EXIT_FAILURE;
	}

	long oflags = fcntl(fileno(device), F_GETFL);
	if (fcntl(fileno(device), F_SETFL, oflags | FASYNC) == -1)
	{
	    printf("ERROR: Unable to set FASYNC flag.\n");
	    return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;

}


void deinit_input(void)
{
	fclose(device);
}

bool get_button(InputButton btn)
{
	return (inputStatus & (1 << (uint8_t)btn)) != 0;
}

bool get_button_down(InputButton btn)
{
	if (checkbit(inputDownStatus, (uint8_t)btn))
	{
		inputDownStatus &= ~(1 << (uint8_t)btn);
		return true;
	}
	else
	{
		return false;
	}
}

void io_handler(int sigio)
{
	uint8_t input = fgetc(device) ^ 0xFF;
	
	for (uint8_t i = 0; i < 8; ++i)
	{
		if (checkbit(input, i) && !checkbit(lastInput, i)) // Press
		{
			inputStatus |= (1 << i);
			inputDownStatus |= (1 << i);
		}
		else if (!checkbit(input, i) && checkbit(lastInput, i)) // Release
		{
			inputStatus &= ~(1 << i);
			inputDownStatus &= ~(1 << i);
		}
	}
	
	lastInput = input;
}
