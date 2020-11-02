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
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <map>
#include <queue>
#include <sstream>
//#include <priority_queue>

#define MYPORT "31053"   // temp port number for ServerB
#define HOST "localhost"
using namespace std;

unordered_map<string ,unordered_map<int,unordered_set<int> > > graph;
struct cmp{
    bool operator() (pair<int,int> a, pair<int,int> b ){
        if( a.second== b.second ) return a.first > b.first;
        return a.second < b.second;
    }
};
string algo(int userId, string nation){
    string non = "None";
//    cout << "The userId is " << userId << " and the nation is " << nation << endl;
    unordered_map<int,unordered_set<int>> cur = graph[nation];
    if(cur.find(userId) == cur.end()) {
        printf("cur.find(userId) == cur.end()\n");

        return "not found";
    }
    unordered_set<int> curChildren = cur[userId];
    if(curChildren.size() == cur.size()-1) {
//        printf("curChildren.size() == cur.size()-1\n");
        return non;
    }
    unordered_set<int> notConnected;
    for(auto const&k:cur){
        if(k.first == userId) continue;
        if(curChildren.find(k.first) != curChildren.end()) continue;
        notConnected.insert(k.first);
    }
    map<int,int> notConnectedVSCommon;
    int max = 0;
    priority_queue< pair<int,int>, vector<pair<int,int>>,cmp> pq;
    for(auto const&k : notConnected){
        int cnt = 0;
        unordered_set<int> children = cur[k];
        for(auto const&c: children){
            if(curChildren.find(c) != curChildren.end()){
                cnt++;
            }
        }
//        cout << "K: " << k << " children.size(): " << children.size() << endl;
        pq.push({k,children.size()});
        notConnectedVSCommon.insert({k,cnt});
        if(max < cnt){
            max = cnt;
        }
    }

    if(max == 0){
        pair<int,int> x = pq.top();
//        printf("max == 0, the result is %d", x.first);
        while(!pq.empty()){
            pair<int,int> cur = pq.top();
//            cout << "{" << cur.first << "," << cur.second << "}" << endl;
            pq.pop();
        }
        return to_string(x.first);
    }
    for(auto const&k : notConnectedVSCommon){
        if(k.second == max) {
//            printf("max != 0, the result is %d", k.first);
            return to_string(k.first);
        }
    }
    return non;
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

    ifstream infile("/home/student/Documents/homework/testcases/testcase4/data2.txt");

    string line;
    string nation;
    string cur;
    string delimiter = " ";

    while(getline(infile,line)){
        if(line[0] >= '0' && line[0] <= '9'){
            istringstream spliter(line);
            string cur;
            spliter >> cur;
//            cout << "father: " << cur << " ";
            int temp = stoi(cur);
            graph[nation].insert({temp,{}});
            while(spliter >> cur){
//                cout << "son: " << cur << " ";
                graph[nation][temp].insert(stoi(cur));
            }
            cout << endl;

        }else{
            unordered_map<int,unordered_set<int>> map;
            graph.insert({line, map});
            nation = line;
            cout << "The nation is " << nation << endl;
        }

    }
//    print_map(graph);
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
        if (::bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
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
    printf("The server B is up and running using UDP on port %s. \n",MYPORT);
    while(1){
        addr_len = sizeof their_addr;
        char recvFromServerMain[1024];
        recvfrom(sockfd, recvFromServerMain, sizeof recvFromServerMain,0,(struct sockaddr *)&their_addr,&addr_len);
        string str_total(recvFromServerMain);
        if(str_total.compare("send data") == 0){
            string nationSet = "nation: ";
            for(auto const& m : graph){
                nationSet = nationSet+" "+m.first;
            }
            char tt[1024];
            strncpy(tt,nationSet.c_str(),nationSet.length());
            tt[nationSet.length()] = '\0';

            sendto(sockfd,tt,sizeof tt, 0, (struct sockaddr *)&their_addr,addr_len);
            cout << "The server B has sent a country list to Main Server" << endl;

        }else{
            istringstream spliter(str_total);
            string userId;
            string nation;
            spliter >> nation;
            spliter >> userId;
            string result;
            cout << "The server B has received request for finding possible friends of User" << userId <<" in " << nation << endl;
            char tt[1024];
            result = algo(stoi(userId), nation);
            if(result.compare("not found") == 0){
                cout << "User" <<userId<<"does not show up in " << nation << endl;
                cout << "The server B has sent \"User" << userId << "not found \" to Main Server";
            }else{
                cout << "The server B is searching possible friends for User" << userId << "..." << endl;
                cout << "Here are the results: User" << result << endl;
            }
            strncpy(tt,result.c_str(),result.length());
            tt[result.length()] = '\0';
            sendto(sockfd,tt,sizeof tt,0,(struct sockaddr *)&their_addr, addr_len);
            cout << "The server B has sent the result(s) to Main Server";
        }


    }
}