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
using namespace std;

#define AWSPORT "25859"   //aws TCP port
#define HOST "localhost"

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*) sa)->sin6_addr);
}
void readFile(){
    printf("readfile...\n");
}
int main(void){
    while(1){
    //  set up TCP --from Beej;
        string inputId;
        char nation[30];
        printf("input your query ID\n");
        scanf("%s",(char *)&inputId);
        printf("query Id is %d\n",(char *)&inputId);

        printf("input your nation\n");
        scanf("%s",nation);
        printf("query Id is %s\n",nation);
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


        readFile();

        send(sockfd, nation, sizeof nation, 0);
        send(sockfd,(char *)&inputId, sizeof inputId,0);
        printf("The client sent the nation %s, userId %d to servermain\n",nation,(char *)&inputId);
        int result = 0;
        recv(sockfd,(char *)&result, sizeof result,0);
        printf("The client received recommendation %d from servermain\n",result);
    }

}
