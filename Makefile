# -*- Makefile -*-

#target: dependencies
#	action

normal_web_server: normal_web_server.o
	gcc normal_web_server.o -o normal_web_server


normal_web_server.o: normal_web_server.c
	gcc -c normal_web_server.c
