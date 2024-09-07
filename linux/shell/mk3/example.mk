CC = gcc
CFLAGS = -Wall -g  
LIB = -lreadline
INC = -Iinclude

SRCS = $(wildcard src/*.c)

OBJS= $(patsubst src/%.c, obj/%.o, ${SRCS})

shell:  $(OBJS)
	@echo $@ $^

obj/%.o:src/%.c
	@echo $^ $@	

.PHONY : clean
clean:
	rm shell obj/*.o 


