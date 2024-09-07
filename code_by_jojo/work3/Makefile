DIR_SRC = ./src/
DIR_INC = ./include/
DIR_OBJ = ./obj/

CC = gcc
REDIS = -lhiredis
CFLAGS_E = -lpthread -o
CFLAGS_O = -g -O -c -o
TAR = jserver

OBJS += $(wildcard $(DIR_OBJ)*.o)

.PHONY: dbset

all: error net clirw wunix wsocket shell pipeexec cmd serv user db
	$(CC) $(DIR_SRC)main.c $(CFLAGS_E) $(TAR) $(OBJS) $(REDIS)

error:
	$(CC) $(DIR_SRC)$@.c $(CFLAGS_O) $(DIR_OBJ)$@.o

net:
	$(CC) $(DIR_SRC)$@.c $(CFLAGS_O) $(DIR_OBJ)$@.o

clirw:
	$(CC) $(DIR_SRC)$@.c $(CFLAGS_O) $(DIR_OBJ)$@.o

wunix:
	$(CC) $(DIR_SRC)$@.c $(CFLAGS_O) $(DIR_OBJ)$@.o

wsocket:
	$(CC) $(DIR_SRC)$@.c $(CFLAGS_O) $(DIR_OBJ)$@.o

shell:
	$(CC) $(DIR_SRC)$@.c $(CFLAGS_O) $(DIR_OBJ)$@.o

pipeexec:
	$(CC) $(DIR_SRC)$@.c $(CFLAGS_O) $(DIR_OBJ)$@.o

cmd:
	$(CC) $(DIR_SRC)$@.c $(CFLAGS_O) $(DIR_OBJ)$@.o

serv:
	$(CC) $(DIR_SRC)$@.c $(CFLAGS_O) $(DIR_OBJ)$@.o

user:
	$(CC) $(DIR_SRC)$@.c $(CFLAGS_O) $(DIR_OBJ)$@.o

db:
	$(CC) $(DIR_SRC)$@.c $(CFLAGS_O) $(DIR_OBJ)$@.o

dbset:
	$(CC) $(DIR_SRC)dbset.c $(CFLAGS_E) dbset $(REDIS)