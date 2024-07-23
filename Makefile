CC = gcc
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: main

%.o: %.c headers/%.h
	$(CC) $< -o $@ -nostartfiles

main: $(OBJS)
	$(CC) -o $@ $^ 