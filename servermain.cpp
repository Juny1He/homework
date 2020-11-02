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
#include <unordered_set>
#include <sstream>
#define TCPPORT "33053"      //TCP port
#define UDPPORT 32053		//UDP port
#define HOST "127.0.0.1"
#define BACKLOG 10 // how many pending connections queue will hold
#define PORTA "30053"
#define PORTB "31053"



using namespace std;

unordered_set<string> nationA;
unordered_set<string> nationB;
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void fillSet(unordered_set<string>  &x, string set){
    istringstream spliter(set);
    string s;
    spliter >> s;
    cout << s << " ";
    while(spliter >> s){
        x.insert(s);
    }
    for(auto const&k : x){
        cout << k << " " ;
    }
    cout << endl;
}
void askForList(char ch){
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
    string askData= "send data";
    char sendToServerA[1024];
    strncpy(sendToServerA,askData.c_str(),askData.length());
    sendToServerA[askData.length()] = '\0';
    sendto(mysock,sendToServerA, sizeof sendToServerA, 0, p->ai_addr,p->ai_addrlen);


    char result[1024];
    recvfrom(mysock, result, sizeof result, 0, NULL,NULL);
//    printf("The recommendation result in function %s.\n",result);
    string nationSet(result);
    if(ch == 'A'){
        cout << "The Main server has received the country list from server A using UDP over port "
             <<  UDPPORT << endl;
        fillSet(nationA,nationSet);
        cout << "Server A" << endl;
        for(auto const&k : nationA){
            cout << k << endl;
        }
    }else{
        cout << "The Main server has received the country list from server B using UDP over port "
             <<  UDPPORT << endl;
        fillSet(nationB,nationSet);
        cout << "Server B" << endl;
        for(auto const&k : nationB){
            cout << k << endl;
        }
    }
}

string udpFunc(char* userId, char* nation){
    int mysock;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char* backserver_port;
    string cur(nation);
    char ch = 'A';
    if(nationA.find(cur) != nationA.end()){
        backserver_port = PORTA;
        cout << nation << " shows up in server A/B" << endl;
        cout << "The Main Server has sent request from User" << userId
             << " to server A/B using UDP over port"
             << UDPPORT << endl;
    }else if(nationB.find(cur) != nationB.end()){
        ch = 'B';
        backserver_port = PORTB;
        cout << nation << " shows up in server A/B" << endl;
        cout << "The Main Server has sent request from User" << userId
             << " to server A/B using UDP over port"
             << UDPPORT << endl;
    }else{
        cout << nation << " does not show up in server A&B" << endl;
        cout << "The Main Server has sent “Country Name: Not found” to the client using TCP over port "
             << TCPPORT << endl;
        return "Country Name: Not found";
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
    sendToServerA[temp_nation.length()+temp_userId.length()+1] = '\0';
    printf("Before: The servermain sent userId and nation %s to server %c.\n",sendToServerA,ch);
    sendto(mysock,sendToServerA, sizeof sendToServerA, 0, p->ai_addr,p->ai_addrlen);
    printf("After: The servermain sent userId and nation %s to server %c.\n",sendToServerA,ch);


    char result[1024];
    recvfrom(mysock, result, sizeof result, 0, NULL,NULL);
    printf("The recommendation result in function %s.\n",result);
    string temp(result);
    if(temp.compare("not found") == 0){
        cout << "The Main server has received “User ID: Not found” from server " << ch << endl;
        cout << "The Main Server has sent error to client using TCP over " << TCPPORT << endl;
    }else{
        cout << "The Main server has received searching result(s) of User " << userId
             << " from server" << ch << endl;
        cout << "The Main Server has sent searching result(s) to client using TCP over port "
             << TCPPORT << endl;
    }

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
        if (::bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
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
    
    printf( "The Main server is up and running. \n");
    askForList('A');
    askForList('B');

    int udp_sockfd = socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in udp_addr;
//    memset(&udp_addr,0,sizeof(udp_addr));
//    udp_addr.sin_family = AF_INET;
//    udp_addr.sin_addr.s_addr = inet_addr(HOST);
//    udp_addr.sin_port = htons(UDPPORT);
//    bind(udp_sockfd,(struct sockaddr *) &udp_addr,sizeof(udp_addr));
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
        if(!fork()){// this is the child process
//            close(sockfd);
            char userId[1024];
            char nation[1024];
            recv(new_fd, nation,sizeof nation, 0);
            recv(new_fd, userId,sizeof userId,0);
//            printf("The servermain received the nation %s, userId %s from client\n",nation,userId);
            cout << "The Main server has received the request on User " << userId << " in "
            << nation << "from the client using TCP over port" << TCPPORT << endl;
            string recUser = udpFunc(userId, nation);
            char tt[1024];
            strncpy(tt,recUser.c_str(),recUser.length());
            tt[recUser.length()] = '\0';
            send(new_fd,tt, sizeof tt,0);

        }

        close(new_fd);
    }
}
