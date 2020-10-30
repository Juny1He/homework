all:
		gcc  -o servermain servermain.cpp 
		gcc -std=c++11 -o serverA serverA.cpp 
		gcc  -o client client.cpp 


$(phony serverA): 
		./serverA
servermain:
		./servermain
client:
		./client
