CC = gcc
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: main

%.o: %.c headers/%.h
	$(CC) $< -o $@

main: $(OBJS)
	$(CC) -o $@ $^ -mconsole