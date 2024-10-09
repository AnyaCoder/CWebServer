#include "HTTPServer.h"

#include "../../System/ThreadPool.h"
#include "../../thirdparty/rxiLog/log.h"

#if !defined( _WIN32)
#include<unistd.h>
#endif // !_WIN32

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void* handler(void* arg);
void launch(struct HTTPServer* server);

struct ClientServer
{
    int client;
    struct HTTPServer* server;
};

struct Route
{
    enum HTTPMethod methods;
    char* uri;
    char* (*route_function)(struct HTTPServer* server, struct HTTPRequest* request);
};

struct HTTPServer* http_server_constructor(const char* host, int port)
{
    struct HTTPServer* server = (struct HTTPServer*)malloc(sizeof(struct HTTPServer));
    // Convert the host to an IP address
    u_long ip_addr = inet_addr(host);
    server->server = server_constructor(AF_INET, SOCK_STREAM, 0, ip_addr, port, 255);
    server->routes = dictionary_constructor(compare_string_keys);
    server->register_routes = register_routes;
    server->launch = launch;
    return server;
}

void http_server_destructor(struct HTTPServer* http_server)
{
    server_destructor(&http_server->server);
    dictionary_destructor(&http_server->routes);
}

// 注册路由
void register_routes(
    struct HTTPServer* server,
    void(*route_function)(struct HTTPServer* server, struct HTTPRequest* request),
    char* uri, int num_methods, ...
)
{
    struct Route route;
    route.methods = 0;

    va_list methods;
    va_start(methods, num_methods);
    for (int i = 0; i < num_methods; i++)
    {
        enum HTTPMethod cur = va_arg(methods, int);
        route.methods |= (int)cur;
    }
 
    route.uri = (char*)malloc(sizeof(char) * (strlen(uri) + 1));
    if (route.uri == NULL)
    {
        log_error("route.uri malloc failed");
        exit(1);
    }
    else {
        strcpy(route.uri, uri);
    }

    route.route_function = route_function;
    server->routes.insert(
        &server->routes, 
        uri, sizeof(char) * (strlen(uri) + 1),
        &route, sizeof(route)
    );

}


void launch(struct HTTPServer* server)
{
    struct ThreadPool* thread_pool = thread_pool_constructor(10);
    struct sockaddr* sock_addr = (struct sockaddr*)&server->server->address;
    socklen_t address_length = sizeof(server->server->address);

    const char* ip_str = inet_ntoa(server->server->address.sin_addr);

    log_info("\033[32mHTTP Server is launched at http://%s:%d\n\033[0m", ip_str, server->server->port);

    while (1)
    {
        struct ClientServer* client_server =
            (struct ClientServer*)malloc(sizeof(struct ClientServer));
        client_server->client = accept(
            server->server->socket,
            sock_addr,
            &address_length
        );
        client_server->server = server;
        struct ThreadJob job = thread_job_constructor(handler, client_server);
        thread_pool->add_work(thread_pool, job);
    }
}


void send_404_response(int client) {
    const char* not_found_response =
        "HTTP/1.1 404 NOT FOUND\r\n"
        "Content-Type: text/plain\r\n\r\n"
        "404 NOT FOUND!";
    time_t current_time;
    time(&current_time);
    struct tm* local_time = localtime(&current_time);

    log_error("==>\033[31m\n%s\n\033[0m", not_found_response);
#if defined (_WIN32) || defined (_WIN64)
    send(client, not_found_response, strlen(not_found_response), 0);
#else
    write(client, not_found_response, strlen(not_found_response));
#endif
}

void send_file_response(int client, const char* file_path) {

    FILE* file = fopen(file_path, "rb");
    if (file != NULL) {
        // 读取文件内容并发送
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        char* file_content = (char*)malloc(file_size + 1);
        fread(file_content, 1, file_size, file);
        file_content[file_size] = '\0';

        fclose(file);

        // 构建 HTTP 响应头
        const char* header_template = "HTTP/1.1 200 OK\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %ld\r\n\r\n";

        // 根据文件扩展名设置Content-Type
        char* content_type = "text/plain";  // 默认值
        if (strstr(file_path, ".html")) {
            content_type = "text/html";
        }
        else if (strstr(file_path, ".css")) {
            content_type = "text/css";
        }
        else if (strstr(file_path, ".js")) {
            content_type = "application/javascript";
        }
        else if (strstr(file_path, ".png")) {
            content_type = "image/png";
        }
        else if (strstr(file_path, ".jpg") || strstr(file_path, ".jpeg")) {
            content_type = "image/jpeg";
        }

        // 计算响应头的大小并生成
        char header[256];
        snprintf(header, sizeof(header), header_template, content_type, file_size);

        // 发送响应头和文件内容
#if defined (_WIN32) || defined (_WIN64)
        send(client, header, strlen(header), 0);
        send(client, file_content, file_size, 0);
#else
        write(client, header, strlen(header));
        write(client, file_content, file_size);
#endif

        free(file_content);
    }
    else {
        // 文件未找到，发送 404 错误响应
        send_404_response(client);
    }
}

