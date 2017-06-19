#ifndef D3DSNET_HPP
#define D3DSNET_HPP

#include <3ds.h>

#include <string>
#include <string.h>

class D3DSNet{
	
	
	public:
		typedef struct{
			long httpcode;
			std::string header;
			std::string body;
		} http_response;
		
		typedef struct{
			std::string url;
			std::string postdata;
			std::string authtoken;
		} thread_request;
		
		D3DSNet();
		~D3DSNet();
		http_response curlDiscordGet(std::string url , std::string authtoken);
		http_response curlDiscordPost(std::string url , std::string postdata , std::string authtoken);



	
	private:
		void init(); 
		void terminate();
		
		
};



#endif




