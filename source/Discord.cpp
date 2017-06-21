#include "Discord.hpp"
#include "log.hpp"
#include "json.hpp"
#include <3ds.h>

uint64_t Discord::osGetTimeMS(){
	return (svcGetSystemTick() / 1000);
}

Discord::Discord(){
	loadedGuilds = false;
	loadingData = false;
}

Discord::~Discord(){
	LeaveChannel(); 
}


void Discord::sendMessage(std::string msg){
	if (msg == "")
		return;
	std::string postData = "{ \"content\":\"" + msg + "\" }";
	std::string sendMessageUrl;
	if (currentGuild == 0)
		sendMessageUrl = "https://discordapp.com/api/v6/channels/" + directMessages[currentChannel].id + "/messages";
	else
		sendMessageUrl = "https://discordapp.com/api/channels/" + guilds[currentGuild].channels[currentChannel].id + "/messages" ;
	D3dsNet.curlDiscordPost(sendMessageUrl , postData , token);	
}

std::string Discord::getUserNick(std::string uID)
{
	std::string nickname = "";
	std::string nickurl = "https://discordapp.com/api/guilds/" + guilds[currentGuild].id + "/members/" + uID;
	D3DSNet::http_response nickresponse = D3dsNet.curlDiscordGet(nickurl, token);
	nlohmann::json j_complete = nlohmann::json::parse(nickresponse.body);
	try {
		if (!j_complete.is_null())
			if (!j_complete["nick"].is_null())
				nickname = j_complete["nick"].get<std::string>();
	}
	catch(std::exception){
		nickname = "";
	}
	return nickname;
}

long Discord::getUserHighestRole(std::string uID)
{
	long highestrole = 0xFFFFFF;
	int highestpos = 0;
	std::string rolesurl = "https://discordapp.com/api/guilds/" + guilds[currentGuild].id + "/members/" + uID;
	D3DSNet::http_response rolesresponse = D3dsNet.curlDiscordGet(rolesurl, token);
	nlohmann::json j_complete = nlohmann::json::parse(rolesresponse.body);
	logSD("Roles body: " + rolesresponse.body);
	std::vector<std::string> rolesvec;
	rolesvec.clear();
	try {
		if (!j_complete.is_null()) {
			for (int i = 0; i < j_complete["roles"].size(); i++) {
				std::string r = j_complete["roles"][i].get<std::string>();
				rolesvec.push_back(r);
				logSD("r: " + r);
			}
		}
	}
	catch (std::exception) {
		logSD("Exception!");
	}

	for(int i = 0; i < guilds[currentGuild].roles.size(); i++)
		for(int j = 0; j < rolesvec.size(); j++)
			if(rolesvec[j] == guilds[currentGuild].roles[i].id)
				if (guilds[currentGuild].roles[i].position > highestpos) {
					highestpos = guilds[currentGuild].roles[i].position;
					highestrole = guilds[currentGuild].roles[i].color;
				}
	logSD("Highest role: " + std::to_string(highestrole));
	logSD("Highest pos: " + std::to_string(highestpos));
	if (highestrole == 0)
		highestrole = 0xFFFFFF;
	return highestrole;
}


void Discord::refreshMessages(){
	
	currentTimeMS = osGetTimeMS();
	if(currentTimeMS - lastFetchTimeMS > fetchTimeMS){
		lastFetchTimeMS = osGetTimeMS();
		if (currentGuild != 0)
			getChannelMessages(currentChannel);
		else
			getCurrentDirectMessages();
	}
	
}




