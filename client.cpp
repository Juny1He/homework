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

#define AWSPORT "33053"   //aws TCP port
#define HOST "127.0.0.1"

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

int main(void){
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
        printf("The client is up and running\n");

        char userId[1024];
        char nation[1024];
        printf("input your query ID\n");
        scanf("%s", userId);
        printf("query Id is %s\n", userId);

        printf("input your nation\n");
        scanf("%s",nation);
        printf("query Id is %s\n",nation);

        send(sockfd, nation, sizeof nation, 0);
        send(sockfd, userId, sizeof userId, 0);
        printf("The client sent the nation %s, userId %s to servermain\n", nation, userId);
        char result[1024];
        recv(sockfd,result, sizeof result,0);
        string rr(result);
        if(rr.compare("Country Name: Not found") == 0){
            cout << "Country Name: Not found" << endl;
        }else{
            cout << "The client received recommendation --" << result << "-- from servermain" << endl;
        }

        close(sockfd);
    }

}
