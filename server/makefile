CC=/usr/local/armgcc/10.2/bin/aarch64-none-linux-gnu-gcc

default all: aesdsocket.c
ifeq ($(CROSS_COMPILE), aarch64-none-linux-gnu-)
	#CC = aarch64-none-linux-gnu-gcc
	#OBJCOPY = aarch64-none-linux-gnu-objcopy
	$(CC) aesdsocket.c -o aesdsocket
	@echo "1"
else
	gcc  aesdsocket.c -o aesdsocket
	@echo "2"
endif

#writer: writer.c
#	gcc writer.c -o writer

clean:
	rm -f *.o aesdsocket
