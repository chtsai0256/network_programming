CC = gcc
CFLAGS = -Wno-implicit-function-declaration  -g -MD  
LFLAGS = -lreadline
INC = -Iinclude

SRCS = $(wildcard src/shell/*.c)

OBJS= $(patsubst src/shell/%.c, obj/%.o, ${SRCS})

#DEPENDS = $(wildcard include/*.h)

.PHONY: depend clean all
DEPS := $(OBJS:.o=.d)

all: shell

-include $(DEPS)

shell:  $(OBJS)
	@$(CC) $^ -o bin/$@ $(LFLAGS)

obj/%.o:src/shell/%.c #$(DEPENDS)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@rm bin/shell obj/*.o obj/*.d 


