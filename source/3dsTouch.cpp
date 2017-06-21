#include "3DSTouch.hpp"
#include <stdlib.h>


D3DSTouch::D3DSTouch(){
	emptyClickPoint.x = emptyClickPoint.y = 0;
	initTouch();
}
void D3DSTouch::initTouch(){
}

void D3DSTouch::readTouch(){
	touch_old = touch;
	touched_old = touched;
	if (hidKeysHeld() & KEY_TOUCH)
		touched = true;
	else
		touched = false;

	hidTouchRead(&touch);
	if (touched_old && !touched){
		if(!scrolling){
			lastClickPoint.x = touch_old.px;
			lastClickPoint.y = touch_old.py;
			scrollDirX = scrollDirY = 0;
			scrolling = false;
			clicking = true;
		}else{
			lastClickPoint = emptyClickPoint;
			scrollDirX = scrollDirY = 0;
			scrolling = false;
			clicking = false;
		}
	}else if(touched_old && touched){

		scrollDirX = touch.px - touch_old.px;
		scrollDirY = touch.py - touch_old.py;
		if(abs(scrollDirX) > 4 || abs(scrollDirY) > 4){
			lastClickPoint = emptyClickPoint;
			scrolling = true;
			clicking = false;
		}
	}else{
		lastClickPoint = emptyClickPoint;
		scrollDirX = scrollDirY = 0;
		scrolling = false;
		clicking = false;
	}
}

vector2 D3DSTouch::getClickPoint(){
	return lastClickPoint;
}




