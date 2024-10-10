#ifndef Client_h
#define Client_h

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>              // For inet_pton and inet_ntop
#pragma comment(lib, "Ws2_32.lib") // Link with Winsock library
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

struct Client {
	int domain;
	int service;
	int protocol;
	int port;
	u_long inter;

	int socket;
	void (*request)(struct Client *client, char *server_ip, char *request);
};
#endif
