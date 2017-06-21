#include "3DSGUI.hpp"
#include "log.hpp"
#include <algorithm>
#include <stdlib.h>

#include "DiScordAvatarCircle_png.h"
#include "DiScordBackground8BIT_png.h"
#include "DiScordDefaultAvatar_png.h"
#include "DiScordLoading8BIT_png.h"
#include "DiScordLoginForm8BIT_png.h"
#include "DiScordMenu_png.h"
#include "DiScordlogo_png.h"
#include "font_ttf.h"

#define PI 3.14159265358979323846



void D3DSGUI::drawBottomScreen()
{
	try {
		sf2d_draw_rectangle(0, 0, 320, 240, RGBA8(0x36, 0x39, 0x3E, 0xFF));
		sf2d_draw_texture(menuImage, 0, 0);
		sf2d_draw_rectangle(0, 200, 320, 40, RGBA8(0x41, 0x44, 0x49, 0xFF));
		if (discordPtr->currentGuild != 0) {
			sftd_draw_text(fontGuilds, 74, 15, RGBA8(255, 255, 255, 255), 22, std::string("#" + discordPtr->guilds[discordPtr->currentGuild].channels[discordPtr->currentChannel].name).c_str());
			text = converter.from_bytes(wrap(discordPtr->guilds[discordPtr->currentGuild].channels[discordPtr->currentChannel].topic.c_str(), fontText, 13, 246));
			sftd_draw_wtext(fontText, 74, 50, RGBA8(255, 255, 255, 230), 13, text.c_str());
			sftd_draw_text(pgf, 20, 210, RGBA8(0x8B, 0x90, 0x96, 0xFF), 15, std::string("Message #" + discordPtr->guilds[discordPtr->currentGuild].channels[discordPtr->currentChannel].name).c_str());
		}
		else {
			sftd_draw_text(fontGuilds, 74, 15, RGBA8(255, 255, 255, 255), 22, discordPtr->directMessages[discordPtr->currentChannel].recipients[0].username.c_str());
			sftd_draw_text(pgf, 20, 210, RGBA8(0x8B, 0x90, 0x96, 0xFF), 15, std::string("Message @" + discordPtr->directMessages[discordPtr->currentChannel].recipients[0].username).c_str());
		}
	}
	catch (std::exception) {
	}
}


D3DSGUI::D3DSGUI(){
	sf2d_init();
	sf2d_set_clear_color(RGBA8(0x40, 0x40, 0x40, 0xFF));
	backgroundImage = sfil_load_PNG_buffer(DiScordBackground8BIT_png,SF2D_PLACE_RAM);
	loginFormImage = sfil_load_PNG_buffer(DiScordLoginForm8BIT_png, SF2D_PLACE_RAM);
	loadingImage = sfil_load_PNG_buffer(DiScordLoading8BIT_png, SF2D_PLACE_RAM);
	logoImage = sfil_load_PNG_buffer(DiScordlogo_png, SF2D_PLACE_RAM);
	menuImage = sfil_load_PNG_buffer(DiScordMenu_png, SF2D_PLACE_RAM);
	defaultAvatarImage = sfil_load_PNG_buffer(DiScordDefaultAvatar_png, SF2D_PLACE_RAM);
	avatarCircleImage = sfil_load_PNG_buffer(DiScordAvatarCircle_png, SF2D_PLACE_RAM);
	pgf = sftd_load_font_mem(font_ttf, font_ttf_size);
	sftd_draw_text(pgf, 0, 0, 0, 25, "a");												//The reason we have multiple fonts is because the text is blurried otherwise
	fontGuilds = sftd_load_font_mem(font_ttf, font_ttf_size);
	sftd_draw_text(fontGuilds, 0, 0, 0, 22, "a");
	fontUser = sftd_load_font_mem(font_ttf, font_ttf_size);
	sftd_draw_text(fontUser, 0, 0, 0, 14, "a");
	fontText = sftd_load_font_mem(font_ttf, font_ttf_size);
	sftd_draw_text(fontText, 0, 0, 0, 13, "a");

	inputbox emailI;
	emailI.x = 125;
	emailI.y = 70;
	emailI.w = 190;
	emailI.h = 25;
	loginInputs.push_back(emailI);
	
	inputbox passwordI;
	passwordI.x = 125;
	passwordI.y = 115;
	passwordI.w = 190;
	passwordI.h = 25;
	loginInputs.push_back(passwordI);
	
	inputbox loginI;
	loginI.x = 125;
	loginI.y = 175;
	loginI.w = 190;
	loginI.h = 34;
	loginInputs.push_back(loginI);


	
	loginTexts.push_back(" ");
	loginTexts.push_back(" ");

	
}
D3DSGUI::~D3DSGUI(){
	clearAvatarVec();
	sf2d_free_texture(backgroundImage);
	sf2d_free_texture(loginFormImage);
	sf2d_free_texture(loadingImage);
	sf2d_free_texture(logoImage);
	sf2d_free_texture(menuImage);
	sf2d_free_texture(avatarCircleImage);
	sf2d_free_texture(defaultAvatarImage);
	sf2d_free_texture(avatarImage);
	sftd_free_font(pgf);
	sftd_free_font(fontGuilds);
	sftd_free_font(fontUser);
	sftd_free_font(fontText);
	sftd_fini();
	sf2d_fini();
}
void D3DSGUI::updateBoxes(){
	
	
}

