# GCC flags
CC	= gcc
CFLAGS	= -g
LDFLAGS = -g

.c.o:
	$(CC) $(CFLAGS) -c $<

all: w32exec.exe

w32exec.exe: loader.o winapi.o dbgout.o w32exec.o

	$(CC) $(LDFLAGS) loader.o winapi.o dbgout.o w32exec.o -o w32exec.exe

loader.o: loader.c
	$(CC) $(CFLAGS) -c loader.c

winapi.o: winapi.c
	$(CC) $(CFLAGS) -c winapi.c

dbgout.o: dbgout.c
	$(CC) $(CFLAGS) -c dbgout.c

w32exec.o: w32exec.c
	$(CC) $(CFLAGS) -c w32exec.c
