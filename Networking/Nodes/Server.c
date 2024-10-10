// CWebServer.cpp: 定义应用程序的入口点。
//

#include "Server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void register_routes_server(struct Server *server,
                            char *(*route_function)(void *arg), char *path);

struct Server *server_constructor(int domain, int service, int protocol,
                                  u_long inter, int port, int backlog) {
	struct Server *server = (struct Server *)malloc(sizeof(struct Server));
	if (server == NULL) {
		perror("Failed to allocate memory for server");
		exit(1);
	}

	server->domain = domain;
	server->service = service;
	server->protocol = protocol;
	server->inter = inter;
	server->port = port;
	server->backlog = backlog;

	server->address.sin_family = domain;
	server->address.sin_port = htons(port);
	server->address.sin_addr.s_addr = inter;

	server->socket = socket(domain, service, protocol);
	server->routes = dictionary_constructor(compare_string_keys);
	server->register_routes = register_routes_server;

	if (server->socket < 0) {
		perror("Failed to create socket");
		free(server); // Free allocated memory before exiting
		exit(1);
	}

	if (bind(server->socket, (struct sockaddr *)&server->address,
	         sizeof(server->address)) < 0) {
		perror("Failed to bind socket");
		close(server->socket);
		free(server);
		exit(1);
	}

	if (listen(server->socket, server->backlog) < 0) {
		perror("Failed to start listening");
		close(server->socket);
		free(server);
		exit(1);
	}

	return server;
}

void server_destructor(struct Server *server) {
	dictionary_destructor(&server->routes);
}

void register_routes_server(struct Server *server,
                            char *(*route_function)(void *arg), char *path) {
	struct ServerRoute route;
	route.route_function = route_function;
	server->routes.insert(&server->routes, path,
	                      sizeof(char) * (strlen(path) + 1), &route,
	                      sizeof(route));
}
