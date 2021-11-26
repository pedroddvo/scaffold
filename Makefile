CFLAGS=-Werror
CC=clang
SRCS=$(wildcard src/*.c)
LIBS=$(wildcard src/*.h)
OBJS=$(SRCS:.c=.o)

%.o: %.c $(LIBS)
	$(CC) $(CFLAGS) -c -o $@ $<

scaffold: $(OBJS)
	$(CC) $(CFLAGS) -o $@.exe $(OBJS)
