#include <iostream>
#include <cstdlib>
#include <regex>

#include "../third_party/yojimbo/include/yojimbo.h"
#include "../Engine/GameServer.hpp"

bool isValidIPAddress(const std::string& ip) {
	// Define the regex pattern for IPv4 and IPv6
	const std::string ipPattern = R"(^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$)";

	// Create a regex object
	std::regex ipRegex(ipPattern);

	// Check if the IP matches the regex pattern
	return std::regex_match(ip, ipRegex);
}

#ifdef OS_WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>

yojimbo::Address GetLocalIPAddress(uint16_t port) {
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

    struct addrinfo hints = { 0 }, * info, * p;
    hints.ai_family = AF_INET;

    if (getaddrinfo(hostname, NULL, &hints, &info) != 0) {
        std::cerr << "getaddrinfo failed!" << std::endl;
        WSACleanup();
		exit(1);
    }

    std::string add_str = "";

    struct sockaddr_in* addr = (struct sockaddr_in*)info->ai_addr;
	add_str = inet_ntoa(addr->sin_addr);

	if (!isValidIPAddress(add_str.c_str()))
	{
		std::cerr << "Invalid IP address\n";
		exit(1);
	}

	yojimbo::Address address("127.0.0.1", port);
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

yojimbo::Address GetLocalIPAddress(uint16_t port) {
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
			if(!strncmp(ifa->ifa_name, "eno1", 4))
			{
				yojimbo::Address address(addressBuffer, port);
				return address;
			}
		}
	}

	std::cerr << "No valid inet eno1 device" << std::endl;
	exit(1);
}

#endif

int main(int argc, char* argv[])
{
	if (argc != 1)
	{
		std::cerr << "Usage: " << argv[0] << "\n";
		exit(1);
	}

	std::cout << "Enter a port: ";
	std::string port;
	std::cin >> port;
	for (int i = 0; i < port.length(); i++)
	{
		if (!isdigit(port[i]))
		{
			std::cerr << "Non-integer port\n";
			exit(1);
		}
	}

	std::cout << "Enter max number of clients: ";
	std::string maxClients;
	std::cin >> maxClients;

	for (int i = 0; i < maxClients.length(); i++)
	{
		if (!isdigit(maxClients[i]))
		{
			std::cerr << "Non-integer maxClients\n";
			exit(1);
		}
	}

	int portInt = std::stoi(port);
	int maxClientsInt = std::stoi(maxClients);

	if (portInt < 1024 || portInt > 65535)
    {
		std::cerr << "Invalid port number\n";
		std::cerr << "Port number must be between 1024 and 65535\n";
		exit(1);
    }
	if (maxClientsInt < 1 || maxClientsInt > 64)
	{
		std::cerr << "Invalid max clients\n";
		std::cerr << "Max clients must be between 1 and 64\n";
		exit(1);
	}

    if (!InitializeYojimbo())
    {
        std::cerr << "Failed to initialize Yojimbo\n";
		exit(1);
	}
	else
	{
		std::cout << "Yojimbo initialized\n";
	}

#ifdef DEBUG
	yojimbo_log_level(YOJIMBO_LOG_LEVEL_INFO);
#else
	yojimbo_log_level(YOJIMBO_LOG_LEVEL_INFO);
#endif

	// start server at address of machine and max clients from command line
	GameServer server(GetLocalIPAddress(portInt), 2);
	server.Start();

	server.Stop();

    std::cout << "Hello World!\n";

	ShutdownYojimbo();

	exit(0);
}
