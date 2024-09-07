CC = gcc
CFLAGS = -Wno-implicit-function-declaration  -g  
LFLAGS = -lreadline
INC = -Iinclude


SRCS = $(wildcard src/*.c)

OBJS= $(patsubst src/%.c, obj/%.o, ${SRCS})

shell:  $(OBJS)
	@$(CC) $^ -o bin/$@ $(LFLAGS)

obj/%.o:src/%.c
	$(CC) $(CFLAGS) $(INC) -c $^ -o $@	

.PHONY : clean
clean:
	@rm bin/* obj/*.o 


