#ifndef _communication_h
#define _communication_h

char output_buffer[15];

struct JS JS_mes;

struct JS {
	int lift;
	int roll;
	int pitch;
	int yaw;
	int mode;
};

void printBits(size_t const size, void const * const ptr);
void encode(int value, char* buffer,int index);
int decode (char* buffer, int index);

#endif
