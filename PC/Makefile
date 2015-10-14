# General settings can be found in here
CC= gcc
CFLAGS= -Wall
SOURCES= $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
TARGET= main.exe

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(CFLAGS) $(LDFLAGS)

%.o: %.cp
	$(CC) -c $(CC_FLAGS) $< -o $@

clean:
	rm -rf $(TARGET) $(OBJECTS)
	rm -rf log.txt
