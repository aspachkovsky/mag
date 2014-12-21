#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define REQUEST_MSG_SIZE 512
#define RESPONSE_MSG_SIZE 8192

const char* SERVER_DEFAULT_PORT = "80";
const char* SERVER_DEFAULT_ADDRESS = "localhost";
const char* HTTP_SEP = "\r\n\r\n";

void usage() {
    printf("Simple web client. Task 3.\n");
    printf("Usage: wclient path [-h] [-a host] [-p port]\n");
    printf(" path       Path to the requsted file.\n"); 
    printf("            Should start with '/'. Positional argument.\n");    
    printf(" -a host    Target host, localhost by default.\n");    
    printf(" -p port    Target port, 80 by default.\n");    
    printf(" -h         Prints this message.\n");    
}

int main(int argc, char *argv[]) {
    
    char address[255];
    strcpy(address, SERVER_DEFAULT_ADDRESS);    

    char port[6];
    strcpy(port, SERVER_DEFAULT_PORT);

    char option;
    while ((option = getopt(argc, argv, "ha:p:")) != -1)
        switch (option) {
            case 'a':
                strcpy(address, optarg);
                break;
            case 'p':
                strcpy(port, optarg);
                break;
            case 'h':
                usage();
                exit(0);
            case '?':
                perror("Client error: invalid argument");
                exit(1);
            default:
                exit(1);
        }

    if (optind >= argc) {
        printf("Client error: positional path argument is not specified\n");
        exit(1);
    }
    
    char* path = (char*) malloc(strlen(argv[optind]) + 1);
    strncpy(path, argv[optind], strlen(argv[optind]));

    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(atoi(port));

    int socketd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketd == -1) {
        perror("Client error: failed to create socket");
        return 1;
    }

    in_addr_t inet_address = inet_addr(address);
    if (inet_address == -1) { //not ip
        struct hostent *he = gethostbyname(address);        
        if (he == NULL) {
            perror("Client error: failed to resolve the host name");
            exit(1);
        }
         
        struct in_addr **addr_list = (struct in_addr **) he->h_addr_list;
        for(int i = 0; addr_list[i] != NULL; i++) {
            server_sockaddr.sin_addr = *addr_list[i];  
            break;
        }

        printf("%s resolved to %s\n", address, inet_ntoa(server_sockaddr.sin_addr));
    } else {
        server_sockaddr.sin_addr.s_addr = inet_address;
    }

         
    if (connect(socketd, (struct sockaddr *) &server_sockaddr, sizeof(server_sockaddr)) < 0) {
        perror("Client error: failed to connect ot the host");
        exit(1);
    }

    char request[REQUEST_MSG_SIZE];
    memset((void*) request, (int) '\0', REQUEST_MSG_SIZE);
    sprintf(request, "GET %s HTTP/1.1\r\n\r\n", path);
    if (send(socketd, request, strlen(request), 0) < 0) {
        perror("Client error: failed to send a request to host");
        exit(1);
    }

    char *response = (char*) malloc(RESPONSE_MSG_SIZE);
    char *p = response;
    int bytes_received = 0, b = 0;

    while ((b = recv(socketd, p, RESPONSE_MSG_SIZE, 0)) > 0) {
        bytes_received += b;
        p += b;
        printf("portion %d\n", b);
    }

    if (b < 0) {
        perror("Client error: failed to receive a response from host");
        exit(1);
    }

    printf("Bytes received: %d\n", bytes_received);            

    char* sep = strstr(response, HTTP_SEP);
    char* content = 0;
    
    if(sep != 0) {
        int content_size = bytes_received - (sep - response) - strlen(HTTP_SEP);
        content = (char*) malloc(content_size + 1);
        memcpy(content, sep + strlen(HTTP_SEP), content_size);
        content[content_size] = 0;
        printf("%s\n", content);
        free(content);
    }

    free(response);
    free(path);
 
	return 0;
}