void Discord::getChannelMessages(int channelIndex){
	currentChannel = channelIndex;
	std::string channelMessagesUrl = "https://discordapp.com/api/channels/" + guilds[currentGuild].channels[currentChannel].id + "/messages";
	D3DSNet::http_response channelmessagesresponse = D3dsNet.curlDiscordGet(channelMessagesUrl , token);
	if(channelmessagesresponse.httpcode == 200){
		nlohmann::json j_complete = nlohmann::json::parse(channelmessagesresponse.body);
		int messagesAmount = j_complete.size();
		
		
		if(!j_complete.is_null()){
			guilds[currentGuild].channels[currentChannel].messages.clear();
			
			for( int i = 0 ; i < messagesAmount ; i++){
				message tmpmessage;
				//guilds[currentGuild].channels[currentChannel].messages.push_back(message());
				
				if(!j_complete[i].is_null()){
					
					if(!j_complete[i]["timestamp"].is_null()){
						tmpmessage.timestamp = j_complete[i]["timestamp"].get<std::string>();
					}else{
						tmpmessage.timestamp = "";
					}
					
					if(!j_complete[i]["id"].is_null()){
						tmpmessage.id = j_complete[i]["id"].get<std::string>();
					}else{
						tmpmessage.id = "";
					}
					
					if(!j_complete[i]["content"].is_null()){
						tmpmessage.content = j_complete[i]["content"].get<std::string>();
					}else{
						tmpmessage.content = "";
					}
					// author :
					if(!j_complete[i]["author"]["username"].is_null()){
						tmpmessage.author.username = j_complete[i]["author"]["username"].get<std::string>();
					}else{
						tmpmessage.author.username = "";
					}
					
					if(!j_complete[i]["author"]["discriminator"].is_null()){
						tmpmessage.author.discriminator = j_complete[i]["author"]["discriminator"].get<std::string>();
					}else{
						tmpmessage.author.discriminator = "";
					}
					
					if(!j_complete[i]["author"]["id"].is_null()){
						tmpmessage.author.id = j_complete[i]["author"]["id"].get<std::string>();
					}else{
						tmpmessage.author.id = "";
					}
					
					if(!j_complete[i]["author"]["avatar"].is_null()){
						tmpmessage.author.avatar = j_complete[i]["author"]["avatar"].get<std::string>();
					}else{
						tmpmessage.author.avatar = "";
					}

				}
				guilds[currentGuild].channels[currentChannel].messages.push_back(tmpmessage);
			}
		}
	
		lastFetchTimeMS = osGetTimeMS();
		
	}
	
}

void Discord::getCurrentDirectMessages() {
	std::string dmChannelUrl = "https://discordapp.com/api/v6/channels/" + directMessages[currentChannel].id + "/messages";
	D3DSNet::http_response dmChannelResponse = D3dsNet.curlDiscordGet(dmChannelUrl, token);



	if (dmChannelResponse.httpcode == 200) {
		try {
			nlohmann::json j_complete = nlohmann::json::parse(dmChannelResponse.body);
			if (!j_complete.is_null()) {
				directMessages[currentChannel].messages.clear();
				int msgAmount = j_complete.size();
				for (int i = 0; i < msgAmount; i++) {

					directMessages[currentChannel].messages.push_back(message());

					if (!j_complete[i]["timestamp"].is_null()) {
						directMessages[currentChannel].messages[i].timestamp = j_complete[i]["timestamp"].get<std::string>();
					}
					else {
						directMessages[currentChannel].messages[i].timestamp = "";
					}

					if (!j_complete[i]["id"].is_null()) {
						directMessages[currentChannel].messages[i].id = j_complete[i]["id"].get<std::string>();
					}
					else {
						directMessages[currentChannel].messages[i].id = "";
					}

					if (!j_complete[i]["content"].is_null()) {
						directMessages[currentChannel].messages[i].content = j_complete[i]["content"].get<std::string>();
					}
					else {
						directMessages[currentChannel].messages[i].content = "";
					}
					// author :
					if (!j_complete[i]["author"]["username"].is_null()) {
						directMessages[currentChannel].messages[i].author.username = j_complete[i]["author"]["username"].get<std::string>();
					}
					else {
						directMessages[currentChannel].messages[i].author.username = "";
					}

					if (!j_complete[i]["author"]["discriminator"].is_null()) {
						directMessages[currentChannel].messages[i].author.discriminator = j_complete[i]["author"]["discriminator"].get<std::string>();
					}
					else {
						directMessages[currentChannel].messages[i].author.discriminator = "";
					}

					if (!j_complete[i]["author"]["id"].is_null()) {
						directMessages[currentChannel].messages[i].author.id = j_complete[i]["author"]["id"].get<std::string>();
					}
					else {
						directMessages[currentChannel].messages[i].author.id = "";
					}

					if (!j_complete[i]["author"]["avatar"].is_null()) {
						directMessages[currentChannel].messages[i].author.avatar = j_complete[i]["author"]["avatar"].get<std::string>();
					}
					else {
						directMessages[currentChannel].messages[i].author.avatar = "";
					}
				}
			}
		}
		catch (const std::exception& e) {

		}

	}
	lastFetchTimeMS = osGetTimeMS();
}

