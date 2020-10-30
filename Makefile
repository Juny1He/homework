all:
		g++  -o servermain servermain.cpp
		g++ -std=c++11 -o serverA serverA.cpp
		g++  -o client client.cpp


$(phony serverA): 
		./serverA
servermain:
		./servermain
client:
		./client
