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

/*
	Initialized the input.
*/
int init_input(void);

/*
	Deinitializes the input.
*/
void deinit_input(void);

/*
	Checks if a button is pressed.
	@param btn	the button to check
*/
bool get_button(InputButton btn);

/*
	Checks if a button is pressed this frame.
	@param btn	the button to check
*/
bool get_button_down(InputButton btn);

#endif // #ifndef __INCLUDE_INPUT_H__
