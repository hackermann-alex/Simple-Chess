#include <stdio.h>
#include "game.h"

void mainLoop();

int
main(int argc, char *argv[])
{
	uint8_t initRet;

	switch ((initRet = init())) {
	case SUCCESS:
		break;
	case TEX:
		printf("Could not load surface as texture\n");
		quit(initRet);
	case IMG:
		printf("Could not load image " SHEET "\n");
		quit(initRet);
	default:
		printf("SDL could not initialize. Error: %s\n",
			SDL_GetError());
		quit(initRet);
	}

	mainLoop();
	quit(0);
}


void
mainLoop()
{
	uint8_t i, prevI;
	uint64_t bitBoard = 0;
	SDL_Event e;
	game_t game = { { W_ROOK, W_KNIGHT, W_BISHOP, W_QUEEN, W_KING, W_BISHOP, W_KNIGHT, W_ROOK,
			  W_PAWN, W_PAWN,   W_PAWN,   W_PAWN,  W_PAWN, W_PAWN,   W_PAWN,   W_PAWN,
			  0, 0, 0, 0, 0, 0, 0, 0,
			  0, 0, 0, 0, 0, 0, 0, 0,
			  0, 0, 0, 0, 0, 0, 0, 0,
			  0, 0, 0, 0, 0, 0, 0, 0,
			  B_PAWN, B_PAWN,   B_PAWN,   B_PAWN,  B_PAWN, B_PAWN,   B_PAWN,   B_PAWN,
			  B_ROOK, B_KNIGHT, B_BISHOP, B_QUEEN, B_KING, B_BISHOP, B_KNIGHT, B_ROOK },
	       004, 074,
	       W_MOVE 
	};

	renderGame(game.board, bitBoard);
loop:
	/* Poll all events on the queue */
	while (SDL_WaitEvent(&e)) {
		switch (e.type) {
		case SDL_QUIT:
			return;
		case SDL_MOUSEBUTTONDOWN:
			i = getTile(e.button.x, e.button.y);
			if (validMove(bitBoard, i)) {
				move(&game, prevI, i);
				bitBoard = 0;
				renderGame(game.board, bitBoard);
			} else {
				bitBoard = getMoves(&game, i);
				renderGame(game.board, bitBoard);
				prevI = i;
			}
		}
	}
	goto loop;
}
