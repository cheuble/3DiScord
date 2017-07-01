#pragma once

#include "GUIElements.hpp"
#include "Discord.hpp"
#include <vector>
#include <string>
#include <locale>
#include <codecvt>
#include <sstream>
#include "json.hpp"

typedef struct {
	std::string userId;
	std::string nick;
	unsigned int colour;
	sf2d_texture *avatarImage;
}Avatar;

typedef struct {
	int x;
	int y;
	long code;
}emoji;

class D3DSGUI{
	public:
		D3DSGUI();
		~D3DSGUI();
		void Draw();
		void AddRectangle(float x , float y , float w , float h , unsigned int color);
		void RemoveRectangle(int index);
		int scroll(int x , int y);
		int click(int x , int y);
		std::vector<std::string> loginTexts;
		
		void SetState(int s);
		int GetState();
		
		void passDiscordPointer(Discord *ptr);
		void setGuildBoxes();
		void setChannelBoxes();
		void setMessageBoxes();
		void updateBoxes();
		
		bool channelAnimIn = false;
		bool channelAnimOut = false;

		bool guildAnimIn = false;
		bool guildAnimOut = false;
		void clearAvatarVec();
		bool filePresent(std::string path);
		std::wstring_convert<std::codecvt_utf16<wchar_t>> converter;
	
	private:
		sf2d_texture * loadTextureFromSdmcOrRomfs(std::string path);
		void draw_text(sftd_font *font, int x, int y, float size, unsigned int colour, std::wstring txt);
		float emojix;
		float emojiy;
		nlohmann::json j_theme;
		void loadThemeFromJson();
		unsigned int getJsonInfo(std::string param);
		unsigned int clear_colour;
		unsigned int credentials_colour;
		unsigned int server_channel_bg_colour;
		unsigned int server_channel_title_bg_colour;
		unsigned int server_channel_title_line_colour;
		unsigned int server_channel_title_text_colour;
		unsigned int server_channel_text_colour;
		unsigned int message_bg_colour;
		unsigned int message_text_colour;
		unsigned int message_timestamp_colour;
		unsigned int message_user_default_colour;
		unsigned int messagebox_bg_colour;
		unsigned int messagebox_text_colour;
		unsigned int bottom_screen_name_colour;
		unsigned int bottom_screen_info_colour;
		unsigned int bg_colour;
		int uInfo;
		std::wstring text;
		void drawBottomScreen();
		int getUserInfo(std::string userId, std::string avatarId);

		void loadUserInfo(std::string uID, std::string avatarID);

		std::vector<Avatar> avatarVec;
		std::vector<emoji> emojiVec;

		Discord *discordPtr;
		
		sftd_font *pgf;
		sftd_font *fontGuilds;
		sftd_font *fontUser;
		sftd_font *fontText;
		
		sf2d_texture *clearImage;
		sf2d_texture *logoImage;
		sf2d_texture *messagesBackgroundImage;
		sf2d_texture *backgroundImage;
		sf2d_texture *loginFormImage;
		sf2d_texture *loadingImage;
		sf2d_texture *menuImage;
		sf2d_texture *defaultAvatarImage;
		sf2d_texture *avatarImage;
		sf2d_texture *avatarCircleImage;
		sf2d_texture *bottommessageImage;
		sf2d_texture *channelbgImage;
		sf2d_texture *titlebgImage;
		sf2d_texture *messageboxImage;
		sf2d_texture *menulineImage;
		sf2d_texture *emojis[4];

		std::vector<rectangle> rectangles;
		std::string loadingScreenstring;
		//std::vector<std::string> guildNames;
		//std::vector<std::string> channelNames;
		//std::vector<std::string> channelTopics;
		//std::vector<std::string> userNames;
		
		std::vector<inputbox> loginInputs;
		std::vector<box> guildBoxes;
		std::vector<box> channelBoxes;
		std::vector<box> messageBoxes;

		std::string pwdDisplayText = "****************";
		
		int guildScrollX = 0;
		int guildScrollY = 50;
		int guildScrollYMin = -1280;
		int guildScrollYMax = 0;
		
		int channelScrollX = 0;
		int channelScrollY = 50;
		int channelScrollYMin = -1280;
		int channelScrollYMax = 0;

		int channelXIndex = -400;
		int guildXIndex = -400;
		

		int messageScrollX = 0;
		int messageScrollY = 0;
		int messageScrollYMin = -1280;
		int messageScrollYMax = 0;
		
		float loadingImageAngle = 0;
		
		int state = 0;  // 0 login , 1 loading , 2 guilds , 3 channels , 4 channellobby , 5 friends , 6 dms , 7 settings , 8 stuff..
		
		std::string long_to_hex(long i)
		{
			std::stringstream ss;
			ss << std::hex << i;
			std::string res(ss.str());
			std::string ret(6, '0');
			for (unsigned int j = 0; j < res.length(); j++)
				ret[5 - j] = res[res.length() - 1 - j];
			return ret;
		}
	
		std::string wrap(const char *text, sftd_font *font, int font_size, size_t line_length);
};



