SRC = main.c render.c logic.c
OBJ = main.o render.o logic.o
DEST = game
INCS = -lSDL2 -lSDL2_image
CFLAGS = -std=c99 -Wall -O3 -pedantic
CC = cc

all: ${OBJ}
	${CC} ${INCS} -o ${DEST} ${OBJ}

main.o: main.c game.h
	${CC} ${CFLAGS} -c -o main.o main.c

render.o: render.c game.h
	${CC} ${CFLAGS} -c -o render.o render.c

logic.o: logic.c game.h
	${CC} ${CFLAGS} -c -o logic.o logic.c

clean:
	rm *.o
