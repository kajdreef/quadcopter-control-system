CC=gcc
CFLAGS=-I$(X32INC)
SOURCES= main.c communication.c
EXECUTABLE= main.exe

main: $(SOURCES)
	$(CC) -o $(EXECUTABLE) main.c communication.c

clean:
	rm -f $(EXECUTABLE) $(OBJS) $(OBJS:.o=.d)
