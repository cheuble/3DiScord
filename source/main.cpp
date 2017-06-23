#include "DiscordApp.hpp"
#include <sys/stat.h>


int main(){
	sf2d_init();
	sftd_init();
	romfsInit();
	mkdir("sdmc:/3ds", 0777);
	mkdir("sdmc:/3ds/3DiScord", 0777);
	mkdir("sdmc:/3ds/3DiScord/theme", 0777);
	remove("sdmc:/3ds/3DiScord/log.txt");
	DiscordApp discordapp;
	discordapp.Start();
	romfsExit();
	return 0;
}