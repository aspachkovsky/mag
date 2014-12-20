#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<signal.h>
#include<fcntl.h>

#define MAX_CONN_AVAIBLE 1000
#define BYTES 1024

char *root_dir;
int connected_clients[MAX_CONN_AVAIBLE];



//client connection
void respond(int n) {
    char mesg[99999], *reqline[3], data_to_send[BYTES], path[99999];
    int received, fd, bytes_read;

    memset((void*) mesg, (int) '\0', 99999);
    received = recv(clients[n], mesg, 99999, 0);

    if (received < 0) {     
        fprintf(stderr,("recv() error\n"));
    } else if (received == 0) {    
        fprintf(stderr,"Client disconnected upexpectedly.\n");
    } else {
        printf("%s", mesg);
        reqline[0] = strtok(mesg, " \t\n");
        if ( strncmp(reqline[0], "GET\0", 4) == 0) {
            reqline[1] = strtok(NULL, " \t");
            reqline[2] = strtok(NULL, " \t\n");
            if (strncmp(reqline[2], "HTTP/1.0", 8) != 0 && strncmp(reqline[2], "HTTP/1.1", 8) !=0) {
                write(clients[n], "HTTP/1.0 400 Bad Request\n", 25);
            } else {
                if (strncmp(reqline[1], "/\0", 2) == 0) {
                    reqline[1] = "/index.html";        //Because if no file is specified, index.html will be opened by default
                }
                strcpy(path, root_dir);
                strcpy(&path[strlen(root_dir)], reqline[1]);
                printf("file: %s\n", path);

                if ( (fd=open(path, O_RDONLY))!=-1 ) {   //FILE FOUND
                    send(clients[n], "HTTP/1.0 200 OK\n\n", 17, 0);
                    while ((bytes_read=read(fd, data_to_send, BYTES)) > 0) {
                        write(clients[n], data_to_send, bytes_read);
                    }
                } else {
                   write(clients[n], "HTTP/1.0 404 Not Found\n", 23); //FILE NOT FOUND
                }
            }
        }
    }

    //Closing SOCKET
    shutdown (clients[n], SHUT_RDWR);         //All further send and recieve operations are DISABLED...
    close(clients[n]);
    clients[n]=-1;
}

int main(int argc, char* argv[]) {
    // set default values for root directory and port
    root_dir = getenv("PWD");
    char PORT[6];
    strcpy(PORT, "8080");

    int slot = 0;

    // parse command line arguments (root derectory and port)
    char option;
    while ((option = getopt (argc, argv, "p:r:")) != -1)
        switch (option) {
            case 'r':
                root_dir = malloc(strlen(optarg));
                strcpy(root_dir,optarg);
                break;
            case 'p':
                strcpy(PORT,optarg);
                break;
            case '?':
                fprintf(stderr,"Wrong arguments given!!!\n");
                exit(1);
            default:
                exit(1);
        }
    
    printf("Server started at port no. %s%s%s with root directory as %s%s%s\n","\033[92m",PORT,"\033[0m","\033[92m", root_dir, "\033[0m");

    int i;
    for (i = 0; i < MAX_CONN_AVAIBLE; i++) {
        clients[i] = -1;
    }
    
    struct addrinfo hints, *res, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    // recieve avaible addrinfo structures for the loopback node (NULL)
    int result = getaddrinfo(NULL, port, &hints, &res);
    if (result != 0) {
        perror("Error: getaddrinfo() error");
        exit(1);
    }

    int socketd; // socket descriptor
    // resolve and bind a socket
    for (p = res; p != NULL; p = p->ai_next) {
        socketd = socket(p->ai_family, p->ai_socktype, 0);
        if (socketd == -1) {
            continue;
        }
        result = bind(socketd, p->ai_addr, p->ai_addrlen)
        if (result == 0) { 
            break;
        }
    }

    if (p == NULL) {
        perror ("socket() or bind()");
        exit(1);
    }

    // an appropriate socket found, so free addrinfo struct 
    freeaddrinfo(res);

    // listen for incoming connections
    result = listen(socketd, 1000000);
    if (result != 0) {
        perror("Error: listen for incomming connection failed.");
        exit(1);
    }

    struct sockaddr_in clientaddr;
    while (1) {
        // accepting client connection
        socklen_t addrlen = sizeof(clientaddr);
        clients[slot] = accept(socketd, (struct sockaddr *) &clientaddr, &addrlen);

        if (clients[slot] < 0) {
            error ("Error: failed to accept a connection.");
        } else {
            // fork a child process to respond on the request
            if (fork() == 0) {
                respond(slot);
                exit(0);
            }
        }

        while (clients[slot] != -1) { 
            slot = (slot + 1) % MAX_CONN_AVAIBLE;
        }
    }

    return 0;
}

