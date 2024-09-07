DIR_SRC = ./src/
DIR_INC = ./include/
DIR_OBJ = ./object/

CC = gcc
CFLAGS_E = -o
CFLAGS_O = -g -O -c -o
TAR = jshell

OBJS += $(wildcard $(DIR_OBJ)*.o)

all: cmd exec init
	$(CC) $(DIR_SRC)main.c $(CFLAGS_E) $(TAR) $(OBJS)

cmd:
	$(CC) $(DIR_SRC)$@.c $(CFLAGS_O) $(DIR_OBJ)$@.o

exec:
	$(CC) $(DIR_SRC)$@.c $(CFLAGS_O) $(DIR_OBJ)$@.o

init:
	$(CC) $(DIR_SRC)$@.c $(CFLAGS_O) $(DIR_OBJ)$@.o