void Discord::getDirectMessageChannels() {
	std::string directMessagesChannelsUrl = "https://discordapp.com/api/v6/users/@me/channels";
	D3DSNet::http_response dmChannelsResponse = D3dsNet.curlDiscordGet(directMessagesChannelsUrl, token);

	if (dmChannelsResponse.httpcode == 200) {
		try {
			nlohmann::json j_complete = nlohmann::json::parse(dmChannelsResponse.body);
			if (!j_complete.is_null()) {
				directMessages.clear();
				int dmChannels = j_complete.size();
				for (int i = 0; i < dmChannels; i++) {
					directMessages.push_back(directMessage());

					if (!j_complete[i]["last_message_id"].is_null()) {
						directMessages[i].last_message_id = j_complete[i]["last_message_id"].get<std::string>();
					}
					else {
						directMessages[i].last_message_id = "0000000000000000";
					}
					if (!j_complete[i]["type"].is_null()) {
						directMessages[i].type = j_complete[i]["type"].get<long>();
					}
					else {
						directMessages[i].type = 1;
					}
					if (!j_complete[i]["id"].is_null()) {
						directMessages[i].id = j_complete[i]["id"].get<std::string>();
					}
					else {
						directMessages[i].id = "0000000000000000";
					}
					if (!j_complete[i]["recipients"].is_null()) {
						directMessages[i].recipients.clear();
						int recAmount = j_complete[i]["recipients"].size();
						for (int r = 0; r < recAmount; r++) {
							// author :
							directMessages[i].recipients.push_back(user());
							if (!j_complete[i]["recipients"][r]["username"].is_null()) {
								directMessages[i].recipients[r].username = j_complete[i]["recipients"][r]["username"].get<std::string>();
							}
							else {
								directMessages[i].recipients[r].username = "";
							}

							if (!j_complete[i]["recipients"][r]["discriminator"].is_null()) {
								directMessages[i].recipients[r].discriminator = j_complete[i]["recipients"][r]["discriminator"].get<std::string>();
							}
							else {
								directMessages[i].recipients[r].discriminator = "";
							}

							if (!j_complete[i]["recipients"][r]["id"].is_null()) {
								directMessages[i].recipients[r].id = j_complete[i]["recipients"][r]["id"].get<std::string>();
							}
							else {
								directMessages[i].recipients[r].id = "";
							}

							if (!j_complete[i]["recipients"][r]["avatar"].is_null()) {
								directMessages[i].recipients[r].avatar = j_complete[i]["recipients"][r]["avatar"].get<std::string>();
							}
							else {
								directMessages[i].recipients[r].avatar = "";
							}


						}
					}




				}

			}

		}
		catch (const std::exception& e) {
		}

	}
	lastFetchTimeMS = osGetTimeMS();

}


void Discord::JoinGuild(int gIndex){
	currentGuild = gIndex;
}

void thrd_rfrshmsg(void * arg)
{
	Discord *discordPtr = reinterpret_cast<Discord *>(arg);
	discordPtr->thread_refreshMessages(arg);
	threadExit(0);
}



void Discord::JoinChannel(int cIndex){
	currentChannel = cIndex;
	inChannel = true;
	if (currentGuild != 0)
		getChannelMessages(currentChannel);
	else
		getCurrentDirectMessages();
	refreshMessages();
	s32 prio = 0;
	svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
	threadRefreshMessages = threadCreate(thrd_rfrshmsg, this, 0x10000, prio - 2, -2, false);
}


void Discord::LeaveChannel()
{
	inChannel = false;
}


void Discord::setToken(std::string tok){
	token = tok;
}


