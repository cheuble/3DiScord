#include "log.hpp"
#include <fstream>

void logSD(std::string logData){
	const char * data = logData.c_str();
	std::ofstream ofs;
	ofs.open("sdmc:/3ds/3DiScord/log.txt", std::ofstream::out | std::ofstream::app);
	ofs << data << std::endl;
	ofs.close();
}

