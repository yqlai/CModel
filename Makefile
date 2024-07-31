CC = gcc
RM = del
SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c, obj/%.o, $(SRCS))
EXES = $(patsubst src/%.c, obj/%.exe, $(SRCS))
CFLAGS = -nostartfiles -Wall -Wno-unused-variable

all: main

obj/%.o: src/%.c src/headers/%.h
	$(CC) -c $< -o $@ $(CFLAGS)

obj/main.o: src/main.c
	$(CC) -c src/main.c -o obj/main.o

main: $(OBJS)
	$(CC) -o $@ $^

clean:
	$(RM) $(OBJS)
	$(RM) $(EXES)
	$(RM) obj/main.o