void Discord::thread_loadData(void *arg){
	
	Discord *discordPtr = reinterpret_cast<Discord *>(arg);
	logSD("start of thread_loadData");
	discordPtr->loadingData = true;
	while (discordPtr->loadingData) {
		if (!discordPtr->loadedGuilds) {
			std::string guildsUrl = "https://discordapp.com/api/users/@me/guilds";
			D3DSNet::http_response guildsresponse = discordPtr->D3dsNet.curlDiscordGet(guildsUrl, token);
			if (guildsresponse.httpcode == 200) {
				try {
					nlohmann::json j_complete = nlohmann::json::parse(guildsresponse.body);
					if (!j_complete.is_null()) {
						discordPtr->guilds.clear();
						discordPtr->guilds.push_back(guild());
						discordPtr->guilds[0].name = "DMs";
						discordPtr->guilds[0].id = "DMs";
						discordPtr->guildsAmount = j_complete.size();
						for (int i = 0; i < guildsAmount; i++) {

							discordPtr->guilds.push_back(guild());

							if (!j_complete[i].is_null()) {


								if (!j_complete[i]["owner"].is_null()) {
									discordPtr->guilds[i+1].owner = j_complete[i]["owner"].get<bool>();
								}
								else {
									discordPtr->guilds[i+1].owner = false;
								}

								if (!j_complete[i]["permissions"].is_null()) {
									discordPtr->guilds[i+1].permissions = j_complete[i]["permissions"].get<long>();
								}
								else {
									discordPtr->guilds[i+1].permissions = 0;
								}

								if (!j_complete[i]["icon"].is_null()) {
									discordPtr->guilds[i+1].icon = j_complete[i]["icon"].get<std::string>();
								}
								else {
									discordPtr->guilds[i+1].icon = "";
								}

								if (!j_complete[i]["id"].is_null()) {
									discordPtr->guilds[i+1].id = j_complete[i]["id"].get<std::string>();
								}
								else {
									discordPtr->guilds[i+1].id = "";
								}

								if (!j_complete[i]["name"].is_null()) {
									discordPtr->guilds[i+1].name = j_complete[i]["name"].get<std::string>();

								}
								else {
									discordPtr->guilds[i+1].name = "";
								}



							}


						}
						discordPtr->loadedGuilds = true;
					}
				}
				catch (const std::exception& e) {
					discordPtr->loadingData = false;
				}

			}
			else {
				discordPtr->loadingData = false;
			}
		}
		else {
			std::string directMessagesChannelsUrl = "https://discordapp.com/api/v6/users/@me/channels";
			D3DSNet::http_response dmChannelsResponse = discordPtr->D3dsNet.curlDiscordGet(directMessagesChannelsUrl, token);
			if (dmChannelsResponse.httpcode == 200) {
				try {
					nlohmann::json j_complete = nlohmann::json::parse(dmChannelsResponse.body);
					if (!j_complete.is_null()) {
						discordPtr->guilds[0].channels.clear();
						int dmChannels = j_complete.size();
						logSD("Amount of DM channels : " + std::to_string(dmChannels));
						for (int i = 0; i < dmChannels; i++) {
							discordPtr->directMessages.push_back(directMessage());

							if (!j_complete[i]["last_message_id"].is_null()) {
								discordPtr->directMessages[i].last_message_id = j_complete[i]["last_message_id"].get<std::string>();
							}
							else {
								discordPtr->directMessages[i].last_message_id = "0000000000000000";
							}
							if (!j_complete[i]["type"].is_null()) {
								discordPtr->directMessages[i].type = j_complete[i]["type"].get<long>();
							}
							else {
								discordPtr->directMessages[i].type = 1;
							}
							if (!j_complete[i]["id"].is_null()) {
								discordPtr->directMessages[i].id = j_complete[i]["id"].get<std::string>();
							}
							else {
								discordPtr->directMessages[i].id = "0000000000000000";
							}
							if (!j_complete[i]["recipients"].is_null()) {
								discordPtr->directMessages[i].recipients.clear();
								int recAmount = j_complete[i]["recipients"].size();
								for (int r = 0; r < recAmount; r++) {
									// author :
									discordPtr->directMessages[i].recipients.push_back(user());
									if (!j_complete[i]["recipients"][r]["username"].is_null()) {
										discordPtr->directMessages[i].recipients[r].username = j_complete[i]["recipients"][r]["username"].get<std::string>();
									}
									else {
										discordPtr->directMessages[i].recipients[r].username = "";
									}

									if (!j_complete[i]["recipients"][r]["discriminator"].is_null()) {
										discordPtr->directMessages[i].recipients[r].discriminator = j_complete[i]["recipients"][r]["discriminator"].get<std::string>();
									}
									else {
										discordPtr->directMessages[i].recipients[r].discriminator = "";
									}

									if (!j_complete[i]["recipients"][r]["id"].is_null()) {
										discordPtr->directMessages[i].recipients[r].id = j_complete[i]["recipients"][r]["id"].get<std::string>();
									}
									else {
										discordPtr->directMessages[i].recipients[r].id = "";
									}

									if (!j_complete[i]["recipients"][r]["avatar"].is_null()) {
										discordPtr->directMessages[i].recipients[r].avatar = j_complete[i]["recipients"][r]["avatar"].get<std::string>();
									}
									else {
										discordPtr->directMessages[i].recipients[r].avatar = "";
									}


								}
							}




						}

					}

				}
				catch (const std::exception& e) {
				}

			}
			for (int i = 1; i < discordPtr->guildsAmount + 1; i++) {
				std::string channelUrl = "https://discordapp.com/api/guilds/" + discordPtr->guilds[i].id + "/channels";
				D3DSNet::http_response channelresponse = discordPtr->D3dsNet.curlDiscordGet(channelUrl, token);
				if (channelresponse.httpcode == 200) {
					try {
						nlohmann::json j_complete = nlohmann::json::parse(channelresponse.body);
						if (!j_complete.is_null()) {
							discordPtr->guilds[i].channels.clear();
							int channelsAmount = j_complete.size();
							int cindex = 0;
							for (int c = 0; c < channelsAmount; c++) {
								if (!j_complete[c].is_null()) {
									if (!j_complete[c]["type"].is_null())
										if (j_complete[c]["type"].get<std::string>() == "voice") {
											cindex++;
											continue;
										}
									discordPtr->guilds[i].channels.push_back(channel());
									if (!j_complete[c]["type"].is_null()) {
										discordPtr->guilds[i].channels[c - cindex].type = j_complete[c]["type"].get<std::string>();
									}
									else {
										discordPtr->guilds[i].channels[c - cindex].type = "";
									}

									if (!j_complete[c]["id"].is_null()) {
										discordPtr->guilds[i].channels[c - cindex].id = j_complete[c]["id"].get<std::string>();
									}
									else {
										discordPtr->guilds[i].channels[c - cindex].id = "";
									}

									if (!j_complete[c]["name"].is_null()) {
										discordPtr->guilds[i].channels[c - cindex].name = j_complete[c]["name"].get<std::string>();
									}
									else {
										discordPtr->guilds[i].channels[c - cindex].name = "name unavailable";
									}

									if (!j_complete[c]["topic"].is_null()) {
										discordPtr->guilds[i].channels[c - cindex].topic = j_complete[c]["topic"].get<std::string>();
									}
									else {
										discordPtr->guilds[i].channels[c - cindex].topic = "";
									}

									if (!j_complete[c]["is_private"].is_null()) {
										discordPtr->guilds[i].channels[c - cindex].is_private = j_complete[c]["is_private"].get<bool>();
									}
									else {
										discordPtr->guilds[i].channels[c - cindex].is_private = false;
									}

									if (!j_complete[c]["last_message_id"].is_null()) {
										discordPtr->guilds[i].channels[c - cindex].last_message_id = j_complete[c]["last_message_id"].get<std::string>();
									}
									else {
										discordPtr->guilds[i].channels[c - cindex].last_message_id = false;
									}
								}

							}

						}
					}
					catch (const std::exception& e) {

					}
				}

			}

			discordPtr->loadingData = false;
		}
	}
	logSD("end of thread_loadData()");
}
void Discord::thread_refreshMessages(void * arg)
{
	Discord *discordPtr = reinterpret_cast<Discord *>(arg);
	while (discordPtr->inChannel) {
		discordPtr->refreshMessages();
		svcSleepThread(100000);
	}
}



