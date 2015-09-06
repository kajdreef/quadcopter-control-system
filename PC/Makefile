CC=gcc
CFLAGS=-I$(X32INC)
SOURCES= main.c communication.c

main: $(SOURCES)
	$(CC) -o main main.c communication.c

clean:
	rm -f main $(OBJS) $(OBJS:.o=.d)
