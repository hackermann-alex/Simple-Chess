#include <stdio.h>
#include "game.h"

void mainLoop(SDL_Renderer *renderer, SDL_Texture *sprites);

int
main(int argc, char *argv[])
{
	uint8_t initRet;
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Texture *sprites = NULL;

	switch ((initRet = init(&window, &renderer, &sprites))) {
	case 0:
		break;
	case INIT_PNG:
		printf("Could not initialize IMG system\n");
		quit(initRet, window, renderer, sprites);
	case IMG:
		printf("Could not load image " SHEET "\n");
		quit(initRet, window, renderer, sprites);
	default:
		printf("SDL could not initialize. Error: %s\n",
			SDL_GetError());
		quit(initRet, window, renderer, sprites);
	}

	mainLoop(renderer, sprites);
	quit(0, window, renderer, sprites);
}


void
mainLoop(SDL_Renderer *renderer, SDL_Texture *sprites)
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
	       W_MOVE 
	};

	renderGame(renderer, sprites, game.board, bitBoard);
	SDL_RenderPresent(renderer);
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
				renderGame(renderer, sprites, game.board, bitBoard);
			} else {
				bitBoard = getMoves(&game, i);
				renderGame(renderer, sprites, game.board, bitBoard);
				prevI = i;
			}

			SDL_RenderPresent(renderer);
		}
	}
	goto loop;
}
