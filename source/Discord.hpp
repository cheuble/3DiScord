#ifndef DISCORD_HPP
#define DISCORD_HPP

#include <string>
#include <vector>
#include <atomic>
#include <fstream>


#include "3DSNet.hpp"


class Discord{
	public:
		Thread threadLoadData;
		Thread threadRefreshMessages;
		friend void thrd_loaddata(void * arg);
		friend void thrd_rfrshmsg(void * arg);

		typedef struct {
			std::string username;
			std::string discriminator;
			std::string id;
			std::string avatar;
		}user;

		typedef struct {
			user author;
			std::string content;
			std::string embed;
			std::string attachment;
			std::string mentions;
			std::string timestamp;
			std::string id;
		}message;

		typedef struct {
			std::string last_message_id;
			long type;
			std::string id;
			std::vector<user> recipients;
			std::vector<message> messages;
		}directMessage;

		typedef struct {
			std::string name;
			std::string last_pin_timestamp;
			std::string topic;
			std::string last_message_id;
			std::string type;
			std::string id;
			bool is_private;

			std::vector<message> messages;
		}channel;

		typedef struct {
			bool hoist;
			std::string	name;
			bool mentionable;
			long color;
			int position;
			std::string id;
			bool managed;
			long permissions;
		}role;

		typedef struct {
			bool owner;
			long permissions;
			std::string icon;
			std::string id;
			std::string name;
			std::vector<channel> channels;
			std::vector<role> roles;
		}guild;


		Discord();
		~Discord();
		void setEmail(std::string mail);
		void setPassword(std::string pass);
		void setToken(std::string tok);
		long login();
		long login(std::string mail , std::string pass);
		long submit2facode(std::string code);
		std::string getToken();
		std::string getTicket();
		std::string getUsername();
		std::string getEmail();
		std::string getPassword();
		void loadData();
		bool loadingData;
		std::vector<guild> guilds;
		int guildsAmount = 0;
		std::string getUserAvatar(std::string userid, std::string avatarid);
		/*
		static void* loadData_wrapper(void* object)
		{
			reinterpret_cast<Discord*>(object)->thread_loadData(object);
			return 0;
		}
		*/
		void JoinGuild(int gIndex);
		uint64_t osGetTimeMS();
		void JoinChannel(int cIndex);
		void LeaveChannel();
		int currentGuild = 0;
		int currentChannel = 0;
		void sendMessage(std::string msg);
		std::string getUserNick(std::string uID);
		long getUserHighestRole(std::string uID);
		void refreshMessages();
		void thread_loadData(void * arg);
		void thread_refreshMessages(void *arg);
		std::vector<directMessage> directMessages;

	private:
		bool inChannel = false;
		D3DSNet D3dsNet;
		std::string email = "", password = "", code2fa , token , ticket , username , id , avatar , discriminator , phone;
		bool verified, mfa_enabled; // mfa == twofactor its the same
		bool twoFactorAuthEnabled;
		user client;
		bool loggedin;
		bool loadedGuilds = false;
		bool loadedChannels = false;
		long fetchUserData();
		void getGuilds();
		void getChannels();
		void getChannelMessages(int guildIndex);
		void getCurrentDirectMessages();
		void getDirectMessageChannels();

		uint64_t lastFetchTimeMS;
		uint64_t fetchTimeMS = 5000;
		uint64_t currentTimeMS;

};








#endif

