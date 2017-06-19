#ifndef DISCORDAPP_HPP
#define DISCORDAPP_HPP



#include "3DSTouch.hpp"
#include "3DSPad.hpp"
#include "3DSIME.hpp"
#include "Discord.hpp"
#include "3DSGUI.hpp"


class DiscordApp{
	
	public:
		void Start();
		void loadUserDataFromFile();
		void saveUserDataToFile(std::string m , std::string p , std::string t);
		
	private:
		D3DSGUI D3dsGUI;
		Discord discord;
		D3DSIME D3dsIME;
		D3DSPad D3dsPad;
		D3DSTouch D3dsTouch;
		int clicked = -1;
		int scrolled = -1;
		int D3DSState = 0;
	
};



#endif




