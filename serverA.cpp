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

#define MYPORT "21859"   // temp port number for ServerA
#define HOST "localhost"
using namespace std;

unordered_map<string ,unordered_map<int,unordered_set<int> > > graph;
struct cmp{
    bool operator() (pair<int,int> a, pair<int,int> b ){//默认是less函数
        //返回true时，a的优先级低于b的优先级（a排在b的后面）

        if( a.second== b.second ) return a.first > b.first;
        return b.second > a.second;
    }
};
void print_small_map(unordered_map<int,unordered_set<int>> const &x)
{
    for (auto const& pair: x) {
        cout << "{" << pair.first << ":";
        for(auto const& k : pair.second)
            std::cout << k << ",";
        cout << "}" << endl;
    }

}
string algo(int userId, string nation){
        string non = "None";
        cout << "The userId is " << userId << " and the nation is " << nation << endl;
        unordered_map<int,unordered_set<int>> cur = graph[nation];
        print_small_map(cur);
        if(cur.find(userId) == cur.end()) {
            printf("cur.find(userId) == cur.end()\n");

            return non;
        }
        unordered_set<int> curChildren = cur[userId];
        if(curChildren.size() == cur.size()-1) {
            printf("curChildren.size() == cur.size()-1\n");
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
            pq.push({k,children.size()});
            notConnectedVSCommon.insert({k,cnt});
            if(max < cnt){
                max = cnt;
            }
        }

        if(max == 0){
            pair<int,int> x = pq.top();
            printf("max == 0, the result is %d", x.second);
            return to_string(x.second);
        }
        for(auto const&k : notConnectedVSCommon){
            if(k.second == max) {
                printf("max != 0, the result is %d", k.first);
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
    
     ifstream infile("/home/student/Documents/homework/testcases/testcase2/data1.txt");

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
          
     }
     print_map(graph);
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
    printf("The server A is up and running using UDP on port %s. \n",MYPORT);
    while(1){
        addr_len = sizeof their_addr;
        char recvFromServerMain[1024];
        recvfrom(sockfd, recvFromServerMain, sizeof recvFromServerMain,0,(struct sockaddr *)&their_addr,&addr_len);
        cout << "The server A has received userId from nation " << recvFromServerMain<< endl;
        string str_total(recvFromServerMain);
        if(str_total.compare("send data")){
            string nationSet = "nation: ";
            for(auto const& m : graph){
                nationSet = nationSet+" "+m.first;
            }
            char tt[1024];
            strncpy(tt,nationSet.c_str(),nationSet.length());
            tt[nationSet.length()] = '\0';
            cout << "The nationset: " << nationSet << endl;
            sendto(sockfd,tt,sizeof tt, 0, (struct sockaddr *)&their_addr,addr_len);


        }else{
            istringstream spliter(str_total);
            string userId;
            string nation;
            spliter >> nation;
            spliter >> userId;
            string result;
            char tt[1024];
            result = algo(stoi(userId), nation);
            strncpy(tt,result.c_str(),result.length());
            tt[result.length()] = '\0';
            cout << "The Server A has get the recommendation " << result << endl;
            cout << "The Server A has get the recommendation tt: " << tt << endl;
            sendto(sockfd,tt,sizeof tt,0,(struct sockaddr *)&their_addr, addr_len);
            printf("The Server A has finished sending the recommendations to MainServer.\n");
        }


    }
}