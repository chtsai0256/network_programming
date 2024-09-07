include ../common.mk

TARGET = dup2
SRCS = $(wildcard src/*.c)
OBJS= $(patsubst src/%.c, obj/%.o, ${SRCS})

.PHONY: depend clean
DEPS := $(OBJS:.o=.d)

all: $(TARGET)

-include $(DEPS)

$(TARGET):  $(OBJS)
	@$(CC) $^ -o ../bins/$@ $(LFLAGS)

obj/%.o:src/%.c #$(DEPENDS)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@rm ../bin/$(TARGET) obj/*.o obj/*.d 


