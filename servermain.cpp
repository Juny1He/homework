#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <string>
#include <iostream>

#define TCPPORT "25859"   //TCP port
#define UDPPORT "24859"		//UDP port
#define HOST "localhost"
#define BACKLOG 10 // how many pending connections queue will hold
#define PORTA "21859"
#define PORTB "22859"
#define PORTC "23859"
using namespace std;

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

string udpFunc( char ch, char* userId, char* nation){
    int mysock;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char* backserver_port;

    if(ch == 'A'){
        backserver_port = PORTA;
    }else if(ch == 'B'){
        backserver_port = PORTB;
    }
//    set up UDP -- from Beej;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if((rv = getaddrinfo(HOST,backserver_port,&hints,&servinfo)) != 0){
        fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }
//    loop through all the results and make a socket ---- Beej
    for(p = servinfo; p!= NULL; p = p->ai_next){
        if((mysock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            perror("talker: socket");
            continue;
        }
        break;
    }

    if(p == NULL){
        fprintf(stderr, "talker: failed to bind socket\n");
        exit(2);
    }

//    using UDP to send data;
    string temp_userId(userId);
    string temp_nation(nation);
    string temp_sendToServerA = temp_nation+" "+temp_userId;
    char sendToServerA[1024];
    strncpy(sendToServerA,temp_sendToServerA.c_str(),temp_sendToServerA.length());
    sendto(mysock,sendToServerA, sizeof sendToServerA, 0, p->ai_addr,p->ai_addrlen);
    printf("After: The servermain sent userId and nation %s to server %c.\n",ch);


    char result[1024];
    recvfrom(mysock, result, sizeof result, 0, NULL,NULL);
    printf("The recommendation result in function %s.\n",result);
    string temp(result);
    return temp;

}

int main(){
    //    set up TCP -- from Beej
    int sockfd, new_fd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    if ((rv = getaddrinfo(HOST, TCPPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
            == -1) {
            perror("server: socket");
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))
            == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }
    freeaddrinfo(servinfo); // all done with this structure

    //listen
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    
    printf( "The AWS is up and running. \n");

    while(1){
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            exit(1);
        }
        // get the port of client
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *) &their_addr), s, sizeof s);
        struct sockaddr_in addrTheir;
        memset(&addrTheir, 0, sizeof(addrTheir));
        int len = sizeof(addrTheir);
        getpeername(new_fd, (struct sockaddr *) &addrTheir, (socklen_t *) &len);
        int client_port = addrTheir.sin_port;



//        char ch;
        char ch = 'A';
        char userId[1024];
        char nation[1024];
        recv(new_fd, nation,sizeof nation, 0);
        recv(new_fd, userId,sizeof userId,0);
        printf("The servermain received the nation %s, userId %s from client\n",nation,userId);
        string recUser = udpFunc(ch,userId, nation);
        char tt[1024];
        strncpy(tt,recUser.c_str(),recUser.length());
        send(new_fd,tt, sizeof tt,0);
        cout << "The recommended user is " << recUser << endl;
        close(new_fd);
    }
}
