IDIR=./include
SDIR=src
CC=gcc
CFLAGS=-I$(IDIR)

DEPS = $(IDIR)/*.h

OBJ = main.o ./src/*.o

pl605: $(OBJ)
	$(CC) $(OBJ) -o pl605 $(CFLAGS)

main.o: main.c $(DEPS)
	$(CC) -c main.c

src:
	cd src && make

.PHONY: clean

clean:
	rm -f *.o *~ core ./src/*.o $(IDIR)/*~ pl605