void thrd_loaddata(void * arg) {
	Discord *discordPtr = reinterpret_cast<Discord *>(arg);
	discordPtr->thread_loadData(discordPtr);
	threadExit(0);
}

void Discord::loadData(){
	logSD("inside loadData()");
	loadingData = true;
	s32 prio = 0;
	svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
	threadLoadData = threadCreate(thrd_loaddata, this, 0x10000, prio - 1, -2, false);
	logSD("end of loadData()");
	
}

std::string Discord::getUserAvatar(std::string userid, std::string avatarid)
{
	std::string avatarUrl = "https://images.discordapp.net/avatars/" + userid + "/" + avatarid + ".png?size=64";
	D3DSNet::http_response avatarresponse = D3dsNet.curlDiscordGet(avatarUrl, "");
	return avatarresponse.body;
}

long Discord::fetchUserData(){
	
	logSD("Fetching userdata");
	std::string userDataUrl = "https://discordapp.com/api/users/@me";
	D3DSNet::http_response userdataresponse = D3dsNet.curlDiscordGet(userDataUrl , token);
	logSD("userdata response : " + userdataresponse.body);
	if(userdataresponse.httpcode == 200){
		// check if Two-Factor-Authentication is activated and needs further user action
		nlohmann::json j_complete = nlohmann::json::parse(userdataresponse.body);
		if(!j_complete.is_null()){
			if(!j_complete["username"].is_null()){
				username = j_complete["username"].get<std::string>();
			}
			if(!j_complete["verified"].is_null()){
				verified = j_complete["verified"].get<bool>();
			}
			if(!j_complete["mfa_enabled"].is_null()){
				mfa_enabled = j_complete["mfa_enabled"].get<bool>();
			}
			if(!j_complete["id"].is_null()){
				id = j_complete["id"].get<std::string>();
			}
			if(!j_complete["phone"].is_null()){
				phone = j_complete["phone"].get<std::string>();
			}
			if(!j_complete["avatar"].is_null()){
				avatar = j_complete["avatar"].get<std::string>();
			}
			if(!j_complete["discriminator"].is_null()){
				discriminator = j_complete["discriminator"].get<std::string>();
			}
		}
		
		
	}
	
	return userdataresponse.httpcode;
	
}

