#ifndef __INCLUDE_INPUT_H__
#define __INCLUDE_INPUT_H__

#include <stdbool.h>

typedef enum InputButton_t
{
	BUTTON_UP = 1,
	BUTTON_DOWN = 3,
	BUTTON_RIGHT = 2,
	BUTTON_LEFT = 0,
	BUTTON_FIRE = 7
} InputButton;

int init_input(void);
void deinit_input(void);
bool get_button(InputButton btn);
bool get_button_down(InputButton btn);

#endif // #ifndef __INCLUDE_INPUT_H__
