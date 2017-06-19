#ifndef D3DSTOUCH_HPP
#define D3DSTOUCH_HPP

#include <3ds.h>
#include <string.h>
#include "mathtools.hpp"

#define SLEEP_CLICK_NORMAL 150000
#define SLEEP_CLICK_EXTENDED 350000

class D3DSTouch{
	public:
		D3DSTouch();
		void readTouch();
		touchPosition touch_old;
		touchPosition touch;
		vector2 getClickPoint();
		vector2 lastClickPoint;
		bool clicking;
		bool scrolling;
		int scrollDirX ;
		int scrollDirY;

	private:
		void initTouch();
		bool touched = false;
		bool touched_old = false;
		vector2 emptyClickPoint;
		
};

#endif