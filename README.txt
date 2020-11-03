Full Name: Junyi He
Student ID: 1967776053
What I've done: 
I have implemented the socket programming with the help of beej tutorial. The client and servermain, servermain and serverA/B can communicate through the TCP or UDP. Besides, I implemented the recommendation algorithm based on the requirement. 


File:
	client.cpp: This is the client code file which contains the TCP, helping the client communicate with the Main server. It send the request to server and then receive the recommendation result.
	serverA.cpp: This is the serverA code file contains the UDP part to help the serverA communicate with Main server and send the recommendation to Main server. This file also contains the recommendation algorithm implementation and data input part.
	serverB.cpp: This is the serverB code file, this file is similar to serverA.cpp. 
	servermain.cpp: This is the Main server code file. It likes a bridge between the client and serverA/B. The Main server receive the request from client and send it to corresponding server according to the nation list. Then get the recommendation result from the server and send it back to the client. 

Format of all the messages exchanged:

1. Main server ask the serverA/B for country name list:  "send data".
2. ServerA/B reply the country name list: "nation: <country name1> <country name2>";
3. Client send the query to Main server: "<userId>" "<country name>"
4. Main server send the query to serverA/B "<country name> <userId>"
5. serverA/B reply the recommendation to the Main server: (1) if userId not found: "not found" (2) if no recommendation: "None" (3) has userId to recommend: "<userId>"
6. Main server send the recommendation to the client: (1) if country name not found: "Country Name: Not found". (2) if userId not found: "not found" (3) if no recommendation: "None" (4) if has recommendation: "<userId>"

No idiosyncrasy.

Reused Code:
TCP and UDP socket programming code are from the beej's tutorial. 