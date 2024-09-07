CC = gcc
CFLAGS = -Wno-implicit-function-declaration  -MD  -g
LFLAGS = -lcurl
INC = -Iinclude

SRCS = $(wildcard src/*.c)

OBJS = $(patsubst src/%.c, obj/%.o, ${SRCS})

#DEPENDS = $(wildcard include/*.h)

.PHONY: depend clean all
DEPS := $(OBJS:.o=.d)

all: makodo

-include $(DEPS)

makodo:  $(OBJS)
	@$(CC) $^ -o $@ $(LFLAGS)

obj/%.o:src/%.c #$(DEPENDS)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<


clean:
	@rm makodo 
	@rm obj/* 


