CC = gcc

INCLUDES = 
CFLAGS = -g -Wall $(INCLUDES)

LDLIBS = -ltiff -lm 

.PHONY: default
default: imEncrypt imDecrypt

imEncrypt: imEncrypt.o

imDecrypt: imDecrypt.o

clean:
	rm imEncrypt imDecrypt *.o