void D3DSGUI::clearAvatarVec()
{
	for (int i = 0; i < avatarVec.size(); i++)
		sf2d_free_texture(avatarVec[i].avatarImage);
	avatarVec.clear();
}



int D3DSGUI::getUserInfo(std::string userId, std::string avatarId)
{
	int index = -1;
	for (unsigned int i = 0; i < avatarVec.size(); i++) {
		if (avatarVec[i].userId == userId) {
			index = i;
			break;
		}
	}
	if (index != -1) {
		return index;
	}
	else {
		loadUserInfo(userId, avatarId);
		return avatarVec.size() - 1;
	}
}


void D3DSGUI::loadUserInfo(std::string uID, std::string avatarID)
{
	Avatar tmpavatar;
	avatarVec.push_back(tmpavatar);
	std::string avatarstring = discordPtr->getUserAvatar(uID, avatarID);
	avatarVec[avatarVec.size() - 1].avatarImage = sfil_load_PNG_buffer(avatarstring.c_str(), SF2D_PLACE_RAM);
	if(discordPtr->currentGuild != 0)
		avatarVec[avatarVec.size() - 1].nick = discordPtr->getUserNick(uID);
	//long colour = discordPtr->getUserHighestRole(uID);
	long colour = 0xFFFFFF;
	u8 r = colour / 256 / 256 % 256;
	u8 g = colour / 256 % 256;
	u8 b = colour % 256;
	logSD("Colour: " + std::to_string(r) + " " + std::to_string(g) + " " + std::to_string(b));
	avatarVec[avatarVec.size() - 1].colour = RGBA8(r, g, b, 255);
	avatarVec[avatarVec.size() - 1].userId = uID;
}

std::string D3DSGUI::wrap(const char * text, sftd_font *font, int font_size, size_t line_length)
{
	std::istringstream words(text);
	std::ostringstream wrapped;
	std::string word;

	if (words >> word) {
		wrapped << word;
		size_t space_left = line_length - sftd_get_text_width(font, font_size, word.c_str());
		while (words >> word) {
			if (space_left < sftd_get_text_width(font, font_size, word.c_str()) + font_size) {
				wrapped << '\n' << word;
				space_left = line_length - sftd_get_text_width(font, font_size, word.c_str());
			}
			else {
				wrapped << ' ' << word;
				space_left -= sftd_get_text_width(font, font_size, word.c_str()) + font_size;
			}
		}
	}
	return wrapped.str();
}

