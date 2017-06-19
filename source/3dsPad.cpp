#include "3DSPad.hpp"

D3DSPad::D3DSPad(){
}

D3DSPad::D3DSPad(bool initstuff){
	left_analog_calibration_x = circlepos.dx;
	left_analog_calibration_y = circlepos.dy;
}


D3DSPad::~D3DSPad(){
	
}

void D3DSPad::Read(){
	hidScanInput();
	select = start = up = right = down = left = lefttrigger = righttrigger = triangle = circle = cross = square = volup = voldown = false;
	left_analog_moving = right_analog_moving = false;
	left_analog_x =   0;
	left_analog_y =   0;
	right_analog_x =  0;
	right_analog_y =  0;
	
	if(hidKeysHeld() & KEY_SELECT)
		select = true;
	if(hidKeysHeld() & KEY_START)
		start = true;
	if(hidKeysHeld() & KEY_UP)
		up = true;
	if(hidKeysHeld() & KEY_RIGHT)
		right = true;
	if(hidKeysHeld() & KEY_DOWN)
		down = true;
	if(hidKeysHeld() & KEY_LEFT)
		left = true;
	if(hidKeysHeld() & KEY_ZL)
		lefttrigger = true;
	if(hidKeysHeld() & KEY_ZR)
		righttrigger = true;
	if(hidKeysHeld() & KEY_L)
		lefttrigger = true;
	if(hidKeysHeld() & KEY_R)
		righttrigger = true;
	if(hidKeysHeld() & KEY_X)
		triangle = true;
	if(hidKeysHeld() & KEY_A)
		circle = true;
	if(hidKeysHeld() & KEY_B)
		cross = true;
	if(hidKeysHeld() & KEY_Y)
		square = true;	
	
	lx = circlepos.dx;
	ly = circlepos.dy;
	//rx = D3DSpad.rx;
	//ry = D3DSpad.ry;
	 
	 
	left_analog_x = lx - left_analog_calibration_x;
	left_analog_y = ly - left_analog_calibration_y;
	//right_analog_x = D3DSpad.rx - right_analog_calibration_x;
	//right_analog_y = D3DSpad.ry - right_analog_calibration_y;
	
}



