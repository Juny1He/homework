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

#define MYPORT "21859"   // temp port number for ServerA
#define HOST "localhost"

void algo(){

}
void readFile(){

}

int main(void){

//    set up UDP -- From Beej

    readFile();
    int sockfd;
    int rv;
    struct addrinfo hints;
    struct addrinfo *servinfo;
    struct sockaddr_storage their_addr;
    struct addrinfo *p;
    socklen_t addr_len;
    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if((rv = getaddrinfo(HOST,MYPORT, &hints,&servinfo)) != 0){
        fprintf(stderr,"DBG: getaddrinfo: %s\n", gai_strerror(rv));
        return 0;
    }
    for(p = servinfo; p != NULL; p = p->ai_next){
        if((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1){
            perror("serverA: socket");
            continue;
        }
        if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            close(sockfd);
            perror("serverA: bind");
            continue;
        }
        break;
    }
    if(p == NULL){
        fprintf(stderr, "serverA: failed to bind socket\n");
        return 0;
    }
    freeaddrinfo(servinfo);
    printf("The server A is up and running using UDP on port %s. \n",MYPORT);
    while(1){
        addr_len = sizeof their_addr;
        int userId;
        recvfrom(sockfd, (char *)&userId, sizeof userId,0,(struct sockaddr *)&their_addr,&addr_len);
        printf("The server A has received userId %d. \n", userId);
        int result;
        algo();
        printf("The Server A has get the recommendation %d \n",result);
        sendto(sockfd,(char *)&result,sizeof result,0,(struct sockaddr *)&their_addr, addr_len);
        printf("The Server A has finished sending the recommendations to MainServer.\n");
    }
}