void* handler(void* arg) {

    struct ClientServer* client_server = (struct ClientServer*)arg;

    char request_string[30000];
#if defined (_WIN32) || defined (_WIN64)
    int bytes_received = recv(client_server->client, request_string, 30000, 0);
#else
    int bytes_received = read(client_server->client, request_string, 30000);
#endif
    if (bytes_received > 0) {
        log_info("\033[32m Received %d bytes\n\033[0m", bytes_received);
        request_string[bytes_received] = '\0';
    }
    else if (bytes_received == 0) {
        log_warn("\033[31mConnection closed\n\033[0m");
    }
    else {
        log_error("\033[31mReceive failed\n\033[0m");
    }

    struct HTTPRequest request = http_request_constructor(request_string);

    char* method = (char*)request.request_line.search(&request.request_line, "method", sizeof("method"));
    char* uri = (char*)request.request_line.search(&request.request_line, "uri", sizeof("uri"));
    struct Route* route = (struct Route*)client_server->server->routes.search(
        &client_server->server->routes, uri, sizeof(char) * (strlen(uri) + 1)
    );

    if (route == NULL) {
        // 如果没有找到路由，尝试发送文件响应
        send_file_response(client_server->client, uri);
    }
    else if (
        ((route->methods & GET) == 0 && strcmp(method, "GET") == 0) ||
        ((route->methods & POST) == 0 && strcmp(method, "POST") == 0) ||
        ((route->methods & PUT) == 0 && strcmp(method, "PUT") == 0) ||
        ((route->methods & DELETE) == 0 && strcmp(method, "DELETE") == 0) ||
        ((route->methods & PATCH) == 0 && strcmp(method, "PATCH") == 0) ||
        ((route->methods & HEAD) == 0 && strcmp(method, "HEAD") == 0) ||
        ((route->methods & OPTIONS) == 0 && strcmp(method, "OPTIONS") == 0)
        ) {
        send_404_response(client_server->client);
    }
    else {
        char* response = route->route_function(client_server->server, &request);
        
        // 构建 HTTP 响应头
        const char* header_template = "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %ld\r\n\r\n";
        char header[256];
        size_t len_response = strlen(response);
        snprintf(header, sizeof(header), header_template, len_response);
        log_info("\033[33m Responsed %d bytes\n\033[0m", len_response);
        // 发送响应头和内容
#if defined (_WIN32) || defined (_WIN64)
        send(client_server->client, header, strlen(header), 0);
        send(client_server->client, response, len_response, 0);
#else
        write(client_server->client, header, strlen(header));
        write(client_server->client, response, strlen(response));
#endif
    }

#if defined (_WIN32) || defined (_WIN64)
    closesocket(client_server->client);
#else
    close(client_server->client);
#endif
    free(client_server);
    return NULL;
}



// Function to replace a variable in the template with its value
void replace_variable(char* buffer, size_t buffer_size, const char* placeholder, const char* value) {
    char* pos = strstr(buffer, placeholder);
    if (!pos) return;

    // Calculate lengths for buffer manipulation
    size_t placeholder_len = strlen(placeholder);
    size_t value_len = strlen(value);
    size_t before_len = pos - buffer;
    size_t after_len = strlen(pos + placeholder_len);

    // Check if the new buffer size will exceed the limit
    size_t new_size = before_len + value_len + after_len + 1;
    if (new_size > buffer_size) {
        log_error("Buffer size exceeded during replacement\n");
        return;
    }

    // Shift the contents after the placeholder if necessary
    memmove(pos + value_len, pos + placeholder_len, after_len + 1);
    memcpy(pos, value, value_len);
}

char* render_template(const char* path, int num_vars, ...) {
    // Initialize buffer variables
    size_t buffer_size = 65536;  // Start with 64KB buffer size
    char* buffer = malloc(buffer_size);
    if (buffer == NULL) {
        log_error("Memory allocation failed\n");
        return NULL;
    }

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        log_error("Could not open file: %s\n", path);
        free(buffer);
        return NULL;
    }

    // Read file contents dynamically
    size_t buffer_position = 0;
    size_t chunk_size = 4096;
    char* chunk = (char*)malloc(chunk_size);

    while (1) {
        size_t bytes_read = fread(chunk, 1, chunk_size, file);
        if (bytes_read == 0) {
            break;
        }

        // Check if we need to expand the buffer
        while (buffer_position + bytes_read >= buffer_size) {
            buffer_size *= 2;
            char* temp = (char*)realloc(buffer, buffer_size);
            if (temp == NULL) {
                log_error("Memory reallocation failed\n");
                free(buffer);
                fclose(file);
                return NULL;
            }
            buffer = temp;
        }

        // Copy the chunk into the buffer
        memcpy(buffer + buffer_position, chunk, bytes_read);
        buffer_position += bytes_read;
    }
    free(chunk);
    buffer[buffer_position] = '\0';  // Null-terminate the buffer
    fclose(file);

    // Handle variables replacement
    va_list args;
    va_start(args, num_vars);

    for (int i = 0; i < num_vars; i++) {
        const char* variable = va_arg(args, const char*);
        const char* value = va_arg(args, const char*);

        // Construct the placeholder syntax {{ variable }}
        char placeholder[256];
        snprintf(placeholder, sizeof(placeholder), "{{ %s }}", variable);

        // Replace occurrences of the placeholder with the actual value
        replace_variable(buffer, buffer_size, placeholder, value);
    }

    va_end(args);

    return buffer;
}