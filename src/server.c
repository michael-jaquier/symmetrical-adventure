// server.c

#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <unistd.h>     // For read/write/close
#include <errno.h>      // For errno
#include <string.h>     // For memset
#include <stdlib.h>     // For exit
#include <stdio.h>      // For fprintf
#include <arpa/inet.h>  // For inet_ntop

#define PORT 8080
#define BUFFER_SIZE 1024

int start_listener(int server_fd, int backlog)
{
    if (listen(server_fd, backlog) == -1)
    {
        fprintf(stderr, "Listen failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("Listening on port %d\n", PORT);
    return 0;
}


int handle_post_request(int client_fd, char *buffer)
{
    printf("Received: %s\n", buffer);
    char *response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
    int rc = write(client_fd, response, strlen(response));
    if ( rc ) {
        fprintf(stderr, "Write failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return 0;
}

typedef enum
{
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE,
    HTTP_HEAD,
    HTTP_OPTIONS,
    HTTP_PATCH,
    HTTP_CONNECT,
    HTTP_TRACE,
    HTTP_UNKNOWN
} HttpMethod;
;

uint32_t fnv1a_hash(const char *str)
{
    uint32_t hash = 2166136261u;
    while (*str)
    {
        hash ^= (uint8_t)(*str);
        hash *= 16777619u;
        str++;
    }
    return hash;
}

// Hash table mapping precomputed hash values to enum values
typedef struct
{
    uint32_t hash;
    HttpMethod method;
} HashEntry;

// Precomputed hashes for HTTP methods
const HashEntry http_hash_table[] = {
    {0x811c9dc5, HTTP_GET},     // "GET"
    {0xe40c292c, HTTP_POST},    // "POST"
    {0xe40c290a, HTTP_PUT},     // "PUT"
    {0xe40c2914, HTTP_DELETE},  // "DELETE"
    {0xe40c28f7, HTTP_HEAD},    // "HEAD"
    {0xe40c2922, HTTP_OPTIONS}, // "OPTIONS"
    {0xe40c2918, HTTP_PATCH},   // "PATCH"
    {0xe40c2905, HTTP_CONNECT}, // "CONNECT"
    {0xe40c290e, HTTP_TRACE}    // "TRACE"
};

#define NUM_HTTP_HASHES (sizeof(http_hash_table) / sizeof(http_hash_table[0]))

HttpMethod get_http_method(const char *request)
{
    // Extract HTTP method (first word before space)
    char method[8] = {0}; // Max method length is 7 chars + null terminator
    int i = 0;

    while (request[i] != ' ' && request[i] != '\0' && i < 7)
    {
        method[i] = request[i];
        i++;
    }
    method[i] = '\0';

    // Compute hash
    uint32_t hash = fnv1a_hash(method);

    // Lookup in hash table
    for (size_t i = 0; i < NUM_HTTP_HASHES; i++)
    {
        if (http_hash_table[i].hash == hash)
        {
            return http_hash_table[i].method;
        }
    }

    return HTTP_UNKNOWN;
}

HTTP_METHOD request_method(char *buffer) {
    strcmp(buffer, "GET") == 0 ? return GET;



    return GET;
}

void handle_connections(int server_fd)
{
    char buffer[BUFFER_SIZE] = {0};
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char client_ip[INET_ADDRSTRLEN];
    while (1)
    {
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd == -1)
        {
            fprintf(stderr, "Accept failed: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("Connection from %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        read(client_fd, buffer, BUFFER_SIZE);
        printf("Received: %s\n", buffer);
        char *response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
        write(client_fd, response, strlen(response));
    }
}

int main() {
    // Server file descriptor and sockets
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Create a socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        fprintf(stderr, "Bind failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    };

    start_listener(server_fd, 128);

    handle_connections(server_fd);

    close(server_fd);
    return 0;


}

