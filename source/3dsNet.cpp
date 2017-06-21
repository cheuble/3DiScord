#include "3DSNet.hpp"
#include "log.hpp"


D3DSNet::D3DSNet(){
	init();
}

D3DSNet::~D3DSNet(){
	terminate();
}

struct stringcurl {
  char *ptr;
  size_t len;
};
void init_string(struct stringcurl *s) {
  s->len = 0;
  s->ptr = (char*)malloc(s->len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}
size_t writefunc(void *ptr, size_t size, size_t nmemb, struct stringcurl *s)
{
  size_t new_len = s->len + size*nmemb;
  s->ptr = (char*)realloc(s->ptr, new_len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
}

D3DSNet::http_response D3DSNet::curlDiscordGet(std::string url, std::string authtoken) {
	D3DSNet::http_response resp;
	struct stringcurl body;
	init_string(&body);
	struct stringcurl header;
	init_string(&header);
	u8 method = HTTPC_METHOD_GET;
	const char* postdata = NULL;
	httpcContext context;
	HTTPC_RequestMethod useMethod = HTTPC_METHOD_GET;

	if (method <= 3 && method >= 1) useMethod = (HTTPC_RequestMethod)method;

	u32 statuscode = 0;
	do {
		if (statuscode >= 301 && statuscode <= 308) {
			char newurl[4096];
			httpcGetResponseHeader(&context, (char*)"Location", &newurl[0], 4096);
			url = &newurl[0];

			httpcCloseContext(&context);
		}

		Result ret = httpcOpenContext(&context, useMethod, (char*)url.c_str(), 0);

		// Lets just disable SSL verification instead of loading default certs.
		httpcSetSSLOpt(&context, SSLCOPT_DisableVerify);
		httpcAddRequestHeaderField(&context, "Content-Type", "application/json");
		httpcAddRequestHeaderField(&context, "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64)");
		if (authtoken != " ")
			httpcAddRequestHeaderField(&context, "Authorization", authtoken.c_str());

		if (useMethod == HTTPC_METHOD_POST && postdata != NULL) {
			httpcAddPostDataRaw(&context, (u32*)postdata, strlen(postdata));
		}
		
		if (ret == 0) {
			httpcBeginRequest(&context);
			long int contentsize = 0; // Crash on the += if u32. WTF?
			u32 readSize = 0;
			httpcGetResponseStatusCode(&context, &statuscode);
			if (statuscode == 200) {
				unsigned char *buffer = (unsigned char*)malloc(0x1000);
				do {
					ret = httpcDownloadData(&context, buffer + contentsize, 0x1000, &readSize);
					contentsize += readSize;
					buffer[contentsize] = '\0';
					if (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING)
						buffer = (unsigned char*)realloc(buffer, contentsize + 0x1000);
				} while (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING);
				buffer = (unsigned char*)realloc(buffer, contentsize + 1);
				buffer[contentsize] = 0;
				body.ptr = (char*)buffer;
				body.len = contentsize;
			}
		}
	} while ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308));
	if ((statuscode < 200 && statuscode > 226) && statuscode != 304) logSD("Error while downloading target");
	httpcCloseContext(&context);
	resp.httpcode = statuscode;
	resp.header = std::string(header.ptr, header.len);
	resp.body = std::string(body.ptr, body.len);
	free(header.ptr);
	free(body.ptr);
	return resp;
}

D3DSNet::http_response D3DSNet::curlDiscordPost(std::string url , std::string postdata , std::string authtoken){

	D3DSNet::http_response resp;
	struct stringcurl body;
	init_string(&body);
	struct stringcurl header;
	init_string(&header);
	u8 method = HTTPC_METHOD_POST;
	httpcContext context;
	HTTPC_RequestMethod useMethod = HTTPC_METHOD_POST;
	if (method <= 3 && method >= 1) useMethod = (HTTPC_RequestMethod)method;
	char * postd = (char*)postdata.c_str();

	u32 statuscode = 0;
	do {
		if (statuscode >= 301 && statuscode <= 308) {
			char newurl[4096];
			httpcGetResponseHeader(&context, (char*)"Location", &newurl[0], 4096);
			url = &newurl[0];

			httpcCloseContext(&context);
		}

		Result ret = httpcOpenContext(&context, useMethod, (char*)url.c_str(), 0);

		// Lets just disable SSL verification instead of loading default certs.
		httpcSetSSLOpt(&context, SSLCOPT_DisableVerify);
		httpcAddRequestHeaderField(&context, "Content-Type", "application/json");
		httpcAddRequestHeaderField(&context, "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64)");
		if(authtoken != " ")
			httpcAddRequestHeaderField(&context, "Authorization", authtoken.c_str());

		if (useMethod == HTTPC_METHOD_POST && postd != NULL) {
			httpcAddPostDataRaw(&context, (u32*)postd, strlen(postd));
		}

		if (ret == 0) {
			httpcBeginRequest(&context);
			long int contentsize = 0; // Crash on the += if u32. WTF?
			u32 readSize = 0;
			httpcGetResponseStatusCode(&context, &statuscode);
			if (statuscode == 200) {
				unsigned char *buffer = (unsigned char*)malloc(0x1000);
				do {
					ret = httpcDownloadData(&context, buffer + contentsize, 0x1000, &readSize);
					contentsize += readSize;
					if (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING)
						buffer = (unsigned char*)realloc(buffer, contentsize + 0x1000);
				} while (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING);
				buffer = (unsigned char*)realloc(buffer, contentsize + 1);
				buffer[contentsize] = '\0';
				body.ptr = (char*)buffer;
				body.len = contentsize;
			}
		}
	} while ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308));
	if ((statuscode < 200 && statuscode > 226) && statuscode != 304) logSD("error opening url");
	httpcCloseContext(&context);
	resp.httpcode = statuscode;
	resp.header = std::string(header.ptr, header.len);
	resp.body = std::string(body.ptr, body.len);
	free(header.ptr);
	free(body.ptr);
	return resp;
}


void D3DSNet::init(){
	int size = 4*1024*1024;
	httpcInit(size);
}
void D3DSNet::terminate(){
	httpcExit();
}



