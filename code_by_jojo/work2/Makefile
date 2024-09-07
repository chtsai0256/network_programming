DIR_SRC = ./src/
DIR_INC = ./include/
DIR_OBJ = ./obj/

CC = gcc
CFLAGS_E = -lpthread -o
CFLAGS_O = -g -O -c -o
TAR = jserver

OBJS += $(wildcard $(DIR_OBJ)*.o)

all: error net services wunix wsocket shell pipeexec cmd chat user
	$(CC) $(DIR_SRC)main.c $(CFLAGS_E) $(TAR) $(OBJS)

error:
	$(CC) $(DIR_SRC)$@.c $(CFLAGS_O) $(DIR_OBJ)$@.o

net:
	$(CC) $(DIR_SRC)$@.c $(CFLAGS_O) $(DIR_OBJ)$@.o

services:
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

chat:
	$(CC) $(DIR_SRC)$@.c $(CFLAGS_O) $(DIR_OBJ)$@.o

user:
	$(CC) $(DIR_SRC)$@.c $(CFLAGS_O) $(DIR_OBJ)$@.o
