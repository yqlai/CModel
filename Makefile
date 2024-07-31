CC = gcc
RM = del
SRCS = $(filter-out main.c, $(wildcard *.c))
OBJS = $(SRCS:.c=.o)
EXES = $(SRCS:.c=.exe)
CFLAGS = -nostartfiles -Wall -Wno-unused-variable

all: main

%.o: %.c headers/%.h
	$(CC) -c $< -o $@ $(CFLAGS)

main: $(OBJS) main.o
	$(CC) -c main.c -o main.o
	$(CC) -o $@ $^

clean:
	$(RM) $(OBJS)
	$(RM) $(EXES)
	$(RM) main