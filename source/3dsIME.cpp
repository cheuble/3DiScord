#include <stdio.h>
#include <3ds.h>

#include "3dsIME.hpp"




std::string D3DSIME::getUserText(char title[]) {
	return getUserText(title, "");
}

std::string D3DSIME::getUserText(char title[], char *showtext) {

	char uInput[1000];

	if (title == "Enter your 2Factor Auth Code!")
		type = SWKBD_TYPE_NUMPAD;
	else
		type = SWKBD_TYPE_WESTERN;


	swkbdInit(&swkbd, type, 2, type == SWKBD_TYPE_NUMPAD ? 6 : -1);

	if (title == "Discord Password")
		swkbdSetPasswordMode(&swkbd, SWKBD_PASSWORD_HIDE_DELAY);
	swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
	swkbdSetFeatures(&swkbd, SWKBD_MULTILINE);
	swkbdSetHintText(&swkbd, title);
	button = swkbdInputText(&swkbd, uInput, sizeof(uInput));

	if (button = SWKBD_BUTTON_LEFT)
		return "";

	return uInput;
}


D3DSIME::D3DSIME() {

}