void Discord::getGuilds(){
	std::string guildUrl = "https://discordapp.com/api/users/@me/guilds";
}
void Discord::getChannels(){
	
}
std::string Discord::getUsername(){
	return username;
}
std::string Discord::getEmail(){
	return email;
}
std::string Discord::getPassword(){
	return password;
}
void Discord::setEmail(std::string mail){
	email = mail;
}
void Discord::setPassword(std::string pass){
	password = pass;
}
long Discord::login(){
	return login(email , password);
}
long Discord::login(std::string mail , std::string pass){
	logSD("Login attempt");
	email = mail;
	password = pass;
	
	if(token.length() > 20){
		if(fetchUserData() == 200){
			loggedin = true;
			return 200;
		}else{
			token = "";
		}
		
	}
	
	std::string loginUrl = "https://discordapp.com/api/auth/login";
	std::string postData = "{ \"email\":\"" + email + "\" , \"password\":\"" + password + "\" }";
	D3DSNet::http_response loginresponse = D3dsNet.curlDiscordPost(loginUrl , postData , token);
	logSD("Login response:");
	logSD(loginresponse.body);
	if(loginresponse.httpcode == 200){
		// check if Two-Factor-Authentication is activated and needs further user action
		nlohmann::json j_complete = nlohmann::json::parse(loginresponse.body);
		try{
			if(!j_complete.is_null()){
				if(!j_complete["mfa"].is_null()){
					if(!j_complete["ticket"].is_null()){
						// Two factor auth is enabled ! grab ticket
						twoFactorAuthEnabled = true;
						ticket = j_complete["ticket"].get<std::string>();
						logSD("Need 2FA Code");
						loginresponse.httpcode = 200000; // NEED MFA
					}
				}else if(!j_complete["token"].is_null()){
					// Logged in !
					token = j_complete["token"].get<std::string>();
					loggedin = true;
					fetchUserData();
				}
				
			}
		}catch(const std::exception& e){
			
		}
		
		
	}
	else{
		// login failed >_>
		
	}
	return loginresponse.httpcode;
	
}
long Discord::submit2facode(std::string code){
	logSD("Submit 2FA Attempt");
	code2fa = code;

	std::string postData = "{ \"code\":\"" + code2fa + "\" , \"ticket\":\"" + ticket + "\" }";
	D3DSNet::http_response submit2facoderesponse = D3dsNet.curlDiscordPost( "https://discordapp.com/api/v6/auth/mfa/totp" , postData , token);
	logSD("Submit 2FA Response:");
	logSD(submit2facoderesponse.body);
	if(submit2facoderesponse.httpcode == 200){
		
		nlohmann::json j_complete2 = nlohmann::json::parse(submit2facoderesponse.body);
		if(!j_complete2.is_null()){
			if(!j_complete2["token"].is_null()){
				token = j_complete2["token"].get<std::string>();
				loggedin = true;
				fetchUserData();
			}
		}
		
	}
	logSD("Return 2FA httpcode:");
	return submit2facoderesponse.httpcode;
	
}
std::string Discord::getToken(){
	return token;
}
std::string Discord::getTicket(){
	return ticket;
}