void D3DSGUI::Draw(){
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
	if(state == 0){
		sf2d_draw_texture(logoImage, 0, 0);
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture( backgroundImage , 0 , 0);
		sf2d_draw_texture( loginFormImage , 0 , 0 );
		sftd_draw_text(pgf, 125, 76, RGBA8(255,255,255,255), 13, loginTexts[0].c_str());;
		sftd_draw_text(pgf, 125, 118, RGBA8(255,255,255,255), 22, loginTexts[1].length() > 1 ? pwdDisplayText.substr(0,loginTexts[1].length()).c_str() : " ");
	}else if(state == 1){
		sf2d_draw_texture( backgroundImage , 0 , 0);
		sf2d_draw_texture_rotate(loadingImage, 200 , 120, 2 * PI * (loadingImageAngle / 360));
		if (loadingImageAngle >= 360)
			loadingImageAngle -= 360;
		if (loadingImageAngle > 340 || loadingImageAngle < 20)
			loadingImageAngle ++;
		else if ((loadingImageAngle > 320 && loadingImageAngle <= 340) || (loadingImageAngle < 40 && loadingImageAngle >=20))
			loadingImageAngle += 5;
		else
			loadingImageAngle += 15;
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		sf2d_draw_texture(backgroundImage, 0, 0);
		sftd_draw_text(fontGuilds, 65, 105, RGBA8(255, 255, 255, 255), 22, "Loading your stuff");

	}else if(state == 2){
		sf2d_draw_texture(logoImage, 0, 0);
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		drawBottomScreen();
		setGuildBoxes();
		if (guildAnimIn) {
			guildXIndex += 10;
			if (guildXIndex >= 0)
				guildAnimIn = false;
		}
		if (guildAnimOut) {
			guildXIndex -= 10;
			if (guildXIndex <= -400) {
				guildAnimOut = false;
				state = 3;
			}
		}
		sf2d_draw_rectangle(guildXIndex, 0, 320, 240, RGBA8(0x4E, 0x56, 0x5B, 0xFF));
		for (int i = 0; i < guildBoxes.size(); i++) {
			if (guildBoxes[i].x + guildBoxes[i].w > 0 && guildBoxes[i].x < 240) {
				text = converter.from_bytes(discordPtr->guilds[i].name.c_str());
				sftd_draw_wtext(fontGuilds, guildScrollX + guildXIndex + 10, guildScrollY + i * 50 + 14, RGBA8(255, 255, 255, 255), 22, text.c_str());
			}
		}
		sf2d_draw_rectangle(guildScrollX + guildXIndex, 0, 320, 49, RGBA8(0x2E, 0x32, 0x35, 0xFF));
		sf2d_draw_rectangle(guildScrollX + guildXIndex, 49, 320, 1, RGBA8(255, 255, 255, 255));
		sftd_draw_text(fontGuilds, guildScrollX + guildXIndex + 10, 13, RGBA8(255, 255, 255, 255), 22, "Servers");
		
		
	}else if(state == 3){
		sf2d_draw_texture(logoImage, 0, 0);
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		drawBottomScreen();
		setChannelBoxes();
		if (channelAnimIn) {
			channelXIndex += 10;
			if (channelXIndex >= 0)
				channelAnimIn = false;
		}
		if (channelAnimOut) {
			channelXIndex -= 10;
			if (channelXIndex <= -400) {
				channelAnimOut = false;
				if (guildAnimIn)
					state = 2;
				else {
					setMessageBoxes();
					messageScrollY = messageScrollYMin;
					state = 4;
				}
			}
		}
		sf2d_draw_rectangle(channelXIndex, 0, 320, 240, RGBA8(0x4E, 0x56, 0x5B, 0xFF));
		for(int i = 0 ; i < channelBoxes.size() ; i++){
			if (channelBoxes[i].x + channelBoxes[i].w > 0 && channelBoxes[i].x < 240) {
				text = discordPtr->currentGuild == 0 ? converter.from_bytes(discordPtr->directMessages[i].recipients[0].username) : converter.from_bytes("#" + discordPtr->guilds[discordPtr->currentGuild].channels[i].name);
				sftd_draw_wtext(fontGuilds, channelScrollX + channelXIndex + 10, channelScrollY + i * 50 + 14, RGBA8(255, 255, 255, 255), 22, text.c_str());
			}
		}
		sf2d_draw_rectangle(channelScrollX + channelXIndex, 0, 320, 49, RGBA8(0x2E, 0x32, 0x35, 0xFF));
		sf2d_draw_rectangle(channelScrollX + channelXIndex, 49, 320, 1, RGBA8(255, 255, 255, 255));
		sftd_draw_text(fontGuilds, channelScrollX + channelXIndex + 10, 13, RGBA8(255, 255, 255, 255), 22, discordPtr->guilds[discordPtr->currentGuild].name.c_str());
		
	}else if(state == 4){
		setMessageBoxes();
		sf2d_draw_rectangle(0, 0, 400, 240, RGBA8(0x2C, 0x2F, 0x33, 0xFF));
		for (int i = 0; i < messageBoxes.size(); i++) {
			if (messageBoxes[messageBoxes.size() - i - 1].y + messageBoxes[messageBoxes.size() - i - 1].h > 0 && messageBoxes[messageBoxes.size() - i - 1].y < 240) {
				if (discordPtr->currentGuild != 0) {
					uInfo = getUserInfo(discordPtr->guilds[discordPtr->currentGuild].channels[discordPtr->currentChannel].messages[messageBoxes.size() - 1 - i].author.id, discordPtr->guilds[discordPtr->currentGuild].channels[discordPtr->currentChannel].messages[messageBoxes.size() - 1 - i].author.avatar);
					sf2d_draw_texture_scale(avatarVec[uInfo].avatarImage == nullptr ? defaultAvatarImage : avatarVec[uInfo].avatarImage, messageScrollX + 8, messageBoxes[messageBoxes.size() - i - 1].y + 8, avatarVec[uInfo].avatarImage == nullptr ? 1 : 0.75, avatarVec[uInfo].avatarImage == nullptr ? 1 : 0.75);
					sf2d_draw_texture(avatarCircleImage, messageScrollX + 8, messageBoxes[messageBoxes.size() - i - 1].y + 8);
					std::string dispname = avatarVec[uInfo].nick == "" ? discordPtr->guilds[discordPtr->currentGuild].channels[discordPtr->currentChannel].messages[messageBoxes.size() - 1 - i].author.username : avatarVec[uInfo].nick;
					text = converter.from_bytes(dispname);
					sftd_draw_wtext(fontUser, messageScrollX + 70, messageBoxes[messageBoxes.size() - i - 1].y + 9, avatarVec[uInfo].colour, 14, text.c_str());
					std::string timestamp = discordPtr->guilds[discordPtr->currentGuild].channels[discordPtr->currentChannel].messages[messageBoxes.size() - 1 - i].timestamp;
					sftd_draw_text(fontText, messageScrollX + 75 + sftd_get_wtext_width(fontUser, 14, text.c_str()), messageBoxes[messageBoxes.size() - i - 1].y + 12, RGBA8(125, 125, 125, 255), 11, std::string(timestamp.substr(8, 2) + "/" + timestamp.substr(5, 2) + " at " + timestamp.substr(11, 5)).c_str());
					text = converter.from_bytes(wrap(discordPtr->guilds[discordPtr->currentGuild].channels[discordPtr->currentChannel].messages[messageBoxes.size() - 1 - i].content.c_str(), fontText, 13, 330));
					sftd_draw_wtext(fontText, messageScrollX + 70, messageBoxes[messageBoxes.size() - i - 1].y + 32, RGBA8(255, 255, 255, 255), 13, text.c_str());
				}
				else {
					uInfo = getUserInfo(discordPtr->directMessages[discordPtr->currentChannel].messages[messageBoxes.size() - i - 1].author.id, discordPtr->directMessages[discordPtr->currentChannel].messages[messageBoxes.size() - i - 1].author.avatar);
					sf2d_draw_texture_scale(avatarVec[uInfo].avatarImage == nullptr ? defaultAvatarImage : avatarVec[uInfo].avatarImage, messageScrollX + 8, messageBoxes[messageBoxes.size() - i - 1].y + 8, avatarVec[uInfo].avatarImage == nullptr ? 1 : 0.75, avatarVec[uInfo].avatarImage == nullptr ? 1 : 0.75);
					sf2d_draw_texture(avatarCircleImage, messageScrollX + 8, messageBoxes[messageBoxes.size() - i - 1].y + 8);
					std::string dispname = discordPtr->directMessages[discordPtr->currentChannel].messages[messageBoxes.size() - i - 1].author.username;
					text = converter.from_bytes(dispname);
					sftd_draw_wtext(fontUser, messageScrollX + 70, messageBoxes[messageBoxes.size() - i - 1].y + 9, avatarVec[uInfo].colour, 14, text.c_str());
					std::string timestamp = discordPtr->directMessages[discordPtr->currentChannel].messages[messageBoxes.size() - i - 1].timestamp;
					sftd_draw_text(fontText, messageScrollX + 75 + sftd_get_wtext_width(fontUser, 14, text.c_str()), messageBoxes[messageBoxes.size() - i - 1].y + 12, RGBA8(125, 125, 125, 255), 11, std::string(timestamp.substr(8, 2) + "/" + timestamp.substr(5, 2) + " at " + timestamp.substr(11, 5)).c_str());
					text = converter.from_bytes(wrap(discordPtr->directMessages[discordPtr->currentChannel].messages[messageBoxes.size() - i - 1].content.c_str(), fontText, 13, 330));
					sftd_draw_wtext(fontText, messageScrollX + 70, messageBoxes[messageBoxes.size() - i - 1].y + 32, RGBA8(255, 255, 255, 255), 13, text.c_str());
				}
			}
		}
		sf2d_end_frame();
		sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
		drawBottomScreen();
	}else if(state == 9){
		int i = 0;
		for( i = 0; i < rectangles.size() ; i++){
			sf2d_draw_rectangle(rectangles[i].x, rectangles[i].y, rectangles[i].w, rectangles[i].h, rectangles[i].color);
		}
	}

	sf2d_end_frame();
	sf2d_swapbuffers();
}

