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
#include <iostream>
#include <vector>
#include <fstream>
#include <regex>
#include "unordered_map"
#include "unordered_set"

#define MYPORT "21859"   // temp port number for ServerA
#define HOST "localhost"
using namespace std;

unordered_map<string ,unordered_map<int,unordered_set<int>>> graph;
void algo(int userId, char* nation){

}

void print_map(std::unordered_map<string,unordered_map<int,unordered_set<int>>> const &x)
{
    for(auto const& m : x){
        cout << "nation: " << m.first<<endl;
        for (auto const& pair: m.second) {
            cout << "{" << pair.first << ":";
            for(auto const& k : pair.second)
                std::cout << k << ",";
            cout << "}" << endl;
        }
    }

}

void read_file() {

    ifstream infile("/Users/juny1/Downloads/test.txt");

    string line;
    string nation;
    string cur;
    string delimiter = " ";
    while(getline(infile,line)){

        if(line[0] >= '0' && line[0] <= '9'){
            size_t pos = line.find(delimiter);
            cur = line.substr(0,pos);
            int temp;

            while((pos = line.find(delimiter)) != string :: npos){
                cur = line.substr(0,pos);
                if(cur.length() != 0){
                    temp = stoi(cur, nullptr);
                    graph[nation].insert({temp,{}});
                    int num = stoi(cur, nullptr);
                    cout << "test: " << num << " ";
                    break;
                }

                line.erase(0,pos+delimiter.length());
            }
            string token;
            line.erase(0,pos+delimiter.length());
            while((pos = line.find(delimiter)) != string :: npos){
                token = line.substr(0,pos);
                if(token.length() != 0){

                    int num = stoi(token, nullptr);
                    graph[nation][temp].insert(num);
                    cout << "test: " << num<< " ";
                }

                line.erase(0,pos+delimiter.length());
            }
            if(line.length() != 0){

                int num = stoi(line, nullptr);
                graph[nation][temp].insert(num);
                cout << "test: " << num;
            }
//        cout << line << endl;
            cout << endl;
        }else{
            unordered_map<int,unordered_set<int>> map;
            graph.insert({line, map});
            nation = line;
            cout << "The nation is " << nation << endl;
        }
        print_map(graph);
    }
}


int main(void){

//    set up UDP -- From Beej

    read_file();
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
        char nation[30];
        int userId;
        recvfrom(sockfd, (char *)&userId, sizeof userId,0,(struct sockaddr *)&their_addr,&addr_len);
        recvfrom(sockfd,nation,sizeof nation,0,(struct sockaddr *)&their_addr,&addr_len);
        printf("The server A has received userId %d from nation %s \n", userId, nation);
        int result;
        algo(userId, nation);
        printf("The Server A has get the recommendation %d \n",result);
        sendto(sockfd,(char *)&result,sizeof result,0,(struct sockaddr *)&their_addr, addr_len);
        printf("The Server A has finished sending the recommendations to MainServer.\n");
    }
}