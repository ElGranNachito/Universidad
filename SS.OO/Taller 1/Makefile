CC=gcc

ARCH := $(shell uname -m)

BINS=ejercicio1 hai

OBJS=$(BINS:=.o)

all: $(BINS)

# Dependencias
ejercicio1.o: ejercicio1.c
hai.o: hai.c

clean:
	rm -rf $(OBJS) $(BINS)