int D3DSGUI::scroll(int x , int y){
	if(state == 2){
		guildScrollX = 0;
		guildScrollY += y;
		if(guildScrollY < guildScrollYMin)
			guildScrollY = guildScrollYMin;
		else if(guildScrollY > guildScrollYMax )
			guildScrollY = guildScrollYMax;
		return 0;
	}else if(state == 3){
		channelScrollX = 0;
		channelScrollY += y;
		if(channelScrollY < channelScrollYMin)
			channelScrollY = channelScrollYMin;
		else if(channelScrollY > channelScrollYMax )
			channelScrollY = channelScrollYMax;
		return 0;
	}else if(state==4){
		messageScrollX = 0;
		messageScrollY += y;
		if(messageScrollY < messageScrollYMin)
			messageScrollY = messageScrollYMin;
		else if(messageScrollY > messageScrollYMax )
			messageScrollY = messageScrollYMax;
		return 0;
	}
	return -1;
}

int D3DSGUI::click(int x , int y){
	if(state == 0){
		for(int i = 0 ; i < loginInputs.size() ; i++){
			if( x > loginInputs[i].x && x < loginInputs[i].x + loginInputs[i].w){
				if( y > loginInputs[i].y && y < loginInputs[i].y + loginInputs[i].h){
					return i;
				}
			}
		}
	}else if(state == 2){
		for(int i = 0 ; i < guildBoxes.size() ; i++){
			if( x  > guildBoxes[i].x && x  < guildBoxes[i].x + guildBoxes[i].w){
				if( y > guildBoxes[i].y && y  < guildBoxes[i].y + guildBoxes[i].h){
					return i;
				}
			}
		}
	}else if(state == 3){
		for(int i = 0 ; i < channelBoxes.size() ; i++){
			if( x  > channelBoxes[i].x && x  < channelBoxes[i].x + channelBoxes[i].w){
				if( y  > channelBoxes[i].y && y  < channelBoxes[i].y + channelBoxes[i].h){
					return i;
				}
			}
		}
	}
	else if (state == 4) {
		if (x > 0 && x < 64) {
			if (y > 0 && y < 64) {
				return 1;
			}
		}
		if (x > 0 && x < 320) {
			if (y > 200 && y < 240) {
				return 2;
			}
		}

	}
	return -1;
}

