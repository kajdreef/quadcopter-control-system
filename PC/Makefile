# General settings can be found in here
include Makefile.inc

CFLAGS=
SOURCES= joystick.c communication.c messages.c main.c
OBJECTS = $(SOURCES:.c=.o)
TARGET= main.exe

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(CFLAGS) $(LDFLAGS)

%.o: %.cp
	$(CC) -c $(CC_FLAGS) $< -o $@

clean:
	rm -rf $(TARGET) $(OBJECTS)
