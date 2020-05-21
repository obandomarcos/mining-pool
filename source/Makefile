CC = gcc
CFLAGS = -g -Wall 

default: all

all: pool.c
	$(CC) $(CFLAGS) -o pool mainpool.c pool.c workers.c -pthread -lrt -lm -lssl -lcrypto
	$(CC) $(CFLAGS) -o miner mainminer.c miner.c workers.c -pthread -lrt -lm -lssl -lcrypto