void D3DSGUI::AddRectangle(float nx , float ny , float nw , float nh , unsigned int ncolor){
	rectangle r;
	r.x = nx;
	r.y = ny;
	r.w = nw;
	r.h = nh;
	r.color = ncolor;
	rectangles.push_back(r);
}
void D3DSGUI::RemoveRectangle(int index){
	
	if(index < rectangles.size()){
		rectangles.erase(rectangles.begin()+index);
	}
	
}

int D3DSGUI::GetState(){
	return state;
}
void D3DSGUI::SetState(int s){
	state = s;
}

void D3DSGUI::passDiscordPointer(Discord *ptr){
	discordPtr = ptr;
}
void D3DSGUI::setGuildBoxes(){
	guildBoxes.clear();
	for(int i = 0; i < discordPtr->guilds.size() ; i++){
		box boxG;
		boxG.x = guildScrollX + 50;
		boxG.y = guildScrollY + i * 50;
		boxG.w = 320;
		boxG.h = 50;
		guildBoxes.push_back(boxG);
	}
	guildScrollYMax = 50;
	guildScrollYMin = -((discordPtr->guilds.size()-1)*50 - 190);
}
void D3DSGUI::setChannelBoxes(){
	channelBoxes.clear();
	int size;

	if (discordPtr->currentGuild == 0)
		size = discordPtr->directMessages.size();
	else
		size = discordPtr->guilds[discordPtr->currentGuild].channels.size();

	for (int i = 0; i < size; i++) {
		box boxC;
		boxC.x = channelScrollX + 50;
		boxC.y = channelScrollY + i * 50;
		boxC.w = 320;
		boxC.h = 50;
		channelBoxes.push_back(boxC);
	}
	channelScrollYMax = 50;
	if (discordPtr->currentGuild != 0)
		channelScrollYMin = -((discordPtr->guilds[discordPtr->currentGuild].channels.size() - 1) * 50 - 190);
	else
		channelScrollYMin = -((discordPtr->directMessages.size() - 1) * 50 - 190);
}
void D3DSGUI::setMessageBoxes(){
	try {
		messageBoxes.clear();
		int sum = 0;
		int size;
		if (discordPtr->currentGuild == 0)
			size = discordPtr->directMessages[discordPtr->currentChannel].messages.size();
		else
			size = discordPtr->guilds[discordPtr->currentGuild].channels[discordPtr->currentChannel].messages.size();
		for (int i = 0; i < size; i++) {
			box boxC;
			std::string s = discordPtr->currentGuild != 0 ? wrap(discordPtr->guilds[discordPtr->currentGuild].channels[discordPtr->currentChannel].messages[discordPtr->guilds[discordPtr->currentGuild].channels[discordPtr->currentChannel].messages.size() - 1 - i].content.c_str(), fontText, 13, 330) : wrap(discordPtr->directMessages[discordPtr->currentChannel].messages[discordPtr->directMessages[discordPtr->currentChannel].messages.size() - 1 - i].content.c_str(), fontText, 13, 330);
			int h = std::count(s.begin(), s.end(), '\n') * 13 + 13;
			boxC.x = messageScrollX;
			boxC.y = messageScrollY + sum;
			boxC.w = 400;
			boxC.h = 32 + h <= 64 ? 64 : 32 + h + 15;
			sum += 32 + h <= 64 ? 64 : 32 + h + 15;
			messageBoxes.push_back(boxC);
		}
		std::reverse(messageBoxes.begin(), messageBoxes.end());
		messageScrollYMin = -sum + 240;
	}
	catch (std::exception) {
	}
}
