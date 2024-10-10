// CWebServer.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#ifndef Server_h
#define Server_h

#include "../../DataStructures/Dictionary/Dictionary.h"
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>              // For inet_pton and inet_ntop
#pragma comment(lib, "Ws2_32.lib") // Link with Winsock library
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

struct Server {
	int domain;
	int service;
	int protocol;
	u_long inter;
	int port;
	int backlog;

	struct sockaddr_in address;

	int socket;

	struct Dictionary routes;

	void (*register_routes)(struct Server *server,
	                        char *(*route_function)(void *arg), char *path);
};

struct ServerRoute {
	char *(*route_function)(void *arg);
};

struct Server *server_constructor(int domain, int service, int protocol,
                                  u_long inter, int port, int backlog);

void server_destructor(struct Server *server);

#endif
