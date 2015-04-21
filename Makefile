CC = gcc
CFLAGS = -Wall
DEPS = r3.h sys.h
OBJ = r3.o sys.o

%.o: %.c $(DEPS)
»·$(CC) $(CFLAGS) -c -g -o $@ $<

r3: $(OBJ)
»·gcc $(CFLAGS) -g -o $@ $^
