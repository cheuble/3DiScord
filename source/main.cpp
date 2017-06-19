#include "DiscordApp.hpp"
#include <3DS.h>
#include <sys/stat.h>


int main(){
	sf2d_init();
	sftd_init();
	mkdir("sdmc:/3ds", 0777);
	mkdir("sdmc:/3ds/3DiScord", 0777);
	remove("sdmc:/3ds/3DiScord/log.txt");
	DiscordApp discordapp;
	discordapp.Start();
	return 0;
}