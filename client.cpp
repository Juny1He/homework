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
#include <ctype.h>
#include <vector>
#include <string>
#include <iostream>
using namespace std;

#define AWSPORT "33053"
#define HOST "127.0.0.1"


void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

int main(void){
    cout << "The client is up and running." << endl;
    while(1){
    //  set up TCP --from Beej;

        int sockfd = 0;
        struct addrinfo hints, *servinfo, *p;
        int rv;
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        if ((rv = getaddrinfo(HOST, AWSPORT, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return 1;
        }
        // loop through all the results and connect to the first we can----Beej
        for (p = servinfo; p != NULL; p = p->ai_next) {
            if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
                == -1) {
                perror("client: socket");
                continue;
            }

            if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                close(sockfd);
                perror("client: connect");
                continue;
            }
            break;
        }
        if(p == NULL){
            fprintf(stderr,"client: failed to connect. \n");
            exit(0);
        }

        freeaddrinfo(servinfo);
        char userId[1024];
        char nation[1024];
        cout << "Please enter the User ID: ";
        scanf("%s", userId);
        cout << "Please enter the Country Name: ";
        scanf("%s",nation);
        string test_userId(userId);
        if(test_userId.compare("") == 0){
            continue;
        }
        string test_nation(nation);
        if(test_nation.compare("") == 0){
            continue;
        }

        send(sockfd, nation, sizeof nation, 0);
        send(sockfd, userId, sizeof userId, 0);
        cout << "The client has sent User" << userId << " and " << nation
        << " to Main Server using TCP." << endl;

        char result[1024];
        recv(sockfd,result, sizeof result,0);
        string rr(result);
        if(rr.compare("Country Name: Not found") == 0){
            cout << nation << " not found" << endl;
        }else if(rr.compare("not found") == 0){
            cout << "User" << userId << " not found" << endl;
        }else{
            cout << "The client has received results from Main Server: User" << rr
            << " is/are possible friend(s) of User" << userId << " in " << nation << endl;
        }

        close(sockfd);
    }

}
