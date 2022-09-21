#makefile for Project 1 - Root folder

CC = gcc
CFLAGS = -g -Wall

subsystem:
	$(MAKE) -C Client
	$(MAKE) -C Server

clean:
	cd Client && rm client
	cd Server && rm server