#include "DiscordApp.hpp"
#include "log.hpp"
#include "easyencryptor.hpp"
#include <fstream>
#include <3DS.h>

#define SLEEP_CLICK_NORMAL 150000
#define SLEEP_CLICK_EXTENDED 350000

void DiscordApp::loadUserDataFromFile(){
	std::string email, password, token;
	std::ifstream fh;
	fh.open("sdmc:/3ds/3DiScord/3DiScord-userdata.txt", std::ios::in);
	if (fh.is_open()) {
		std::getline(fh, email);
		std::getline(fh, password);
		std::getline(fh, token);
		fh.close();
	}
	else {
		logSD("Error while opening settings file: settings not loaded!");
		fh.close();
		return;
	}

	email = decrypt(email);
	password = decrypt(password);
	token = decrypt(token);

	logSD("set mail");
	discord.setEmail(email);
	logSD("set pass");
	discord.setPassword(password);
	logSD("set token");
	discord.setToken(token);

	D3dsGUI.loginTexts[0] = discord.getEmail();
	D3dsGUI.loginTexts[1] = discord.getPassword();

	

}

void DiscordApp::saveUserDataToFile(std::string mail , std::string pass , std::string _tok){


	mail = encrypt(mail);
	pass = encrypt(pass);
	_tok = encrypt(_tok);

	std::string userdata = mail + "\n" + pass + "\n" + _tok + "\n";
	std::ofstream fh("sdmc:/3ds/3DiScord/3DiScord-userdata.txt");
	fh << userdata;
	fh.close();
}


void DiscordApp::Start(){


	logSD("____App started!_____");

	logSD("load userdata file");
	loadUserDataFromFile();
	logSD("pass discord pointer to D3DSGUI");
	D3dsGUI.passDiscordPointer( &discord );
	logSD("start program loop");
	while(aptMainLoop()){
		try {
			D3dsGUI.Draw();
		}
		catch(std::exception){
			sf2d_end_frame();
			sf2d_swapbuffers();
		}
		D3dsPad.Read();
		D3dsTouch.readTouch();
		int scrolldirx = D3dsTouch.scrollDirX, scrolldiry = D3dsTouch.scrollDirY;
		if (D3dsPad.left)
			scrolldirx = -10;
		if (D3dsPad.right)
			scrolldirx = 10;
		if (D3dsPad.down)
			scrolldiry = -10;
		if (D3dsPad.up)
			scrolldiry = 10;
		if (D3dsPad.up || D3dsPad.down)
			D3dsTouch.scrolling = true;
		if(D3dsTouch.clicking){
			clicked = D3dsGUI.click(D3dsTouch.lastClickPoint.x , D3dsTouch.lastClickPoint.y);
		}else{
			clicked = -1;
		}
		if(D3dsTouch.scrolling){
			scrolled = D3dsGUI.scroll(scrolldirx , scrolldiry);
		}else{
			scrolled = -1;
		}
		D3DSState = D3dsGUI.GetState();
		std::string mel;
		std::string pwd;
		if(D3DSState == 0){
			switch(clicked){
				case 0:
					mel = D3dsIME.getUserText("Discord Email");
					discord.setEmail(mel);
					D3dsGUI.loginTexts[clicked] = discord.getEmail();
					discord.setToken("");
					svcSleepThread(SLEEP_CLICK_NORMAL);
					break;
				case 1:
					pwd = D3dsIME.getUserText("Discord Password");
					discord.setPassword(pwd);
					D3dsGUI.loginTexts[clicked] = discord.getPassword();
					discord.setToken("");
					svcSleepThread(SLEEP_CLICK_NORMAL);
					break;

				case 2:
					int loginR = discord.login();
					logSD("loginResult: " + std::to_string(loginR));
					if(loginR  == 200){
						logSD("Login Success");
						logSD("Saving data to file...");
						saveUserDataToFile(discord.getEmail() , discord.getPassword() , discord.getToken());
						discord.loadData();
						logSD("Loaded data");
						D3dsGUI.SetState(1);
						logSD("Set state");
					}else if(loginR == 200000){
						if( discord.submit2facode(D3dsIME.getUserText("Enter your 2Factor Auth Code!")) == 200){
							logSD("Login (2FA) Success");
							saveUserDataToFile(discord.getEmail() , discord.getPassword() , discord.getToken());
							discord.loadData();
							logSD("Loaded data");
							D3dsGUI.SetState(1);
							logSD("Set state");
						}
					}

					svcSleepThread(SLEEP_CLICK_EXTENDED);
					break;

			}
		}else if(D3DSState == 1){
			if(discord.loadingData){

			}else{
				D3dsGUI.SetState(2);
				D3dsGUI.guildAnimIn = true;
				svcSleepThread(SLEEP_CLICK_NORMAL);
			}
		}else if(D3DSState == 2){

			switch(clicked){
				case -1:
					break;
				default:
					discord.JoinGuild(clicked);
					D3dsGUI.guildAnimOut = true;
					D3dsGUI.channelAnimIn = true;
					svcSleepThread(SLEEP_CLICK_NORMAL);
					break;

			}
		}else if(D3DSState == 3){
			if(D3dsPad.cross && !D3dsGUI.channelAnimIn){
				discord.currentChannel = 0;
				D3dsGUI.channelAnimOut = true;
				D3dsGUI.guildAnimIn = true;
				svcSleepThread(SLEEP_CLICK_NORMAL);
			}
			switch(clicked){
				case -1:
					break;
				default:
					//if(discord.currentGuild != 0)
						discord.JoinChannel(clicked);
					D3dsGUI.channelAnimOut = true;
					svcSleepThread(SLEEP_CLICK_NORMAL);
					break;

			}

		}else if(D3DSState == 4){
			if(D3dsPad.circle || clicked == 2){
				std::string userMessage = D3dsIME.getUserText("Message");
				discord.sendMessage(userMessage);
				svcSleepThread(SLEEP_CLICK_NORMAL);
			}else if(D3dsPad.cross || clicked == 1){
				D3dsGUI.channelAnimIn = true;
				discord.LeaveChannel();
				D3dsGUI.SetState(3);
				D3dsGUI.clearAvatarVec();
				svcSleepThread(SLEEP_CLICK_NORMAL);
			}else{

			}

			switch(clicked){
				case -1:
					break;
				case 3:
					discord.JoinChannel(clicked);
					D3dsGUI.SetState(4);
					svcSleepThread(SLEEP_CLICK_NORMAL);
					break;
			}

		}

	}
	discord.loadingData = false;
	discord.LeaveChannel();
	threadJoin(discord.threadLoadData, U64_MAX);
	threadFree(discord.threadLoadData);
	threadJoin(discord.threadRefreshMessages, U64_MAX);
	threadFree(discord.threadRefreshMessages);

}


