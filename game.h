#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <stdint.h>

#define SCREEN_W 960
#define SCREEN_H 720
#define NAME "Poor Man's Chess"
#define SHEET "assets.png"
#define BG 0x47, 0x80, 0x61, 0xFF
#define TILE_W 64
#define PIECE_W 16
#define PIECE_H 32
#define CHAR 8

#define TILE_H (TILE_W >> 1) + 1
#define ORIGIN_X (SCREEN_W >> 1) - (TILE_W << 2)
#define ORIGIN_Y (SCREEN_H - TILE_H) >> 1

enum codes { SUCCESS, INIT_SDL, INIT_PNG, WINDOW, RENDERER, IMG };
enum states { W_MOVE, B_MOVE, W_WON, B_WON, DRAW };
enum pieces { EMPTY, W_PAWN, B_PAWN, W_KNIGHT, B_KNIGHT, W_BISHOP, B_BISHOP,
	W_ROOK, B_ROOK, W_QUEEN, B_QUEEN, W_KING, B_KING };

typedef struct {
	uint8_t board[64];
	uint8_t state;
} game_t;

void renderGame(SDL_Renderer *renderer, SDL_Texture *sprites,
		const uint8_t *game, uint64_t bitBoard);
uint8_t init(SDL_Window **window, SDL_Renderer **renderer,
		SDL_Texture **sprites);
void quit(uint8_t code, SDL_Window *window, SDL_Renderer *renderer,
		SDL_Texture *sprites);

int8_t getTile(int ix, int iy);
uint64_t getMoves(const game_t *game, uint8_t i);
uint8_t validMove(uint64_t bitBoard, uint8_t i);
void move(game_t *game, uint8_t prevI, uint8_t i);

#endif
