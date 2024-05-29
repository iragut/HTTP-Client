#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include "helpers.hpp"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// Extract the cookie from the HTTP response
char* get_cookie_from_response(char* response) {
    if (response == NULL)
        return NULL;

    char* cookie = strstr(response, "Set-Cookie: ");

    if (cookie == NULL)
        return NULL;

    cookie += 12;
    cookie = strtok(cookie, "\r\n");
    return cookie;
}

// Extract the jwt token from the HTTP response
char* get_token_from_response(char* response) {
    if (response == NULL)
        return NULL;

    char* token = basic_extract_json_response(response);

    if (token == NULL)
        return NULL;

    json token_json = json::parse(token);
    token = strdup(token_json["token"].dump().c_str());
    token += 1;
    token[strlen(token) - 1] = '\0';
    return token;
}

// Send a register or login request to the server and return the response
char* send_register_or_login(int sockfd, string username, string password, string ip_host, string acces_route) {
    char *message;

    json reg_pack = {
        {"username", username},
        {"password", password}
    };

    char* reg_pack_str = strdup(reg_pack.dump().c_str());
    char* acces_route_str = strdup(acces_route.c_str());
    char* type = strdup(JSON_TYPE);

    message = compute_post_request((char *)ip_host.c_str(), acces_route_str, type,
                                    &reg_pack_str, 1, NULL, 0, NULL);
    send_to_server(sockfd, message);

    free(reg_pack_str);
    free(acces_route_str);
    free(type);
    free(message);
    return receive_from_server(sockfd);
}

// Create a book and send it to the server
char* send_book(int sockfd, string title, string author, string genre, 
                string publisher, int page_count, string ip_host, 
                string acces_route, char* cookie, char* token) {
    char *message;

    json book = {
        {"title", title},
        {"author", author},
        {"genre", genre},
        {"publisher", publisher},
        {"page_count", page_count}
    };

    char* book_str = strdup(book.dump().c_str());
    char* acces_route_str = strdup(acces_route.c_str());
    char* type = strdup(JSON_TYPE);

    message = compute_post_request((char *)ip_host.c_str(), acces_route_str, type,
                                    &book_str, 1, NULL, 0, token);
    send_to_server(sockfd, message);

    free(book_str);
    free(acces_route_str);
    free(type);
    free(message);
    return receive_from_server(sockfd);
}

// Send a get or delete request to the server and return the response
char* send_get(int sockfd, string ip_host, string acces_route, char* cookie, char* token, bool delet) {
    char *message;

    char* acces_route_str = strdup(acces_route.c_str());

    message = compute_get_request((char *)ip_host.c_str(), acces_route_str, NULL, &cookie, 1, token, delet);
    send_to_server(sockfd, message);

    free(message);
    free(acces_route_str);
    return receive_from_server(sockfd);
}

// Print the response from the server or the error message
void print_info(char* response, string mess) {
    char *payload = basic_extract_json_response(response);
    if (payload != NULL) {
        json message = json::parse(payload);
        if (message.contains("error")) {
            cout << ERROR << message["error"] << "\n";
        } else {
            cout << SUCCES << mess << "\n";
        }
    } else {
        cout << SUCCES << mess << "\n";
    }
}

// Get the number from a string
int get_number(string str) {
    size_t i;
    for (i = 0; i < str.size(); i++) {
        if (str[i] < '0' || str[i] > '9')
            return -1;
    }
    return atoi(str.c_str());
}

