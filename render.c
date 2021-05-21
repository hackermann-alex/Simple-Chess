#include "game.h"

void
renderTile(SDL_Renderer *renderer, SDL_Texture *sprites, uint8_t i)
{
	uint8_t j = i & 7;
	const static SDL_Rect clips[2] = { { TILE_W, PIECE_H, TILE_W, TILE_H },
	      { 0, PIECE_H, TILE_W, TILE_H } };
	SDL_Rect renderQuad = { ORIGIN_X + (j + (i >> 3)) * (TILE_W >> 1),
		(ORIGIN_Y) + (j - (i >> 3)) * ((TILE_W >> 2) + 1),
		TILE_W, TILE_H };

	SDL_RenderCopy(renderer, sprites, clips + (((i >> 3) + j) & 1), &renderQuad);
}

void
renderMoves(SDL_Renderer *renderer, SDL_Texture *sprites, uint64_t bitBoard)
{
	const static SDL_Rect clip = { 12 * PIECE_W, 16, 32, 16 };
	SDL_Rect renderQuad = { 0, 0, 32, 16 };
	uint8_t i, j;

	for (i = 0; i < 8; ++i) {
		renderQuad.x = ORIGIN_X + 16 + i * (TILE_W >> 1);
		renderQuad.y = (SCREEN_H >> 1) - 8 - i * ((TILE_W >> 2) + 1);
		for (j = 0; j < 8; ++j) {
			if ((bitBoard >> ((i << 3) + j)) & 1)
				SDL_RenderCopy(renderer, sprites, &clip, &renderQuad);
			renderQuad.x += TILE_W >> 1;
			renderQuad.y += (TILE_W >> 2) + 1;
		}
	}
}

void
renderScene(SDL_Renderer *renderer, SDL_Texture *sprites)
{
	const static SDL_Rect clips[4] = { { TILE_W << 1, PIECE_H, TILE_W >> 1, TILE_H },
		{ TILE_W * 3, PIECE_H, TILE_W >> 1, TILE_H },
		{ TILE_W * 5 >> 1, PIECE_H, TILE_W >> 1, TILE_H },
		{ TILE_W * 7 >> 1, PIECE_H, TILE_W, TILE_H } };
	SDL_Rect text = { 12 * PIECE_W, 0, CHAR, CHAR };
	SDL_Rect renderQuad = { ORIGIN_X, SCREEN_H >> 1, TILE_W >> 1, TILE_H };
	SDL_Rect renderText = { renderQuad.x, renderQuad.y + CHAR, CHAR, CHAR };
	uint8_t i, j;

	for (i = 0; i < 8; ++i) {
		SDL_RenderCopy(renderer, sprites, clips + (i & 1), &renderQuad);
		SDL_RenderCopy(renderer, sprites, &text, &renderText);
		text.x += CHAR;
		renderQuad.x += TILE_W >> 1;
		renderText.x += TILE_W >> 1;
		renderQuad.y += (TILE_W >> 2) + 1;
		renderText.y += (TILE_W >> 2) + 1;
	}
	renderQuad.y -= (TILE_W >> 2) + 1;
	renderText.x += (TILE_W >> 1) - CHAR;
	renderText.y = renderQuad.y + CHAR;
	text.x = 12 * PIECE_W;
	text.y = CHAR;
	for (j = 0; j < 8; ++j) {
		SDL_RenderCopy(renderer, sprites, clips + !(j & 1) + 2, &renderQuad);
		SDL_RenderCopy(renderer, sprites, &text, &renderText);
		text.x += CHAR;
		renderQuad.x += TILE_W >> 1;
		renderText.x += TILE_W >> 1;
		renderQuad.y -= (TILE_W >> 2) + 1;
		renderText.y -= (TILE_W >> 2) + 1;
	}
	for (i = 0; i < 64; ++i)
		renderTile(renderer, sprites, i);
}

void
renderBoard(SDL_Renderer *renderer, SDL_Texture *sprites)
{
	uint8_t i;
	for (i = 0; i < 64; ++i)
		renderTile(renderer, sprites, i);
}

void
renderPiece(SDL_Renderer *renderer, SDL_Texture *sprites,
		uint8_t i, const uint8_t piece)
{
	uint8_t j = i & 7;
	SDL_Rect clip = { 0, 0, PIECE_W, PIECE_H };
	SDL_Rect renderQuad = { (ORIGIN_X) + ((i >> 3) + j) * (TILE_W >> 1) + 24,
		(SCREEN_H >> 1) - ((i >> 3) - j) * ((TILE_W >> 2) + 1) - 28,
		PIECE_W, PIECE_H };

	if (!piece)
		return;
	clip.x = PIECE_W * (piece - 1);
	SDL_RenderCopy(renderer, sprites, &clip, &renderQuad);
}

void
renderGame(SDL_Renderer *renderer, SDL_Texture *sprites, const uint8_t *board, uint64_t bitBoard)
{
	const static SDL_Rect rect = { ORIGIN_X,
		(SCREEN_H >> 1) - 9 * (TILE_W >> 2) - 9,
		TILE_W << 3, 9 * (TILE_W >> 2) + 9 };
	int8_t i;

	SDL_RenderFillRect(renderer, &rect);
	renderBoard(renderer, sprites);
	renderMoves(renderer, sprites, bitBoard);
	for (i = 63; i >= 0; --i) {
		if (board[i])
			renderPiece(renderer, sprites, i, board[i]);
	}
}

uint8_t
init(SDL_Window **window, SDL_Renderer **renderer, SDL_Texture **sprites)
{
	SDL_Surface *loadSurface;

	/* Init SDL */
	if (SDL_Init(SDL_INIT_VIDEO))
		return INIT_SDL;
	/* Create window */
	*window = SDL_CreateWindow(NAME,
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			SCREEN_W, SCREEN_H,
			SDL_WINDOW_SHOWN);
	if (!*window)
		return WINDOW;
	/* Create renderer */
	*renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
	if (!*renderer)
		return RENDERER;
	/* Load sprite sheet */
	if (!(loadSurface = SDL_LoadBMP(SHEET)))
		return IMG;
	SDL_SetColorKey(loadSurface, SDL_TRUE, SDL_MapRGB(loadSurface->format, KEY));
	*sprites = SDL_CreateTextureFromSurface(*renderer, loadSurface);
	SDL_FreeSurface(loadSurface);
	if (!*sprites)
		return TEX;
	/* Render scene */
	SDL_SetRenderDrawColor(*renderer, BG);
	SDL_RenderClear(*renderer);
	renderScene(*renderer, *sprites);
	return SUCCESS;
}

void
quit(uint8_t code, SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *sprites)
{
	switch (code) {
	case SUCCESS:
		SDL_DestroyTexture(sprites);
	case TEX:
	case IMG:
		SDL_DestroyRenderer(renderer);
	case RENDERER:
		SDL_DestroyWindow(window);
	case WINDOW:
		SDL_Quit();
	default:
		exit(code);
	}
}
