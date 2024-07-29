CC = gcc
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
CFLAGS = -nostartfiles -Wall

all: main

%.o: %.c headers/%.h
	$(CC) -c $< -o $@ $(CFLAGS)

main: $(OBJS)
	$(CC) -o $@ $^