int main(int argc, char *argv[])
{
    char *response = NULL; char *cookie = NULL;
    char command[BUFLEN]; char *token = NULL;
    int sockfd = 0;
    
    string ip_host = IP_HOST;

    // Get the command from the user and execute it
    // For each commnad, open a connection with the server
    while (true) {
        cin.getline(command, BUFLEN);

        sockfd = open_connection((char *)ip_host.c_str(), PORT, AF_INET, SOCK_STREAM, 0);

        // If we need to register a user, get the username and password 
        // then send the request to the server
        if (strncmp(command, "register", 8) == 0) {
            if (cookie != NULL) {
                cout << ERROR << "You must logout to register a new user!\n";
                continue;
            }
            string username, password;
            cout << "username="; getline(cin, username);
            cout << "password="; getline(cin, password);
            if (username.empty() || password.empty()) {
                cout << ERROR << "All fields must be filled!\n";
                continue;
            } else if (find(username.begin(), username.end(), ' ') != username.end()) {
                cout << ERROR << "Username must not contain spaces!\n";
                continue;
            } else if (find(password.begin(), password.end(), ' ') != password.end()) {
                cout << ERROR << "Password must not contain spaces!\n";
                continue;
            }


            response = send_register_or_login(sockfd, username, password, ip_host, REGISTER_URL);

            print_info(response, username + " successfully registered!");
            token = NULL; cookie = NULL;
        // If we need to login a user, get the username and password
        // then send the request to the server, save the cookie
        } else if (strncmp(command, "login", 5) == 0){
            string username, password;
            cout << "username="; getline(cin, username);
            cout << "password="; getline(cin, password);
            if (username.empty() || password.empty()) {
                cout << ERROR << "All fields must be filled!\n";
                continue;
            } else if (find(username.begin(), username.end(), ' ') != username.end()) {
                cout << ERROR << "Username must not contain spaces!\n";
                continue;
            } else if (find(password.begin(), password.end(), ' ') != password.end()) {
                cout << ERROR << "Password must not contain spaces!\n";
                continue;
            }

            response = send_register_or_login(sockfd, username, password, ip_host, LOGIN_URL);

            print_info(response, username + " successfully logged in!");
            cookie = get_cookie_from_response(response);
            token = NULL;

        // If we need to enter the library, send the request to the server
        // if we are logged in, the save the token
        } else if (strncmp(command, "enter_library", 13) == 0) {
            if (cookie == NULL) {
                cout << ERROR << "You must be logged in to enter the library!\n";
                continue;
            }

            response = send_get(sockfd, ip_host, LIBRARY_ACCESS_URL, cookie, token, false);

            print_info(response, "You have entered the library!");
            token = get_token_from_response(response);
        
        // If we need to get the books from the library, send the request to the server
        // if we are logged in and in the library and have access
        } else if (strncmp(command, "get_books", 9) == 0) {
            if (cookie == NULL) {
                cout << ERROR << "You must be logged in to get the books!\n";
                continue;
            } else if (token == NULL) {
                cout << ERROR << "You must enter the library to get the books!\n";
                continue;
            }
            
            response = send_get(sockfd, ip_host, BOOKS_URL, cookie, token, false);

            char* payload = basic_extract_json_response(response);
            if (payload != NULL)
                cout << payload << "\n";

        // If we need to get a book from the library, get ip and send the ip to the server
        } else if (strncmp(command, "get_book", 8) == 0){
            if (cookie == NULL) {
                cout << ERROR << "You must be logged in to get the books!\n";
                continue;
            } else if (token == NULL) {
                cout << ERROR << "You must enter the library to get the books!\n";
                continue;
            }

            string id;
            cout << "id="; cin >> id;

            string acces_route = BOOKS_URL;
            acces_route = acces_route + "/" + id;

            response = send_get(sockfd, ip_host, acces_route, cookie, token, false);

            char* payload = basic_extract_json_response(response);
            if (payload != NULL)
                print_info(response, payload);
            else
                print_info(response, "Book not found!");
        
        // If we need to add a book to the library, get the book details and send them to the server
        } else if (strncmp(command, "add_book", 8) == 0) {
            string title, author, genre, publisher, page_count;
            cout << "title="; getline(cin, title);
            cout << "author="; getline(cin, author);
            cout << "genre="; getline(cin, genre);
            cout << "publisher="; getline(cin, publisher);
            cout << "page_count="; getline(cin, page_count);

            if (cookie == NULL) {
                cout << ERROR << "You must be logged in to get the books!\n";
                continue;
            } else if (token == NULL) {
                cout << ERROR << "You must enter the library to get the books!\n";
                continue;
            }

            // Check if the fields are filled and if the page count is a number
            int page_count_num = get_number(page_count);
            if (title.empty() || author.empty() || genre.empty() || publisher.empty() || page_count.empty()) {
                cout << ERROR << "All fields must be filled!\n";
                continue;
            } else if (page_count_num == -1) {
                cout << ERROR << "Page count must be a number!\n";
                continue;
            } else if (page_count_num < 0) {
                cout << ERROR << "Page count must be a positive number!\n";
                continue;
            }

            response = send_book(sockfd, title, author, genre, publisher, page_count_num, ip_host, BOOKS_URL, cookie, token);
            print_info(response, title + " successfully added!");

        // If we need to delete a book from the library, get the id and send it to the server
        } else if (strncmp(command, "delete_book", 11) == 0) {
            string id;
            cout << "id="; cin >> id;
            if (cookie == NULL) {
                cout << ERROR << "You must be logged in to get the books!\n";
                continue;
            } else if (token == NULL) {
                cout << ERROR << "You must enter the library to get the books!\n";
                continue;
            }

            string acces_route = BOOKS_URL;
            acces_route = acces_route + "/" + id;

            response = send_get(sockfd, ip_host, acces_route, cookie, token, true);

            char* payload = basic_extract_json_response(response);
            if (payload != NULL)
                print_info(response, "Book not found!");
            else
                print_info(response, "Book successfully deleted!");

        // If we need to logout, send the request to the server and delete the token & cookie
        } else if (strncmp(command, "logout", 6) == 0) {
            if (cookie == NULL) {
                cout << ERROR << "You must be logged in to get the books!\n";
                continue;
            } else if (token == NULL) {
                cout << ERROR << "You must enter the library to get the books!\n";
                continue;
            }

            response = send_get(sockfd, ip_host, LOGOUT_URL, cookie, token, false);

            print_info(response, "Successfully logged out!");
            token = NULL; cookie = NULL;
        
        // If we need to exit, close the connection and exit the program
        } else if (strncmp(command, "exit", 4) == 0) {
            close_connection(sockfd);
            return 0;
        }

        close_connection(sockfd);
    }
    return 0;
}
