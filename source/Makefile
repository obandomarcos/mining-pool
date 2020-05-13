CC = gcc
CFLAGS = -g -Wall

default: all

all: pool.c
	$(CC) $(CFLAGS) -o pool mainpool.c pool.c
	$(CC) $(CFLAGS) -o miner mainminer.c miner.c

