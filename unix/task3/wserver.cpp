#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>

#define LISTEN_BACKLOG 10
#define RECV_MSG_LENGTH 2048
#define SEND_MSG_PORTION_LENGTH 2048

const char* HTTP_RESP_STATUS_200 = "HTTP/1.0 200 OK\r\n";
const char* HTTP_RESP_STATUS_400 = "HTTP/1.0 400 Bad Request\r\n";
const char* HTTP_RESP_STATUS_404 = "HTTP/1.0 404 Not Found\r\n";
const char* NOT_FOUND_MSG = "The resource you requested is not found.\r\n";

const char* HTTP_METHOD_GET = "GET";
const char* HTTP_VERSION = "HTTP/1.1";
const char* HTTP_REQLINE_SP = " \t\n";

const char* SERVER_DEFAULT_PORT = "8080";

struct consumer_arguments {
    int client_socketd;
    char* root_dir;
};

void *consume_request(void*);

void usage() {
    printf("Simple web server. Task 3.\n");
    printf("Usage: wserver [-h] [-r rootdir] [-p port]\n");
    printf(" -r rootdir Server fs root, current working directory by default.\n");    
    printf(" -p port    Server port, 8080 by default.\n");    
    printf(" -h         Prints this message.\n");    
}

int main(int argc, char* argv[]) {
    char* root_dir = getenv("PWD");
    char port[6];
    strcpy(port, SERVER_DEFAULT_PORT);

    char option;
    while ((option = getopt(argc, argv, "hp:r:")) != -1)
        switch (option) {
            case 'r':
                root_dir = (char*) malloc(strlen(optarg));
                strcpy(root_dir, optarg);
                break;
            case 'p':
                strcpy(port, optarg);
                break;
            case 'h':
                usage();
                exit(0);
            case '?':
                perror("Client: invalid argument");
                exit(1);
            default:
                exit(1);
        }
    
    printf("Server: started at port %s with root directory %s\n", port, root_dir);
   
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0; 
    
    struct addrinfo *res;
    if (getaddrinfo(NULL, port, &hints, &res) != 0) {
        perror("Server error: getaddrinfo() error");
        exit(1);
    }

    int server_socketd; 
    struct addrinfo *rp;
    for (rp = res; rp != NULL; rp = rp->ai_next) {
        server_socketd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        if (server_socketd == -1) {
            continue;
        }

        if (bind(server_socketd, rp->ai_addr, rp->ai_addrlen) == 0) { 
            break;
        }

        close(server_socketd);
    }

    if (rp == NULL) {
        perror("Server error: socket or bind call failed");
        exit(1);
    }

    freeaddrinfo(res);

    if (listen(server_socketd, LISTEN_BACKLOG) != 0) {
        perror("Server error: listen for incomming connection failed");
        exit(1);
    }
    
    while (1) {
        struct sockaddr_in clientaddr;
        socklen_t addrlen = sizeof(clientaddr);
        int client_socketd = accept(server_socketd, (struct sockaddr*) &clientaddr, &addrlen);

        if (client_socketd < 0) {
            perror("Server error: failed to accept a client connection");
        } else {
            printf("Server: got a connection from %s on port %d\n", 
                inet_ntoa(clientaddr.sin_addr), htons(clientaddr.sin_port));
            
            struct consumer_arguments *arguments = (struct consumer_arguments*) 
                malloc(sizeof(struct consumer_arguments));
            arguments->client_socketd = client_socketd;
            arguments->root_dir = (char*) malloc(strlen(root_dir) + 1);
            strcpy(arguments->root_dir, root_dir);

            pthread_t consuming_thread;
            if (pthread_create(&consuming_thread, NULL, consume_request, (void*) arguments) != 0) {
                perror("Server error: failed to create a thread\n");
            }    
        }
    }

    return 0;
}

void *consume_request(void* resp_arguments) {
    struct consumer_arguments *arguments = (struct consumer_arguments*) resp_arguments;
    char recv_msg[RECV_MSG_LENGTH];
    memset((void*) recv_msg, (int) '\0', RECV_MSG_LENGTH);
    int received = recv(arguments->client_socketd, recv_msg, RECV_MSG_LENGTH, 0);

    if (received > 0) {
        printf("Consumer: received request:\n%s", recv_msg);      
        char* reqline_method = strtok(recv_msg, HTTP_REQLINE_SP);
        if (strncmp(reqline_method, HTTP_METHOD_GET, strlen(HTTP_METHOD_GET)) == 0) {

            char* reqline_uri = strtok(0, HTTP_REQLINE_SP);
            char* reqline_http_version = strtok(0, HTTP_REQLINE_SP);

            if (strncmp(reqline_http_version, HTTP_VERSION, strlen(HTTP_VERSION)) == 0) {
                char path[RECV_MSG_LENGTH];
                strcpy(path, arguments->root_dir);
                strcat(path, reqline_uri);
                printf("Consumer: requested file: %s\n", path);
		
                int fd = -1;
                struct stat filestat;
                if (access(path, R_OK) != -1) {                     
                    if (stat(path, &filestat) == 0) {
                        if (S_ISREG(filestat.st_mode)) {
                            fd = open(path, O_RDONLY);
                        } else {
                            printf("Consumer: requested resource is not regular file: %s, sending 404\n", path);
                        }                           
                    } else {
                        perror("Consumer: stat() call failed for the requested file");
                    }
                } else {
                     printf("Consumer: requested resource not found: %s, sending 404\n", path);
                }                

                if (fd != -1) {
                    printf("Consumer: transmitting requested file content: %s, sending 200\n", path);
                    int bytes_sent = 0;                    
                    bytes_sent += write(arguments->client_socketd, HTTP_RESP_STATUS_200, strlen(HTTP_RESP_STATUS_200));

                    off_t filesize = filestat.st_size;
                    char content_length_header[40];
                    
                    sprintf(content_length_header, "Content-Lenght: %ld\r\n\r\n", filestat.st_size);                   
		            bytes_sent += write(arguments->client_socketd, content_length_header, strlen(content_length_header));

                    char send_msg[SEND_MSG_PORTION_LENGTH];
                    int bytes_read;
                    while ((bytes_read = read(fd, send_msg, SEND_MSG_PORTION_LENGTH)) > 0) {
                        bytes_sent += write(arguments->client_socketd, send_msg, bytes_read);
                    }
                    close(fd);
                    printf("Bytes sent: %d\n", bytes_sent);
                } else {
                    write(arguments->client_socketd, HTTP_RESP_STATUS_404, strlen(HTTP_RESP_STATUS_404));
                    write(arguments->client_socketd, "\r\n", 2);
                    write(arguments->client_socketd, NOT_FOUND_MSG, strlen(NOT_FOUND_MSG));
                }
            } else {
                write(arguments->client_socketd, HTTP_RESP_STATUS_400, strlen(HTTP_RESP_STATUS_400));
                printf("Consumer: wrong HTTP version: %s, sending 400\n", reqline_http_version);
            }
        }
    } else {
        perror("Consumer error: failed to receive request message");
    }

    shutdown(arguments->client_socketd, SHUT_WR);
    close(arguments->client_socketd);

    free(arguments->root_dir);
    free(arguments);
}

