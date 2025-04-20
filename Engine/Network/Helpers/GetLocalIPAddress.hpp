#pragma once

#include <iostream>
#include <cstdlib>
#include <regex>

#include <yojimbo.h>

namespace Engine
{
#ifdef OS_WINDOWS
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#include <winsock2.h>
	#include <ws2tcpip.h>

	// Windows getter of the current device's ip
	static inline yojimbo::Address GetLocalIPAddress(uint16_t port) {
		WSADATA wsaData;
		char hostname[256];

		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			std::cerr << "WSAStartup failed!" << std::endl;
			exit(1);
		}

		if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
			std::cerr << "Error getting hostname" << std::endl;
			WSACleanup();
			exit(1);
		}

		struct addrinfo hints = { 0 }, * info;
		hints.ai_family = AF_INET;

		if (getaddrinfo(hostname, NULL, &hints, &info) != 0) {
			std::cerr << "getaddrinfo failed!" << std::endl;
			WSACleanup();
			exit(1);
		}

		std::string add_str = "";

		struct sockaddr_in* addr = (struct sockaddr_in*)info->ai_addr;
		add_str = inet_ntoa(addr->sin_addr);

		/*if (!isValidIPAddress(add_str.c_str()))
		{
			std::cerr << "Invalid IP address\n";
			exit(1);
		}*/

		yojimbo::Address address(add_str.c_str(), port);
		freeaddrinfo(info);
		WSACleanup();
		return address;
	}

#elif defined(OS_LINUX)
	#include <sys/types.h>
	#include <ifaddrs.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <cstring>

	// Linux getter of the current device's ip
	static inline yojimbo::Address GetLocalIPAddress(uint16_t port) {
		struct ifaddrs* ifAddrStruct = NULL;
		struct ifaddrs* ifa = NULL;
		void* tmpAddrPtr = NULL;

		if (getifaddrs(&ifAddrStruct) == -1) {
			std::cerr << "Error getting network interfaces" << std::endl;
			exit(1);
		}

		for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
			if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
				tmpAddrPtr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
				char addressBuffer[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
				if (!strncmp(ifa->ifa_name, "eno1", 4))
				{
					yojimbo::Address address(addressBuffer, port);
					std::cout << port << std::endl;
					return address;
				}
			}
		}

		std::cerr << "No valid inet eno1 device" << std::endl;
		exit(1);
	}

#endif
}
