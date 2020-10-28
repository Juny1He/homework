all:
		gcc -o servermain servermain.cpp
		gcc -o serverA serverA.cpp
		gcc -o client client.cpp


$(phony serverA): 
		./serverA
servermain:
		./servermain
client:
		./client