

#include "../DataStructures/Trees/BinarySearchTree.h"
#include "../Networking/Nodes/HTTPServer.h"

char *home(struct HTTPServer *server, struct HTTPRequest *request) {
	char *page = render_template("D:/CppProject/index.html",
	                             2, // Number of variables
	                             "title", "My Index", "content",
	                             "This is the home page content.");
	return page;
}

char *about(struct HTTPServer *server, struct HTTPRequest *request) {
	char *content_type = request->request_line.search(
	    &request->header_fields, "Content-Type", sizeof("Content-Type"));
	if (content_type && strcmp(content_type, "application/json") == 0) {
		char *res =
		    request->body.search(&request->body, "name", sizeof("name"));
		if (res) {
			printf("Your name is : %s\n", res);
		}
	}

	char *page = render_template("D:/CppProject/about.html",
	                             2, // Number of variables
	                             "title", "My About", "content",
	                             "This is the about page content.");
	return page;
}

int main() {

#ifdef _WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		// Error handling
		return 1;
	}
#endif

	const char *host =
	    "127.0.0.1"; // or "0.0.0.0" to bind to all available interfaces
	int port = 8080; // Specify the port you want to use

	struct HTTPServer *server = http_server_constructor(host, port);

	server->register_routes(server, home, "/", 2, GET, POST);
	server->register_routes(server, about, "/about", 2, GET, POST);

	server->launch(server);

	http_server_destructor(server);
#ifdef _WIN32
	WSACleanup();
#endif

	return 0;
}
