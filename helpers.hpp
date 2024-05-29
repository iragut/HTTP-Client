#include <stddef.h>
#ifndef _HELPERS_
#define _HELPERS_

#define PORT 8080
#define BUFLEN 4096
#define LINELEN 1000

#define ERROR "[ERROR]: "
#define SUCCES "[SUCCES]: "

// defines for URL and utils for client
#define IP_HOST "34.246.184.49"
#define HEADER_TERMINATOR "\r\n\r\n"
#define JSON_TYPE "application/json"
#define CONTENT_LENGTH "Content-Length: "
#define REGISTER_URL "/api/v1/tema/auth/register"
#define LOGIN_URL "/api/v1/tema/auth/login"
#define LIBRARY_ACCESS_URL "/api/v1/tema/library/access"
#define BOOKS_URL "/api/v1/tema/library/books"
#define LOGOUT_URL "/api/v1/tema/auth/logout"

#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)

typedef struct {
    char *data;
    size_t size;
} buffer;

// shows the current error
void error(const char *msg);

// adds a line to a string message
void compute_message(char *message, const char *line);

// opens a connection with server host_ip on port portno, returns a socket
int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag);

// closes a server connection on socket sockfd
void close_connection(int sockfd);

// send a message to a server
void send_to_server(int sockfd, char *message);

// receives and returns the message from a server
char *receive_from_server(int sockfd);

// extracts and returns a JSON from a server response
char *basic_extract_json_response(char *str);

// initializes a buffer
buffer buffer_init(void);

// destroys a buffer
void buffer_destroy(buffer *buffer);

// adds data of size data_size to a buffer
void buffer_add(buffer *buffer, const char *data, size_t data_size);

// checks if a buffer is empty
int buffer_is_empty(buffer *buffer);

// finds data of size data_size in a buffer and returns its position
int buffer_find(buffer *buffer, const char *data, size_t data_size);

// finds data of size data_size in a buffer in a
// case-insensitive fashion and returns its position
int buffer_find_insensitive(buffer *buffer, const char *data, size_t data_size);

// computes and returns a GET and DELETE request string (query_params
// ,cookies and jwt_token can be set to NULL if not needed)
char *compute_get_request(char *host, char *url, char *query_params,
							char **cookies, int cookies_count, char *jwt_token, bool delet);

// computes and returns a POST request string (cookies and jwt_token can be set to NULL if not needed)
char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
							int body_data_fields_count, char** cookies, int cookies_count, char *jwt_token);

#endif
