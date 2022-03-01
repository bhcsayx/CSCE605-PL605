IDIR=../include
CC=gcc
CFLAGS=-I$(IDIR)

DEPS = $(IDIR)/*.h

OBJ = scanner.o 

$(warning $(OBJ))
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)