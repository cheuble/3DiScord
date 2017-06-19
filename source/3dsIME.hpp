#ifndef D3DSIME_HPP
#define D3DSIME_HPP


#include <stdio.h>
#include <string>


class D3DSIME {
public:
	D3DSIME();
	std::string getUserText(char title[]);
	std::string getUserText(char title[], char *showtext);

private:
	SwkbdType type;
	SwkbdState swkbd;
	SwkbdStatusData swkbdStatus;
	SwkbdButton button = SWKBD_BUTTON_NONE;

};
#endif