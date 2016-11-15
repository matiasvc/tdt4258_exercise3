#
# Simple development makefile
#

#
# add more flags here,
# but don't remove the "+="
#
CC = arm-cortexm3-uclinuxeabi-gcc
CFLAGS += -O2 -g -W -Wall -std=c99
LDFLAGS += -lm

OBJECTS = graphics.o matrix.o input.o level.o

all: game

game: ${OBJECTS}
	${CC} ${CFLAGS} game.c ${OBJECTS}  -o game ${LDFLAGS}

clean:
	-rm -f game *.o

install:

.PHONY: all install clean

# end of development makefile
