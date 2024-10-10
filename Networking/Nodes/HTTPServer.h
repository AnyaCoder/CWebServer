
#ifndef HTTPServer_h
#define HTTPServer_h

#include "../Protocols/HTTPRequest.h"
#include "Server.h"
#ifdef DELETE
#undef DELETE
#endif

enum HTTPMethod {
	TRACE = 0b00000000,
	GET = 0b00000001,
	POST = 0b00000010,
	PUT = 0b00000100,
	DELETE = 0b00001000,
	PATCH = 0b00010000,
	HEAD = 0b00100000,
	OPTIONS = 0b01000000,
	CONNECT = 0b10000000
};

struct HTTPServer {
	struct Server *server;
	struct Dictionary routes;
	void (*register_routes)(struct HTTPServer *server,
	                        void (*route_function)(struct HTTPServer *server,
	                                               struct HTTPRequest *request),
	                        char *uri, int num_methods, ...);
	void (*launch)(struct HTTPServer *server);
};

struct HTTPServer *http_server_constructor(const char *host, int port);
void http_server_destructor(struct HTTPServer *http_server);
void register_routes(struct HTTPServer *server,
                     void (*route_function)(struct HTTPServer *server,
                                            struct HTTPRequest *request),
                     char *uri, int num_methods, ...);
char *render_template(const char *path, int num_vars, ...);
void launch(struct HTTPServer *server);
#endif
