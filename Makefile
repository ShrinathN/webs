CC = gcc
CFLAGS = -I. -Wall
webs: webs.c helpers.h
	$(CC) $(CFLAGS) webs.c -o webs
