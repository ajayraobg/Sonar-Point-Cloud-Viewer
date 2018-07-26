CC = gcc
CFLAGS = -c -Wall -march=armv7-a -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a8 -mtune=cortex-a8 -O3 -ggdb

application: final_code.o
	$(CC) final_code.o -lrt -lm -static -o $@


clean:
	rm -f application *.s *.o *.d
