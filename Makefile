CC = gcc
RM = del
SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, obj/%.o, $(SRCS))
EXES = $(patsubst src/%.c, obj/%.exe, $(SRCS))
CFLAGS = -nostartfiles -Wno-unused-variable

all: main

obj/%.o: src/%.c src/headers/%.h
	$(CC) -c $< -o $@ $(CFLAGS)

obj/main.o: src/main.c
	$(CC) -c src/main.c -o obj/main.o

main: $(OBJS)
	$(CC) -o $@ $^

run:
	./main

clean:
	$(RM) $(patsubst obj/%.o, obj\\%.o, $(OBJS))
	$(RM) main.exe